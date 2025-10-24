#pragma once 
#include "net/NetCore.h"
#include "conf.h"

class Server:NetCore {
public:
    Server(std::string ip, int port);
    ~Server();
    void run() override;

private:
    std::string _ip;
    int _port;
    std::map<int, infoSocket> socketEvents;   //{дискриптор, флаг ивента}
    std::vector<epoll_event> Events;            //вектор для буфера обмена с ядром
    int epoll_fd;                               //дискриптор epool
    int main_socket;                            //Сеансовый сокет

private:
    int create_socket() override;
    void socket_push_epoll(int socket_fd, int flag) override;
    void sendData() override;
    void close_socket(int fd) override;
    void readData() override;

private:
    int add_client();
    void close_sockets();


};