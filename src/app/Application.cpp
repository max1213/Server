#include "app/Application.h"

bool stop_flag = false;

void handle_sigint(int) {
    stop_flag = true;
}

Application::Application(std::string ip, std::string port) :
client (ip, 8080), FSServices(client, ip, port),
FRServices(client){
    signal(SIGINT, handle_sigint);
    //Server s("127.0.0.2", 5050);
    
    int port2 = 8080;
    
    
}

void Application::run() {
    FSServices.getPluginName();
    std::vector<PluginLicenseData> vecLic;
    Server server("0.0.0.0", 5050);
    while(!stop_flag) {
        server.run();
        // FSServices.send();
        
        // FRServices.recv();
        // int err = FRServices.getVecLic(vecLic);
        // if (err) {
        //     for (auto v : vecLic) {
        //         std::cout << v.clientId << " " << v.pluginId << " " << v.name << std::endl;
        //     }
        // }
    }
}