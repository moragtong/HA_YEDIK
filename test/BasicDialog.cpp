#include "stdafx.h"
#include "resource.h"
#include "DownloadList.h"
#include "MainFrm.h"
#include "BasicDialog.h"

BasicDialog::BasicDialog(CMain &parent)
	: m_parent(parent) {
}

LRESULT BasicDialog::OnEditCtrlChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_ok.EnableWindow(m_path.GetWindowTextLength() && !m_ip.IsBlank());
	m_ok.SetButtonStyle(BS_DEFPUSHBUTTON);
	m_browse.SetButtonStyle(BS_PUSHBUTTON);
	return 0;
}
