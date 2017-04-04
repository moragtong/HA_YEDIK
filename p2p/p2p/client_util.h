#pragma once
#include <util.h>
namespace p2p_client {
	enum { buffsize = 2048 };
	struct filedata {
		char name[64];
		unsigned long size;
		friend std::istream& operator>>(std::istream& in, filedata& self) {
			in >> self.name;
			struct stat stat_buf;
			int rc = stat(self.name, &stat_buf);
			self.size = rc == 0 ? stat_buf.st_size : 0;
			return in;
		}
	};
	template <class T, size_t N>
	struct recvbuff {
		T* _end;
		T _buff[N];
		recvbuff()
			: _end(_buff) {
		}
		T* begin() {
			return _buff;
		}
		T* end() {
			return _end;
		}
		size_t size() {
			return _end - _buff;
		}
	};
	enum trk_com_enum { ADD, LIST, REMOVE };
	struct trk_com {
		trk_com_enum command;
		//unsigned long param = 0;
	};
	enum cln_com_enum { FILEDATA, PKT };
	struct cln_com {
		cln_com_enum command;
		unsigned long param = 0;
	};
}