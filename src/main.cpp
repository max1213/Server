#include "server.h"

int main() {
    Server s("127.0.0.1", 5050);
    s.run();
    return 0;
}
