//
// Created by BoringWednesday on 2021/7/7.
//

#ifndef LIBAMF_AMF3_H
#define LIBAMF_AMF3_H

#include <flv/amf_item.h>

#ifdef __cplusplus
extern "C" {
#endif

enum amf3_type_t
{
    AMF3_UNDEFINED    = 0x00,
    AMF3_NULL         = 0x01,
    AMF3_FALSE        = 0x02,
    AMF3_TRUE         = 0x03,
    AMF3_INTEGER      = 0x04,
    AMF3_DOUBLE       = 0x05,
    AMF3_STRING       = 0x06,
    AMF3_XML_DOCUMENT = 0x07,
    AMF3_DATE         = 0x08,
    AMF3_ARRAY        = 0x09,
    AMF3_OBJECT       = 0x0A,
    AMF3_XML          = 0x0B,
    AMF3_BYTEARRAY    = 0x0C
};

uint8_t *amf3_write(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);

uint8_t *amf3_write_undefined(uint8_t *ptr, const uint8_t *end);
uint8_t *amf3_write_null(uint8_t *ptr, const uint8_t *end);
uint8_t *amf3_write_false(uint8_t *ptr, const uint8_t *end);
uint8_t *amf3_write_true(uint8_t *ptr, const uint8_t *end);
uint8_t *amf3_write_integer(uint8_t *ptr, const uint8_t *end, uint32_t value);
uint8_t *amf3_write_double(uint8_t *ptr, const uint8_t *end, double value);
uint8_t *amf3_write_string(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);
uint8_t *amf3_write_xml_document(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);
uint8_t *amf3_write_date(uint8_t *ptr, const uint8_t *end, double timestamp);
uint8_t *amf3_write_array(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf3_write_object(uint8_t *ptr, const uint8_t *end, struct amf_object_item_t *item);
uint8_t *amf3_write_xml(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);
uint8_t *amf3_write_byte_array(uint8_t *ptr, const uint8_t *end, const char *string, uint32_t length);

const uint8_t *amf3_read(const uint8_t *data, const uint8_t *end, struct amf_object_item_t *item);

#ifdef __cplusplus
}
#endif

#endif // LIBAMF_AMF3_H
