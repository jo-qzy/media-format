//
// Created by BoringWednesday on 2021/7/7.
//

#include <assert.h>
#include <string.h>

#include <base/data.h>
#include <flv/amf0.h>
#include <flv/amf3.h>

/*
 * AMF Format:
 *      AMF_TYPE: 1 byte
 *      DATA_LENGTH: 0/2/4 bytes
 *      DATA: N bytes
 */

static inline uint8_t *amf0_write_string16(uint8_t *ptr, const uint8_t *end, const char *str, uint32_t length)
{
    if (ptr + 2 + length > end || !str)
        return NULL;

    ptr = write_uint16_be(ptr, end, (uint16_t) length);
    memcpy(ptr, str, length);

    return ptr + length;
}

static inline uint8_t *amf0_write_string32(uint8_t *ptr, const uint8_t *end, const char *str, uint32_t length)
{
    if (ptr + 4 + length > end || !str)
        return NULL;

    ptr = write_uint32_be(ptr, end, length);
    memcpy(ptr, str, length);

    return ptr + length;
}

static inline uint8_t *amf0_write_item(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!ptr || !item)
        return NULL;

    switch (item->type) {
        case AMF0_NUMBER:
            AMF_CHECK_POINTER(item->value);
            return amf0_write_number(ptr, end, *(double *) item->value);

        case AMF0_BOOLEAN:
            AMF_CHECK_POINTER(item->value);
            return amf0_write_boolean(ptr, end, *(uint8_t *) item->value);

        case AMF0_STRING:
            AMF_CHECK_POINTER(item->value);
            return amf0_write_string(ptr, end, item->value, item->size);

        case AMF0_OBJECT:
            return amf0_write_object(ptr, end, item);

        case AMF0_MOVIE_CLIP:
            return NULL;

        case AMF0_NULL:
            return amf0_write_null(ptr, end);

        case AMF0_UNDEFINED:
            return amf0_write_undefined(ptr, end);

        case AMF0_REFERENCE:
            AMF_CHECK_POINTER(item->value);
            return amf0_write_reference(ptr, end, *(uint16_t *) item->value);

        case AMF0_ECMA_ARRAY:
            return amf0_write_ecma_array(ptr, end, item);

        case AMF0_OBJECT_END:
            return amf0_write_object_end(ptr, end);

        case AMF0_STRICT_ARRAY:
            return amf0_write_strict_array(ptr, end, item);

        case AMF0_DATE:
            AMF_CHECK_POINTER(item->value);

            // Use item->value = milliseconds, item->size = timezone
            return amf0_write_date(ptr, end, *(double *) item->value, *(int16_t *) ((uint8_t *) item->value + 8));

        case AMF0_LONG_STRING:
            AMF_CHECK_POINTER(item->value);

            return amf0_write_long_string(ptr, end, item->value, item->size);

        case AMF0_UNSUPPORTED:
        case AMF0_RECORDSET:
            return NULL;

        case AMF0_XML_DOCUMENT:
            return amf0_write_xml_document(ptr, end, item);

        case AMF0_TYPED_OBJECT:
            return amf0_write_typed_object(ptr, end, item);

        case AMF0_AVMPLUS_OBJECT:
            return amf0_write_avmplus_object(ptr, end, item);

        default:
            return NULL;
    }
}

uint8_t *amf0_write(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *items, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count && ptr && ptr < end; i++) {
        ptr = amf0_write_item(ptr, end, items + i);
    }

    if (count != i)
        return NULL;

    return ptr;
}

uint8_t *amf0_write_number(uint8_t *ptr, const uint8_t *end, double value)
{
    if (!ptr || ptr + 9 > end)
        return NULL;

    *ptr++ = AMF0_NUMBER;

    return write_double_be(ptr, end, value);
}

uint8_t *amf0_write_boolean(uint8_t *ptr, const uint8_t *end, uint8_t value)
{
    if (!ptr || ptr + 2 > end)
        return NULL;

    *ptr++ = AMF0_BOOLEAN;
    *ptr++ = value == 0 ? 0 : 1;

    return ptr;
}

uint8_t *amf0_write_string(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length)
{
    if (!ptr || length > UINT16_MAX || ptr + 1 + 2 + length > end)
        return NULL;

    *ptr++ = AMF0_STRING;

    return amf0_write_string16(ptr, end, string, length);
}

