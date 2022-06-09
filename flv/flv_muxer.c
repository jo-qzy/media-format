//
// Created by BoringWednesday on 2021/8/8.
//

#include <core/mfa_core.h>
#include <flv/flv_muxer.h>
#include <flv/h264.h>
#include <flv/mpeg4_aac.h>

typedef struct flv_muxer_t
{
    void      *param;
    mfa_buf_t *buf;

    flv_muxer_handler handler;

    union
    {
        h264_config_t avc;
    } video;

    union
    {
        mpeg4_aac_t aac;
    } audio;
} flv_muxer_t;

int flv_muxer_aac(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts)
{
    if (mfa_buf_size(muxer->buf))
        return 0;

    return 0;
}

int flv_muxer_avc(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts)
{
    return 0;
}
