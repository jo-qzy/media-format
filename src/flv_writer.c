//
// Created by BoringWednesday on 2021/8/9.
//

#include "flv_header.h"
#include "flv_writer.h"

#include <stdlib.h>
#include <string.h>

#define FLV_HEADER_LENGTH       9
#define FLV_TAG_HEADER_LENGTH   11

static int flv_write_header(flv_writer_t *flv, int audio, int video)
{
    flv_vec_t   vec;
    uint8_t     header[FLV_HEADER_LENGTH + 4]; // Include 4 bytes PreviousTagSize

    flv_header_write(audio, video, header, FLV_HEADER_LENGTH);
    flv_tag_size_write(header + FLV_HEADER_LENGTH, 4, 0);

    vec.data = header;
    vec.bytes = FLV_HEADER_LENGTH + 4;

    return flv->on_write(flv->param, &vec, 1);
}

flv_writer_t *flv_writer_create(void* param, int audio, int video, flv_writer_handler_t *handler)
{
    flv_writer_t *flv_writer;

    flv_writer = (flv_writer_t *) calloc(1, sizeof(flv_writer_t));
    if (!flv_writer) {
        return NULL;
    }

    flv_writer->param = param;
    flv_writer->on_write = handler->on_write;

    if (0 != flv_write_header(flv_writer, audio, video)) {
        flv_writer_destroy(flv_writer);

        return NULL;
    }

    return flv_writer;
}

void flv_writer_destroy(flv_writer_t *flv)
{
    free(flv);
}

int flv_writer_input(flv_writer_t *flv, int type, uint32_t timestamp, const void *data, uint32_t bytes)
{
    flv_tag_t   tag;
    flv_vec_t   vec[3];
    uint8_t     tag_buf[FLV_TAG_HEADER_LENGTH + 4];

    memset(&tag, 0, sizeof(tag));

    tag.tag_type = type;
    tag.data_size = bytes;
    tag.timestamp = timestamp;

    flv_tag_write(&tag, tag_buf, FLV_TAG_HEADER_LENGTH);
    flv_tag_size_write(tag_buf + 11, 4, FLV_TAG_HEADER_LENGTH + bytes);

    vec[0].data = tag_buf;
    vec[1].bytes = FLV_TAG_HEADER_LENGTH;
    vec[1].data = data;
    vec[1].bytes = bytes;
    vec[2].data = tag_buf + FLV_TAG_HEADER_LENGTH;
    vec[2].bytes = 4;

    return flv->on_write(flv->param, vec, 3);
}
