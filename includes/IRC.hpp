#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>

#define BUFFER_SIZE 1024

int createServerSocket(int port);
void validateArguments(char *port, char *password, int &portNumber, std::string &serverPassword);
void runServerLoop(int servSocket);