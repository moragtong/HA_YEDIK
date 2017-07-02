#include "..\stdafx.h"
#include "..\SocketBase.h"
#include "..\TCPBase.h"
#include "Client.h"

namespace Socket::TCP {
	int Client::Connect(char const *pszAddr, unsigned short int nPort) {
		auto hp = ::gethostbyname(pszAddr);
		::memcpy(&sock_info.sin_addr, hp->h_addr, hp->h_length);
		sock_info.sin_family = AF_INET;
		sock_info.sin_port = ::htons(nPort);
		return ::connect(sock, (sockaddr*)&sock_info, sizeof(sock_info));
	}
}