//
// Created by quzhenyu on 2021/12/3.
//

#include <flv/flv_demuxer.h>
#include <flv/flv_header.h>
#include <flv/flv_type.h>
#include <flv/mpeg4_aac.h>
#include <flv/h264.h>

#include <memory.h>
#include <stdlib.h>

struct flv_demuxer_t
{
    void    *param;
    uint8_t *buffer;
    uint32_t capacity;

    flv_demuxer_handler handler;

    union
    {
        h264_config_t avc;
    } video;

    union
    {
        struct mpeg4_aac_t aac;
    } audio;
};

flv_demuxer_t *flv_demuxer_create(void *param, flv_demuxer_handler handler)
{
    struct flv_demuxer_t *demuxer;

    demuxer = (struct flv_demuxer_t *) malloc(sizeof(struct flv_demuxer_t));
    if (NULL == demuxer)
        return NULL;

    memset(demuxer, 0, sizeof(struct flv_demuxer_t));
    demuxer->handler = handler;
    demuxer->param   = param;

    return demuxer;
}

void flv_demuxer_free(flv_demuxer_t *demuxer)
{
    if (!demuxer)
        return;

    if (demuxer->buffer)
        free(demuxer->buffer);

    free(demuxer);
}

static int flv_demuxer_check_and_alloc(flv_demuxer_t *demuxer, uint32_t bytes)
{
    if (bytes > demuxer->capacity) {
        void *ptr = realloc(demuxer->buffer, bytes);
        if (!ptr)
            return -1;

        demuxer->buffer   = (uint8_t *) ptr;
        demuxer->capacity = bytes;
    }

    return 0;
}

static int flv_demuxer_audio(flv_demuxer_t *demuxer, const uint8_t *data, uint32_t bytes, uint32_t timestamp)
{
    flv_audio_tag_header_t audio_header;
    int                    read_size;

    read_size = flv_audio_tag_header_read(&audio_header, data, bytes);
    if (-1 == read_size)
        return -1;

    switch (audio_header.sound_format) {
        case FLV_AUDIO_AAC:
            if (FLV_SEQUENCE_HEADER == audio_header.aac_packet_type) {
                // ISO/IEC 14496-3: AudioSpecificConfig
                if (-1 == mpeg4_decode_audio_specific_config(&demuxer->audio.aac, data + read_size, bytes - read_size))
                    return -1;

                return demuxer->handler(demuxer->param, FLV_AUDIO_ASC, data + read_size, bytes - read_size, timestamp,
                                        0, 0);
            } else if (FLV_MEDIA_PACKET == audio_header.aac_packet_type) {
                // AAC stream is RAW stream in flv, need transfer to ADTS format
                read_size = mpeg4_aac_raw_to_adts(&demuxer->audio.aac, data + read_size, bytes - read_size,
                                                  demuxer->buffer, demuxer->capacity);

                return demuxer->handler(demuxer->param, FLV_AUDIO_AAC, demuxer->buffer, read_size, timestamp, timestamp,
                                        0);
            }
            break;
        default:
            return -1;
    }

    return demuxer->handler(demuxer->param, audio_header.sound_format, data + read_size, bytes - read_size, timestamp,
                            timestamp, 0);
}

static int flv_demuxer_video(flv_demuxer_t *demuxer, const uint8_t *data, uint32_t bytes, uint32_t timestamp)
{
    flv_video_tag_header_t video_header;
    int                    read_size;

    read_size = flv_video_tag_header_read(&video_header, data, bytes);
    if (-1 == read_size)
        return -1;

    switch (video_header.codec_id) {
        case FLV_VIDEO_H264:
            if (FLV_SEQUENCE_HEADER == video_header.packet_type) {
                // ISO/IEC 14496-15: AVCDecoderConfigurationRecord
                // ISO/IEC 14496-10: Sequence parameter set RBSP syntax
                // ISO/IEC 14496-10: Picture parameter set RBSP syntax
                if (-1 == h264_decode_extradata(&demuxer->video.avc, data + read_size,
                                                                        bytes - read_size))
                    return -1;

                return 0;
            } else if (FLV_MEDIA_PACKET == video_header.packet_type) {
                // H.264 stream is AVCC format in flv, need transfer to Annex-B format
                read_size = h264_avcc_to_annexb(&demuxer->video.avc, data + read_size, bytes - read_size,
                                                 demuxer->buffer, demuxer->capacity);
                if (read_size < 0)
                    return -1;

                return demuxer->handler(demuxer->param, FLV_VIDEO_H264, demuxer->buffer, read_size,
                                        timestamp + video_header.cts, timestamp,
                                        FLV_KEY_FRAME == video_header.frame_type ? 1 : 0);
            }
    }

    return demuxer->handler(demuxer->param, video_header.codec_id, data + read_size, bytes - read_size,
                            timestamp + video_header.cts, timestamp,
                            (FLV_KEY_FRAME == video_header.frame_type) ? 1 : 0);
}

int flv_demuxer_input(flv_demuxer_t *demuxer, int tag_type, const void *data, uint32_t bytes, uint32_t timestamp)
{
    if (!demuxer)
        return -1;

    if (0 != flv_demuxer_check_and_alloc(demuxer, bytes + 1024))
        return -22;

    switch (tag_type) {
        case FLV_AUDIO: return flv_demuxer_audio(demuxer, data, bytes, timestamp);
        case FLV_VIDEO: return flv_demuxer_video(demuxer, data, bytes, timestamp);
        case FLV_SCRIPT: return 0; //flv_demuxer_script();
        default: return -1;
    }
}
