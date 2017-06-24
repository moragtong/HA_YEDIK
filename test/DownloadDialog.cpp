// aboutdlg.cpp : implementation of the CDownloadDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "DownloadList.h"
#include "MainFrm.h"
#include "P2PClient.h"
#include "DownloadDialog.h"

CDownloadDialog::CDownloadDialog(CMainFrame &parent)
	: m_parent(parent) {
	m_port.SetLimitText(5);
}
LRESULT CDownloadDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CenterWindow(m_parent);
	
	return TRUE;
}

LRESULT CDownloadDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT CDownloadDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	DoDataExchange(DDX_SAVE);
	P2PClient(this->m_parent);
	EndDialog(wID);
	return 0;
}
