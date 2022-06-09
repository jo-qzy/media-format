//
// Created by BoringWednesday on 2021/8/7.
//

#ifndef FLV_H
#define FLV_H

#include <stddef.h>
#include <stdint.h>

typedef struct flv_handler_t
{
    int (*on_write)(void *param, const void *data, uint32_t bytes);

    /// @param[in] param user-defined parameter
    /// @param[in] type audio/video format (see flv_type.h)
    /// @param[in] data flv tag + audio/video data
    /// @param[in] bytes data length
    /// @param[in] timestamp timestamp in milliseconds
    /// @return 0: ok, other: error
    int (*on_frame_write)(void *param, int type, const void *data, uint32_t bytes, uint32_t timestamp);

    /// @param[in] param user-defined parameter
    /// @param[in] codec audio/video format (see flv_type.h)
    /// @param[in] data flv tag + audio/video data
    /// @param[in] bytes data length
    /// @param[in] pts audio/video presentation timestamp
    /// @param[in] dts audio/video decoding timestamp
    /// @param[in] key 1-video keyframe, other-undfined
    /// @return 0: ok, other: error
    int (*on_packet)(void *param, int codec, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts, int key);
} flv_handler_t;

#endif // FLV_H
