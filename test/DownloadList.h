// testView.h : interface of the CDownloadList class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

struct CDownloadList final : CWindowImpl<CDownloadList, CListViewCtrl> {
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())
	
	BOOL PreTranslateMessage(MSG* /*pMsg*/);

	BEGIN_MSG_MAP(CDownloadList)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
