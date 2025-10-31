
// #include "net/server.h"
// #include "nlohmann/json.hpp"
// #include "net/JsonPacketManager.h"



// extern "C" {
// #include "picohttpparser/picohttpparser.h"
// }

// const int count_events = 10;


// Server::Server(std::string ip, int port) : _ip(ip), _port(port), Events(count_events) {

//     main_socket = create_socket();
//     if (main_socket < 0) {
//         std::cerr << "[ERROR] Dont open socket.";
//         return;
//     }

//     epoll_fd = epoll_create1(0);

//     if (epoll_fd == -1) {
//         perror("epoll_create1");
//         close(main_socket);
//         return;
// }

//     epoll_event Event;
//     Event.data.fd = main_socket;
//     Event.events = EPOLLIN;
//     epoll_ctl(epoll_fd, EPOLL_CTL_ADD, main_socket, &Event);

// }

// Server::~Server() {

//     close_sockets();
// }

// int Server::create_socket() {
//     int sok = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

//     if (sok == -1) {
//         perror("socket");
//         return -1;
//     }

//     struct sockaddr_in sa;
//     sa.sin_family = AF_INET;
//     sa.sin_port = htons(_port);
//     sa.sin_addr.s_addr = inet_addr(_ip.c_str());

//     if (bind(sok, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
//         std::cerr << "❌ bind() failed: " << strerror(errno) << "\n";
//         close(sok);
//         return -1;
//     }

//     if (listen(sok, SOMAXCONN) == -1) {
//         perror("listen");
//         close(sok);
//         return -1;
//     }

//     NetCore::set_nonblock(sok);
//     return sok;
// }

// void Server::socket_push_epoll(int socket_fd, int flag = EPOLL_CTL_ADD) {
//     epoll_event Event;
//     Event.data.fd = socket_fd;
//     Event.events = EPOLLIN;

//     if (epoll_ctl(epoll_fd, flag, socket_fd, &Event) == -1) {
//         perror("epoll_ctl");
//     }

// }

// void Server::run() {

//     const char *method;           //Тут будет храниться тип например GET
//     size_t method_len;
//     const char *path;             //Тут хранится путь запроса
//     size_t path_len ;
//     int minor_version;                      //Версия HTTP
//     struct phr_header headers[100];   //Массив структур для хранения заголовков (name, value, name_len, value_len).
//     size_t num_headers;                     //Вход: размер массива headers; выход: сколько реально заголовков было найдено.
//     size_t prevbuflen = 0;                      //Длина предыдущего буфера если первый раз то ставь 0
//     int pret;

//     int N = epoll_wait(epoll_fd, &Events[0], count_events, -1);

//     if (N < 0) {
//         perror("epoll_wait");
//         return;
//     }


//     for (int i = 0; i < N; ++i) {
//         int fd = Events[i].data.fd;

//         if (Events[i].events & EPOLLERR) {
//             printf("Ошибка на сокете %d\n", fd);
//             close_socket(fd);
//             continue;
//         }

//         if (Events[i].events & EPOLLHUP) {
//             printf("Сокет %d был закрыт другой стороной\n", fd);
//             close_socket(fd);
//             continue;
//         }

//         if (Events[i].events & EPOLLIN) {
//             if (fd == main_socket) {
//                 size_t sok = add_client();
//                 continue;
//             }
//             infoSocket &info_sock = socketEvents.find(fd)->second;
//             auto &buffer = info_sock.buffer_vec;
            
//             char tmp[4096];
//             int n = recv(fd, tmp, sizeof(tmp), 0);

//             if (n > 0) {
//                 // Данные пришли
//                 std::cout << "📥 Received " << n << " bytes from " << fd << std::endl;

            
//                 JsonPacketManager JPM(info_sock, n, tmp);
                
                

//             } else if (n == 0) {
//                 // Клиент реально закрыл соединение
//                 close_socket(fd);
//                 continue;

//             } else if (n < 0) { // n < 0
//                 if (errno == EAGAIN || errno == EWOULDBLOCK) {

//                     continue;
//                     // Данных нет, но сокет жив
//                     // Просто ждем следующего события EPOLLIN
//                 } else {
//                     // Реальная ошибка
//                     perror("recv");
//                     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
//                     close(fd);
//                     socketEvents.erase(fd);
//                     continue;
//                 }
//             }
                
            
//         }
//     }
// }

// int Server::add_client () {
//     int sock = accept(main_socket, NULL, NULL);
//     if (sock == -1) {
//         perror("accept");
//         return -1;
//     }
    
//     NetCore::set_nonblock(sock);
//     socket_push_epoll(sock);
//     infoSocket info;
//     info.flag = EPOLLIN;
//     info.buffer_vec = std::vector<char>();
//     info.expected_size_buf = 0;
//     info.is_first_packet = false;
//     socketEvents.insert(std::make_pair(sock, info));

//     std::cout << "👤 New client: " << sock <<"\n";

//     return sock;
// }

// void Server::close_socket(int fd) {
//     std::cout << "❌ Client " << fd << " disconnected\n";
//     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
//     close(fd);
//     socketEvents.erase(fd);
// }

// void Server::close_sockets() {
//      //закрываем все сокеты
//     for (auto it = socketEvents.begin(); it != socketEvents.end(); it++) {
//         epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
//         close(it->first);
//     }
//     //закрываем сеансовый сокет
//     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, main_socket, NULL);
//     close(main_socket);
//     //закрываем дискриптор epool
//     close(epoll_fd);
// }

// void Server::sendData() {
//     //send(int sockfd, const void *buf, size_t len, int flags);
    

// }

// void Server::readData() {
    
    
// }