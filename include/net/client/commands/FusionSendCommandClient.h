#pragma once

#include "nlohmann/json.hpp"
#include "net/client/Client.h"
#include "net/conf.h"
#include "vector"

extern "C" {
#include "picohttpparser/picohttpparser.h"
}

class SendCommandClient {

public:
    SendCommandClient(Client &c, std::string &ip, std::string &port);
    void getPluginName();
    void send();

private:
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

private:
    Client &client;
    std::string _port;
    std::string _ip;
    HttpResponse head;
};