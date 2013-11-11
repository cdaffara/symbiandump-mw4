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

#ifndef __WEBFRAMEVIEW_H__
#define __WEBFRAMEVIEW_H__

#include "Shared.h"
#include "WebCoreWidget.h"
#include "ScrollTypes.h"
#include "ScrollBar.h"
#include <wtf/RefPtr.h>

namespace WebCore {
    class String;
    class PlatformScrollbar;
    class IntRect;
}

class WebFrame;
class WebView;
class TRect;

class WebFrameView : public MScrollView, public WebCore::Shared<WebFrameView>, public WebCore::ScrollbarClient
{
public:

    // from MScrollView
    void draw(WebCoreGraphicsContext&, const TRect&);
    void invalidateRect(const TRect&, TBool);
    void scrollTo(const TPoint&);
    TRect visibleRect() const;
    TSize contentSize() const;
    void resizeContent(const TSize& /*aSize*/);
    void setMayUseCopyScroll(TBool /*aCopy*/);
    TPoint convertContentToView(const TPoint& /*aPoint*/);
    TPoint convertViewToContent(const TPoint& /*aPoint*/);
    TBool hasHorizontalScrollbar() const;
    TBool hasVerticalScrollbar() const;
    void setHorizontalScrollingMode(WebCore::ScrollbarMode /*aScrollMode*/);
    void setVerticalScrollingMode(WebCore::ScrollbarMode /*aScrollMode*/);
    void setScrollingMode(WebCore::ScrollbarMode /*aScrollMode*/);
    WebCore::ScrollbarMode horizontalScrollingMode();
    WebCore::ScrollbarMode verticalScrollingMode();
    WebCore::PlatformScrollbar*  vScrollbar() {return m_vScrollbar;}
    WebCore::PlatformScrollbar*  hScrollbar() {return m_hScrollbar;}
    void setScrollbarsSuppressed(TBool /*aSuppressed*/, TBool /*aRepaintOnUnsuppress*/);
    void addChild(MWebCoreWidget* /*aChild*/ );
    TInt scalingFactor() const;
    TBool isVisible() const;    
    TRect rect() const;
    TBool isFocused() const;
    void makeVisible(TBool /*aVisible*/);
    void setFocus(TBool aFocus);
    void activate();
    void setParent(MScrollView* aParent);
    void setNeedsDisplay(TBool);
    void setCursor(CursorTypes);
    void setRect(const TRect& aRect);
    void ref() { ++m_refCount; }
    void deref() { if( --m_refCount == 0 ) delete this; }

    // accessors
    WebFrame* frame() const          { return m_frame.get(); }
    WebView* topView() const         { return m_topView; }
    void setWebFrame(WebFrame*);
    void setTopView(WebView*);
    TPoint contentPos() { return m_contentPos; }
    WebFrameView* initWithFrame(TRect frame);
    void setAllowsScrolling(bool allowsScrolling);
    void setMarginWidth(int marginWidth);
    void setMarginHeight(int marginHeight);
    int marginWidth() const;
    int marginHeight() const;
    void setContentPos(TPoint pt);

    WebFrameView();
    virtual ~WebFrameView();
    static WebFrameView* viewClassForMIMEType(const WebCore::String& MIMEType);

    void valueChanged(WebCore::Scrollbar*);
    WebCore::IntRect windowClipRect() const;

    TPoint nearestPointInFrame(const TPoint &aPoint);
    TPoint frameCoordsInViewCoords(const TPoint& aPoint);
    TPoint viewCoordsInFrameCoords(const TPoint& aPoint);
    TRect rectInGlobalCoords() const;
    TRect frameCoordsInViewCoords(const TRect &aRect);
    
    void checkScrollbarVisibility();
    int getBidiWidth();
    bool needScroll(const TPoint& aPoint);
private:
    WebFrameView(const WebFrameView&);            // not implemented
    WebFrameView& operator=(const WebFrameView&); // not implemented
    
    RefPtr<WebFrame>        m_frame;            // the frame associated with this view
    WebView*                m_topView;          // the top drawable view
    WebFrameView*           m_parent;           // the parent view

    TRect   m_frameRect;                        // the location and size of this frame in its parent frame
    TPoint  m_contentPos;
    TSize   m_contentSize;

    WebCore::PlatformScrollbar*  m_vScrollbar;           // scrollbars
    WebCore::PlatformScrollbar*  m_hScrollbar;
    WebCore::ScrollbarMode m_verticalScrollBarMode;
    WebCore::ScrollbarMode m_horizontalScrollBarMode;

    bool    m_hasBorder;
    bool    m_allowsScrolling;
    bool    m_mayUseCopyScroll;

    bool    m_resizeByEngine;
    int     m_refCount;
	TBool   m_isVisible;
};

#endif // END OF FILE