uint8_t *amf0_write_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t i;

    if (!ptr || ptr + 1 > end || !item || item->type != AMF0_OBJECT)
        return NULL;

    *ptr++ = AMF0_OBJECT;

    for (i = 0; i < item->size; i++) {
        struct amf_object_item_t *current_item = ((struct amf_object_item_t *) item->value) + i;

        ptr = amf0_write_string16(ptr, end, current_item->name, (uint32_t) strlen(current_item->name));
        AMF_CHECK_POINTER(ptr);

        ptr = amf0_write_item(ptr, end, current_item);
        AMF_CHECK_POINTER(ptr);
    }

    return amf0_write_object_end(ptr, end);
}

uint8_t *amf0_write_null(uint8_t *ptr, const uint8_t *end)
{
    if (!ptr || ptr + 1 > end)
        return NULL;

    *ptr++ = AMF0_NULL;

    return ptr;
}

uint8_t *amf0_write_undefined(uint8_t *ptr, const uint8_t *end)
{
    if (!ptr || ptr + 1 > end)
        return NULL;

    *ptr++ = AMF0_UNDEFINED;

    return ptr;
}

uint8_t *amf0_write_reference(uint8_t *ptr, const uint8_t *end, uint16_t reference)
{
    if (!ptr || ptr + 3 > end)
        return NULL;

    *ptr++ = AMF0_REFERENCE;

    return write_uint16_be(ptr, end, reference);
}

uint8_t *amf0_write_ecma_array(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t i;

    if (!ptr || ptr + 5 > end || !item || item->type != AMF0_ECMA_ARRAY)
        return NULL;

    *ptr++ = AMF0_ECMA_ARRAY;

    // associative-count: 4 bytes
    ptr = write_uint32_be(ptr, end, item->size);

    for (i = 0; i < item->size; i++) {
        struct amf_object_item_t *current_item = ((struct amf_object_item_t *) item->value) + i;

        ptr = amf0_write_string16(ptr, end, current_item->name, (uint32_t) strlen(current_item->name));
        AMF_CHECK_POINTER(ptr);

        ptr = amf0_write_item(ptr, end, current_item);
        AMF_CHECK_POINTER(ptr);
    }

    return amf0_write_object_end(ptr, end);
}

uint8_t *amf0_write_object_end(uint8_t *ptr, const uint8_t *end)
{
    if (!ptr || ptr + 3 > end)
        return NULL;

    // End of Object: 0x00 0x00 0x09
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = AMF0_OBJECT_END;

    return ptr;
}

uint8_t *amf0_write_strict_array(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t i;

    if (!ptr || ptr + 5 > end || !item || item->type != AMF0_STRICT_ARRAY)
        return NULL;

    *ptr++ = AMF0_STRICT_ARRAY;

    // associative-count: 4 bytes
    ptr = write_uint32_be(ptr, end, item->size);

    for (i = 0; i < item->size; i++) {
        struct amf_object_item_t *current_item = ((struct amf_object_item_t *) item->value) + i;

        ptr = amf0_write_item(ptr, end, current_item);
        AMF_CHECK_POINTER(ptr);
    }

    return ptr;
}

uint8_t *amf0_write_date(uint8_t *ptr, const uint8_t *end, double milliseconds, uint16_t timezone)
{
    if (!ptr || ptr + 11 > end)
        return NULL;

    *ptr++ = AMF0_DATE;

    ptr = write_double_be(ptr, end, milliseconds);
    ptr = write_uint16_be(ptr, end, timezone);

    return ptr;
}

uint8_t *amf0_write_long_string(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length)
{
    if (!ptr || length > UINT32_MAX || ptr + 1 + 4 + length > end)
        return NULL;

    *ptr++ = AMF0_LONG_STRING;

    return amf0_write_string32(ptr, end, string, length);
}

uint8_t *amf0_write_xml_document(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!ptr || ptr + 1 > end)
        return NULL;

    *ptr++ = AMF0_XML_DOCUMENT;

    return amf0_write_item(ptr, end, (struct amf_object_item_t *) item->value);
}

uint8_t *amf0_write_typed_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t i;

    if (!ptr || ptr + 1 > end)
        return NULL;

    *ptr++ = AMF0_TYPED_OBJECT;

    ptr = amf0_write_string16(ptr, end, item->name, (uint32_t) strlen(item->name));
    AMF_CHECK_POINTER(ptr);

    for (i = 0; i < item->size; i++) {
        struct amf_object_item_t *current_item = ((struct amf_object_item_t *) item->value) + i;

        ptr = amf0_write_string16(ptr, end, current_item->name, (uint32_t) strlen(current_item->name));
        AMF_CHECK_POINTER(ptr);

        ptr = amf0_write_item(ptr, end, current_item);
        AMF_CHECK_POINTER(ptr);
    }

    return amf0_write_object_end(ptr, end);
}

