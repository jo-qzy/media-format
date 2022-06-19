//
// Created by quzhenyu on 2022/6/19.
//

#ifndef MFA_MEDIA_IO_H
#define MFA_MEDIA_IO_H

#include <cstdint>
#include <functional>
#include <string>

enum class MediaIOMode
{
    MEDIA_IO_NONE  = -1,
    MEDIA_IO_READ  = 0,
    MEDIA_IO_WRITE = 1,
};

class MediaIO
{
public:
    MediaIO();
    ~MediaIO();

    bool Init(const std::string &filename, bool write_flag);
    bool Init(const std::function<int(void *, uint8_t *, int)>& func, bool write_flag);

    int  Size();
    void Skip(int size);
    void Flush();
    bool Eof();

    int ReadPacket(int size);
    int WritePacket();

    int Read8(uint8_t *val);
    int ReadL16(uint16_t *val);
    int ReadB16(uint16_t *val);
    int ReadL24(uint32_t *val);
    int ReadB24(uint32_t *val);
    int ReadL32(uint32_t *val);
    int ReadB32(uint32_t *val);
    int ReadL64(uint64_t *val);
    int ReadB64(uint64_t *val);
    int ReadData(uint8_t *data, int size);

    int Write8(uint8_t val);
    int WriteL16(uint16_t val);
    int WriteB16(uint16_t val);
    int WriteL24(uint32_t val);
    int WriteB24(uint32_t val);
    int WriteL32(uint32_t val);
    int WriteB32(uint32_t val);
    int WriteL64(uint64_t val);
    int WriteB64(uint64_t val);
    int WriteData(const uint8_t *data, int size);

private:
    bool check_buf(int size);
    int read_bytes(uint64_t &val, int bytes, bool big_endian);
    int write_bytes(uint64_t val, int bytes, bool big_endian);

private:
    MediaIOMode m_mode;
    void       *m_param;
    uint8_t    *m_buffer;
    int         m_buffer_size;
    uint8_t    *m_buf_pos;
    uint8_t    *m_buf_end;
    bool        m_eof;

    std::function<int(void *, uint8_t *, int)> m_read_func;
    std::function<int(void *, uint8_t *, int)> m_write_func;
};

#endif // MFA_MEDIA_IO_H
