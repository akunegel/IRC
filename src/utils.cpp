#include "../inc/IRC.hpp"

void exitWithError(const std::string& message) {
    std::cerr << message << std::endl;
    exit(EXIT_FAILURE);
}

void set_socket_non_blocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        exitWithError("Error: Could not get socket flags");
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        exitWithError("Error: Could not set socket to non-blocking");
    }
}

void send_message(int client_socket, const std::string& message) {
    send(client_socket, message.c_str(), message.length(), 0);
}