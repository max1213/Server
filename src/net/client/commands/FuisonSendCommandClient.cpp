#include "net/client/commands/FusionSendCommandClient.h"

SendCommandClient::SendCommandClient(Client &c, std::string &ip, std::string &port) : 
client(c), _ip(ip), _port(port) {
    head = { 0 };
}

void SendCommandClient::getPluginName() {
    std::string get_str =
    "GET /exec/api/getEquipments HTTP/1.1\r\n"
    "Host: "+ _ip + ":" + _port + "\r\n"
    "User-Agent: Client\r\n"
    "Accept: application/json\r\n"
    "Connection: close\r\n"
    "\r\n";

    std::vector<uint8_t> vec(get_str.begin(), get_str.end());
    client.sendToClinet(vec);
}

void SendCommandClient::send() {
    client.sendData();
}