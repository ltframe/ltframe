/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010 Apple, Inc.  All rights reserved.
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

#include "config.h"
#include "WebView.h"

#include "ChromeClientWinCE.h"
#include "ContextMenuClientWinCE.h"
#include "DragClientWinCE.h"
#include "EditorClientWinCE.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClientWinCE.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "InitializeThreading.h"
#include "InspectorClientWinCE.h"
#include "IntSize.h"
#include "MainThread.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformMouseEvent.h"
#include "PlatformStrategiesWinCE.h"
#include "PlatformWheelEvent.h"
#include "ResourceRequest.h"
#include "Settings.h"
#include "SharedBuffer.h"
#include "WebCoreInstanceHandle.h"
#include <WebCore/TextEncoding.h>
#include "icuwin.h"
#include <WebCore/SubstituteData.h>
#include <WebCore/BitmapInfo.h>
#include <JSDOMWindowBase.h>
#include <DOMWrapperWorld.h>

#include <WinGdi.h>
#include <BString.h>
#include "Psapi.h"
#include <WebCore/DragController.h>
#include <WebCore/DragData.h>
#include <WebCore/DragSession.h>
#include <WebCore/GCController.h>
#include <WebCore/DatabaseTracker.h>
#include <WebCore/ApplicationCacheStorage.h>
#include <WebCore/HWndDC.h>
#include <WebCore/FocusDirection.h>
#include <cairo.h>
#include <ShlObj.h>
#include <comutil.h>
#include <dimm.h>
#include <oleacc.h>
#include <wchar.h>
#include <windowsx.h>
using namespace WebCore;



#define MAX_NAME_LENGTH 32
typedef struct _jsFunctionInfo
{
    char name[MAX_NAME_LENGTH];
    JSC::NativeFunction fn;
    unsigned int argCount;
}JsFunInfo;

typedef struct {  
	UINT      cbSize;  
	UINT      uFlags;  
	HWND      hwnd;  
	UINT_PTR  uId;  
	RECT      rect;  
	HINSTANCE hinst;  
	LPWSTR    lpszText;  
	LPARAM    lParam;  
} myTOOLINFO;  


static Vector<JsFunInfo> JsFunInfoList;

/*paint part*/
enum {
    UpdateActiveStateTimer = 1,
    DeleteBackingStoreTimer = 2,
};
static HashSet<WebView*> pendingDeleteBackingStoreSet;
static const int delayBeforeDeletingBackingStoreMsec = 5000;
/*paint part*/

#define JSGC 1100
HINSTANCE WebView::GetWebCoreInstanceHandle()
{
	return WebCore::instanceHandle();
}

WebView* WebView::CreateSkinInstance(I_LTFRAME_IWebView* iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
									 int x,int y,int nWidth,int nHeight)
{
	
	//WebView* v =new WebView(iwebview,lpWindowName,hWndParent,dwStyle,x,y,nWidth,nHeight);
	return 0;
}

PassRefPtr<SharedBuffer> loadResourceIntoBuffer(const char* name)
{
    notImplemented();
    return 0;
}


WebView::WebView(I_LTFRAME_IWebView* iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
				 int x,int y,int nWidth,int nHeight,HICON icon)
    : m_frame(0)
    , m_page(0),islayered(false),dwProcessId(0),hProcess(0),m_jsgcstarttime(0),m_autojsgc(false),m_deleteBackingStoreTimerActive(false), m_topLevelParent(0),
	m_lastSetCursor(0),m_toolTipHwnd(0),m_mouseActivated(false),m_mouseOutTracker(false),m_enabledragframechangesize(true),m_dragData(0),m_refCount(0),m_maxmemorysize(50)

{

	m_backingStoreSize.cx = m_backingStoreSize.cy = 0;

	Dlgpt.x=0;
	Dlgpt.y=0;
	Dlgsize.cx=nWidth;
	Dlgsize.cy=nHeight;


	m_Blend.AlphaFormat = AC_SRC_ALPHA;
	m_Blend.BlendFlags = 0;
	m_Blend.BlendOp = AC_SRC_OVER;
	m_Blend.SourceConstantAlpha = 255;


	
	m_windowclassname = iwebview->GetWindowClassName();
	initialize(GetWebCoreInstanceHandle(),icon);
	Settings::setDefaultMinDOMTimerInterval(0.004);

	winrect.x =x;
	winrect.y=y;
	winrect.cx =nWidth;
	winrect.cy = nHeight;
	m_windowHandle =CreateUiWindow(lpWindowName,hWndParent,dwStyle,x,y,nWidth,nHeight);

	SetWindowLongPtr(m_windowHandle,GWLP_USERDATA, reinterpret_cast<LPARAM>(iwebview));

	CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper,(void**)&m_dropTargetHelper);


	registerDragDrop();
    Page::PageClients pageClients;
    pageClients.chromeClient = new WebKit::ChromeClientWinCE(this);
    pageClients.contextMenuClient = new WebKit::ContextMenuClientWinCE(this);
    pageClients.editorClient = new WebKit::EditorClientWinCE(this);
    pageClients.dragClient = new WebKit::DragClientWinCE(this);
    pageClients.inspectorClient = new WebKit::InspectorClientWinCE(this);
    m_page = new Page(pageClients);

    Settings* settings = m_page->settings();
    settings->setDefaultFixedFontSize(14);
    settings->setDefaultFontSize(14);
    settings->setMinimumFontSize(8);
    settings->setMinimumLogicalFontSize(8);
	settings->setScriptEnabled(true);
	settings->setPluginsEnabled(true);
    settings->setLoadsImagesAutomatically(true);
    settings->setDefaultFixedFontSize(13);
	settings->setJavaScriptCanOpenWindowsAutomatically(true);
	settings->setAllowUniversalAccessFromFileURLs(true);
	settings->setAllowFileAccessFromFileURLs(true);
	settings->setLocalStorageEnabled(true);
	settings->setPrivateBrowsingEnabled(false);
	settings->setWebSecurityEnabled(true);
	m_page->setCookieEnabled(true);

    WebKit::FrameLoaderClientWinCE* loaderClient = new WebKit::FrameLoaderClientWinCE(this);
    RefPtr<Frame> frame = Frame::create(m_page, 0, loaderClient);
    m_frame = frame.get();
    loaderClient->setFrame(m_frame);

    m_page->mainFrame()->init();
	//m_page->focusController()->setActive(true);

	m_page->focusController()->setFocused(true);
	//view()->setCanHaveScrollbars(false);
	windowAncestryDidChange();
	SetAllowKeyDownAutoScroll(true);

    if (view()) {
		OnSize(m_windowHandle);
		GetWindowThreadProcessId(m_windowHandle, &dwProcessId);
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwProcessId);
			m_jsgcstarttime =::GetTickCount();
			::SetTimer(m_windowHandle,JSGC,1,0);
    }
	
}

LPCWSTR WebView::GetWindowClassName(){
	return L"";
}

static DWORD dragOperationToDragCursor(DragOperation op) {
    DWORD res = DROPEFFECT_NONE;
    if (op & DragOperationCopy) 
        res = DROPEFFECT_COPY;
    else if (op & DragOperationLink) 
        res = DROPEFFECT_LINK;
    else if (op & DragOperationMove) 
        res = DROPEFFECT_MOVE;
    else if (op & DragOperationGeneric) 
        res = DROPEFFECT_MOVE; //This appears to be the Firefox behaviour
    return res;
}

HRESULT WebView::registerDragDrop()
{
    return ::RegisterDragDrop(m_windowHandle, this);
}

HRESULT __stdcall WebView::DragEnter(IDataObject* pDataObject
									 , DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	
	I_LTFRAME_IWebView* iwebview = NULL;
    iwebview = reinterpret_cast<I_LTFRAME_IWebView*>(::GetWindowLongPtr(m_windowHandle, GWLP_USERDATA));


    m_dragData = 0;

    if (m_dropTargetHelper)
        m_dropTargetHelper->DragEnter(m_windowHandle, pDataObject, (POINT*)&pt, *pdwEffect);

    POINTL localpt = pt;
    ::ScreenToClient(m_windowHandle, (LPPOINT)&localpt);
    DragData data(pDataObject, IntPoint(localpt.x, localpt.y), 
        IntPoint(pt.x, pt.y), keyStateToDragOperation(grfKeyState));
    *pdwEffect = dragOperationToDragCursor(m_page->dragController()->dragEntered(&data).operation);

    m_lastDropEffect = *pdwEffect;
    m_dragData = pDataObject;
	iwebview->OnDragEnter(pDataObject,grfKeyState,pt,pdwEffect);
    return S_OK;
}


