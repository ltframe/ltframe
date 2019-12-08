#include "StdAfx.h"
#include "UIContextMenu.h"
#include "UIClass.h"

UIContextMenu* UIContextMenuC;
DEFINEFUNCTION SetPlayFun(LTExecState* es)
{
	//MessageBox(0,0,0,0);
	UIContextMenuC->SendMessageToParentWindow(WM_CONTEXTPLAY_MESSAGE,1,0);
	UIContextMenuC->applactionclose =1;
	return JsUndefined();
}

DEFINEFUNCTION OpenFloderFun(LTExecState* es)
{

	UIContextMenuC->SendMessageToParentWindow(WM_CONTEXTPLAY_MESSAGE,2,0);
	UIContextMenuC->applactionclose =1;
	return JsUndefined();
}

DEFINEFUNCTION RemoveRecordFun(LTExecState* es)
{
	UIContextMenuC->SendMessageToParentWindow(WM_CONTEXTPLAY_MESSAGE,3,0);
	UIContextMenuC->applactionclose =1;
	return JsUndefined();
}

DEFINEFUNCTION EmptyListFun(LTExecState* es)
{
	UIContextMenuC->SendMessageToParentWindow(WM_CONTEXTPLAY_MESSAGE,4,0);
	UIContextMenuC->applactionclose =1;
	return JsUndefined();
}

UIContextMenu::UIContextMenu(void):applactionclose(0)
{
}

UIContextMenu::~UIContextMenu(void)
{
}

int UIContextMenu::SendMessageToParentWindow(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return PostMessage(parentHwnd,uMsg,wParam,lParam);
}


void UIContextMenu::CreateContextMenu(HWND hwnd,char* applicationpath,int x,int y,int cx,int cy)
{
	UIContextMenuC = this;
	parentHwnd = hwnd;
	ltskinview= CreateLTFrameInstance(this,L"UIContextMenu",hwnd,WS_POPUP,x,y,cx,cy,0);
	ltskinview->SetViewTransparent(true);
	ltskinview->SetAllowKeyDownAutoScroll(false);
	string _contextmenupath = string(applicationpath)+string(TEMPLATEPATH)+"contextmenu.html";
	wchar_t* contextmenupath =CLTCommon::AnsiToWideChar(const_cast<char*>(_contextmenupath.c_str())); 
	ltskinview->loadFile(contextmenupath);
	free(contextmenupath);
	ltskinview->EnableDragFrameChangeSize(false);
	SetForegroundWindow(ltskinview->windowHandle());
	ltskinview->BindUserFunction("SetPlay",SetPlayFun,0);
	ltskinview->BindUserFunction("OpenFloder",OpenFloderFun,0);
    ltskinview->BindUserFunction("RemoveRecord",RemoveRecordFun,0);
	ltskinview->BindUserFunction("EmptyList",EmptyListFun,0);

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

	DestroyWindow(ltskinview->windowHandle());
	

}
HWND UIContextMenu::GetWindowHandle()
{
   return ltskinview->windowHandle();
}

LRESULT UIContextMenu::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	if (uMsg == WM_MOUSEACTIVATE)
	{
		return MA_NOACTIVATE;
	}
	return ltskinview->HandleMessage(hwnd,uMsg,wParam,lParam);
}

LPCWSTR UIContextMenu::GetWindowClassName()
{
	return L"LTSkinUI";
}

//LPCWSTR UIContextMenu::MySelfDefinedJavaScriptFunction(const wchar_t* str)
//{
//	char a[4000];
//	strcpy(a,CLTCommon::ConvertWCharToChar((wchar_t*)str));
//	Json::Value obj = GetJsonRoot(string(a));
//	switch(GetJavaScriptIntValue(obj["action"].asString()))
//	{
//		case 1:
//			ParentInstance->RemoveRecord();
//		break;
//		case 2:
//			ParentInstance->OpenFileFolder();
//			break;
//		case 3:
//			ParentInstance->EmptyList();
//			break;
//		case 4:
//			ParentInstance->MusicPlay();
//			break;
//
//	}
//	return L"1";
//}
//
//Json::Value UIContextMenu::GetJsonRoot(string s)
//{
//	Json::Reader reader;
//	Json::Value root;
//	if (!reader.parse(s, root, false))
//	{
//		return 0;
//	}
//	return root;
//}

int UIContextMenu::GetJavaScriptIntValue(string s)
{
	map<string,int>::iterator it;
    for(it=JavaScriptMap.begin();it!=JavaScriptMap.end();++it)
	{
		if(it->first==s){
			return it->second;
		}
	}
    return   0;
}
