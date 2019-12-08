#include "stdafx.h"

#include "UIClass.h"
#include "Commdlg.h"
#include "Shellapi.h"
#include <Shlwapi.h>
#include "DropFile.h"
#include "NetClass.h"
#include <process.h> /* _beginthread, _endthread */
#include "base64class.h"
#include "urlcodeing.h"
#include <list>
#include<gdiplus.h>//gdi+头文件
#include "tagHelper.h" 
using namespace Gdiplus;
using namespace std;

//typedef void (*WindowClassName)();
//wchar_t* GetMianWindowClassName();
//wchar_t* GetContextWindowClassName();
//
//wchar_t* GetMianWindowClassName(){
//	
//	return L"";
//}
//wchar_t* GetContextWindowClassName()
//{
//	return L"";
//}

#define CONVERPAHT  "UserData\\cover\\src\\"
#define CONVERBLURPATH "UserData\\cover\\blur\\"

#define MEDIATYPE "mp3,wma,ogg,ape"
#define TRAYMENUCX 116
#define TRAYMENUCY 188

void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget,UIClass * inst);
void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget);
static char* applicationpath;
ULONG_PTR pGdiToken;
UIClass* UIClassC=0;

DEFINEFUNCTION DeleteNotifyIconFun(LTExecState* es)
{
	UIClassC->DeleteNotifyIcon();
	return JsUndefined();
}

DEFINEFUNCTION ShowNotifyIconFun(LTExecState* es)
{
	UIClassC->ShowNotifyIcon();
	return JsUndefined();
}


DEFINEFUNCTION DragWindowFun(LTExecState* es)
{
	UIClassC->DragWindow();
	return JsUndefined();
}

DEFINEFUNCTION SetLayerWindowFun(LTExecState* es)
{
	bool t = JsValue2Boolean(es,GetJsParameter(es,0));
	UIClassC->SetLayerWindow(t);
	return JsUndefined();
}

DEFINEFUNCTION QuitAppFun(LTExecState* es)
{
	UIClassC->QuitApp();
	return JsUndefined();
}


DEFINEFUNCTION OpenFileDialogFun(LTExecState* es)
{
	UIClassC->OpenFileDialog();
	return JsUndefined();
}

DEFINEFUNCTION MinWindowFun(LTExecState* es)
{
	UIClassC->MinWindow();
	return JsUndefined();
}

DEFINEFUNCTION GetWindowSizeFun(LTExecState* es)
{
	RECT rect;
	rect = UIClassC->GetCurrentRect();
	LTEncodedJSValue value = CreateObject(es);
	int width = rect.right-rect.left;
	int height = rect.bottom-rect.top;
	CreateJsObject(es,value,"width",Int2JsValue(width));
	CreateJsObject(es,value,"height",Int2JsValue(height));

	return value;
}

DEFINEFUNCTION MusicPlayFun(LTExecState* es)
{


	ZeroMemory(UIClassC->CurrentPlayerMusicInfo.title,1000);
	ZeroMemory(UIClassC->CurrentPlayerMusicInfo.author,100);
	ZeroMemory(UIClassC->CurrentPlayerMusicInfo.times,100);
	ZeroMemory(UIClassC->CurrentPlayerMusicInfo.path,MAX_PATH);





	int filenamecount= JsValue2String(es,GetJsObjectParameter(es,0,"filename"),0);
	char * filename=(char*)malloc(filenamecount);
	ZeroMemory(filename,filenamecount);
	JsValue2String(es,GetJsObjectParameter(es,0,"filename"),filename);
	strcpy(UIClassC->CurrentPlayerMusicInfo.title,filename);



	int authorcount= JsValue2String(es,GetJsObjectParameter(es,0,"author"),0);
	char * author=(char*)malloc(authorcount);
	ZeroMemory(author,authorcount);
	JsValue2String(es,GetJsObjectParameter(es,0,"author"),author);
	strcpy(UIClassC->CurrentPlayerMusicInfo.author,author);



	int filetimecount= JsValue2String(es,GetJsObjectParameter(es,0,"filetime"),0);
	char * filetime=(char*)malloc(filetimecount);
	ZeroMemory(filetime,filetimecount);
	JsValue2String(es,GetJsObjectParameter(es,0,"filetime"),filetime);
	strcpy(UIClassC->CurrentPlayerMusicInfo.times,filetime);


	int filepathcount= JsValue2String(es,GetJsObjectParameter(es,0,"filepath"),0);
	char * filepath=(char*)malloc(filepathcount);
	ZeroMemory(filepath,filepathcount);
	JsValue2String(es,GetJsObjectParameter(es,0,"filepath"),filepath);
	strcpy(UIClassC->CurrentPlayerMusicInfo.path,filepath);


	//int x = JsValue2String(es,GetJsParameter(es,0),0);
	//char * ret1=(char*)malloc(x);
	//ZeroMemory(ret1,x);
	//JsValue2String(es,GetJsParameter(es,0),ret1);





	//int k= JsValue2String(es,GetJsParameter(es,1,"b"),0);
	//char * ret2=(char*)malloc(k);
	//ZeroMemory(ret2,k);
	//JsValue2String(es,GetJsObjectParameter(es,1,"b"),ret2);


	//int k= JsValue2String(es,GetJsParameter(es,1),0);
	//char * ret2=(char*)malloc(k);
	//ZeroMemory(ret2,k);
	//JsValue2String(es,GetJsParameter(es,1),ret2);

	//////ret2[k] = '\0';
	UIClassC->MusicPlay(filepath,filename);
	free(filename);
	free(author);
	free(filetime);
	free(filepath);
	return JsUndefined();
}

DEFINEFUNCTION MusicStopFun(LTExecState* es)
{

	UIClassC->MusicStop();
	return JsUndefined();
}

DEFINEFUNCTION WriteFileFun(LTExecState* es)
{

	int x = JsValue2String(es,GetJsParameter(es,0),0);
	char * ret1=(char*)malloc(x);
	ZeroMemory(ret1,x);
	JsValue2String(es,GetJsParameter(es,0),ret1);
	string path = string(applicationpath)+string(MUSICLIST_DATA);
	
	int isappend =JsValue2Int(es,GetJsParameter(es,1));


	
	char* writestr =new char[x];
	ZeroMemory(writestr,x);
	char* bbb = CLTCommon::GetFile((char*)path.c_str());
	if(bbb==0 || strcmp(bbb,"")==0)
	{
		strncpy(writestr,ret1+1,strlen(ret1)-1);
		UIClassC->WriteFile(writestr,(char*)path.c_str(),isappend);
	}
	else
	{
		UIClassC->WriteFile(ret1,(char*)path.c_str(),isappend);
	}
	delete[] bbb;	
	free(ret1);
	delete [] writestr;
	return JsUndefined();
}


DEFINEFUNCTION GetFileFun(LTExecState* es)
{
	UIClassC->GetFile();
	return JsUndefined();
}

DEFINEFUNCTION OpenFileFolderFun(LTExecState* es)
{
	int x = JsValue2String(es,GetJsParameter(es,0),0);
	char * ret1=(char*)malloc(x);
	ZeroMemory(ret1,x);
	JsValue2String(es,GetJsParameter(es,0),ret1);

	UIClassC->OpenFileFolder(ret1);
	free(ret1);

	return JsUndefined();
}

