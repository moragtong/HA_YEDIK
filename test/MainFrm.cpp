﻿// MainFrm.cpp : implmentation of the CMain class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "DownloadDialog.h"
#include "DownloadList.h"
#include "MainFrm.h"

CMain::CMain()
	: m_down_dlg(*this) {
}

BOOL CMain::PreTranslateMessage(MSG* pMsg) {
	if (CFrameWindowImpl<CMain>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_downlist.PreTranslateMessage(pMsg);
}

BOOL CMain::OnIdle() {
	UIUpdateToolBar();
	if (m_down_dlg.m_hWnd) {
		m_down_dlg.m_ok.EnableWindow(m_down_dlg.m_path.GetWindowTextLength());
		m_down_dlg.m_ok.SetButtonStyle(BS_DEFPUSHBUTTON);
		m_down_dlg.m_browse.SetButtonStyle(BS_PUSHBUTTON);
	}
	return FALSE;
}

LRESULT CMain::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
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
	return 1;
}

LRESULT CMain::OnDownload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EnableWindow(false);
	m_down_dlg.Create(*this);
	m_down_dlg.ShowWindow(SW_SHOWNORMAL);
	return 0;
}