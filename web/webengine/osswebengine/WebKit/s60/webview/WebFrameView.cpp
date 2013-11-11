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
#include "WebFrameView.h"
#include "WebFrame.h"
#include "WebView.h"
#include "WebCursor.h"
#include "WebFrameBridge.h"
#include "WebCoreGraphicsContext.h"
#include "GraphicsContext.h"
#include "PlatformScrollbar.h"
#include "pagescaler.h"
#include "WebKitLogger.h"
#include "BrCtl.h"
#include "SettingsContainer.h"
#include "StaticObjectsContainer.h"
#include "WebTabbedNavigation.h"

using namespace WebCore;

WebFrameView::WebFrameView() : 
      m_parent(NULL),
      m_hasBorder(true),
      m_refCount(1),
	  m_isVisible(ETrue)
{
    m_hScrollbar = new WebCore::PlatformScrollbar(this, WebCore::HorizontalScrollbar, WebCore::RegularScrollbar);
    m_vScrollbar = new WebCore::PlatformScrollbar(this, WebCore::VerticalScrollbar, WebCore::RegularScrollbar); 
    m_horizontalScrollBarMode = ScrollbarAuto;
    m_verticalScrollBarMode = ScrollbarAuto;
}

WebFrameView::~WebFrameView() 
{
    delete m_hScrollbar;
    delete m_vScrollbar;
}

void WebFrameView::setTopView(WebView* v)
{
    m_topView = v;
}

void WebFrameView::setWebFrame(WebFrame* f)
{
    m_frame = f;
}

WebFrameView* WebFrameView::initWithFrame(TRect frame)
{
    setRect(frame);
    return this;
}

void WebFrameView::draw(WebCoreGraphicsContext& gc, const TRect& r)
{
    TRect vr(visibleRect());
    TRect rect(r);
    if (isScaled() || m_frame->isFrameSet())
        rect.Grow(1,1);             // eliminate rounding errors
    TRect frameRect(m_frameRect);

    rect.Move(-frameRect.iTl);
    rect.Move(m_contentPos);
    if (vr.Intersects(rect)) {
        // save the gc state
        TWebCoreSavedContext saved(gc.save());
        // set the current frame
        gc.setView(*this);

        TPoint cpos(toViewCoords(m_contentPos));

        gc.setOrigin(gc.origin()+toViewCoords(m_frameRect.iTl)-cpos);
        rect.Intersection(vr);
        // calculate clip for zoom level
        TRect clip(toViewCoords(rect));
        
        // set clipping
        if (gc.hasClipping()) {
            TRect t(gc.clippingRect());
            t.Move(-toViewCoords(m_frameRect.iTl)+cpos);
            clip.Intersection(t);
        }
        
        gc.setClippingRect( clip );

        // draw frame content
        m_frame->paintRect(gc, rect);
        gc.cancelClipping();
        gc.setOrigin( gc.origin() + cpos);
        if (m_parent) {
            WebFrameView* parent = m_parent;
            TRect frameClip(0,0,KMaxTInt,KMaxTInt);
            do {
                TRect pr = parent->visibleRect();
                TRect fc(TRect(parent->frameCoordsInViewCoords(pr.iTl), parent->frameCoordsInViewCoords(pr.iBr))); 
                frameClip.Intersection(fc);
                parent = parent->m_parent;
            }
            while(parent);
            frameClip = TRect(viewCoordsInFrameCoords(frameClip.iTl), viewCoordsInFrameCoords(frameClip.iBr)); 
            frameClip = TRect(toViewCoords(frameClip));
            frameClip.Move(-cpos);
            gc.setClippingRect( frameClip );
        }
        // draw frame border
        CFbsBitGc& realgc = gc.gc();
        if (m_hasBorder && !m_frame->isFrameSet()) {
            // already moved the origin
            TRect borderRect(TPoint(-1,-1),toViewCoords(m_frameRect).Size());
            borderRect.iBr += TPoint(2,2);
            realgc.SetPenColor(TRgb(0x55,0x55,0x55));
            realgc.SetPenStyle(CGraphicsContext::ESolidPen);
            realgc.SetBrushStyle(CGraphicsContext::ENullBrush);
            realgc.SetPenSize(TSize(1,1));
            realgc.DrawRect(borderRect);
            // double border in bottom/right
            borderRect.iBr += TPoint(1,1);
            realgc.DrawRect(borderRect);
        }

        // draw scrollbars
        rect.Move( -m_contentPos );
        WebCore::GraphicsContext ctx(&gc);
        if (m_vScrollbar->isEnabled())
            m_vScrollbar->paint(&ctx, rect);
        if (m_hScrollbar->isEnabled())
            m_hScrollbar->paint(&ctx, rect);

        gc.cancelClipping();
        gc.restore(saved);
    }
}

