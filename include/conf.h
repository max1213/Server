#pragma once

struct infoSocket {
    std::vector<char> buffer_vec;  //буфер
    size_t expected_size_buf;
    bool is_first_packet;            
    unsigned int flag;      //флаг ивента
};