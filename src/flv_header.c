//
// Created by BoringWednesday on 2021/8/10.
//

#include "flv_header.h"
#include "flv_type.h"

#include <assert.h>
#include <string.h>

static inline const uint8_t *flv_read_uint24_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val)
{
    if (ptr + 3 > end)
        return NULL;

    *val = (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];

    return ptr + 4;
}

static inline const uint8_t *flv_read_uint32_be(const uint8_t *ptr, const uint8_t *end, uint32_t *val)
{
    if (ptr + 4 > end)
        return NULL;

    *val = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];

    return ptr + 4;
}

static inline uint8_t *flv_write_uint24_be(uint8_t *ptr, const uint8_t *end, uint32_t val)
{
    if (ptr + 3 > end)
        return NULL;

    ptr[0] = (uint8_t) ((val >> 16) & 0xFF);
    ptr[1] = (uint8_t) ((val >> 8) & 0xFF);
    ptr[2] = (uint8_t) (val & 0xFF);

    return ptr + 3;
}

static inline uint8_t *flv_write_uint32_be(uint8_t *ptr, const uint8_t *end, uint32_t val)
{
    if (ptr + 4 > end)
        return NULL;

    ptr[0] = (uint8_t) ((val >> 24) & 0xFF);
    ptr[1] = (uint8_t) ((val >> 16) & 0xFF);
    ptr[2] = (uint8_t) ((val >> 8) & 0xFF);
    ptr[3] = (uint8_t) (val & 0xFF);

    return ptr + 4;
}

int flv_header_read(flv_header_t *header, const uint8_t *buf, uint32_t len)
{
    if (!header || len < FLV_HEADER_SIZE || 'F' != buf[0] || 'L' != buf[1] || 'V' != buf[2])
        return -1;

    assert(0x00 == (buf[4] & 0xFA));

    memmove(header->FLV, buf, 3);
    header->version = buf[3];
    header->audio   = (buf[4] >> 2) & 0x01;
    header->video   = buf[4] & 0x01;
    flv_read_uint32_be(buf + 5, buf + len, &header->data_offset);

    if (1 == header->version && FLV_HEADER_SIZE != header->data_offset)
        return -1;

    return FLV_HEADER_SIZE;
}

int flv_header_write(int audio, int video, uint8_t *buf, uint32_t len)
{
    if (!buf || len < FLV_HEADER_SIZE) {
        return -1;
    }

    buf[0] = 'F';
    buf[1] = 'L';
    buf[2] = 'V';
    buf[3] = 0x01;
    buf[4] = ((audio ? 1 : 0) << 2) | (video ? 1 : 0);
    flv_write_uint32_be(buf + 5, buf + len, FLV_HEADER_SIZE);

    return FLV_HEADER_SIZE;
}

int flv_tag_read(flv_tag_t *tag, const uint8_t *buf, uint32_t len)
{
    const uint8_t *end;

    if (!tag || !buf || len < FLV_TAG_HEADER_SIZE) {
        return -1;
    }

    end = buf + len;

    tag->filter   = (buf[0] >> 5) & 0x01;
    tag->tag_type = buf[0] & 0x1F;
    flv_read_uint24_be(buf + 1, end, &tag->data_size);
    flv_read_uint24_be(buf + 4, end, &tag->timestamp);
    if (0 != buf[7]) {
        // Read timestamp extended
        tag->timestamp |= buf[7] << 24;
    }

    flv_read_uint24_be(buf + 8, end, &tag->stream_id);

    assert(0 == tag->stream_id);
    assert(FLV_VIDEO == tag->tag_type || FLV_AUDIO == tag->tag_type || FLV_SCRIPT == tag->tag_type);

    return FLV_TAG_HEADER_SIZE;
}

int flv_tag_write(const flv_tag_t *tag, uint8_t *buf, uint32_t len)
{
    const uint8_t *end;

    if (!tag || !buf || len < FLV_TAG_HEADER_SIZE)
        return -1;

    assert(0 == tag->stream_id);
    assert(FLV_VIDEO == tag->tag_type || FLV_AUDIO == tag->tag_type || FLV_SCRIPT == tag->tag_type);

    end = buf + len;

    buf[0] = ((tag->filter & 0x01) << 5) | (tag->tag_type & 0x1F);
    flv_write_uint24_be(buf + 1, end, tag->data_size);
    flv_write_uint24_be(buf + 4, end, tag->timestamp);
    buf[5] = (tag->timestamp >> 24) & 0xFF;
    flv_write_uint24_be(buf + 8, end, tag->stream_id);

    return FLV_TAG_HEADER_SIZE;
}

