/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

#include "config.h"
#include "BrowserExtensionSymbian.h"

//#include "BlockExceptions.h"
#include "FloatRect.h"
#include "FrameSymbian.h"
#include "FrameTree.h"
#include "Page.h"
#include "Screen.h"
#include "WebCoreFrameBridge.h"
#include "WebCorePageBridge.h"
#include "WebCoreWidget.h"

namespace WebCore {

BrowserExtensionSymbian::BrowserExtensionSymbian(Frame *frame)
    : m_frame(Symbian(frame))
    {
    }

void BrowserExtensionSymbian::createNewWindow(const ResourceRequest& request)
    {
    createNewWindow(request, WindowArgs(), NULL);
    }

void BrowserExtensionSymbian::createNewWindow(
    const ResourceRequest& request,
    const WindowArgs& winArgs,
    Frame*& part)
    {
    createNewWindow(request, winArgs, &part);
    }

void BrowserExtensionSymbian::createNewWindow(const ResourceRequest& request,
                                          const WindowArgs& winArgs,
                                          Frame** partResult)
    {
    ASSERT(!winArgs.dialog || request.frameName.isEmpty());

    if (partResult)
        *partResult = NULL;

    const KURL& url = request.url();


    const String* frameName = request.frameName.isEmpty() ? NULL : &(request.frameName);
    if (frameName) {
        // FIXME: Can't we just use m_frame->findFrame?
        if (CWebCoreFrameBridge* frameBridge = m_frame->bridge()->FindFrameNamed(frameName->des()) )
            {
            if (!url.isEmpty())
                {
                String argsReferrer = request.referrer();
                String referrer;
                if (!argsReferrer.isEmpty())
                    referrer = argsReferrer;
                else
                    referrer = frameBridge->Referrer();
                frameBridge->Callback().LoadURL( url.des(), referrer.des(), request.reload, ETrue, request.frameName, EEventNull );
                }

            frameBridge->Callback().ActivateWindow();

            if (partResult)
                *partResult = frameBridge->impl();

            return;
        }
    }

    CWebCorePageBridge *pageBridge;
    if (winArgs.dialog)
        pageBridge = m_frame->bridge()->Callback().createModalDialogWithURL( url.des() );
    else
        pageBridge = m_frame->bridge()->Callback().createWindowWithURL( url.des(), request.frameName, ETrue );
    if (!pageBridge)
        return;

    CWebCoreFrameBridge *frameBridge = pageBridge->mainFrame();
    if ( frameBridge->impl() )
        frameBridge->impl()->tree()->setName(AtomicString(request.frameName));

    if (partResult)
        *partResult = frameBridge->impl();

    frameBridge->Callback().setToolbarsVisible( winArgs.toolBarVisible || winArgs.locationBarVisible );
    frameBridge->Callback().setStatusbarVisible( winArgs.statusBarVisible );
    frameBridge->Callback().setScrollbarsVisible( winArgs.scrollbarsVisible );
    frameBridge->Callback().setWindowIsResizable( winArgs.resizable );

    TRect windowRect = pageBridge->impl()->windowRect().rect();
    if (winArgs.xSet)
      windowRect.iTl.iX = winArgs.x;
    if (winArgs.ySet)
      windowRect.iTl.iY = winArgs.y;

    // 'width' and 'height' specify the dimensions of the WebView, but we can only resize the window,
    // so we compute a WebView delta and apply it to the window.
    TRect webViewRect = pageBridge->callback().outerView()->rect();
    if (winArgs.widthSet)
      windowRect.SetWidth( windowRect.Width() + winArgs.width - webViewRect.Width() );
    if (winArgs.heightSet)
      windowRect.SetHeight( windowRect.Height() + winArgs.height - webViewRect.Height() );

    pageBridge->impl()->setWindowRect(windowRect);

    frameBridge->Callback().showWindow();
    }

void BrowserExtensionSymbian::setIconURL(const KURL &url)
    {
    m_frame->bridge()->Callback().SetIconURL( url.des() );
    }

void BrowserExtensionSymbian::setTypedIconURL(const KURL &url, const String &type)
    {
    m_frame->bridge()->Callback().SetIconURL( url.des(), type );
    }

int BrowserExtensionSymbian::getHistoryLength()
    {
    return m_frame->bridge()->Callback().HistoryLength();
    }

void BrowserExtensionSymbian::goBackOrForward(int distance)
    {
    m_frame->bridge()->Callback().GoBackOrForward( distance );
    }

KURL BrowserExtensionSymbian::historyURL(int distance)
    {
    return KURL( m_frame->bridge()->Callback().HistoryURL( distance ) );
    }

}
