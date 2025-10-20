#pragma once 
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

class Server {
public:
    Server(std::string ip, int port);
    ~Server();

    void run();

private:
    void sendMachine_id();

    struct infoSocket {
        std::vector<char> buf;  //буфер
        unsigned int flag;      //флаг ивента
    };

private:
    std::string _ip;
    int _port;
    std::map<int, infoSocket> socketEvents;   //{дискриптор, флаг ивента}
    std::vector<epoll_event> Events;            //вектор для буфера обмена с ядром
    int epoll_fd;                               //дискриптор epool
    int main_socket;                            //Сеансовый сокет
    

private:
    int create_socket();
    void socket_push_epoll(int socket_fd, int flag);
    int add_client();
    int set_nonblock(int fd);

};