#include "net/NetCore.h"
#include "conf.h"
#include <string>
#include <vector>
#include <deque>

class Client:NetCore {

public:
    Client(std::string ip, int port);
    ~Client();
    void run() override;
    void sendToServer(std::vector<uint8_t> &send_buf);

private:
    std::string _ip;
    int _port;
    epoll_event event; 
    int epoll_fd;                               //дискриптор epool
    std::deque<std::vector<uint8_t>> deque_send_buf;
    infoSocket socketEvent;                     //данные о сокете
    int main_socket;                            //Сеансовый сокет
    
private:
    int create_socket();
    void socket_push_epoll(int socket_fd, int flag) override;
    void sendData() override;
    void readData() override;
    void close_socket(int fd) override;

};