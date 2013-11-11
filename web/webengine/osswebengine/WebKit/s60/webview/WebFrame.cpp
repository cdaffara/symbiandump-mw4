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
* Description:   Frame in webkit side
*
*/


#include "config.h"
#include "../../bidi.h"
#include "WebFrame.h"
#include "WebCoreWidget.h"
#include "WebCoreFrameBridge.h"
#include "WebFrameBridge.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "WebChromeClient.h"
#include "WebFrameLoaderClient.h"

#include "Page.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameTree.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "SelectionController.h"
#include "SubstituteData.h"
#include "RenderWidget.h"

#include "HTMLNames.h"
#include "GraphicsContext.h"
#include "HTMLFormElement.h"
#include "ResourceRequest.h"
#include "WebKitLogger.h"
#include "WebUtil.h"
#include "PluginSkin.h"
#include "PluginHandler.h"
#include "kjs_proxy.h"
#if PLATFORM(SYMBIAN)
#include "oom.h"
#endif

using namespace WebCore;
using namespace HTMLNames;

WebFrame::WebFrame() :
    m_view(0)
   ,m_bridge(0)
{
}

WebFrame::~WebFrame() 
{
    if (m_view)
        m_view->deref();
}

void WebFrame::initWithWebFrameView(WebFrameView* view, WebView* topView, WebFrameBridge* bridge)
{
    if (m_view)
        m_view->deref();
    m_view = view;
    if (m_view) {
        m_view->ref();
        m_view->setWebFrame(this);
        m_view->setTopView(topView);
    }
    setBridge(bridge);
}

void WebFrame::loadRequest(const WebCore::ResourceRequest& request,const WebCore::String* windowType)
{
    FrameLoader* fl = frameLoader();
    if (fl) {
        if(windowType)
            fl->load(request,*windowType);
        else
            fl->load(request);
    }
        
 }

void WebFrame::loadData(const WebCore::ResourceRequest& request, WebCore::SubstituteData& substituteData)
{
    if (FrameLoader* fl = frameLoader())
        fl->load(request, substituteData);
}

void WebFrame::loadURL(const TDesC8& url, TInt cachemode, const String& referrer,const WebCore::String* windowType) 
{
#if PLATFORM(SYMBIAN)
    OOM_PRE_CHECK(1024*1024, 0, "WebFrame::loadURL")
#endif
    ResourceRequestCachePolicy cachePolicy;
    switch (cachemode) 
    {
    default:
    case TBrCtlDefs::ECacheModeNormal: 
        cachePolicy = UseProtocolCachePolicy;
        break;
    case TBrCtlDefs::ECacheModeNoCache:
        cachePolicy = ReloadIgnoringCacheData;
        break;
    case TBrCtlDefs::ECacheModeHistory:
        cachePolicy = ReturnCacheDataElseLoad;
        break;
    case TBrCtlDefs::ECacheModeOnlyCache:
        cachePolicy = ReturnCacheDataDontLoad;
        break;
    }
    ResourceRequest request = (KURL(url));
    request.setMainLoad(true);
    request.setCachePolicy(cachePolicy);
    if(!referrer.isNull())
        request.setHTTPReferrer(referrer);
    request.setHTTPMethod("GET");
    loadRequest(request,windowType);
#if PLATFORM(SYMBIAN)
    OOM_POST_CHECK_FAILED(return;)
#endif    
}

// Called by the bridge to load into a sub-frmae
void WebFrame::loadURL(const TPtrC8 url, const TPtrC referrer, WebFrame* childFrame)
{
    /* fixme tot: do we need to hook up with bf list?
    HistoryItem* parentItem = core(this)->loader()->currentHistoryItem();
    FrameLoadType loadType = _frameLoader()->loadType();
    FrameLoadType childLoadType = FrameLoadTypeRedirectWithLockedHistory;

    if (parentItem && parentItem->children().size() && 
        (isBackForwardLoadType(loadType) 
        || loadType == FrameLoadTypeReload 
        || loadType == FrameLoadTypeReloadAllowingStaleData)) 
    {
        HistoryItem* childItem = parentItem->childItemWithName(childFrame->name());
        if (childItem) {
            childLoadType = loadType;

            if (isBackForwardLoadType(loadType))
                core(childFrame)->loader()->setProvisionalHistoryItem(childItem);
            else
                core(childFrame)->loader()->setCurrentHistoryItem(childItem);
        }
    }*/

    // tot fixme: saved pages not implemented.
    FrameLoadType childLoadType = frameLoader()->loadType();
    childFrame->frameLoader()->load(url, referrer, childLoadType, String(), 0, 0);
}

