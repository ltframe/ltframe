#include "StdAfx.h"
#include "UITrayMenu.h"
#include "Shellapi.h"

UITrayMenu* UITrayMenuC=0;
DEFINEFUNCTION CloseWindowFun(LTExecState* es)
{
	UITrayMenuC->SendMessageToParentWindow(WM_CLOSEMAINWINDOW_MESSAGE,0,0);
	UITrayMenuC->applactionclose =1;
	return JsUndefined();
}
DEFINEFUNCTION OpenWebSiteFun(LTExecState* es)
{
	ShellExecute(NULL,L"open",L"http://www.ltplayer.com",NULL,NULL,SW_SHOW);
	UITrayMenuC->applactionclose =1;
	return JsUndefined();
}
DEFINEFUNCTION SetVolumeValueFun(LTExecState* es)
{
	int v = JsValue2Int(es,GetJsParameter(es,0));
	UITrayMenuC->SendMessageToParentWindow(WM_SETVOLUMEVALUE_MESSAGE,v,0);
	return JsUndefined();
}

DEFINEFUNCTION SynInterfaceFun(LTExecState* es)
{
	UITrayMenuC->SendMessageToParentWindow(WM_SETTRAYMENUVALUE_MESSAGE,0,0);
	return JsUndefined();
}

DEFINEFUNCTION PreviousMusicFun(LTExecState* es)
{
	UITrayMenuC->SendMessageToParentWindow(WM_SETPLAYMUISC_MESSAGE,0,2);
	return JsUndefined();
}

DEFINEFUNCTION PlayMusicFun(LTExecState* es)
{
	UITrayMenuC->SendMessageToParentWindow(WM_SETPLAYMUISC_MESSAGE,0,1);
	return JsUndefined();
}

DEFINEFUNCTION NextMusicFun(LTExecState* es)
{
	UITrayMenuC->SendMessageToParentWindow(WM_SETPLAYMUISC_MESSAGE,0,3);
	return JsUndefined();
}

DEFINEFUNCTION StopMusicFun(LTExecState* es)
{
    UITrayMenuC->SendMessageToParentWindow(WM_SETPLAYMUISC_MESSAGE,0,4);
	return JsUndefined();
}
DEFINEFUNCTION UserSettingFun(LTExecState* es)
{
    UITrayMenuC->SendMessageToParentWindow(WM_SETPLAYMUISC_MESSAGE,0,5);
	UITrayMenuC->applactionclose =1;
	return JsUndefined();
}

UITrayMenu::UITrayMenu():applactionclose(0)
{
}

UITrayMenu::~UITrayMenu(void)
{

}
void UITrayMenu::CreateTrayBox(HWND hwnd,char* applicationpath,int x,int y,int cx,int cy)
{
	parentHwnd = hwnd;
	ltskinview= CreateLTFrameInstance(this,L"UITrayMenu",hwnd,WS_POPUP | WS_VISIBLE,x,y,cx,cy,0);
	ltskinview->SetViewTransparent(true);
	ltskinview->SetAllowKeyDownAutoScroll(false);
	string _traypath = string(applicationpath)+string(TEMPLATEPATH)+"tray.html";
	wchar_t* traypath =CLTCommon::AnsiToWideChar(const_cast<char*>(_traypath.c_str())); 
	ltskinview->loadFile(traypath);
	free(traypath);
	ltskinview->EnableDragFrameChangeSize(false);
	UITrayMenuC = this;

	SetForegroundWindow(ltskinview->windowHandle());
	SetWindowPos(ltskinview->windowHandle(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	ltskinview->BindUserFunction("CloseWindow",CloseWindowFun,0);
	ltskinview->BindUserFunction("OpenWebSite",OpenWebSiteFun,0);
	ltskinview->BindUserFunction("SetVolumeValue",SetVolumeValueFun,1);
	ltskinview->BindUserFunction("SynInterface",SynInterfaceFun,0);
	ltskinview->BindUserFunction("PreviousMusic",PreviousMusicFun,0);
	ltskinview->BindUserFunction("PlayMusic",PlayMusicFun,0);
	ltskinview->BindUserFunction("StopMusic",StopMusicFun,0);
	ltskinview->BindUserFunction("NextMusic",NextMusicFun,0);
	ltskinview->BindUserFunction("UserSetting",UserSettingFun,0);
	//SendMessageToParentWindow(WM_SETTRAYMENUVALUE_MESSAGE,0,0);


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

int UITrayMenu::SendMessageToParentWindow(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  return PostMessage(parentHwnd,uMsg,wParam,lParam);
}


LRESULT UITrayMenu::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
			//case WM_ACTIVATE: 
			//{ 
			//	if(LOWORD(wParam)==WA_INACTIVE) 
			//	{ 
			//		PostMessage(ltskinview->windowHandle(),WM_USERWINDOW_ACTION_MESSAGE,0,0);
			//	} 
			//}
			//return 0;
			//break;
			//case WM_CREATE:
			//	SetForegroundWindow(hwnd);
			//	return 0;
			//break;
			//case WM_USERWINDOW_ACTION_MESSAGE:
			//	
			//	SendMessageToParentWindow(WM_SETWINDOWENABLE_MESSAGE,0,1);
			//	ShowWindow(ltskinview->windowHandle(),SW_HIDE);
			//	break;
			case WM_SETVOLUMEVALUE_MESSAGE:	
					SetVolume(wParam);
				break;
			case WM_SETTRAYCONFIG_MESSAGE:
					SetPlayStatus(wParam,lParam);
			break;
	}
	return ltskinview->HandleMessage(hwnd,uMsg,wParam,lParam);
}


HWND UITrayMenu::GetWindowHandle()
{
	return ltskinview->windowHandle();
}

LPCWSTR UITrayMenu::GetWindowClassName()
{
	return L"LTSkinUI";
}

void UITrayMenu::SetVolume(WPARAM wParam)
{
	char *outwebjson =new char[MAX_PATH+1];
	ZeroMemory(outwebjson,MAX_PATH+1);
	sprintf(outwebjson,"$(\"#volumerange\").val(%d)",wParam);
	ltskinview->RunJavaScript(outwebjson);
	delete [] outwebjson;
}

void UITrayMenu::SetPlayStatus(WPARAM wParam,LPARAM lParam)
{
	char* str = reinterpret_cast<char*>(wParam);	
	char *outwebjson =new char[MAX_PATH+1];
	ZeroMemory(outwebjson,MAX_PATH+1);

	sprintf(outwebjson,"SetPlayStatus('%s',%d)",str,lParam);

	wchar_t * jsstr = CLTCommon::AnsiToWideChar(outwebjson);

	ltskinview->RunJavaScript(jsstr);
	delete [] outwebjson;
	delete [] str;
	free(jsstr);
}


Json::Value UITrayMenu::GetJsonRoot(string s)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(s, root, false))
	{
		return 0;
	}
	return root;
}

int UITrayMenu::GetJavaScriptIntValue(string s)
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
