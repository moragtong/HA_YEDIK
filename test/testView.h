// testView.h : interface of the CTestView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

struct CTestView : CWindowImpl<CTestView, CListViewCtrl> {
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())
	
	BOOL PreTranslateMessage(MSG* /*pMsg*/);

	BEGIN_MSG_MAP(CTestView)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
