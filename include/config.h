#pragma once
#include <vector>
#include <cstddef>

struct infoSocket {
    std::vector<char> buffer_vec;  //буфер
    size_t expected_size_buf;
    bool is_first_packet;     
    bool json_mode;          
    unsigned int flag;      //флаг ивента
};