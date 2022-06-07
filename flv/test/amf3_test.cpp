//
// Created by BoringWednesday on 2021/7/28.
//

#include <stdio.h>
#include <flv/amf3.h>

void am3_test()
{
    uint8_t buffer[256];

    uint8_t *ptr;

    ptr = amf3_write_string(buffer, buffer + 256, "hello", 5);
}
