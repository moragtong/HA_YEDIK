#pragma once
struct CMainFrame;
struct CDownloadDialog final :
	CDialogImpl<CDownloadDialog>, 
	CWinDataExchange<CDownloadDialog> {

	enum { IDD = IDD_FORMVIEW };

	CMainFrame &m_parent;
	CIPAddressCtrl m_ip;
	CEdit m_port;

	BEGIN_MSG_MAP(CDownloadDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancelCmd)
		COMMAND_ID_HANDLER(IDOK, OnOKCmd)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDownloadDialog)
		DDX_CONTROL_HANDLE(IDC_IPADDRESS, m_ip)
		DDX_CONTROL_HANDLE(IDC_EDIT, m_port)
	END_DDX_MAP()

	CDownloadDialog(CMainFrame &);
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancelCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

