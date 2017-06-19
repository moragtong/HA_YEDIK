// socket.cpp
//

#include "stdafx.h"
#include "resource.h"
#include "socket.h"
#ifdef _WIN32 // _WIN32 is defined by many compilers available for the Windows operating system, but not by others.
#pragma comment(lib, "Ws2_32.lib")
#include <Ws2tcpip.h>
#define WIN_SOCK_DECLARE		WSADATA wsaData;

//********* WIN_SOCK_INIT & WIN_SOCK_SHUTDOWN should be called for each thread
#define WIN_SOCK_INIT			WSAStartup(MAKEWORD(2,2), &wsaData); 
#define WIN_SOCK_SHUTDOWN		WSACleanup();
//*****************************************************************************

#define GET_LAST_SOCKET_ERROR	WSAGetLastError();
#define socklen_t		int 

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

#define WIN_SOCK_DECLARE
#define WIN_SOCK_INIT
#define WIN_SOCK_SHUTDOWN

//	#define GET_LAST_SOCKET_ERROR	errno;
#define SOCKET 			int
#define INVALID_SOCKET		(-1)

#endif
namespace Socket {
	namespace detail {
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
			} else {
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
		bool TCP::Create() {
			sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			return INVALID_SOCKET != sock;
		}
		int TCP::Send(void const * pBuff, size_t nBuffSize, int nFlags) const {
			return ::send(sock, (const char*)pBuff, nBuffSize, nFlags);
		}
		int TCP::Recv(void * pBuff, size_t nBuffSize, int nFlags) {
			return ::recv(sock, (char*)pBuff, nBuffSize, nFlags);
		}
	}
	int TCPClient::Connect(char const * pszAddr, unsigned short int nPort) {
		auto hp = ::gethostbyname(pszAddr);
		::memcpy(&sock_info.sin_addr, hp->h_addr, hp->h_length);
		sock_info.sin_family = AF_INET;
		sock_info.sin_port = ::htons(nPort);
		return ::connect(sock, (sockaddr*)&sock_info, sizeof(sock_info));
	}
	int TCPServer::Listen(int nQueueWait) {
		return ::listen(sock, nQueueWait);
	}
	TCPClient TCPServer::Accept() {
		TCPClient pSockClient;
		SOCKET sock_temp = ::accept(sock, nullptr, nullptr);
		if (sock_temp != INVALID_SOCKET)
			pSockClient.SetSOCK(sock_temp);
		return pSockClient;
	}
	bool UDP::Create() {
		sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		return INVALID_SOCKET != sock;
	}
	int UDP::RecvFrom(void * pBuff, size_t nBuffSize, sockaddr_in *client_addr, int nFlag) {
		socklen_t nLen = sizeof(client_addr);
		return ::recvfrom(sock, (char*)pBuff, nBuffSize, nFlag, (::sockaddr *)client_addr, &nLen);
	}
	int UDP::SendTo(void * pBuff, size_t nBuffSize, const sockaddr_in &client_addr, int nFlag) {
		return ::sendto(sock, (const char*)pBuff, nBuffSize, nFlag, (const ::sockaddr *)&client_addr, sizeof(client_addr));
	}
}
#undef LZZ_INLINE
