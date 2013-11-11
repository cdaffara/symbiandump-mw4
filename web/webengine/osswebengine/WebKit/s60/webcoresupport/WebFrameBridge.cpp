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
* Description:  Implemetation of WebFrameBridge
*
*/

#include "config.h"
#include "../../bidi.h"
#include "Document.h"
#include "WebFrameBridge.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "WebFrameLoaderClient.h"
#include "FrameTree.h"
#include "HTMLFrameOwnerElement.h"
#include "Page.h"
#include "WebKitLogger.h"
#include "PluginSkin.h"
#include "BrCtl.h"
#include "HistoryInterface.h"
#include "Element.h"

using namespace WebCore;

WebFrameBridge::WebFrameBridge() :
    m_refCount(1)
{
}

WebFrameBridge::~WebFrameBridge() 
{
    m_webFrame.release();
    if (m_frame)
        m_frame->setBridge(NULL);

}

WebView* WebFrameBridge::webView() const
{
    if (!m_frame)
        return NULL;
    
    return kit(m_frame->page());
}

WebFrame* WebFrameBridge::webFrame() const
{
    return m_webFrame.get();
}

WebCoreFrameBridge* WebFrameBridge::mainFrame() const
{
    ASSERT(m_webFrame != NULL);
    return webView()->mainFrame()->bridge();
}

MScrollView* WebFrameBridge::documentView() const
{
    ASSERT(m_webFrame != NULL);
    return m_webFrame->frameView();
}

void WebFrameBridge::finishInitializingWithPage(Page* page, const String& frameName, 
    WebFrameView* frameView, HTMLFrameOwnerElement* ownerElement) 
{
    WebView* webView = kit(page);

    m_webFrame = new WebFrame();
    m_webFrame->initWithWebFrameView(frameView, webView, this);

    m_frame = new Frame(page, ownerElement, new WebFrameLoaderClient(m_webFrame.get()));
    m_frame->setBridge(this);
    m_frame->tree()->setName(frameName);
    m_frame->init();
}

void WebFrameBridge::initMainFrameWithPage(Page* page, const String& frameName, WebFrameView* frameView)
{
    init();
    finishInitializingWithPage(page, frameName, frameView, NULL);
}

void WebFrameBridge::initSubframeWithOwnerElement(HTMLFrameOwnerElement* ownerElement, const String& frameName, WebFrameView* frameView)
{
    init();
    finishInitializingWithPage(ownerElement->document()->frame()->page(), frameName, frameView, ownerElement);
}

void WebFrameBridge::close()
{
    WebCoreFrameBridge::close();
    m_webFrame.release();
    m_webFrame = NULL;
}

Frame* WebFrameBridge::createChildFrameNamed(const String* frameName, const TPtrC8 withURL, const TPtrC referrer,
    HTMLFrameOwnerElement* ownerElement, bool allowsScrolling, int marginWidth, int marginHeight)        
{
    ASSERT(m_webFrame);
    
    WebFrameView* childView = new WebFrameView();
    childView->initWithFrame(TRect(0,0,0,0));
    childView->setAllowsScrolling(allowsScrolling);
    childView->setMarginWidth(marginWidth);
    childView->setMarginHeight(marginHeight);

    WebFrameBridge* newBridge = new WebFrameBridge();
    newBridge->initSubframeWithOwnerElement(ownerElement, *frameName, childView);
    childView->deref();
    
    m_webFrame->addChild(newBridge->webFrame());
    newBridge->deref();

    RefPtr<Frame> newFrame = newBridge->frame();

#if USE(LOW_BANDWIDTH_DISPLAY)
    newFrame->loader()->setUseLowBandwidthDisplay(webView()->widgetExtension() != NULL);
#endif
    
    m_webFrame->loadURL(withURL, referrer, kit(newFrame.get()));

    // The frame's onload handler may have removed it from the document.
    if (!newFrame->tree()->parent())
        return NULL;

    return newFrame.get();
}

ObjectElementType WebFrameBridge::determineObjectFromMIMEType(const String& MIMEType, const TPtrC8 URL)
{
    if (MIMEType.length() == 0) {
        // Try to guess the MIME type based off the extension.
        int index = MIMEType.des().LocateReverse('.');
        if (index != KErrNotFound) { 
            TPtrC extPtr = MIMEType.des().Mid(++index);
            if (extPtr.Length() > 0 && webView()->pluginForExtension(String(extPtr)))
                    // If no MIME type is specified, use a plug-in if we have one that can handle the extension.
                    return ObjectElementPlugin;
        }
    }

    if (MIMEType.length() == 0)
        return ObjectElementFrame; // Go ahead and hope that we can display the content.

    //if (MimeTypeRegistry::isSupportedImageMIMEType(MIMEType))
    //    return ObjectElementImage;

    if (webView()->isMIMETypeRegisteredAsPlugin(MIMEType))
        return ObjectElementPlugin;

    if (WebFrameView::viewClassForMIMEType(MIMEType))
        return ObjectElementFrame;
    
    return ObjectElementNone;
}

void WebFrameBridge::notifyMetaData(String& name, String& value)
{
    webView()->notifyMetaData(name, value);
}

unsigned WebFrameBridge::getHistoryLength()
{
     return (unsigned)webView()->brCtl()->historyHandler()->historyController()->historyLength();
}

void WebFrameBridge::scheduleHistoryNavigation(int distance)
{
    webView()->brCtl()->historyHandler()->historyController()->goBackOrForward(distance);
}

MWebCoreWidget* WebFrameBridge::viewForPluginWithURL( const TPtrC8& url, const Vector<String>& paramNames, const Vector<String>& paramValues,
            const TPtrC8& baseUrl, const String& mimeType, TBool loadContent, Element* pluginElement )
{
    /*tot:fixme TBool hideReferrer;
    if (!CanLoadUrl(aUrl,iWebCoreBridge->Referrer(),hideReferrer))
        {
        return 0;
        }*/
    PluginSkin* pluginSkin = NULL;
    //tot: need to fix url convert
    TRAP_IGNORE( pluginSkin = PluginSkin::NewL( *m_webFrame,
        url, paramNames, paramValues, baseUrl, mimeType, loadContent ) );
    pluginSkin->setElement(pluginElement);    
    return pluginSkin; 
}
            
// end of file