DEFINEFUNCTION ShowUserListContextMenuFun(LTExecState* es)
{
	UIClassC->ShowUserListContextMenu(JsValue2Int(es,GetJsParameter(es,0)),JsValue2Int(es,GetJsParameter(es,1)),JsValue2Int(es,GetJsParameter(es,2)),JsValue2Int(es,GetJsParameter(es,3)));
	return JsUndefined();
}


DEFINEFUNCTION CreateGUIDFun(LTExecState* es)
{
	wchar_t* a = UIClassC->CreateGUID();
	LTEncodedJSValue arg = String2JsValue(es,a);
	free(a);
	return arg; 
}

DEFINEFUNCTION SetMusicPositionFun(LTExecState* es)
{
	UIClassC->SetMusicPosition(JsValue2Double(es,GetJsParameter(es,0)));
	return JsUndefined();
}

DEFINEFUNCTION  WriteAppConfigFileFun(LTExecState* es)
{
	int x = JsValue2String(es,GetJsParameter(es,0),0);
	char * ret2=(char*)malloc(x+1);
	ZeroMemory(ret2,x+1);
	JsValue2String(es,GetJsParameter(es,0),ret2);
	string path = string(applicationpath)+string(PLAYCONFIG_DATA);

	UIClassC->WriteAppConfigFile(ret2,(char*)path.c_str(),0);
	free(ret2);

	LTEncodedJSValue fun = GetJsParameter(es,1);
	JsCall(es,fun,0,0);

	return JsUndefined();
}

DEFINEFUNCTION  SetMusicVolumeFun(LTExecState* es)
{
	UIClassC->SetMusicVolume(JsValue2Double(es,GetJsParameter(es,0)));
	return JsUndefined();
}

DEFINEFUNCTION  ShowSettingBoxFun(LTExecState* es)
{
	UIClassC->ShowSettingBox(JsValue2Int(es,GetJsParameter(es,0)),JsValue2Int(es,GetJsParameter(es,0)),JsValue2Int(es,GetJsParameter(es,0)),JsValue2Int(es,GetJsParameter(es,0)));
	return JsUndefined();
}

DEFINEFUNCTION LoadConfigVarFun(LTExecState* es)
{
   char* str =  UIClassC->LoadConfigVar();
   
   LTEncodedJSValue fun = GetJsParameter(es,0);
   wchar_t* strw = CLTCommon::AnsiToWideChar(str);
   LTEncodedJSValue arg1 =String2JsValue(es,strw==0?L"":strw);
   LTEncodedJSValue  ptarg[1] = {arg1};
   JsCall(es,fun,ptarg,1); 
   delete [] str;
   free(strw);
   //CLTCommon::FreeHeap(str);
   //CLTCommon::FreeHeap(strw);
	return JsUndefined();
}

DEFINEFUNCTION InitAppConfigInfoFun(LTExecState* es)
{
	UIClassC->InitAppConfigInfo();
	return JsUndefined();
}

DEFINEFUNCTION CloseAppFun(LTExecState* es)
{

	UIClassC->CloseApp();
	//LTEncodedJSValue fun = GetJsParameter(es,0);
	//JsCall(es,fun,0,0);
	return JsUndefined();
}

DEFINEFUNCTION GetMusicInfoFun(LTExecState* es)
{

	int x = JsValue2String(es,GetJsParameter(es,0),0);
	char * ret1=(char*)malloc(x);
	ZeroMemory(ret1,x);
	JsValue2String(es,GetJsParameter(es,0),ret1);

	MusicBasicInfo info = UIClassC->GetMusicInfo(ret1);

    wchar_t* strw = CLTCommon::AnsiToWideChar(info.times);
	wchar_t* w_author = CLTCommon::AnsiToWideChar(info.author);
	wchar_t* w_title = CLTCommon::AnsiToWideChar(info.title);

    LTEncodedJSValue times =String2JsValue(es,strw);
	LTEncodedJSValue author =String2JsValue(es,w_author);
	LTEncodedJSValue title =String2JsValue(es,w_title);


	LTEncodedJSValue value = CreateObject(es);
	CreateJsObject(es,value,"times",times);
	CreateJsObject(es,value,"title",title);
	CreateJsObject(es,value,"author",author);
	free(ret1);
	free(strw);
	free(w_author);
	free(w_title);
	return value;
}

DEFINEFUNCTION  EnableDragFrameChangeSizeFun(LTExecState* es)
{
	bool b = JsValue2Boolean(es,GetJsParameter(es,0));
	UIClassC->EnableDragFrameChangeSize(b);
	return JsUndefined();
}

UIClass::UIClass() : uiset(0),hModule(0),MainWindowEnableStatus(1)
{	

	GdiplusStartupInput gdiplusStartupInput;
	
	GdiplusStartup(&pGdiToken,&gdiplusStartupInput,NULL);//初始化GDI+
	

	UIClassC =this;
    hModule = ::GetModuleHandle(0);
	applicationpath  = (char*)malloc((MAX_PATH+1)/**sizeof(char)*/); 
	GetModuleFileNameA(NULL, applicationpath, MAX_PATH);
	(strrchr(applicationpath, '\\'))[1] = 0;
	/*
	if(!CLTCommon::CheckAllTemplateFile(applicationpath)){
		 MessageBox(0,L"程序所需文件校验失败,程序将退出",0,0);
		 free(applicationpath);
		 return ;
	}*/
	char* str =  UIClassC->LoadConfigVar();
    int ww=0,wh=0; 
	if(str){
		Json::Value obj = GetJsonRoot(string(str));
		ww = obj["interface"]["windowwidth"].asInt();
		wh = obj["interface"]["windowheight"].asInt();	
	}
	delete [] str;
	if (ww==0)ww=307;
	if (wh==0)wh=400;
	cltcommon =new CLTCommon();
	OleInitialize(0);
	RECT rect = CenterWindow(ww,wh);
	icon = ::LoadIcon(hModule,MAKEINTRESOURCE(IDI_LTPLAYER));
	ltskinview= CreateLTFrameInstance(this,L"静静聆听音乐播放器",NULL,WS_POPUP | WS_VISIBLE,rect.left,rect.top,ww,wh,icon);
	ltskinview->SetViewTransparent(false);
	ltskinview->SetAllowKeyDownAutoScroll(false);
	ltskinview->BindUserFunction("DragWindow",DragWindowFun,0);
	ltskinview->BindUserFunction("SetLayerWindow",SetLayerWindowFun,1);
	ltskinview->BindUserFunction("QuitApp",QuitAppFun,0);
	ltskinview->BindUserFunction("CloseApp",CloseAppFun,0);
	ltskinview->BindUserFunction("OpenFileDialog",OpenFileDialogFun,0);
	ltskinview->BindUserFunction("MinWindow",MinWindowFun,0);
	ltskinview->BindUserFunction("MusicPlayNow",MusicPlayFun,2);
	ltskinview->BindUserFunction("MusicStop",MusicStopFun,0);
	ltskinview->BindUserFunction("WriteFile",WriteFileFun,3);
	ltskinview->BindUserFunction("GetFile",GetFileFun,0);
	ltskinview->BindUserFunction("OpenFileFolder",OpenFileFolderFun,1);
	ltskinview->BindUserFunction("ShowUserListContextMenu",ShowUserListContextMenuFun,4);
	ltskinview->BindUserFunction("CreateGUID",CreateGUIDFun,0);
	ltskinview->BindUserFunction("SetMusicPosition",SetMusicPositionFun,1);
	ltskinview->BindUserFunction("WriteAppConfigFile",WriteAppConfigFileFun,2);
	ltskinview->BindUserFunction("SetMusicVolume",SetMusicVolumeFun,1);
	ltskinview->BindUserFunction("ShowSettingBox",ShowSettingBoxFun,4);
	ltskinview->BindUserFunction("InitAppConfigInfo",InitAppConfigInfoFun,0);
	ltskinview->BindUserFunction("LoadConfigVar",LoadConfigVarFun,1);
    ltskinview->BindUserFunction("GetWindowSize",GetWindowSizeFun,0);
	ltskinview->BindUserFunction("GetMusicInfo",GetMusicInfoFun,1);

	ltskinview->BindUserFunction("DeleteNotifyIcon",DeleteNotifyIconFun,0);
    ltskinview->BindUserFunction("ShowNotifyIcon",ShowNotifyIconFun,0);
	ltskinview->BindUserFunction("EnableChangeWindowSize",EnableDragFrameChangeSizeFun,1);
	//ltskinview->SetWebSqlPath();
	//ltskinview->SetLocalStoragePath();
	ltskinview->IsAutoGC(true);
	SetWindowText(ltskinview->windowHandle(),L"静静聆听音乐播放器");
	playclass= new PlayClass(ltskinview->windowHandle());
	pDropTarget = new DropFile(ltskinview->windowHandle(),this);
	string _indexpath = string(applicationpath)+string(TEMPLATEPATH)+"index.html";
	wchar_t* indexpath =CLTCommon::AnsiToWideChar(const_cast<char*>(_indexpath.c_str())); 
	ltskinview->loadFile(indexpath);
	free(indexpath);



	ltskinview->MessageLoop();


}


