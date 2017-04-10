// p2p_tracker.cpp : Defines the entry point for the console application.
//
#include <util.h>
#include <vector>
#include <algorithm>
using namespace socket;
namespace p2p_tracker {
	enum trk_com_enum : char { ADD, REMOVE, NEW, LIST };
	struct trk_com {
		trk_com_enum command;
		unsigned long param;
	};
	void init() {
		socket::init_winsock();
		const char * enum_repr[]{ "ADD", "REMOVE", "NEW", "LIST" };
		trk_com com;
		std::vector<std::vector<util::sockaddr>> downloads;
		SOCKET sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
		util::sockaddr addr("127.0.0.1", 16673);
		std::cout << bind(sock, &addr, sizeof(addr));
		for (;;) {
			util::sockaddr client;
			int fromlen = sizeof(client);
			recvfrom(sock, (char*)&com, sizeof(com), 0, &client, &fromlen);
			std::cout << socket::WSAGetLastError();
			unsigned long ret;
			if (com.command == LIST)
				sendto(sock, (char*)downloads[com.param].data(), downloads[com.param].size() * sizeof(util::sockaddr), 0, &client, sizeof(client));
			else {
				switch (com.command) {
					case ADD:
						if (ret = com.param < downloads.size()) {
							downloads[com.param].push_back(client);
							downloads[com.param].back().port(downloads[com.param].back().port() + 1);
						}
						break;
					case REMOVE:
						downloads[com.param].erase(std::remove(downloads[com.param].begin(), downloads[com.param].end(), client), downloads[com.param].end());
						ret = 1;
						break;
					case NEW:
						ret = downloads.size();
						downloads.push_back(std::vector<util::sockaddr>{client});
				}
				sendto(sock, (char*)&ret, sizeof(ret), 0, &client, sizeof(client));
			}
			puts(enum_repr[com.command]);
		}
	}
}
int main() {
	p2p_tracker::init();
	return 0;
}

