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
		return ::accept(sock, nullptr, nullptr);
	}
}