HRESULT __stdcall  WebView::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	I_LTFRAME_IWebView* iwebview = NULL;
    iwebview = reinterpret_cast<I_LTFRAME_IWebView*>(::GetWindowLongPtr(m_windowHandle, GWLP_USERDATA));

    if (m_dropTargetHelper)
        m_dropTargetHelper->DragOver((POINT*)&pt, *pdwEffect);

    if (m_dragData) {
        POINTL localpt = pt;
        ::ScreenToClient(m_windowHandle, (LPPOINT)&localpt);
        DragData data(m_dragData.get(), IntPoint(localpt.x, localpt.y), 
            IntPoint(pt.x, pt.y), keyStateToDragOperation(grfKeyState));
        *pdwEffect = dragOperationToDragCursor(m_page->dragController()->dragUpdated(&data).operation);
    } else
        *pdwEffect = DROPEFFECT_NONE;
	iwebview->OnDragOver(grfKeyState,pt,pdwEffect);
    m_lastDropEffect = *pdwEffect;
    return S_OK;
}

HRESULT __stdcall WebView::DragLeave()
{
	I_LTFRAME_IWebView* iwebview = NULL;
    iwebview = reinterpret_cast<I_LTFRAME_IWebView*>(::GetWindowLongPtr(m_windowHandle, GWLP_USERDATA));

    if (m_dropTargetHelper)
        m_dropTargetHelper->DragLeave();

    if (m_dragData) {
        DragData data(m_dragData.get(), IntPoint(), IntPoint(), 
            DragOperationNone);
        m_page->dragController()->dragExited(&data);
        m_dragData = 0;
    }
	iwebview->OnDragLeave();
    return S_OK;
}

HRESULT __stdcall WebView::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	I_LTFRAME_IWebView* iwebview = NULL;
    iwebview = reinterpret_cast<I_LTFRAME_IWebView*>(::GetWindowLongPtr(m_windowHandle, GWLP_USERDATA));


    if (m_dropTargetHelper)
        m_dropTargetHelper->Drop(pDataObject, (POINT*)&pt, *pdwEffect);

    m_dragData = 0;
    *pdwEffect = m_lastDropEffect;
    POINTL localpt = pt;
    ::ScreenToClient(m_windowHandle, (LPPOINT)&localpt);
    DragData data(pDataObject, IntPoint(localpt.x, localpt.y), 
        IntPoint(pt.x, pt.y), keyStateToDragOperation(grfKeyState));
   m_page->dragController()->performDrag(&data);
   iwebview->OnDrop(pDataObject,grfKeyState,pt,pdwEffect);
    return S_OK;
}

HRESULT __stdcall WebView::QueryInterface(REFIID iid, void** ppvObject)
{
	if(iid == IID_IDropTarget || iid == IID_IUnknown)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
    return S_OK;
}

ULONG __stdcall WebView::AddRef(void)
{
    return ++m_refCount;
}

ULONG __stdcall WebView::Release(void)
{
  //  ASSERT(!m_deletionHasBegun);

    if (m_refCount == 1) {
        // Call close() now so that clients don't have to. (It's harmless to call close() multiple
        // times.) We do this here instead of in our destructor because close() can cause AddRef()
        // and Release() to be called, and if that happened in our destructor we would be destroyed
        // more than once.
        revokeDragDrop();
    }
    ULONG newRef = --m_refCount;
    return newRef;
}
HRESULT WebView::revokeDragDrop()
{
    return ::RevokeDragDrop(m_windowHandle);
}

DragOperation WebView::keyStateToDragOperation(DWORD grfKeyState) const
{
    if (!m_page)
        return DragOperationNone;

    // Conforms to Microsoft's key combinations as documented for 
    // IDropTarget::DragOver. Note, grfKeyState is the current 
    // state of the keyboard modifier keys on the keyboard. See:
    // <http://msdn.microsoft.com/en-us/library/ms680129(VS.85).aspx>.
    DragOperation operation = m_page->dragController()->sourceDragOperation();

    if ((grfKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT))
        operation = DragOperationLink;
    else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
        operation = DragOperationCopy;
    else if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
        operation = DragOperationGeneric;

    return operation;
}

void WebView::SetAllowKeyDownAutoScroll(bool b)
{
	m_page->mainFrame()->eventHandler()->SetAllowKeyDownAutoScroll(b);
}

bool WebView::OnSize(HWND hwnd)
{	
	RECT windowRect;
	::GetWindowRect(hwnd, &windowRect);
	IntRect intrect = IntRect(windowRect);

	deleteBackingStore();
	view()->resize(intrect.size());
	return true;
}

bool WebView::OnSizeIng(HWND hwnd)
{
	MoveWindow(hwnd, 0, 0, 700,600,false);
	return true;
}


void WebView::MessageLoop()
{
	MSG msg; 
	/*::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);*/
	while (GetMessage(&msg, NULL, 0, 0)) 
	{ 
		//int x = GetLastError();
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
		
	} 
}

WebView::~WebView()
{
	if (::IsWindow(m_toolTipHwnd))
        ::DestroyWindow(m_toolTipHwnd);
}

/*
void WebView::ReleaseWindow()
{
	//DestroyWindow(m_windowHandle);
	//

	//UnregisterWindow();
	//delete(this);
	
	
}*/
void WebView::Close()
{
    if (m_page) {
        if (Frame* frame = m_page->mainFrame())
            frame->loader()->detachFromParent();
    }
   /*if (m_mouseOutTracker) {
        m_mouseOutTracker->dwFlags = TME_CANCEL;
        ::TrackMouseEvent(m_mouseOutTracker.get());
        m_mouseOutTracker.clear();
    }*/

   if (m_mouseOutTracker) {
	   TRACKMOUSEEVENT tme = { 0 };
	   tme.cbSize = sizeof(TRACKMOUSEEVENT);
	   tme.dwFlags = TME_CANCEL;
	   tme.hwndTrack = m_windowHandle;
	   ::TrackMouseEvent(&tme);
	   m_mouseOutTracker = false;
   }


    if (m_windowHandle) {
        // We can't check IsWindow(m_viewWindow) here, because that will return true even while
        // we're already handling WM_DESTROY. So we check !isBeingDestroyed() instead.

           // DestroyWindow(m_windowHandle);
        // Either we just destroyed m_viewWindow, or it's in the process of being destroyed. Either
        // way, we clear it out to make sure we don't try to use it later.
        m_windowHandle = 0;
    }
    delete m_page;
    m_page = 0;
    deleteBackingStore();
}


HWND WebView::GetViewHwnd()
{
	return m_windowHandle;
}

void WebView::initialize(HINSTANCE instanceHandle,HICON icon)
{
	CoInitialize(NULL);

    JSC::initializeThreading();
    WTF::initializeMainThread();
    PlatformStrategiesWinCE::initialize();
    WebCore::setInstanceHandle(instanceHandle);


	WNDCLASSEX wce; 
	wce.cbSize        = sizeof(WNDCLASSEX);
	wce.style         = CS_HREDRAW | CS_VREDRAW;
	wce.lpfnWndProc   = (WNDPROC)_WinProc;        //The WNDPROC type defines a pointer to this callback function
	wce.cbClsExtra    = 0;	//The system initializes the bytes to zero                      
	wce.cbWndExtra    = 0;	//The system initializes the bytes to zero
	wce.hIcon         = icon;//::LoadIcon(NULL,IDI_WINLOGO);
	wce.hCursor       = ::LoadCursor(NULL,IDC_ARROW);
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wce.lpszMenuName  = NULL;
	wce.lpszClassName = m_windowclassname;
	wce.hIconSm       = 0;//::LoadIcon(NULL,IDI_WINLOGO);
	wce.hInstance=instanceHandle;
	RegisterClassEx(&wce); 	
}

void WebView::UnregisterWindow()
{
    UnregisterClass(m_windowclassname, WebCore::instanceHandle());
}