UIClass::~UIClass(void)
{
	OleUninitialize();
	free(applicationpath);
	delete ltskinview;
}


MusicBasicInfo UIClass::GetMusicInfo(char* charfilepath)
{
	return playclass->GetMusicBasicInformation(charfilepath);
}

void UIClass::EnableDragFrameChangeSize(bool b)
{
	ltskinview->EnableDragFrameChangeSize(b);
}

RECT UIClass::CenterWindow(int w,int h)
{
	int scrWidth,scrHeight;
	RECT rect;
	//获得屏幕尺寸
	scrWidth=GetSystemMetrics(SM_CXSCREEN);
	scrHeight=GetSystemMetrics(SM_CYSCREEN);
	//获取窗体尺寸
	rect.left=(scrWidth-w)/2;
	rect.top=(scrHeight-h)/2;
	return rect;
}


LRESULT UIClass::HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_MYCLOSEWINDOW_MESSAGE:
		OnUserSelfMessageNOTIFY(wParam,lParam);
	    break;
	//case WM_DROPFILES:
	//	DropFiles(wParam);
		//return 1;
		//break;
	case WM_GETMINMAXINFO:
		GetMinMaxInfo((MINMAXINFO*)lParam);
		break;
	case WM_TIMER:
		 TimePRO(wParam);
		 break;
	case WM_SETCONVER_MESSAGE:
		{
			string* str = reinterpret_cast<string*>(wParam);			
			char *outwebjson =new char[MAX_PATH+1];
			ZeroMemory(outwebjson,MAX_PATH+1);
			sprintf(outwebjson,"setcover('%s')",(*str).c_str());
			ltskinview->RunJavaScript(outwebjson);
			delete [] outwebjson;
			delete str;
		}
		break;
	case WM_CLOSEMAINWINDOW_MESSAGE:
		{
			//string* str = reinterpret_cast<string*>(wParam);			
			//char *outwebjson =new char[MAX_PATH+1];
			//ZeroMemory(outwebjson,MAX_PATH+1);
			//sprintf(outwebjson,"closewindow('%s')",(*str).c_str());
			ltskinview->RunJavaScript("closewindow(0)");
			//delete [] outwebjson;
			//delete str;
		}
		return 0;
		break;
	case WM_SETVOLUMEVALUE_MESSAGE:	{	
			char *outwebjson =new char[MAX_PATH+1];
			ZeroMemory(outwebjson,MAX_PATH+1);
			sprintf(outwebjson,"ChangeMusicVolume(%d)",wParam);
			ltskinview->RunJavaScript(outwebjson);
			delete [] outwebjson;
									}
		break;
	case WM_SETTRAYMENUVALUE_MESSAGE:
		{
			SetTrayMenuVar();
		}
		break;
	case WM_SETWINDOWENABLE_MESSAGE:
		{
			MainWindowEnableStatus = lParam;
		}
		break;

	case WM_MOUSEMOVE:   
    case WM_MOUSEWHEEL:
		{
			if(!MainWindowEnableStatus)
			{
				return 0;
			}
		}
		break;
	case WM_CONTEXTPLAY_MESSAGE:
		ContextMenuOperate(wParam);
		break;

	case WM_SETPLAYMUISC_MESSAGE:
		{
			if(lParam==1){ltskinview->RunJavaScript(L"playevent('playbuttonpanel')");}
			if(lParam==2){ltskinview->RunJavaScript(L"preevent()");}
			if(lParam==3){ltskinview->RunJavaScript(L"nextevent()");}
			if(lParam==4){ltskinview->RunJavaScript(L"playevent('')");}
			if(lParam==5){
				EnableWindow(ltskinview->windowHandle(),false);
				::ShowWindow(ltskinview->windowHandle(),SW_SHOW);
				ltskinview->RunJavaScript(L"settingboxshow('setting')");
				EnableWindow(ltskinview->windowHandle(),true);
			}
		}
		break;
	}
	return ltskinview->HandleMessage(hwnd,uMsg,wParam,lParam);
}

void UIClass::ContextMenuOperate(WPARAM t)
{
	switch (t)
	{
		case 1:
		  ltskinview->RunJavaScript(L"extern_contextmenuplay()");
		break;
		case 2:
			ltskinview->RunJavaScript("extern_contextmenuopenfilefloder()");
		break;
		case 3:
			ltskinview->RunJavaScript("deletemusicfrommusiclist()");
		break;
		case 4:
			ltskinview->RunJavaScript("emptylist()");
			break;
	}
}


void UIClass::QuitApp()
{
	//PostQuitMessage(0);
	ltskinview->QuitApp();
}

void UIClass::CloseApp()
{
	//::CloseWindow(ltskinview->windowHandle());
	
	//ltskinview->CloseWindow();
//	DestroyWindow(ltskinview->windowHandle());
	DeleteNotifyIcon();
	CloseTrayMenu();
	ltskinview->CloseWindow();
}

