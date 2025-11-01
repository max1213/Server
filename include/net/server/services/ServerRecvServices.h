#pragma once
#include <vector>
#include "net/server/models/ServerPluginInfo.h"
#include "net/server/commands/ServerRecvCommand.h"
#include "net/server/server.h"

class ServerRecvServices {

public:
    ServerRecvServices(Server &s);
    ~ServerRecvServices();
    int getVecLic(std::vector<PluginLicenseData> &vecLic);
    void recv();
    // void getLic(PluginLicenseData &licData);

private:
    ServerRecvCommand command;
};