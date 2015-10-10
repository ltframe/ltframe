#include "StdAfx.h"
#include "UIContextMenu.h"
#include "UIClass.h"

UIContextMenu* UIContextMenuC;
DEFINEFUNCTION ShowMenuClickMessageFun(LTExecState* es)
{



    int x = JsValue2StringW(es,GetJsParameter(es,0),0);
	wchar_t * ret1=(wchar_t*)malloc((x+1)*sizeof(wchar_t)); 
	ZeroMemory(ret1,x);
	JsValue2StringW(es,GetJsParameter(es,0),ret1);
	MessageBox(0,ret1,0,0);
	free(ret1);
	UIContextMenuC->applactionclose =1;
	return JsUndefined();
}


UIContextMenu::UIContextMenu(void):applactionclose(0)
{
}

UIContextMenu::~UIContextMenu(void)
{
}
void UIContextMenu::CreateContextMenu(HWND hwnd,int x,int y,int cx,int cy)
{
	HMODULE hModule = ::GetModuleHandle(0);
	applicationpath  = (wchar_t*)malloc((MAX_PATH+1)*sizeof(wchar_t)); 
	GetModuleFileName(NULL, applicationpath, MAX_PATH);
	(wcsrchr(applicationpath, '\\'))[1] = 0;

	OleInitialize(0);

	UIContextMenuC = this;
	ltskinview= new LTFrameView(this,L"UIContextMenu",hwnd,WS_POPUP,x,y,cx,cy,0);


	SetWindowText(ltskinview->windowHandle(),L"LTFrame-ModelWindow");	


	wstring path = wstring(applicationpath)+L"./template/ContextMenu.html";
	ltskinview->loadFile(path.c_str());
	ltskinview->SetAllowKeyDownAutoScroll(false);
	ltskinview->EnableDragFrameChangeSize(false);
	SetForegroundWindow(ltskinview->windowHandle());
	
	ltskinview->BindUserFunction("ShowMenuClickMessage",ShowMenuClickMessageFun,1);

	while(true)
	{		
		if(applactionclose)
		{
			break;
		}
		if(GetForegroundWindow() != ltskinview->windowHandle())	
		{
			break;
		}
		BOOL bInterceptOther(FALSE);
		MSG msg = {0};
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_LBUTTONDOWN
				|| msg.message  == WM_RBUTTONDOWN
				|| msg.message  == WM_NCLBUTTONDOWN
				|| msg.message  == WM_NCRBUTTONDOWN
				||msg.message   ==WM_LBUTTONDBLCLK)
			{
				if(msg.hwnd != ltskinview->windowHandle())
				{
					::PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
					break;
				}
			}

			if (msg.message == WM_MOUSEMOVE)
			{
				if(msg.hwnd != ltskinview->windowHandle())
				{
					bInterceptOther=TRUE;
				}
			}

			if(!bInterceptOther){
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}
		else
		{
			MsgWaitForMultipleObjects (0, 0, 0, 10, QS_ALLINPUT);
		}
	}

	ltskinview->CloseWindow();
	

}
HWND UIContextMenu::GetWindowHandle()
{
   return ltskinview->windowHandle();
}

LRESULT UIContextMenu::HandleUserMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (uMsg == WM_MOUSEACTIVATE)
	{
		return MA_NOACTIVATE;
	}
	return ltskinview->HandleMessage(hwnd,uMsg,wParam,lParam);
}

LPCWSTR UIContextMenu::GetWindowClassName()
{
	return L"UIContextMenu";
}
