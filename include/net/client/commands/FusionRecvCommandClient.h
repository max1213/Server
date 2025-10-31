#pragma once
#include "nlohmann/json.hpp"
#include <iostream>
#include "conf.h"
#include "net/client/models/ClientPluginInfo.h"
#include "net/client/interface.h"
#include "net/client/Client.h"

class FusionRecvCommandClient{
    
public:
    FusionRecvCommandClient(Client &c);
    int response(std::vector<PluginLicenseData>& vecLicData);
    void recvData();
    
private:
    void jsonPars(std::vector<uint8_t> &buffer, std::vector<PluginLicenseData> &vecLicData);
    void onDataReady();
    
private:
    Client &client;



};