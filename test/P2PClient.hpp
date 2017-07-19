#pragma once

struct P2PClient {
	static constexpr unsigned int BUFFSIZE = 1492;

	enum class RecvResult {
		SUCCESS,
		ERROR_NO_CLIENTS,
		ERROR_FILEIO,
	};

	struct TrkCom {
		enum TrkComEnum : unsigned char {
			ADD,
			REMOVE,
			LIST,
		};

		TrkComEnum m_command;
		//unsigned long m_param;
	};

	struct ClnCom {
		enum ClnComEnum {
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

	TCHAR *m_file_name_str;
	FileProps m_fileprops;
	::std::vector<::sockaddr_in> m_clients;
	::sockaddr_in m_tracker;
	Socket::UDP m_sock;
	const unsigned int m_idx;

	P2PClient(CDownloadList &, Socket::UDP &&);
	bool RequestClientList();
	bool RequestFileProps();
	RecvResult RecvFileContents();
	void Seed();
	void StartDownload(unsigned long, unsigned short, TCHAR [MAX_PATH]);
	void StartShare(unsigned long, unsigned long, TCHAR [MAX_PATH]);
	~P2PClient();
};

