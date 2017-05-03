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
enum { CLN_NUM = 32, TIMEOUT = 1500 };
enum trk_com_enum : char { ADD, REMOVE, LIST };
struct trk_com {
	trk_com_enum command;
	//unsigned long param;
};
namespace socket {
#ifdef WIN32
#include <WinSock2.h>
	/**
	 * \brief	Initializes WinSock.
	*/
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
	/**
	 * \class ip4addr
	 * \brief	IPV4 address
	 * Represents an IPV4 address.
	*/
	struct ip4addr {
		char _addr[4];
		ip4addr() {}
		ip4addr(unsigned long raw) {
			(unsigned long&)*_addr = raw;
		}
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
		/**
		 * \brief	Initializes the client.
		 * Initializes the socket to UDP/IP.
		 * Binds the socket to a random port and prints it for debug purposes.
		*/
		void init() {
			sock = socket::socket(AF_INET, SOCK_DGRAM, 0);
			util::sockaddr addr({ 0,0,0,0 }, 0);
			std::cout << bind(sock, &addr, sizeof(addr));
			int p = sizeof(addr);
			getsockname(sock, &addr, &p);
			std::cout << "sock port: " << addr.port() << '\n';
			settimeout(0, TIMEOUT);
		}
		/**
		 * \brief	Sets the timeout.
		 * Sets the timeout of the socket using the native API.
		 * \param	sec	the seconds
		 * \param	usec	the miliseconds
		*/
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