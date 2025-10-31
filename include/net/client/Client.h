
#pragma once

#include "net/client/interface.h"
#include "conf.h"
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
#include <cstddef>
extern "C" {
#include "picohttpparser/picohttpparser.h"
}


class Client{

private:
    struct HttpHead {
        const char *method;           //Тут будет храниться тип например GET
        size_t method_len;
        const char *path;             //Тут хранится путь запроса
        size_t path_len ;
        int minor_version;                      //Версия HTTP
        size_t prevbuflen;                      //Длина предыдущего буфера если первый раз то ставь 0
        int pret;
        struct phr_header headers[100];   //Массив структур для хранения заголовков (name, value, name_len, value_len).
        size_t num_headers;                     //Вход: размер массива headers; выход: сколько реально заголовков было найдено.
    };

    struct HttpResponse { 
        int minor_version;
        int status;
        const char *msg;
        size_t msg_len;
        struct phr_header headers[100];
        size_t num_headers;
        size_t last_len;
        int pret;
    };

public:
    Client(std::string ip, int port);
    ~Client();
    void sendData();
    void sendToClinet(std::vector<uint8_t> &send_buf);
    void recvData();
    int onDataReady(std::vector<uint8_t> &out);

private:
    std::string _ip;
    int _port;
    epoll_event event; 
    int epoll_fd;                               //дискриптор epool
    std::deque<std::vector<uint8_t>> deque_send_buf;
    std::deque<std::vector<uint8_t>> deque_recv_buf;
    infoSocket socketEvent;                     //данные о сокете
    int main_socket;                            //Сеансовый сокет
    HttpResponse head;

private:
    
    int set_nonblock(int fd);
    int create_socket();
    void socket_push_epoll(int socket_fd, int flag);
    void close_socket(int fd);
    void http_pars(uint8_t *sys_buffer, int recv_size);
};