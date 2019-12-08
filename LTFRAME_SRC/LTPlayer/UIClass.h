#include "stdafx.h"
#include "InterFace.h"
#include "LTUtils.h"
#include "json/json.h"
#include "PlayClass.h"
#include "UIContextMenu.h"
#include "UISetting.h"
#include "UITrayMenu.h"
#include <string>
#include "DropFile.h"
using namespace std; 
#define MUSIC_TIMER 100001
#define BINDJAVASCRIPTTOAPP(str,index)  JavaScriptMap.insert(make_pair(string(str),index));
class UIClass : public I_LTFRAME_IWebView
{
public:
	UIClass();
	~UIClass(void);
	virtual LRESULT HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	virtual LPCWSTR MySelfDefinedJavaScriptFunction(const wchar_t* str);
	Json::Value GetJsonRoot(string);
	void RemoveRecord();
	void EmptyList();
    void MusicPlay();
    void OpenFileFolder();
	void DragWindow();
	void SetLayerWindow(bool type);
	void QuitApp();
	void CloseApp();
	void OpenFileDialog();
	void MinWindow();
	void MusicPlay(const char* filepath,const char* filename);
	void MusicStop();
	void WriteFile(char*,char*,int);
	void GetFile();
	void OpenFileFolder(char* path);
	void ShowUserListContextMenu(int x,int y,int cx,int cy);
	void CloseListContextMenu();
	wchar_t* CreateGUID();
	void SetMusicPosition(double);
	void WriteAppConfigFile(char* x,char*,int);
	void SetMusicVolume(double);
	void ShowSettingBox(int x,int y,int cx,int cy);
	char* LoadConfigVar();
	void InitAppConfigInfo();
	void DropFiles(PVOID data);
	void DropEnter(PVOID data);
	void DropLeave();
virtual bool  OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual bool  OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual bool  OnDragLeave();
	
	virtual bool  OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	RECT GetCurrentRect();
	MusicBasicInfo CurrentPlayerMusicInfo;
	void DeleteNotifyIcon();
	void ShowNotifyIcon();
	void TestJs();
	void GaussianBlurImage(wchar_t* srcfile,wchar_t* dstfile);
	void SetMianWindowFocus();
	void SetTrayMenuVarProcFun(HWND);
	void EnableDragFrameChangeSize(bool b);
	MusicBasicInfo GetMusicInfo(char* charfilepath);
	HICON icon;
private:
	I_LTFRAME_IWebView* ltskinview;
	CLTCommon* cltcommon;
	AppConfigInfo appconfiginfo;
	PlayClass* playclass;
	map<std::string,int> JavaScriptMap;
	int GetJavaScriptIntValue(string s);
    void GetAPPConfigInfo();
	static void DonwloadCoverThread(void*);
	void ContextMenuOperate(WPARAM t);
	
	void SetCurrentMusicTime();
	void TimePRO(WPARAM wParam);
	UISetting* uiset;
	HMODULE hModule;
	int MainWindowEnableStatus;
	void GetMinMaxInfo(MINMAXINFO* lpMMI);
	RECT CenterWindow(int w,int h);
	void SetAutoStart(int i);
	long OnUserSelfMessageNOTIFY(WPARAM wParam, LPARAM lParam);
	void ShowTrayContextMenu(int x,int y,int cx,int cy);
	void CloseTrayMenu();
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	void SetTrayMenuVar();
	DropFile *pDropTarget;

};
