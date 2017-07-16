#pragma once
#include "Base.h"
#include <tuple>

namespace Socket {
	struct UDP : detail::Base {
		using Base::Bind;
		bool Create();
		std::tuple<int, ::sockaddr_in> RecvFrom(void *, size_t, int = 0);
		int SendTo(void *pBuff, size_t nBuffSize, const ::sockaddr_in &client_addr, int nFlag = 0);
	};
}