PassRefPtr<Frame> WebView::createFrame(const KURL& url, const String& name, HTMLFrameOwnerElement* ownerElement, const String& referrer,
                                       bool /*allowsScrolling*/, int /*marginWidth*/, int /*marginHeight*/)
{
    Frame* coreFrame = m_frame;

    WebKit::FrameLoaderClientWinCE *loaderClient = new WebKit::FrameLoaderClientWinCE(this);
    RefPtr<Frame> childFrame = Frame::create(m_page, ownerElement, loaderClient);
    loaderClient->setFrame(childFrame.get());

    coreFrame->tree()->appendChild(childFrame);
    childFrame->tree()->setName(name);
    childFrame->init();

    // The creation of the frame may have run arbitrary JavaScript that removed it from the page already.
    if (!childFrame->page())
        return 0;

    coreFrame->loader()->loadURLIntoChildFrame(url, referrer, childFrame.get());

    // The frame's onload handler may have removed it from the document.
    if (!childFrame->tree()->parent())
        return 0;

    return childFrame.release();
}

void WebView::runJavaScriptAlert(BSTR str)
{
	::ReleaseCapture();
	MessageBox(NULL,str,NULL,NULL);
}

bool WebView::runJavaScriptConfirm(const String& message)
{
    notImplemented();
    return false;
}


bool WebView::runJavaScriptPrompt(const String& message, const String& defaultValue, String& result)
{
    notImplemented();
    return false;
}


FrameView* WebView::view() const
{
    return m_frame ? m_frame->view() : 0;
}


void WebView::loadFile(const wchar_t* filename)
{
    wchar_t url[1024];
	if(!wcsncmp(L"http://",filename,wcslen(L"http://")) || !wcsncmp(L"https://",filename,wcslen(L"https://")))
	{
		_snwprintf(url, 1024, L"%s", filename);		
	}else{
		_snwprintf(url, 1024, L"file:///%s", filename);		
	}
	url[1023] = L'\0';
	load(url);
}

void WebView::loadHTML(const wchar_t* html)
{
	String mime = "text/html";
	RefPtr<WebCore::SharedBuffer> sharedBuffer = WebCore::SharedBuffer::create((const char*)html, wcslen(html)*2);

	WebCore::KURL url(WebCore::KURL(), "", WebCore::UTF8Encoding());
	WebCore::ResourceRequest request(url);
	WebCore::SubstituteData substituteData(sharedBuffer, mime, WebCore::UTF16LittleEndianEncoding().name(), url);
	frame()->loader()->load(request, substituteData, false);
}

void WebView::load(LPCWSTR url)
{
	WebCore::KURL urld(WebCore::KURL(), url, WebCore::UTF8Encoding());
	WebCore::ResourceRequest req(urld);
	frame()->loader()->load(req, false);
}

void WebView::reload()
{
    frame()->loader()->reload();
}

void WebView::stop()
{
    frame()->loader()->stopAllLoaders();
}

void WebView::handlePaint(HWND hWnd)
{

}
LTEncodedJSValue WebView::RunJavaScript(const char* script)
{

	String string(script);
	WebCore::ScriptValue value = m_frame->script()->executeScript(string, true);
	if (value.hasNoValue())
		return JSC::jsUndefined();

    JSC::JSValue scriptExecutionResult = value.jsValue();
	return (LTEncodedJSValue)JSC::JSValue::encode(scriptExecutionResult);
}

LTEncodedJSValue WebView::RunJavaScript(const wchar_t* script)
{

	String string(script);
	WebCore::ScriptValue value = m_frame->script()->executeScript(string, true);
	if (value.hasNoValue())
		return JSC::jsUndefined();

    JSC::JSValue scriptExecutionResult = value.jsValue();
	return (LTEncodedJSValue)JSC::JSValue::encode(scriptExecutionResult);
}

LTExecState WebView::GetGlobalExecState()
{
	JSC::ExecState* exec = m_frame->script()->globalObject(mainThreadNormalWorld())->globalExec();
	return (LTExecState)exec;
}
//LPWSTR WebView::RunJavaScript(const wchar_t* script)
//{
//	String string(script);
//	WebCore::ScriptValue value = m_frame->script()->executeScript(string, true);
//	if (value.hasNoValue())
//		return L"";
//
//    JSC::JSValue scriptExecutionResult = value.jsValue();
//
//	JSC::ExecState* exec = m_frame->script()->globalObject(mainThreadNormalWorld())->globalExec();
//
//
//
//
//
//	JSC::JSObject* jsAttrs = scriptExecutionResult.getObject();
//	JSC::Identifier alpha(exec, "filepath");
//	JSC::JSValue value1  = jsAttrs->get(exec, alpha);
//    JSC::UString str = value1.toString(exec)->value(exec);
//
//
//	if(scriptExecutionResult.isNumber()){
//	
//		wchar_t w[1000]={};
//		return _itow(scriptExecutionResult.toInt32(exec),w,10);
//	}
//
//	if(scriptExecutionResult.isString())
//	{
//		return (wchar_t*)ustringToString(scriptExecutionResult.getString(exec)).characters();
//	}
//	return L"";
//}

//LPSTR WebView::RunJavaScript(const char* script)
//{
//	String string(script);
//	WebCore::ScriptValue value = m_frame->script()->executeScript(string, true);
//	if (value.hasNoValue())
//		return "";
//
//    JSC::JSValue scriptExecutionResult = value.jsValue();
//
//	JSC::ExecState* exec = m_frame->script()->globalObject(mainThreadNormalWorld())->globalExec();
//	if(scriptExecutionResult.isNumber()){
//	
//		char w[1000]={};
//		return _itoa(scriptExecutionResult.toInt32(exec),w,10);
//	}
//
//	if(scriptExecutionResult.isString())
//	{
//		return (char*)ustringToString(scriptExecutionResult.getString(exec)).characters();
//	}
//	return "";
//}


