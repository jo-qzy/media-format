//
// Created by BoringWednesday on 2021/7/27.
//

#ifndef LIBAMF_ITEM_H
#define LIBAMF_ITEM_H

#include <stddef.h>
#include <stdint.h>

#define AMF_CHECK_POINTER(ptr) if (!ptr) return NULL

struct amf_object_item_t
{
    int         type;
    const char *name;
    void       *value;
    uint32_t    size;
};

struct amf_object_item_t *amf_item_create();
void                      amf_item_destroy(struct amf_object_item_t *item, int version);

#endif // LIBAMF_ITEM_H
