// MainFrm.cpp : implmentation of the CMain class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.hpp"
#include "resource.h"
#include <vector>
#include <thread>
#include <array>
#include "UDP.hpp"
#include "ShareDialog.hpp"
#include "DownloadDialog.hpp"
#include "DownloadList.hpp"
#include "P2PClient.hpp"
#include "MainFrm.hpp"

BOOL CMain::PreTranslateMessage(MSG* pMsg) {
	if (CFrameWindowImpl<CMain>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_downlist.PreTranslateMessage(pMsg);
}

BOOL CMain::OnIdle() {
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMain::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	Socket::WSInit();
	CreateSimpleToolBar(IDR_TOOLBAR1);
	m_hWndClient = m_downlist.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_downlist.AddColumn(_T("name"), 0);
	m_downlist.AddColumn(_T("size"), 1);
	m_downlist.AddColumn(_T("ip"), 2);
	m_downlist.AddColumn(_T("port"), 3);
	m_downlist.AddColumn(_T("progress"), 4);
	UIAddToolBar(m_hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	return 0;
}

LRESULT CMain::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	bHandled = FALSE;

	Socket::UDP central;
	central.Create();
	{
		unsigned long addr = INADDR_LOOPBACK;
		central.Bind(61586, (::in_addr &)addr);
	}
	for (auto &&temp : m_store) {
		auto addr = 16777343UL;
		central.SendTo(nullptr, 0, { AF_INET, std::get<1>(temp), (::in_addr &)addr });
		std::get<0>(temp).join();
	}
	Socket::WSDeInit();
	return 1;
}

LRESULT CMain::OnDownload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CDownloadDialog(*this).DoModal(*this);
	return 0;
}

LRESULT CMain::OnShare(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CShareDialog(*this).DoModal(*this);
	return 0;
}

void CMain::StartNewDownload(const unsigned long addr, const unsigned short port, ::std::array<TCHAR, MAX_PATH> &path) {
	Socket::UDP sock;
	sock.Create();
	sock.Bind();
	m_store.emplace_back(
		[=, sock(::std::move(sock))] {
			P2PClient(m_downlist, (::Socket::UDP &&)::std::move(sock)).StartDownload(addr, port, (TCHAR *)path.data());
		},
		sock.GetInfo().sin_port
	);
}

void CMain::StartNewShare(unsigned long addr, unsigned long size, ::std::array<TCHAR, MAX_PATH> &name) {
	Socket::UDP sock;
	sock.Create();
	sock.Bind();
	m_store.emplace_back(
		[=, sock(::std::move(sock))] {
			P2PClient(m_downlist, (::Socket::UDP &&)::std::move(sock)).StartShare(addr, size, (TCHAR *)name.data());
		},
		sock.GetInfo().sin_port
	);
}