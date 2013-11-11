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
* Description:   Implemetation of CWebFrame
*
*/


#ifndef __WEBFRAME_H__
#define __WEBFRAME_H__

#include <e32base.h>
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>
#include "Shared.h"

namespace WebCore
{
    class Frame;
    class FrameLoader;
    class DocumentLoader;
    class DOMDocument;
    class Page;
    struct ResourceRequest;
    class SubstituteData;
    class RenderObject;
    class RenderObject;
    class Node;
    class String;
}

class WebView;
class WebFrameView;
class WebFrameLoaderClient;
class WebFrameBridge;
class WebCoreFrameBridge;
class WebCoreGraphicsContext;
class TBrCtlImageCarrier;
class TBrCtlSubscribeTo;
class CBrCtl;
class PluginSkin;
class MWebCoreObjectWidget;

class WebFrame : public WebCore::Shared<WebFrame>
{
public:

    WebFrame();
    virtual ~WebFrame();

    void initWithWebFrameView(WebFrameView* view, WebView* topView, WebFrameBridge* bridge);
    void loadURL(const TPtrC8 url, const TPtrC referrer, WebFrame* child);
    void loadRequest(const WebCore::ResourceRequest& request,const WebCore::String* windowType = NULL);
    void loadURL(const TDesC8& url, TInt cachemode, const WebCore::String& referrer,const WebCore::String* windowType = NULL);
    void loadData(const WebCore::ResourceRequest& request, WebCore::SubstituteData& substituteData);

    // accessors
    WebFrameBridge* bridge() const { return m_bridge; }
    void setBridge(WebFrameBridge*);
    bool isMainFrame() const;
    WebFrameView* frameView() const {return m_view;}
    void setFrameView(WebFrameView*);

    // selection
    bool hasSelection();
    void clearSelection();
    WebFrame* findFrameWithSelection();

    WebCore::FrameLoader* frameLoader();

    void stopLoading();
    void reload();

    WebFrame* findFrameNamed(const TPtr& name);
    WebFrame* parentFrame();
    WTF::Vector<WebFrame*> childFrames();
    void addChild(WebFrame* child);
    WebCore::DocumentLoader* documentLoader();
    WebCore::DOMDocument* DOMDocument();

    bool isIframe() const;
    bool isFrameSet() const;

    void paintRect(WebCoreGraphicsContext& gc, const TRect& r);

    void notifyPluginsOfScrolling();
    void notifyPluginOfScrolling(WebCore::RenderObject* renderer);
    PluginSkin* focusedPlugin();
    WebFrame* frameAtPoint(const TPoint& pt_);
    
    // utility functions
    int imageCount(bool visibleOnly_);
    CArrayFixFlat<TBrCtlImageCarrier>* imageData(bool visibleOnly_);
    CArrayFixFlat<TBrCtlSubscribeTo>* findSubscribeTo();

    void scalingFactorChanged(int factor);
    void makeVisiblePlugins(TBool visible);
    bool executeScript(const WebCore::String& script);
    WebCore::Node* getClosestAnchorElement(const TPoint& pt, TPoint& newPos);
    
    void setpluginToScroll(bool pluginScroll){m_pluginToScroll=pluginScroll;}
    bool pluginToScroll(){return m_pluginToScroll;}
    
private:
    WebFrame(const WebFrame&);                    // not implemented
    WebFrame& operator=(const WebFrame&);         // not implemented    

    WebFrameView*                   m_view;              // the frame view
    RefPtr<WebFrameLoaderClient>    m_loaderClient;      // the loader client
    WebFrameBridge*                 m_bridge;        // the bridge
    bool                            m_pluginToScroll;
};

// utility functions
WebCore::Frame* core(const WebFrame*);
WebFrame* kit(WebCore::Frame*);
WebView* kit(WebCore::Page* page);
WebFrame* mainFrame(WebFrame*);
CBrCtl* control(WebCore::Frame*);
CBrCtl* control(const WebFrame*);
MWebCoreObjectWidget* widget(WebCore::Node*);
MWebCoreObjectWidget* widget(WebCore::RenderObject*);

#endif
// END OF FILE
