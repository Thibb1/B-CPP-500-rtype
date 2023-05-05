/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** UpdServer.cpp by thibb1
*/

#include "network.hpp"

/**
 * It starts the server
 */
void UdpServer::start() {
    log(LOG_INFO, "UDP server started on port " + std::to_string(_port));
    srand(time(nullptr));
    _timer = std::thread([this]() {
        while (!_stopServer) {
            std::this_thread::sleep_for(seconds(3));
            checkAlive();
        }
    });
    //_timer.detach();
    _simulation = std::thread([this]() {
        auto startTime = steady_clock::now();
        _enemySpawnTimer = startTime;
        _lastFrame = startTime;
        while (!_stopServer)
            simulate();
        _stopServer = true;
        //CloseWindow();
    });
    //_simulation.detach();
    receiveRequest();
    _io_context.run();
}

/**
 * It closes the socket and cancels any pending asynchronous operations
 */
UdpServer::~UdpServer() {
    _stopServer = true;
    _io_context.stop();
    _timer.join();
    _simulation.join();
    _socket.close();
    log(LOG_INFO, "UDP server stopped");
}
/**
 * It handles a request from a client
 *
 * @param ec error code
 * @param bytes_recvd The number of bytes received.
 *
 * @return The return type is void.
 */
void UdpServer::handleRequest(std::error_code ec, std::size_t bytes_recvd) {
    if (!ec && bytes_recvd > 0) {
        std::string msg(_buffer.begin(), _buffer.begin() + bytes_recvd);
        log(LOG_INFO, "From: " + _sender_endpoint.address().to_string() + ":" + std::to_string(_sender_endpoint.port()));
        log(LOG_INFO, "Received: " + msg);
        if (_clients.find(_sender_endpoint) == _clients.end()) {
            if (_ids.empty()) {
                sendResponse(_sender_endpoint, "full");
                return;
            }
            auto id = *_ids.begin();
            _ids.erase(id);
            _clients[_sender_endpoint] = Player(id);
            dynamic_cast<text *>(_overlay.getId("players_nbr"))->setText(std::to_string(_clients.size()));
            sendResponse(_sender_endpoint, "id:" + std::to_string(_clients[_sender_endpoint].getId()));
            sendAll("new:" + std::to_string(_clients[_sender_endpoint].getId()) + "," + vectorToString(_clients[_sender_endpoint].getPosition()),
                    false);
            // send all players to new player
            for (auto &client : _clients) {
                if (client.first != _sender_endpoint) {
                    sendResponse(_sender_endpoint,
                                 "new:" + std::to_string(client.second.getId()) + "," + vectorToString(client.second.getPosition()));
                    sendResponse(_sender_endpoint, "name:" + std::to_string(client.second.getId()) + "," + client.second.getName());
                    sendResponse(_sender_endpoint, "score:" + std::to_string(client.second.getId()) + "," + std::to_string(client.second.getScore()));
                }
            }
            // send all enemies to new player
            for (auto &enemy : _enemies) {
                sendResponse(_sender_endpoint, "spawn:" + std::to_string(enemy.getId()) + "," + vectorToString(enemy.getPosition()) + "," +
                                                   vectorToString(enemy.getVelocity()) + "," + std::to_string(enemy.getHp()));
            }
        }
        auto &player = _clients[_sender_endpoint];
        log(LOG_INFO, "Player: " + std::to_string(player.getId()));
        if (msg == "ping") {
            sendResponse(_sender_endpoint, "pong");
            player.setAlive(true);
        } else if (msg == "disconnect") {
            removeClient(_sender_endpoint);
        } else if (isMatch(msg, "^move:(left|right|up|down)," NB_R "," NB_R "$")) {
            auto match = getMatches(msg, "^move:(left|right|up|down)," NB_R "," NB_R "$");
            auto direction = match[1];
            auto x = std::stof(match[2]);
            auto y = std::stof(match[3]);
            auto pos = Vector2{x, y};
            auto valid = player.move(direction, pos);
            sendAll("move:" + std::to_string(player.getId()) + "," + vectorToString(player.getPosition()), !valid);
        } else if (msg == "shoot") {
            auto bulletPosition = player.getPosition();
            bulletPosition.x += 0.5;
            auto bulletVelocity = Vector2{5, 0};
            _bullets.emplace_back(bulletPosition, bulletVelocity, .25, player.getId());
            sendAll("shoot:" + std::to_string(player.getId()), false);
        } else if (isMatch(msg, "^name:([a-zA-Z0-9 ]+)$")) {
            auto name = getMatch(msg, "^name:([a-zA-Z0-9 ]+)$", 1);
            player.setName(name);
            sendAll("name:" + std::to_string(player.getId()) + "," + name);
        }
    }
}

/**
 * "Send a message to the specified endpoint."
 *
 * The first thing we do is lock the socket mutex. This is because we're going to be using the socket,
 * and we don't want another thread to be using it at the same time
 *
 * @param endpoint The endpoint to send the message to.
 * @param msg The message to send.
 */
void UdpServer::sendResponse(const udp::endpoint &endpoint, const std::string &msg) {
    std::lock_guard<std::mutex> lock(_socketMutex);
    _socket.send_to(asio::buffer(msg), endpoint);
}

/**
 * "When a request is received, handle it and then wait for the next request."
 *
 * The first thing we do is check if the server has been stopped. If it has, we return immediately
 *
 * @return a future that will receive the result of the asynchronous operation.
 */
