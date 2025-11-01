#pragma once 

#include "net/conf.h"
#include <string>
#include <deque>

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <map>
#include <vector>
#include <cstdio>
#include <thread>
#include <unordered_map>
#include <cstddef>

class Server {

public:
    Server(std::string ip, int port);
    ~Server();
    void run();
    void add_send_buf(std::vector<uint8_t> buf);

private:
    struct __attribute__((packed)) Header {
        uint8_t  mode;
        uint16_t size_packet;
    };

    enum RecvMods
    {
        ID = 51,
        STATUS = 70
    };

    enum SendMods
    {
        LICENCE = 60,
        REQUEST_ID = 11
    };

private:
    std::string _ip;
    int _port;
    std::unordered_map<int, infoSocket> socketEvents;   //{дискриптор, флаг ивента}
    std::unordered_map<std::string, std::vector<uint8_t>> send_buf; //id, buf
    std::vector<epoll_event> Events;            //вектор для буфера обмена с ядром
    int epoll_fd;                               //дискриптор epool
    int main_socket;                            //Сеансовый сокет

private:
    int create_socket();
    void socket_push_epoll(int socket_fd, int flag);
    void sendData();
    void close_socket(int fd);
    void readData();
    int add_client();
    void close_sockets();
    int set_nonblock(int fd);
    void head_pars(uint8_t *buf, int size,  infoSocket &info);
    bool decodFNV32a(std::vector<uint8_t> &vec);
    void coderFNV32a(std::vector<uint8_t> &vec);

};