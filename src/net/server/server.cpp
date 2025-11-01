
#include "net/server/server.h"

const int count_events = 10;


Server::Server(std::string ip, int port) : _ip(ip), _port(port), Events(count_events) {

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

    epoll_event Event;
    Event.data.fd = main_socket;
    Event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, main_socket, &Event);

}

Server::~Server() {

    close_sockets();
}

int Server::create_socket() {
    int sok = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sok == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(_port);
    sa.sin_addr.s_addr = inet_addr(_ip.c_str());

    if (bind(sok, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        std::cerr << "❌ bind() failed: " << strerror(errno) << "\n";
        close(sok);
        return -1;
    }

    if (listen(sok, SOMAXCONN) == -1) {
        perror("listen");
        close(sok);
        return -1;
    }

    set_nonblock(sok);
    return sok;
}

void Server::socket_push_epoll(int socket_fd, int flag = EPOLL_CTL_ADD) {
    epoll_event Event;
    Event.data.fd = socket_fd;
    Event.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, flag, socket_fd, &Event) == -1) {
        perror("epoll_ctl");
    }

}

void Server::run() {

   
    int N = epoll_wait(epoll_fd, &Events[0], count_events, -1);

    if (N < 0) {
        perror("epoll_wait");
        return;
    }


    for (int i = 0; i < N; ++i) {
        int fd = Events[i].data.fd;

        if (Events[i].events & EPOLLERR) {
            printf("Ошибка на сокете %d\n", fd);
            close_socket(fd);
            continue;
        }

        if (Events[i].events & EPOLLHUP) {
            printf("Сокет %d был закрыт другой стороной\n", fd);
            close_socket(fd);
            continue;
        }

        if (Events[i].events & EPOLLIN) {
            if (fd == main_socket) {
                size_t sok = add_client();
                continue;
            }
            infoSocket &info_sock = socketEvents.find(fd)->second;
            auto &buffer = info_sock.buffer_vec;
            
            uint8_t tmp[4096];
            int n = recv(fd, tmp, sizeof(tmp), 0);

            if (n > 0) {
                // Данные пришли
                std::cout << "📥 Received " << n << " bytes from " << fd << std::endl;

                head_pars(tmp, n, info_sock);
                
                
                

            } else if (n == 0) {
                // Клиент реально закрыл соединение
                close_socket(fd);
                continue;

            } else if (n < 0) { // n < 0
                if (errno == EAGAIN || errno == EWOULDBLOCK) {

                    continue;
                    // Данных нет, но сокет жив
                    // Просто ждем следующего события EPOLLIN
                } else {
                    // Реальная ошибка
                    perror("recv");
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    socketEvents.erase(fd);
                    continue;
                }
            }
                
            
        }
    }
}

int Server::add_client () {
    int sock = accept(main_socket, NULL, NULL);
    if (sock == -1) {
        perror("accept");
        return -1;
    }
    
    set_nonblock(sock);
    socket_push_epoll(sock);
    infoSocket info;
    info.flag = EPOLLIN;
    info.buffer_vec = std::vector<uint8_t>();
    info.expected_size_buf = 0;
    info.is_first_packet = false;
    socketEvents.insert(std::make_pair(sock, info));

    std::cout << "👤 New client: " << sock <<"\n";

    return sock;
}

void Server::close_socket(int fd) {
    std::cout << "❌ Client " << fd << " disconnected\n";
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    socketEvents.erase(fd);
}

void Server::close_sockets() {
     //закрываем все сокеты
    for (auto it = socketEvents.begin(); it != socketEvents.end(); it++) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
        close(it->first);
    }
    //закрываем сеансовый сокет
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, main_socket, NULL);
    close(main_socket);
    //закрываем дискриптор epool
    close(epoll_fd);
}

void Server::sendData() {
    //send(int sockfd, const void *buf, size_t len, int flags);
    

}

void Server::readData() {
    
    
}

int Server::set_nonblock(int fd) {
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

void Server::head_pars(uint8_t *buf, int size, infoSocket& info) {
    if (!info.is_first_packet) {
        
        Header head = { 0 };
        memcpy(&head, buf, 3);
        info.is_first_packet = 1;
        info.expected_size_buf = ntohs(head.size_packet);
    } 
       
    info.buffer_vec.insert(info.buffer_vec.end(), buf, buf + size);

    if (info.buffer_vec.size() == info.expected_size_buf) {
        if (!decodFNV32a(info.buffer_vec)) {
            //ошибка хеш не правильный, пакет не полный обработать ошибку
            std::cout << "[ERROR] пакет поврежден" << std::endl;
            return;
        }

        info.buffer_vec.erase(info.buffer_vec.begin(), info.buffer_vec.begin() + 3);
        info.buffer_vec.erase(info.buffer_vec.end()-4, info.buffer_vec.end());

        for (auto i : info.buffer_vec) {
            std::cout << i;
        }
    }
}
//hash = (hash ^ byte) * FNV_prime_32
void Server::coderFNV32a(std::vector<uint8_t> &vec) {
    uint32_t FNV_prime = 16777619;
    uint32_t hash = 2166136261;
    
    for (auto byte : vec) {
        hash = (hash ^ byte) * FNV_prime;
    }

    uint8_t h[4];
    h[0] = hash;
    h[1] = hash >> 8;
    h[2] = hash >> 16;
    h[3] = hash >> 24;

    vec.insert(vec.end(), h, h + 4);

}

bool Server::decodFNV32a(std::vector<uint8_t> &vec) {
    uint32_t FNV_prime = 16777619;
    uint32_t hash = 2166136261;
    uint32_t out;
    memcpy(&out, vec.data() + (vec.size() - 4), 4);
    
    for (size_t i = 0; i < vec.size()-4; ++i) {
        hash = (hash ^ vec[i]) * FNV_prime;
    }
    if (hash == out) {
        return 1;
    } else {
        return 0;
    }

}