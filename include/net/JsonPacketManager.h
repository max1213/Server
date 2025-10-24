#pragma once
#include "nlohmann/json.hpp"
#include <iostream>
#include "net/NetCore.h"
#include "conf.h"

extern "C" {
#include "picohttpparser/picohttpparser.h"
}

class JsonPacketManager {

    struct HttpHead {
        const char *method;           //Тут будет храниться тип например GET
        size_t method_len;
        const char *path;             //Тут хранится путь запроса
        size_t path_len ;
        int minor_version;                      //Версия HTTP
        size_t prevbuflen;                      //Длина предыдущего буфера если первый раз то ставь 0
        int pret;
        struct phr_header headers[100];   //Массив структур для хранения заголовков (name, value, name_len, value_len).
        size_t num_headers;                     //Вход: размер массива headers; выход: сколько реально заголовков было найдено.
    };

    struct HttpResponse {
        const char *buf; 
        size_t len;
        int *minor_version;
        int *status;
        const char **msg;
        size_t *msg_len;
        struct phr_header *headers;
        size_t *num_headers;
        size_t last_len;
        int pret;
    };
public:
    JsonPacketManager(infoSocket& sock, int n, char *buf);

private:
    void event_loop();

private:
    char* sys_buffer;
    infoSocket* info_sock;
    HttpResponse head;
    int recv_size;
};