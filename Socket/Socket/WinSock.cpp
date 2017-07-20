#include "WinSock.h"

namespace Socket {
	void WSInit() {
#ifdef _WIN32
		::WSADATA wsaData;
		::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	}

	void WSDeInit() {
#ifdef _WIN32
		::WSACleanup();
#endif
	}
}