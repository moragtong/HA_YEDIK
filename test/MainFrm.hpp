// MainFrm.h : interface of the CMain class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DownloadDialog.hpp"
#include "ShareDialog.hpp"

struct CMain final : CFrameWindowImpl<CMain>, CUpdateUI<CMain>, CMessageFilter, CIdleHandler {
	CDownloadList m_downlist;

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	BOOL PreTranslateMessage(MSG *pMsg);
	BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMain)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(decltype(*this))
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_DOWNLOAD_BUTTON, OnDownload)
		COMMAND_ID_HANDLER(ID_SHARE_BUTTON, OnShare)
		CHAIN_MSG_MAP(CUpdateUI<CMain>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMain>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShare(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	::std::vector<::std::tuple<::std::thread, ::sockaddr_in>> m_store;

	void StartNewDownload(unsigned long, unsigned short, ::std::array<TCHAR, MAX_PATH> &);
	void StartNewShare(unsigned long, unsigned long, ::std::array<TCHAR, MAX_PATH> &);
};
