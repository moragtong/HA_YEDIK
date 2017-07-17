// test.cpp : main source file for test.exe
//

#include "stdafx.h"

#include "resource.h"
#include <vector>
#include <thread>
#include "ShareDialog.h"
#include "DownloadDialog.h"
#include "DownloadList.h"
#include "MainFrm.h"

CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow) {
	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	CMain wndMain;
	int nRet;
	if (wndMain.CreateEx() == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		nRet = 0;
	} else {
		wndMain.ShowWindow(nCmdShow);

		nRet = theLoop.Run();
		_Module.RemoveMessageLoop();

		_Module.Term();
		::CoUninitialize();
	}
	return nRet;
}
