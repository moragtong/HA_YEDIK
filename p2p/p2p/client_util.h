#pragma once
#include <util.h>
/**
* \defgroup Protocol
* @{
*/
enum { buffsize = 1492 };
struct filedata {
	unsigned long size;
	char name[FILENAME_MAX];
};
enum cln_com_enum : char { FILEDATA, PKT };
struct cln_com {
	cln_com_enum command;
	unsigned long param = 0;
};
/**@}*/