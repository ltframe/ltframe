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
#include "DragClientWinCE.h"
#include "WebDropSource.h"
#include <shlobj.h>
#include "WebView.h"
#include <WebCore/ClipboardWin.h>
#include <WebCore/DragController.h>
#include <WebCore/DragData.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameView.h>
#include <WebCore/GraphicsContext.h>
#include <WebCore/Page.h>

#include "NotImplemented.h"

using namespace WebCore;

static DWORD draggingSourceOperationMaskToDragCursors(DragOperation op)
{
    DWORD result = DROPEFFECT_NONE;
    if (op == DragOperationEvery)
        return DROPEFFECT_COPY | DROPEFFECT_LINK | DROPEFFECT_MOVE; 
    if (op & DragOperationCopy)
        result |= DROPEFFECT_COPY; 
    if (op & DragOperationLink)
        result |= DROPEFFECT_LINK; 
    if (op & DragOperationMove)
        result |= DROPEFFECT_MOVE;
    if (op & DragOperationGeneric)
        result |= DROPEFFECT_MOVE;
    return result;
}




namespace WebKit {


DragClientWinCE::DragClientWinCE(WebView* webView)
    : m_webView(webView) 
{
    ASSERT(webView);
}


void DragClientWinCE::willPerformDragDestinationAction(DragDestinationAction, DragData*)
{
    notImplemented();
}

void DragClientWinCE::willPerformDragSourceAction(DragSourceAction, const IntPoint&, Clipboard*)
{
    notImplemented();
}

DragDestinationAction DragClientWinCE::actionMaskForDrag(DragData*)
{
    notImplemented();
    return DragDestinationActionAny;
}

DragSourceAction DragClientWinCE::dragSourceActionMaskForPoint(const IntPoint&)
{
    notImplemented();
    return DragSourceActionAny;
}

void DragClientWinCE::startDrag(DragImageRef image, const IntPoint& imageOrigin, const IntPoint& dragPoint, Clipboard* clipboard, Frame* frame, bool isLink)
{
	//return;
	COMPtr<IDragSourceHelper> helper;
	COMPtr<IDataObject> dataObject;
	COMPtr<IDropSource> source;
	if (FAILED(WebDropSource::createInstance(m_webView, &source)))
		return;


	dataObject = static_cast<ClipboardWin*>(clipboard)->dataObject();
	if (source && (image || dataObject)) {
		if (image) {
			if(SUCCEEDED(CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER,
				IID_IDragSourceHelper,(LPVOID*)&helper))) {
					BITMAP b;
					GetObject(image, sizeof(BITMAP), &b);
					SHDRAGIMAGE sdi;
					sdi.sizeDragImage.cx = b.bmWidth;
					sdi.sizeDragImage.cy = b.bmHeight;
					sdi.crColorKey = 0xffffffff;
					sdi.hbmpDragImage = image;
					sdi.ptOffset.x = dragPoint.x() - imageOrigin.x();
					sdi.ptOffset.y = dragPoint.y() - imageOrigin.y();
					if (isLink)
						sdi.ptOffset.y = b.bmHeight - sdi.ptOffset.y;

					helper->InitializeFromBitmap(&sdi, dataObject.get());
			}
		}

		DWORD okEffect = draggingSourceOperationMaskToDragCursors(m_webView->page()->dragController()->sourceDragOperation());
		DWORD effect = DROPEFFECT_MOVE;
		/*
		COMPtr<IWebUIDelegate> ui;
		
		if (m_webView->uiDelegate(&ui))) {
			COMPtr<IWebUIDelegatePrivate> uiPrivate;
			if (SUCCEEDED(ui->QueryInterface(IID_IWebUIDelegatePrivate, (void**)&uiPrivate)))
				hr = uiPrivate->doDragDrop(m_webView, dataObject.get(), source.get(), okEffect, &effect);
		}*/
		HRESULT hr = E_NOTIMPL;
		if (hr == E_NOTIMPL)
			hr = DoDragDrop(dataObject.get(), source.get(), okEffect, &effect);

		DragOperation operation = DragOperationNone;
		if (hr == DRAGDROP_S_DROP) {
			if (effect & DROPEFFECT_COPY)
				operation = DragOperationCopy;
			else if (effect & DROPEFFECT_LINK)
				operation = DragOperationLink;
			else if (effect & DROPEFFECT_MOVE)
				operation = DragOperationMove;
		}
		frame->eventHandler()->dragSourceEndedAt(generateMouseEvent(m_webView, false), operation);
		
	}
}

void DragClientWinCE::dragControllerDestroyed()
{
    delete this;
}

} // namespace WebKit
