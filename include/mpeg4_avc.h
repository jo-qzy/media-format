//
// Created by quzhenyu on 2021/12/6.
// Copyright (c) 2021 quzhenyu. All rights reserved.
//

#ifndef LIBFLV_MPEG4_AVC_H
#define LIBFLV_MPEG4_AVC_H

#include <stdint.h>

// ISO/IEC 14496-15:2010(E) 5.2.4.1.1
// AVC Decoder Configuration Record
typedef struct mpeg4_avc_t
{
    uint8_t configuration_version;
    uint8_t avc_profile_indication;
    uint8_t profile_compatibility;
    uint8_t avc_level_indication;
    uint8_t length_size_minus_one:2;
    uint8_t num_of_sequence_parameter_sets:5;
    uint8_t num_of_picture_parameter_sets;

    struct picture_parameter_sets_t
    {
        uint16_t        bytes;
        const uint8_t  *data;
    } pps[31];

    struct sequence_parameter_sets_t
    {
        uint16_t        bytes;
        const uint8_t  *data;
    } sps[255];

    // profile_idc == 100/110/122/144
    uint8_t chroma_format:2;
    uint8_t bit_depth_luma_minus8:3;
    uint8_t bit_depth_chroma_minus8:3;
    uint8_t num_of_sequence_parameter_set_ext_length;

    struct sequence_parameter_set_ext_t
    {
        uint16_t        bytes;
        const uint8_t  *data;
    } sps_ext[255];
} mpeg4_avc_t;

int mpeg4_decode_avc_decoder_configuration_record(mpeg4_avc_t *avc, const void *data, uint32_t bytes);
int mpeg4_get_avc_decoder_configuration_record(mpeg4_avc_t *avc, uint8_t annexb, uint8_t *data, uint32_t bytes);

int mpeg4_avcc_to_annexb(mpeg4_avc_t *avc, const void *in_data, uint32_t in_bytes,
                         uint8_t *out_data, uint32_t out_bytes);

#endif //LIBFLV_MPEG4_AVC_H
