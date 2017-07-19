#pragma once
#include <tuple>
#include "Base.hpp"

namespace Socket {
	struct UDP : detail::Base {
		using Base::Bind;
		bool Create();
		std::tuple<int, ::sockaddr_in> RecvFrom(void *, size_t, int = 0);
		int SendTo(void *, size_t, const ::sockaddr_in &, int = 0);
	};
}