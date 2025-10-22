#pragma once

#include "nlohmann/json.hpp"

extern "C" {
#include "picohttpparser/picohttpparser.h"
}

class JsonPacketManager {

void event_loop();
};