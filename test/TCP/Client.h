#pragma once
namespace Socket::TCP {
	struct Client : detail::TCPBase {
		int Connect(char const *pszAddr, unsigned short int nPort);
	};
}