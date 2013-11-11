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
#include "ScrollView.h"

#include "WebCoreWidget.h"
#include "FloatRect.h"
#include "IntRect.h"

/*
    This class implementation does NOT actually emulate the Qt ScrollView.
    It does provide an implementation that khtml will use to interact with
    WebKit's WebFrameView documentView and our NSScrollView subclass.

    ScrollView's view is a NSScrollView (or subclass of NSScrollView)
    in most cases. That scrollview is a subview of an
    WebCoreFrameView. The WebCoreFrameView's documentView will also be
    the scroll view's documentView.

    The WebCoreFrameView's size is the frame size.  The WebCoreFrameView's documentView
    corresponds to the frame content size.  The scrollview itself is autosized to the
    WebCoreFrameView's size (see Widget::resize).
*/

namespace WebCore {

int ScrollView::visibleWidth() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->visibleRect().Width():0;
    }

int ScrollView::visibleHeight() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->visibleRect().Height():0;
    }

FloatRect ScrollView::visibleContentRect() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?FloatRect( v->visibleRect() ):FloatRect( 0, 0, 0, 0 );
    }

int ScrollView::contentsWidth() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->contentSize().iWidth:0;
    }

int ScrollView::contentsHeight() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->contentSize().iHeight:0;
    }

int ScrollView::contentsX() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->visibleRect().iTl.iX:0;
    }

int ScrollView::contentsY() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?v->visibleRect().iTl.iY:0;
    }

IntSize ScrollView::scrollOffset() const
    {
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?IntPoint( v->visibleRect().iTl ) - IntPoint():IntSize();
    }

void ScrollView::scrollBy(int dx, int dy)
    {
    setContentsPos(contentsX() + dx, contentsY() + dy);
    }

void ScrollView::scrollRectIntoViewRecursively(const IntRect& rect)
    {
    int x = rect.x();
    int y = rect.y();
    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;
    /*NSPoint p = NSMakePoint(x,y);

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    NSView *docView;
    NSView *view = getView();
    docView = getDocumentView();
    if (docView)
        view = docView;

    NSView *originalView = view;
    while (view) {
        if ([view isKindOfClass:[NSClipView class]]) {
            NSPoint viewPoint = [view convertPoint:p fromView:originalView];
            [view scrollPoint:viewPoint];
        }
        view = [view superview];
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    setContentsPos(x,y);
    }

FloatRect ScrollView::visibleContentRectConsideringExternalScrollers() const
{
    // external scrollers not supported for now
    return visibleContentRect();
}

void ScrollView::setContentsPos(int x, int y)
    {
    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;
    /*NSPoint p =  NSMakePoint(x,y);

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    NSView *docView;
    NSView *view = getView();
    docView = getDocumentView();
    if (docView)
        view = docView;
    [view scrollPoint:p];
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->scrollTo(TPoint(x,y));
        }

    }

void ScrollView::setVScrollbarMode(ScrollbarMode vMode)
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        [frameView setVerticalScrollingMode: (WebCoreScrollbarMode)vMode];
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->setVerticalScrollingMode(vMode);
        }
    }

void ScrollView::setHScrollbarMode(ScrollbarMode hMode)
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        [frameView setHorizontalScrollingMode: (WebCoreScrollbarMode)hMode];
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->setHorizontalScrollingMode(hMode);
        }

    }

void ScrollView::setScrollbarsMode(ScrollbarMode mode)
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        [frameView setScrollingMode: (WebCoreScrollbarMode)mode];
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->setScrollingMode(mode);
        }
    }

ScrollbarMode ScrollView::vScrollbarMode() const
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        return (ScrollbarMode)[frameView verticalScrollingMode];
    }
    END_BLOCK_OBJC_EXCEPTIONS;

    return ScrollbarAuto;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?(ScrollbarMode)v->verticalScrollingMode():ScrollbarAuto;
    }

ScrollbarMode ScrollView::hScrollbarMode() const
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        return (ScrollbarMode)[frameView horizontalScrollingMode];
    }
    END_BLOCK_OBJC_EXCEPTIONS;

    return ScrollbarAuto;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    return v?(ScrollbarMode)v->horizontalScrollingMode():ScrollbarAuto;
    }

void ScrollView::suppressScrollbars(bool suppressed,  bool repaintOnUnsuppress)
    {
    /*NSView* view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view conformsToProtocol:@protocol(WebCoreFrameView)]) {
        NSView<WebCoreFrameView>* frameView = (NSView<WebCoreFrameView>*)view;
        [frameView setScrollbarsSuppressed: suppressed
                       repaintOnUnsuppress: repaintOnUnsuppress];
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->setScrollbarsSuppressed(suppressed,repaintOnUnsuppress);
        }
    }

void ScrollView::addChild(Widget* child )
    {
    ASSERT(child != this);

    /*
    // we don't need to do the offscreen position initialization that KDE needs
    if (x != -500000)
        child->move(x, y);

    NSView *thisView = getView();
    NSView *thisDocView = getDocumentView();
    if (thisDocView)
        thisView = thisDocView;

#ifndef NDEBUG
    NSView *subview = child->getOuterView();

    LOG(Frames, "Adding %p %@ at (%d,%d) w %d h %d\n", subview,
        [(id)[subview class] className], x, y, (int)[subview frame].size.width, (int)[subview frame].size.height);
#endif
    child->addToSuperview(thisView);*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->addChild(child->getView());
        }
    }

void ScrollView::removeChild(Widget* child)
    {
    //child->removeFromSuperview();
	if(child)
	    {
		child->hide();    
	    }
    }

void ScrollView::resizeContents(int w, int h)
    {
    /*BEGIN_BLOCK_OBJC_EXCEPTIONS;
    int _w = w;
    int _h = h;

    LOG(Frames, "%p %@ at w %d h %d\n", getView(), [(id)[getView() class] className], w, h);
    NSView *view = getView();
    if ([view isKindOfClass:[NSScrollView class]]){
        view = getDocumentView();

        LOG(Frames, "%p %@ at w %d h %d\n", view, [(id)[view class] className], w, h);
        if (_w < 0)
            _w = 0;
        if (_h < 0)
            _h = 0;

        NSSize tempSize = { _w, _h }; // workaround for 4213314
        [view setFrameSize:tempSize];
    } else {
        resize (_w, _h);
    }
    END_BLOCK_OBJC_EXCEPTIONS;*/
    if (w < 0)
        {
        w = 0;
        }
    if (h < 0)
        {
        h = 0;
        }
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->resizeContent(TSize(w,h));
        }
    }