RECT UIClass::GetCurrentRect()
{
	RECT rect;
	GetWindowRect(ltskinview->windowHandle(),&rect);
	return rect;
}

void UIClass::GetFile()
{
	string   path = string(applicationpath)+string(MUSICLIST_DATA);
	
	char* bbb = CLTCommon::GetFile((char*)path.c_str());
	if(!bbb)
	{
		free(bbb);
		return;
	}


	char *outwebjson =new char[strlen(bbb)+100];
	ZeroMemory(outwebjson,strlen(bbb)+100);
	sprintf(outwebjson,"addtomusiclist('%s',false)",bbb);

	//char * t =new char[strlen(outwebjson)+1];
	//ZeroMemory(t,strlen(outwebjson)+1);
	//strcpy(t,outwebjson);
	wchar_t * jsstr = cltcommon->AnsiToWideChar(outwebjson);
	ltskinview->RunJavaScript(jsstr);
	 free(bbb);
	free(jsstr);
	delete [] outwebjson;
}

void UIClass::WriteAppConfigFile(char* x,char * p,int isappend)
{


	CLTCommon::WriteFile(x,p,isappend);
	
}

void UIClass::TestJs()
{
	LTEncodedJSValue jsv =  ltskinview->RunJavaScript("TestJs()");
	LTExecState exec = ltskinview->GetGlobalExecState();
	LTEncodedJSValue v = GetJsObjectAttribute(exec,jsv,"bbb");
	LTEncodedJSValue xc = GetJsObjectAttribute(exec,v,"kkk");
	int x = JsValue2Int(exec,xc);
	
}

bool UIClass::OnDragLeave()
{
	return pDropTarget->DragLeave();
}
bool UIClass::OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return pDropTarget->DragEnter(pDataObject,grfKeyState,pt,pdwEffect);
}

bool UIClass:: OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return pDropTarget->DragOver(grfKeyState, pt, pdwEffect);
}
bool UIClass:: OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return pDropTarget->Drop(pDataObject,grfKeyState,pt,pdwEffect);
}

void UIClass::DropEnter(PVOID data)
{
	if(data==0)
	{
		ltskinview->RunJavaScript("showdragfilemask(false)");
		return;
	}
	UINT count;
	wchar_t filePath[MAX_PATH];
	count = DragQueryFile((HDROP)data, 0xFFFFFFFF, NULL, 0);
	for(UINT i=0; i<count; i++)
	{
		int pathLen = DragQueryFile((HDROP)data, i, NULL, 0);
		DragQueryFile((HDROP)data,i,filePath,pathLen+1);
		char * fileext = CLTCommon::GetFileExt(filePath);
		if(::PathIsDirectory(filePath) || !CLTCommon::IsStrInStrAarry(fileext,MEDIATYPE))
		{
			free(fileext);
			continue;
		}
		free(fileext);
		//if(CLTCommon::IsStrInStrAarry(""))
		char *outwebjson =new char[MAX_PATH];

		ZeroMemory(outwebjson, MAX_PATH);

		

		char* charfilepath =CLTCommon::ConvertWCharToChar(filePath);

		MusicBasicInfo musicbasicinfo = playclass->GetMusicBasicInformation(charfilepath);

		//char* ddddd = CLTCommon::ReplaceFilePath(ffff,"\\","/");

		if(count>1)
			sprintf(outwebjson,"showdragfilemask(\"您将要添加:%s等%d个文件\")",musicbasicinfo.title,count);
		else
			sprintf(outwebjson,"showdragfilemask(\"您将要添加:%s\")",musicbasicinfo.title);


		LPWSTR outwebjsonwstr =cltcommon->AnsiToWideChar(outwebjson);
		ltskinview->RunJavaScript(outwebjsonwstr);
		delete [] outwebjson;
		delete charfilepath;
		free(outwebjsonwstr);
		break;
	}
}
void UIClass::DropLeave()
{
	ltskinview->RunJavaScript("showdragfilemask(false)");
}

void UIClass::DropFiles(PVOID data)
{
	
	//list<char*> * c1 =new list<char*>(); 

	UINT count;
	HDROP hdrop = (HDROP)data;
	
	count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
	for(UINT i=0; i<count; i++)
	{
		wchar_t* filePath =new wchar_t[MAX_PATH];
		int pathLen = DragQueryFile(hdrop, i, NULL, 0);
		DragQueryFile(hdrop,i,filePath,pathLen+1);
		char * fileext = CLTCommon::GetFileExt(filePath);
		if(::PathIsDirectory(filePath) || !CLTCommon::IsStrInStrAarry(fileext,MEDIATYPE))
		{
			delete [] filePath;
			free(fileext);
			continue;
		}
		free(fileext);
		char* outwebjson =new char[10000];

		ZeroMemory(outwebjson, 10000);

	

		char* charfilepath =CLTCommon::ConvertWCharToChar(filePath);

		MusicBasicInfo musicbasicinfo = playclass->GetMusicBasicInformation(charfilepath);

	    char * replacepath = CLTCommon::ReplaceFilePath(charfilepath,"\\","/");
			sprintf(outwebjson,"setdragfileslist('{\"filename\":\"%s\",\"filepath\":\"%s\",\"filetime\":\"%s\",\
			\"author\":\"%s\",\"guid\":\"%s\"}')",musicbasicinfo.title,replacepath,musicbasicinfo.times,musicbasicinfo.author,CLTCommon::CreateGUID());
		LPWSTR outwebjsonwstr =cltcommon->AnsiToWideChar(outwebjson);
		ltskinview->RunJavaScript(outwebjsonwstr);

		//c1->push_back(outwebjson);

		delete [] outwebjson;
		free(charfilepath);
		delete []  replacepath;
		free(outwebjsonwstr);
		delete [] filePath;
	}
	//uintptr_t hStdOut = _beginthread(UIClass::test_addmusic, 0, (void*)c1);
	ltskinview->RunJavaScript("setdragfilemarktitleandpath()");
}

void UIClass::GetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 306;   //x宽度  
	lpMMI->ptMinTrackSize.y = 400;   //y高度  
	lpMMI->ptMaxTrackSize.x = 700;
	lpMMI->ptMaxTrackSize.y= 537;
}

