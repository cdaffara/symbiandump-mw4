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
* Description:  
*
*/

#include <../bidi.h>
#include "config.h"
#include "WebCoreFrameBridge.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "runtime_root.h"
#include "runtime.h"
#include "kjs_window.h"
#include "DOMWindow.h"
#include "Document.h"
#include "DocumentLoader.h"
#include "RenderPart.h"
#include "HTMLNames.h"
#include "EventNames.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "WebUtil.h"
#include "BrCtl.h"
#include "SettingsContainer.h"
#include "RenderView.h"
#include "Page.h"
#include "HTMLImageElement.h"
#include "RenderImage.h"

#include "StaticObjectsContainer.h"
#include "PlatformFontCache.h"

using namespace WebCore;
using namespace HTMLNames;
using namespace EventNames;

using KJS::Bindings::RootObject;
using KJS::ExecState;
using KJS::JSObject;
using KJS::Window;


const int KXHTMLDefaultFontMultiplier = 120;
const int KDefaultLowResFontMultiplier = 120;
const int KDefaultFontMultiplier = 140;


static PassRefPtr<RootObject> createRootObject(void* nativeHandle)
{
/*    NSView *view = (NSView *)nativeHandle;
    WebCoreFrameBridge *bridge = [[WebCoreViewFactory sharedFactory] bridgeForView:view];
    if (!bridge)
        return 0;
    Frame* frame = bridge->_frame;
    return frame->createRootObject(nativeHandle, frame->scriptProxy()->interpreter()); */
    return NULL;
}

static void updateRenderingForBindings(ExecState* exec, JSObject* rootObject)
{
    if (!rootObject)
        return;
        
    Window* window = static_cast<Window*>(rootObject);
    if (!window)
        return;
        
    if (Document* doc = window->impl()->frame()->document())
        doc->updateRendering();
}


static WebCoreFrameBridge* bridge(Frame* frame)
{
    if (!frame)
        return NULL;
    return frame->bridge();
}


WebCoreFrameBridge::WebCoreFrameBridge() :
    m_frame(0)

{
    m_focusableNodeList.clear();    
}

WebCoreFrameBridge::~WebCoreFrameBridge() 
{
}

void WebCoreFrameBridge::init()
{
    static bool initializedKJS = false;
    if (!initializedKJS) {
        initializedKJS = true;

        // s602 mac only?
        //RootObject::setCreateRootObject(createRootObject);
        KJS::Bindings::Instance::setDidExecuteFunction(updateRenderingForBindings);
    }
    
    m_shouldCreateRenderers = true;
}
void WebCoreFrameBridge::addJSExtension(const TDesC& id, KJS::JSObject* obj)
{
    KJS::Window* window = KJS::Window::retrieveWindow(m_frame);        
    if (window) {       
        ExecState* exec = window->interpreter()->globalExec();        
        if (exec && obj) {        
            obj->setPrototype(window->interpreter()->builtinObjectPrototype());
            window->put(exec, KJS::Identifier((const KJS::UChar *)id.Ptr(), id.Length()), obj, KJS::DontDelete);
        }        
    }            
}
WebCore::Frame* WebCoreFrameBridge::frame() 
{
    return m_frame;
}

void WebCoreFrameBridge::close()
{
    clearFrame();
    m_closed = true;
}

void WebCoreFrameBridge::addData(const char* data, int length)
{
    Document *doc = m_frame->document();
    
    // Document may be nil if the part is about to redirect
    // as a result of JS executing during load, i.e. one frame
    // changing another's location before the frame's document
    // has been created. 
    if (doc) {
        doc->setShouldCreateRenderers(shouldCreateRenderers());
        m_frame->loader()->addData(data, length);
    }
}

void WebCoreFrameBridge::receivedData(const char* data, int length, String textEncodingName)
{
    // Set the encoding. This only needs to be done once, but it's harmless to do it again later.
    String encoding;
    if (m_frame)
        encoding = m_frame->loader()->documentLoader()->overrideEncoding();
    bool userChosen = !encoding.isNull() && !encoding.isEmpty();
    if (encoding.isNull()||encoding.isEmpty())
        encoding = textEncodingName;
    m_frame->loader()->setEncoding(encoding, userChosen);
    addData(data, length);
}


void WebCoreFrameBridge::clearFrame()
{
    m_frame = NULL;
}

