/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements page scrolling functionality
*
*/



#ifndef WEBPAGESCROLLHANDLER_H
#define WEBPAGESCROLLHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include <rt_gesturehelper.h>
#include "WebScrollingDecelerator.h"


// MACROS

// FORWARD DECLARATIONS
class WebView;
class WebFrame;
class WebFrameView;
class WebScrollbarDrawer;
class WebScrollingDeceleratorGH;

namespace WebCore
{
    class Element;        
};

enum ScrollDirectionState
{
    ScrollDirectionUnassigned,
    ScrollDirectionX,
    ScrollDirectionY,
    ScrollDirectionXY
};

class ScrollableView : public CBase
{
    public:
        ScrollableView() {}
        ~ScrollableView() {}
        TPoint contentPos();
        WebFrameView* activeFrameView();

    public:
        WebFrameView* m_frameView;
        WebCore::Element* m_scrollingElement;
};

// CLASS DECLARATION
class WebPageScrollHandler: public CBase
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static WebPageScrollHandler* NewL( WebView& webView );

        /**
        * Destructor.
        */
        virtual ~WebPageScrollHandler();

    private:  // Constructors

        /**
        * C++ default constructor.
        */
        WebPageScrollHandler( WebView& webView );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void constructL();

    public: // New functions
        /**
        * @since 5.0
        * @param
        * @return
        */
        bool  touchScrolling() { return m_touchScrolling;}
        
        /**
        * @since 5.0
        * @param
        * @return
        */
        void handlePageOverviewScrollingL(const TPointerEvent& pointerEvent);

        /**
        * @since 5.0
        * param
        * return
        */
        void handlePageOverviewScrolling();
        

        /**
        * @since 5.0
        * param
        * return
        */
        void setupScrolling(const TPoint& aNewPosition);

        /**
        * @since 5.0
        * param
        * return
        */ 
        void clearScrollingElement();
        
        /**
        * @since 5.0
        * param
        * return
        */
        WebFrameView* currentScrollingFrameView() { return m_scrollableView.m_frameView;}
        
        /**
        * @since 5.0
        * param
        * return
        */
        WebCore::Element* currentScrollingElement() { return m_scrollableView.m_scrollingElement;}
        
        /**
        * @since 5.0
        * param
        * return
        */
        void updateScrolling(const TPointerEvent& pointerEvent);
        
        /**
        * @since 5.0
        * param
        * return
        */
        void scrollContent();
        
        /**
        * @since 5.0
        * param
        * return
        */
        void scrollContent(TPoint& scrollDelta);
        
        /**
        * @since 5.0
        * param
        * return
        */
        void handlePageOverviewScrollCallback();
        
        /**
        * @since 5.0
        * param
        * return
        */
        WebScrollbarDrawer*  scrollbarDrawer() {return m_scrollbarDrawer;}

        //callbacks
        static int pageOverviewScrollCallback( TAny* aPtr );
        //static int handleScrollTimerEventCallback( TAny* ptr);
        void scrollPageOverviewGH();
        void handleScrollingGH(const RT_GestureHelper::TGestureEvent& aEvent);
        void handleTouchDownGH(const RT_GestureHelper::TGestureEvent& aEvent);
        void handleTouchUpGH(const RT_GestureHelper::TGestureEvent& aEvent);
        void updateScrollbars(const TPoint& scrollPos, TPoint& newscrollDelta);

public:

     private:
        void calculateScrollDirection(int absX, int absY);
        bool calculateScrollableFrameView(const TPoint& aNewPosition);
        bool calculateScrollableElement(const TPoint& aNewPosition);
        
        void scrollPageOverview(const TPointerEvent& pointerEvent);
        bool startDeceleration(const RT_GestureHelper::TGestureEvent& aEvent);
        
     private:  
        // Pointer to owning view
        WebView* m_webView; // not owned

        CPeriodic* m_pageOverviewScrollPeriodic;

        TPointerEvent m_lastPointerEvent;
        TPointerEvent m_currentPointerEvent;
        TPoint m_lastPosition;
        TPoint m_currentPosition;
        TPoint m_focalPoint;
        bool m_touchScrolling; // in page overview mode
        bool m_isXScrollable;
        bool m_isYScrollable;
        TTime m_lastMoveEventTime;
        TTime m_pageOverviewEventTime;
        WebScrollingDecelerator* m_decel;
	WebScrollingDeceleratorGH* m_decelGH; 
        TPointerEvent m_lastDragEvent;
        CPeriodic* m_scrollTimer;
        ScrollableView m_scrollableView;
        TPoint m_currentNormalizedPosition; //normalized position
        enum ScrollDirectionState m_scrollDirectionState;        
        WebScrollbarDrawer* m_scrollbarDrawer;

    };

#endif      // WEBPAGESCROLLHANDLER_H

// End of File
