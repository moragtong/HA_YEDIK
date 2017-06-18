// socket.h
//
#pragma once
#ifndef LZZ_socket_h
#define LZZ_socket_h
#ifdef _WIN32 // _WIN32 is defined by many compilers available for the Windows operating system, but not by others.
#pragma comment(lib, "Ws2_32.lib")
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
		class SocketBase {
			SocketBase(const SocketBase &) = delete;
			SocketBase& operator=(const SocketBase &) = delete;
		protected:
			::SOCKET sock;
			::sockaddr_in sock_info;
			SocketBase();
			void SetSOCK(::SOCKET Sock);
			int Bind(unsigned short int nPort);
		public:
			SocketBase(SocketBase &&) = default;
			SocketBase& operator=(SocketBase &&) = default;
			void CloseSocket();
			int SetNonBlockingMode(bool bBlockingMode);
			int SetTimeout(unsigned long sec, unsigned long usec);
			bool IsValidSocket();
			~SocketBase();
		};
		class TCP : public SocketBase {
		protected:
			TCP() = default;
		public:
			using SocketBase::SetSOCK;
			bool Create();
			int Send(void const *pBuff, size_t nBuffSize, int nFlags = 0) const;
			int Recv(void *pBuff, size_t nBuffSize, int nFlags = 0);
		};
	}
	struct TCPClient : detail::TCP {
		int Connect(char const * pszAddr, unsigned short int nPort);
	};
	struct TCPServer : detail::TCP {
		using SocketBase::Bind;
		int Listen(int nQueueWait = 5);
		TCPClient Accept();
	};
	struct UDP : detail::SocketBase {
		using SocketBase::Bind;
		bool Create();
		int RecvFrom(void * pBuff, size_t nBuffSize, ::sockaddr_in *client_addr, int nFlag = 0);
		int SendTo(void * pBuff, size_t nBuffSize, ::sockaddr_in &client_addr, int nFlag = 0);
	};
}
#undef LZZ_INLINE
#endif
