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

#include "config.h"
#include "ChromeClientWinCE.h"
#include "FileChooser.h"
#include "FileIconLoader.h"
#include "Icon.h"
#include "NotImplemented.h"
#include "NavigationAction.h"
#include "PopupMenuWin.h"
#include "SearchPopupMenuWin.h"
#include "WebView.h"
#include <wtf/text/CString.h>
#include <WebCore/FileChooser.h>
#include <WebCore/LocalWindowsContext.h>
#include <WebCore/BString.h>
#include <WebCore/LocalizedStrings.h>
#include <WebCore/DatabaseTracker.h>
#include <WebCore/Frame.h>
using namespace WebCore;
static const size_t maxFilePathsListSize = USHRT_MAX;

namespace WebKit {

ChromeClientWinCE::ChromeClientWinCE(WebView* webView)
    : m_webView(webView)
{
    ASSERT(m_webView);
}

void ChromeClientWinCE::chromeDestroyed()
{
    delete this;
}

FloatRect ChromeClientWinCE::windowRect()
{
    if (!m_webView)
        return FloatRect();

    RECT rect;
    m_webView->frameRect(&rect);
    return rect;
}

void ChromeClientWinCE::setWindowRect(const FloatRect&)
{
    notImplemented();
}

FloatRect ChromeClientWinCE::pageRect()
{
    return windowRect();
}

void ChromeClientWinCE::focus()
{
    notImplemented();
}

void ChromeClientWinCE::unfocus()
{
    notImplemented();
}

Page* ChromeClientWinCE::createWindow(Frame*, const FrameLoadRequest&, const WindowFeatures&, const NavigationAction&)
{
    notImplemented();
    return 0;
}

void ChromeClientWinCE::show()
{
    notImplemented();
}

bool ChromeClientWinCE::canRunModal()
{
    notImplemented();
    return false;
}

void ChromeClientWinCE::runModal()
{
    notImplemented();
}

void ChromeClientWinCE::setToolbarsVisible(bool)
{
    notImplemented();
}

bool ChromeClientWinCE::toolbarsVisible()
{
    return false;
}

void ChromeClientWinCE::setStatusbarVisible(bool)
{
    notImplemented();
}

bool ChromeClientWinCE::statusbarVisible()
{
    notImplemented();
    return false;
}

void ChromeClientWinCE::setScrollbarsVisible(bool d)
{
	bool x = d;
    notImplemented();
}

bool ChromeClientWinCE::scrollbarsVisible()
{
  //  notImplemented();
    return false;
}

void ChromeClientWinCE::setMenubarVisible(bool)
{
    notImplemented();
}

bool ChromeClientWinCE::menubarVisible()
{
    notImplemented();
    return false;
}

void ChromeClientWinCE::setResizable(bool)
{
    notImplemented();
}

void ChromeClientWinCE::closeWindowSoon()
{
    PostMessageW(m_webView->windowHandle(), WM_CLOSE, 0, 0);
}

bool ChromeClientWinCE::canTakeFocus(FocusDirection)
{
    return true;
}

void ChromeClientWinCE::takeFocus(FocusDirection)
{
    unfocus();
}

void ChromeClientWinCE::focusedNodeChanged(Node*)
{
    notImplemented();
}

void ChromeClientWinCE::focusedFrameChanged(Frame*)
{
}

bool ChromeClientWinCE::canRunBeforeUnloadConfirmPanel()
{
    return true;
}

bool ChromeClientWinCE::runBeforeUnloadConfirmPanel(const String& message, Frame* frame)
{
    return runJavaScriptConfirm(frame, message);
}

void ChromeClientWinCE::addMessageToConsole(MessageSource, MessageType, MessageLevel, const String&, unsigned int, const String&)
{
    notImplemented();
}

void ChromeClientWinCE::runJavaScriptAlert(Frame*, const String& message)
{

    m_webView->runJavaScriptAlert(BString(message));
}

bool ChromeClientWinCE::runJavaScriptConfirm(Frame*, const String& message)
{
    return m_webView->runJavaScriptConfirm(message);
	//return true;
}

bool ChromeClientWinCE::runJavaScriptPrompt(Frame*, const String& message, const String& defaultValue, String& result)
{
    return m_webView->runJavaScriptPrompt(message, defaultValue, result);
}

void ChromeClientWinCE::setStatusbarText(const String&)
{
    notImplemented();
}

bool ChromeClientWinCE::shouldInterruptJavaScript()
{
    notImplemented();
    return false;
}

KeyboardUIMode ChromeClientWinCE::keyboardUIMode()
{
    return KeyboardAccessTabsToLinks;
}

IntRect ChromeClientWinCE::windowResizerRect() const
{
    notImplemented();
    return IntRect();
}

//void ChromeClientWinCE::invalidateRootView(const IntRect& windowRect, bool immediate)
//{
//    invalidateContentsAndRootView(windowRect, immediate);
//}
//
//void ChromeClientWinCE::invalidateContentsAndRootView(const IntRect& windowRect, bool immediate)
//{
//    //m_webView->repaint(windowRect, true /*contentChanged*/, immediate /*immediate*/, false /*repaintContentOnly*/);
//	RECT rect = windowRect;
//    InvalidateRect(m_webView->windowHandle(), &rect, true);
//
//    if (immediate)
//        UpdateWindow(m_webView->windowHandle());
//}
//
//void ChromeClientWinCE::invalidateContentsForSlowScroll(const IntRect& windowRect, bool immediate)
//{
//   invalidateContentsAndRootView(windowRect, immediate);
//}


void ChromeClientWinCE::invalidateRootView(const IntRect& windowRect, bool immediate)
{
    m_webView->repaint(windowRect, false /*contentChanged*/, immediate, false /*repaintContentOnly*/);
}

void ChromeClientWinCE::invalidateContentsAndRootView(const IntRect& windowRect, bool immediate)
{
    m_webView->repaint(windowRect, true /*contentChanged*/, immediate /*immediate*/, false /*repaintContentOnly*/);
}

void ChromeClientWinCE::invalidateContentsForSlowScroll(const IntRect& windowRect, bool immediate)
{
    m_webView->repaint(windowRect, true /*contentChanged*/, immediate, true /*repaintContentOnly*/);
}

void ChromeClientWinCE::scroll(const IntSize& delta, const IntRect& scrollViewRect, const IntRect& clipRect)
{
    m_webView->scrollBackingStore(delta.width(), delta.height(), scrollViewRect, clipRect);
}


#if ENABLE(SQL_DATABASE)
void ChromeClientWinCE::exceededDatabaseQuota(WebCore::Frame* frame, const String&)
{
	const unsigned long long defaultQuota = 5 * 1024 * 1024; 
	DatabaseTracker::tracker().setQuota(frame->document()->securityOrigin(), defaultQuota);
}
#endif

//void ChromeClientWinCE::scroll(const IntSize&, const IntRect& rectToScroll, const IntRect&)
//{
//   invalidateContentsAndRootView(rectToScroll, false);
//}

IntRect ChromeClientWinCE::rootViewToScreen(const IntRect& rect) const
{
    notImplemented();
    return rect;
}

IntPoint ChromeClientWinCE::screenToRootView(const IntPoint& point) const
{
    notImplemented();
    return point;
}

void ChromeClientWinCE::runOpenPanel(Frame*, PassRefPtr<FileChooser> prpFileChooser)
{
	RefPtr<FileChooser> fileChooser = prpFileChooser;

	HWND viewWindow = m_webView->windowHandle();
	//if (FAILED(m_webView->viewWindow(reinterpret_cast<OLE_HANDLE*>(&viewWindow))))
	//	return;

	bool multiFile = fileChooser->settings().allowsMultipleFiles;
	Vector<WCHAR> fileBuf(multiFile ? maxFilePathsListSize : MAX_PATH);

	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));

	// Need to zero out the first char of fileBuf so GetOpenFileName doesn't think it's an initialization string
	fileBuf[0] = '\0';

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = viewWindow;
	String allFiles = allFilesText();
	allFiles.append(L"\0*.*\0\0", 6);
	ofn.lpstrFilter = allFiles.charactersWithNullTermination();
	ofn.lpstrFile = fileBuf.data();
	ofn.nMaxFile = fileBuf.size();
	String dialogTitle = uploadFileText();
	ofn.lpstrTitle = dialogTitle.charactersWithNullTermination();
	ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	if (multiFile)
		ofn.Flags = ofn.Flags | OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&ofn)) {
		WCHAR* files = fileBuf.data();
		Vector<String> fileList;
		String file(files);
		if (multiFile) {
			while (!file.isEmpty()) {
				// When using the OFN_EXPLORER flag, the file list is null delimited.
				// When you create a String from a ptr to this list, it will use strlen to look for the null character.
				// Then we find the next file path string by using the length of the string we just created.
				WCHAR* nextFilePtr = files + file.length() + 1;
				String nextFile(nextFilePtr);
				// If multiple files are selected, there will be a directory name first, which we don't want to add to the vector.
				// We know a single file was selected if there is only one filename in the list.  
				// In that case, we don't want to skip adding the first (and only) name.
				if (files != fileBuf.data() || nextFile.isEmpty())
					fileList.append(file);
				files = nextFilePtr;
				file = nextFile;
			}
		} else
			fileList.append(file);
		ASSERT(fileList.size());
		fileChooser->chooseFiles(fileList);
	}
	// FIXME: Show some sort of error if too many files are selected and the buffer is too small.  For now, this will fail silently.
}

