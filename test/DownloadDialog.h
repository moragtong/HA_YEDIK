#pragma once
struct CMain;
struct CDownloadDialog final :
	CDialogImpl<CDownloadDialog> {
	enum { IDD = IDD_FORMVIEW };

	CMain &m_parent;
	CIPAddressCtrl m_ip;
	CUpDownCtrl m_spin;
	CEdit m_port;
	CEdit m_path;
	CButton m_ok;
	CButton m_browse;

	BEGIN_MSG_MAP(CDownloadDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_PATH, EN_CHANGE, OnEditControlChange)
		COMMAND_HANDLER(IDC_IPADDRESS, EN_CHANGE, OnEditControlChange)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancelCmd)
		COMMAND_ID_HANDLER(IDOK, OnOKCmd)
		COMMAND_ID_HANDLER(IDC_BROWSE, OnBrowseCmd)
	END_MSG_MAP()

	CDownloadDialog(CMain &);
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//  LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBrowseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditControlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

