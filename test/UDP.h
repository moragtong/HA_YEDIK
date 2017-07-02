#pragma once
namespace Socket {
	struct UDP : detail::SocketBase {
		using SocketBase::Bind;
		bool Create();
		int RecvFrom(void *pBuff, size_t nBuffSize, ::sockaddr_in *client_addr, int nFlag = 0);
		int SendTo(void *pBuff, size_t nBuffSize, const ::sockaddr_in &client_addr, int nFlag = 0);
	};
}
