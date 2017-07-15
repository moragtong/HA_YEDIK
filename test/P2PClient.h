#pragma once
bool operator==(const in_addr &, const in_addr &);
bool operator!=(const in_addr &, const in_addr &);
bool operator==(const sockaddr_in &, const sockaddr_in &);
bool operator!=(const sockaddr_in &, const sockaddr_in &);
class P2PClient {
	enum : unsigned int {
		BUFFSIZE = 1492
	};

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

	TCHAR m_file_name_str[MAX_PATH];
	FileProps m_fileprops;
	::std::vector<::sockaddr_in> m_clients;
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

