#include "stdafx.hpp"
#include "resource.h"
#include <fstream>
#include <vector>
#include <thread>
#include <array>
#include "UDP.hpp"
#include "DownloadList.hpp"
#include "P2PClient.hpp"
#include "MainFrm.hpp"

P2PClient::P2PClient(CDownloadList &_downlist, Socket::UDP &&sock)
	: m_downlist(_downlist), m_sock(std::move(sock)),
	m_idx(m_downlist.GetItemCount() - 1), // happens fast, no point in syncing
	m_clients(32) {
	m_tracker.sin_family = AF_INET;
	Socket::WSInit();
}

bool P2PClient::RequestClientList() {
	DEBUG_OUTPUT(_T("RequestClientList"));
	char count = 4;
	int size;
	do {
		TrkCom temp{ TrkCom::LIST };
		m_sock.SendTo(&temp, sizeof(temp), m_tracker);
		sockaddr_in secure_tracker;
		std::tie(size, secure_tracker) = m_sock.RecvFrom(m_clients.data(), m_clients.size() * sizeof(sockaddr_in));
		if (secure_tracker != m_tracker)
			return false;
		--count;
	} while (size < 0 && count);
	m_clients.resize(size > 0 ? size / sizeof(sockaddr_in) : 0);
	return size > 0;
}

bool P2PClient::RequestFileProps() {
	DEBUG_OUTPUT(_T("RequestFileProps"));
	for (const auto &client : m_clients) {
		ClnCom temp{ ClnCom::FILEDATA };
		m_sock.SendTo(&temp, sizeof(temp), client);
		auto [size, secure_client] = m_sock.RecvFrom(&m_fileprops, sizeof(m_fileprops));
		if (client != secure_client)
			return false;
		if (size > 0) {
			TCHAR buff[11];
			_ltot(m_fileprops.m_size, buff, 10);
			m_downlist.SetItemText(m_idx, 0, m_fileprops.m_name);
			_tcscat(m_file_name_str, _T("\\"));
			_tcscat(m_file_name_str, m_fileprops.m_name);
			m_downlist.SetItemText(m_idx, 1, buff);
			return true;
		}
	}
	return false;
}

P2PClient::RecvResult P2PClient::RecvFileContents() {
	DEBUG_OUTPUT(_T("RecvFileContents\n"));
	DEBUG_OUTPUT(m_fileprops.m_name);
	DEBUG_OUTPUT(_T("\n"));
	DEBUG_OUTPUT(m_file_name_str);
	DEBUG_OUTPUT(_T("\n"));
	if (std::ofstream fd(m_file_name_str, std::fstream::binary); fd) {
		ClnCom temp{ ClnCom::PKT };
		::std::vector<char> relib(m_clients.size(), 2);
		char i = 0;
		do {
			union {
				char buff[BUFFSIZE];
				TCHAR len_buff[5];
			};
			m_sock.SendTo(&temp, sizeof(temp), m_clients[i]);
			auto [size, secure_client] = m_sock.RecvFrom(buff, sizeof(buff));
			if (size < 0)
				if (relib[i])
					relib[i]--;
				else {
					m_clients.erase(m_clients.cbegin() + i);
					if (m_clients.empty())
						return RecvResult::ERROR_NO_CLIENTS;
					relib.erase(relib.cbegin() + i);
					continue;
				}
			else if (secure_client == m_clients[i]) {
				temp.m_param += size;
				relib[i] = 2;
				fd.write(buff, size);
				
				_itot(temp.m_param * 100 / m_fileprops.m_size, len_buff, 10);
				_tcscat(len_buff, _T("%"));
				m_downlist.SetItemText(m_idx, 4, len_buff);
			}
			i = (i + 1) % m_clients.size();
		} while (temp.m_param < m_fileprops.m_size);
		fd.close();
		return RecvResult::SUCCESS;
	}
	DEBUG_OUTPUT(_T("RecvResult::ERROR_FILEIO\n"));
	return RecvResult::ERROR_FILEIO;
}

void P2PClient::Seed() {
	DEBUG_OUTPUT(_T("Seeding\n"));
	const auto file_props_size = sizeof(m_fileprops.m_size) + ((char*)_tmemchr(m_fileprops.m_name, _T('\0'), sizeof(m_fileprops.m_name)) - (char*)m_fileprops.m_name) + sizeof(_T('\0'));
	for (;;) {
		ClnCom temp;
		auto[size, client] = m_sock.RecvFrom(&temp, sizeof(temp));
		DEBUG_OUTPUT(_T("got one\n"));
		sockaddr_in temp_addr{ AF_INET, 37616 };
		temp_addr.sin_addr.s_addr = 16777343L;
		if (size == sizeof(temp)) {
			switch (temp.m_command) {
				case ClnCom::PKT:
					DEBUG_OUTPUT(_T("ClnCom::PKT\n"));
					if (std::ifstream fd(m_file_name_str, std::fstream::binary); fd) {
						fd.seekg(temp.m_param);
						char buff[BUFFSIZE];
						fd.read(buff, sizeof(buff));
						m_sock.SendTo(buff, fd.gcount(), client);
						fd.close();
					}
					break;

				case ClnCom::FILEDATA:
					DEBUG_OUTPUT(_T("ClnCom::FILEDATA\n"));
					m_sock.SendTo(&m_fileprops, file_props_size, client);
					break;

				default:
					DEBUG_OUTPUT(_T("eh?\n"));
			}
		} else if (!size && client == temp_addr)
			break;
	}
	TrkCom temp{ TrkCom::REMOVE };
	m_sock.SendTo(&temp, sizeof(temp), m_tracker);
}

void P2PClient::StartDownload(const unsigned long addr, const unsigned short port, TCHAR location[MAX_PATH]) {
	m_tracker.sin_port = htons(port);
	(unsigned long &)m_tracker.sin_addr = htonl(addr);
	m_file_name_str = location;

	m_sock.SetTimeout(1);

	if (RequestClientList()) {//temp
		if (RequestFileProps()) {
			if (RecvFileContents() == RecvResult::SUCCESS) {
				m_sock.SetTimeout(0);
				TrkCom temp{ TrkCom::ADD };
				m_sock.SendTo(&temp, sizeof(temp), m_tracker);
				Seed();
			}
		}
	}
}

void P2PClient::StartShare(const unsigned long addr, const unsigned long m_size, TCHAR m_name[MAX_PATH]) {
	(unsigned long &)m_tracker.sin_addr = htonl(addr);
	m_tracker.sin_port = 8513; //htons(16673)
	m_fileprops.m_size = m_size;
	m_file_name_str = m_name;
	_tcsncpy(m_fileprops.m_name, _tcsrchr(m_name, '\\') + 1, MAX_PATH);
	m_sock.SendTo(nullptr, 0, m_tracker);
	std::tie(std::ignore, m_tracker) = m_sock.RecvFrom(nullptr, 0);
	m_sock.SendTo(nullptr, 0, m_tracker);
	TCHAR buff[6];
	_itot(ntohs(m_tracker.sin_port), buff, 10);
	m_downlist.SetItemText(m_idx, 3, buff);
	Seed();
}

P2PClient::~P2PClient() {
	Socket::WSDeInit();
}