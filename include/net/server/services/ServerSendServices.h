#include "net/server/commands/ServerSendCommand.h"
#include "net/server/server.h"

#include <string>

class ServerSendServices {

public:
    ServerSendServices(Server &s, std::string &ip, std::string &port);
    void send();
    void getPluginName();

private:
    std::string _ip;
    std::string _port;
    ServerSendCommand command;
};