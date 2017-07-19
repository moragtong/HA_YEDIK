#include "WinSock.h"
#ifdef _WIN32
namespace Socket {
	void WSInit() {
		::WSADATA wsaData;
		::WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	void WSDeInit() {
		::WSACleanup();
	}
}
#endif