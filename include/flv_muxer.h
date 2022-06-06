//
// Created by BoringWednesday on 2021/8/8.
//

#ifndef LIBFLV_FLV_MUXER_H
#define LIBFLV_FLV_MUXER_H

#include "flv.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct flv_muxer_t flv_muxer_t;

typedef int (*flv_muxer_handler)(void* param, int type, const void* data, size_t bytes, uint32_t timestamp);

int flv_muxer_aac(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts);

int flv_muxer_avc(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts);

#if defined(__cplusplus)
}
#endif

#endif // LIBFLV_FLV_MUXER_H
