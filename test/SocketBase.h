#pragma once
#ifdef _WIN32
using socklen_t = int;
#else
using SOCKET = int;
enum { INVALID_SOCKET = -1 };
#endif
namespace Socket::detail {
	class SocketBase {
#ifdef _WIN32
		//static ctor
		static struct _wsinit {
			_wsinit();
			~_wsinit();
		} _wsinit;
#endif
		SocketBase(const SocketBase &) = delete;
		SocketBase& operator=(const SocketBase &) = delete;

	protected:
		::SOCKET sock;
		::sockaddr_in sock_info;
		SocketBase();
		SocketBase(::SOCKET Sock);
		int Bind(unsigned short int nPort);

	public:
		SocketBase(SocketBase &&) = default;
		SocketBase& operator=(SocketBase &&) = default;
		void Close();
		int SetNonBlockingMode(bool bBlockingMode);
		int SetTimeout(unsigned long sec, unsigned long usec = 0);
		bool IsValid();
		~SocketBase();

		static int GetLastError();
	};
}

