//
// Created by quzhenyu on 2022/6/8.
//

#include <stdlib.h>
#include <string.h>

#include <core/mfa_core.h>

mfa_buf_t *mfa_buf_alloc(uint32_t bytes)
{
    mfa_buf_t *buf = (mfa_buf_t *) malloc(sizeof(mfa_buf_t));
    if (!buf)
        return NULL;

    memset(buf, 0, sizeof(mfa_buf_t));
    buf->recycled = (bytes != 0);

    if (MFA_OK != mfa_buf_resize(buf, bytes)) {
        free(buf);
        return NULL;
    }

    return buf;
}

void mfa_buf_free(mfa_buf_t *buf)
{
    if (!buf)
        return;

    if (buf->start)
        free(buf->start);

    free(buf);
}

int mfa_buf_resize(mfa_buf_t *buf, uint32_t bytes)
{
    if (!buf || mfa_buf_capacity(buf) > (int) bytes || (buf->start && 0 == buf->recycled))
        return MFA_ERROR;

    long long pos_index  = buf->pos - buf->start;
    long long last_index = buf->last - buf->start;
    void     *ptr;

    if (bytes != 0) {
        ptr = realloc(buf->start, bytes);
        if (!ptr)
            return MFA_ERROR;

        buf->recycled = 1;
        buf->start    = (uint8_t *) ptr;
        buf->end      = buf->start + bytes;
        buf->pos      = buf->start + pos_index;
        buf->last     = buf->start + last_index;
    } else {
        free(buf->start);
        memset(buf, 0, sizeof(mfa_buf_t));
    }

    return MFA_OK;
}

void mfa_buf_flush(mfa_buf_t *buf)
{
    if (!buf)
        return;

    buf->pos = buf->last = buf->start;
}

int mfa_buf_size(mfa_buf_t *buf)
{
    return buf ? (int) (buf->last - buf->pos) : -1;
}

int mfa_buf_avaliable_size(mfa_buf_t *buf)
{
    return buf ? (int) (buf->end - buf->last) : -1;
}

int mfa_buf_capacity(mfa_buf_t *buf)
{
    return buf ? (int) (buf->end - buf->start) : -1;
}

int mfa_buf_write_uint8(mfa_buf_t *buf, uint8_t val)
{
    if (!buf || buf->last + 1 > buf->end)
        return MFA_ERROR;

    buf->last[0] = val;

    buf->last += 1;

    return MFA_OK;
}

int mfa_buf_write_uint16(mfa_buf_t *buf, uint16_t val)
{
    if (!buf || buf->last + 2 > buf->end)
        return MFA_ERROR;

    buf->last[0] = (uint8_t) ((val >> 8) & 0xFF);
    buf->last[1] = (uint8_t) (val & 0xFF);

    buf->last += 2;

    return MFA_OK;
}

int mfa_buf_write_uint24(mfa_buf_t *buf, uint32_t val)
{
    if (!buf || buf->last + 3 > buf->end)
        return MFA_ERROR;

    buf->last[0] = (uint8_t) ((val >> 16) & 0xFF);
    buf->last[1] = (uint8_t) ((val >> 8) & 0xFF);
    buf->last[2] = (uint8_t) (val & 0xFF);

    buf->last += 3;

    return MFA_OK;
}

int mfa_buf_write_uint32(mfa_buf_t *buf, uint32_t val)
{
    if (!buf || buf->last + 4 > buf->end)
        return MFA_ERROR;

    buf->last[0] = (uint8_t) ((val >> 24) & 0xFF);
    buf->last[1] = (uint8_t) ((val >> 16) & 0xFF);
    buf->last[2] = (uint8_t) ((val >> 8) & 0xFF);
    buf->last[3] = (uint8_t) (val & 0xFF);

    buf->last += 4;

    return MFA_OK;
}

int mfa_buf_write_data(mfa_buf_t *buf, const uint8_t *data, uint32_t bytes)
{
    if (!buf || buf->last + bytes > buf->end)
        return MFA_ERROR;

    memmove(buf->last, data, bytes);

    buf->last += bytes;

    return MFA_OK;
}
