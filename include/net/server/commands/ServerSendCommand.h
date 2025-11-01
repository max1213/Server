#pragma once

#include "net/server/server.h"
#include "net/conf.h"
#include "vector"


class ServerSendCommand {

public:
    ServerSendCommand(Server &s, std::string &ip, std::string &port);
    void getPluginName();
    void send();


private:
    Server &server;
    std::string _port;
    std::string _ip;
};