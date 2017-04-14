#pragma once
#include <util.h>
enum { buffsize = 508 };
struct filedata {
	unsigned long size;
	char name[FILENAME_MAX];
	/*filedata() {
		memset(name, 0, sizeof(name));
	}*/
	void init(const Glib::ustring& _name) {
		auto off = _name.find_last_of('\\') + 1;
		_name.copy(name, _name.length() - off, off);
		name[_name.length() - off] = 0;
		for (auto c : name)
			putchar(c);
		struct stat stat_buf;
		int rc = stat(_name.c_str(), &stat_buf);
		size = rc == 0 ? stat_buf.st_size : 0;
	}
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