#include "net/client/Client.h"

#include "nlohmann/json.hpp"

extern "C" {
#include "picohttpparser/picohttpparser.h"
}


const int count_event = 1;

Client::Client(std::string ip, int port) : 
_ip(ip), _port(port), event(), socketEvent() {
    head = { 0 };
    socketEvent.is_first_packet = false;
    socketEvent.expected_size_buf = 0;
    main_socket = create_socket();
    if (main_socket < 0) {
        std::cerr << "[ERROR] Dont open socket.";
        return;
    }

    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(main_socket);
        return;
}

    event.data.fd = main_socket;
    event.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, main_socket, &event);

}

Client::~Client() {
    close_socket(main_socket);
}

void Client::sendToClinet(std::vector<uint8_t> &send_buf) {
    deque_send_buf.push_back(send_buf);
}

int Client::create_socket() {
    int sok = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sok == -1) {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(_port);

    if (inet_pton(AF_INET, _ip.c_str(), &sa.sin_addr) != 1) {
        perror("inet_pton");
        return -1;
    }
    set_nonblock(sok);
    if (connect(sok, (sockaddr*)&sa, sizeof(sa)) > 0) {
        std::cerr << "❌ connect() failed: " << strerror(errno) << "\n";
        close(sok);
        return -1;
    }
   
    
    return sok;
}

void Client::socket_push_epoll(int socket_fd, int flag = EPOLL_CTL_ADD) {
    epoll_event event;
    event.data.fd = socket_fd;
    event.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, flag, socket_fd, &event) == -1) {
        perror("epoll_ctl");
    }

}

void Client::sendData() {
    //send(int sockfd, const void *buf, size_t len, int flags);
    if(!deque_send_buf.empty()) {
        int n = epoll_wait(epoll_fd, &event, 1, 0);

        if (n < 0) {
            perror("epoll_wait");
            return;
        }

        if (event.events & EPOLLERR) {
            printf("Ошибка на сокете %d\n", main_socket);
            close_socket(main_socket);
            return;
        }

        if (event.events & EPOLLHUP) {
            printf("Сокет %d был закрыт другой стороной\n", main_socket);
            close_socket(main_socket);
            return;
        }
        
        if (n > 0) {
            if (event.events & EPOLLOUT) {
                auto buf = deque_send_buf.front();

                n = send(main_socket, buf.data(), buf.size(), 0);
                std::string s (buf.begin(), buf.end());
                if ( n < 0) {
                    std::cout << "[ERROR] send: " << s << std::endl;
                    return;
                } else if (n == 0) {
                    std::cout << "[ERROR] send: dont connect." << std::endl;
                    close_socket(main_socket);
                    return;
                }
                std::cout << "OK send: " << s << std::endl;
                deque_send_buf.pop_front();
            }
        }

    }

}

void Client::recvData() {
    int N = epoll_wait(epoll_fd, &event, count_event, -1);

    if (N < 0) {
        perror("epoll_wait");
        return;
    }

    int fd = event.data.fd;

    if (event.events & EPOLLERR) {
        printf("Ошибка на сокете %d\n", fd);
        close_socket(fd);
        return;
    }

    if (event.events & EPOLLHUP) {
        printf("Сокет %d был закрыт другой стороной\n", fd);
        close_socket(fd);
        return;
    }

    if (event.events & EPOLLIN) {
 
        auto &info_sock = socketEvent;
        auto &buffer = info_sock.buffer_vec;
        
        uint8_t tmp[4096];
        int n = recv(fd, tmp, sizeof(tmp), 0);

        if (n > 0) {
            // Данные пришли
            std::cout << "📥 Received " << n << " bytes from " << fd << std::endl;
            http_pars(tmp, n);   
            

        } else if (n == 0) {
            // Клиент реально закрыл соединение
            close_socket(fd);
            return;

        } else if (n < 0) { // n < 0
            if (errno == EAGAIN || errno == EWOULDBLOCK) {

                return;
                // Данных нет, но сокет жив
                // Просто ждем следующего события EPOLLIN
            } else {
                // Реальная ошибка
                perror("recv");
                close_socket(fd);
                return;
            }
        }
            
    }

}

void Client::close_socket(int fd) {
    std::cout << "❌ Client " << fd << " disconnected\n";
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
}

void Client::http_pars(uint8_t *sys_buffer, int recv_size) {
    auto &buffer = socketEvent.buffer_vec;

    // std::string s(sys_buffer, sys_buffer + recv_size);
    // std::cout << s << std::endl;
   
    if (!socketEvent.is_first_packet) {
        head.num_headers = sizeof(head.headers) / sizeof(head.headers[0]);
        head.pret = phr_parse_response(
        (char*)sys_buffer,
        recv_size,
        &head.minor_version,
        &head.status,
        &head.msg,
        &head.msg_len,
        head.headers,
        &head.num_headers,
        0);
    
    
        if (head.pret > 0) {
            for (int i = 0; i != head.num_headers; ++i) {

                std::string name(head.headers[i].name, head.headers[i].name_len);
                std::string value(head.headers[i].value, head.headers[i].value_len);

                std::cout << name << std::endl;
                if (name == "Content-Length")  {
    
                    socketEvent.expected_size_buf = atoi(value.c_str());
                    buffer.insert(buffer.end(), sys_buffer + head.pret, sys_buffer + recv_size);  //от шапки до конца
                    socketEvent.is_first_packet = 1;
                    break;
                }
                
            }
            return;
        } else if (head.pret == -1) {
            std::cout << "[ERROR] pars bufer socket." << std::endl;
            return; //ошибка парсинга

        } else if (head.pret == -2) { 
            std::cout << "еще не пришел\n" << std::endl;
            return; // еще не пришел вест пакет 
        }
        
    }
    
    
   
    if (socketEvent.expected_size_buf > buffer.size()) {
        buffer.insert(buffer.end(), sys_buffer, sys_buffer + recv_size);  //от шапки до конца
        
    }
    

    if (socketEvent.expected_size_buf == buffer.size()) {
        deque_recv_buf.push_back(buffer);

        buffer.clear();
        buffer.shrink_to_fit();
        socketEvent.expected_size_buf = 0;
        socketEvent.is_first_packet = 0;
        
        return; /* successfully parsed the request */
    }
}

int Client::onDataReady(std::vector<uint8_t> &out) {
    if (deque_recv_buf.empty()) {

        return 0;
    } 
    out = std::move(deque_recv_buf.front());
    deque_recv_buf.pop_front();
        
    return 1;
}

int Client::set_nonblock(int fd) {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}