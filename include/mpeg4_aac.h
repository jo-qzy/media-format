//
// Created by BoringWednesday on 2021/12/9.
//

#ifndef LIBFLV_MPEG4_AAC_H
#define LIBFLV_MPEG4_AAC_H

#include <stdint.h>

typedef struct mpeg4_aac_t
{
    uint8_t audio_object_type;
    // uint8_t audio_object_type_ext;

    uint8_t  sampling_frequency_index;
    uint32_t sampling_frequency;

    uint8_t channel_configuration;
} mpeg4_aac_t;

int mpeg4_decode_audio_specific_config(mpeg4_aac_t *aac, const void *data, uint32_t bytes);

int mpeg4_aac_raw_to_adts(mpeg4_aac_t *aac, const void *in_data, uint32_t in_bytes,
                          uint8_t *out_data, uint32_t out_bytes);

#endif // LIBFLV_MPEG4_AAC_H
