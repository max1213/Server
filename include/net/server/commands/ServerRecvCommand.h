#pragma once

#include "net/conf.h"
#include "net/server/models/ServerPluginInfo.h"
#include "net/server/server.h"

class ServerRecvCommand {
    
public:
    ServerRecvCommand(Server &s);
    int response(std::vector<PluginLicenseData>& vecLicData);
    void recvData();
    
private:
    void jsonPars(std::vector<uint8_t> &buffer, std::vector<PluginLicenseData> &vecLicData);
    void onDataReady();
    
private:
    Server &server;



};