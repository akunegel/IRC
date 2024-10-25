#include "../inc/IRC.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "Error: Format is: ./ircserv <port> <password>." << std::endl;
		return 0;
	}
	
	t_data data;
	data = parse_args(argv[2], argv[1]);

	run_server(&data);
	return 0;
}