// p2p_tracker.cpp : Defines the entry point for the console application.
//
#include <util.h>
#include <vector.h>
#include <algorithm>
namespace p2p_tracker {
	enum trk_com_enum { ADD, REMOVE, LIST };
	struct trk_com {
		trk_com_enum command;
		//unsigned long param = 0;
	};
	void init() {
		socket::init_winsock();
		const char * enum_repr[]{ "ADD", "REMOVE", "LIST" };
		trk_com com;
		etl::vector<util::sockaddr, 128> clients;
		socket::SOCKET sock;
		util::sockaddr addr({ 127, 0, 0, 1 }, 16673);
		sock = util::udp_sock();
		std::cout << bind(sock, &addr, sizeof(addr));
		for (;;) {
			util::sockaddr client;
			int fromlen = sizeof(client);
			socket::recvfrom(sock, (char*)&com, sizeof(com), 0, &client, &fromlen);
			std::cout << socket::WSAGetLastError();
			if (com.command <= REMOVE) {
				if (com.command == ADD) {
					clients.push_back(client);
					clients.back().port(clients.back().port() + 1);
				} else //if (com.command == REMOVE)
					clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
				socket::sendto(sock, (char*)&com.command, sizeof(com.command), 0, &client, sizeof(client));
			} else //if (com.command == LIST)
				socket::sendto(sock, (char*)clients.data(), clients.size() * sizeof(util::sockaddr), 0, &client, sizeof(client));
			puts(enum_repr[com.command]);
		}
	}
}
int main() {
	p2p_tracker::init();
	return 0;
}