void WebCoreFrameBridge::createFrameViewWithScrollView(MWebCoreWidget* view, int marginWidth, int marginHeight)
{
    // If we own the view, delete the old one - otherwise the render m_frame will take care of deleting the view.
    if (m_frame)
        m_frame->setView(NULL);

    FrameView* frameView = new FrameView(m_frame);
    m_frame->setView(frameView);
    frameView->deref();

    frameView->setView(view);
    if (marginWidth >= 0)
        frameView->setMarginWidth(marginWidth);
    if (marginHeight >= 0)
        frameView->setMarginHeight(marginHeight);
}

void WebCoreFrameBridge::forceLayoutAdjustingViewSize(bool flag)
{
    m_frame->forceLayout(!flag);
    if (flag)
        m_frame->view()->adjustViewSize();
}

void WebCoreFrameBridge::forceLayoutWithMinimumPageWidth(float minPageWidth, float maxPageWidth, bool adjustingViewSize)
{
    m_frame->forceLayoutWithPageWidthRange(minPageWidth, maxPageWidth, adjustingViewSize);
}

void WebCoreFrameBridge::sendResizeEvent()
{
    m_frame->sendResizeEvent();
}

void WebCoreFrameBridge::sendScrollEvent()
{
    m_frame->sendScrollEvent();
}

void WebCoreFrameBridge::installInFrame(MWebCoreWidget* view)
{
    // If this isn't the main frame, it must have a render m_frame set, or it
    // won't ever get installed in the view hierarchy.
    ASSERT(m_frame == m_frame->page()->mainFrame() || m_frame->ownerElement());

    m_frame->view()->setView(view);
    // FIXME: frame tries to do this too, is it needed?
    if (m_frame->ownerRenderer()) {
        m_frame->ownerRenderer()->setWidget(m_frame->view());
        // Now the render part owns the view, so we don't any more.
    }

    m_frame->view()->initScrollbars();
}


void WebCoreFrameBridge::setShouldCreateRenderers(bool f)
{
    m_shouldCreateRenderers = f;
}

bool WebCoreFrameBridge::shouldCreateRenderers()
{
    return m_shouldCreateRenderers;
}


bool WebCoreFrameBridge::needsLayout()
{
    return m_frame->view() ? m_frame->view()->needsLayout() : false;
}

void WebCoreFrameBridge::setNeedsLayout()
{
    if (m_frame->view())
        m_frame->view()->setNeedsLayout();
}
  
TPtrC8 WebCoreFrameBridge::baseURL()
{
    return m_frame->loader()->completeURL(m_frame->document()->baseURL()).des();
}

void WebCoreFrameBridge::updateFocusableRectList()
{
    // layout is changed, focusrect list needs update
    m_focusableNodeList.clear();

    for(Node* n = frame()->document(); n != 0; n = n->traverseNextNode()) {
        if(n->isFocusable() || n->hasTagName(areaTag))
            m_focusableNodeList.append(n->getRect());
    }
}

void WebCoreFrameBridge::updateThumbnail()
{
    WebFrame* frame = kit(m_frame);
    if (frame) {
        frame->frameView()->topView()->updatePageScaler();
    }
}

int WebCoreFrameBridge::textZoomPercent() const
{
    // FIXME: need to hook up with screen size.
    int sizeMultiplier = KXHTMLDefaultFontMultiplier;
    if (m_frame->document() && m_frame->document()->htmlMode() != Document::XHtml) {
        sizeMultiplier = StaticObjectsContainer::instance()->screenResolution() == ELowScreenResolution ?
        KDefaultLowResFontMultiplier : KDefaultFontMultiplier;
    }
#if PLATFORM(SYMBIAN)
    TLanguage uilang = User::Language();
    if(uilang == ELangKorean)
    {
       sizeMultiplier = 100;
    }
#endif
    if (control(m_frame)->settings())
        sizeMultiplier = textMultiplier(control(m_frame)->settings()->brctlSetting(TBrCtlDefs::ESettingsFontSize), sizeMultiplier);
    return sizeMultiplier;
}

bool WebCoreFrameBridge::searchFor(TPtrC string, bool forward, bool caseFlag, bool wrapFlag, bool startInSelection)
{
    return m_frame->findString(string, forward, caseFlag, wrapFlag, startInSelection);
}

void WebCoreFrameBridge::scalingFactorChanged(int factor)
{
    StaticObjectsContainer::instance()->fontCache()->setFontZoomFactor(factor);
}

int WebCoreFrameBridge::maxBidiWidth() 
{
    if (m_frame) {
        WebFrame* frame = kit(m_frame);
        if (frame && frame->frameView())
            return frame->frameView()->getBidiWidth();    
        }
    return KMaxTInt;        
}

