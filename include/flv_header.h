//
// Created by BoringWednesday on 2021/8/9.
//
// Base on Adobe document:
//  https://www.adobe.com/content/dam/acom/en/devnet/flv/video_file_format_spec_v10_1.pdf
//

#ifndef LIBFLV_FLV_HEADER_H
#define LIBFLV_FLV_HEADER_H

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FLV_HEADER_SIZE         9
#define FLV_PREVIOUS_TAG_LENGTH 4
#define FLV_TAG_HEADER_SIZE     11

typedef struct flv_header_t
{
    uint8_t  FLV[3];
    uint8_t  version;
    uint8_t  audio;
    uint8_t  video;
    uint32_t data_offset;
} flv_header_t;

typedef struct flv_tag_t
{
    // 0 - No pre-processing required
    // 1 - Pre-processing of packet is reuiqred
    uint8_t  filter : 1;
    uint8_t  tag_type : 5; // 8-audio, 9-video, 18-script
    uint32_t data_size;    // Data length after StreamID
    uint32_t timestamp;    // Include extended timestamp
    uint32_t stream_id;
} flv_tag_t;

typedef struct flv_audio_tag_header_t
{
    uint8_t sound_format;    // 10-AAC, more type see flv_type.h
    uint8_t sound_rate;      // 0-5.5kHz, 1-11kHz, 2-22kHz, 3-44kHz
    uint8_t sound_size;      // 0-8-bit samples, 1-16-bit samples
    uint8_t sound_type;      // 0-Mono sound, 1-Stereo sound
    uint8_t aac_packet_type; // 0-AAC sequence header, 1-AAC raw
} flv_audio_tag_header_t;

typedef struct flv_video_tag_header_t
{
    uint8_t frame_type;              // 1-key frame, 2-inter frame, 3-5 can see in doc
    uint8_t codec_id;                // 7-AVC
    uint8_t avc_packet_type;         // 0-AVC sequence header, 1-AVC NALU, 2-AVC end of sequence
    int32_t composition_time_offset; // CTS
} flv_video_tag_header_t;

int flv_header_read(flv_header_t *header, const uint8_t *buf, uint32_t len);
int flv_header_write(int audio, int video, uint8_t *buf, uint32_t len);

int flv_tag_read(flv_tag_t *tag, const uint8_t *buf, uint32_t len);
int flv_tag_write(const flv_tag_t *tag, uint8_t *buf, uint32_t len);

int flv_audio_tag_header_read(flv_audio_tag_header_t *audio_tag, const uint8_t *buf, uint32_t len);
int flv_audio_tag_header_write(const flv_audio_tag_header_t *audio_tag, uint8_t *buf, uint32_t len);

int flv_video_tag_header_read(flv_video_tag_header_t *video_tag, const uint8_t *buf, uint32_t len);
int flv_video_tag_header_write(const flv_video_tag_header_t *video_tag, uint8_t *buf, uint32_t len);

int flv_tag_size_read(const uint8_t *buf, uint32_t len, uint32_t *tag_size);
int flv_tag_size_write(uint8_t *buf, uint32_t len, uint32_t tag_size);

#if defined(__cplusplus)
}
#endif

#endif // LIBFLV_FLV_HEADER_H
