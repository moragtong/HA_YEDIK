// p2p_tracker.cpp : Defines the entry point for the console application.
//
#include <util.h>
#include <thread>
#include <vector>
#include <forward_list>
#include <algorithm>
using namespace util;
using namespace socket;
enum { CLN_NUM = 32, TIMEOUT = 1500 };
enum trk_com_enum : char { ADD, REMOVE, LIST };
struct trk_com {
	trk_com_enum command;
	//unsigned long param;
};
const char * enum_repr[]{ "ADD", "REMOVE", "LIST" };
struct p2p_tracker : protected p2p_socket {
	p2p_tracker() {}
	void startall(util::sockaddr starter) {
		init();
		start(starter);
	}
	unsigned short init() {
		sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
		util::sockaddr addr({ 0,0,0,0 }, 0);
		std::cout << bind(sock, &addr, sizeof(addr));
		int p = sizeof(addr);
		getsockname(sock, &addr, &p);
		std::cout << "tracker sock port: " << addr.port() << '\n';
		return addr._port();
	}
	void start(const util::sockaddr& starter) {
		{
			settimeout(0, TIMEOUT);
			char count = 4;
			int status;
			do {
				sendto(sock, 0, 0, 0, &starter, sizeof(starter));
				status = recvfrom(sock, 0, 0, 0, 0, 0);
				count--;
			} while (status < 0 && count);
			settimeout(0, 0);
		}
		trk_com com;
		std::vector<util::sockaddr> clients{ starter };
		util::sockaddr client;
		int fromlen = sizeof(client);
		for (unsigned short i = 0;; i = (i + 1) % clients.size()) {
			if (clients.size() - i < CLN_NUM)
				i = 0;
			recvfrom(sock, (char*)&com, sizeof(com), 0, &client, &fromlen);
			auto wsc = socket::WSAGetLastError();

			std::cout << wsc << ' ';
			if (com.command == LIST)
				sendto(sock, (char*)(clients.data() + i), min(CLN_NUM, clients.size()) * sizeof(util::sockaddr), 0, &client, sizeof(client));
			else {
				unsigned short ret = 1;
				switch (com.command) {
					case ADD:
						clients.push_back(client);
						break;
					case REMOVE:
						clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
						ret = 1;
						break;
					default:
						ret = 0;
				}
				sendto(sock, (char*)&ret, sizeof(ret), 0, &client, sizeof(client));
			}
			puts(enum_repr[com.command]);
		}
	}
};
struct download {
	std::thread thread;
	p2p_tracker tracker;
	download(const util::sockaddr& starter)
		: tracker(), thread(&p2p_tracker::startall, &tracker, starter) {
	}
};
int main() {
	init_winsock();
	std::forward_list<download> trackers;
	auto sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
	util::sockaddr addr({ 0,0,0,0 }, 16673);
	std::cout << "main: " << bind(sock, &addr, sizeof(addr)) << '\n';
	util::sockaddr client;
	int fromlen = sizeof(client);
	for (;;) {
		recvfrom(sock, 0, 0, 0, &client, &fromlen);
		trackers.emplace_front(client);
	}
	return 0;
}