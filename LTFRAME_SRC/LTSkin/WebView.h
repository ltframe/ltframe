/*
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebView_h_caonima
#define WebView_h_caonima

#include "AtomicString.h"
#include "IntRect.h"
#include "OwnPtr.h"
#include <WebCore/COMPtr.h>
#include "PassRefPtr.h"
#include <BridgeJSC.h>
#include <windows.h>
#include "InterFace.h"
#include <Shobjidl.h>
#include <WebCore/RefCountedGDIHandle.h>
#include <ScriptController.h>
#include <ScriptValue.h>
#include <WebCore/DragActions.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/JSFunction.h>
#include <JavaScriptCore/SourceCode.h>
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/Heap.h>
#include <JavaScriptCore/JSGlobalData.h>
#include <WinUser.h>
namespace WTF {
class String;
}

namespace WebCore {
class Frame;
class Page;
class FrameView;
class HTMLFrameOwnerElement;
class KURL;
class ResourceRequest;
class GraphicsContext;
class JSDOMWindow;
}
interface IDropTargetHelper;
typedef struct _WindowPos
{
     int x;
	 int y;
	 int cx;
	 int cy;
}WindowPos;

typedef WebCore::RefCountedGDIHandle<HBITMAP> RefCountedHBITMAP;
typedef WebCore::RefCountedGDIHandle<HRGN> RefCountedHRGN;

class  WebView :public IDropTarget,public I_LTFRAME_IWebView{
public:

	WebView(I_LTFRAME_IWebView* iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
		int x,int y,int nWidth,int nHeight,HICON icon);
    ~WebView();	
	HINSTANCE GetWebCoreInstanceHandle();
    void initialize(HINSTANCE instanceHandle,HICON icon);
	static WebView* CreateSkinInstance(I_LTFRAME_IWebView* iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
		int x,int y,int nWidth,int nHeight);
	
    WebCore::Frame* frame() const { return m_frame; }
    WebCore::Page* page() const { return m_page; }
    WebCore::FrameView* view() const;

	virtual LRESULT HandleUserMessage(HWND, UINT, WPARAM, LPARAM){return 1;};
	virtual LPCWSTR GetWindowClassName();
   
	HWND windowHandle() const { return m_windowHandle; }
    void load(LPCWSTR url);
	void loadFile(LPCWSTR filename);
	LTEncodedJSValue RunJavaScript(const char* script);
	LTEncodedJSValue RunJavaScript(const wchar_t* script);
	LTExecState GetGlobalExecState();
	void CloseWindow();
	void QuitApp();
	void BindUserFunction(const char* name, LTNativeFunction fn, unsigned int argCount);
    void loadHTML(const wchar_t* html);
	void MessageLoop();
	void SetViewTransparent(bool);
	void EnableDragFrameChangeSize(bool a){m_enabledragframechangesize =a;};
	void SetAllowKeyDownAutoScroll(bool a);
	void IsAutoGC(bool autocg,int maxmemorysize);
	void CleanMemory();
	
	void SetWebSqlPath(const wchar_t* path=NULL);
	void SetLocalStoragePath(const wchar_t* path=NULL);
	void GaussianBlurImage(HBITMAP hBitmap,int radius);
	//void ReleaseWindow();


	LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void reload();
    void stop();
    void frameRect(RECT* rect) const;
    PassRefPtr<WebCore::Frame> createFrame(const WebCore::KURL&, const WTF::String&, WebCore::HTMLFrameOwnerElement*, const WTF::String&, bool, int, int);
    void runJavaScriptAlert(BSTR);
	bool runJavaScriptConfirm(const WTF::String& message);
    bool runJavaScriptPrompt(const WTF::String& message, const WTF::String& defaultValue, WTF::String& result);
	void handlePaint(HWND hwnd);
	HWND GetViewHwnd();
	WebCore::Frame* m_frame;
	WebCore::Page* m_page;
	void ShowMainWindow();
	bool IsTransparentView();
	void AddJsToFunList(WebCore::JSDOMWindow* jsdow);
	void scrollBackingStore(int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);
    void repaint(const WebCore::IntRect&, bool contentChanged, bool immediate = false, bool repaintContentOnly = false);
	void setLastCursor(HCURSOR cursor) { m_lastSetCursor = cursor; }
	void setToolTip(const WTF::String& toolTip);



private:
	
	void UnregisterWindow();
	bool m_enabledragframechangesize;
	//virtual LPCWSTR GetWindowClassName();
    bool m_mouseOutTracker;
	bool m_mouseActivated;
	void setMouseActivated(bool flag) { m_mouseActivated = flag; }
	static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND CreateUiWindow(LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
int x,int y,int nWidth,int nHeight);
    bool handleMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool handleMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isHorizontal);
    bool handleKeyDown(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyPress(WPARAM charCode, LPARAM keyData, bool systemKeyDown);
    bool handleKeyUp(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown);
	void BindFunction(JSC::JSGlobalObject* globalObject, const char* name, JSC::NativeFunction function, unsigned int argCount);
	void Close();
	void ResizeWindow();
	int OnNcHitTest(HWND hWnd);
	bool OnSize(HWND hwnd);
	bool OnSizeIng(HWND hwnd);
    bool islayered;
	void SetViewBgColor(bool islayered);
	BLENDFUNCTION m_Blend;
	POINT   Dlgpt;
	SIZE   Dlgsize;
	HWND m_toolTipHwnd;
	WTF::String m_toolTip;

	LPCWSTR m_windowclassname;
	HWND m_windowHandle;
	WindowPos winrect;
	//WebCore::GraphicsContext *gc;
	DWORD dwProcessId;
	HANDLE hProcess;

	DWORD m_jsgcstarttime;
	bool m_autojsgc;
	int m_maxmemorysize;
	/*paint part*/
	bool m_deleteBackingStoreTimerActive;
	RefPtr<RefCountedHBITMAP> m_backingStoreBitmap;
	RefPtr<RefCountedHRGN> m_backingStoreDirtyRegion;
	enum WindowsToPaint { PaintWebViewOnly, PaintWebViewAndChildren };
	SIZE m_backingStoreSize;
	HWND m_topLevelParent;
	HWND topLevelParent() const { return m_topLevelParent; }
    void paintIntoWindow(HDC bitmapDC, HDC windowDC, const WebCore::IntRect& dirtyRect);
    bool ensureBackingStore();
    void addToDirtyRegion(const WebCore::IntRect&);
    void addToDirtyRegion(HRGN);
    void scrollBackingStore(WebCore::FrameView*, int dx, int dy, const WebCore::IntRect& scrollViewRect, const WebCore::IntRect& clipRect);
    void deleteBackingStore();
    void paintIntoBackingStore(WebCore::FrameView*, HDC bitmapDC, const WebCore::IntRect& dirtyRect, WindowsToPaint);
    void updateBackingStore(WebCore::FrameView*, HDC = 0, bool backingStoreCompletelyDirty = false, WindowsToPaint = PaintWebViewOnly);
    void updateActiveStateSoon() const;
    void deleteBackingStoreSoon();
    void cancelDeleteBackingStoreSoon();
    void performLayeredWindowUpdate();
	bool active();
	void windowAncestryDidChange();
	void updateActiveState();
	void paint(HDC, LPARAM);
	/*paint part*/
	HCURSOR m_lastSetCursor;
	bool verticalScroll(WPARAM wParam, LPARAM /*lParam*/);
	bool horizontalScroll(WPARAM wParam, LPARAM /*lParam*/);
	void initializeToolTipWindow();


	COMPtr<IDataObject> m_dragData;
    COMPtr<IDropTargetHelper> m_dropTargetHelper;
	DWORD m_lastDropEffect;
	ULONG m_refCount;
	ULONG  __stdcall Release(void);
	ULONG  __stdcall AddRef(void);
	HRESULT  __stdcall QueryInterface(REFIID riid, void** ppvObject);
	HRESULT  __stdcall Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT  __stdcall DragLeave();
	HRESULT  __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT  __stdcall DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT revokeDragDrop();
	WebCore::DragOperation keyStateToDragOperation(DWORD grfKeyState) const;
	HRESULT registerDragDrop();
};

#endif // WebView_h