uint8_t *amf0_write_avmplus_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!ptr || ptr + 1 > end)
        return NULL;

    *ptr++ = AMF0_AVMPLUS_OBJECT;

    return amf3_write(ptr, end, item->value);
}

static inline const uint8_t *amf0_read_item(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!item)
        return NULL;

    switch (item->type) {
        case AMF0_NUMBER:
            return amf0_read_number(data, end, item->value);

        case AMF0_BOOLEAN:
            return amf0_read_boolean(data, end, item->value);

        case AMF0_STRING:
            return amf0_read_string(data, end, item->value, &item->size);

        case AMF0_OBJECT:
            return amf0_read_object(data, end, item);

        case AMF0_NULL:
            return amf0_read_null(data, end);

        case AMF0_UNDEFINED:
            return amf0_read_undefined(data, end);

        case AMF0_REFERENCE:
            return amf0_read_reference(data, end, item->value);

        case AMF0_ECMA_ARRAY:
            return amf0_read_ecma_array(data, end, item);

        case AMF0_STRICT_ARRAY:
            return amf0_read_strict_array(data, end, item);

        case AMF0_DATE:
            if (item->size < 10) {
                return NULL;
            }

            return amf0_read_date(data, end, item->value, (uint16_t *) ((uint8_t *) item->value + 8));

        case AMF0_LONG_STRING:
            return amf0_read_long_string(data, end, item->value, &item->size);

        case AMF0_XML_DOCUMENT:
            return amf0_read_xml_document(data, end, item);

        case AMF0_TYPED_OBJECT:
            return amf0_read_typed_object(data, end, item);

        case AMF0_AVMPLUS_OBJECT:
            return amf0_read_avmplus_object(data, end, item);

        default:
            return NULL;
    }
}

const uint8_t *amf0_read(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *items, uint32_t count)
{
    uint32_t i;
    uint8_t  type;

    for (i = 0; i < count; i++) {
        if (!data || data >= end) {
            break;
        }

        type = *data;
        if (type != items[i].type) {
            return NULL;
        }

        data = amf0_read_item(data, end, items + i);
    }

    return data;
}

static inline const uint8_t *amf0_read_int16(const uint8_t *data, const uint8_t *end, uint16_t *value)
{
    if (data + 2 > end || !value)
        return NULL;

    *value = ((uint16_t) data[0] << 8) | data[1];

    return data + 2;
}

static inline const uint8_t *amf0_read_int32(const uint8_t *data, const uint8_t *end, uint32_t *value)
{
    if (data + 4 > end || !value)
        return NULL;

    *value = ((uint32_t) data[0] << 24) | ((uint32_t) data[1] << 16) | ((uint32_t) data[2] << 8) | data[3];

    return data + 4;
}

static inline const uint8_t *amf0_read_double(const uint8_t *data, const uint8_t *end, double *value)
{
    uint8_t *ptr = (uint8_t *) value;
    if (!data || data + 8 > end || !value)
        return NULL;

    *ptr++ = data[7];
    *ptr++ = data[6];
    *ptr++ = data[5];
    *ptr++ = data[4];
    *ptr++ = data[3];
    *ptr++ = data[2];
    *ptr++ = data[1];
    *ptr++ = data[0];

    return data + 8;
}

const uint8_t *amf0_read_number(const uint8_t *data, const uint8_t *end, double *value)
{
    if (!data || data + 9 > end || AMF0_NUMBER != *data++)
        return NULL;

    return amf0_read_double(data, end, value);
}

const uint8_t *amf0_read_boolean(const uint8_t *data, const uint8_t *end, uint8_t *value)
{
    if (!data || data + 2 > end || AMF0_BOOLEAN != *data++)
        return NULL;

    *value = *data++;

    return data;
}

const uint8_t *amf0_read_string(const uint8_t *data, const uint8_t *end, char *string, uint32_t *length)
{
    uint16_t str_length;

    if (!data || data + 3 > end || AMF0_STRING != *data++)
        return NULL;

    data = amf0_read_int16(data, end, &str_length);
    if (data + str_length > end || str_length > *length)
        return NULL;

    *length = str_length;
    memcpy(string, data, str_length);

    return data + str_length;
}