void ScrollView::updateContents(const IntRect &rect, bool now)
    {
    /*BEGIN_BLOCK_OBJC_EXCEPTIONS;

    NSView *view = getView();

    if ([view isKindOfClass:[NSScrollView class]])
        view = getDocumentView();

    // Checking for rect visibility is an important optimization for the case of
    // Select All of a large document. AppKit does not do this check, and so ends
    // up building a large complicated NSRegion if we don't perform the check.
    NSRect dirtyRect = NSIntersectionRect(rect, [view visibleRect]);
    if (!NSIsEmptyRect(dirtyRect)) {
        [view setNeedsDisplayInRect:dirtyRect];
        if (now) {
            [[view window] displayIfNeeded];
            [[view window] flushWindowIfNeeded];
        }
    }

    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->invalidateRect( rect, now );
        }
    }

void ScrollView::update()
{
/*
    BEGIN_BLOCK_OBJC_EXCEPTIONS;

    NSView *view = getView();
    [[view window] displayIfNeeded];
    [[view window] flushWindowIfNeeded];

    END_BLOCK_OBJC_EXCEPTIONS;
*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->invalidateRect( v->visibleRect(), true );
        }
}

// NB, for us "viewport" means the NSWindow's coord system, which is origin lower left

IntPoint ScrollView::contentsToWindow(const IntPoint& contentsPoint) const
    {
    /*BEGIN_BLOCK_OBJC_EXCEPTIONS;

    NSView *docView;
    NSView *view = getView();

    docView = getDocumentView();
    if (docView)
        view = docView;

    NSPoint tempPoint = { contentsPoint.x(), contentsPoint.y() }; // workaround for 4213314
    NSPoint np = [view convertPoint:tempPoint toView: nil];
    return IntPoint(np);

    END_BLOCK_OBJC_EXCEPTIONS;

    return IntPoint();*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    TPoint p(0,0);
    if (v)
        {
        TPoint tempPoint( contentsPoint.x(), contentsPoint.y() );
        p = v->convertContentToView( tempPoint );
        }
    return IntPoint( p );
    }

IntPoint ScrollView::windowToContents(const IntPoint& viewportPoint) const
    {
    /*BEGIN_BLOCK_OBJC_EXCEPTIONS;

    NSView *docView;
    NSView *view = getView();

    docView = getDocumentView();
    if (docView)
        view = docView;

    NSPoint tempPoint = { viewportPoint.x(), viewportPoint.y() }; // workaround for 4213314
    NSPoint np = [view convertPoint:tempPoint fromView: nil];

    return IntPoint(np);

    END_BLOCK_OBJC_EXCEPTIONS;

    return IntPoint();*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    TPoint p;
    if (v)
        {
        TPoint tempPoint( viewportPoint.x(), viewportPoint.y() );
        p = v->convertViewToContent( tempPoint );
        }
    return IntPoint( p );
    }

void ScrollView::setStaticBackground(bool b)
    {
    /*NSScrollView *view = (NSScrollView *)getView();
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view isKindOfClass:[NSScrollView class]])
        [[view contentView] setCopiesOnScroll: !b];
    END_BLOCK_OBJC_EXCEPTIONS;*/
    MScrollView* v = static_cast<MScrollView*>(getView());
    if (v)
        {
        v->setMayUseCopyScroll(!b);
        }
    }

MScrollView& ScrollView::getDocumentView() const
    {
    /*id view = getView();

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    if ([view respondsToSelector:@selector(documentView)])
        return [view documentView];
    END_BLOCK_OBJC_EXCEPTIONS;

    return nil;*/
    return *static_cast<MScrollView*>(getView());
    }

bool ScrollView::inWindow() const
    {
    return true; //[getView() window];
    }

}
