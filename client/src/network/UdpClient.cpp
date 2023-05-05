/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** UdpClient.cpp
*/

#include "network/UdpClient.hpp"

/**
 * It creates a UDP socket, binds it to a given port and starts the client
 * @param ip the ip address of the server
 * @param port The port to listen on.
 */
UdpClient::UdpClient(const std::string &ip, unsigned short port)
    : _socket(_io_context, udp::endpoint(udp::v4(), 0)), _endpoint(address::from_string(ip), port) {}

/**
 * It stops the client
 * It sends a disconnect message to the server, then closes the socket
 */
void UdpClient::stop() {
    if (!isConnected())
        return;
    send("disconnect");
    _connected = false;
    _stop = true;
    _socket.cancel();
    if (_listener.joinable())
        _listener.join();
    if (_heartbeat.joinable())
        _heartbeat.join();
    if (_timeout.joinable())
        _timeout.join();
    if (_socket.is_open())
        _socket.close();
    std::cout << "Udp client stopped" << std::endl
}

/**
 * It sets the ip and port of the endpoint
 *
 * @param ip The ip address of the endpoint.
 * @param port The port of the endpoint.
 */
void UdpClient::setEndpoint(const std::string &ip, unsigned short port) { _endpoint = udp::endpoint(address::from_string(ip), port); }

/**
 * It starts the client
 * It starts the different threads and sends a connect message to the server
 */
void UdpClient::start() {
    _stop = false;
    if (!_socket.is_open())
        _socket.open(udp::v4());
    _listener = std::thread([this]() { threadListener(); });
    _heartbeat = std::thread([this]() { threadHeartbeat(); });
    _timeout = std::thread([this]() { threadTimeout(); });
    std::cout << "Udp client started" << std::endl;
}

/**
 * It stops the client then prints a message
 */
UdpClient::~UdpClient() {
    stop();
    std::cout << "Udp client stopped" << std::endl;
}

/**
 * It sends a message to the server
 *
 * @param msg The message to send.
 */
void UdpClient::send(const std::string &msg) {
    std::error_code ec;
    _socket.send_to(asio::buffer(msg), _endpoint, 0, ec);
    if (ec) {
        std::cerr << "Error while sending message: " << ec.message() << std::endl;
    }
}

/**
 * It waits until there's a message in the queue, then returns it
 *
 * @return A string
 */
std::string UdpClient::receive() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this]() { return !_messages.empty(); });
    std::string msg = _messages.front();
    _messages.pop();
    return msg;
}

/**
 * If the messages queue is not empty, return true.
 *
 * @return A boolean value.
 */
bool UdpClient::hasMessage() {
    std::lock_guard<std::mutex> lock(_mutex);
    return !_messages.empty();
}

/**
 * It returns the value of the private variable _connected.
 *
 * @return A boolean value.
 */
bool UdpClient::isConnected() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _connected;
}

/**
 * It listens for incoming messages and stores them in a queue
 */
void UdpClient::threadListener() {
    // Set socket to non-blocking mode
    _socket.non_blocking(true);

    while (!_stop) {
        std::error_code ec;
        std::size_t bytes_recvd = _socket.receive_from(asio::buffer(_buffer), _endpoint, 0, ec);
        if (ec == asio::error::operation_aborted || ec == asio::error::bad_descriptor) {
            break;
        }
        if (ec == asio::error::would_block) {
            // No data received, do something else or sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        if (ec) {
            std::cerr << "Error while receiving message: " << ec.message() << std::endl;
            continue;
        }
        std::string msg(_buffer.data(), bytes_recvd);
        std::lock_guard<std::mutex> lock(_mutex);
        _messages.push(msg);
        _lastMessage = std::chrono::steady_clock::now();
        _connected = !_stop;
        _cv.notify_one();
    }
}

/**
 * It sends a "ping" message to the server every 3 seconds
 */
void UdpClient::threadHeartbeat() {
    while (!_stop) {
        send("ping");
        std::this_thread::sleep_for(3s);
    }
}

/**
 * "If the last message was received more than 3 seconds ago, set the connected flag to false."
 *
 * The first thing we do is sleep for 5 seconds. This is to prevent the thread from constantly checking
 * the time
 */
void UdpClient::threadTimeout() {
    while (!_stop) {
        std::this_thread::sleep_for(5s);
        if (std::chrono::steady_clock::now() - _lastMessage > 3s) {
            std::cout << "Server timed out" << std::endl;
            std::lock_guard<std::mutex> lock(_mutex);
            _connected = false;
        }
    }
}

void UdpClient::setPort(unsigned short port) {
    stop();
    // wait for the threads to stop
    std::this_thread::sleep_for(200ms);
    setEndpoint(_endpoint.address().to_string(), port);
    start();
}
