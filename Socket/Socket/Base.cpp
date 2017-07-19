#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "Base.hpp"

bool operator==(const in_addr &first, const in_addr &second) {
	return (unsigned long &)first == (unsigned long &)second;
}

bool operator!=(const in_addr &first, const in_addr &second) {
	return !(first == second);
}

bool operator==(const sockaddr_in &first, const sockaddr_in &second) {
	return first.sin_addr == second.sin_addr && first.sin_family == second.sin_family && first.sin_port == second.sin_port;
}

bool operator!=(const sockaddr_in &first, const sockaddr_in &second) {
	return !(first == second);
}

namespace Socket::detail {
	Base::_wsinit::_wsinit() {
		Socket::WSInit();
	}

	Base::_wsinit::~_wsinit() {
		::WSACleanup();
	}

	Base::Base(::SOCKET _sock)
		: m_sock(_sock) {
	}

	Base::Base(Base &&other) 
		: m_sock(other.m_sock), m_sock_info(other.m_sock_info) {
		other.m_sock = INVALID_SOCKET;
	}

	Base& Base::operator=(Base &&other) {
		m_sock = other.m_sock;
		m_sock_info = other.m_sock_info;
		other.m_sock = INVALID_SOCKET;
		return *this;
	}

	std::error_code Base::Bind(unsigned short port, ::in_addr addr) {
		m_sock_info.sin_family = AF_INET;
		m_sock_info.sin_port = htons(port);
		(unsigned long &)m_sock_info.sin_addr = htonl((unsigned long &)addr);
		auto retbind = ::bind(m_sock, (sockaddr *)&m_sock_info, sizeof(m_sock_info));
		int infolen = sizeof(m_sock_info);
		getsockname(m_sock, (sockaddr *)&m_sock_info, &infolen);
		return { retbind, std::system_category() };
	}

	const ::sockaddr_in& Base::GetInfo() const {
		return m_sock_info;
	}

	std::error_code Base::Close() {
		return { ::
#ifdef _WIN32
				closesocket
#else
				close
#endif	
				(m_sock), std::system_category()
		};
	}

	std::error_code Base::SetNonBlockingMode(bool bBlockingMode) {
#ifdef _WIN32
		unsigned long nFlag;
		if (bBlockingMode) {
			nFlag = 1; //for non-blocking mode
		} else {
			nFlag = 0; //for blocking mode
		}
		return { ::ioctlsocket
#else
			int nFlag;
#ifdef O_NONBLOCK
			if (-1 == (nFlag = ::fcntl(sock, F_GETFL, 0))) {
				nFlag = 0;
			}
			return ::fcntl(sock, F_SETFL, nFlag | O_NONBLOCK);
#else
			nFlag = 1;
			return { ::ioctl
#endif
#endif
			(m_sock, FIONBIO, &nFlag), std::system_category() };
	}

	std::error_code Base::SetTimeout(unsigned long sec, unsigned long usec) {
#ifdef _WIN32
		const unsigned long tv = sec * 1000 + usec;
#else
		const timeval tv{ sec, usec };
#endif
		return {
			::setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)),
			std::system_category()
		};
	}

	bool Base::IsValid() const {
		return INVALID_SOCKET != m_sock;
	}

	Base::~Base() {
		if (IsValid())
			Close();
	}

	std::error_code Base::GetLastError() {
		return {
#ifdef _WIN32
			WSAGetLastError()
#else
			errno
#endif
			, std::system_category() };
	}
}