int flv_audio_tag_header_read(flv_audio_tag_header_t *audio_tag, const uint8_t *buf, uint32_t len)
{
    if (!audio_tag || !buf || len < 1)
        return -1;

    audio_tag->sound_format = (buf[0] >> 4) & 0x0F;
    audio_tag->sound_rate   = (buf[0] >> 2) & 0x03;
    audio_tag->sound_size   = (buf[0] >> 1) & 0x01;
    audio_tag->sound_type   = buf[0] & 0x01;

    assert(audio_tag->sound_format <= 15 && 9 != audio_tag->sound_format && 13 != audio_tag->sound_format);

    if (FLV_AUDIO_AAC == audio_tag->sound_format) {
        if (len < 2)
            return -1;

        audio_tag->aac_packet_type = buf[1];
        assert(buf[1] <= 1);

        return 2;
    }

    return 1;
}

int flv_audio_tag_header_write(const flv_audio_tag_header_t *audio_tag, uint8_t *buf, uint32_t len)
{
    if (!audio_tag || !buf || len < 1 + (uint32_t) (FLV_AUDIO_AAC == audio_tag->sound_format ? 1 : 0))
        return -1;

    assert(audio_tag->sound_format <= 15 && 9 != audio_tag->sound_format && 13 != audio_tag->sound_format);

    buf[0] = ((audio_tag->sound_format & 0x0F) << 4);
    buf[0] |= ((audio_tag->sound_rate & 0x03) << 2);
    buf[0] |= ((audio_tag->sound_size & 0x01) << 1);
    buf[0] |= audio_tag->sound_type & 0x01;

    if (FLV_AUDIO_AAC == audio_tag->sound_format) {
        buf[1] = audio_tag->aac_packet_type;
        assert(audio_tag->aac_packet_type <= 1);

        return 2;
    }

    return 1;
}

int flv_video_tag_header_read(flv_video_tag_header_t *video_tag, const uint8_t *buf, uint32_t len)
{
    uint32_t composition_time_offset;

    if (!video_tag || !buf || len < 1) {
        return -1;
    }

    video_tag->frame_type = (buf[0] >> 4) & 0x0F;
    video_tag->codec_id   = buf[0] & 0x0F;

    assert(video_tag->frame_type <= 5 && video_tag->codec_id >= 2 && video_tag->codec_id <= 7);

    if (FLV_VIDEO_H264 == video_tag->codec_id) {
        if (len < 5)
            return -1;

        video_tag->avc_packet_type = buf[1];
        assert(video_tag->avc_packet_type <= 2);

        // Transfer Signed INT24 to Signed INT32
        flv_read_uint24_be(buf + 2, buf + len, &composition_time_offset);
        composition_time_offset            = (composition_time_offset + 0xFF800000) ^ 0xFF800000;
        video_tag->composition_time_offset = (int32_t) composition_time_offset;

        return 5;
    }

    return 1;
}

int flv_video_tag_header_write(const flv_video_tag_header_t *video_tag, uint8_t *buf, uint32_t len)
{
    uint32_t composition_time_offset;

    if (!video_tag || !buf || len < 1 + (uint32_t) (FLV_VIDEO_H264 == video_tag->codec_id ? 4 : 0))
        return -1;

    assert(video_tag->frame_type <= 5 && video_tag->codec_id >= 2 && video_tag->codec_id <= 7);

    buf[0] = ((video_tag->frame_type & 0x0F) << 4) | (video_tag->codec_id & 0x0F);

    if (FLV_VIDEO_H264 == video_tag->codec_id) {
        buf[1] = video_tag->avc_packet_type;

        // Transfer Signed INT32 to Signed INT24
        composition_time_offset = (uint32_t) video_tag->composition_time_offset;
        composition_time_offset = (composition_time_offset ^ 0xFF800000) - 0xFF800000;
        flv_write_uint24_be(buf + 2, buf + len, composition_time_offset);

        return 5;
    }

    return 1;
}

int flv_tag_size_read(const uint8_t *buf, uint32_t len, uint32_t *tag_size)
{
    return flv_read_uint32_be(buf, buf + len, tag_size) ? 4 : 0;
}

int flv_tag_size_write(uint8_t *buf, uint32_t len, uint32_t tag_size)
{
    return flv_write_uint32_be(buf, buf + len, tag_size) ? 4 : 0;
}
