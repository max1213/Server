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
#include <thread>
#include <cstddef>

class NetCore {
protected:
    int set_nonblock(int fd);

protected:
    virtual void run() = 0;
    virtual int create_socket() = 0;
    virtual void socket_push_epoll(int socket_fd, int flag) = 0;
    virtual void sendData() = 0;
    virtual void readData() = 0;
    virtual void close_socket(int fd) = 0;

};