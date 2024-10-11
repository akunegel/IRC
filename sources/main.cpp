#include "../includes/IRC.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Error: Wrong Format: ./ircserv <port> <password> expected." << std::endl;
		return 0;
	}

	int portNumber;
	std::string serverPassword;

	validateArguments(argv[1], argv[2], portNumber, serverPassword);

	int servSocket = createServerSocket(portNumber);
	runServerLoop(servSocket);

	return 0;
}