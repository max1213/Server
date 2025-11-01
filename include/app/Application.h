#pragma once
#include "net/client/Client.h"
#include "net/client/services/FusionSendServices.h"
#include "net/client/services/FusionRecvServices.h"
#include "net/client/models/ClientPluginInfo.h"
#include "net/server/models/ServerPluginInfo.h"
#include "net/server/services/ServerRecvServices.h"
#include "net/server/services/ServerSendServices.h"
#include "net/server/server.h"
#include <csignal>

class Application {

public:
    Application(std::string ip, std::string port);
    void run();

private:
    Client client;
    FusionSendServices FSServices;
    FusionRecvServices FRServices;
};