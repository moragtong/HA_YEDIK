#include "..\stdafx.h"
#include "..\SocketBase.h"
#include "..\TCPBase.h"
#include "Client.h"
#include "Server.h"
namespace Socket::TCP {
	int Server::Listen(int nQueueWait) {
		return ::listen(sock, nQueueWait);
	}

	Client Server::Accept() {
		Client pSockClient;
		SOCKET sock_temp = ::accept(sock, nullptr, nullptr);
		if (sock_temp != INVALID_SOCKET)
			pSockClient.SetSOCK(sock_temp);
		return pSockClient;
	}
}