bool WebView::handleMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
/*
	static LONG globalClickCount;
    static IntPoint globalPrevPoint;
    static MouseButton globalPrevButton;
    static LONG globalPrevMouseDownTime;

    if (message == WM_CANCELMODE) {
        m_page->mainFrame()->eventHandler()->lostMouseCapture();
        return true;
	}
    // Create our event.
    // On WM_MOUSELEAVE we need to create a mouseout event, so we force the position
    // of the event to be at (MINSHORT, MINSHORT).
   // PlatformMouseEvent mouseEvent(hWnd, message, wParam, lParam);


	LPARAM position = (message == WM_MOUSELEAVE) ? ((MINSHORT << 16) | MINSHORT) : lParam;
    PlatformMouseEvent mouseEvent(hWnd, message, wParam, position, m_mouseActivated);

    setMouseActivated(false);



    bool insideThreshold = abs(globalPrevPoint.x() - mouseEvent.position().x()) < ::GetSystemMetrics(SM_CXDOUBLECLK)
                           && abs(globalPrevPoint.y() - mouseEvent.position().y()) < ::GetSystemMetrics(SM_CYDOUBLECLK);
    LONG messageTime = 0;

    bool handled = false;
    if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
        // FIXME: I'm not sure if this is the "right" way to do this
        // but without this call, we never become focused since we don't allow
        // the default handling of mouse events.
       // SetFocus(m_windowHandle);

        PlatformMouseEvent moveEvent(hWnd, WM_MOUSEMOVE, 0, lParam, false);
        moveEvent.setClickCount(0);
        m_page->mainFrame()->eventHandler()->handleMouseMoveEvent(moveEvent);

        // Always start capturing events when the mouse goes down in our HWND.
        SetCapture(m_windowHandle);

        if (insideThreshold && mouseEvent.button() == globalPrevButton)
            globalClickCount++;
        else
            // Reset the click count.
            globalClickCount = 1;
        globalPrevMouseDownTime = messageTime;
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.position();

        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    } else if (message == WM_LBUTTONDBLCLK || message == WM_MBUTTONDBLCLK || message == WM_RBUTTONDBLCLK) {
        globalClickCount++;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    } else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
        // Record the global position and the button of the up.
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.position();
        mouseEvent.setClickCount(globalClickCount);
        m_page->mainFrame()->eventHandler()->handleMouseReleaseEvent(mouseEvent);
        ReleaseCapture();
    }else if (message == WM_MOUSELEAVE) {
        // Once WM_MOUSELEAVE is fired windows clears this tracker
        // so there is no need to disable it ourselves.
        m_page->mainFrame()->eventHandler()->mouseMoved(mouseEvent);
        handled = true;
	  }
	else if (message == WM_MOUSEMOVE) {
        if (!insideThreshold)
            globalClickCount = 0;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->mouseMoved(mouseEvent);
    }

    return handled;
*/

	static LONG globalClickCount;
    static IntPoint globalPrevPoint;
    static MouseButton globalPrevButton;
    static LONG globalPrevMouseDownTime;

    if (message == WM_CANCELMODE) {
        m_page->mainFrame()->eventHandler()->lostMouseCapture();
        return true;
    }

    // Create our event.
    // On WM_MOUSELEAVE we need to create a mouseout event, so we force the position
    // of the event to be at (MINSHORT, MINSHORT).
    LPARAM position = (message == WM_MOUSELEAVE) ? ((MINSHORT << 16) | MINSHORT) : lParam;
    PlatformMouseEvent mouseEvent(hWnd, message, wParam, position, m_mouseActivated);

    setMouseActivated(false);

    bool insideThreshold = abs(globalPrevPoint.x() - mouseEvent.position().x()) < ::GetSystemMetrics(SM_CXDOUBLECLK) &&
                           abs(globalPrevPoint.y() - mouseEvent.position().y()) < ::GetSystemMetrics(SM_CYDOUBLECLK);
    LONG messageTime = ::GetMessageTime();

    bool handled = false;

    if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
        // FIXME: I'm not sure if this is the "right" way to do this
        // but without this call, we never become focused since we don't allow
        // the default handling of mouse events.
        SetFocus(hWnd);

        // Always start capturing events when the mouse goes down in our HWND.
        ::SetCapture(hWnd);

        if (((messageTime - globalPrevMouseDownTime) < (LONG)::GetDoubleClickTime()) && 
            insideThreshold &&
            mouseEvent.button() == globalPrevButton)
            globalClickCount++;
        else
            // Reset the click count.
            globalClickCount = 1;
        globalPrevMouseDownTime = messageTime;
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.position();
        
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    } else if (message == WM_LBUTTONDBLCLK || message == WM_MBUTTONDBLCLK || message == WM_RBUTTONDBLCLK) {
        globalClickCount++;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->handleMousePressEvent(mouseEvent);
    } else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
        // Record the global position and the button of the up.
        globalPrevButton = mouseEvent.button();
        globalPrevPoint = mouseEvent.position();
        mouseEvent.setClickCount(globalClickCount);
        m_page->mainFrame()->eventHandler()->handleMouseReleaseEvent(mouseEvent);
        ::ReleaseCapture();
    } else if (message == WM_MOUSELEAVE && m_mouseOutTracker) {
        // Once WM_MOUSELEAVE is fired windows clears this tracker
        // so there is no need to disable it ourselves.
        //m_mouseOutTracker.clear();
		m_mouseOutTracker = false;
        m_page->mainFrame()->eventHandler()->mouseMoved(mouseEvent);
        handled = true;
    } else if (message == WM_MOUSEMOVE) {
        if (!insideThreshold)
            globalClickCount = 0;
        mouseEvent.setClickCount(globalClickCount);
        handled = m_page->mainFrame()->eventHandler()->mouseMoved(mouseEvent);
        if (!m_mouseOutTracker) {
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			::TrackMouseEvent(&tme);
			m_mouseOutTracker = true;
        }
    }
    return handled;
}

bool WebView::handleMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam, bool isHorizontal)
{
    PlatformWheelEvent wheelEvent(hWnd, wParam, lParam, isHorizontal);
    return frame()->eventHandler()->handleWheelEvent(wheelEvent);
}

bool WebView::handleKeyDown(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown)
{
    Frame* frame = m_page->focusController()->focusedOrMainFrame();

    PlatformKeyboardEvent keyEvent(m_windowHandle, virtualKeyCode, keyData, PlatformKeyboardEvent::RawKeyDown, systemKeyDown);
    bool handled = frame->eventHandler()->keyEvent(keyEvent);

    // These events cannot be canceled, and we have no default handling for them.
    // FIXME: match IE list more closely, see <http://msdn2.microsoft.com/en-us/library/ms536938.aspx>.
    if (systemKeyDown && virtualKeyCode != VK_RETURN)
        return false;

    if (handled) {
        MSG msg;
        if (!systemKeyDown)
            ::PeekMessage(&msg, m_windowHandle, WM_CHAR, WM_CHAR, PM_REMOVE);
        return true;
    }

    return handled;
}

bool WebView::handleKeyPress(WPARAM charCode, LPARAM keyData, bool systemKeyDown)
{
    Frame* frame = m_page->focusController()->focusedOrMainFrame();

    PlatformKeyboardEvent keyEvent(m_windowHandle, charCode, keyData, PlatformKeyboardEvent::Char, systemKeyDown);
    // IE does not dispatch keypress event for WM_SYSCHAR.
    if (systemKeyDown)
        return frame->eventHandler()->handleAccessKey(keyEvent);
    if (frame->eventHandler()->keyEvent(keyEvent))
        return true;

    return false;
}

bool WebView::handleKeyUp(WPARAM virtualKeyCode, LPARAM keyData, bool systemKeyDown)
{
    PlatformKeyboardEvent keyEvent(m_windowHandle, virtualKeyCode, keyData, PlatformKeyboardEvent::KeyUp, systemKeyDown);

    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    return frame->eventHandler()->keyEvent(keyEvent);
}

LRESULT CALLBACK WebView::_WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	I_LTFRAME_IWebView* pThis = NULL;
    pThis = reinterpret_cast<I_LTFRAME_IWebView*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if( uMsg == WM_NCDESTROY && pThis != NULL )
	{
	    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0L);
		pThis->OnFinalMessage();
		return 0;
	}
    if( pThis != NULL ) {
	   return pThis->HandleUserMessage(hWnd,uMsg, wParam, lParam);
    } 
   else {
      return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
   }
}



void WebView::ResizeWindow()
{

}


static bool initCommonControls()
{
    static bool haveInitialized = false;
    if (haveInitialized)
        return true;

    INITCOMMONCONTROLSEX init;
    init.dwSize = sizeof(init);
    init.dwICC = ICC_TREEVIEW_CLASSES;
    haveInitialized = !!::InitCommonControlsEx(&init);
    return haveInitialized;
}


