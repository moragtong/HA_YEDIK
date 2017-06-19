#include "stdafx.h"
#include "resource.h"
#include <vector.h>
#include <Ws2tcpip.h>
#include "testView.h"
#include "MainFrm.h"
#include "P2PClient.h"


P2PClient::P2PClient(CMainFrame& _main)
	: m_main(_main),
	m_idx(m_main.m_view.GetItemCount()) {
}
bool P2PClient::RequestClientList() {
	if (m_tracker.sin_port != 8513) { // htons(16673)
		m_clients.resize(m_clients.capacity());
		int size;
		char count = 4;
		do {
			{
				TrkCom temp{ TrkCom::LIST };
				m_sock.SendTo(&temp, sizeof(temp), m_tracker);
			}
			size = m_sock.RecvFrom(m_clients.data(), m_clients.capacity() * sizeof(sockaddr_in), nullptr);
			--count;
		} while (size < 0 && count);
		m_clients.resize((count ? size : 0) / sizeof(sockaddr_in));
		return count;
	}
	return false;
}
bool P2PClient::RequestFileProps() {
	ClnCom temp{ ClnCom::FILEDATA };
	for (const auto &client : m_clients) {
		m_sock.SendTo(&temp, sizeof(temp), client);
		auto check = m_sock.RecvFrom(&m_fileprops, sizeof(m_fileprops), nullptr);
		if (check) {
			itoa(m_fileprops.m_size, m_size_str, 10);
			itoa(m_tracker.sin_port, m_port, 10); // not here!
			inet_pton(4, m_ip_str, &m_tracker.sin_addr); //not here!!
			m_main.m_view.SetItemText(m_idx, 0, m_fileprops.m_name);
			m_main.m_view.SetItemText(m_idx, 1, m_size_str);
			m_main.m_view.SetItemText(m_idx, 2, m_ip_str);
			return true;
		}
	}
	return false;
}
char P2PClient::RecvFileContents() {

}
