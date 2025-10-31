#pragma once
#include "net/client/Client.h"
#include "net/client/services/FusionSendServices.h"
#include "net/client/services/FusionRecvServices.h"
#include "net/client/models/ClientPluginInfo.h"
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