#pragma once
namespace Socket::TCP {
	class Client : public detail::TCPBase {
	protected:
		using detail::TCPBase::TCPBase;
	public:
		friend class Server;
		int Connect(char const *pszAddr, unsigned short int nPort);
	};
}