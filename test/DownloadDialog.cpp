// aboutdlg.cpp : implementation of the CDownloadDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "DownloadDialog.h"

CDownloadDialog::CDownloadDialog(etl::ivector<std::thread>& down_thread_store)
	: m_down_thread_store(down_thread_store) {
}
LRESULT CDownloadDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CDownloadDialog::OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}

LRESULT CDownloadDialog::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	EndDialog(wID);
	return 0;
}
