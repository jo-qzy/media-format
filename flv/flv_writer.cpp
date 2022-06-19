//
// Created by BoringWednesday on 2021/8/9.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <core/mfa_core.h>
#include <flv/flv_header.h>
#include <flv/flv_writer.h>

#define FLV_HEADER_LENGTH     9
#define FLV_TAG_HEADER_LENGTH 11

typedef struct flv_writer_t
{
    void              *param;
    flv_writer_handler write;
} flv_writer_t;

static int write_header(flv_writer_t *flv, int audio, int video)
{
    vec_t   vec;
    uint8_t header[FLV_HEADER_LENGTH + 4]; // Include 4 bytes PreviousTagSize

    flv_header_write(audio, video, header, FLV_HEADER_LENGTH);
    flv_tag_size_write(header + FLV_HEADER_LENGTH, 4, 0);

    vec.data  = header;
    vec.bytes = FLV_HEADER_LENGTH + 4;

    return flv->write(flv->param, &vec, 1);
}

static int flv_writer_default_write(void *param, vec_t *vec, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (fwrite(vec[i].data, 1, vec[i].bytes, param) != vec[i].bytes)
            return ferror(param);
    }

    return 0;
}

flv_writer_t *flv_writer_create(const char *filename, int audio, int video)
{
    if (!filename)
        return NULL;

    FILE *file = fopen(filename, "wb+");
    if (!file)
        return NULL;

    return flv_writer_create2(file, audio, video, flv_writer_default_write);
}

flv_writer_t *flv_writer_create2(void *param, int audio, int video, flv_writer_handler handler)
{
    flv_writer_t *flv_writer;

    flv_writer = (flv_writer_t *) calloc(1, sizeof(flv_writer_t));
    if (!flv_writer) {
        return NULL;
    }

    flv_writer->param = param;
    flv_writer->write = handler;

    if (0 != write_header(flv_writer, audio, video)) {
        flv_writer_free(flv_writer);
        return NULL;
    }

    return flv_writer;
}

void flv_writer_free(flv_writer_t *flv)
{
    free(flv);
}

int flv_writer_input(flv_writer_t *flv, int type, uint32_t timestamp, const void *data, uint32_t bytes)
{
    flv_tag_t tag;
    vec_t     vec[3];
    uint8_t   tag_buf[FLV_TAG_HEADER_LENGTH + 4];

    memset(&tag, 0, sizeof(tag));

    tag.tag_type  = type;
    tag.data_size = bytes;
    tag.timestamp = timestamp;

    flv_tag_write(&tag, tag_buf, FLV_TAG_HEADER_LENGTH);
    flv_tag_size_write(tag_buf + 11, 4, FLV_TAG_HEADER_LENGTH + bytes);

    vec[0].data  = tag_buf;
    vec[0].bytes = FLV_TAG_HEADER_LENGTH;
    vec[1].data  = data;
    vec[1].bytes = bytes;
    vec[2].data  = tag_buf + FLV_TAG_HEADER_LENGTH;
    vec[2].bytes = 4;

    return flv->write(flv->param, vec, 3);
}
