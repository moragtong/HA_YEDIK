#include "stdafx.h"
#include "SocketBase.h"
#include "TCPBase.h"
namespace Socket::detail {
	bool TCPBase::Create() {
		sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return INVALID_SOCKET != sock;
	}

	int TCPBase::Send(void const * pBuff, size_t nBuffSize, int nFlags) const {
		return ::send(sock, (const char*)pBuff, nBuffSize, nFlags);
	}

	int TCPBase::Recv(void * pBuff, size_t nBuffSize, int nFlags) {
		return ::recv(sock, (char*)pBuff, nBuffSize, nFlags);
	}
}