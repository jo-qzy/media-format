//
// Created by BoringWednesday on 2021/8/9.
//

#ifndef FLV_WRITER_H
#define FLV_WRITER_H

#include <stddef.h>
#include <stdint.h>

#include <core/mfa_core.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct flv_writer_t flv_writer_t;

/// @param[in] param User parameters
/// @param[in] vec flv data vector
/// @param[in] len flv data length
/// @return MFA_OK: ok, other: error
typedef int (*flv_writer_handler)(void *param, vec_t *vec, uint32_t len);

flv_writer_t *flv_writer_create(const char *filename, int audio, int video);

flv_writer_t *flv_writer_create2(void *param, int audio, int video, flv_writer_handler handler);

void flv_writer_free(flv_writer_t *flv);

int flv_writer_input(flv_writer_t *flv, int type, uint32_t timestamp, const void *data, uint32_t bytes);

#if defined(__cplusplus)
}
#endif

#endif // FLV_WRITER_H