void WebFrame::paintRect(WebCoreGraphicsContext& gc, const TRect& rect)
{
    WebCore::GraphicsContext ctx(&gc);
    core(this)->paint(&ctx, enclosingIntRect(rect));
}

FrameLoader* WebFrame::frameLoader()
{
    Frame* frame = core(this);
    return frame ? frame->loader() : 0;
}

bool WebFrame::isMainFrame() const
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;
    return coreFrame == coreFrame->page()->mainFrame();
}

// selection
bool WebFrame::hasSelection()
{
    if (Frame* coreFrame = core(this)) {
        return !coreFrame->selectionController()->isNone();
    }
    return false;
}

void WebFrame::clearSelection()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return;
    coreFrame->selectionController()->clear();
}

WebFrame* WebFrame::findFrameWithSelection()
{
    Frame* coreFrame = core(this);
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame))
        if (kit(frame)->hasSelection())
            return kit(frame);
    
    return 0;
}

void WebFrame::stopLoading()
{
    if (FrameLoader* fl = frameLoader())
        fl->stopForUserCancel();
}

void WebFrame::reload()
{
    frameLoader()->reload();
}

WebFrame* WebFrame::findFrameNamed(const TPtr& name)
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return 0;
    return kit(coreFrame->tree()->find(name));
}

WebFrame* WebFrame::parentFrame()
{
    Frame* coreFrame = core(this);
    if (!coreFrame) 
        return 0;
    return kit(coreFrame->tree()->parent());
}

bool WebFrame::isIframe() const
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;
    Frame* pf = coreFrame->tree()->parent();
    if (pf)
        return !pf->isFrameSet();
    return false;
}

bool WebFrame::isFrameSet() const
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return false;
    return coreFrame->isFrameSet();
}

WTF::Vector<WebFrame*> WebFrame::childFrames()
{
    Frame* coreFrame = core(this);
    if (!coreFrame)
        return WTF::Vector<WebFrame*>();

    WTF::Vector<WebFrame*> children;
    children.reserveCapacity(coreFrame->tree()->childCount());
    for (Frame* child = coreFrame->tree()->firstChild(); child; child = child->tree()->nextSibling())
        children.append(kit(child));
    return children;
}

void WebFrame::addChild(WebFrame* child)
{
    core(this)->tree()->appendChild(adoptRef(core(child)));
    if (child->documentLoader())
        child->documentLoader()->setOverrideEncoding(documentLoader()->overrideEncoding());

    // update the view heirachy
    m_view->addChild(child->frameView());
}

DocumentLoader* WebFrame::documentLoader()
{
    return frameLoader()->documentLoader();
}

void WebFrame::notifyPluginsOfScrolling()
{
    setpluginToScroll(true);
    Frame* coreFrame = core(this);
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {
        PassRefPtr<HTMLCollection> objects = frame->document()->objects();       
        for (Node* n = objects->firstItem(); n; n = objects->nextItem()) 
            notifyPluginOfScrolling(n->renderer());             

        PassRefPtr<HTMLCollection> embeds = frame->document()->embeds();       
        for (Node* n = embeds->firstItem(); n; n = embeds->nextItem()) 
            notifyPluginOfScrolling(n->renderer()); 

        }
    setpluginToScroll(false);
}

void WebFrame::notifyPluginOfScrolling(RenderObject* renderer)
{        
    MWebCoreObjectWidget* view = widget(renderer);
    //Don't repaint the plugin objects if Browser is in PageView mode
    if (view) {
        view->positionChanged();
        TRect r = m_view->toDocCoords(static_cast<PluginSkin*>(view)->getPluginWinRect());
        m_view->topView()->scheduleRepaint(r);
    }
}

