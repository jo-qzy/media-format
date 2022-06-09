//
// Created by quzhenyu on 2021/12/6.
// Copyright (c) 2021 quzhenyu. All rights reserved.
//

#ifndef LIBFLV_MPEG4_AVC_H
#define LIBFLV_MPEG4_AVC_H

#include <stdint.h>

// ISO/IEC 14496-15:2010(E), 5.2.4.1.1 AVC Decoder Configuration Record
typedef struct h264_config_t
{
    uint8_t configuration_version;
    uint8_t avc_profile_indication;
    uint8_t profile_compatibility;
    uint8_t avc_level_indication;
    uint8_t length_size_minus_one : 2;
    uint8_t num_of_sequence_parameter_sets : 5;
    uint8_t num_of_picture_parameter_sets;

    struct picture_parameter_sets_t
    {
        uint16_t       bytes;
        const uint8_t *data;
    } pps[31];

    struct sequence_parameter_sets_t
    {
        uint16_t       bytes;
        const uint8_t *data;
    } sps[255];

    // profile_idc == 100/110/122/144
    uint8_t chroma_format : 2;
    uint8_t bit_depth_luma_minus8 : 3;
    uint8_t bit_depth_chroma_minus8 : 3;
    uint8_t num_of_sequence_parameter_set_ext_length;

    struct sequence_parameter_set_ext_t
    {
        uint16_t       bytes;
        const uint8_t *data;
    } sps_ext[255];

    uint8_t  data[1024];
    uint32_t data_size;
} h264_config_t;

enum h264_nal_type_t
{
    // For Annex-B and AVCC
    H264_NAL_SLICE = 1, // Coded slice of a non-IDR picture
    H264_NAL_IDR   = 5, // Coded slice of an IDR picture
    H264_NAL_SEI   = 6, // Supplemental enhancement information
    H264_NAL_SPS   = 7, // Sequence parameter set
    H264_NAL_PPS   = 8, // Picture parameter set
    H264_NAL_AUD   = 9, // Access unit delimiter

    // For AVCC only
    AVCC_EXT_DATA = 0, // sequence header
};

/* API for AVCC format */
int h264_decode_extradata(h264_config_t *avc, const void *data, uint32_t bytes);
int h264_extradata_size(h264_config_t *avc);
int h264_get_extradata(h264_config_t *avc, uint8_t *data, uint32_t bytes);
int h264_avcc_to_annexb(h264_config_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                        uint32_t out_bytes);

/* API for Annex-B format */
int h264_decode_sps_pps(h264_config_t *avc, const void *data, uint32_t bytes);
int h264_sps_pps_size(h264_config_t *avc);
int h264_get_sps_pps(h264_config_t *avc, uint8_t *data, uint32_t bytes);
int h264_annexb_to_avcc(h264_config_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                         uint32_t out_bytes);

/* API for AVCC and Annex-B bitstream */
typedef int (*h264_bitstream_handler)(void *param, int type, const uint8_t *data, uint32_t bytes);

typedef struct h264_bitstream_t h264_bitstream_t;

h264_bitstream_t *h264_bitstream_create(void *param, h264_config_t *avc, h264_bitstream_handler handler);

void h264_bitstream_free(h264_bitstream_t *avc_bitstream);

int h264_annexb_to_avcc_bitstream(h264_bitstream_t *bitstream, const void *data, uint32_t bytes);

int h264_avcc_to_annexb_bitstream(h264_bitstream_t *bitstream, const void *data, uint32_t bytes);

#endif // LIBFLV_MPEG4_AVC_H
