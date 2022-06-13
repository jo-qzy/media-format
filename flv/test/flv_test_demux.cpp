//
// Created by quzhenyu on 2021/12/7.
// Copyright (c) quzhenyu. All rights reserved.
//

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include <flv/flv_demuxer.h>
#include <flv/flv_muxer.h>
#include <flv/flv_reader.h>
#include <flv/flv_type.h>
#include <flv/flv_writer.h>

using namespace std;

int onRead(void *param, void *buf, uint32_t len)
{
    auto stream = (fstream *) param;

    stream->read((char *) buf, len);

    return (int) stream->gcount();
}

int onWrite(void *param, void *buf, uint32_t len)
{
    return (int) fwrite(buf, 1, len, (FILE *) param);
}

int onDemux(void *param, int codec, const void *data, uint32_t bytes, uint32_t pts, uint32_t dts, int flags)
{
    //    auto file_map = (std::map<int, std::fstream *> *) param;
    //
    //    if (file_map->find(codec) == file_map->end())
    //        return 0;
    //
    //    auto file = file_map->at(codec);

    switch (codec) {
        case FLV_VIDEO_H264: {
            // file->write(static_cast<const char *>(data), bytes);
            flv_muxer_h264((flv_muxer_t *) param, data, bytes, pts, dts);
            std::cout << "Demux video (h264): " << bytes << std::endl;
            break;
        }
        case FLV_AUDIO_AAC: {
            // file->write(static_cast<const char *>(data), bytes);
            // std::cout << "Demux audio (aac): " << bytes << std::endl;
            break;
        }
        default:
            break;
    }

    return 0;
    //    return file ? (int) file->gcount() : 0;
}

int onMux(void *param, int type, const void *data, size_t bytes, uint32_t timestamp)
{
    return flv_writer_input((flv_writer_t *) param, type, timestamp, data, bytes);
}

int flv_demux_test(const std::string &flv_path)
{
    int      ret, tag_type;
    uint32_t timestamp, tag_len;

    std::fstream flv_file;
    std::fstream h264_file;
    std::fstream aac_file;

    std::map<int, std::fstream *> file_map;

    flv_file.open(flv_path, std::ios::in | std::ios::binary);
    h264_file.open("test.h264", std::ios::out | std::ios::binary);
    aac_file.open("test.aac", std::ios::out | std::ios::binary);
    file_map[FLV_VIDEO_H264] = &h264_file;
    file_map[FLV_AUDIO_AAC]  = &aac_file;

    flv_reader_t  *reader  = flv_reader_create2(&flv_file, onRead);
    flv_writer_t  *writer  = flv_writer_create("output.flv", 0, 1);
    flv_muxer_t   *muxer   = flv_muxer_create(writer, onMux);
    flv_demuxer_t *demuxer = flv_demuxer_create(muxer, onDemux);

    std::vector<uint8_t> flv_stream;

    flv_stream.resize(2 * 1024 * 1024);

    while (1 ==
           flv_reader_read(reader, &tag_type, &timestamp, &tag_len, flv_stream.data(), (uint32_t) flv_stream.size())) {
        ret = flv_demuxer_input(demuxer, tag_type, flv_stream.data(), tag_len, timestamp);
        if (ret != 0)
            break;
    }

    return 0;
}
