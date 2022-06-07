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

typedef struct buf_t
{
    uint8_t *start;
    uint8_t *cur;
    uint8_t *end;
} buf_t;

const uint8_t *read_uint16_be(const uint8_t *ptr, const uint8_t *end, uint16_t *val);
const uint8_t *read_uint24_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val);
const uint8_t *read_uint32_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val);

uint8_t *write_uint16_be(uint8_t *ptr, const uint8_t *end, uint16_t val);
uint8_t *write_uint24_be(uint8_t *ptr, const uint8_t *end, uint32_t val);
uint8_t *write_uint32_be(uint8_t *ptr, const uint8_t *end, uint32_t val);
uint8_t *write_double_be(uint8_t *ptr, const uint8_t *end, double value);


#endif // DATA_H
