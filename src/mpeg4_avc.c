//
// Created by quzhenyu on 2021/12/6.
// Copyright (c) 2021 quzhenyu. All rights reserved.
//

#include "mpeg4_avc.h"

#include <assert.h>
#include <memory.h>

enum h264_nal_type_t
{
    H264_NAL_IDR = 5,   // Coded slice of an IDR picture
    H264_NAL_SPS = 7,   // Sequence parameter set
    H264_NAL_PPS = 8,   // Picture parameter set
    H264_NAL_AUD = 9,   // Access unit delimiter
};

/*
ISO/IEC 14496-15:2010(E)

aligned(8) class AVCDecoderConfigurationRecord {
    unsigned int(8) configurationVersion = 1;
    unsigned int(8) AVCProfileIndication;
    unsigned int(8) profile_compatibility;
    unsigned int(8) AVCLevelIndication;
    bit(6) reserved = ‘111111’b;
    unsigned int(2) lengthSizeMinusOne;
    bit(3) reserved = ‘111’b;
    unsigned int(5) numOfSequenceParameterSets;

    for (i=0; i< numOfSequenceParameterSets; i++) {
        unsigned int(16) sequenceParameterSetLength ;
        bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
    }

    unsigned int(8) numOfPictureParameterSets;
    for (i=0; i< numOfPictureParameterSets; i++) {
        unsigned int(16) pictureParameterSetLength;
        bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
    }

    if( profile_idc == 100 || profile_idc == 110 ||
    profile_idc == 122 || profile_idc == 144 )
    {
        bit(6) reserved = ‘111111’b;
        unsigned int(2) chroma_format;
        bit(5) reserved = ‘11111’b;
        unsigned int(3) bit_depth_luma_minus8;
        bit(5) reserved = ‘11111’b;
        unsigned int(3) bit_depth_chroma_minus8;
        unsigned int(8) numOfSequenceParameterSetExt;
        for (i=0; i< numOfSequenceParameterSetExt; i++) {
            unsigned int(16) sequenceParameterSetExtLength;
            bit(8*sequenceParameterSetExtLength) sequenceParameterSetExtNALUnit;
        }
    }
}
*/

int mpeg4_decode_avc_decoder_configuration_record(mpeg4_avc_t *avc, const void *data, uint32_t bytes)
{
    int i;
    const uint8_t *cur, *end;

    if (bytes < 7)
        return -1;

    cur = data;
    end = cur + bytes;

    assert(cur[0] == 1);

    avc->configuration_version = cur[0];
    avc->avc_profile_indication = cur[1];
    avc->profile_compatibility = cur[2];
    avc->avc_level_indication = cur[3];
    avc->length_size_minus_one = cur[4] & 0x03;

    cur += 5;

    // Sequence Parameter Sets
    avc->num_of_sequence_parameter_sets = *cur++ & 0x1F;
    for (i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (cur + 2 > end)
            return -1;

        avc->sps[i].bytes = (cur[0] << 8) | cur[1];
        avc->sps[i].data = cur + 2;
        if (avc->sps[i].data + avc->sps[i].bytes > end)
            return -1;

        cur += avc->sps[i].bytes + 2;
    }

    // Picture Parameter Sets
    avc->num_of_picture_parameter_sets = *cur++;
    for (i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (cur + 2 > end)
            return -1;

        avc->pps[i].bytes = (cur[0] << 8) | cur[1];
        avc->pps[i].data = cur + 2;
        if (avc->pps[i].data + avc->pps[i].bytes > end)
            return -1;

        cur += avc->pps[i].bytes + 2;
    }
    
    // Extension may not exist
    if (end - cur >= 4) {
        // Extension
        if (100 == avc->avc_profile_indication || 110 == avc->avc_profile_indication ||
            122 == avc->avc_profile_indication || 144 == avc->avc_profile_indication) {
            avc->chroma_format = cur[0] & 0x03;
            avc->bit_depth_luma_minus8 = cur[1] & 0x07;
            avc->bit_depth_chroma_minus8 = cur[2] & 0x07;

            cur += 3;

            // Sequence Parameter Set Extension NAL Unit
            avc->num_of_sequence_parameter_set_ext_length = *cur++;
            for (i = 0; i < avc->num_of_sequence_parameter_set_ext_length; i++) {
                if (cur + 2 > end)
                    return -1;

                avc->sps_ext[i].bytes = (cur[0] << 8) | cur[1];
                avc->sps_ext[i].data = cur + 2;
                if (avc->sps_ext[i].data + avc->sps_ext[i].bytes > end)
                    return -1;

                cur += avc->sps_ext[i].bytes + 2;
            }
        }
    }

    return (int) (cur - (const uint8_t *) data);
}

