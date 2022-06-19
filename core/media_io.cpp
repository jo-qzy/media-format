//
// Created by quzhenyu on 2022/6/19.
//

#include <cstdio>

#include "media_io.h"
#include "mfa_core.h"

#define MEDIA_IO_BUFFER_SIZE 32768

MediaIO::MediaIO()
    : m_mode(MediaIOMode::MEDIA_IO_NONE)
    , m_param(nullptr)
    , m_buffer(nullptr)
    , m_buffer_size(0)
    , m_buf_pos(nullptr)
    , m_buf_end(nullptr)
    , m_eof(false)
{
    m_buffer = new (std::nothrow) uint8_t[MEDIA_IO_BUFFER_SIZE];
    if (!m_buffer) {
        MLOG(ERROR, "alloc buffer error");
    }

    m_buffer_size = MEDIA_IO_BUFFER_SIZE;
    m_buf_pos = m_buf_end = m_buffer;
}

MediaIO::~MediaIO()
{
    delete[] m_buffer;
}

bool MediaIO::Init(const std::string &filename, bool write_flag)
{
    if (filename.empty())
        return false;

    if (write_flag) {
        m_mode  = MediaIOMode::MEDIA_IO_WRITE;
        m_param = fopen(filename.c_str(), "wb");
    } else {
        m_mode  = MediaIOMode::MEDIA_IO_READ;
        m_param = fopen(filename.c_str(), "rb");
    }

    if (!m_param)
        return false;

    return true;
}

bool MediaIO::Init(const std::function<int(void *, uint8_t *, int)> &func, bool write_flag)
{
    if (!func)
        return false;

    if (write_flag) {
        m_mode       = MediaIOMode::MEDIA_IO_WRITE;
        m_write_func = func;
    } else {
        m_mode      = MediaIOMode::MEDIA_IO_READ;
        m_read_func = func;
    }

    return true;
}

int MediaIO::Size()
{
    return m_buf_end - m_buf_pos;
}

void MediaIO::Skip(int size)
{
    if (m_buf_pos + size < m_buf_end)
        m_buf_pos += size;
    else
        m_buf_pos = m_buf_end = m_buffer;
}

void MediaIO::Flush()
{
    if (MediaIOMode::MEDIA_IO_WRITE == m_mode)
        WritePacket();

    m_buf_pos = m_buf_end = m_buffer;
}

bool MediaIO::Eof()
{
    return m_eof;
}

int MediaIO::ReadPacket(int size)
{
    if (!check_buf(size))
        return MFA_IO_ERROR;

    int read_size;

    if (m_read_func)
        read_size = m_read_func(m_param, m_buf_end, size);
    else
        read_size = (int) fread(m_buf_end, 1, size, (FILE *) m_param);

    if (read_size < 0)
        return MFA_IO_ERROR;
    else if (0 == read_size) {
        m_eof = true;
        return MFA_EOF;
    }

    m_buf_end += size;

    return read_size;
}

int MediaIO::WritePacket()
{
    auto data_size = Size();
    if (0 == data_size)
        return 0;

    int write_size;

    if (m_write_func)
        write_size = m_write_func(m_param, m_buf_pos, data_size);
    else
        write_size = (int) fwrite(m_buffer, 1, data_size, (FILE *) m_param);

    if (write_size != data_size)
        return MFA_IO_ERROR;

    m_buf_pos = m_buf_end = m_buffer;

    return write_size;
}

int MediaIO::Read8(uint8_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 1, false);
    *val = (uint8_t) tmp_val;

    return ret;
}

int MediaIO::ReadL16(uint16_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 2, false);
    *val = (uint16_t) tmp_val;

    return ret;
}

int MediaIO::ReadB16(uint16_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 2, true);
    *val = (uint16_t) tmp_val;

    return ret;
}

int MediaIO::ReadL24(uint32_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 3, false);
    *val = (uint32_t) tmp_val;

    return ret;
}

int MediaIO::ReadB24(uint32_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 3, true);
    *val = (uint32_t) tmp_val;

    return ret;
}

int MediaIO::ReadL32(uint32_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 4, false);
    *val = (uint32_t) tmp_val;

    return ret;
}

