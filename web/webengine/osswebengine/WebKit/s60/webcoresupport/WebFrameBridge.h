/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   extension of WebCore::WebCoreFrameBridge
*
*/

#ifndef __WEBFRAMEBRIDGE_H__
#define __WEBFRAMEBRIDGE_H__

#include "WebCoreFrameBridge.h"
#include "RefPtr.h"

namespace WebCore {
    class Page;
    class String;
    class HTMLFrameOwnerElement;
    class Element;
}

class WebFrame;
class WebView;
class WebFrameView;
class TPtrC8;


class WebFrameBridge : public WebCoreFrameBridge
{
public:

    WebFrameBridge();
    virtual ~WebFrameBridge();
    void initMainFrameWithPage(WebCore::Page* page, const WebCore::String& frameName, WebFrameView* frameView);
    void initSubframeWithOwnerElement(WebCore::HTMLFrameOwnerElement* ownerElement, const WebCore::String& frameName, WebFrameView* frameView);
    void ref() { ++m_refCount; }
    void deref() { if( --m_refCount == 0 ) delete this; }

    void close();

    WebView* webView() const;
    WebFrame* webFrame() const;
    WebCoreFrameBridge* mainFrame() const;
    MWebCoreWidget* viewForPluginWithURL( const TPtrC8& url, const Vector<WebCore::String>& paramNames, const Vector<WebCore::String>& paramValues,
            const TPtrC8& baseUrl, const WebCore::String& mimeType, TBool loadContent, WebCore::Element* pluginElement );
            
    // from WebCoreFrameBridge
    MScrollView* documentView() const;
    WebCore::Frame* createChildFrameNamed(const WebCore::String* frameName, const TPtrC8 withURL, const TPtrC referrer,
        WebCore::HTMLFrameOwnerElement* ownerElement, bool allowsScrolling, int marginWidth, int marginHeight);
    ObjectElementType determineObjectFromMIMEType(const WebCore::String& MIMEType, const TPtrC8 URL);
    void notifyMetaData(WebCore::String& name, WebCore::String& value);
    unsigned getHistoryLength();
    void scheduleHistoryNavigation(int distance);
  
private:

    WebFrameBridge(const WebFrameBridge&);                 // not implemented
    WebFrameBridge& operator=(const WebFrameBridge&);      // not implemented

    void finishInitializingWithPage(WebCore::Page* page, const WebCore::String& frameName, WebFrameView* frameView, WebCore::HTMLFrameOwnerElement* ownerElement);

    RefPtr<WebFrame>           m_webFrame;                    // frame
    int                        m_refCount;
};

#endif // __WEBFRAMEBRIDGE_H__

// END OF FILE