PlatformPageClient ChromeClientWinCE::platformPageClient() const
{
  return m_webView->windowHandle();
}

void ChromeClientWinCE::contentsSizeChanged(Frame*, const IntSize&) const
{
    notImplemented();
}

void ChromeClientWinCE::scrollRectIntoView(const IntRect&) const
{
    notImplemented();
}

void ChromeClientWinCE::scrollbarsModeDidChange() const
{
    notImplemented();
}

void ChromeClientWinCE::mouseDidMoveOverElement(const HitTestResult&, unsigned)
{
    notImplemented();
}

void ChromeClientWinCE::setToolTip(const String&  toolTip, TextDirection)
{
	m_webView->setToolTip(toolTip);
   // notImplemented();
}

void ChromeClientWinCE::print(Frame*)
{
    notImplemented();
}
void ChromeClientWinCE::reachedMaxAppCacheSize(int64_t)
{
    notImplemented();
}

void ChromeClientWinCE::reachedApplicationCacheOriginQuota(SecurityOrigin*, int64_t)
{
    notImplemented();
}

#if ENABLE(TOUCH_EVENTS)
void ChromeClientWinCE::needTouchEvents(bool)
{
    notImplemented();
}
#endif

#if USE(ACCELERATED_COMPOSITING)
void ChromeClientWinCE::attachRootGraphicsLayer(Frame*, GraphicsLayer*)
{
    notImplemented();
}

