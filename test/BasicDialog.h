#pragma once
struct CMain;
struct BasicDialog {
	CMain &m_parent;
	CIPAddressCtrl m_ip;
	CEdit m_path;
	CButton m_ok;
	CButton m_browse;

	BasicDialog(CMain &parent);

	BEGIN_MSG_MAP(decltype(*this))
		COMMAND_HANDLER(IDC_PATH, EN_CHANGE, OnEditCtrlChange)
		COMMAND_HANDLER(IDC_IPADDRESS, EN_CHANGE, OnEditCtrlChange)
	END_MSG_MAP()

	LRESULT OnEditCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

