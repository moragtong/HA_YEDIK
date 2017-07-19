// test.cpp : main source file for test.exe
//

#include "stdafx.hpp"
#include "resource.h"
#include <vector>
#include <thread>
#include <array>
#include "UDP.hpp"
#include "ShareDialog.hpp"
#include "DownloadDialog.hpp"
#include "DownloadList.hpp"
#include "MainFrm.hpp"

CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow) { 
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(::CoInitialize(nullptr)));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(nullptr, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	ATLASSERT(SUCCEEDED(_Module.Init(nullptr, hInstance)));

	CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
	CMain wndMain;
	int ret;
	if (wndMain.CreateEx()) {
		wndMain.ShowWindow(nCmdShow);

		ret = loop.Run();
		_Module.RemoveMessageLoop();

		_Module.Term();
		::CoUninitialize();
	} else {
		ATLTRACE(_T("Main window creation failed!\n"));
		ret = 0;
	}
	return ret;
}
