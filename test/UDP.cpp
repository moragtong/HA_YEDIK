#include "stdafx.h"
#include "SocketBase.h"
#include "UDP.h"
namespace Socket {
	bool UDP::Create() {
		sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		return INVALID_SOCKET != sock;
	}

	int UDP::RecvFrom(void *pBuff, size_t nBuffSize, sockaddr_in *client_addr, int nFlag) {
		::socklen_t nLen = sizeof(client_addr);
		return ::recvfrom(sock, (char*)pBuff, nBuffSize, nFlag, (::sockaddr *)client_addr, &nLen);
	}

	int UDP::SendTo(void *pBuff, size_t nBuffSize, const sockaddr_in &client_addr, int nFlag) {
		return ::sendto(sock, (const char*)pBuff, nBuffSize, nFlag, (const ::sockaddr *)&client_addr, sizeof(client_addr));
	}
}
