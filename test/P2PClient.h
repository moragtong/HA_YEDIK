#pragma once
class P2PClient {
	enum {
		CLN_NUM = 32,
		BUFFSIZE = 1492,
		TIMEOUT = 1500
	};

	enum class RecvResult : char {
		SUCCESS,
		ERROR_NO_CLIENTS,
		ERROR_FILEIO,
	};

	struct TrkCom {
		enum TrkComEnum : char {
			ADD,
			REMOVE,
			LIST,
		};

		TrkComEnum m_command;
		//unsigned long m_param;
	};

	struct ClnCom {
		enum ClnComEnum : char {
			FILEDATA,
			PKT,
		};

		ClnComEnum m_command;
		unsigned long m_param = 0;
	};
	struct FileProps {
		unsigned long m_size;
		TCHAR m_name[MAX_PATH];
	};

	union {
		TCHAR m_file_name_str[MAX_PATH];
		TCHAR m_port_str[6];
		TCHAR m_ip_str[16];
	};
	FileProps m_fileprops;
	etl::vector<::sockaddr_in, 32> m_clients;
	::sockaddr_in m_tracker;
	Socket::UDP m_sock;
	CMain &m_main;
	const unsigned int m_idx;

public:
	explicit P2PClient(CMain &);
	bool RequestClientList();
	bool RequestFileProps();
	RecvResult RecvFileContents();
	void Seed();
	void StartDownload();
	void StartShare();
};

