#pragma once

#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"
#include "utils.hpp"
#include <set>

using namespace asio::ip;
using namespace std::chrono;

namespace std {
template <>
struct hash<udp::endpoint> {
    std::size_t operator()(const udp::endpoint &endpoint) const noexcept {
        std::size_t h1 = std::hash<std::string>()(endpoint.address().to_string());
        std::size_t h2 = std::hash<unsigned short>()(endpoint.port());
        return h1 ^ (h2 << 1);
    }
};
} // namespace std

// udp server that can handle multiple clients
class UdpServer {
  private:
    asio::io_context _io_context;
    udp::socket _socket;
    std::unordered_map<udp::endpoint, Player> _clients;
    std::set<int> _ids;
    udp::endpoint _sender_endpoint;
    std::array<char, 1024> _buffer{};
    std::thread _timer;
    std::thread _simulation;
    // mutex for socket endpoint
    std::mutex _socketMutex;
    // bullets
    std::vector<Bullet> _bullets;
    // enemies
    std::vector<Enemy> _enemies;
    std::set<int> _enemyIds;
    // timer for enemy spawn
    std::chrono::time_point<std::chrono::system_clock> _enemySpawnTimer;
    // frame time
    std::chrono::time_point<std::chrono::system_clock> _lastFrame;
    // stop variable
    std::atomic<bool> _stopServer;

  public:
    UdpServer() : _socket(_io_context, udp::endpoint(udp::v4(), 12345)), _stopServer(false) {
        for (int i = 1; i <= 4; i++)
            _ids.insert(i);
        for (int i = 1; i <= 10; i++)
            _enemyIds.insert(i);
    }

    ~UdpServer();

    // start the server
    void start();

    // handle request from client
    void handleRequest(std::error_code ec, std::size_t bytes_recvd);

    // send response to client
    void sendResponse(const udp::endpoint &endpoint, const std::string &msg);

    // receive request from client
    void receiveRequest();

    // check if client still alive
    void checkAlive();

    // send message to all clients
    void sendAll(const std::string &msg, bool includeSender = true);

    // remove client from server
    void removeClient(const udp::endpoint &endpoint, bool erase = true);

    // simulate game
    void simulate();
};