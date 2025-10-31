#pragma once
#include <vector>
#include "net/client/models/ClientPluginInfo.h"
#include "net/client/commands/FusionRecvCommandClient.h"
#include "net/client/Client.h"

class FusionRecvServices {

public:
    FusionRecvServices(Client &c);
    ~FusionRecvServices();
    int getVecLic(std::vector<PluginLicenseData> &vecLic);
    void recv();
    // void getLic(PluginLicenseData &licData);

private:
    FusionRecvCommandClient command;
};