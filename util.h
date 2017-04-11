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
#ifdef WIN32
#include <WinSock2.h>
	WSADATA init_winsock() {
		WSADATA wsaData;
		socket::WSAStartup(MAKEWORD(2, 2), &wsaData);
		return wsaData;
	}
#else
#include <sys/socket.h>
#endif
}
namespace util {
	class ip4addr {
	private:
		char _addr[4];
	public:
		ip4addr(char num1, char num2, char num3, char num4)
			: _addr{ num1, num2, num3, num4 } {
		}
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
		void _port(unsigned short val) {
			((socket::sockaddr_in*)this)->sin_port = val;
		}
		sockaddr& port(unsigned short _port) {
			((socket::sockaddr_in*)this)->sin_port = socket::htons(_port);
			return *this;
		}
		bool operator==(sockaddr const& other) const {
			return family() == other.family() && addr() == other.addr() && _port() == other._port();
		}
	};
	struct p2p_socket {
		socket::SOCKET sock;
		int settimeout(unsigned long sec, unsigned long usec) {
			#ifdef _WIN32
			const unsigned long tv = sec * 1000 + usec;
			#else
			const timeval tv{ sec, usec };
			#endif
			auto v = socket::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
			std::cout << "setsockopt(" << sec << ", " << usec << ") -> " << v << '\n';
			return v;
		}
	};
}