void ChromeClientWinCE::setNeedsOneShotDrawingSynchronization()
{
    notImplemented();
}

void ChromeClientWinCE::scheduleCompositingLayerSync()
{
    notImplemented();
}
#endif

//void ChromeClientWinCE::runOpenPanel(Frame*, PassRefPtr<FileChooser> prpFileChooser)
//{
//    notImplemented();
//}

void ChromeClientWinCE::loadIconForFiles(const Vector<String>& filenames, FileIconLoader* loader)
{
    loader->notifyFinished(Icon::createIconForFiles(filenames));
}

void ChromeClientWinCE::setCursor(const Cursor& cursor)
{
    HCURSOR platformCursor = cursor.platformCursor()->nativeCursor();
    if (!platformCursor)
        return;

      ::SetCursor(platformCursor);

    setLastSetCursorToCurrentCursor();
}

void ChromeClientWinCE::setCursorHiddenUntilMouseMoves(bool)
{
    notImplemented();
}

void ChromeClientWinCE::setLastSetCursorToCurrentCursor()
{
	m_webView->setLastCursor(::GetCursor());
}

void ChromeClientWinCE::formStateDidChange(const Node*)
{
    notImplemented();
}

void ChromeClientWinCE::requestGeolocationPermissionForFrame(Frame*, Geolocation*)
{
    notImplemented();
}

void ChromeClientWinCE::cancelGeolocationPermissionRequestForFrame(Frame*, Geolocation*)
{
    notImplemented();
}

bool ChromeClientWinCE::selectItemWritingDirectionIsNatural()
{
    return false;
}

bool ChromeClientWinCE::selectItemAlignmentFollowsMenuWritingDirection()
{
    return false;
}

bool ChromeClientWinCE::hasOpenedPopup() const
{
    notImplemented();
    return false;
}

PassRefPtr<PopupMenu> ChromeClientWinCE::createPopupMenu(PopupMenuClient* client) const
{
    return adoptRef(new PopupMenuWin(client));
}

PassRefPtr<SearchPopupMenu> ChromeClientWinCE::createSearchPopupMenu(PopupMenuClient* client) const
{
    return adoptRef(new SearchPopupMenuWin(client));
}

} // namespace WebKit
