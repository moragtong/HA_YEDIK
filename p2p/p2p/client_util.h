#pragma once
#include <util.h>
namespace p2p_client {
	enum { buffsize = 2048 };
	struct filedata {
		char name[64];
		unsigned long size;
		filedata() {
			std::memset(name, 0, sizeof(name));
		}
		friend std::istream& operator>>(std::istream& in, filedata& self) {
			in >> self.name;
			struct stat stat_buf;
			int rc = stat(self.name, &stat_buf);
			self.size = rc == 0 ? stat_buf.st_size : 0;
			return in;
		}
	};
	enum trk_com_enum : char { ADD, REMOVE, LIST };
	struct trk_com {
		trk_com_enum command;
		//unsigned long param;
	};
	enum cln_com_enum : char { FILEDATA, PKT };
	struct cln_com {
		cln_com_enum command;
		unsigned long param = 0;
	};
}