void WebView::initializeToolTipWindow()
{
	if (!initCommonControls())
        return;

    m_toolTipHwnd = CreateWindowEx(0, TOOLTIPS_CLASS, 0, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                   m_windowHandle, 0, 0, 0);
    if (!m_toolTipHwnd)
        return;

    myTOOLINFO info = {0};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS ;
    info.uId = reinterpret_cast<UINT_PTR>(m_windowHandle);

    ::SendMessage(m_toolTipHwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&info));
    ::SendMessage(m_toolTipHwnd, TTM_SETMAXTIPWIDTH, 0, 250);

    ::SetWindowPos(m_toolTipHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WebView::setToolTip(const String& toolTip)
{

    if (!m_toolTipHwnd)
        return;

    if (toolTip == m_toolTip)
        return;

    m_toolTip = toolTip;

    if (!m_toolTip.isEmpty()) {
        myTOOLINFO info = {0};
        info.cbSize = sizeof(info);
        info.uFlags = TTF_IDISHWND;
        info.uId = reinterpret_cast<UINT_PTR>(m_windowHandle);
        info.lpszText = const_cast<UChar*>(m_toolTip.charactersWithNullTermination());
        ::SendMessage(m_toolTipHwnd, TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(&info));
    }

    ::SendMessage(m_toolTipHwnd, TTM_ACTIVATE, !m_toolTip.isEmpty(), 0);

}



int WebView::OnNcHitTest(HWND hWnd)
{
	if(!m_enabledragframechangesize)
	{
		return HTCLIENT;
	}
	POINT point;
	GetCursorPos(&point);
	RECT rect;
	GetWindowRect(hWnd,&rect);
	if (point.x <= rect.left+3)
		return HTLEFT;
	else if (point.x >= rect.right-3)
		return HTRIGHT;
	else if (point.y <= rect.top+3)
		return HTTOP;
	else if (point.y >= rect.bottom-3)
		return HTBOTTOM;
	else if (point.x <= rect.left+10 && point.y <= rect.top+10)
		return HTTOPLEFT;
	else if (point.x >= rect.right-10 && point.y <= rect.top+10)
		return HTTOPRIGHT;
	else if (point.x <= rect.left+10 && point.y >= rect.bottom-10)
		return HTBOTTOMLEFT;
	else if (point.x >= rect.right-10 && point.y >= rect.bottom-10)
		return HTBOTTOMRIGHT;
	else
		return HTCLIENT;
}

bool WebView::verticalScroll(WPARAM wParam, LPARAM /*lParam*/)
{
    ScrollDirection direction;
    ScrollGranularity granularity;
    switch (LOWORD(wParam)) {
    case SB_LINEDOWN:
        granularity = ScrollByLine;
        direction = ScrollDown;
        break;
    case SB_LINEUP:
        granularity = ScrollByLine;
        direction = ScrollUp;
        break;
    case SB_PAGEDOWN:
        granularity = ScrollByDocument;
        direction = ScrollDown;
        break;
    case SB_PAGEUP:
        granularity = ScrollByDocument;
        direction = ScrollUp;
        break;
    default:
        return false;
        break;
    }
    
    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    return frame->eventHandler()->scrollRecursively(direction, granularity);
}
bool WebView::horizontalScroll(WPARAM wParam, LPARAM /*lParam*/)
{
    ScrollDirection direction;
    ScrollGranularity granularity;
    switch (LOWORD(wParam)) {
    case SB_LINELEFT:
        granularity = ScrollByLine;
        direction = ScrollLeft;
        break;
    case SB_LINERIGHT:
        granularity = ScrollByLine;
        direction = ScrollRight;
        break;
    case SB_PAGELEFT:
        granularity = ScrollByDocument;
        direction = ScrollLeft;
        break;
    case SB_PAGERIGHT:
        granularity = ScrollByDocument;
        direction = ScrollRight;
        break;
    default:
        return false;
    }

    Frame* frame = m_page->focusController()->focusedOrMainFrame();
    return frame->eventHandler()->scrollRecursively(direction, granularity);
}

LRESULT WebView::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	LRESULT lResult = 0;
	if (PluginView::isCallingPlugin()) {
        if (message == WM_PAINT)
            PostMessage(hWnd, message, wParam, lParam);
        return 0;
    }

    bool handled = true;
    if (view()) {
        switch (message) {
        case WM_PAINT:
				paint(0, 0);
				if(islayered){
					performLayeredWindowUpdate();
				}
            break;
		case WM_ERASEBKGND:
			if(islayered){
				handled = true;
				lResult = 1;
			}
			break;
		case WM_PRINTCLIENT:
              paint((HDC)wParam, lParam);
            break;
		case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        //case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        //case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_LBUTTONUP:
        //case WM_MBUTTONUP:
        case WM_RBUTTONUP: 
		case WM_MOUSELEAVE:
		case WM_CANCELMODE:
			if (frame()->eventHandler() && view()->didFirstLayout())
                handled = handleMouseEvent(hWnd, message, wParam, lParam);
            break;
       
        case WM_MOUSEWHEEL:
            if (frame()->eventHandler() && view()->didFirstLayout())
                handled = handleMouseWheel(hWnd, wParam, lParam, wParam & MK_SHIFT);
            break;

        case WM_SYSKEYDOWN:
            handled = handleKeyDown(wParam, lParam, true);
            break;

        case WM_KEYDOWN:
            handled = handleKeyDown(wParam, lParam, false);
            break;

        case WM_SYSKEYUP:
            handled = handleKeyUp(wParam, lParam, true);
            break;

        case WM_KEYUP:
            handled = handleKeyUp(wParam, lParam, false);
            break;

        case WM_SYSCHAR:
            handled = handleKeyPress(wParam, lParam, true);
            break;

        case WM_CHAR:
            handled = handleKeyPress(wParam, lParam, false);
            break;
        case WM_DESTROY:
			{
				Close();
				if(!::GetParent(hWnd) && !::GetWindow(hWnd,GW_OWNER))
					QuitApp();
			}
            break;
		case WM_SIZE:
			handled = OnSize(hWnd);
			break;

		case WM_NCHITTEST:
			  return OnNcHitTest(hWnd);
			break;
		case WM_TIMER:
		   {

			    switch (wParam) {
                case UpdateActiveStateTimer:
                    KillTimer(hWnd, UpdateActiveStateTimer);
                    updateActiveState();
                    break;
                case DeleteBackingStoreTimer:
                    deleteBackingStore();
                    break;
				default:
					{
							if(JSGC==wParam && m_autojsgc){     
								PROCESS_MEMORY_COUNTERS pmc;
								GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
								float fCurUse = float(pmc.WorkingSetSize / (1024*1024));
								if(fCurUse>m_maxmemorysize || (::GetTickCount()-m_jsgcstarttime)/1000>300)
								{
									try
									{
										gcController().garbageCollectNow();
										m_jsgcstarttime = ::GetTickCount();
									}
									catch (...)
									{
										
									}
									
								}

							}
					}
					break;
                }
		   }
	      break;
		//case WM_NCLBUTTONDOWN:
		//		POINT point;
		//		GetCursorPos(&point);
		//		switch(wParam)
		//		{
		//		case HTTOP:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTBOTTOM:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTLEFT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTRIGHT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTTOPLEFT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTTOPRIGHT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTBOTTOMLEFT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(point.x, point.y));
		//			break;
		//		case HTBOTTOMRIGHT:
		//			SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(point.x, point.y));
		//			break;
		//	
		//		}
		//	break;
		  /*  case WM_SETCURSOR:
				handled = ::SetCursor(m_lastSetCursor);
            break;*/
        case WM_SETFOCUS: {
            FocusController* focusController = m_page->focusController();
                focusController->setFocused(true);
            break;
        }
        case WM_KILLFOCUS: {
            FocusController* focusController = m_page->focusController();
                focusController->setFocused(false);
            break;
        }
		 case WM_MOUSEACTIVATE:
            setMouseActivated(true);
            handled = false;
            break;
		  case WM_VSCROLL:
            handled =verticalScroll(wParam, lParam);
            break;
          case WM_HSCROLL:
            handled =horizontalScroll(wParam, lParam);
            break;
		  default:
				handled = false;
			break;;
        }
    }

    if (!handled)
       lResult =  DefWindowProc(hWnd, message, wParam, lParam);

	return (message == WM_KEYDOWN || message == WM_SYSKEYDOWN || message == WM_KEYUP || message == WM_SYSKEYUP) ? !handled : lResult;
  
}

/*paint part*/
static void getUpdateRects(HRGN region, const IntRect& dirtyRect, Vector<IntRect>& rects)
{
    ASSERT_ARG(region, region);

    const int cRectThreshold = 10;
    const float cWastedSpaceThreshold = 0.75f;

    rects.clear();

    DWORD regionDataSize = GetRegionData(region, sizeof(RGNDATA), NULL);
    if (!regionDataSize) {
        rects.append(dirtyRect);
        return;
    }

    Vector<unsigned char> buffer(regionDataSize);
    RGNDATA* regionData = reinterpret_cast<RGNDATA*>(buffer.data());
    GetRegionData(region, regionDataSize, regionData);
    if (regionData->rdh.nCount > cRectThreshold) {
        rects.append(dirtyRect);
        return;
    }

    double singlePixels = 0.0;
    unsigned i;
    RECT* rect;
    for (i = 0, rect = reinterpret_cast<RECT*>(regionData->Buffer); i < regionData->rdh.nCount; i++, rect++)
        singlePixels += (rect->right - rect->left) * (rect->bottom - rect->top);

    double unionPixels = dirtyRect.width() * dirtyRect.height();
    double wastedSpace = 1.0 - (singlePixels / unionPixels);
    if (wastedSpace <= cWastedSpaceThreshold) {
        rects.append(dirtyRect);
        return;
    }

    for (i = 0, rect = reinterpret_cast<RECT*>(regionData->Buffer); i < regionData->rdh.nCount; i++, rect++)
        rects.append(*rect);
}