void WebFrameView::invalidateRect(const TRect& rect, TBool drawNow)
{
    TRect r(rect);
    if (m_parent) {
        TRect frameRect(m_frameRect);
        r.Move(frameRect.iTl - m_contentPos);
        r.Intersection(frameRect);
        m_parent->invalidateRect(r, drawNow);
    }
    else {
        // fix rounding errors from zooming
        if (isScaled() || m_frame->isFrameSet())
            r.Grow(1,1);
    }

    if (!m_topView->isClosing()) {
        if(drawNow)
            m_topView->syncRepaint(r);
        else
            m_topView->scheduleRepaint(r);
    }
}

void WebFrameView::activate()
{
}

void WebFrameView::setRect(const TRect& rect)
{
    TRect frameRect(rect);

    if (m_frameRect!=frameRect) {

        m_frameRect = frameRect;
        checkScrollbarVisibility();
    }
}

int WebFrameView::getBidiWidth()
{
    TInt bidiWidth = KMaxTInt;
    // don't limit bidi width for low subframes
    if (!(toViewCoords(m_frameRect).Height()<150 && m_parent))
        bidiWidth = m_topView->maxBidiSize().iWidth;

    return bidiWidth;
}

void WebFrameView::checkScrollbarVisibility()
{
    if (!m_parent || m_parent->frame()->isFrameSet()) {
        m_vScrollbar->setEnabled(false);
        m_hScrollbar->setEnabled(false);
        return;
    }
    bool vScrollbar = m_vScrollbar->isEnabled();
    bool hScrollbar = m_hScrollbar->isEnabled();
    
    if (!m_allowsScrolling) {
        m_vScrollbar->setEnabled(false);
        m_hScrollbar->setEnabled(false);
        return;
    }
    TRect frameRect(m_frameRect);
    switch(m_verticalScrollBarMode) {
        default:
        case ScrollbarAlwaysOff:
            vScrollbar = false;
        break;
        case ScrollbarAlwaysOn:
            vScrollbar = true;
        break;
        case ScrollbarAuto:
            vScrollbar = contentSize().iHeight > frameRect.Height();
        break;
    }

    switch(m_horizontalScrollBarMode) {
        default:
        case ScrollbarAlwaysOff:
            hScrollbar = false;
        break;
        case ScrollbarAlwaysOn:
            hScrollbar = false;
        break;
        case ScrollbarAuto:
            hScrollbar = contentSize().iWidth > frameRect.Width();
        break;
    }
    
    m_hScrollbar->setEnabled(hScrollbar);
    m_vScrollbar->setEnabled(vScrollbar);

    TRect r(TPoint(0,0), frameRect.Size() - TSize(vScrollbar? KScrollbarWidth : 0, hScrollbar ? KScrollbarWidth : 0));
    if (hScrollbar) {
        m_hScrollbar->setRect(TRect(TPoint(r.iTl.iX, r.iBr.iY ), TSize(r.Width(), KScrollbarWidth)));
        m_hScrollbar->setProportion(r.Width(), m_contentSize.iWidth);
    }

    if (vScrollbar)  {
        m_vScrollbar->setRect(TRect(TPoint(r.iBr.iX, r.iTl.iY), TSize(KScrollbarWidth, r.Height())));
        m_vScrollbar->setProportion(r.Height(), m_contentSize.iHeight);
    }
}

void WebFrameView::setCursor(CursorTypes cursorType)
{
    if (m_topView && StaticObjectsContainer::instance()->webCursor())
        StaticObjectsContainer::instance()->webCursor()->setCursor(cursorType);
}

void WebFrameView::setParent(MScrollView* parent)
{
    m_parent = static_cast<WebFrameView*>(parent);
}

TRect WebFrameView::rect() const
{
    return m_frameRect;
}

TRect WebFrameView::visibleRect() const
{
    TSize s(m_frameRect.Size());
    return TRect(m_contentPos, s );
}