int MediaIO::ReadB32(uint32_t *val)
{
    uint64_t tmp_val;
    int      ret;

    ret  = read_bytes(tmp_val, 4, true);
    *val = (uint32_t) tmp_val;

    return ret;
}

int MediaIO::ReadL64(uint64_t *val)
{
    return read_bytes(*val, 8, false);
}

int MediaIO::ReadB64(uint64_t *val)
{
    return read_bytes(*val, 8, true);
}

int MediaIO::ReadData(uint8_t *data, int size)
{
    if (Size() < size) {
        int ret = ReadPacket(size);
        if (MFA_IO_ERROR == ret)
            return ret;
    }

    int data_size = Size();
    int read_size = size < data_size ? size : data_size;

    memmove(data, m_buf_pos, read_size);

    return read_size;
}

int MediaIO::Write8(uint8_t val)
{
    return write_bytes(val, 1, true);
}

int MediaIO::WriteL16(uint16_t val)
{
    return write_bytes(val, 2, false);
}

int MediaIO::WriteB16(uint16_t val)
{
    return write_bytes(val, 2, true);
}

int MediaIO::WriteL24(uint32_t val)
{
    return write_bytes(val, 3, false);
}

int MediaIO::WriteB24(uint32_t val)
{
    return write_bytes(val, 3, true);
}

int MediaIO::WriteL32(uint32_t val)
{
    return write_bytes(val, 4, false);
}

int MediaIO::WriteB32(uint32_t val)
{
    return write_bytes(val, 4, true);
}

int MediaIO::WriteL64(uint64_t val)
{
    return write_bytes(val, 8, false);
}

int MediaIO::WriteB64(uint64_t val)
{
    return write_bytes(val, 8, true);
}

int MediaIO::WriteData(const uint8_t *data, int size)
{
    if (!check_buf(size))
        return MFA_IO_ERROR;

    memmove(m_buf_end, data, size);
    m_buf_end += size;

    return MFA_OK;
}

bool MediaIO::check_buf(int size)
{
    int data_size = Size();

    if (m_buffer_size - data_size > size) {
        if (m_buffer + m_buffer_size - m_buf_end < size)
            memmove(m_buffer, m_buf_pos, data_size);

        return true;
    }

    if (m_buffer_size == 0)
        m_buffer_size = MEDIA_IO_BUFFER_SIZE;

    while (m_buffer_size - data_size < size)
        m_buffer_size *= 2;

    auto tmp_buf = new (std::nothrow) uint8_t[m_buffer_size];
    if (!tmp_buf) {
        MLOG(ERROR, "alloc buffer error");
        return false;
    }

    memmove(tmp_buf, m_buf_pos, data_size);

    m_buffer = m_buf_pos = m_buf_end = tmp_buf;

    return true;
}

int MediaIO::read_bytes(uint64_t &val, int bytes, bool big_endian)
{
    if (Size() < bytes) {
        int ret = ReadPacket(bytes);
        if (bytes != ret)
            return ret;
    }

    val = 0;

    if (big_endian) {
        for (int i = 0; i < bytes; i++)
            val = val << 8 | m_buf_pos[i];
    } else {
        for (int i = 0; i < bytes; i++)
            val = val << 8 | m_buf_pos[bytes - i - 1];
    }

    m_buf_pos += bytes;
    if (m_buf_pos == m_buf_end)
        m_buf_pos = m_buf_end = m_buffer;

    return MFA_OK;
}

int MediaIO::write_bytes(uint64_t val, int bytes, bool big_endian)
{
    if (!check_buf(bytes))
        return MFA_IO_ERROR;

    if (big_endian) {
        for (int i = 0; i < bytes; i++)
            m_buf_end[i] = (uint8_t) (val >> (8 * (bytes - i - 1))) & 0xFF;
    } else {
        for (int i = 0; i < bytes; i++)
            m_buf_end[bytes - i - 1] = (uint8_t) (val >> (8 * (bytes - i - 1))) & 0xFF;
    }

    m_buf_end += bytes;

    return MFA_OK;
}
