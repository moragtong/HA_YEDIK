#pragma once
#include <system_error>
#ifdef _WIN32
#include <WinSock2.h>
using socklen_t = int;
#else
using SOCKET = unsigned int;
enum { INVALID_SOCKET = -1 };
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif
#include "WinSock.hpp"
bool operator==(const in_addr &, const in_addr &);
bool operator!=(const in_addr &, const in_addr &);
bool operator==(const sockaddr_in &, const sockaddr_in &);
bool operator!=(const sockaddr_in &, const sockaddr_in &);
namespace Socket::detail {
	class Base {
#ifdef _WIN32
		//static ctor
		static struct _wsinit {
			_wsinit();
			~_wsinit();
		} _wsinit;
#endif
		Base(const Base &) = delete;
		Base& operator=(const Base &) = delete;

	protected:
		::SOCKET m_sock;
		::sockaddr_in m_sock_info;
		Base(::SOCKET = INVALID_SOCKET);
		std::error_code Bind(unsigned short = 0, ::in_addr = { 0 });

	public:
		Base(Base &&);
		Base& operator=(Base &&);
		const ::sockaddr_in& GetInfo() const;
		std::error_code Close();
		std::error_code SetNonBlockingMode(bool);
		std::error_code SetTimeout(unsigned long, unsigned long = 0);
		bool IsValid() const;
		~Base();
		static std::error_code GetLastError();
	};
}