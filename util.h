#pragma once
#include <iostream>
#include <thread>
/*These two are included with MSVC Runtime.
Soon I'll move to MinGW, and I'll be able to port the
project to Linux (sys/socket.h instead of winsock2.h)
and maybe maybe maybe to Mac*/
#ifndef WIN32
#include <cstring>
#include <cstdlib>
#endif

//#include <bitset>
namespace socket {
#ifdef _WIN32
		#include <WinSock2.h>
	WSADATA init_winsock() {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		return wsaData;
	}
	#else
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <sys/types.h>
		#include <time.h>
		#include <sys/ioctl.h>
	#endif
}
namespace util {
	class ip4addr {
	private:
		char _addr[4];
	public:
		ip4addr(char num1, char num2, char num3, char num4)
			: _addr{num1, num2, num3, num4} {}
		operator long() {
			return *((long*)_addr);
		}
	};
	struct sockaddr : socket::sockaddr {
		sockaddr() {
		}
		sockaddr(ip4addr _addr, unsigned short _port) {
			family(AF_INET).addr(_addr).port(_port);
		}
		auto family() const {
			return ((socket::sockaddr_in*)this)->sin_family;
		}
		sockaddr& family(socket::ADDRESS_FAMILY a) {
			((socket::sockaddr_in*)this)->sin_family = AF_INET;
			return *this;
		}
		auto addr() const {
			return ((socket::sockaddr_in*)this)->sin_addr.S_un.S_addr;
		}
		sockaddr& addr(ip4addr _addr) {
			((socket::sockaddr_in*)this)->sin_addr.S_un.S_addr = _addr;
			return *this;
		}
		auto port() const {
			return socket::ntohs(((socket::sockaddr_in*)this)->sin_port);
		}
		auto _port() const {
			return ((socket::sockaddr_in*)this)->sin_port;
		}
		sockaddr& port(unsigned short _port) {
			((socket::sockaddr_in*)this)->sin_port = socket::htons(_port);
			return *this;
		}
		bool operator==(sockaddr const& other) const {
			return family() == other.family() && addr() == other.addr() && _port() == other._port();
		}
	};
	/*void recvfrom_s(socket::SOCKET s, char* buff, int size, int flags = 0, socket::sockaddr* from = 0, int len = 0) {
		int check;
		do
			check = socket::recvfrom(s, buff, size, flags, from, len ? &len : 0);
		while (check == -1);
	}*/
}