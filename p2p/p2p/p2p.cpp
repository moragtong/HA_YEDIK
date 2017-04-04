#include "p2p_client.h"
int main() {
	p2p_client::p2p_client client;
	char command[16];
	//for (;;) {
		std::memset(command, 0, sizeof(command));
		std::cin >> command;
		if (!strcmp(command, "remove"))
			client.command_tracker(p2p_client::REMOVE);
		else if (!strcmp(command, "recv")) {
			client.request_list();
			client.request_filedata();
			client.recv_file();
			client.command_tracker(p2p_client::ADD);
		} else if (!strcmp(command, "add")) {
			std::cin >> client.f;
			std::cout << command << ' ' << client.f.name << '\n';
			client.command_tracker(p2p_client::ADD);
		} else
			std::puts("invalid input");
	//}
	std::puts("done");
	return 0;
}