// LTSkin.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include "config.h"

#include "InterFace.h"
#include "WebView.h"


// This is the constructor of a class that has been exported.
// see LTSkin.h for the class definition


LTFrameView::LTFrameView(I_LTFRAME_IWebView * iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
												   int x,int y,int nWidth,int nHeight,HICON icon):_itframeview(0)
{
    char * applicationpath  = (char*)malloc((MAX_PATH+1)/**sizeof(char)*/); 
	GetModuleFileNameA(NULL, applicationpath, MAX_PATH);
	(strrchr(applicationpath, '\\'))[1] = 0;
	strcat(applicationpath,"icudt40l.dat");
	u_setDataDirectory(applicationpath);
	free(applicationpath);
	_itframeview=new WebView(iwebview,lpWindowName,hWndParent,dwStyle,x,y,nWidth,nHeight,icon);
}

LTFrameView::~LTFrameView()
{
	if(_itframeview)
	  delete _itframeview;
}

LRESULT LTFrameView::HandleMessage(HWND _hwnd, UINT _unit, WPARAM _wparam, LPARAM _lparam)
{
	return _itframeview->HandleMessage(_hwnd, _unit, _wparam, _lparam);
}
void LTFrameView::MessageLoop()
{
	_itframeview->MessageLoop();
}
void LTFrameView::loadHTML(const wchar_t* html)
{
	_itframeview->loadHTML(html);
}
void LTFrameView::load(const wchar_t* html)
{
	_itframeview->load(html);
}
void LTFrameView::loadFile(const wchar_t* html)
{
	_itframeview->loadFile(html);
}
void LTFrameView::SetViewTransparent(bool b)
{
	_itframeview->SetViewTransparent(b);
}
void LTFrameView::IsAutoGC(bool b,int maxmemorysize)
{
	_itframeview->IsAutoGC(b,maxmemorysize);
}
void LTFrameView::CleanMemory()
{
	_itframeview->CleanMemory();
}
void LTFrameView::SetWebSqlPath(const wchar_t* path)
{
	_itframeview->SetWebSqlPath(path);
}
void LTFrameView::SetLocalStoragePath(const wchar_t* path)
{
	_itframeview->SetLocalStoragePath(path);
}
LTEncodedJSValue LTFrameView::RunJavaScript(const char* script)
{
	return _itframeview->RunJavaScript(script);
}
LTEncodedJSValue LTFrameView::RunJavaScript(const wchar_t* script)
{
	return _itframeview->RunJavaScript(script);
}
LTExecState LTFrameView::GetGlobalExecState()
{
	return _itframeview->GetGlobalExecState();
}
HWND LTFrameView::windowHandle() const
{
	return _itframeview->windowHandle();
}
void LTFrameView::CloseWindow()
{
	_itframeview->CloseWindow();
}
void LTFrameView::QuitApp()
{
	_itframeview->QuitApp();
}
void LTFrameView::BindUserFunction(const char* name, LTNativeFunction fn, unsigned int argCount)
{
	_itframeview->BindUserFunction(name,fn,argCount);
}
void LTFrameView::GaussianBlurImage(HBITMAP hBitmap,int radius)
{
	_itframeview->GaussianBlurImage(hBitmap,radius);
}
void LTFrameView::SetAllowKeyDownAutoScroll(bool b)
{
	_itframeview->SetAllowKeyDownAutoScroll(b);
}
void LTFrameView::EnableDragFrameChangeSize(bool b)
{
	_itframeview->EnableDragFrameChangeSize(b);
}

