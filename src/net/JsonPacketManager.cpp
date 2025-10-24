#include "net/JsonPacketManager.h"

using json = nlohmann::json;

JsonPacketManager::JsonPacketManager(infoSocket& sock, int n, char *buf) : 
 info_sock(&sock), recv_size(n), sys_buffer(buf){
    
    head = { 0 };
    event_loop();
}

void JsonPacketManager::event_loop() {

    auto &buffer = info_sock->buffer_vec;

    std::string s(sys_buffer, sys_buffer + recv_size);
    std::cout << s << std::endl;

    if (!info_sock->is_first_packet) {
    
        head.pret = phr_parse_response(
        sys_buffer,
        recv_size,
        head.minor_version,
        head.status,
        head.msg,
        head.msg_len,
        head.headers,
        head.num_headers,
        0);
    

        if (head.pret > 0) {
            for (int i = 0; i != *head.num_headers; ++i) {

                std::string name(head.headers[i].name, head.headers[i].name_len);
                std::string value(head.headers[i].value, head.headers[i].value_len);

                if (name == "Content-Length")  {
    
                    info_sock->expected_size_buf = atoi(value.c_str());
                    buffer.insert(buffer.end(), sys_buffer + head.pret, sys_buffer + recv_size);  //от шапки до конца
                    info_sock->is_first_packet = 1;
                    break;
                }
                
            }

            return;
        } else if (head.pret == -1) {
            std::cout << "[ERROR] pars bufer socket." << std::endl;
            return; //ошибка парсинга

        } else if (head.pret == -2) { 
            std::cout << "еще не пришел\n" << std::endl;
            return; // еще не пришел вест пакет 
        }
        
    }
    
    
   
    if (info_sock->expected_size_buf > buffer.size()) {
        buffer.insert(buffer.end(), sys_buffer, sys_buffer + recv_size);  //от шапки до конца
        
    }
    

    if (info_sock->expected_size_buf == buffer.size()) {
        json j = json::parse(buffer.begin(), buffer.end());

        if (j.is_array()) {
            for (int i = 0; i < j.size(); i++) {
                std::cout << "id: " << j[i]["id"] << ", type: "
                << j[i]["type"] << "sn: " << j[i]["sn"] << std::endl;
            }

        } else if (j.is_object()) {
            std::cout << "id: " << j["id"] << ", type: "
            << j["type"] << "sn: " << j["sn"] << std::endl;
        }

        buffer.clear();
        buffer.shrink_to_fit();
        info_sock->expected_size_buf = 0;
        info_sock->is_first_packet = 0;
        return; /* successfully parsed the request */
    }
}

//    head.num_headers = sizeof(head.headers) / sizeof(head.headers[0]);
//         head.pret = phr_parse_request(
//         sys_buffer, 
//         recv_size, 
//         &head.method,
//         &head.method_len,
//         &head.path, 
//         &head.path_len,
//         &head.minor_version, 
//         head.headers, 
//         &head.num_headers, 
//         head.prevbuflen);
    