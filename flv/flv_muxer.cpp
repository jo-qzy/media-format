//
// Created by BoringWednesday on 2021/8/8.
//

#include <stdlib.h>
#include <string.h>

#include <core/mfa_core.h>
#include <flv/flv_header.h>
#include <flv/flv_muxer.h>
#include <flv/flv_type.h>
#include <flv/h264.h>
#include <flv/mpeg4_aac.h>

typedef struct flv_muxer_t
{
    void      *param;
    mfa_buf_t *buf;
    uint8_t    update_sequence;

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

flv_muxer_t *flv_muxer_create(void *param, flv_muxer_handler handler)
{
    if (!handler)
        return NULL;

    flv_muxer_t *muxer = (flv_muxer_t *) malloc(sizeof(flv_muxer_t));
    if (!muxer)
        return NULL;

    memset(muxer, 0, sizeof(flv_muxer_t));

    muxer->buf = mfa_buf_alloc(MFA_BUF_DEFAULT_SIZE);
    if (!muxer->buf) {
        free(muxer);
        return NULL;
    }

    muxer->param   = param;
    muxer->handler = handler;

    return muxer;
}

void flv_muxer_free(flv_muxer_t *muxer)
{
    if (!muxer)
        return;

    mfa_buf_free(muxer->buf);
    free(muxer);
}

static int flv_muxer_check_buf(flv_muxer_t *muxer, uint32_t bytes)
{
    if (mfa_buf_capacity(muxer->buf) < (int) bytes) {
        uint32_t dst_bytes = mfa_buf_capacity(muxer->buf);
        while (dst_bytes < bytes)
            dst_bytes *= 2;

        if (MFA_OK != mfa_buf_resize(muxer->buf, dst_bytes))
            return MFA_ERROR;
    }

    mfa_buf_flush(muxer->buf);

    return MFA_OK;
}

int flv_muxer_aac(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts)
{
    if (MFA_OK != flv_muxer_check_buf(muxer, bytes))
        return MFA_ERROR;

    return 0;
}

int flv_muxer_h264(flv_muxer_t *muxer, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts)
{
    int                    ret;
    int                    update = 0;
    int                    vcl    = 0;
    flv_video_tag_header_t tag;

    if (MFA_OK != flv_muxer_check_buf(muxer, bytes))
        return MFA_ERROR;

    ret = h264_annexb_to_avcc(&muxer->video.avc, data, bytes, muxer->buf->start + 5, mfa_buf_capacity(muxer->buf) - 5,
                              &update, &vcl);
    if (MFA_ERROR == ret)
        return MFA_ERROR;

    muxer->buf->last = muxer->buf->pos + 5 + ret;

    tag.codec_id    = FLV_VIDEO_H264;
    tag.cts         = (int32_t) (pts - dts);
    tag.frame_type  = vcl;
    tag.packet_type = FLV_MEDIA_PACKET;

    ret = flv_video_tag_header_write(&tag, muxer->buf->start, 5);
    if (MFA_ERROR == ret)
        return MFA_ERROR;

    if (update) {
        ret = h264_extradata_size(&muxer->video.avc);
        if (MFA_ERROR == ret)
            return MFA_AGAIN;

        muxer->update_sequence = 1;

        tag.cts         = 0;
        tag.frame_type  = FLV_KEY_FRAME;
        tag.packet_type = FLV_SEQUENCE_HEADER;

        ret = flv_video_tag_header_write(&tag, muxer->buf->last, mfa_buf_avaliable_size(muxer->buf));
        ret += h264_get_extradata(&muxer->video.avc, muxer->buf->last + ret, mfa_buf_avaliable_size(muxer->buf) - ret);

        ret = muxer->handler(muxer->param, FLV_VIDEO, muxer->buf->last, ret, dts);
        if (MFA_ERROR == ret)
            return MFA_ERROR;
    }

    if (!muxer->update_sequence)
        return MFA_AGAIN;

    return muxer->handler(muxer->param, FLV_VIDEO, muxer->buf->start, mfa_buf_size(muxer->buf), dts);
}