void UdpServer::receiveRequest() {
    if (_stopServer)
        return;
    _socket.async_receive_from(asio::buffer(_buffer), _sender_endpoint, [this](std::error_code ec, std::size_t bytes_recvd) {
        handleRequest(ec, bytes_recvd);
        receiveRequest();
    });
}

/**
 * If a client hasn't sent a message in the last 5 seconds, remove it from the server
 *
 * @return A pointer to the client
 */
void UdpServer::checkAlive() {
    if (_clients.empty())
        return;
    log(LOG_INFO, "Checking clients...");
    auto it = _clients.begin();
    while (it != _clients.end()) {
        if (!it->second.isAlive()) {
            log(LOG_INFO, "Client: " + it->first.address().to_string() + ":" + std::to_string(it->first.port()) + " is dead");
            removeClient(it->first, false);
            it = _clients.erase(it);
        } else {
            it->second.setAlive(false);
            ++it;
        }
    }
}

/**
 * It sends a message to all clients except the sender
 *
 * @param msg The message to send to all clients.
 * @param includeSender If true, the sender of the message will also receive the message.
 */
void UdpServer::sendAll(const std::string &msg, bool includeSender) {
    for (auto &client : _clients) {
        if (includeSender || client.first != _sender_endpoint)
            sendResponse(client.first, msg);
    }
}

/**
 * It removes a client from the server
 *
 * @param endpoint The endpoint of the client to remove.
 * @param erase if true, the client will be erased from the map.
 */
void UdpServer::removeClient(const udp::endpoint &endpoint, bool erase) {
    if (_clients.find(endpoint) != _clients.end()) {
        _ids.insert(_clients[endpoint].getId());
        sendAll("del:" + std::to_string(_clients[endpoint].getId()), true);
        if (erase) {
            _clients.erase(endpoint);
            dynamic_cast<text *>(_overlay.getId("players_nbr"))->setText(std::to_string(_clients.size()));
            // if (_clients.empty())
            //     stop();
        }
    }
}

/**
 * It updates the positions of the enemies and bullets, checks for collisions, and sends messages to
 * the clients
 */
void UdpServer::simulate() {
    // lock the mutex
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // update frame time
    auto tp = steady_clock::now();
    auto deltaTime = float(duration_cast<milliseconds>(tp - _lastFrame).count()) / 1000.0f;
    _lastFrame = tp;
    for (auto enemy = _enemies.begin(); enemy != _enemies.end();) {
        enemy->update(deltaTime);
        auto enemyPosition = enemy->getPosition();
        auto enemyBounds = enemy->getBounds();
        // check if enemy collide with player
        bool collide = false;
        for (auto &client : _clients) {
            if (client.second.getHp() <= 0)
                continue;
            auto playerBounds = client.second.getBounds();
            if (CheckCollisionBoxes(enemyBounds, playerBounds)) {
                client.second.takeDamage(1);
                if (client.second.getHp() <= 0) {
                    log(LOG_INFO, "Player: " + client.first.address().to_string() + ":" + std::to_string(client.first.port()) + " was killed");
                    sendAll("kill:" + std::to_string(client.second.getId()), true);
                } else {
                    sendResponse(client.first, "hit");
                }
                _enemyIds.insert(enemy->getId());
                sendAll("dead:" + std::to_string(enemy->getId()), true);
                enemy = _enemies.erase(enemy);
                collide = true;
                break;
            }
        }
        if (collide)
            continue;
        if (enemyPosition.x < -10) {
            log(LOG_INFO, "Enemy: " + std::to_string(enemy->getId()) + " out of bounds");
            _enemyIds.insert(enemy->getId());
            enemy = _enemies.erase(enemy);
        } else {
            ++enemy;
        }
    }
    // update bullets
    for (auto bullet = _bullets.begin(); bullet != _bullets.end();) {
        bullet->update(deltaTime);
        bool bulletHit = false;
        for (auto enemy = _enemies.begin(); enemy != _enemies.end();) {
            if (bullet->isColliding(enemy->getBounds())) {
                enemy->takeDamage(bullet->getDamage());
                if (enemy->isDead()) {
                    log(LOG_INFO, "Enemy: " + std::to_string(enemy->getId()) + " was killed");
                    sendAll("dead:" + std::to_string(enemy->getId()));
                    _enemyIds.insert(enemy->getId());
                    _enemies.erase(enemy);
                } else {
                    sendAll("damaged:" + std::to_string(enemy->getId()) + "," + std::to_string(enemy->getHp()));
                }
                bulletHit = true;
                // score player increase
                for (auto &client : _clients) {
                    auto id = client.second.getId();
                    if (id == bullet->getPlayerId()) {
                        client.second.addScore(100);
                        sendAll("score:" + std::to_string(id) + "," + std::to_string(client.second.getScore()));
                        break;
                    }
                }
                break;
            } else {
                ++enemy;
            }
        }
        if (bulletHit || bullet->isOutOfBounds()) {
            bullet = _bullets.erase(bullet);
        } else {
            ++bullet;
        }
    }
    if (!_enemyIds.empty()) {
        auto now = std::chrono::steady_clock::now();
        if (now - _enemySpawnTimer > std::chrono::seconds(2)) {
            _enemySpawnTimer = now;
            auto id = *_enemyIds.begin();
            _enemyIds.erase(id);
            auto position = Vector2{11, (float)rand() / RAND_MAX * 8 - 4};
            auto velocity = Vector2{-3, 0};
            _enemies.emplace_back(id, position, velocity, 2);
            sendAll("spawn:" + std::to_string(id) + "," + vectorToString(position) + "," + vectorToString(velocity) + ",2");
        }
    }
}