PluginSkin* WebFrame::focusedPlugin()
{
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    PluginSkin* plugin = pluginHandler->activePlugin();
    if (!plugin) {
        //no focused plugin. If only one plugin is present return that plugin
        WTF::HashSet<PluginSkin*> pluginObjs = pluginHandler->pluginObjects();
        if (pluginObjs.size() == 1) {
            WTF::HashSet<PluginSkin*>::iterator it = pluginObjs.begin();
            plugin = *it;
        }
    }
    
    return plugin;
}

void WebFrame::setFrameView(WebFrameView* v) 
{
    if (m_view)
        m_view->deref();
    m_view = v;
    if (m_view)
        m_view->ref();
}

void WebFrame::setBridge(WebFrameBridge* b)
{
    if (m_bridge) 
        m_bridge->deref();
    m_bridge = b;
    if (m_bridge) 
        m_bridge->ref();
}

WebFrame* WebFrame::frameAtPoint(const TPoint& pt_)
{
    WTF::Vector<WebFrame*> ch = childFrames();
    WebFrame* frm = 0;
    // Check the children of the frame only if this frame also contains pt_
    // If a child iframe is bigger than the parent, it should not be picked.    
    if (m_view->isVisible() && m_view->rectInGlobalCoords().Contains(pt_)) {
        Vector<WebFrame*>::iterator end = ch.end();
        for (Vector<WebFrame*>::iterator itr = ch.begin(); itr != end; itr++) {
            WebFrame* f = (*itr);
            if( (f = f->frameAtPoint(pt_ + m_view->toViewCoords(m_view->contentPos()))) != 0 )
                {
                // TODO: this only fixes www.aol.com login iframe issue,
                // a better solution is to check the z-index for each frame.
                // Anyway, overlapping iframes are not common.
                frm = f;
                }
        }

        if (frm)
            return frm;


        return this;
    }
    return 0;
}

int WebFrame::imageCount(bool visibleOnly_) 
{   
    int imageCount = imageCountInFrame(*this, visibleOnly_);
    //
    WTF::Vector<WebFrame*> ch = childFrames();
    WebFrame* frm = 0;
    
    Vector<WebFrame*>::iterator end = ch.end();
    for (Vector<WebFrame*>::iterator itr = ch.begin(); itr != end; itr++)
        imageCount+=(*itr)->imageCount(visibleOnly_);
        
    return imageCount;
}

CArrayFixFlat<TBrCtlImageCarrier>* WebFrame::imageData(bool visibleOnly_) 
{   
    CArrayFixFlat<TBrCtlImageCarrier>* imageList = imagesInFrame(*this, visibleOnly_);
    //
    WTF::Vector<WebFrame*> ch = childFrames();
    WebFrame* frm = 0;
    
    Vector<WebFrame*>::iterator end = ch.end();
    for (Vector<WebFrame*>::iterator itr = ch.begin(); itr != end; itr++) {
        CArrayFixFlat<TBrCtlImageCarrier>* childImageList = (*itr)->imageData(visibleOnly_);
        for (int i = 0; i < childImageList->Count(); i++)
            imageList->AppendL(childImageList->At(i));
        childImageList->Reset();
        delete childImageList;
    }
       
    return imageList;
}

CArrayFixFlat<TBrCtlSubscribeTo>* WebFrame::findSubscribeTo() 
{   
    CArrayFixFlat<TBrCtlSubscribeTo>* linkList = findSubscribeToInFrame(*this);
    //
    WTF::Vector<WebFrame*> ch = childFrames();
    WebFrame* frm = 0;
    
    Vector<WebFrame*>::iterator end = ch.end();
    for (Vector<WebFrame*>::iterator itr = ch.begin(); itr != end; itr++) {
        CArrayFixFlat<TBrCtlSubscribeTo>* childLinkList = (*itr)->findSubscribeTo();
        for (int i = 0; i < childLinkList->Count(); i++)
            linkList->AppendL(childLinkList->At(i));
        childLinkList->Reset();
        delete childLinkList;
    }
    return linkList;
}

DOMDocument* WebFrame::DOMDocument()
{
    return 0;
}

void WebFrame::scalingFactorChanged(int factor)
{
    bridge()->scalingFactorChanged(factor);
}

// helpers
Frame* core(const WebFrame* frame)
{
    if (!frame)
        return NULL;
    
    if (!frame->bridge())
        return NULL;

    return frame->bridge()->frame();
}