void WebFrameView::scrollTo(const TPoint& aPoint)
{
    if (m_parent) {
        // tot:fixme frame scrolling when frame-flat is off
        if (m_frame->isIframe()) {
            // iframe, support scrolling
            if (aPoint != m_contentPos) {
                TPoint p(nearestPointInFrame(aPoint));
                m_contentPos = p;
                m_frame->notifyPluginsOfScrolling();
                m_topView->syncRepaint( TRect(0,0,KMaxTInt/2,KMaxTInt/2) );
            }
        }
        else {
            if (aPoint.iX==0 && aPoint.iY==0) {
                // this gets called for all frames during loading, needs to be ignored (breaks some cases though)
                return;
            }

            TPoint p(0, aPoint.iY); // don't to x shifts, they are confusing here
            // subframes don't scroll in the current implementation, scroll the main frame
            p += toDocCoords(rectInGlobalCoords().iTl);
            m_topView->mainFrame()->frameView()->scrollTo(p);
        }
    }
    else {
        // main frame, scroll
        if (aPoint != m_contentPos) {
            TPoint p( nearestPointInFrame(aPoint) );
            TBool copyScroll = m_mayUseCopyScroll;

            if( p != aPoint && m_topView->inFindState() ) {
                p = aPoint;
                m_topView->clearOffScreenBitmap();
                copyScroll = EFalse;
            }

            TPoint from( toViewCoords(m_contentPos) );
            TPoint to( toViewCoords(p) );

            m_topView->scrollBuffer(to, from, copyScroll);
            m_contentPos = p;

            
            m_frame->notifyPluginsOfScrolling();
            
            
            if( m_topView->pageScaler() && m_topView->pageScaler()->Visible())
                m_topView->pageScaler()->DocumentViewportMoved();

            //Update scroll bar , thumb position
            m_topView->updateScrollbars(m_contentSize.iHeight, m_contentPos.iY, m_contentSize.iWidth, m_contentPos.iX);

            if (m_topView->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeTabbed) {
                int h = (to.iX - from.iX);
                int v = (to.iY - from.iY);
                h = (h != 0) ? ((h > 0) ? 1 : -1) : h;
                v = (v != 0) ? ((v > 0) ? 1 : -1) : v;
                m_topView->tabbedNavigation()->updateCursorPosAfterScroll(core(m_frame.get()), h, v);
            }
            // trigger painting
            m_topView->syncRepaint();
        }
    }
}


bool WebFrameView::needScroll(const TPoint& aPoint)
{
    bool rv = false;
    if (m_parent) {
        if (m_frame->isIframe()) {
            if (aPoint != m_contentPos) {
                rv = true;
            }
        }
        else {
            if (!(aPoint.iX==0 && aPoint.iY==0)){
                 rv = true;
            }
        }
    }
    else {
        // main frame
        if (aPoint != m_contentPos) {
            TPoint p( nearestPointInFrame(aPoint) );
            TBool copyScroll = m_mayUseCopyScroll;

            if( m_contentPos != p ){
                rv = true;
            }
        }
    }
    return rv;
}

TPoint WebFrameView::nearestPointInFrame(const TPoint &aPoint)
{
    if( m_parent && !m_frame->isIframe() ) return aPoint;
    TSize frameSize( m_frameRect.Size() );
    TPoint p(aPoint);
    p.iX = Min(contentSize().iWidth - frameSize.iWidth, p.iX);
    p.iY = Min(contentSize().iHeight - frameSize.iHeight, p.iY);
    p.iX = Max(0, p.iX);
    p.iY = Max(0, p.iY);

    return p;
}

void WebFrameView::resizeContent(const TSize &size)
{

    if (!m_parent) {
        if (m_topView->brCtl()->capabilities()&TBrCtlDefs::ECapabilityFitToScreen) {
            if( size.iHeight != 0 && size.iWidth != 0 ){
                m_topView->updateMinZoomLevel(size);
            }
        }
    }

    if (size != m_contentSize) {

        m_resizeByEngine = ETrue;
        m_contentSize = size;
        checkScrollbarVisibility();
        
        //maybe the content got smaller and we need to scroll back to view?
        TPoint p( nearestPointInFrame(m_contentPos) );
        if (p!=m_contentPos)
            // this will also update scrollbars is necessary
            scrollTo(p);
        else if (!m_parent) {
            // top level
            m_topView->updateScrollbars(m_contentSize.iHeight, m_contentPos.iY, m_contentSize.iWidth, m_contentPos.iX);
        }
    }
}

void WebFrameView::setMayUseCopyScroll(TBool aCopy)
{
    m_mayUseCopyScroll = aCopy;
}

TPoint WebFrameView::convertContentToView(const TPoint& aConvert)
{
    return frameCoordsInViewCoords(aConvert);
}

