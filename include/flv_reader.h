//
// Created by quzhenyu on 2021/12/3.
//

#ifndef LIBFLV_FLV_READER_H
#define LIBFLV_FLV_READER_H

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct flv_reader_t flv_reader_t;

/// Read require flv data length
/// @param[in] param user-defined parameter
/// @param[out] buf flv data buffer
/// @param[in] len require length of flv data
/// @return 0-success, other-error
typedef int (*flv_reader_handler)(void *param, void *buf, uint32_t len);

/// @param[in] file the flv file name need to be read, can be NULL
/// @param[in] handler read flv use handler, not use default , can be NULL
/// @param[in] param user-defined parameter
/// @return NULL-error, other-success
flv_reader_t *flv_reader_create(const char *file, flv_reader_handler handler, void *param);

void flv_reader_destroy(flv_reader_t *reader);

/// Read flv data, return one flv packet
/// @param[out] tagtype 8-audio, 9-video, 18-script data
/// @param[out] timestamp FLV timestamp
/// @param[out] taglen flv tag length(0 is ok but should be silently discard)
/// @param[out] buffer FLV stream
/// @param[in] bytes buffer size
/// @return 1-got a packet, 0-EOF, other-error
int flv_reader_read(flv_reader_t *reader, int *tag_type, uint32_t *timestamp, uint32_t *taglen,
                    void *buffer, uint32_t bytes);

#if defined(__cplusplus)
}
#endif

#endif //LIBFLV_FLV_READER_H
