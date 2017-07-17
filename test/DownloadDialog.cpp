#include "stdafx.h"
#include "resource.h"
#include <vector>
#include <thread>
#include <fstream>
#include "DownloadList.h"
#include "MainFrm.h"
#include "UDP.h"
#include "P2PClient.h"
#include "DownloadDialog.h"

CDownloadDialog::CDownloadDialog(CMain &parent)
	: m_parent(parent) {
}

LRESULT CDownloadDialog::OnEditCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_ok.EnableWindow(m_path.GetWindowTextLength() && !m_ip.IsBlank());
	m_ok.SetButtonStyle(BS_DEFPUSHBUTTON);
	m_browse.SetButtonStyle(BS_PUSHBUTTON);
	return 0;
}

LRESULT CDownloadDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CenterWindow(m_parent);
	m_ip = GetDlgItem(IDC_IPADDRESS);
	m_path = GetDlgItem(IDC_PATH);
	m_ok = GetDlgItem(IDOK);
	m_browse = GetDlgItem(IDC_BROWSE);
	m_port = GetDlgItem(IDC_EDIT);
	m_spin = GetDlgItem(IDC_SPIN);
	m_port.SetLimitText(5);
	m_spin.SetRange32(2000, 65536);
	m_spin.SetPos32(2000);
	TCHAR buff[MAX_PATH];
	GetEnvironmentVariable(_T("USERPROFILE"), buff, sizeof(buff));
	StrCat(buff, _T("\\Downloads"));
	m_path.SetWindowText(buff);
	return TRUE;
}

LRESULT CDownloadDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT CDownloadDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	auto idx = m_parent.m_downlist.GetItemCount();
	m_parent.m_downlist.AddItem(idx, 0, nullptr);

	m_parent.m_downlist.AddItem(idx, 1, nullptr);
	{
		TCHAR m_ip_str[16];
		m_ip.GetWindowText(m_ip_str, sizeof(m_ip_str) / sizeof(TCHAR));
		m_parent.m_downlist.AddItem(idx, 2, m_ip_str);
	}
	{
		TCHAR m_port_str[6];
		m_port.GetWindowText(m_port_str, sizeof(m_port_str) / sizeof(TCHAR));
		m_parent.m_downlist.AddItem(idx, 3, m_port_str);
	}
	m_parent.m_downlist.AddItem(idx, 4, _T("0%"));
#ifdef _READY
	{
		DWORD addr;
		m_ip.GetAddress(&addr);
		auto pass = m_spin.GetPos();
		auto path = new TCHAR[MAX_PATH];
		m_path.GetWindowText(path, MAX_PATH);
		m_parent.m_down_thread_store.emplace_back([=] {
			P2PClient(m_parent.m_downlist).StartDownload(addr, pass, path);
		});
	}
#endif
	EndDialog(wID);
	return 0;
}

LRESULT CDownloadDialog::OnBrowseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	TCHAR buff[MAX_PATH];
	m_path.GetWindowText(buff, sizeof(buff) / sizeof(TCHAR));
	CFolderDialog folderdlg(m_hWnd, _T("Choose where the file is going to be downloaded to:"));
	folderdlg.SetInitialFolder(buff);
	if (folderdlg.DoModal() == IDOK)
		m_path.SetWindowText(folderdlg.GetFolderPath());
	return 0;
}
