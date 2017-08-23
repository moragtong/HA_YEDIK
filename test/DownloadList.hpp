// testView.h : interface of the CDownloadList class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

struct CDownloadList final : CWindowImpl<CDownloadList, CListViewCtrl> {
	::std::vector<CWindow> m_subobjects;

	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* /*pMsg*/);

	BEGIN_MSG_MAP(decltype(*this))
		NOTIFY_CODE_HANDLER(HDN_DIVIDERDBLCLICK, OnResize)
		NOTIFY_CODE_HANDLER(HDN_BEGINTRACK, OnResize)
		NOTIFY_CODE_HANDLER(HDN_ENDTRACK, OnResize)
		NOTIFY_CODE_HANDLER(HDN_TRACK, OnResize)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnResize(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

	template<class T>
	void AddSubObject(unsigned int x, unsigned int y, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) {
		AddItem(x, y, nullptr);
		RECT rect;
		this->GetSubItemRect(x, y, LVIR_BOUNDS, &rect);
		m_subobjects.emplace_back(T().Create(*this, &rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam));
		m_subobjects.back().ShowWindow(SW_SHOW);
	}
};
