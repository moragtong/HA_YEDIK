#pragma once
class P2PClient {
	enum { CLN_NUM = 32, BUFFSIZE = 1492, TIMEOUT = 1500 };
	struct TrkCom {
		enum TrkComEnum : char {
			ADD, REMOVE, LIST
		} m_command;
		//unsigned long param;
	};
	struct ClnCom {
		enum ClnComEnum : char {
			FILEDATA, PKT
		} m_command;
		unsigned long param = 0;
	};
	struct FileProps {
		unsigned long m_size;
		TCHAR m_name[FILENAME_MAX / 5];
	} m_fileprops;
	TCHAR m_name[FILENAME_MAX],
		m_size_str[50],
		m_ip_str[22],
		m_port[5];
	etl::vector<::sockaddr_in, 32> m_clients;
	::sockaddr_in m_tracker;
	Socket::UDP m_sock;
	CMainFrame& m_main;
	const unsigned int m_idx;
public:
	P2PClient(CMainFrame& _main);
	bool RequestClientList();
	bool RequestFileProps();
	char RecvFileContents();
};

