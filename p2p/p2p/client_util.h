#pragma once
#include <util.h>
enum { buffsize = 508 };
struct filedata {
	unsigned long size;
	char name[FILENAME_MAX];
	/*filedata() {
		memset(name, 0, sizeof(name));
	}*/
	
	/*friend std::istream& operator>>(std::istream& in, filedata& self) {
		in >> self.name;
		struct stat stat_buf;
		int rc = stat(self.name, &stat_buf);
		self.size = rc == 0 ? stat_buf.st_size : 0;
		return in;
	}*/
};
enum trk_com_enum : char { ADD, REMOVE, LIST };
struct trk_com {
	trk_com_enum command;
	//unsigned long param = 0;
};
enum cln_com_enum : char { FILEDATA, PKT };
struct cln_com {
	cln_com_enum command;
	unsigned long param = 0;
};