static const uint8_t *amf0_read_object_internal(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item,
                                                uint32_t count)
{
    uint16_t length;
    uint32_t i;

    while (data && data + 2 <= end) {
        data = amf0_read_int16(data, end, &length);
        if (0 == length) {
            // Read end of object
            break;
        }

        if (data + length + 1 > end) {
            return NULL;
        }

        for (i = 0; i < count; i++) {
            if (strlen(item[i].name) == length && 0 == memcmp(item[i].name, data, length) &&
                item[i].type == data[length]) {
                data += length;
                data = amf0_read_item(data, end, item + i);

                break;
            }
        }
    }

    if (!data || data >= end || AMF0_OBJECT_END != *data++)
        return NULL;

    return data;
}

const uint8_t *amf0_read_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!data || data + 1 > end || *data++ != AMF0_OBJECT)
        return NULL;

    return amf0_read_object_internal(data, end, item->value, item->size);
}

const uint8_t *amf0_read_null(const uint8_t *data, const uint8_t *end)
{
    if (!data || data + 1 > end || AMF0_NULL != *data++)
        return NULL;

    return data;
}

const uint8_t *amf0_read_undefined(const uint8_t *data, const uint8_t *end)
{
    if (!data || data + 1 > end || AMF0_UNDEFINED != *data++)
        return NULL;

    return data;
}

const uint8_t *amf0_read_reference(const uint8_t *data, const uint8_t *end, uint16_t *reference)
{
    if (!data || data + 3 > end || AMF0_REFERENCE != *data++)
        return NULL;

    return amf0_read_int16(data, end, reference);
}

const uint8_t *amf0_read_ecma_array(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t item_length;

    if (!data || data + 5 > end || AMF0_ECMA_ARRAY != *data++)
        return NULL;

    // U32 associative-count
    data = amf0_read_int32(data, end, &item_length);
    if (item->size <= item_length)
        return NULL;

    return amf0_read_object_internal(data, end, item->value, item->size);
}

const uint8_t *amf0_read_object_end(const uint8_t *data, const uint8_t *end)
{
    if (!data || data + 3 > end)
        return NULL;

    if (0 != data[0] || 0 != data[1] || AMF0_OBJECT_END != data[2])
        return NULL;

    return data + 3;
}

const uint8_t *amf0_read_strict_array(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    uint32_t i;
    uint32_t item_length;

    if (!data || data + 5 > end || AMF0_STRICT_ARRAY != *data++)
        return NULL;

    data = amf0_read_int32(data, end, &item_length);

    for (i = 0; i < item_length && data; i++)
        data = amf0_read_item(data, end, ((struct amf_object_item_t *) item->value) + i);

    if (item_length != i)
        return NULL;

    return data;
}

const uint8_t *amf0_read_date(const uint8_t *data, const uint8_t *end, double *milliseconds, uint16_t *timezone)
{
    if (!data || data + 11 > end || AMF0_DATE != *data++)
        return NULL;

    data = amf0_read_double(data, end, milliseconds);
    data = amf0_read_int16(data, end, timezone);

    return data;
}

const uint8_t *amf0_read_long_string(const uint8_t *data, const uint8_t *end, char *string, uint32_t *length)
{
    uint32_t str_length;

    if (!data || data + 5 > end || AMF0_STRING != *data++)
        return NULL;

    data = amf0_read_int32(data, end, &str_length);
    if (data + str_length > end || str_length > *length)
        return NULL;

    *length = str_length;
    memcpy(string, data, str_length);

    return data + str_length;
}

const uint8_t *amf0_read_xml_document(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!data || data + 1 > end || AMF0_XML_DOCUMENT != *data++)
        return NULL;

    return amf0_read_item(data, end, item->value);
}

const uint8_t *amf0_read_typed_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    uint16_t str_length;

    if (!data || data + 3 > end || AMF0_TYPED_OBJECT != *data++)
        return NULL;

    data = amf0_read_int16(data, end, &str_length);
    data += str_length;
    AMF_CHECK_POINTER(data);

    return amf0_read_object_internal(data, end, item->value, item->size);
}

const uint8_t *amf0_read_avmplus_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item)
{
    if (!data || data + 1 > end || AMF0_AVMPLUS_OBJECT != *data++)
        return NULL;

    return amf3_read(data, end, item->value);
}
