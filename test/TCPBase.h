#pragma once
namespace Socket::detail {
	class TCPBase : public SocketBase {
	protected:
		using SocketBase::SocketBase;

	public:
		bool Create();
		int Send(void const *pBuff, size_t nBuffSize, int nFlags = 0) const;
		int Recv(void *pBuff, size_t nBuffSize, int nFlags = 0);
	};
}

