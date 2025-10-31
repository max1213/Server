#include "net/client/commands/FusionSendCommandClient.h"
#include "net/client/Client.h"

#include <string>

class FusionSendServices {

public:
    FusionSendServices(Client &c, std::string &ip, std::string &port);
    void send();
    void getPluginName();

private:
    std::string _ip;
    std::string _port;
    SendCommandClient command;
};