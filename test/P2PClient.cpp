#include "stdafx.h"
#include "resource.h"
#include <fstream>
#include <vector.h>
#include "DownloadList.h"
#include "MainFrm.h"
#include "SocketBase.h"
#include "UDP.h"
#include "P2PClient.h"

P2PClient::P2PClient(CDownloadList &_downlist)
	: m_downlist(_downlist),
	m_idx(m_downlist.GetItemCount()) {
	m_tracker.sin_family = AF_INET;
}

bool P2PClient::RequestClientList() {
	m_clients.resize(m_clients.capacity());
	int size;
	char count = 4;
	do {
		TrkCom temp{ TrkCom::LIST };
		m_sock.SendTo(&temp, sizeof(temp), m_tracker);
		size = m_sock.RecvFrom(m_clients.data(), m_clients.capacity() * sizeof(sockaddr_in), nullptr);
		--count;
	} while (size < 0 && count);
	m_clients.resize(size > 0 ? size / sizeof(sockaddr_in) : 0);
	return size > 0;
}

bool P2PClient::RequestFileProps() {
	ClnCom temp{ ClnCom::FILEDATA };
	for (const auto &client : m_clients) {
		m_sock.SendTo(&temp, sizeof(temp), client);
		if (m_sock.RecvFrom(&m_fileprops, sizeof(m_fileprops), nullptr) > 0) {
			TCHAR buff[11];
			_ltot(m_fileprops.m_size, buff, 10);
			m_downlist.SetItemText(m_idx, 0, m_fileprops.m_name);
			m_downlist.SetItemText(m_idx, 1, buff);
			return true;
		}
	}
	return false;
}

P2PClient::RecvResult P2PClient::RecvFileContents() {
	std::ofstream fd(m_file_name_str);
	if (fd.is_open()) {
		ClnCom temp{ ClnCom::PKT };
		etl::array<char, CLN_NUM> relib;
		relib.fill(2);
		char i = 0;
		do {
			union {
				char buff[BUFFSIZE];
				TCHAR len_buff[5];
			};
			m_sock.SendTo(&temp, sizeof(temp), m_clients[i]);
			if (m_sock.RecvFrom(buff, sizeof(buff), nullptr) < 0) {
				if (relib[i])
					relib[i]--;
				else {
					m_clients.erase(m_clients.data() + i);
					if (m_clients.empty())
						return RecvResult::ERROR_NO_CLIENTS;
					relib.erase_at(i);
					continue;
				}
			} else {
				temp.m_param += BUFFSIZE;
				relib[i] = 2;
				fd.write(buff, sizeof(buff));
				_itot(temp.m_param * 100 / m_fileprops.m_size, len_buff, sizeof(len_buff) / sizeof(TCHAR));
				StrCat(len_buff, _T("%"));
				m_downlist.SetItemText(m_idx, 4, len_buff);
			}
			i = (i + 1) % m_clients.size();
		} while (temp.m_param < m_fileprops.m_size);
		fd.close();
		return RecvResult::SUCCESS;
	}
	return RecvResult::ERROR_FILEIO;
}

void P2PClient::Seed() {
	m_sock.SetTimeout(0);
	std::ifstream fd(m_file_name_str);
	auto file_props_size = sizeof(m_fileprops.m_size) + AtlStrLen(m_fileprops.m_name) + 1;
	for (;;) {
		ClnCom temp;
		sockaddr_in client;
		m_sock.RecvFrom(&temp, sizeof(temp), &client);
		if (temp.m_command == ClnCom::PKT) {
			char buff[BUFFSIZE];
			fd.seekg(temp.m_param);
			m_sock.SendTo(buff, fd.readsome(buff, sizeof(buff)), client);
			fd.close();
		} else
			m_sock.SendTo(&m_fileprops, file_props_size, client);
	}
}

void P2PClient::StartDownload(const DWORD addr, const WORD port) {
	m_tracker.sin_port = port;
	m_tracker.sin_addr = (in_addr&)addr;

	m_sock.Create();
	m_sock.SetTimeout(1);

	if (RequestClientList()) //temp
		if (RequestFileProps())
			if (RecvFileContents() == RecvResult::SUCCESS)
				Seed();
}

void P2PClient::StartShare(const unsigned long m_size, const TCHAR m_name[MAX_PATH], const TCHAR m_name_cut[MAX_PATH]) {
	m_fileprops.m_size = m_size;
	StrCpyN(m_file_name_str, m_name, MAX_PATH);
	StrCpyN(m_fileprops.m_name, m_name_cut, MAX_PATH);
	Seed();
}