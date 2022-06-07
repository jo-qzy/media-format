//
// Created by quzhenyu on 2022/6/7.
//

#include <base/data.h>

const uint8_t *read_uint16_be(const uint8_t *ptr, const uint8_t *end, uint16_t *val)
{
    if (ptr + 2 > end)
        return NULL;

    *val = (ptr[0] << 8) | ptr[1];

    return ptr + 2;
}

const uint8_t *read_uint24_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val)
{
    if (ptr + 3 > end)
        return NULL;

    *val = (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];

    return ptr + 3;
}

const uint8_t *read_uint32_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val)
{
    if (ptr + 4 > end)
        return NULL;

    *val = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];

    return ptr + 4;
}

uint8_t *write_uint16_be(uint8_t *ptr, const uint8_t *end, uint16_t val)
{
    if (ptr + 2 > end)
        return NULL;

    ptr[0] = (uint8_t) ((val >> 8) & 0xFF);
    ptr[1] = (uint8_t) (val & 0xFF);

    return ptr + 2;
}

uint8_t *write_uint24_be(uint8_t *ptr, const uint8_t *end, uint32_t val)
{
    if (ptr + 3 > end)
        return NULL;

    ptr[0] = (uint8_t) ((val >> 16) & 0xFF);
    ptr[1] = (uint8_t) ((val >> 8) & 0xFF);
    ptr[2] = (uint8_t) (val & 0xFF);

    return ptr + 3;
}

uint8_t *write_uint32_be(uint8_t *ptr, const uint8_t *end, uint32_t val)
{
    if (ptr + 4 > end)
        return NULL;

    ptr[0] = (uint8_t) ((val >> 24) & 0xFF);
    ptr[1] = (uint8_t) ((val >> 16) & 0xFF);
    ptr[2] = (uint8_t) ((val >> 8) & 0xFF);
    ptr[3] = (uint8_t) (val & 0xFF);

    return ptr + 4;
}

uint8_t *write_double_be(uint8_t *ptr, const uint8_t *end, double value)
{
    uint8_t *double_ptr;

    if (!ptr || ptr + 8 > end)
        return NULL;

    double_ptr = (uint8_t *) &value;

    ptr[0] = double_ptr[7];
    ptr[1] = double_ptr[6];
    ptr[2] = double_ptr[5];
    ptr[3] = double_ptr[4];
    ptr[4] = double_ptr[3];
    ptr[5] = double_ptr[2];
    ptr[6] = double_ptr[1];
    ptr[7] = double_ptr[0];

    return ptr + 8;
}