LPCWSTR UIClass::GetWindowClassName()
{
	return L"LTSkinUI";
}
void UIClass::OpenFileDialog()
{
	OPENFILENAME ofn;
	TCHAR szOpenFileNames[80*MAX_PATH];
	TCHAR szPath[MAX_PATH];
	
	TCHAR* p;
	int nLen = 0;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ltskinview->windowHandle();
	ofn.lpstrFile = szOpenFileNames;
	ofn.nMaxFile = sizeof(szOpenFileNames);
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = TEXT("All Files(*.*)\0*.*\0");


	if( GetOpenFileName( &ofn ) )
	{  
		//把第一个文件名前的复制到szPath,即:
		//如果只选了一个文件,就复制到最后一个'\'
		//如果选了多个文件,就复制到第一个NULL字符
		lstrcpyn(szPath, szOpenFileNames, ofn.nFileOffset );
		//当只选了一个文件时,下面这个NULL字符是必需的.
		//这里不区别对待选了一个和多个文件的情况
		szPath[ ofn.nFileOffset ] = '\0';
		nLen = lstrlen(szPath);

		if( szPath[nLen-1] != '\\' )   //如果选了多个文件,则必须加上'\\'
		{
			lstrcat(szPath, TEXT("\\"));
		}

		p = szOpenFileNames + ofn.nFileOffset; //把指针移到第一个文件
		
		//strcat(outfile,"[");
		//int index=0;
	/*	char * _list =(char*)malloc(3000);
		ZeroMemory(_list,3000);*/

		//string _list("");

		while( *p )
		{   

			TCHAR szFileName[80*MAX_PATH];
			ZeroMemory(szFileName, sizeof(szFileName));

			lstrcat(szFileName, szPath);  //给文件名加上路径  
			lstrcat(szFileName, p);    //加上文件名  

			char* charfilepath =CLTCommon::ConvertWCharToChar(szFileName);
			MusicBasicInfo musicbasicinfo = playclass->GetMusicBasicInformation(charfilepath);

			char * _singlejson = (char*)malloc(10000);
			char * _rpath = CLTCommon::ReplaceFilePath(cltcommon->ConvertWCharToChar(szFileName),"\\","/");

			sprintf(_singlejson,"setdragfileslist('{\"filename\":\"%s\",\"filepath\":\"%s\",\"filetime\":\"%s\",\"author\":\"%s\",\"guid\":\"%s\"}')",musicbasicinfo.title,_rpath,musicbasicinfo.times,musicbasicinfo.author,CLTCommon::CreateGUID());

			LPWSTR outwebjsonwstr =cltcommon->AnsiToWideChar(_singlejson);
			ltskinview->RunJavaScript(outwebjsonwstr);


			free(_singlejson);
			delete _rpath;
			free(outwebjsonwstr);
			free(charfilepath);
			p += lstrlen(p) +1;     //移至下一个文件


			//index++;
		}
		//int filestrlength = _list.length();
		//_list[filestrlength-1]='\0';
		/*
		char *outwebjson =new char[filestrlength+100];
		ZeroMemory(outwebjson,filestrlength+100);
		sprintf(outwebjson,"addtomusiclist('%s',true)",_list.c_str());
		ltskinview->RunJavaScript(cltcommon->AnsiToWideChar(outwebjson));
		delete [] outwebjson;
		*/
		ltskinview->RunJavaScript("showdragfilemask('');setdragfilemarktitleandpath()");
	}
}

LPCWSTR UIClass::MySelfDefinedJavaScriptFunction(const wchar_t* str)
{
	
	char a[4000];
	strcpy(a,cltcommon->ConvertWCharToChar((wchar_t*)str));
	//char* a = ;
	Json::Value obj = GetJsonRoot(string(a));
	switch(GetJavaScriptIntValue(obj["action"].asString()))
	{
		case 1:
			//DragWindow();
		break;
		case 2:
			//SetLayerWindow(obj["type"].asBool());
			break;
		case 3:
			//ltskinview->QuitApp();
			break; 
		case 4:
			//OpenFileDialog();
			break; 
		case 5:
			//MinWindow();
			break; 
		case 6:
			MusicPlay(obj["filepath"].asCString(),obj["filename"].asCString());
			break; 
		case 7:
			//MusicStop();
			break;
		case 8:
			break; 
	    case 9:
			break; 
		case 10:
			break; 
		case 11:
			
			break; 
		case 12:
			{

			}
			break;
		case 13:
			{
				
			}
			break;
		case 14:
			//ShowUserListContextMenu(obj["x"].asInt(),obj["y"].asInt(),1,1);
			break;
		case 15:
			//CloseListContextMenu();
			break;
		case 16:
			//return CLTCommon::AnsiToWideChar((char*)CLTCommon::CreateGUID());
			break;
		case 17:
			{
	
			}
			break;
		case 18:
			//GetAPPConfigInfo(obj["configinfo"].asCString());
			break;
		case 19:
			
			break;
		case 20:{
			
				}
			break;
		case 21:
			//ShowSettingBox(obj["x"].asInt(),obj["y"].asInt(),obj["cx"].asInt(),obj["cy"].asInt());
			break;
		case 22:
			//LoadConfigVar();
			break;
		case 23:
			//playclass->SetMusicEffect(obj["effectid"].asInt());
			break;
	}
	
	return L"1";
}
wchar_t* UIClass::CreateGUID()
{
	return CLTCommon::AnsiToWideChar((char*)CLTCommon::CreateGUID());
}

void UIClass::WriteFile(char* a ,char* b,int isappend)
{
	CLTCommon::WriteFile(a,b,isappend);
}

void UIClass::SetMusicPosition(double v)
{
	playclass->SetMusicPosition(v);
}




BOOL CALLBACK SetMusicVolumeProc(HWND hwnd, LPARAM lParam)
{
    wchar_t NAME[255];  
	ZeroMemory(NAME,255);
	//wchar_t* _classname = NAME;
 //   wchar_t* _windowtext = NAME;
    INT nMaxCounte=255;  
	GetClassName(hwnd,NAME,nMaxCounte);
	if(!wcscmp(L"LTSkinUI",NAME)/* && !wcscmp(L"UITray",_windowtext)*/)
	{
		ZeroMemory(NAME,255);
		GetWindowText(hwnd,NAME,nMaxCounte);
		if(!wcscmp(L"UITrayMenu",NAME))
		{
			PostMessage(hwnd,WM_SETVOLUMEVALUE_MESSAGE,lParam,0);
			return false;
		}
	}
	return true;
}


void UIClass::SetMusicVolume(double v)
{
	playclass->SetMusicVolume(v/10);
	//if (traymenu!=0)
	//HWND uitrayhwnd = FindWindowEx(ltskinview->windowHandle(),0,,);
	////int x =GetLastError();

		EnumWindows(SetMusicVolumeProc,(LPARAM)v);
	//if(uitrayhwnd!=0)
		
}

char* UIClass::LoadConfigVar()
{
	char* bbb =0;
	string path = string(applicationpath)+string(PLAYCONFIG_DATA);
	bbb = CLTCommon::GetFile((char*)path.c_str());
	return bbb;
	//char outwebjson[3000] ={};
	//sprintf(outwebjson,"loadconfig('%s')",bbb);
	//ltskinview->RunJavaScript(cltcommon->AnsiToWideChar(outwebjson));
}
int UIClass::GetJavaScriptIntValue(string s)
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

Json::Value UIClass::GetJsonRoot(string s)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(s, root, false))
	{
		return 0;
	}
	return root;
}

void UIClass::DragWindow()
{
	ReleaseCapture();
	POINT DownPoint;
	GetCursorPos(&DownPoint);
	SendMessage(ltskinview->windowHandle(),WM_NCLBUTTONDOWN,HTCAPTION,MAKEWPARAM(DownPoint.x,DownPoint.y));
}

void UIClass::SetLayerWindow(bool type)
{
	ltskinview->SetViewTransparent(type);
}

void UIClass::MinWindow()
{
	::ShowWindow(ltskinview->windowHandle(),SW_HIDE);	
}