void WebCoreFrameBridge::setWritingDirectionRtl(bool isRtl)
{
    // update rtl only for mainframe
    if (m_frame && m_frame == m_frame->page()->mainFrame()) {
        if (control(m_frame)->brCtlLayoutObserver())
            control(m_frame)->brCtlLayoutObserver()->NotifyLayoutChange(isRtl ? EOriginTopRight : EOriginTopLeft);
        if (kit(m_frame->page())->pageScaler())
            kit(m_frame->page())->pageScaler()->SetRelativePosition(0, 3, isRtl ? CPageScaler::ETopLeft : CPageScaler::ETopRight);
    }
}

bool WebCoreFrameBridge::getTypeFromElement(Node* node, TBrCtlDefs::TBrCtlElementType& aElType, TRect& aFocusRect, Node*& aRNode) const
{ 
     
    if(node && m_frame) {
    
        // get the navigable node at this point
        for (Node* n = node; n; n = n->parentNode()) {
            if( n->isFocusable() ) {
                aElType = nodeTypeB(n, m_frame);
                aFocusRect = n->getRect();
                aRNode = n;
                return true;
            }
            else {
                EventTargetNode* etn = NULL;
                if (n->isEventTargetNode()) {
                    etn = EventTargetNodeCast(n);
                }
                if( etn && (etn->getHTMLEventListener(mouseoverEvent) 
                    || etn->getHTMLEventListener(mousedownEvent)
                    || etn->getHTMLEventListener(mouseupEvent)) ) {                
                    aFocusRect = n->getRect();
                    aElType = TBrCtlDefs::EElementMouseButtonListener;
                    aRNode = n;
                    return true;
                }
                else if (n->isElementNode() && n->hasTagName(areaTag)) {
                    HTMLElement* e =  static_cast<HTMLElement*>(n);
                    if(!e->getAttribute(hrefAttr).isNull()) {
                        aElType = TBrCtlDefs::EElementAreaBox;
                        aFocusRect = n->getRect();
                        aRNode = n;
                        return true;
                    }
                }
                else if (n->isElementNode() && n->hasTagName(imgTag)) {
                    HTMLElement* e =  static_cast<HTMLElement*>(n);
                    // First check if the image is inside an anchor
                    for (Node* np = n; np; np = np->parentNode()) {
                        if( np->hasTagName(aTag) ) {
                            HTMLElement* ep =  static_cast<HTMLElement*>(np);
                            if(!ep->getAttribute(hrefAttr).isNull()) {
                                aElType = nodeTypeB(np, m_frame);
                                aFocusRect = np->getRect();
                                aRNode = np;
                                return true;
                            }
                        }
                    }
                    HTMLImageElement* imageElement = static_cast<HTMLImageElement *>(n);
                    RenderImage *r = static_cast<RenderImage *>(n->renderer());
                    CachedImage* ci = r->cachedImage();
                    if (ci && ci->errorOccurred()) {
                        aElType = TBrCtlDefs::EElementBrokenImage;
                    }
                    else {
                        String alttext = imageElement->altText();
                        if (alttext.find("nokia") != 0 && alttext.find("no_save") != 0) {
                            aElType = TBrCtlDefs::EElementImageBox;
                        }
                        else {
                            aElType = TBrCtlDefs::EElementNone;
                        }
                    }
                    aFocusRect = n->getRect().Rect();
                    aRNode = n;
                    return true;
                }
                else if (n->renderer() && n->renderer()->layer()) {
                    /*FIXME: to be implemneted for scrollbar
                    Scrollbar* hbar = n->renderer()->layer()->horizontalScrollbar();
                    if (hbar && IntRect(hbar->pos(),hbar->size()).contains(aPoint.iX,aPoint.iY)) {
                        aElType = EWebCoreElementScrollBar;
                        aFocusRect = IntRect(hbar->pos(),hbar->size()).Rect();
                        return true;
                    }
                    Scrollbar* vbar = n->renderer()->layer()->verticalScrollbar();
                    if (vbar && IntRect(vbar->pos(),vbar->size()).contains(aPoint.iX,aPoint.iY)) {
                        aElType = EWebCoreElementScrollBar;
                        aFocusRect = IntRect(vbar->pos(),vbar->size()).Rect();
                        return true;
                    }*/
               }
            }

        }
    }
    return false;
}




// end of file
