#include "net/client/commands/FusionRecvCommandClient.h"
using json = nlohmann::json;

FusionRecvCommandClient::FusionRecvCommandClient(Client &c) : client(c) {

}

void FusionRecvCommandClient::jsonPars(std::vector<uint8_t> &buffer, std::vector<PluginLicenseData> &vecLicData) {
    
    json j = json::parse(buffer.begin(), buffer.end());
    PluginLicenseData pld;

    if (j.is_array()) {
        vecLicData.clear();
        for (int i = 0; i < j.size(); i++) {
            pld.clientId = j[i]["id"];
            pld.pluginId = j[i]["type"];
            pld.name = j[i]["sn"];
            vecLicData.push_back(pld);

        }

    } else if (j.is_object()) {
        pld.clientId = j["id"];
        pld.pluginId = j["type"];
        pld.name = j["sn"];
        vecLicData.push_back(pld);
    }

}

int FusionRecvCommandClient::response(std::vector<PluginLicenseData>& vecLicData) {
    std::vector<uint8_t> buffer;
    int is_empty_buf = client.onDataReady(buffer);
    if (!is_empty_buf) {
        return 0;
    }    
    jsonPars(buffer, vecLicData);
    return 1;
}

void FusionRecvCommandClient::recvData() {
    client.recvData();
}