void UIClass::ShowNotifyIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=ltskinview->windowHandle();
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP ;
	nid.uCallbackMessage=WM_MYCLOSEWINDOW_MESSAGE;//自定义的消息名称
	nid.hIcon=LoadIcon(hModule,MAKEINTRESOURCE(IDI_LTPLAYER));
	//nid.hIcon=NULL;
	wcscpy(nid.szTip,_T("静静聆听音乐播放器"));//信息提示条为“计划任务提醒”
	Shell_NotifyIcon(NIM_ADD,&nid);//在托盘区添加图标
}


void UIClass::DeleteNotifyIcon()
{
	NOTIFYICONDATA nid;
	nid.hWnd=ltskinview->windowHandle();
	nid.uID=IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE,&nid);//在托盘区添加图标
}

long UIClass::OnUserSelfMessageNOTIFY(WPARAM wParam, LPARAM lParam)
{
	//if(wParam!=IDR_MAINFRAME)
	//	return 1;

		switch(lParam)
		{
			case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭”
				{
					int cx=GetSystemMetrics(SM_CXSCREEN); 
					int cy=GetSystemMetrics(SM_CYSCREEN); 
					RECT systembar;
					SystemParametersInfo(SPI_GETWORKAREA, 0, &systembar, 0);
							POINT point;
					::GetCursorPos(&point);
					if(systembar.left==0 && systembar.top==0 && systembar.right==cx){
						//printf("下");
						ShowTrayContextMenu(point.x-121,point.y-TRAYMENUCY,TRAYMENUCX,TRAYMENUCY);
					}
					if(systembar.left==0 && systembar.top==0 && systembar.bottom==cy){
						//printf("右");
						ShowTrayContextMenu(point.x-121,point.y-TRAYMENUCY,TRAYMENUCX,TRAYMENUCY);
					}
					if(systembar.left>0){
						ShowTrayContextMenu(point.x,point.y-TRAYMENUCY,TRAYMENUCX,TRAYMENUCY);
					}
					if(systembar.top>0){
						//printf("上");
						ShowTrayContextMenu(point.x-121,point.y,TRAYMENUCX,TRAYMENUCY);
					}
			
	/*				ClientToScreen(0,&point);*/
					//ShowTrayContextMenu(point.x-121,point.y-188,116,188);
				}
				break;
			case WM_LBUTTONDBLCLK://双击左键的处理
				{
					GetAPPConfigInfo();
					if(!appconfiginfo.alwaytray){
						DeleteNotifyIcon();
					} 
					::ShowWindow(ltskinview->windowHandle(),SW_SHOW);
					SetForegroundWindow(ltskinview->windowHandle());
				}
				break;
			default:
				break;
		}
	return 1;
}

std::wstring  StringToWString(const std::string& s)
{
	 std::wstring wszStr;

	 int nLength = MultiByteToWideChar( CP_ACP, 0, s.c_str(), -1, NULL, NULL );
	 wszStr.resize(nLength);
	 LPWSTR lpwszStr = new wchar_t[nLength];
	 MultiByteToWideChar( CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength );
	 wszStr = lpwszStr;
	 delete [] lpwszStr;

	 return wszStr;
}


void  UIClass::DonwloadCoverThread(void* uiobj)
{
	
	UIClass * uic = static_cast<UIClass*>(uiobj);
	tagHelper th(uic->CurrentPlayerMusicInfo.path);
	string _base64name = base64_encode(reinterpret_cast<const unsigned char*>(uic->CurrentPlayerMusicInfo.title),strlen(uic->CurrentPlayerMusicInfo.title));
	string _path = string(applicationpath)+string(CONVERPAHT)+_base64name;
	if(th.getAlbumart((char*)_path.c_str())){		
		string blurpath = string(applicationpath)+string(CONVERBLURPATH)+_base64name;
		wchar_t* src = CLTCommon::AnsiToWideChar((char*)_path.c_str());
		wchar_t* dsc = CLTCommon::AnsiToWideChar((char*)blurpath.c_str());
		uic->GaussianBlurImage(src,dsc);
		free(src);free(dsc);
		string* poststr =new string(_base64name);
		PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
		_endthread();
		return;
	}

	//string* poststr =new string("nocover.png");
	//PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
	//_endthread();
	//return;


	strCoding encode;

	NetClass netclass;
	wstring apiurl =wstring(L"http://api.douban.com/v2/music/search?q=");
	apiurl+=StringToWString(encode.UrlUTF8(uic->CurrentPlayerMusicInfo.title));
	string result = netclass.HttpRequest(apiurl);

	Json::Value obj = uic->GetJsonRoot(result);
	if(obj==0){

		string* poststr =new string("nocover.png");
		PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
		_endthread();
		return;
	}
	if (obj["total"].asInt()>20)
	{
		string* poststr =new string("nocover.png");
		PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
		_endthread();
		return;
	}
	else
	{
		int musiccount = obj["musics"].size();
		for(int i=0;i<musiccount;i++)
		{
			string title = obj["musics"][i]["title"].asString();
			//int  author = obj["musics"][i]["author"].size();
			string author = "";
			for(int a=0;a< obj["musics"][i]["author"].size();a++){
				author = obj["musics"][i]["author"][a]["name"].asString();
				if(author!="")break;
			}
			string base64name = base64_encode(reinterpret_cast<const unsigned char*>(uic->CurrentPlayerMusicInfo.title),strlen(uic->CurrentPlayerMusicInfo.title));
			string path = string(applicationpath)+string(CONVERPAHT)+base64name;
			string blurpath = string(applicationpath)+string(CONVERBLURPATH)+base64name;

			/*if ((title.substr(0,strlen(uic->CurrentPlayerMusicInfo.title))==uic->CurrentPlayerMusicInfo.title
                 && author==string(uic->CurrentPlayerMusicInfo.author)) || musiccount==1)*/
			if(((title.substr(0,strlen(uic->CurrentPlayerMusicInfo.title))==uic->CurrentPlayerMusicInfo.title ||
				string(uic->CurrentPlayerMusicInfo.title).substr(0,title.length())==title
				)&& author==string(uic->CurrentPlayerMusicInfo.author)) || musiccount==1)
			{
				string coverurl = obj["musics"][i]["image"].asString();
				CLTCommon::string_replace(coverurl,"/spic/","/mpic/");
				if(!netclass.InternetDownloadFile((char*)coverurl.c_str(),(char*)path.c_str()))
				{
					while(true)
					{
						
						WIN32_FIND_DATAA FindFileData;
						HANDLE hFind;
						hFind = FindFirstFileA(path.c_str(), &FindFileData);
						if (hFind != INVALID_HANDLE_VALUE) 
						{
							break;
						}
					}
					wchar_t* src = CLTCommon::AnsiToWideChar((char*)path.c_str());
					wchar_t* dsc = CLTCommon::AnsiToWideChar((char*)blurpath.c_str());
					uic->GaussianBlurImage(src,dsc);
					free(src);free(dsc);
					string* poststr =new string(base64name);
					PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
					_endthread();
					return;
					
				}else{			
					CLTCommon::string_replace(coverurl,"/mpic/","/spic/");
					if(!netclass.InternetDownloadFile((char*)coverurl.c_str(),(char*)path.c_str()))
					{
						while(true)
						{
							
							WIN32_FIND_DATAA FindFileData;
							HANDLE hFind;
							hFind = FindFirstFileA(path.c_str(), &FindFileData);
							if (hFind != INVALID_HANDLE_VALUE) 
							{
								break;
							}
						}
						wchar_t* src = CLTCommon::AnsiToWideChar((char*)path.c_str());
						wchar_t* dsc = CLTCommon::AnsiToWideChar((char*)blurpath.c_str());
						uic->GaussianBlurImage(src,dsc);
						free(src);free(dsc);
						string* poststr =new string(base64name);
						PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
						_endthread();
						return;

					}
				}
			}
		}		
	}
	string* poststr =new string("nocover.png");
	PostMessage(uic->ltskinview->windowHandle(),WM_SETCONVER_MESSAGE,(WPARAM)poststr,0);
	//switch(GetJavaScriptIntValue(obj["action"].asString()))
	_endthread();
	//return 0;
}

