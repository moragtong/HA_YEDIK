// socket.h
//
#pragma once
#ifndef LZZ_socket_h
#define LZZ_socket_h
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
		int Connect(char const *pszAddr, unsigned short int nPort);
	};
	struct TCPServer : detail::TCP {
		using SocketBase::Bind;
		int Listen(int nQueueWait = 5);
		TCPClient Accept();
	};
	struct UDP : detail::SocketBase {
		using SocketBase::Bind;
		bool Create();
		int RecvFrom(void *pBuff, size_t nBuffSize, ::sockaddr_in *client_addr, int nFlag = 0);
		int SendTo(void *pBuff, size_t nBuffSize, const ::sockaddr_in &client_addr, int nFlag = 0);
	};
}
#undef LZZ_INLINE
#endif
