//
// Created by quzhenyu on 2021/12/6.
// Copyright (c) 2021 quzhenyu. All rights reserved.
//

#include <assert.h>
#include <memory.h>

#include <core/mfa_core.h>
#include <flv/h264.h>

static const uint8_t startcode[4] = {0x00, 0x00, 0x00, 0x01};

static const uint8_t *h264_annexb_startcode(const uint8_t *data, uint32_t bytes)
{
    for (uint32_t i = 0; i + 3 < bytes; i++) {
        if (0x00 == data[i] && 0x00 == data[i + 1] && 0x01 == data[i + 2])
            return data + i + 3;
    }

    return NULL;
}

int h264_get_nal_type(h264_config_t *avc, int annexb, const void *data, uint32_t bytes)
{
    if (annexb) {
        const uint8_t *cur;

        cur = h264_annexb_startcode(data, bytes);
        if (!cur || cur - (uint8_t *) data > 4)
            return MFA_ERROR;

        return cur[0] & 0x1F;
    } else {
        if (!avc || avc->length_size_minus_one + 1 > (int) bytes)
            return MFA_ERROR;

        return ((uint8_t *) data)[avc->length_size_minus_one + 1] & 0x1F;
    }
}

int h264_sps_pps_size(h264_config_t *avc, int annexb)
{
    int bytes = 0;

    if (0 == avc->num_of_sequence_parameter_sets || 0 == avc->num_of_picture_parameter_sets)
        return MFA_ERROR;

    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++)
        bytes += (annexb ? 4 : 0) + avc->sps[i].bytes;

    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++)
        bytes += (annexb ? 4 : 0) + avc->pps[i].bytes;

    return bytes;
}

int h264_get_sps_pps(h264_config_t *avc, int annexb, uint8_t *data, uint32_t bytes)
{
    mfa_buf_t buf;
    uint32_t  i;

    buf.start = buf.pos = buf.last = data;
    buf.end                        = data + bytes;

    if (!data)
        return -1;

    for (i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (MFA_OK != mfa_buf_write_uint32(&buf, annexb ? 1 : avc->sps[i].bytes))
            return MFA_ERROR;

        if (MFA_OK != mfa_buf_write_data(&buf, avc->sps[i].data, avc->sps[i].bytes))
            return MFA_ERROR;
    }

    for (i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (MFA_OK != mfa_buf_write_uint32(&buf, annexb ? 1 : avc->pps[i].bytes))
            return MFA_ERROR;

        if (MFA_OK != mfa_buf_write_data(&buf, avc->pps[i].data, avc->pps[i].bytes))
            return MFA_ERROR;
    }

    return mfa_buf_size(&buf);
}

