// testView.cpp : implementation of the CDownloadList class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.hpp"
#include "resource.h"
#include <vector>
#include "DownloadList.hpp"

BOOL CDownloadList::PreTranslateMessage(MSG* pMsg) {
	pMsg;
	return FALSE;
}

LRESULT CDownloadList::OnResize(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/) {
	auto iter = m_subobjects.data();
	for (unsigned int i = 0; i < m_subobjects.size(); i++, iter++) {
		RECT rect;
		this->GetSubItemRect(i, 4, LVIR_BOUNDS, &rect);
		iter->SetWindowPos(HWND_BOTTOM, &rect, SWP_NOZORDER);
	}
	return 0;
}