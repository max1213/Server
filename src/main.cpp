#include "app/Application.h"


int main() {
    std::string ip = "127.0.0.1";
    std::string port = "8080";
    Application app (ip, port);
    app.run();
    return 0;
}
