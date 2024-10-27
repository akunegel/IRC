#include "../inc/IRC.hpp"

void exitWithError(const std::string& message) {
    std::cerr << message << std::endl;
    exit(0);
}

void set_socket_non_blocking(int socket) {
    if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
        exitWithError("Error: Could not set socket to non-blocking");
    }
}


void send_message(int client_socket, const std::string& message) {
    send(client_socket, message.c_str(), message.length(), 0);
}