//
// Created by quzhenyu on 2021/12/6.
// Copyright (c) 2021 quzhenyu. All rights reserved.
//

#ifndef LIBFLV_MPEG4_AVC_H
#define LIBFLV_MPEG4_AVC_H

#include <stdint.h>

// ISO/IEC 14496-15:2010(E), 5.2.4.1.1 AVC Decoder Configuration Record
typedef struct mpeg4_avc_t
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
} mpeg4_avc_t;

enum mpeg4_nal_type_t
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

typedef struct mpeg4_vec_t
{
    const uint8_t *data;
    uint32_t       bytes;
} mpeg4_vec_t;

// API for AVCC format
int mpeg4_decode_avc_decoder_configuration_record(mpeg4_avc_t *avc, const void *data, uint32_t bytes);
int mpeg4_get_avc_decoder_configuration_record(mpeg4_avc_t *avc, uint8_t *data, uint32_t bytes);
int mpeg4_avcc_to_annexb(mpeg4_avc_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                         uint32_t out_bytes);

// API for Annex-B format
int mpeg4_update_sps_pps(mpeg4_avc_t *avc, const void *data, uint32_t bytes);
int mpeg4_annexb_to_avcc(mpeg4_avc_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                         uint32_t out_bytes);

typedef struct mpeg4_avcc_handler
{
    void        *param;
    mpeg4_avc_t *avc;
    int (*on_write)(void *param, int type, mpeg4_vec_t *vec, uint32_t len);
} mpeg4_avcc_handler;
int mpeg4_annexb_to_avcc_bitstream(const void *in_data, uint32_t in_bytes, mpeg4_avcc_handler *handler);

int mpeg4_get_sps_pps(mpeg4_avc_t *avc, uint8_t annexb, uint8_t *data, uint32_t bytes);

#endif // LIBFLV_MPEG4_AVC_H
