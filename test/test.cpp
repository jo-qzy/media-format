#include <iostream>

int amf0_test(const std::string &amf0_file);
int flv_demux_test(const std::string &flv_path);

int main()
{
    amf0_test("res/flv/");
    flv_demux_test("res/LetHerGo.flv");

    return 0;
}