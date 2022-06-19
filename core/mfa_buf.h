//
// Created by quzhenyu on 2022/6/8.
//

#ifndef MFA_BUF_H
#define MFA_BUF_H

#include <cstdint>
#include <functional>

#define MFA_BUF_DEFAULT_SIZE 1024

typedef struct mfa_buf_t
{
    uint8_t *start; // buffer start
    uint8_t *end;   // buffer end

    uint8_t *pos;  // data start
    uint8_t *last; // data end

    uint8_t recycled : 1; // 0: cannot free, 1: can free
} mfa_buf_t;

/// Alloc new buffer
/// @param[in] bytes if not 0, will auto call mfa_buf_resize() to alloc memory
/// @return NULL: error, other: success
mfa_buf_t *mfa_buf_alloc(uint32_t bytes);

/// Free buffer
/// @param[in] buf buffer pointer
void mfa_buf_free(mfa_buf_t *buf);

/// Resize buffer to specific size
/// @param[in] buf buffer pointer
/// @param[in] bytes buffer size, if bytes is 0, it will free memory in buf
/// @return MFA_OK: success, other: error
int mfa_buf_resize(mfa_buf_t *buf, uint32_t bytes);

/// Flush buffer, drop data
/// @param[in] buf buffer pointer
void mfa_buf_flush(mfa_buf_t *buf);

/// Data size in buffer
/// @param[in] buf buffer pointer
/// @return buffer size, MFA_ERROR-buffer invalid
int mfa_buf_size(mfa_buf_t *buf);

/// Free size in buffer can write
/// @param[in] buf buffer pointer
/// @return > 0: buffer size, MFA_ERROR: buffer invalid
int mfa_buf_avaliable_size(mfa_buf_t *buf);

/// Buffer capacity
/// @param[in] buf buffer pointer
/// @return >0: buffer capacity, MFA_ERROR: buffer invalid
int mfa_buf_capacity(mfa_buf_t *buf);

/// Write uint8 to buf
/// @param[in] buf buffer pointer
/// @param[in] val value
/// @return MFA_OK: success, other: error
int mfa_buf_write_uint8(mfa_buf_t *buf, uint8_t val);

/// Write uint16 to buf
/// @param[in] buf buffer pointer
/// @param[in] val value
/// @return MFA_OK: success, other: error
int mfa_buf_write_uint16(mfa_buf_t *buf, uint16_t val);

/// Write uint24 to buf
/// @param[in] buf buffer pointer
/// @param[in] val value
/// @return MFA_OK: success, other: error
int mfa_buf_write_uint24(mfa_buf_t *buf, uint32_t val);

/// Write uint32 to buf
/// @param[in] buf buffer pointer
/// @param[in] val value
/// @return MFA_OK: success, other: error
int mfa_buf_write_uint32(mfa_buf_t *buf, uint32_t val);

/// Write data to buf
/// @param[in] buf buffer pointer
/// @param[in] data data pointer
/// @param[in] bytes data size
/// @return MFA_OK: success, other: error
int mfa_buf_write_data(mfa_buf_t *buf, const uint8_t *data, uint32_t bytes);

#endif // MFA_BUF_H