int mpeg4_get_avc_decoder_configuration_record(mpeg4_avc_t *avc, uint8_t annexb, uint8_t *data, uint32_t bytes)
{
    int read_size = 0;
    uint32_t i;
    const uint8_t start_code[4] = {0x00, 0x00, 0x00, 0x01};

    if (!data)
        return -1;

    for (i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (read_size + avc->sps[i].bytes + (annexb ? 4 : 0) > (int) bytes)
            return -1;

        if (annexb) {
            memmove(data + read_size, start_code, 4);
            read_size += 4;
        }

        memmove(data + read_size, avc->sps[i].data, avc->sps[i].bytes);
        read_size += avc->sps[i].bytes;
    }

    for (i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (read_size + avc->pps[i].bytes + (annexb ? 4 : 0) > (int) bytes)
            return -1;

        if (annexb) {
            memmove(data + read_size, start_code, 4);
            read_size += 4;
        }

        memmove(data + read_size, avc->pps[i].data, avc->pps[i].bytes);
        read_size += avc->pps[i].bytes;
    }

    return read_size;
}

int mpeg4_avcc_to_annexb(mpeg4_avc_t *avc, const void *in_data, uint32_t in_bytes,
                         uint8_t *out_data, uint32_t out_bytes)
{
    uint32_t nalu_size, read_size, i;
    uint8_t *dst, sps_pps_flag;
    const uint8_t *src, *end;
    const uint8_t start_code[4] = {0x00, 0x00, 0x00, 0x01};

    sps_pps_flag = 0;
    src = in_data;
    end = src + in_bytes;
    dst = out_data;

    // lengthSizeMinusOne: indicates the length in bytes of the NALUnitLength field in an AVC video
    // sample or AVC parameter set sample of the associated stream minus one. For example, a size of
    // one byte is indicated with a value of 0. The value of this field shall be one of 0, 1, or 3
    // corresponding to a length encoded with 1, 2, or 4 bytes, respectively.
    for ( ; src + avc->length_size_minus_one + 1 < end; ) {
        for (i = 0, nalu_size = 0; i < (uint32_t) (avc->length_size_minus_one + 1); i++)
            nalu_size = (nalu_size << 8) | src[i];

        if (src + avc->length_size_minus_one + 1 + nalu_size > end)
            return -22;

        src += avc->length_size_minus_one + 1;

        switch (src[0] & 0x1F) {
            case H264_NAL_SPS:
            case H264_NAL_PPS:
                // Stream may include sps and pps
                sps_pps_flag = 1;
                break;
            case H264_NAL_IDR:
                if (!sps_pps_flag) {
                    read_size = mpeg4_get_avc_decoder_configuration_record(avc, 1, dst,
                                                                           out_bytes - (uint32_t) (dst - out_data));
                    if (read_size <= 0)
                        return -1;

                    dst += read_size;
                    sps_pps_flag = 1;
                }

                break;
            case H264_NAL_AUD:
                // Skip
                break;
        }

        if (dst + 4 + nalu_size > out_data + out_bytes)
            return -22;

        memmove(dst, start_code, 4);
        memmove(dst + 4, src, nalu_size);
        src += nalu_size;
        dst += 4 + nalu_size;
    }

    return (int) (dst - out_data);
}
