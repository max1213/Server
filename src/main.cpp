#include "net/server.h"
#include "net/Client.h"
#include <csignal>


bool stop_flag = false;

void handle_sigint(int) {
    stop_flag = true;
}

int main() {
    signal(SIGINT, handle_sigint); 

    //Server s("127.0.0.2", 5050);
    Client c("127.0.0.1", 8080);
    std::string str =
    "GET /exec/api/getEquipments HTTP/1.1\r\n"
    "Host: 127.0.0.1\r\n"
    "User-Agent: MyClient/1.0\r\n"
    "Accept: application/json\r\n"
    "Connection: close\r\n"
    "\r\n";
    std::vector<uint8_t> buf (str.begin(), str.end());
    c.sendToServer(buf);
    while(!stop_flag) {
        //s.run();
        c.run();
    }
    return 0;
}
