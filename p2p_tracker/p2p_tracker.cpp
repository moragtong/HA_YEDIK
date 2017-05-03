// p2p_tracker.cpp : Defines the entry point for the console application.
//
#include <util.h>
#include <thread>
#include <vector>
#include <forward_list>
#include <algorithm>
using namespace util;
using namespace socket;
const char * enum_repr[]{ "ADD", "REMOVE", "LIST" };
/**
 * \class p2p_tracker
 * \brief Coordinates between clients.
*/
struct p2p_tracker : protected p2p_socket {
	p2p_tracker() {}
	/**
	 * \brief	Initializes the tracker and starts it.
	 * \param	starter	first client to seed.
	*/
	void startall(util::sockaddr starter) {
		init();
		start(starter);
	}
	/**
	 * \brief	Starts the tracker
	 * Sends an empty packet until a confirmation (another empty packet) is received.
	 * Receives commands according to the protocol and acts accordingly.
	 * \param	starter	first client to seed.
	*/
	void start(const util::sockaddr& starter) {
		{
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
/**
 * \class download
 * Represents a tracker running on a seperate thread.
*/
struct download {
	std::thread thread;
	p2p_tracker tracker;
	download(const util::sockaddr& starter)
		: tracker(), thread(&p2p_tracker::startall, &tracker, starter) {
	}
};
/**
 * \brief	A default tracker that launches other trackers.
 * Holds a list of downloads. Uses UDP/IP.
 * Launches other trackers in a loop.
*/
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