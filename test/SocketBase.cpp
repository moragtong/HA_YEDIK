#include "stdafx.h"
#include "SocketBase.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif
namespace Socket::detail {
#ifdef _WIN32
	SocketBase::_wsinit::_wsinit() {
		::WSADATA wsaData;
		::WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	SocketBase::_wsinit::~_wsinit() {
		::WSACleanup();
	}
#endif
	SocketBase::SocketBase() {
		sock = INVALID_SOCKET;
		::memset(&sock_info, 0, sizeof(sock_info));
	}

	void SocketBase::SetSOCK(SOCKET Sock) {
		if (Sock > 0) {
			sock = Sock;
		}
	}

	int SocketBase::Bind(unsigned short int nPort) {
		sock_info.sin_family = AF_INET;
		sock_info.sin_port = htons(nPort);
		sock_info.sin_addr.s_addr = htonl(INADDR_ANY);
		return ::bind(sock, (sockaddr*)&sock_info, sizeof(sock_info));
	}

	void SocketBase::CloseSocket() {
		if (sock != INVALID_SOCKET) {
#ifdef _WIN32
			::closesocket(sock);
#else
			::close(sock);
#endif	
		}
		sock = INVALID_SOCKET;
		::memset(&sock_info, 0, sizeof(sock_info));
	}

	int SocketBase::SetNonBlockingMode(bool bBlockingMode) {
#ifdef _WIN32
		unsigned long nFlag;
		if (bBlockingMode) {
			nFlag = 1; //for non-blocking mode...
		}
		else {
			nFlag = 0; //for blocking mode...
		}
		return ::ioctlsocket(sock, FIONBIO, &nFlag);
#else
		int nFlag;
#if defined(O_NONBLOCK)
		if (-1 == (nFlag = ::fcntl(sock, F_GETFL, 0))) {
			nFlag = 0;
		}
		return ::fcntl(sock, F_SETFL, nFlag | O_NONBLOCK);
#else
		nFlag = 1;
		return ::ioctl(sock, FIONBIO, &nFlag);
#endif
#endif
	}

	int SocketBase::SetTimeout(unsigned long sec, unsigned long usec) {
#ifdef _WIN32
		const unsigned long tv = sec * 1000 + usec;
#else
		const timeval tv{ sec, usec };
#endif
		return ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
	}

	bool SocketBase::IsValidSocket() {
		return INVALID_SOCKET != sock;
	}

	SocketBase::~SocketBase() {
		CloseSocket();
	}

	int SocketBase::GetLastError() {
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno
#endif
	}
}