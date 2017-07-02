#pragma once
namespace Socket::TCP {
	struct Server : detail::TCPBase {
		using SocketBase::Bind;
		int Listen(int nQueueWait = 5);
		Client Accept();
	};
}