void WebView::repaint(const WebCore::IntRect& windowRect, bool contentChanged, bool immediate, bool repaintContentOnly)
{
#if USE(ACCELERATED_COMPOSITING)
    if (isAcceleratedCompositing()) {
        // The contentChanged, immediate, and repaintContentOnly parameters are all based on a non-
        // compositing painting/scrolling model.
        addToDirtyRegion(windowRect);
        return;
    }
#endif

    if (!repaintContentOnly) {
        RECT rect = windowRect;
        ::InvalidateRect(m_windowHandle, &rect, false);
    }
    if (contentChanged)
        addToDirtyRegion(windowRect);
    if (immediate) {
        if (repaintContentOnly)
            updateBackingStore(view());
        else
            ::UpdateWindow(m_windowHandle);
    }
}
void WebView::addToDirtyRegion(const IntRect& dirtyRect)
{
    // FIXME: We want an assert here saying that the dirtyRect is inside the clienRect,
    // but it was being hit during our layout tests, and is being investigated in
    // http://webkit.org/b/29350.

#if USE(ACCELERATED_COMPOSITING)
    if (isAcceleratedCompositing()) {
        m_backingLayer->setNeedsDisplayInRect(dirtyRect);
        return;
    }
#endif

    HRGN newRegion = ::CreateRectRgn(dirtyRect.x(), dirtyRect.y(),
                                     dirtyRect.maxX(), dirtyRect.maxY());
    addToDirtyRegion(newRegion);
}

