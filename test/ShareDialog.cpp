#include "stdafx.hpp"
#include "resource.h"
#include <fstream>
#include <vector>
#include <thread>
#include <array>
#include <experimental\filesystem>
#include "DownloadList.hpp"
#include "UDP.hpp"
#include "MainFrm.hpp"
#include "P2PClient.hpp"
#include "ShareDialog.hpp"

CShareDialog::CShareDialog(CMain &parent)
	: m_parent(parent) {
}

LRESULT CShareDialog::OnEditCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_ok.EnableWindow(m_path.GetWindowTextLength() && !m_ip.IsBlank());
	m_ok.SetButtonStyle(BS_DEFPUSHBUTTON);
	m_browse.SetButtonStyle(BS_PUSHBUTTON);
	return 0;
}

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
	std::array<TCHAR, MAX_PATH> name_str;
	m_path.GetWindowText(name_str.data(), MAX_PATH);
	long size;
	{
		TCHAR file_size_str[11];
		{
			std::error_code eret;
			size = std::experimental::filesystem::file_size(name_str.data(), eret);
			if (eret) {
				MessageBox(_T("The file does not exist. Please enter the file's name again."), nullptr, MB_OK | MB_ICONWARNING | MB_APPLMODAL);
				return 1;
			}
			_ltot(size, file_size_str, 10);
		}
		m_parent.m_downlist.AddItem(idx, 0, _tcsrchr(name_str.data(), '\\') + 1);
		m_parent.m_downlist.AddItem(idx, 1, file_size_str);
	}
	{
		TCHAR m_ip_str[16];
		m_ip.GetWindowText(m_ip_str, sizeof(m_ip_str) / sizeof(TCHAR));
		m_parent.m_downlist.AddItem(idx, 2, m_ip_str);
	}
	m_parent.m_downlist.AddItem(idx, 3, nullptr);
	m_parent.m_downlist.AddItem(idx, 4, _T("100%"));
	DWORD addr;
	m_ip.GetAddress(&addr);
	m_parent.StartNewShare(addr, size, name_str);
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

