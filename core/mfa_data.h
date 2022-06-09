//
// Created by quzhenyu on 2022/6/7.
//

#ifndef DATA_H
#define DATA_H


#include <stdint.h>
#include <stddef.h>

typedef struct vec_t
{
    const uint8_t *data;
    uint32_t       bytes;
} vec_t;

const uint8_t *mfa_read_uint16(const uint8_t *ptr, const uint8_t *end, uint16_t *val);
const uint8_t *mfa_read_uint24(const uint8_t *ptr, const uint8_t *end, uint32_t *val);
const uint8_t *mfa_read_uint32(const uint8_t *ptr, const uint8_t *end, uint32_t *val);

uint8_t *mfa_write_uint16(uint8_t *ptr, const uint8_t *end, uint16_t val);
uint8_t *mfa_write_uint24(uint8_t *ptr, const uint8_t *end, uint32_t val);
uint8_t *mfa_write_uint32(uint8_t *ptr, const uint8_t *end, uint32_t val);
uint8_t *mfa_write_double(uint8_t *ptr, const uint8_t *end, double value);


#endif // DATA_H
