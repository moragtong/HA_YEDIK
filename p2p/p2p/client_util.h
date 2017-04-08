#pragma once
#include <util.h>
namespace p2p_client {
	enum { buffsize = 2048, piecesize = 2097152 };
	/*struct filedata {
		BYTE file_count;
		filedata() {}
		filedata& construct(const char* raw, char* output) {
			auto ret = (filedata&)*output;
			ret.file_count = 0;
			for (auto i = output; *i; i++)
				if (*i == ':')
					ret.file_count++;
			

		}
		unsigned long* filesizes() {
			return (unsigned long *)(&file_count + 1);
		}
		BYTE* filenames(BYTE i) {
			if (i)
				return ((BYTE**)(filesizes() + file_count))[i + 1];
			return (BYTE*)(filesizes() + file_count - 1) + file_count;
		}
		BYTE* file_hash(BYTE i) {
			return filenames(file_count) + filesizes()[i] / piecesize * 16;
		}
		friend std::istream& operator>>(std::istream& in, filedata& self) {
			in >> self.name;
			struct stat stat_buf;
			int rc = stat(self.name, &stat_buf);
			self.size = rc == 0 ? stat_buf.st_size : 0;
			return in;
		}
	};*/
	class parser {
		private:
			BYTE* _fdata;
			const unsigned long* _fsize;
			const char* _fname;
			BYTE* _checksum;
		public:
			parser() {}
			void init(BYTE* __fdata) {
				_fdata = __fdata;
				_fsize = (const unsigned long*)(__fdata + 1);
				_fname = (const char*)__fdata + 1 + (*__fdata) * sizeof(unsigned long);
				auto ptr = _fname;
				for (auto i = *_fdata; i; i--)
					ptr = strchr(ptr, 0) + 1;
				_checksum = (BYTE*)ptr;
			}
			explicit parser(BYTE* __fdata) {
				init(__fdata);
			}
			auto fdata() {
				return _fdata;
			}
			auto fsize() {
				return *_fsize;
			}
			auto fname() {
				return _fname;
			}
			auto checksum() {
				return _checksum;
			}
			parser& next() {
				_fsize++;
				_fname = strchr(_fname, 0) + 1;
			}
			BYTE* next_checksum() {
				_checksum += 16;
				return _checksum;
			}
	};
	enum trk_com_enum : int { ADD, REMOVE, LIST };
	struct trk_com {
		trk_com_enum command;
		//unsigned long param = 0;
	};
	enum cln_com_enum : int { FILEDATA, PKT, FILEDATA_SIZE };
	struct cln_com {
		cln_com_enum command;
		unsigned long param = 0;
	};
}