WebFrame* kit(Frame* frame)
{
    return frame ? ((WebFrameBridge *)frame->bridge())->webFrame(): NULL;
}

WebView *kit(Page* page)
{
    return page ? static_cast<WebChromeClient*>(page->chrome()->client())->webView() : 0;
}

WebFrame* mainFrame(WebFrame* frame) 
{
    if (!frame)
        return 0;
    if (frame->isMainFrame())
        return frame;
    
    Frame* coreFrame = core(frame);
    if (!coreFrame)
        return 0;
    return kit(coreFrame->page()->mainFrame());
}

CBrCtl* control(WebCore::Frame* frame) 
{
    return control(kit(frame));
}

CBrCtl* control(const WebFrame* webFrame)
{
    if (webFrame && webFrame->frameView())
        return webFrame->frameView()->topView()->brCtl();
    return NULL;
}

MWebCoreObjectWidget* widget(RenderObject* render_) 
{   
    if (!render_)
        return NULL;
        
    if (render_->isWidget()) {
        Widget* widget = static_cast<RenderWidget*>(render_)->widget();
        if (widget) {
            MWebCoreWidget* view = widget->getView();    
            if(view && view->isObjectView())
                return static_cast<MWebCoreObjectWidget*>(view);
        }
    }
    return NULL;    
}


MWebCoreObjectWidget* widget(Node* node_) 
{   
    if (!node_)
        return NULL;
        
    return widget(node_->renderer());
}

bool WebFrame::executeScript(const WebCore::String& script)
{
    bool result = false;
    KJSProxy *proxy = core(this)->scriptProxy();
    if (proxy) {
        KJS::JSValue *v = proxy->evaluate(String(), 0, script);
        if (v)
            result = true;
    }
    return result;
}

void WebFrame::makeVisiblePlugins(TBool visible)
{
    MWebCoreObjectWidget* view = NULL;
    int pluginCount = 0;
    Frame* coreFrame = core(this);
    PluginSkin* ptr = 0;
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {

        PassRefPtr<HTMLCollection> objects = frame->document()->objects();     
        for (Node* n = objects->firstItem(); n; n = objects->nextItem()) {
            view = widget(n); 
            if (view) {
                ptr = static_cast<PluginSkin*>(view);
                ptr->makeVisible(visible);
            }
        }
        PassRefPtr<HTMLCollection> embeds = frame->document()->embeds();       
        for (Node* n = embeds->firstItem(); n; n = embeds->nextItem()) {
            view = widget(n);
            if (view) {
                ptr = static_cast<PluginSkin*>(view);
                ptr->makeVisible(visible);
            }
        }
    }
}


inline int xInRect(const IntRect& r, int x)
{
	return std::min(std::max(x, r.location().x()), r.location().x() + r.width());
}

inline int yInRect(const IntRect& r, int y)
{
	return std::min(std::max(y, r.location().y()), r.location().y() + r.height());
}

Node* WebFrame::getClosestAnchorElement(const TPoint& viewPt, TPoint& newPos)
{
    IntPoint pt = m_view->viewCoordsInFrameCoords(viewPt);
    
    Frame* coreFrame = core(this);

	int dist = 99999999;
	Node* result = 0;
	//for (Node* n=links->firstItem(); n; n=links->nextItem()) {
	for(Node* n = coreFrame->document(); n != 0; n = n->traverseNextNode()) {
        if(n->isFocusable() || n->hasTagName(areaTag)) {
			IntRect r = n->getRect();
			if (r.contains(pt)) {
				dist = 0;
				result = n;
				break;
			}
			
			int x = xInRect(r, pt.x());
			int y = yInRect(r, pt.y());
			int d = (pt.x() - x) * (pt.x() - x) + (pt.y() - y) * (pt.y() - y);
			if (dist > d) {
				dist = d;
				result = n;
			}
        }
	}
	
    // check if we are close enough and calcualte with zoom factor. 
    if (dist< (400/m_view->topView()->scalingFactor() * 100)) {
        IntRect r = result->getRect();
        r.inflate(-2);
        TPoint docPos(xInRect(r, pt.x()), yInRect(r, pt.y()));
        newPos = m_view->frameCoordsInViewCoords(docPos);
        return result;        
    }
    
    return 0;
}

// END OF FILE