/*
ISO/IEC 14496-15:2017(E)
5.3.3.1 AVC decoder configuration record

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

int h264_decode_extradata(h264_config_t *avc, const void *data, uint32_t bytes)
{
    int            read_size = 0;
    const uint8_t *cur, *end;

    if (bytes < 7)
        return -1;

    cur = data;
    end = cur + bytes;

    assert(cur[0] == 1);

    avc->configuration_version  = cur[0];
    avc->avc_profile_indication = cur[1];
    avc->profile_compatibility  = cur[2];
    avc->avc_level_indication   = cur[3];
    avc->length_size_minus_one  = cur[4] & 0x03;

    cur += 5;

    // SPS: Sequence Parameter Sets
    avc->num_of_sequence_parameter_sets = *cur++ & 0x1F;
    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (cur + 2 > end)
            return -1;

        avc->sps[i].bytes = (cur[0] << 8) | cur[1];
        cur += 2;

        if (cur + avc->sps[i].bytes > end || read_size + avc->sps[i].bytes > sizeof(avc->data))
            return -1;

        memmove(avc->data + read_size, cur, avc->sps[i].bytes);
        avc->sps[i].data = avc->data + read_size;
        avc->data_size += avc->sps[i].bytes;
        cur += avc->sps[i].bytes;
        read_size += avc->sps[i].bytes;
    }

    // PPS: Picture Parameter Sets
    avc->num_of_picture_parameter_sets = *cur++;
    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (cur + 2 > end)
            return -1;

        avc->pps[i].bytes = (cur[0] << 8) | cur[1];
        cur += 2;

        if (cur + avc->pps[i].bytes > end || read_size + avc->sps[i].bytes > sizeof(avc->data))
            return -1;

        memmove(avc->data + read_size, cur, avc->pps[i].bytes);
        avc->pps[i].data = avc->data + read_size;
        avc->data_size += avc->pps[i].bytes;
        cur += avc->pps[i].bytes;
        read_size += avc->pps[i].bytes;
    }

    // Extension may not exist
    if (end - cur >= 4) {
        // Extension
        if (100 == avc->avc_profile_indication || 110 == avc->avc_profile_indication ||
            122 == avc->avc_profile_indication || 144 == avc->avc_profile_indication) {
            avc->chroma_format           = cur[0] & 0x03;
            avc->bit_depth_luma_minus8   = cur[1] & 0x07;
            avc->bit_depth_chroma_minus8 = cur[2] & 0x07;

            cur += 3;

            // Sequence Parameter Set Extension NAL Unit
            avc->num_of_sequence_parameter_set_ext_length = *cur++;
            for (int i = 0; i < avc->num_of_sequence_parameter_set_ext_length; i++) {
                if (cur + 2 > end)
                    return -1;

                avc->sps_ext[i].bytes = (cur[0] << 8) | cur[1];
                avc->sps_ext[i].data  = cur + 2;
                if (avc->sps_ext[i].data + avc->sps_ext[i].bytes > end)
                    return -1;

                cur += avc->sps_ext[i].bytes + 2;
            }
        }
    }

    return (int) (cur - (const uint8_t *) data);
}

int h264_extradata_size(h264_config_t *avc)
{
    int bytes = 9;

    if (0 == avc->num_of_sequence_parameter_sets || 0 == avc->num_of_picture_parameter_sets)
        return MFA_ERROR;

    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++)
        bytes += 2 + avc->sps[i].bytes;

    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++)
        bytes += 2 + avc->pps[i].bytes;

    return bytes;
}

int h264_get_extradata(h264_config_t *avc, uint8_t *data, uint32_t bytes)
{
    uint8_t *cur, *end;

    if (bytes < 7)
        return -1;

    cur = data;
    end = data + bytes;

    cur[0] = avc->configuration_version; // Always 1
    cur[1] = avc->avc_profile_indication;
    cur[2] = avc->profile_compatibility;
    cur[3] = avc->avc_level_indication;
    cur[4] = avc->length_size_minus_one | 0xFC;

    cur += 5;

    *cur++ = avc->num_of_sequence_parameter_sets | 0xE0;
    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (cur + avc->sps[i].bytes + 2 > end)
            return -1;

        cur[0] = (avc->sps[i].bytes >> 8) & 0xFF;
        cur[1] = avc->sps[i].bytes & 0xFF;
        memmove(cur + 2, avc->sps[i].data, avc->sps[i].bytes);

        cur += avc->sps[i].bytes + 2;
    }

    *cur++ = avc->num_of_picture_parameter_sets;
    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (cur + avc->pps[i].bytes + 2 > end)
            return -1;

        cur[0] = (avc->pps[i].bytes >> 8) & 0xFF;
        cur[1] = avc->pps[i].bytes & 0xFF;
        memmove(cur + 2, avc->pps[i].data, avc->pps[i].bytes);

        cur += avc->pps[i].bytes + 2;
    }

    // Extension
    if (100 == avc->avc_profile_indication || 110 == avc->avc_profile_indication ||
        122 == avc->avc_profile_indication || 144 == avc->avc_profile_indication) {
        // TODO: extension
    }

    return (int) (cur - data);
}

int h264_avcc_to_annexb(h264_config_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                        uint32_t out_bytes)
{
    int            nalu_size, read_size, i;
    uint8_t        sps_pps_flag = 0;
    uint8_t       *dst;
    const uint8_t *cur, *end;

    cur = in_data;
    end = cur + in_bytes;
    dst = out_data;

    // lengthSizeMinusOne: indicates the length in bytes of the NALUnitLength field in an AVC video
    // sample or AVC parameter set sample of the associated stream minus one. For example, a size of
    // one byte is indicated with a value of 0. The value of this field shall be one of 0, 1, or 3
    // corresponding to a length encoded with 1, 2, or 4 bytes, respectively.
    for (; cur + avc->length_size_minus_one + 1 < end;) {
        for (i = 0, nalu_size = 0; i < avc->length_size_minus_one + 1; i++)
            nalu_size = (nalu_size << 8) | cur[i];

        if (cur + avc->length_size_minus_one + 1 + nalu_size > end)
            return -22;

        cur += avc->length_size_minus_one + 1;

        switch (cur[0] & 0x1F) {
            case H264_NAL_SPS:
            case H264_NAL_PPS:
                // Stream may include sps and pps
                sps_pps_flag = 1;
                break;
            case H264_NAL_IDR:
                if (!sps_pps_flag) {
                    read_size = h264_get_sps_pps(avc, 1, dst, out_bytes - (uint32_t) (dst - out_data));
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

        // VLC not support 3 bytes startcode, ffplay support
        memmove(dst, startcode, 4);
        memmove(dst + 4, cur, nalu_size);
        cur += nalu_size;
        dst += 4 + nalu_size;
    }

    return (int) (dst - out_data);
}

static uint8_t h264_read_ue(const uint8_t *data, uint32_t bytes, uint8_t *offset)
{
    uint8_t leading_zero_bits = 0;
    uint8_t bit_value;

    for (bit_value = 0; bit_value == 0; leading_zero_bits++) {
        bit_value = data[*offset / 8] >> (7 - *offset % 8);
        *offset   = *offset + 1;

        if ((uint32_t) (*offset / 4) > bytes)
            return -1;
    }

    bit_value = 0;

    for (int i = 0; i < leading_zero_bits; i++) {
        bit_value = (bit_value << 1) | (data[*offset / 8] >> (7 - *offset % 8) & 0x01);
        *offset   = *offset + 1;

        if ((uint32_t) (*offset / 4) > bytes)
            return MFA_ERROR;
    }

    return (1 << leading_zero_bits) - 1 + bit_value;
}

static void h264_remove_sps_pps(h264_config_t *avc, uint8_t *ptr, uint32_t bytes)
{
    memmove(ptr, ptr + bytes, (avc->data + sizeof(avc->data)) - (ptr + bytes));

    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        if (avc->sps[i].data > ptr)
            avc->sps[i].data -= bytes;
    }

    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        if (avc->pps[i].data > ptr)
            avc->pps[i].data -= bytes;
    }

    avc->data_size -= bytes;
}

/// Parse SPS
/// @return MFA_OK: ok, 1: need update, other: error
static int h264_decode_sps(h264_config_t *avc, const uint8_t *data, uint32_t bytes)
{
    // ITU-T H.264 (01/2012), 7.3.2.1.1 Sequence parameter set data syntax
    uint8_t sps_id;
    uint8_t offset;
    int     sps_index = -1;

    if (bytes < 4)
        return -1;

    offset = 4 * 8; // NALu type(1 byte), profile_idc(1 byte), compatibility(1 byte), level_idc(1 byte)
    sps_id = h264_read_ue(data, bytes, &offset);

    for (int i = 0; i < avc->num_of_sequence_parameter_sets; i++) {
        offset = 4 * 8;
        if (h264_read_ue(avc->sps[i].data, avc->sps[i].bytes, &offset) != sps_id)
            continue;

        if (bytes == avc->sps[i].bytes && 0 == memcmp(avc->sps[i].data, data, bytes))
            return 0;

        sps_index = i;
        break;
    }

    if (-1 == sps_index) {
        // new SPS need insert, check buffer capacity
        if (avc->num_of_sequence_parameter_sets == sizeof(avc->sps) / sizeof(avc->sps[0]) ||
            bytes > avc->data_size - sizeof(avc->data))
            return -1;

        sps_index = avc->num_of_sequence_parameter_sets++;
    } else {
        // exist SPS need update, check buffer capacity and remove old SPS
        if (bytes > sizeof(avc->data) - avc->data_size - avc->sps[sps_index].bytes)
            return -1;

        h264_remove_sps_pps(avc, avc->data + (avc->sps[sps_index].data - avc->data), avc->sps[sps_index].bytes);
    }

    memmove(avc->data + avc->data_size, data, bytes);
    avc->sps[sps_index].data  = avc->data + avc->data_size;
    avc->sps[sps_index].bytes = bytes;
    avc->data_size += bytes;

    if (1 == avc->num_of_sequence_parameter_sets) {
        avc->avc_profile_indication = data[1];
        avc->profile_compatibility  = data[2];
        avc->avc_level_indication   = data[3];
    }

    return 1;
}

/// Parse PPS
/// @return MFA_OK: ok, 1: need update, other: error
static int h264_decode_pps(h264_config_t *avc, const uint8_t *data, uint32_t bytes)
{
    // ITU-T H.264 (01/2012), 7.3.2.2 Picture parameter set RBSP syntax
    uint8_t pps_id, sps_id;
    uint8_t offset;
    int     pps_index = -1;

    offset = 8; // NALu type (1 byte)
    pps_id = h264_read_ue(data, bytes, &offset);
    sps_id = h264_read_ue(data, bytes, &offset);

    for (int i = 0; i < avc->num_of_picture_parameter_sets; i++) {
        offset = 8;
        if (h264_read_ue(avc->pps[i].data, avc->pps[i].bytes, &offset) != pps_id ||
            h264_read_ue(avc->pps[i].data, avc->pps[i].bytes, &offset) != sps_id)
            continue;

        if (bytes == avc->pps[i].bytes && 0 == memcmp(avc->pps[i].data, data, bytes))
            return MFA_OK;

        pps_index = i;
        break;
    }

    if (pps_index) {
        // new PPS need insert, check buffer capacity
        if (avc->num_of_picture_parameter_sets == sizeof(avc->pps) / sizeof(avc->pps[0]) ||
            bytes > avc->data_size - sizeof(avc->data))
            return -1;

        pps_index = avc->num_of_picture_parameter_sets++;
    } else {
        // Check buffer capacity
        if (bytes > sizeof(avc->data) - avc->data_size - avc->pps[pps_index].bytes)
            return MFA_ERROR;

        h264_remove_sps_pps(avc, avc->data + (avc->pps[pps_index].data - avc->data), avc->pps[pps_index].bytes);
    }

    memmove(avc->data + avc->data_size, data, bytes);
    avc->pps[pps_index].data  = avc->data + avc->data_size;
    avc->pps[pps_index].bytes = bytes;
    avc->data_size += bytes;

    return 1;
}

int h264_decode_sps_pps(h264_config_t *avc, const void *data, uint32_t bytes)
{
    const uint8_t *cur = data;

    // version always 1, length_size_minus_one always regard as 3
    avc->configuration_version = 1;
    avc->length_size_minus_one = 3;

    switch (cur[0] & 0x1F) {
        case H264_NAL_SPS:
            return h264_decode_sps(avc, cur, bytes);
        case H264_NAL_PPS:
            return h264_decode_pps(avc, cur, bytes);
    }

    return 0;
}

typedef struct h264_bitstream_t
{
    mfa_buf_t     *buf;
    h264_config_t *avc;
    int           *update; // Update sequence header
    int           *vcl;    // 0: seq, 1: key, 2: inter
} h264_bitstream_t;

static int h264_annexb_to_avcc_handler(void *param, const uint8_t *data, uint32_t bytes)
{
    uint8_t           nalu_type = data[0] & 0x1F;
    h264_bitstream_t *bitstream = (h264_bitstream_t *) param;

    if ((H264_NAL_SPS == nalu_type || H264_NAL_PPS == nalu_type) &&
        1 == h264_decode_sps_pps(bitstream->avc, data, bytes)) {
        if (bitstream->update)
            *bitstream->update = 1;
    }

    if (bitstream->vcl && H264_NAL_SLICE <= nalu_type && nalu_type <= H264_NAL_IDR)
        *bitstream->vcl = H264_NAL_IDR == nalu_type ? 1 : 2;

    if (mfa_buf_avaliable_size(bitstream->buf) < (int) (4 + bytes))
        return MFA_ERROR;

    mfa_buf_write_uint32(bitstream->buf, bytes);
    mfa_buf_write_data(bitstream->buf, data, bytes);

    return MFA_OK;
}

int h264_annexb_to_avcc(h264_config_t *avc, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                        uint32_t out_bytes, int *update, int *vcl)
{
    h264_bitstream_t bitstream;
    mfa_buf_t        buf;
    int              ret;

    buf.start = out_data;
    buf.end   = out_data + out_bytes;
    buf.pos   = out_data;
    buf.last  = out_data;

    bitstream.avc    = avc;
    bitstream.buf    = &buf;
    bitstream.update = update;
    bitstream.vcl    = vcl;

    ret = h264_annexb_bitstream(&bitstream, h264_annexb_to_avcc_handler, in_data, in_bytes);
    if (MFA_OK != ret)
        return MFA_ERROR;

    return mfa_buf_size(&buf);
}

int h264_annexb_bitstream(void *param, h264_bitstream_handler handler, const void *data, uint32_t bytes)
{
    int nalu_size;

    const uint8_t *cur, *next_nalu, *end;

    if (!handler)
        return MFA_ERROR;

    end = (const uint8_t *) data + bytes;

    for (cur = h264_annexb_startcode(data, bytes); cur; cur = next_nalu) {
        next_nalu = h264_annexb_startcode(cur, bytes - (uint32_t) (cur - (uint8_t *) data));

        // start code may have 4 bytes
        if (next_nalu)
            nalu_size = (int) (next_nalu - cur) - (0x00 == *(next_nalu - 4) ? 4 : 3);
        else
            nalu_size = (int) (end - cur);

        handler(param, cur, nalu_size);
    }

    return MFA_OK;
}
