
#include "server.h"
const int count_events = 10;
extern "C" {
#include "picohttpparser/picohttpparser.h"
}

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
    std::map<int, std::vector<char>> buffer_vec;
    
    const char *method;           //Тут будет храниться тип например GET
    size_t method_len;
    const char *path;             //Тут хранится путь запроса
    size_t path_len ;
    int minor_version;                      //Версия HTTP
    struct phr_header headers[100];   //Массив структур для хранения заголовков (name, value, name_len, value_len).
    size_t num_headers;                     //Вход: размер массива headers; выход: сколько реально заголовков было найдено.
    size_t prevbuflen = 0;                      //Длина предыдущего буфера если первый раз то ставь 0
    int pret;

    while (true) {
        int N = epoll_wait(epoll_fd, &Events[0], count_events, -1);

        if (N < 0) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < N; ++i) {
            int fd = Events[i].data.fd;

            if (Events[i].events & EPOLLIN) {
                if (fd == main_socket) {
                    add_client();
                    buffer_vec.insert({fd, std::vector<char>()});
                    continue;
                }
                std::cout<<"zahel v http";


               if (buffer_vec.find(fd) == buffer_vec.end()) {
                    buffer_vec.insert({fd, std::vector<char>()});
               }
                auto &buffer = buffer_vec.find(fd)->second;
                char tmp[1024];
                int n = recv(fd, tmp, sizeof(tmp), 0);

                if (n > 0) {
                    // Данные пришли
                    buffer.insert(buffer.end(), tmp, tmp + n); 

                    std::cout << "📥 Received " << n << " bytes from " << fd << "\n";
                    pret = phr_parse_request(buffer.data(), buffer.size(), &method, &method_len, &path, &path_len,
                    &minor_version, headers, &num_headers, prevbuflen);
                    std::cout << "ok";
                    if (pret > 0) {
                        printf("request is %d bytes long\n", pret);
                        printf("method is %.*s\n", (int)method_len, method);
                        printf("path is %.*s\n", (int)path_len, path);
                        printf("HTTP version is 1.%d\n", minor_version);
                        printf("headers:\n");
                        for (i = 0; i != num_headers; ++i) {
                            printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
                            (int)headers[i].value_len, headers[i].value);
                        }
                        buffer.clear();
                        buffer.shrink_to_fit();
                        break; /* successfully parsed the request */
                    }
                    else if (pret == -1) {
                        std::cout << "[ERROR] pars bufer socket.";
                        continue; //ошибка парсинга
                    }
                    
                    


                } else if (n == 0) {
                    // Клиент реально закрыл соединение
                    std::cout << "❌ Client " << fd << " disconnected\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    socketEvents.erase(fd);
                    continue;
                } else { // n < 0
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

            // std::map<int, infoSocket>::iterator it = socketEvents.find(fd);
            // std::vector<char> &bufSock = it->second.buf;
            // if(!it->second.buf.empty()) {


            // } else {
            //     uint8_t size_buf = 0;

            //     size_buf = buffer[0];
            //     bufSock.insert(bufSock.end(), 5, 0);
            // }



            // switch (buffer[0])
            // {
            // case Server::SendCommands.ECHO:
            //     /* code */
            //     break;

            // default:
            //     break;
            // }

            //     if (buffer[0] == '2') {
            //         const char* msg = "2";
            //         send(fd, msg, strlen(msg), 0);
            //         std::cout << "✅ Sent close confirmation\n";
            //         epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            //         close(fd);
            //         socketEvents.erase(fd);
            //     } else {
            //         std::cout << "📩 Received (" << n << " bytes): ";
            //         std::cout.write(buffer, n);
            //         std::cout << "\n";
            //     }
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
    socketEvents.insert(std::make_pair(sock, info));

    std::cout << "👤 New client: " << sock << "\n";

    return 0;
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
