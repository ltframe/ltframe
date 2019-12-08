#include "StdAfx.h"
#include "UISetting.h"
#include "UIClass.h"
UISetting::UISetting(UIClass* p)
{
	ParentInstance = reinterpret_cast<UIClass*>(p);
}

UISetting::~UISetting(void)
{
}



void UISetting::CreateSettingBox(HWND hwnd,int x,int y,int cx,int cy)
{
	ltskinview= CreateLTFrameInstance(this,L"pppppppppp",hwnd,WS_POPUP | WS_VISIBLE,x,y,cx,cy,0);
	ltskinview->SetViewTransparent(false);
	ltskinview->loadFile(L"D:/website/set.html");
	ltskinview->MessageLoop();
}

LRESULT UISetting::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{

	}
	return ltskinview->HandleMessage(hwnd,uMsg,wParam,lParam);
}

LPCWSTR UISetting::GetWindowClassName()
{
	return L"LTSkinUI";
}