void UIClass::MusicPlay(const char* filepath,const char* filename)
{
	char showfilename[3000]={};
	strcat(showfilename,"静静聆听音乐播放器-");
	strcat(showfilename,filename);
	SetWindowTextA(ltskinview->windowHandle(),showfilename);

 
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind,hbgFind;
	string base64name = base64_encode(reinterpret_cast<const unsigned char*>(filename),strlen(filename));
	string path = string(applicationpath)+string(CONVERPAHT)+base64name;
	hFind = FindFirstFileA(path.c_str(), &FindFileData);

	string blurpath = string(applicationpath)+string(CONVERBLURPATH)+base64name;
	hbgFind = FindFirstFileA(blurpath.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE && hbgFind != INVALID_HANDLE_VALUE) 
	{

		FindClose(hFind);
		playclass->MusicPlay(filepath);
		::SetTimer(ltskinview->windowHandle(),MUSIC_TIMER,1000,NULL); 
		char *outwebjson =new char[path.length()+1];
		ZeroMemory(outwebjson,path.length()+1);
		sprintf(outwebjson,"setcover('%s')",base64name.c_str());
		ltskinview->RunJavaScript(outwebjson);
		delete [] outwebjson;
		SetTrayMenuVar();
		return;
	}

	playclass->MusicPlay(filepath);
	SetTrayMenuVar();
	::SetTimer(ltskinview->windowHandle(),MUSIC_TIMER,1000,NULL); 
    
	if(hFind == INVALID_HANDLE_VALUE)
	{
		uintptr_t hStdOut = _beginthread(UIClass::DonwloadCoverThread, 0, (void*)this);
	}
	/*
	DWORD dwThreadId;
	HANDLE hThread; 

	hThread= CreateThread(NULL,0,UIClass::DonwloadCoverThread,(LPVOID)filename,0,&dwThreadId);   
	while(TRUE)
	{

		DWORD result ; 
		MSG msg ; 

		result = MsgWaitForMultipleObjects(1, &hThread, 
			FALSE, 10000, QS_ALLINPUT); 

		if (result == (WAIT_OBJECT_0))
		{
			break;
		} 
		else 
		{ 
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);   
			}
		} 
	}
	CloseHandle(hThread);
	*/


}


void UIClass::MusicPlay()
{
	ltskinview->RunJavaScript("externmusicplay();");
}

void UIClass::MusicStop()
{
	playclass->MusicStop();
	SetTrayMenuVar();
}

void UIClass::SetCurrentMusicTime()
{
	char outfile[3000]={};
	char a[100]={};
	if (playclass->GetMusicStatus()==BASS_ACTIVE_STOPPED)
	{
		KillTimer(ltskinview->windowHandle(),MUSIC_TIMER); 
		ltskinview->RunJavaScript("extern_musicloop()");
		return;
	}
	strcpy(a,CLTCommon::Seconds2Time(playclass->CurrentMusicTime()));
	int k = playclass->GetSliderPre();
	sprintf(outfile,"setcurrentsliderandtime('%s',%d)",a,k);
	ltskinview->RunJavaScript(outfile);
	
}

void UIClass::TimePRO(WPARAM wParam)
{
	switch (wParam)
	{
	   case MUSIC_TIMER:
		  SetCurrentMusicTime();
	   break ;
	}
}

void UIClass::OpenFileFolder(char* path)
{
	//ShellExecuteA(NULL, "open", "explorer.exe",params, NULL, SW_SHOWNORMAL);
   // path = "C:\\Users\\Administrator\\Downloads\\爱的意义.mp3";
	char * a = 	CLTCommon::ReplaceFilePath(path,"/","\\");
	if(!CLTCommon::FileExists(a))
	{

		delete a;
		ltskinview->EnableDragFrameChangeSize(false);
		ltskinview->RunJavaScript(L"alertdialogshow('文件未找到',false,fileexistsalertokcallback,null)");
		return;
	}
	char * params =(char*)malloc(MAX_PATH+100);


	ZeroMemory(params,MAX_PATH+100);
	sprintf(params,"/select,%s",a);
	ShellExecuteA(NULL, "open", "explorer.exe",params, NULL, SW_SHOW);
	params[strlen(params)]='\0';
	delete a;
	free(params);
	

}
void UIClass::OpenFileFolder()
{
	//OpenFileFolder();
	ltskinview->RunJavaScript("openfilefolder()");
}
void UIClass::ShowUserListContextMenu(int x,int y,int cx,int cy)
{
	//if (uicm!=0)
	//{   

	//	uicm->SetWindowStatus(0);
	//	bool b = (GetWindowLong(uicm->GetWindowHandle(), GWL_STYLE) & WS_VISIBLE);

	//	if(!b)
	//	{
	//		::ShowWindow(uicm->GetWindowHandle(),SW_SHOW);
	//		::SetWindowPos(uicm->GetWindowHandle(),HWND_TOPMOST,x-30,y,cx,cy,SWP_NOSIZE);
	//		SetForegroundWindow(uicm->GetWindowHandle());
	//		MainWindowEnableStatus = 0;
	//		
	//	}
	//	else{
	//		::SetWindowPos(uicm->GetWindowHandle(),HWND_TOPMOST,x-30,y,cx,cy,SWP_NOSIZE);
	//		SetForegroundWindow(uicm->GetWindowHandle());
	//		MainWindowEnableStatus = 1;
	//	}

	//	

	//	return;
	//}
	int posy =y;
	int screency = GetSystemMetrics(SM_CYSCREEN);
	if(screency-y<cy)
	{
		posy = y-cy+23;
	}

	UIContextMenu* uicm =new UIContextMenu();
	uicm->CreateContextMenu(ltskinview->windowHandle(),applicationpath,x,posy,cx,cy);
}

void UIClass::SetMianWindowFocus()
{
	SetForegroundWindow(ltskinview->windowHandle());
}