void WebView::addToDirtyRegion(HRGN newRegion)
{
#if USE(ACCELERATED_COMPOSITING)
    ASSERT(!isAcceleratedCompositing());
#endif

    //LOCAL_GDI_COUNTER(0, __FUNCTION__);

    if (m_backingStoreDirtyRegion) {
        HRGN combinedRegion = ::CreateRectRgn(0,0,0,0);
        ::CombineRgn(combinedRegion, m_backingStoreDirtyRegion->handle(), newRegion, RGN_OR);
        ::DeleteObject(newRegion);
        m_backingStoreDirtyRegion = RefCountedHRGN::create(combinedRegion);
    } else
        m_backingStoreDirtyRegion = RefCountedHRGN::create(newRegion);

}
void WebView::updateBackingStore(FrameView* frameView, HDC dc, bool backingStoreCompletelyDirty, WindowsToPaint windowsToPaint)
{
#if USE(ACCELERATED_COMPOSITING)
    ASSERT(!isAcceleratedCompositing());
#endif

    //LOCAL_GDI_COUNTER(0, __FUNCTION__);

    HDC bitmapDC = dc;
    HGDIOBJ oldBitmap = 0;
    if (!dc) {
        HWndDC windowDC(m_windowHandle);
        bitmapDC = ::CreateCompatibleDC(windowDC);
        oldBitmap = ::SelectObject(bitmapDC, m_backingStoreBitmap->handle());
    }

    if (m_backingStoreBitmap && (m_backingStoreDirtyRegion || backingStoreCompletelyDirty)) {
        // Do a layout first so that everything we render to the backing store is always current.
      
           
                view()->updateLayoutAndStyleIfNeededRecursive();

        Vector<IntRect> paintRects;
        if (!backingStoreCompletelyDirty && m_backingStoreDirtyRegion) {
            RECT regionBox;
            ::GetRgnBox(m_backingStoreDirtyRegion->handle(), &regionBox);
            getUpdateRects(m_backingStoreDirtyRegion->handle(), regionBox, paintRects);
        } else {
            RECT clientRect;
            ::GetClientRect(m_windowHandle, &clientRect);
            paintRects.append(clientRect);
        }

        for (unsigned i = 0; i < paintRects.size(); ++i)
            paintIntoBackingStore(frameView, bitmapDC, paintRects[i], windowsToPaint);

        //if (m_uiDelegatePrivate)
        //    m_uiDelegatePrivate->webViewPainted(this);

        m_backingStoreDirtyRegion.clear();
    }

    if (!dc) {
        ::SelectObject(bitmapDC, oldBitmap);
        ::DeleteDC(bitmapDC);
    }

    GdiFlush();
}
void WebView::paintIntoBackingStore(FrameView* frameView, HDC bitmapDC, const IntRect& dirtyRect, WindowsToPaint windowsToPaint)
{
    // FIXME: This function should never be called in accelerated compositing mode, and we should
    // assert as such. But currently it *is* sometimes called, so we can't assert yet. See
    // <http://webkit.org/b/58539>.

   // LOCAL_GDI_COUNTER(0, __FUNCTION__);

    // FIXME: We want an assert here saying that the dirtyRect is inside the clienRect,
    // but it was being hit during our layout tests, and is being investigated in
    // http://webkit.org/b/29350.

    RECT rect = dirtyRect;

#if FLASH_BACKING_STORE_REDRAW
    {
        HWndDC dc(m_viewWindow);
        OwnPtr<HBRUSH> yellowBrush(CreateSolidBrush(RGB(255, 255, 0)));
        FillRect(dc, &rect, yellowBrush.get());
        GdiFlush();
        Sleep(50);
        paintIntoWindow(bitmapDC, dc, dirtyRect);
    }
#endif

    GraphicsContext gc(bitmapDC, islayered);
    gc.setShouldIncludeChildWindows(windowsToPaint == PaintWebViewAndChildren);
    gc.save();
    if (islayered)
        gc.clearRect(dirtyRect);
    else
        FillRect(bitmapDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    //COMPtr<IWebUIDelegatePrivate2> uiPrivate(Query, m_uiDelegate);
    //if (uiPrivate)
    //    uiPrivate->drawBackground(this, reinterpret_cast<OLE_HANDLE>(bitmapDC), &rect);

    if (frameView && frameView->frame() && frameView->frame()->contentRenderer()) {
        gc.clip(dirtyRect);
        frameView->paint(&gc, dirtyRect);
    }
    gc.restore();
}
void WebView::paint(HDC dc, LPARAM options)
{
    //LOCAL_GDI_COUNTER(0, __FUNCTION__);

#if USE(ACCELERATED_COMPOSITING)
    if (isAcceleratedCompositing() && !usesLayeredWindow()) {
        m_layerTreeHost->flushPendingLayerChangesNow();
        // Flushing might have taken us out of compositing mode.
        if (isAcceleratedCompositing()) {
            // FIXME: We need to paint into dc (if provided). <http://webkit.org/b/52578>
            m_layerTreeHost->paint();
            ::ValidateRect(m_viewWindow, 0);
            return;
        }
    }
#endif
    FrameView* frameView = view();

    RECT rcPaint;
    HDC hdc;
    OwnPtr<HRGN> region;
    int regionType = NULLREGION;
    PAINTSTRUCT ps;
    WindowsToPaint windowsToPaint;
    if (!dc) {
        region = adoptPtr(CreateRectRgn(0,0,0,0));
        regionType = GetUpdateRgn(m_windowHandle, region.get(), false);
        hdc = BeginPaint(m_windowHandle, &ps);
        rcPaint = ps.rcPaint;
        // We're painting to the screen, and our child windows can handle
        // painting themselves to the screen.
        windowsToPaint = PaintWebViewOnly;
    } else {
        hdc = dc;
        ::GetClientRect(m_windowHandle, &rcPaint);
        if (options & PRF_ERASEBKGND)
            ::FillRect(hdc, &rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
        // Since we aren't painting to the screen, we want to paint all our
        // children into the HDC.
        windowsToPaint = PaintWebViewAndChildren;
    }

    bool backingStoreCompletelyDirty = ensureBackingStore();
    if (!m_backingStoreBitmap) {
        if (!dc)
            EndPaint(m_windowHandle, &ps);
        return;
    }


    HDC bitmapDC = ::CreateCompatibleDC(hdc);
    HGDIOBJ oldBitmap = ::SelectObject(bitmapDC, m_backingStoreBitmap->handle());

    // Update our backing store if needed.
    updateBackingStore(frameView, bitmapDC, backingStoreCompletelyDirty, windowsToPaint);

    // Now we blit the updated backing store
    IntRect windowDirtyRect = rcPaint;
    
    // Apply the same heuristic for this update region too.
    Vector<IntRect> blitRects;
    if (region && regionType == COMPLEXREGION)
        getUpdateRects(region.get(), windowDirtyRect, blitRects);
    else
        blitRects.append(windowDirtyRect);

    for (unsigned i = 0; i < blitRects.size(); ++i)
        paintIntoWindow(bitmapDC, hdc, blitRects[i]);

    ::SelectObject(bitmapDC, oldBitmap);
    ::DeleteDC(bitmapDC);

    if (!dc)
        EndPaint(m_windowHandle, &ps);


    if (active())
        cancelDeleteBackingStoreSoon();
    else
        deleteBackingStoreSoon();
}
void WebView::paintIntoWindow(HDC bitmapDC, HDC windowDC, const IntRect& dirtyRect)
{
    // FIXME: This function should never be called in accelerated compositing mode, and we should
    // assert as such. But currently it *is* sometimes called, so we can't assert yet. See
    // <http://webkit.org/b/58539>.

    //LOCAL_GDI_COUNTER(0, __FUNCTION__);
#if FLASH_WINDOW_REDRAW
    OwnPtr<HBRUSH> greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    RECT rect = dirtyRect;
    FillRect(windowDC, &rect, greenBrush.get());
    GdiFlush();
    Sleep(50);
#endif

    // Blit the dirty rect from the backing store into the same position
    // in the destination DC.
	if(!islayered)
    BitBlt(windowDC, dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height(), bitmapDC,
           dirtyRect.x(), dirtyRect.y(), SRCCOPY);
}
void WebView::deleteBackingStoreSoon()
{
    if (pendingDeleteBackingStoreSet.size() > 2) {
        Vector<WebView*> views;
        HashSet<WebView*>::iterator end = pendingDeleteBackingStoreSet.end();
        for (HashSet<WebView*>::iterator it = pendingDeleteBackingStoreSet.begin(); it != end; ++it)
            views.append(*it);
        for (int i = 0; i < views.size(); ++i)
            views[i]->deleteBackingStore();
        ASSERT(pendingDeleteBackingStoreSet.isEmpty());
    }

    pendingDeleteBackingStoreSet.add(this);
    m_deleteBackingStoreTimerActive = true;
    SetTimer(m_windowHandle, DeleteBackingStoreTimer, delayBeforeDeletingBackingStoreMsec, 0);
}

void WebView::cancelDeleteBackingStoreSoon()
{
    if (!m_deleteBackingStoreTimerActive)
        return;
    pendingDeleteBackingStoreSet.remove(this);
    m_deleteBackingStoreTimerActive = false;
    KillTimer(m_windowHandle, DeleteBackingStoreTimer);
}

bool WebView::ensureBackingStore()
{
    RECT windowRect;
    ::GetClientRect(m_windowHandle, &windowRect);
    LONG width = windowRect.right - windowRect.left;
    LONG height = windowRect.bottom - windowRect.top;
    if (width > 0 && height > 0 && (width != m_backingStoreSize.cx || height != m_backingStoreSize.cy)) {
        deleteBackingStore();

        m_backingStoreSize.cx = width;
        m_backingStoreSize.cy = height;
        BitmapInfo bitmapInfo = BitmapInfo::createBottomUp(IntSize(m_backingStoreSize));

        void* pixels = NULL;
        m_backingStoreBitmap = RefCountedHBITMAP::create(::CreateDIBSection(0, &bitmapInfo, DIB_RGB_COLORS, &pixels, 0, 0));
        return true;
    }

    return false;
}
void WebView::deleteBackingStore()
{
    pendingDeleteBackingStoreSet.remove(this);

    if (m_deleteBackingStoreTimerActive) {
        KillTimer(m_windowHandle, DeleteBackingStoreTimer);
        m_deleteBackingStoreTimerActive = false;
    }
    m_backingStoreBitmap.clear();
    m_backingStoreDirtyRegion.clear();
    m_backingStoreSize.cx = m_backingStoreSize.cy = 0;
}
void WebView::performLayeredWindowUpdate()
{
    // The backing store may have been destroyed if the window rect was set to zero height or zero width.
    if (!m_backingStoreBitmap)
        return;

    HWndDC hdcScreen(m_windowHandle);
    OwnPtr<HDC> hdcMem = adoptPtr(::CreateCompatibleDC(hdcScreen));
    HBITMAP hbmOld = static_cast<HBITMAP>(::SelectObject(hdcMem.get(), m_backingStoreBitmap->handle()));

    BITMAP bmpInfo;
    ::GetObject(m_backingStoreBitmap->handle(), sizeof(bmpInfo), &bmpInfo);
    SIZE windowSize = { bmpInfo.bmWidth, bmpInfo.bmHeight };

    BLENDFUNCTION blendFunction;
    blendFunction.BlendOp = AC_SRC_OVER;
    blendFunction.BlendFlags = 0;
    blendFunction.SourceConstantAlpha = 0xFF;
    blendFunction.AlphaFormat = AC_SRC_ALPHA;

    POINT layerPos = { 0, 0 };
    ::UpdateLayeredWindow(m_windowHandle, hdcScreen, 0, &windowSize, hdcMem.get(), &layerPos, 0, &blendFunction, ULW_ALPHA);

    ::SelectObject(hdcMem.get(), hbmOld);
}
static HWND findTopLevelParent(HWND window)
{
    if (!window)
        return 0;

    HWND current = window;
    for (HWND parent = GetParent(current); current; current = parent, parent = GetParent(parent))
        if (!parent || !(GetWindowLongPtr(current, GWL_STYLE) & (WS_POPUP | WS_CHILD)))
            return current;
    ASSERT_NOT_REACHED();
    return 0;
}
bool WebView::active()
{
    HWND activeWindow = GetActiveWindow();
    if (islayered && activeWindow == m_windowHandle)
        return true;

    return (activeWindow && m_topLevelParent == findTopLevelParent(activeWindow));
}
void WebView::windowAncestryDidChange()
{
    //HWND newParent;
    //if (m_windowHandle)
    //    newParent = findTopLevelParent(m_hostWindow);
    //else {
    //    // There's no point in tracking active state changes of our parent window if we don't have
    //    // a window ourselves.
    //    newParent = 0;
    //}

    //m_topLevelParent = newParent;


    //updateActiveState();



	HWND newParent = findTopLevelParent(m_windowHandle);
	if (newParent == m_topLevelParent)
		return;
	m_topLevelParent = newParent;

	updateActiveState();

}
void WebView::updateActiveState()
{
    m_page->focusController()->setActive(active());
}

void WebView::scrollBackingStore(int dx, int dy, const IntRect& scrollViewRect, const IntRect& clipRect)
{
#if USE(ACCELERATED_COMPOSITING)
    if (isAcceleratedCompositing()) {
        // FIXME: We should be doing something smarter here, like moving tiles around and painting
        // any newly-exposed tiles. <http://webkit.org/b/52714>
        m_backingLayer->setNeedsDisplayInRect(scrollViewRect);
        return;
    }
#endif
	if(!m_backingStoreBitmap)return;
  
    // If there's no backing store we don't need to update it

    // Make a region to hold the invalidated scroll area.
    HRGN updateRegion = ::CreateRectRgn(0, 0, 0, 0);

    // Collect our device context info and select the bitmap to scroll.
    HWndDC windowDC(m_windowHandle);
    HDC bitmapDC = ::CreateCompatibleDC(windowDC);
    HGDIOBJ oldBitmap = ::SelectObject(bitmapDC, m_backingStoreBitmap->handle());
    
    // Scroll the bitmap.
    RECT scrollRectWin(scrollViewRect);
    RECT clipRectWin(clipRect);
    ::ScrollDC(bitmapDC, dx, dy, &scrollRectWin, &clipRectWin, updateRegion, 0);
    RECT regionBox;
    ::GetRgnBox(updateRegion, &regionBox);

    // Flush.
    GdiFlush();

    // Add the dirty region to the backing store's dirty region.
    addToDirtyRegion(updateRegion);


    // Update the backing store.
    updateBackingStore(view(), bitmapDC, false);

    // Clean up.
    ::SelectObject(bitmapDC, oldBitmap);
    ::DeleteDC(bitmapDC);
}

/*paint part*/

void WebView::frameRect(RECT* rect) const
{
	GetWindowRect(m_windowHandle, rect);
}

HWND WebView::CreateUiWindow(LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
int x,int y,int nWidth,int nHeight)
{

    HWND WINHWND  = CreateWindow(m_windowclassname, lpWindowName, dwStyle,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWndParent, (HMENU)NULL, WebCore::instanceHandle(), NULL);

	//MoveWindow(WINHWND, x, y, nWidth, nHeight, TRUE);
	ShowWindow(WINHWND,SW_SHOW); 
	UpdateWindow(WINHWND); 	

	return WINHWND;
}

void WebView::ShowMainWindow()
{

	MoveWindow(m_windowHandle, winrect.x, winrect.y, winrect.cx, winrect.cy, TRUE);
	initializeToolTipWindow();
}
void WebView::IsAutoGC(bool t,int maxmemorysize){

	m_maxmemorysize = maxmemorysize;
	m_autojsgc = t;
}
void WebView::CleanMemory()
{
	HANDLE _m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_SET_QUOTA, 0,GetCurrentProcessId());
	SetProcessWorkingSetSize(_m_hProcess, -1, -1);
	EmptyWorkingSet(_m_hProcess);
	CloseHandle(_m_hProcess);
}
void WebView::SetWebSqlPath(const wchar_t* path)
{
	if(path==NULL)
	{
		WebCore::DatabaseTracker::initializeTracker(String("C:\\LTSQLDatabase"));
	}
	else{
		WebCore::DatabaseTracker::initializeTracker(String(path));
	}
}
void WebView::SetLocalStoragePath(const wchar_t* path)
{
	if(path==NULL)
	{
		m_page->settings()->setLocalStorageDatabasePath(String("C:\\LTLocalStorage"));
	}
	else{
		m_page->settings()->setLocalStorageDatabasePath(String(path));
	}

}


void WebView::SetViewTransparent(bool v)
{
	if(v){

		SetWindowLong(m_windowHandle,GWL_EXSTYLE,GetWindowLong(m_windowHandle,GWL_EXSTYLE)|0x80000);
		SetViewBgColor(true);

	}
	else
	{
		SetWindowLong(m_windowHandle,GWL_EXSTYLE,GetWindowLong(m_windowHandle,GWL_EXSTYLE)&~0x80000);
		SetViewBgColor(false);
	}
	deleteBackingStore();
	islayered  = v;

	RECT rect;
	::GetClientRect(m_windowHandle,&rect);
	InvalidateRect(m_windowHandle, &rect, FALSE);
	UpdateWindow(m_windowHandle);
}

void WebView::SetViewBgColor(bool islayered)
{
	WebCore::Color backgroundColor = islayered ?  WebCore::Color::transparent :WebCore::Color::white ;
	view()->setBaseBackgroundColor
		(backgroundColor);
}

void WebView::CloseWindow()
{
	PostMessage(m_windowHandle, WM_CLOSE, NULL, NULL);
}

void WebView::QuitApp()
{
	PostMessage(m_windowHandle, WM_QUIT, 0, NULL);
}
bool WebView::IsTransparentView()
{
	return islayered ;
	/*return true;*/
}

JSC::EncodedJSValue  JSC_HOST_CALL garbageColl(JSC::ExecState* es)
{
	gcController().garbageCollectNow();
	return JSC::JSValue::encode(JSC::jsUndefined());
}

JSC::EncodedJSValue  JSC_HOST_CALL garbageColltt(JSC::ExecState* es)
{
	gcController().garbageCollectNow();
	return JSC::JSValue::encode(JSC::jsUndefined());
}


void WebView::BindFunction(JSC::JSGlobalObject* globalObject, const char* name, JSC::NativeFunction function, unsigned int argCount)
{
    JSC::ExecState* exec = globalObject->globalExec();

    JSC::Identifier identifier(exec, name);
    JSC::JSFunction* funcObject = JSC::JSFunction::create(exec, globalObject, argCount, identifier, function);

    globalObject->putDirect(globalObject->globalData(), identifier, funcObject);
}
void WebView::AddJsToFunList(WebCore::JSDOMWindow* jsdow)
{
      //JSC::ExecState* exec = jsdow->globalExec();
	  BindFunction(jsdow,"garbageCollect",garbageColl,0);
	  // BindFunction(jsdow,"ccc",js_getWebViewName,0);
	  //   BindFunction(jsdow,"mmm",jsTrue,0);

    for (size_t i = 0; i < JsFunInfoList.size(); ++i)
    {
        BindFunction(jsdow, JsFunInfoList[i].name, JsFunInfoList[i].fn, JsFunInfoList[i].argCount);
    }
		 
}

void WebView::BindUserFunction(const char* name, LTNativeFunction fn, unsigned int argCount)
{
	for (unsigned int i = 0; i < JsFunInfoList.size(); ++i)
    {
        if (strncmp(name, JsFunInfoList[i].name, MAX_NAME_LENGTH) == 0)
        {
			JsFunInfoList[i].fn = (JSC::NativeFunction)fn;
            JsFunInfoList[i].argCount = argCount;
            return;
        }
    }
    JsFunInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = (JSC::NativeFunction)fn;
    funcInfo.argCount = argCount;
    JsFunInfoList.append(funcInfo);
}

void cairo_blur_image_surface(cairo_surface_t *surface, int radius)
{
	int kernelSize;
	int kernelSum = 0;
	int *kernel = NULL, **mult = NULL;

	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
	{
		return;
	}

	if(true) /* precalc stuff */
	{
		kernelSize = radius * 2 + 1;
		kernel = new int[kernelSize];
		mult = new int*[kernelSize];
		for(int i = 0; i < kernelSize; i++)
		{
			mult[i] = new int[256];
		}

		for(int i = 1; i <= radius; i++)
		{
			int szi = radius - i, szj = radius + i;
			kernel[szj] = kernel[szi] = (szi + 1) * (szi + 1);
			kernelSum += (kernel[szj] + kernel[szi]);
			for(int j = 0; j < 256; j++)
			{
				mult[szj][j] = mult[szi][j] = kernel[szj] * j;
			}
		}
		kernel[radius] = (radius + 1) * (radius + 1);
		kernelSum += kernel[radius];
		for(int j = 0; j < 256; j++)
		{
			mult[radius][j] = kernel[radius] * j;
		}
	}

	int width, height;
	uint8_t *src;

	width = cairo_image_surface_get_width(surface);
	height = cairo_image_surface_get_height(surface);

	// we get: BGRA!
	src = cairo_image_surface_get_data(surface);

	int wh = width * height;

	uint8_t *b2 = new uint8_t[wh];
	uint8_t *g2 = new uint8_t[wh];
	uint8_t *r2 = new uint8_t[wh];
	uint8_t *a2 = new uint8_t[wh];

	int asum, bsum, gsum, rsum;
	int start = 0;
	int index = 0;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			asum = bsum = gsum = rsum = 0;
			int read = index - radius;

			for(int z = 0; z < kernelSize; z++)
			{
				int idx = read;

				if(read < start)
					idx = start;
				else if(read > start + width - 1)
					idx = start + width - 1;

				uint8_t* srcptr = src + idx * 4;
				bsum += mult[z][*(srcptr + 0)];
				gsum += mult[z][*(srcptr + 1)];
				rsum += mult[z][*(srcptr + 2)];
				asum += mult[z][*(srcptr + 3)];

				read++;
			}
			b2[index] = (bsum / kernelSum);
			g2[index] = (gsum / kernelSum);
			r2[index] = (rsum / kernelSum);
			a2[index] = (asum / kernelSum);
			index++;
		}
		start += width;
	}

	int tempy;
	uint32_t* ptr = (uint32_t*)src;

	for(int i = 0; i < height; i++)
	{
		int y = i - radius;
		start = y * width;
		for(int j = 0; j < width; j++)
		{
			asum = bsum = gsum = rsum = 0;
			int read = start + j;
			tempy = y;
			for(int z = 0; z < kernelSize; z++)
			{
				int idx = read;

				if(tempy < 0)
					idx = j;
				else if(tempy > height -1)
					idx = wh - (width - j);

				bsum += mult[z][b2[idx]];
				gsum += mult[z][g2[idx]];
				rsum += mult[z][r2[idx]];
				asum += mult[z][a2[idx]];

				read += width;
				++tempy;
			}

			*(ptr++) = (asum / kernelSum) << 24 | (rsum / kernelSum) << 16 | (gsum / kernelSum) << 8 | bsum / kernelSum;
		}
	}

	delete[] kernel;
	if(mult) for(int i = 0; i < kernelSize; i++) delete[] mult[i];
	delete[] mult;
	delete[] b2;
	delete[] g2;
	delete[] r2;
	delete[] a2;

	cairo_surface_mark_dirty(surface);
}

void WebView::GaussianBlurImage(HBITMAP hBitmap,int radius)
{
	BITMAP info;
	GetObject(hBitmap, sizeof(info), &info);
	cairo_surface_t *image;
	image =cairo_image_surface_create_for_data((unsigned char*)info.bmBits,CAIRO_FORMAT_RGB24,info.bmWidth,info.bmHeight,info.bmWidthBytes);
	cairo_blur_image_surface(image,radius);
	cairo_surface_destroy (image);
}