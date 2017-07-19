#include "UDP.hpp"

namespace Socket {
	bool UDP::Create() {
		m_sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		return m_sock != INVALID_SOCKET;
	}

	::std::tuple<int, ::sockaddr_in> UDP::RecvFrom(void *buff, size_t size, int flag) {
		::std::tuple<int, ::sockaddr_in> client;
		::socklen_t len = sizeof(std::get<1>(client));
		std::get<0>(client) = ::recvfrom(m_sock, (char*)buff, size, flag, (::sockaddr *)&std::get<1>(client), &len);
		return client;
	}

	int UDP::SendTo(void *buff, size_t size, const sockaddr_in &client_addr, int flag) {
		return ::sendto(m_sock, (const char *)buff, size, flag, (const ::sockaddr *)&client_addr, sizeof(client_addr));
	}
}