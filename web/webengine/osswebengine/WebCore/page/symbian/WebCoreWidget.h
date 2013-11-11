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


#ifndef MWEBCOREWIDGET_H_
#define MWEBCOREWIDGET_H_

//  INCLUDES
#include <e32base.h>
#include <bitstd.h>
#include  "WebCorePalette.h"
#include "ScrollTypes.h"


// DATA TYPES
typedef enum {
    EWebCoreScrollbarAuto,
    EWebCoreScrollbarAlwaysOff,
    EWebCoreScrollbarAlwaysOn
} TWebCoreScrollbarMode;

typedef enum {
    PointerCursor,
    CrossCursor,
    HandCursor,
    MoveCursor,
    IBeamCursor,
    WaitCursor,
    HelpCursor,
    SelectMultiCursor
    } CursorTypes;

// FORWARD DECLARATIONS
class MScrollView;
class QPainter;
class CCoeControl;
class GraphicsContext;
class WebCoreGraphicsContext;

class MWebCoreWidget
  {
  public:
    virtual void draw(WebCoreGraphicsContext&, const TRect&) = 0;
    virtual TBool isVisible() const = 0;
    virtual TRect rect() const = 0;
    virtual TSize sizeHint() const = 0;
    virtual TBool isFocused() const = 0;
    virtual void makeVisible(TBool) = 0;
    virtual void setFocus(TBool aFocus) = 0;
    virtual void setRect(const TRect& aRect) = 0;
    virtual void activate() = 0;
    virtual void setParent(MScrollView* aParent) =  0;
    virtual void ref() = 0;
    virtual void deref() = 0;
    virtual TBool isScrollView() const { return EFalse; }
    virtual TBool isObjectView() const { return EFalse; }
    virtual void setPalette(TWebCorePalette) {}
    virtual void setOutlineWidth(TUint8 ) {}
    virtual void setWritingDirectionRtl(TBool) {}
    virtual void setNeedsDisplay(TBool) = 0;
    virtual void setCursor(CursorTypes) = 0;
    virtual TBool isFocusable() const { return ETrue; }
    virtual void* pluginScriptableObject() { return 0; }
    virtual TBool hasResized() { return EFalse; }
  };

class MScrollView: public MWebCoreWidget
  {
  public:    
    TBool isScrollView() const { return ETrue; }
    virtual TSize sizeHint() const { return TSize(); }
    virtual void draw(WebCoreGraphicsContext&, const TRect&) { }
    virtual void invalidateRect(const TRect&, TBool) = 0;
    virtual void scrollTo(const TPoint&) = 0;
    virtual TRect visibleRect() const = 0;
    virtual TSize contentSize() const = 0;
    virtual void resizeContent(const TSize&) = 0;
    virtual void setMayUseCopyScroll(TBool) = 0;
    virtual TPoint convertContentToView(const TPoint&) = 0;
    virtual TPoint convertViewToContent(const TPoint&) = 0;
    virtual TBool hasHorizontalScrollbar() const = 0;
    virtual TBool hasVerticalScrollbar() const = 0;
    virtual void setHorizontalScrollingMode(WebCore::ScrollbarMode) = 0;
    virtual void setVerticalScrollingMode(WebCore::ScrollbarMode) = 0;
    virtual void setScrollingMode(WebCore::ScrollbarMode) = 0;
    virtual WebCore::ScrollbarMode horizontalScrollingMode() = 0;
    virtual WebCore::ScrollbarMode verticalScrollingMode() = 0;
    virtual void setScrollbarsSuppressed(TBool, TBool) = 0;
    virtual void addChild(MWebCoreWidget*) = 0;
    virtual TInt scalingFactor() const = 0;

    // coordinates transforming
    TPoint  toDocCoords(const TPoint& aFrom) const;
    TSize   toDocCoords(const TSize& aFrom) const;
    TRect   toDocCoords(const TRect& aFrom) const;
    TPoint  toViewCoords(const TPoint& aFrom) const;
    TSize   toViewCoords(const TSize& aFrom) const;
    TRect   toViewCoords(const TRect& aFrom) const;

    bool isScaled() const;
  };

class MWebCoreObjectWidget: public MWebCoreWidget
{
public: 
    virtual ~MWebCoreObjectWidget() {};
    TBool isObjectView() const { return ETrue; }
    virtual TBool isActive() const = 0;
    virtual void deActivate() = 0;
    virtual void setFont(CFont* aFont) = 0;
    virtual void playPluginContent(const TDesC8& aRequestUrl) = 0;
    virtual void stopPluginContent(const TDesC8& aRequestUrl) = 0;
    virtual void positionChanged() {}    
    virtual TBool isFocusable() const { return ETrue; }
    virtual TBool hasResized() { return EFalse; }
    };


inline TPoint MScrollView::toDocCoords(const TPoint& aFrom) const
    {
    TInt z = scalingFactor();
    return TPoint(aFrom.iX*100/z,aFrom.iY*100/z);
    }

inline TSize MScrollView::toDocCoords(const TSize& aFrom) const
    {
    TInt z = scalingFactor();
    return TSize(aFrom.iWidth*100/z,aFrom.iHeight*100/z);
    }

inline TRect MScrollView::toDocCoords(const TRect& aFrom) const
    {
    return TRect(toDocCoords(aFrom.iTl),toDocCoords(aFrom.iBr));
    }

inline TPoint MScrollView::toViewCoords(const TPoint& aFrom) const
    {
    TInt z = scalingFactor();
    return TPoint(aFrom.iX*z/100,aFrom.iY*z/100);
    }

inline TSize MScrollView::toViewCoords(const TSize& aFrom) const
    {
    TInt z = scalingFactor();
    return TSize(aFrom.iWidth*z/100,aFrom.iHeight*z/100);
    }

inline TRect MScrollView::toViewCoords(const TRect& aFrom) const
    {
    return TRect(toViewCoords(aFrom.iTl),toViewCoords(aFrom.iBr));
    }

inline bool MScrollView::isScaled() const 
    { 
    return scalingFactor()!=100; 
    }

#endif //MWEBCOREWIDGET_H_
