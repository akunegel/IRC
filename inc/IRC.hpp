#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <map>
#include <cerrno>
#include <vector>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 512

typedef struct Client {
    int socket;
    bool authenticated;
    std::string nickname;
    std::string username;

    Client() : socket(-1), authenticated(false), nickname(""), username("") {}
} t_client;

typedef struct s_data {
    int port;
    std::string password;
    std::map<int, t_client> clients;
    std::map<std::string, std::vector<int> > channels;
} t_data;


t_data parse_args(std::string password, std::string port);
void run_server(t_data *data);
void exitWithError(const std::string& message);
void set_socket_non_blocking(int socket);
void send_message(int client_socket, const std::string& message);
void process_command(int client_socket, const std::string& command, t_data* data);