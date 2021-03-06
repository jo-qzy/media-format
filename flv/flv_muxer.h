//
// Created by BoringWednesday on 2021/8/8.
//

#ifndef FLV_MUXER_H
#define FLV_MUXER_H

#include <flv/flv.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct flv_muxer_t flv_muxer_t;

typedef int (*flv_muxer_handler)(void* param, int type, const void* data, size_t bytes, uint32_t timestamp);

flv_muxer_t *flv_muxer_create(void *param, flv_muxer_handler handler);

int flv_muxer_aac(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts);

int flv_muxer_h264(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts);

#if defined(__cplusplus)
}
#endif

#endif // FLV_MUXER_H
