//
// Created by BoringWednesday on 2021/7/7.
//

#ifndef LIBAMF_AMF0_H
#define LIBAMF_AMF0_H

#include <flv/amf_item.h>



enum amf0_type_t
{
    AMF0_NUMBER         = 0x00,
    AMF0_BOOLEAN        = 0x01,
    AMF0_STRING         = 0x02,
    AMF0_OBJECT         = 0x03,
    AMF0_MOVIE_CLIP     = 0x04,
    AMF0_NULL           = 0x05,
    AMF0_UNDEFINED      = 0x06,
    AMF0_REFERENCE      = 0x07,
    AMF0_ECMA_ARRAY     = 0x08,
    AMF0_OBJECT_END     = 0x09,
    AMF0_STRICT_ARRAY   = 0x0A,
    AMF0_DATE           = 0x0B,
    AMF0_LONG_STRING    = 0x0C,
    AMF0_UNSUPPORTED    = 0x0D,
    AMF0_RECORDSET      = 0x0E,
    AMF0_XML_DOCUMENT   = 0x0F,
    AMF0_TYPED_OBJECT   = 0x10,
    AMF0_AVMPLUS_OBJECT = 0x11
};

uint8_t *amf0_write(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *items, uint32_t count);

uint8_t *amf0_write_number(uint8_t *ptr, const uint8_t *end, double value);
uint8_t *amf0_write_boolean(uint8_t *ptr, const uint8_t *end, uint8_t value);
uint8_t *amf0_write_string(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);
uint8_t *amf0_write_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf0_write_null(uint8_t *ptr, const uint8_t *end);
uint8_t *amf0_write_undefined(uint8_t *ptr, const uint8_t *end);
uint8_t *amf0_write_reference(uint8_t *ptr, const uint8_t *end, uint16_t reference);
uint8_t *amf0_write_ecma_array(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf0_write_object_end(uint8_t *ptr, const uint8_t *end);
uint8_t *amf0_write_strict_array(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf0_write_date(uint8_t *ptr, const uint8_t *end, double milliseconds, uint16_t timezone);
uint8_t *amf0_write_long_string(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);
uint8_t *amf0_write_xml_document(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf0_write_typed_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf0_write_avmplus_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);

const uint8_t *amf0_read(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *items, uint32_t count);

const uint8_t *amf0_read_number(const uint8_t *data, const uint8_t *end, double *value);
const uint8_t *amf0_read_boolean(const uint8_t *data, const uint8_t *end, uint8_t *value);
const uint8_t *amf0_read_string(const uint8_t *data, const uint8_t *end, char *string, uint32_t *length);
const uint8_t *amf0_read_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);
const uint8_t *amf0_read_null(const uint8_t *data, const uint8_t *end);
const uint8_t *amf0_read_undefined(const uint8_t *data, const uint8_t *end);
const uint8_t *amf0_read_reference(const uint8_t *data, const uint8_t *end, uint16_t *reference);
const uint8_t *amf0_read_ecma_array(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);
const uint8_t *amf0_read_object_end(const uint8_t *data, const uint8_t *end);
const uint8_t *amf0_read_strict_array(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);
const uint8_t *amf0_read_date(const uint8_t *data, const uint8_t *end, double *milliseconds, uint16_t *timezone);
const uint8_t *amf0_read_long_string(const uint8_t *data, const uint8_t *end, char *string, uint32_t *length);
const uint8_t *amf0_read_xml_document(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);
const uint8_t *amf0_read_typed_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);
const uint8_t *amf0_read_avmplus_object(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);



#endif // LIBAMF_AMF0_H
