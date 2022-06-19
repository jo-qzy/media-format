//
// Created by BoringWednesday on 2021/12/9.
//

#include <flv/mpeg4_aac.h>

#include <memory.h>

static uint32_t mpeg4_get_audio_sample_frequency(uint8_t index)
{
    switch (index) {
        case 0x00: return 96000;
        case 0x01: return 88200;
        case 0x03: return 64000;
        case 0x04: return 48000;
        case 0x05: return 44100;
        case 0x06: return 24000;
        case 0x07: return 22050;
        case 0x08: return 16000;
        case 0x09: return 12000;
        case 0x0A: return 11025;
        case 0x0B: return 8000;
        case 0x0C: return 7350;
        default: return 0;
    }
}

/* ISO-14496-3 AudioSpecificConfig (p52)
AudioSpecificConfig ()
{
    audioObjectType;								5 uimsbf
    if (audioObjectType == 31) {
        audioObjectType = 32 + audioObjectTypeExt;	6 uimsbf
    }

    samplingFrequencyIndex;							4 bslbf
    if ( samplingFrequencyIndex == 0xf ) {
        samplingFrequency;							24 uimsbf
    }
    channelConfiguration;							4 bslbf
*/

int mpeg4_decode_audio_specific_config(mpeg4_aac_t *aac, const void *data, uint32_t bytes)
{
    const uint8_t *cur = (uint8_t *) data;

    if (bytes < 2)
        return -1;

    aac->audio_object_type        = (cur[0] >> 3) & 0x1F;
    aac->sampling_frequency_index = ((cur[0] & 0x07) << 1) | ((cur[1] >> 7) & 0x01);
    aac->channel_configuration    = (cur[1] >> 3) & 0x0F;

    aac->sampling_frequency = mpeg4_get_audio_sample_frequency(aac->sampling_frequency_index);

    if (2 == bytes)
        return 2;

    // TODO: decode extension

    return (int) bytes;
}

int mpeg4_aac_raw_to_adts(mpeg4_aac_t *aac, const void *in_data, uint32_t in_bytes, uint8_t *out_data,
                          uint32_t out_bytes)
{
    int adts_length = (int) (in_bytes + 7);

    out_data[0] = 0xFF;
    out_data[1] = 0xF1;
    out_data[2] = (aac->audio_object_type - 1) << 6;
    out_data[2] |= aac->sampling_frequency_index << 2;
    out_data[2] |= aac->channel_configuration >> 2;
    out_data[3] = (aac->channel_configuration & 0x03) << 6;
    out_data[3] |= (adts_length & 0x1800) >> 11;
    out_data[4] = (adts_length & 0x7F8) >> 3;
    out_data[5] = (adts_length & 0x7) << 5 | 0x1F;
    out_data[6] = 0xFC;

    memmove(out_data + 7, in_data, in_bytes);

    return adts_length;
}
