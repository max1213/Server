#include "net/client/services/FusionRecvServices.h"

FusionRecvServices::FusionRecvServices(Client &c) : command(c) {
}


int FusionRecvServices::getVecLic(std::vector<PluginLicenseData> &vecLic) {
    int is_empty = command.response(vecLic);

    if (!is_empty) {
        return 0;
    } 
    return 1;
}

void FusionRecvServices::recv() {
    command.recvData();
}

FusionRecvServices::~FusionRecvServices() {
    //delete command;
}
// void FusionRecvServices::getLic(PluginLicenseData &licData) {
//     std::cout << licData.clientId << " " << licData.pluginId << " " << licData.name << std::endl;
// }