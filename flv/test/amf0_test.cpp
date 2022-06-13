//
// Created by BoringWednesday on 2021/8/3.
//

#include <flv/amf0.h>

#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

#define AMF_OBJECT_ITEM_VALUE(item, amf_type, amf_name, amf_value, amf_size) \
    { (item).type = amf_type; (item).name = amf_name; (item).value = amf_value; (item).size = amf_size; }

struct rtmp_result_t
{
    char onStatus[64];
    double number;
    char level[64];
    char code[64];
    char description[256];
};

int amf0_test(const std::string &amf0_file)
{
    rtmp_result_t result = {};

    amf_object_item_t items[4];
    amf_object_item_t object_item[3];

    uint8_t out_amf_buffer[1024], in_amf_buffer[1024];
    uint8_t *ptr, *end;

    fstream in_amf_stream(amf0_file, ios::in);

    // Read example AMF stream
    in_amf_stream.read(reinterpret_cast<char *>(in_amf_buffer), 1024);

    AMF_OBJECT_ITEM_VALUE(items[0], AMF0_STRING, nullptr, result.onStatus, sizeof(result.onStatus));
    AMF_OBJECT_ITEM_VALUE(items[1], AMF0_NUMBER, nullptr, &result.number, 8);
    AMF_OBJECT_ITEM_VALUE(items[2], AMF0_NULL, nullptr, nullptr, 0);
    AMF_OBJECT_ITEM_VALUE(items[3], AMF0_OBJECT, nullptr, object_item, sizeof(object_item) / sizeof(object_item[0]));
    AMF_OBJECT_ITEM_VALUE(object_item[0], AMF0_STRING, "level", result.level, sizeof(result.level));
    AMF_OBJECT_ITEM_VALUE(object_item[1], AMF0_STRING, "code", result.code, sizeof(result.code));
    AMF_OBJECT_ITEM_VALUE(object_item[2], AMF0_STRING, "description", result.description, sizeof(result.description));

    // Test AMF read
    ptr = in_amf_buffer;
    end = in_amf_buffer + 1024;
    if (!amf0_read(ptr, end, items, sizeof(items) / sizeof(items[0]))) {
        cout << "Test read amf0 error." << endl;

        return -1;
    }

    // Test AMF write
    ptr = out_amf_buffer;
    end = out_amf_buffer + 1024;
    ptr = amf0_write(ptr, end, items, sizeof(items) / sizeof(items[0]));
    if (!ptr) {
        cout << "Test write amf0 error." << endl;

        return -1;
    }

    int ret = memcmp(in_amf_buffer, out_amf_buffer, ptr - out_amf_buffer);
    if (ret != 0) {
        cout << "Generate amf0 stream different from input amf0 stream." << endl;

        return -1;
    }

    cout << "Test amf0 success." << endl;

    return 0;
}
