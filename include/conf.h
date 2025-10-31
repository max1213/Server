#pragma once
#include <vector>
#include <cstddef>

struct infoSocket {
    std::vector<uint8_t> buffer_vec;  //буфер
    size_t expected_size_buf;
    bool is_first_packet;            
    unsigned int flag;      //флаг ивента
};