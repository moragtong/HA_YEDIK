// aboutdlg.cpp : implementation of the CDownloadDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "DownloadList.h"
#include "MainFrm.h"
#include "P2PClient.h"
#include "DownloadDialog.h"

CDownloadDialog::CDownloadDialog(CMain &parent)
	: m_parent(parent) {
}

LRESULT CDownloadDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CenterWindow(m_parent);
	m_ip = GetDlgItem(IDC_IPADDRESS);
	m_port = GetDlgItem(IDC_EDIT);
	m_spin = GetDlgItem(IDC_SPIN);
	m_port.SetLimitText(5);
	m_spin.SetRange32(2000, 65536);
	m_spin.SetPos32(2000);
	return TRUE;
}

LRESULT CDownloadDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT CDownloadDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	P2PClient(this->m_parent).StartDownload();
	EndDialog(wID);
	return 0;
}
