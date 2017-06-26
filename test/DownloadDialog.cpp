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
	m_path = GetDlgItem(IDC_PATH);
	m_ok = GetDlgItem(IDOK);
	m_browse = GetDlgItem(IDC_BROWSE);
	m_port.SetLimitText(5);
	m_spin.SetRange32(2000, 65536);
	m_spin.SetPos32(2000);
	TCHAR buff[MAX_PATH];
	GetEnvironmentVariable(_T("USERPROFILE"), buff, sizeof(buff));
	m_path.SetWindowText(buff);
	m_path.AppendText(_T("\\downloads"));
	return TRUE;
}

LRESULT CDownloadDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_parent.EnableWindow();
	DestroyWindow();
	return 0;
}

LRESULT CDownloadDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	P2PClient(m_parent).StartDownload();
	m_parent.EnableWindow();
	DestroyWindow();
	return 0;
}

LRESULT CDownloadDialog::OnBrowseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	TCHAR buff[MAX_PATH];
	m_path.GetWindowText(buff, sizeof(buff));
	CFolderDialog m_folderdlg(*this, _T("Choose where the file is going to be downloaded to:"));
	m_folderdlg.SetInitialFolder(buff);
	if (m_folderdlg.DoModal() == IDOK)
		m_path.SetWindowText(m_folderdlg.GetFolderPath());
	return 0;
}
