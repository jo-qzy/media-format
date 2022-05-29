//
// Created by BoringWednesday on 2021/8/7.
//

#ifndef LIBFLV_FLV_TYPE_H
#define LIBFLV_FLV_TYPE_H

#define FLV_AUDIO   8
#define FLV_VIDEO   9
#define FLV_SCRIPT  18

// FLV Audio Type
#define FLV_AUDIO_PCM               0
#define FLV_AUDIO_ADPCM		        1
#define FLV_AUDIO_MP3		        2
#define FLV_AUDIO_LINEAR_PCM        3 // Little Endian
#define FLV_AUDIO_NELLYMOSER_16K    4
#define FLV_AUDIO_NELLYMOSER_8K     5
#define FLV_AUDIO_NELLYMOSER        6
#define FLV_AUDIO_G711A		        7 // G711 A-law
#define FLV_AUDIO_G711U             8 // G711 mu-law
#define FLV_AUDIO_AAC		        10
#define FLV_AUDIO_SPEEX             11
#define FLV_AUDIO_MP3_8K	        14
#define FLV_DEVICE_SPECIFIC         15

#define FLV_AUDIO_ASC		0x100 // AudioSpecificConfig(ISO-14496-3)

// FLV Video Type
#define FLV_VIDEO_H263		2
#define FLV_VIDEO_SCREEN    3
#define FLV_VIDEO_VP6		4
#define FLV_VIDEO_VP6_ALPHA 5
#define FLV_VIDEO_SCREEN_2  6
#define FLV_VIDEO_H264		7

#define FLV_VIDEO_AVCC		0x200 // AVCDecoderConfigurationRecord(ISO-14496-15)
#define FLV_VIDEO_HVCC		0x201 // HEVCDecoderConfigurationRecord(ISO-14496-15)
#define FLV_VIDEO_AV1C		0x202 // AV1CodecConfigurationRecord(av1-isobmff)

enum flv_frame_type_t
{
    FLV_KEY_FRAME               = 1, // For AVC, a seekable frame
    FLV_INTER_FRAME             = 2, // For AVC, a non-seekable frame
    FLV_DISPOSABLE_INTER_FRAME  = 3,
    FLV_GENERATED_KEY_FRAME     = 4,
    FLV_VIDEO_INFO              = 5,
};

enum flv_packet_type_t
{
    FLV_SEQUENCE_HEADER = 0, // Audio/Video Sequence header (for AAC/AVC)
    FLV_MEDIA_PACKET    = 1, // Audio/Video media packet (AAC: one frame, AVC: one or more NALUs)
};

#endif //LIBFLV_FLV_TYPE_H
