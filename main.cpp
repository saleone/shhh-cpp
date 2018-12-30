#include <iostream>
#include <stdexcept>
#include <string_view>
#include <cstdint>

#include "vendor/uWebSockets/src/App.h"


#define DEFAULT_PORT 9876


uint16_t port = DEFAULT_PORT;
struct PerSocketData {};

uint16_t get_port(char *);

int main(int argc, char **argv)
{
    if (argc > 1)
        port = get_port(argv[1]);

    if (port == 0)
        return 1;

    uWS::App().ws<PerSocketData>("/*", {
        .compression = uWS::SHARED_COMPRESSOR,
        .maxPayloadLength = 16 * 1024,
        .open = [](auto *ws, auto *req) {},
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
            ws->send(message, opCode);
        },
        .drain = [](auto *ws) {},
        .ping = [](auto *ws) {},
        .pong = [](auto *ws) {},
        .close = [](auto *ws, int code, std::string_view message) {}
    }).listen(port, [](auto *token) {
        if (token)
            std::cout << "Listening on port " << port << std::endl;
    }).run();

    return 0;
}

uint16_t get_port(char *user_port)
{
    try {
        int passed_port = std::stoi(user_port);

        if (passed_port > UINT16_MAX)
            throw std::invalid_argument("invalid port");

        return passed_port;
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Port number is invalid: "<< user_port << std::endl;
        return 0;
    }
}