TPoint WebFrameView::convertViewToContent(const TPoint& aConvert)
{
    return viewCoordsInFrameCoords( aConvert );
}

TBool WebFrameView::hasHorizontalScrollbar() const 
{
    return m_vScrollbar != NULL;
}

TBool WebFrameView::hasVerticalScrollbar() const
{
    return m_hScrollbar != NULL;
}

void WebFrameView::setHorizontalScrollingMode(ScrollbarMode aMode)
{
    m_horizontalScrollBarMode = aMode;
    checkScrollbarVisibility();
}

WebCore::ScrollbarMode WebFrameView::verticalScrollingMode() 
{
    return m_verticalScrollBarMode;
}

void WebFrameView::setVerticalScrollingMode(ScrollbarMode aMode)
{
    m_verticalScrollBarMode = aMode;
    checkScrollbarVisibility();
}

WebCore::ScrollbarMode WebFrameView::horizontalScrollingMode()
{
    return m_horizontalScrollBarMode;
}

void WebFrameView::setScrollingMode(ScrollbarMode aMode)
{
    setHorizontalScrollingMode(aMode);
    setVerticalScrollingMode(aMode);
}

void WebFrameView::setScrollbarsSuppressed(TBool /*aSuppressed*/,TBool /*aRepaintOnUnsuppress*/ )
{
    // ### NOT IMPLEMENTED
}

void WebFrameView::addChild(MWebCoreWidget* aChild)
{
    if(aChild)
        aChild->setParent(this);
}

TInt WebFrameView::scalingFactor() const
{
    if (m_topView)
        return m_topView->scalingFactor();
    return 100;
}

TPoint WebFrameView::viewCoordsInFrameCoords(const TPoint &aPoint)
{
    TPoint pt = aPoint;

    pt += toViewCoords(m_contentPos);
    WebFrameView* pv = m_parent;
    while(pv)
        {
        pt += pv->toViewCoords(pv->m_contentPos);
        pv = pv->m_parent;
        }

    pt -= rectInGlobalCoords().iTl;
    pt = m_topView->mainFrame()->frameView()->toDocCoords(pt);
    return pt;
}

TPoint WebFrameView::frameCoordsInViewCoords(const TPoint &aPoint)
{
    TPoint pt(aPoint);

    pt = m_topView->mainFrame()->frameView()->toViewCoords(aPoint);
    pt += rectInGlobalCoords().iTl;
    pt -= toViewCoords(m_contentPos);
    WebFrameView* pv= m_parent;
    while(pv)
        {
        pt -= pv->toViewCoords(pv->m_contentPos);
        pv = pv->m_parent;
        }
    return pt;
}

TRect WebFrameView::frameCoordsInViewCoords(const TRect &aRect)
{
    TSize s = aRect.Size(); 
    return TRect(frameCoordsInViewCoords(aRect.iTl), toViewCoords(s));
}


TRect WebFrameView::rectInGlobalCoords() const
{
    TRect rect(toViewCoords(m_frameRect));

    for (WebFrameView* p = m_parent; p; p=p->m_parent)
        rect.Move(toViewCoords(p->m_frameRect.iTl));

    return rect;
}

void WebFrameView::setNeedsDisplay(TBool n) 
{
}

void WebFrameView::setFocus(TBool aFocus) 
{
}

void WebFrameView::makeVisible(TBool aVisible) 
{
    m_isVisible = aVisible;
}

TBool WebFrameView::isFocused() const
{
    return ETrue;
}

TBool WebFrameView::isVisible() const
{
    return m_isVisible;
}

TSize WebFrameView::contentSize() const
{
    return m_contentSize;
}

void WebFrameView::setAllowsScrolling(bool allowsScrolling)
{
    m_allowsScrolling = allowsScrolling;
}

void WebFrameView::setMarginWidth(int marginWidth)
{
}

void WebFrameView::setMarginHeight(int marginHeight)
{
}

int WebFrameView::marginWidth() const
{
    return 0;
}

int WebFrameView::marginHeight() const 
{
    return 0;
}

WebFrameView* WebFrameView::viewClassForMIMEType(const WebCore::String& MIMEType) 
{
    return NULL;
}

void WebFrameView::valueChanged(WebCore::Scrollbar*) 
{
}

WebCore::IntRect WebFrameView::windowClipRect() const
{
    return WebCore::IntRect();
}

void WebFrameView::setContentPos(TPoint pt)
{
    m_contentPos = pt;
}

// END OF FILE
