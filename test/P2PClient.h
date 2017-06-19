#pragma once
class P2PClient {
	enum {
		CLN_NUM = 32,
		BUFFSIZE = 1492,
		TIMEOUT = 1500
	};
	enum class RecvResult : char {
		ERROR_NO_CLIENTS,
		ERROR_FILEIO,
		SUCCESS
	};
	struct TrkCom {
		enum TrkComEnum : char {
			ADD,
			REMOVE,
			LIST
		};
		TrkComEnum m_command;
		//unsigned long m_param;
	};
	struct ClnCom {
		enum ClnComEnum : char {
			FILEDATA,
			PKT
		};
		ClnComEnum m_command;
		unsigned long m_param = 0;
	};
	struct FileProps {
		unsigned long m_size;
		TCHAR m_name[FILENAME_MAX / 5];
	};

	/*TCHAR m_file_name_str[FILENAME_MAX],
		m_ip_str[22],
		m_port_str[6];*/

	std::ofstream m_fd;
	FileProps m_fileprops;
	etl::vector<::sockaddr_in, 32> m_clients;
	::sockaddr_in m_tracker;
	Socket::UDP m_sock;
	CMainFrame& m_main;
	const unsigned int m_idx;
public:
	P2PClient(CMainFrame& _main);
	bool RequestClientList();
	bool RequestFileProps();
	RecvResult RecvFileContents();
};

