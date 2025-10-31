#include "net/client/services/FusionSendServices.h"

FusionSendServices::FusionSendServices(Client &c, std::string &ip, std::string &port) : 
command(c, ip, port), _ip(ip), _port(port) {

}

void FusionSendServices::send() {
    command.send();
}

void FusionSendServices::getPluginName() {
    command.getPluginName();
}