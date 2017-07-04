#include "stdafx.h"
#include "resource.h"
#include "DownloadList.h"
#include "BasicDialog.h"
#include "MainFrm.h"
#include <fstream>
#include <vector.h>
#include "SocketBase.h"
#include "UDP.h"
#include "P2PClient.h"
#include "ShareDialog.h"
#include <experimental\filesystem>
#include <string_view>

LRESULT CShareDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CenterWindow(m_parent);
	m_ip = GetDlgItem(IDC_IPADDRESS);
	m_path = GetDlgItem(IDC_PATH);
	m_ok = GetDlgItem(IDOK);
	m_browse = GetDlgItem(IDC_BROWSE);
	return TRUE;
}

LRESULT CShareDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT CShareDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	auto idx = m_parent.m_downlist.GetItemCount();
	TCHAR name_str[MAX_PATH];
	m_path.GetWindowText(name_str, MAX_PATH);
	std::basic_string_view<TCHAR> name_str_v = name_str;
	auto display = name_str_v.substr(name_str_v.find_last_of('\\') + 1);
	long size;
	{
		TCHAR file_size_str[11];
		{
			std::error_code eret;
			size = std::experimental::filesystem::file_size(name_str, eret);
			if (eret) {
				MessageBox(_T("The file does not exist. Please enter the file's name again."), nullptr, MB_OK | MB_ICONWARNING | MB_APPLMODAL);
				return 1;
			}
			_ltot(size, file_size_str, 10);
		}
		m_parent.m_downlist.AddItem(idx, 0, display.data());
		m_parent.m_downlist.AddItem(idx, 1, file_size_str);
	}
	{
		TCHAR m_ip_str[16];
		m_ip.GetWindowText(m_ip_str, sizeof(m_ip_str) / sizeof(TCHAR));
		m_parent.m_downlist.AddItem(idx, 2, m_ip_str);
	}
	m_parent.m_downlist.AddItem(idx, 3, nullptr);
	m_parent.m_downlist.AddItem(idx, 4, _T("100%"));
#ifdef _READY
	P2PClient(m_parent.m_downlist).StartShare(size, name_str, display.data());
#endif
	EndDialog(wID);
	return 0;
}

LRESULT CShareDialog::OnBrowseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CFileDialog filedlg(TRUE, nullptr, nullptr, 6, nullptr, m_parent);
	m_path.GetWindowText(filedlg.m_szFileName, sizeof(filedlg.m_szFileName) / sizeof(filedlg.m_szFileName[0]));
	if (filedlg.DoModal() == IDOK)
		m_path.SetWindowText(filedlg.m_szFileName);
	return 0;
}

