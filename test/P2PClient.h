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

	CDownloadList &m_downlist;

	TCHAR m_file_name_str[MAX_PATH];
	FileProps m_fileprops;
	etl::vector<::sockaddr_in, 32> m_clients;
	::sockaddr_in m_tracker;
	Socket::UDP m_sock;
	const unsigned int m_idx;

public:
	explicit P2PClient(CDownloadList &);
	bool RequestClientList();
	bool RequestFileProps();
	RecvResult RecvFileContents();
	void Seed();
	void StartDownload(const DWORD, const WORD);
	void StartShare(unsigned long, const TCHAR[MAX_PATH], const TCHAR m_name_cut[MAX_PATH]);
};