void UIClass::SetTrayMenuVarProcFun(HWND hwnd){


	string s = "getglobalvar('currentrecordobject.filename')";

	LTEncodedJSValue jsv =  ltskinview->RunJavaScript(s.c_str());

	char * params =new char[4000];
	ZeroMemory(params,4000);
	LTExecState exec = ltskinview->GetGlobalExecState();
	JsValue2String(exec,jsv,params);

	s = "getglobalvar('CURRENTPLAYSTATUS')";
	LTEncodedJSValue pstatus = ltskinview->RunJavaScript(s.c_str());
	int st = JsValue2Int(exec,pstatus);
	PostMessage(hwnd,WM_SETTRAYCONFIG_MESSAGE,(WPARAM)params,(LPARAM)st);
	PostMessage(hwnd,WM_SETVOLUMEVALUE_MESSAGE,(WPARAM)(appconfiginfo.volumerange),0);
	//delete [] params;
}

BOOL CALLBACK SetTrayMenuVarProc(HWND hwnd, LPARAM lParam)
{
    wchar_t NAME[255];  
	ZeroMemory(NAME,255);
    INT nMaxCounte=255;  
	GetClassName(hwnd,NAME,nMaxCounte);
	if(!wcscmp(L"LTSkinUI",NAME)/* && !wcscmp(L"UITray",_windowtext)*/)
	{
		ZeroMemory(NAME,255);
		GetWindowText(hwnd,NAME,nMaxCounte);
		if(!wcscmp(L"UITrayMenu",NAME))
		{
			UIClass * uic =reinterpret_cast<UIClass*>(lParam);
			uic->SetTrayMenuVarProcFun(hwnd);
			return false;
		}
	}
	return true;
}

void UIClass::SetTrayMenuVar()
{
	GetAPPConfigInfo();
	/*
	HWND uitrayhwnd = FindWindowEx(ltskinview->windowHandle(),0,L"LTSkinUI",L"UITray");
	PostMessage(uitrayhwnd,WM_SETVOLUMEVALUE_MESSAGE,appconfiginfo.alwaytray,0);
	
	//if(playclass->IsPlayMusic()){
	if(true){
		string s = "getglobalvar('currentrecordobject.filename')";
		
		LTEncodedJSValue jsv =  ltskinview->RunJavaScript(s.c_str());

		char * params =(char*)malloc(4000);
		ZeroMemory(params,4000);
		LTExecState exec = ltskinview->GetGlobalExecState();
		JsValue2String(exec,jsv,params);
		PostMessage(uitrayhwnd,WM_SETTRAYCONFIG_MESSAGE,(WPARAM)params,0);
		free(params);
	}
	*/


	EnumWindows(SetTrayMenuVarProc,(LPARAM)this);
}

void UIClass::ShowTrayContextMenu(int x,int y,int cx,int cy)
{
	/*
	if (traymenu!=0)
	{
		//CloseTrayMenu();
		//delete traymenu;
		//traymenu = 0;
		bool b = (GetWindowLong(traymenu->GetWindowHandle(), GWL_STYLE) & WS_VISIBLE);
		if(!b)
		{
	
			::ShowWindow(traymenu->GetWindowHandle(),SW_SHOW);
			SetForegroundWindow(traymenu->GetWindowHandle());
			MainWindowEnableStatus = 0;
		}
		else{
			::ShowWindow(traymenu->GetWindowHandle(),SW_HIDE);
			MainWindowEnableStatus = 1;
		}

		return;
	}
	*/
	UITrayMenu* traymenu =new UITrayMenu();
	traymenu->CreateTrayBox(ltskinview->windowHandle(),applicationpath,x,y,cx,cy);

	//SetForegroundWindow(traymenu->GetWindowHandle());
	//MainWindowEnableStatus = 0;
}

void UIClass::ShowSettingBox(int x,int y,int cx,int cy)
{
    if (uiset!=0)
	{
		//CloseListContextMenu();
		delete uiset;
		uiset = 0;
		//return;
	}
	uiset =new UISetting(this);
	uiset->CreateSettingBox(ltskinview->windowHandle(),x,y,cx,cy);
}

void UIClass::CloseTrayMenu()
{
	/*
	if (traymenu!=0)
	{
		DestroyWindow(traymenu->GetWindowHandle());
		delete traymenu;
		traymenu = 0;
	}
	*/
}

void UIClass::RemoveRecord()
{
	ltskinview->RunJavaScript("deletemusicfrommusiclist();");
}

void UIClass::EmptyList()
{
	ltskinview->RunJavaScript("emptylist()");
}

void UIClass::InitAppConfigInfo()
{
	string path = string(applicationpath)+string(PLAYCONFIG_DATA);
	char* bbb = CLTCommon::GetFile((char*)path.c_str());
	char *outwebjson =new char[strlen(bbb)+1];
	sprintf(outwebjson,"initappconfiginfo('%s')",bbb);
	ltskinview->RunJavaScript(outwebjson);
	delete [] bbb;
	delete [] outwebjson;

}
void UIClass::GetAPPConfigInfo()
{
	//MinWindowType mintype;
	LTEncodedJSValue jsv =  ltskinview->RunJavaScript("configvarobject()");
	LTExecState exec = ltskinview->GetGlobalExecState();
	LTEncodedJSValue v = GetJsObjectAttribute(exec,jsv,"general");
	LTEncodedJSValue xc = GetJsObjectAttribute(exec,v,"alwaytray");
	appconfiginfo.alwaytray =JsValue2Boolean(exec,xc);
	v = GetJsObjectAttribute(exec,jsv,"musicplay");
	xc = GetJsObjectAttribute(exec,v,"volumerange");
	appconfiginfo.volumerange =JsValue2Int(exec,xc);

}



void UIClass::SetAutoStart(int i)
{
	HKEY hKey;  
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	wchar_t p[MAX_PATH];
	wcscpy(p,szFilePath);
	_tcsrchr(p, _T('\\'))[1]=0;
	//CString appname(szFilePath);
	WCHAR appname[3000];
	wcscpy(appname,szFilePath);
	//CString strTemp=_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	wchar_t* strTemp =L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	if (!i)
	{
		long lRet=RegOpenKeyEx( HKEY_LOCAL_MACHINE,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), 0, KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE|KEY_SET_VALUE, &hKey );
		if( lRet == ERROR_SUCCESS){
			RegDeleteValue(hKey, _T("LTplayer"));
		}
		RegCloseKey(hKey);
		return;
	}


	RegCreateKey(HKEY_LOCAL_MACHINE,strTemp,&hKey);  
	RegSetValueExW(hKey,_T("LTplayer"),0,REG_SZ,(LPBYTE)appname,(wcslen(appname)+1)*sizeof(wchar_t));
	RegCloseKey(hKey);  
}

int UIClass::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void UIClass::GaussianBlurImage(wchar_t* srcfile,wchar_t* dstfile)
{
	Bitmap* bitmap = new Bitmap(srcfile);
	HBITMAP hBitmap;
	bitmap->GetHBITMAP(NULL,&hBitmap);

	CLSID pngClsid;  
	GetEncoderClsid(L"image/jpeg", &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   
	ltskinview->GaussianBlurImage(hBitmap,80);
	Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);  
	pbmSrc->Save(dstfile, &pngClsid);;
	delete bitmap;
	DeleteObject(hBitmap);

}