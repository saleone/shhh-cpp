#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

#include "vendor/uWebSockets/src/App.h"

#define DEFAULT_PORT 9876
#define SSL false

typedef uWS::TemplatedApp<SSL> App;

uint16_t port = DEFAULT_PORT;
struct UserData {
  bool lockOwner = false;
};

static bool lock = false;
static std::forward_list<uWS::WebSocket<SSL, true> *> clients;

uint16_t get_port(char *);
void notify_all(std::forward_list<uWS::WebSocket<SSL, true> *> const *,
                std::string);
void notify_all(std::forward_list<uWS::WebSocket<SSL, true> *> const *, bool);

int main(int argc, char **argv) {
  if (argc > 1)
    port = get_port(argv[1]);

  if (port == 0)
    return 1;

  App()
      .ws<UserData>(
          "/*",
          {.compression = uWS::SHARED_COMPRESSOR,
           .maxPayloadLength = 16 * 1024,
           .open =
               [](auto *ws, auto *req) {
                 std::cout << "+conn" << std::endl;
                 clients.push_front(ws);
               },
           .message =
               [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                 std::cout << "+msg" << std::endl;

                 UserData *data = static_cast<UserData *>(ws->getUserData());

                 if (lock && !data->lockOwner)
                   return;

                 lock = !lock;

                 std::cout << "1" << std::endl;
                 notify_all(&clients, lock);
                 std::cout << "2" << std::endl;

                 data->lockOwner = lock;
               },
           .drain = [](auto *ws) {},
           .ping = [](auto *ws) {},
           .pong = [](auto *ws) {},
           .close =
               [](auto *ws, int code, std::string_view message) {
                 std::cout << "-conn" << std::endl;
                 clients.remove(ws);
               }})
      .listen(port,
              [](auto *token) {
                if (token)
                  std::cout << "Running: " << port << std::endl;
              })
      .run();

  return 0;
}

void notify_all(std::forward_list<uWS::WebSocket<SSL, true> *> const *conns,
                bool lock) {
  notify_all(conns, lock ? "LOCKED" : "UNLOCKED");
}

void notify_all(std::forward_list<uWS::WebSocket<SSL, true> *> const *conns,
                std::string msg) {
  std::cout << "notify string" << std::endl;
  std::for_each(conns->begin(), conns->end(),
                [msg](auto *ws) { ws->send(msg, uWS::OpCode::TEXT); });
}

uint16_t get_port(char *user_port) {
  try {
    int passed_port = std::stoi(user_port);

    if (passed_port > UINT16_MAX)
      throw std::invalid_argument("invalid port");

    return passed_port;
  } catch (std::invalid_argument &e) {
    std::cout << "Port number is invalid: " << user_port << std::endl;
    return 0;
  }
}
