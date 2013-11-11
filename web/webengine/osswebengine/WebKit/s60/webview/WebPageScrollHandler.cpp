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
* Description:   Implementation of drag scrolling
*
*/


// INCLUDE FILES
#include <Browser_platform_variant.hrh>
#include <../bidi.h>
#include "WebPageScrollHandler.h"
#include "BrCtl.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebView.h"
#include <AknUtils.h>
#include "Page.h"
#include "Frame.h"
#include "WebCoreFrameBridge.h"
#include "FrameView.h"
#include "FocusController.h"
#include "PlatformScrollbar.h"
#include "WebScrollbarDrawer.h"
#include "RenderObject.h"
#include "WebScrollingDeceleratorGH.h"

#include "WebKitLogger.h"
using namespace WebCore;
using namespace RT_GestureHelper;
// constants
const int KPageOverviewScrollPeriodic = 20 * 1000; // Update frequently for faster, smoother scrolling
const int KMicroInterval = 300000;
const int KPageOverviewScrollStart = 1000;
const int KCancelDecelerationTimeout = 200000; //Decelerate only if flicked KCancelDecelerationTimeout microsec after last drag event.

const int KScrollIntervalTimeout = 40000; // scroll timer interval in microseconds
const int KAngularDeviationThreshold = 160; // % deviation ignored from minor axis of motion(120 means 20 %)
const int KScrollThresholdPixels = 10; // scrolls only if delta is above this threshold
const int KScrollDirectionBoundary = 30; // Bound around focal point within which scrolling locks in X or Y states
const float KTanOfThresholdAngle = 0.46; // tan of 25 degree

int handleScrollTimerEventCallback( TAny* ptr);

// ============================= LOCAL FUNCTIONS ===============================
int WebPageScrollHandler::pageOverviewScrollCallback( TAny* aPtr )
{
#ifdef BRDO_USE_GESTURE_HELPER
    static_cast<WebPageScrollHandler*>(aPtr)->scrollPageOverviewGH();
#else
    static_cast<WebPageScrollHandler*>(aPtr)->handlePageOverviewScrollCallback();
#endif //BRDO_USE_GESTURE_HELPER
    return KErrNone;
}
    
int handleScrollTimerEventCallback( TAny* ptr)
{
    WebPageScrollHandler* scrollHandler = static_cast<WebPageScrollHandler*>(ptr);
    scrollHandler->scrollContent();
    return EFalse;    
}      

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ScrollableView::contentPos
// 
// -----------------------------------------------------------------------------
//
TPoint ScrollableView::contentPos()
{
    WebFrameView* fv = activeFrameView();
    if (fv) return fv->contentPos();
    return TPoint (0,0);
}

WebFrameView* ScrollableView::activeFrameView()
{
    if (m_scrollingElement) {
        return kit(m_scrollingElement->document()->frame())->frameView();
    }
    else {
        return m_frameView;
    }
}


// -----------------------------------------------------------------------------
// WebPageScrollHandler::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPageScrollHandler* WebPageScrollHandler::NewL(WebView& webView)
    {
    WebPageScrollHandler* self = new (ELeave) WebPageScrollHandler( webView );
    CleanupStack::PushL(self);
    self->constructL();
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// WebPageScrollHandler::WebPageScrollHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebPageScrollHandler::WebPageScrollHandler(WebView& webView)
: m_webView( &webView ), m_decel(0), m_decelGH(NULL)
    {
    }

// -----------------------------------------------------------------------------
// WebPageScrollHandler::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPageScrollHandler::constructL()
    {
        m_scrollTimer = CPeriodic::NewL(CActive::EPriorityUserInput - 1);
        m_pageOverviewScrollPeriodic = CPeriodic::NewL(CActive::EPriorityUserInput - 1);
        m_lastPosition = TPoint(0, 0);
        m_scrollbarDrawer = WebScrollbarDrawer::NewL();
        if(AknLayoutUtils::PenEnabled()) {
            m_touchScrolling = true;
            m_scrollDirectionState = ScrollDirectionUnassigned;
            
#ifndef BRDO_USE_GESTURE_HELPER            
            m_decel = WebScrollingDecelerator::NewL(*m_webView);
#else            
	        m_decelGH = WebScrollingDeceleratorGH::NewL(*m_webView);
#endif	    
        }
        else  {
            m_touchScrolling = false;            
        }
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
WebPageScrollHandler::~WebPageScrollHandler()
{    
    
    if (m_scrollTimer) {
        m_scrollTimer->Cancel();
        delete m_scrollTimer;    
    }

    if (m_pageOverviewScrollPeriodic) {
        m_pageOverviewScrollPeriodic->Cancel();
        delete m_pageOverviewScrollPeriodic;
    }

    delete m_decel;
    delete m_decelGH;
    delete m_scrollbarDrawer;
}
    
void WebPageScrollHandler::handlePageOverviewScrollingL(const TPointerEvent& pointerEvent)
{
    switch (pointerEvent.iType){
        case TPointerEvent::EButton1Down:            
            m_webView->setViewIsScrolling(false);
            m_lastPointerEvent.iPosition.SetXY(0,0);
            m_lastDragEvent = pointerEvent;
            break;

        case TPointerEvent::EDrag:
            if (!m_webView->viewIsScrolling()){
                m_webView->setViewIsScrolling(true);
                m_pageOverviewScrollPeriodic->Start( 0, KPageOverviewScrollPeriodic, TCallBack(&pageOverviewScrollCallback, this));
            }
            break;

        case TPointerEvent::EButton1Up:
            if (m_pageOverviewScrollPeriodic->IsActive()){ 
                m_pageOverviewScrollPeriodic->Cancel();
            }
            m_webView->closePageView();
            m_webView->setViewIsScrolling(false);
            break;

        default:
            break;
   }
  
   // Scroll here only for EButton1Up and EButton1Down if needed
   if(pointerEvent.iType != TPointerEvent::EDrag && m_lastPointerEvent.iPosition != pointerEvent.iPosition){
       scrollPageOverview( pointerEvent );
   }
   m_lastPointerEvent = pointerEvent; 
}

void WebPageScrollHandler::handlePageOverviewScrollCallback()
{
    int absX = Abs( m_lastDragEvent.iPosition.iX - m_lastPointerEvent.iPosition.iX);
    int absY = Abs( m_lastDragEvent.iPosition.iY - m_lastPointerEvent.iPosition.iY);

   // Scrolling for EDrag events

    if( absX > 3 ||  absY > 3){
        scrollPageOverview( m_lastPointerEvent );
        m_lastDragEvent = m_lastPointerEvent;
    }
}


void WebPageScrollHandler::scrollPageOverview(const TPointerEvent& pointerEvent)
{

    TRect indicatorRect = m_webView->pageScaler()->IndicatorRect();
    TInt zoomLevel = m_webView->pageScaler()->ZoomOutLevel();
    TPoint currentPosition = m_webView->mainFrame()->frameView()->contentPos();
    TInt xInDoc = ((pointerEvent.iPosition.iX - indicatorRect.iTl.iX  - indicatorRect.Width() / 2) * zoomLevel ) / 100 + currentPosition.iX;
    TInt yInDoc = ((pointerEvent.iPosition.iY - indicatorRect.iTl.iY  - indicatorRect.Height() / 2) * zoomLevel ) / 100 + currentPosition.iY;
           
    m_webView->mainFrame()->frameView()->scrollTo(TPoint(xInDoc, yInDoc));    

}

void WebPageScrollHandler::updateScrolling(const TPointerEvent& pointerEvent)
{
    switch (pointerEvent.iType)
    {
        case TPointerEvent::EButton1Down:
        {        
            m_lastMoveEventTime = 0; 
            m_lastPosition = pointerEvent.iPosition;
            m_currentPosition = pointerEvent.iPosition;
            m_webView->setViewIsScrolling(false);         
            // stop deceleration scrolling cycle            
            if (m_decel) {
                m_decel->stopDecelL();    
            }
            m_scrollableView.m_scrollingElement = NULL;
            m_scrollableView.m_frameView = NULL;
            break;
        }
        case TPointerEvent::EButton1Up:
        {
            m_scrollTimer->Cancel();
            if (m_scrollableView.m_scrollingElement) {
                clearScrollingElement();
            }
            else {
                if (m_lastMoveEventTime != 0)
                {           
                    // Start deceleration only if the delta since last drag event is less than threshold
                    TTime now;
                    now.HomeTime();             
                    TTimeIntervalMicroSeconds ms = now.MicroSecondsFrom(m_lastMoveEventTime);
                    if (ms < KCancelDecelerationTimeout) {
                        m_decel->startDecelL();    
                    }
                }            
                if (m_webView->viewIsScrolling()) {
                    Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
                    frame->bridge()->sendScrollEvent();                            
                }
            }
            m_scrollbarDrawer->fadeScrollbar();
            m_scrollDirectionState = ScrollDirectionUnassigned;
            m_lastMoveEventTime = 0;
            break;
        }
    }
}

void WebPageScrollHandler::setupScrolling(const TPoint& aNewPosition)
{
    if (m_lastPosition == TPoint(0, 0)) {
        m_lastPosition = aNewPosition;
    }
   
    if(m_lastPosition == aNewPosition)
        return; // no displacement -- means no need for scrolling    

    //Ignore move events until they jump the threshold (avoids jittery finger effect) 
    TInt absX = Abs( aNewPosition.iX - m_lastPosition.iX);
    TInt absY = Abs( aNewPosition.iY - m_lastPosition.iY);
    if( absX < KScrollThresholdPixels &&  absY < KScrollThresholdPixels)
        return;
            
    if (calculateScrollableFrameView(aNewPosition))  {

        // normalize current position to minimize cumulative rounding error 
        m_currentNormalizedPosition.iX = m_scrollableView.contentPos().iX * 100;
        m_currentNormalizedPosition.iY = m_scrollableView.contentPos().iY * 100;
        // do the scrolling in a time out
        m_scrollTimer->Cancel();                   
        m_scrollTimer->Start( 0, KScrollIntervalTimeout, TCallBack(&handleScrollTimerEventCallback,this));
        m_webView->setViewIsScrolling(true);
        m_webView->toggleRepaintTimer(false);
    }
}


void WebPageScrollHandler::clearScrollingElement()
{
    if (m_scrollableView.m_scrollingElement) {
        m_scrollableView.m_scrollingElement->deref();
        m_scrollableView.m_scrollingElement = NULL;
    }
}

void WebPageScrollHandler::scrollContent()
{
    TPoint scrollDelta = m_lastPosition - m_currentPosition;
    scrollContent(scrollDelta);
}

void WebPageScrollHandler::scrollContent(TPoint& aScrollDelta)
{
    if(!m_scrollableView.activeFrameView())
            return;
        
    
    int absX = Abs(aScrollDelta.iX);
    int absY = Abs(aScrollDelta.iY);
                                       
    if(absX || absY) //move only if necessary
    {      
        // calculate which direction we are trying to scroll
        if(m_scrollDirectionState == ScrollDirectionUnassigned) {
            m_focalPoint = m_currentPosition;
            calculateScrollDirection(absX, absY);
        }   
        
        switch (m_scrollDirectionState)
        {           
            case ScrollDirectionX: //scroll in X dir
            {
                aScrollDelta.iY = 0;
                aScrollDelta.iX *= 100;
                //Fallback to XY state if the current position is out of bounds                
                TPoint boundaryCheckpoint = m_focalPoint - m_currentPosition;                                
                if(Abs(boundaryCheckpoint.iY) > KScrollDirectionBoundary)
                    m_scrollDirectionState = ScrollDirectionXY;
                break;
            }
            case ScrollDirectionY: //scroll in Y dir
            {                
                aScrollDelta.iX = 0;
                aScrollDelta.iY *= 100;
                //Fallback to XY state if the current position is out of bounds                
                TPoint boundaryCheckpoint = m_focalPoint - m_currentPosition;                                
                if(Abs(boundaryCheckpoint.iX) > KScrollDirectionBoundary)
                    m_scrollDirectionState = ScrollDirectionXY;
                break;
            }
            case ScrollDirectionXY: //scroll in XY
            {
                aScrollDelta.iX *= 100;
                aScrollDelta.iY *= 100;
                m_scrollDirectionState = ScrollDirectionUnassigned;
                break;
            }
        }        
        if (m_scrollableView.m_scrollingElement) {
            bool shouldScrollVertically = false;
            bool shouldScrollHorizontally = false;
            WebFrame* frame = kit(m_scrollableView.m_scrollingElement->document()->frame());
            RenderObject* render = m_scrollableView.m_scrollingElement->renderer();
            __ASSERT_DEBUG(render->isScrollable(), User::Panic(_L(""), KErrGeneral));
            if (aScrollDelta.iY)
                shouldScrollVertically = !render->scroll(ScrollDown, ScrollByPixel, frame->frameView()->toDocCoords(aScrollDelta).iY / 100);
            if (aScrollDelta.iX)
                shouldScrollHorizontally = !render->scroll(ScrollRight, ScrollByPixel, frame->frameView()->toDocCoords(aScrollDelta).iX / 100);

            TPoint scrollPos = frame->frameView()->contentPos();
            TPoint newscrollDelta = frame->frameView()->toDocCoords(aScrollDelta);
            m_currentNormalizedPosition +=  newscrollDelta;     

            if (shouldScrollHorizontally) {
                scrollPos.iX = m_currentNormalizedPosition.iX/100;
            }
            if (shouldScrollVertically) {
                scrollPos.iY = m_currentNormalizedPosition.iY/100;
            }
            
            
            if (shouldScrollVertically || shouldScrollHorizontally){
                if (m_scrollableView.m_frameView->needScroll(scrollPos)) {
                    frame->frameView()->scrollTo(scrollPos);
                    updateScrollbars(scrollPos, newscrollDelta);
                    core(frame)->sendScrollEvent();
                }
            }
            else {
                m_webView->syncRepaint();
            }
            m_lastPosition = m_currentPosition;
            m_currentNormalizedPosition.iX = frame->frameView()->contentPos().iX * 100;
            m_currentNormalizedPosition.iY = frame->frameView()->contentPos().iY * 100;
        }
        else {
            TPoint scrollPos;
            TPoint newscrollDelta = m_scrollableView.m_frameView->toDocCoords(aScrollDelta);
            m_currentNormalizedPosition +=  newscrollDelta;  
            scrollPos.iX = m_currentNormalizedPosition.iX/100;
            scrollPos.iY = m_currentNormalizedPosition.iY/100;
            TPoint cpos = m_scrollableView.m_frameView->contentPos();
           
            if(!m_scrollableView.m_frameView->needScroll(scrollPos)) {
                m_scrollDirectionState = ScrollDirectionUnassigned;
                m_lastPosition = m_currentPosition;
                m_currentNormalizedPosition.iX = m_scrollableView.contentPos().iX * 100;
                m_currentNormalizedPosition.iY = m_scrollableView.contentPos().iY * 100;
            }
            else {
          
                m_scrollableView.m_frameView->scrollTo(scrollPos);
                m_lastPosition = m_currentPosition;
#ifndef BRDO_USE_GESTURE_HELPER                
                m_decel->updatePos();
#endif                
            // update scroll bars
                updateScrollbars(scrollPos, newscrollDelta);
            }
        }
    }
} 

bool WebPageScrollHandler::calculateScrollableFrameView(const TPoint& aNewPosition)
{
    calculateScrollableElement(aNewPosition);
    
    //First figure out the direction we are scrolling    
    bool x_r = false;
    bool x_l = false;
    bool y_t = false;
    bool y_b = false;

    //find the x direction
    int x_delta = aNewPosition.iX - m_lastPosition.iX;
    if (x_delta > 0) {
        x_l = true;                
    } else if (x_delta < 0){
        x_r = true;
    }

    //find the y direction
    int y_delta = aNewPosition.iY - m_lastPosition.iY;
    if (y_delta > 0) {
        y_t = true;                
    } else if (y_delta < 0){
        y_b = true;
    }
        
    m_scrollableView.m_frameView = NULL;
        
    //find the frame that can scroll if we can't scroll check the parents        
    WebFrame* frame = m_webView->mainFrame()->frameAtPoint(aNewPosition);
    for (; frame; frame = frame->parentFrame())  {
    
        // Dont scroll a frameset
        if (frame->parentFrame() && frame->parentFrame()->isFrameSet()) 
            continue;     
        
        TPoint contentpos = frame->frameView()->contentPos();
        TSize contentsize = frame->frameView()->contentSize();        
        TRect framerect = frame->frameView()->rect();        
        
        //if the content width is > frameview width
        if (contentsize.iWidth > framerect.Width() && frame->frameView()->hScrollbar()->isEnabled()) {
            //IF we are trying to scroll to the right we need 
            //to see if the content position < content size - frameview size        
            //ELSE if we are trying to the left we need 
            //to see if the content position > 0 
            if (x_r) {        
                if (contentpos.iX < (contentsize.iWidth - framerect.Width())) {
                    m_scrollableView.m_frameView = frame->frameView();
                    return true;                
                }

            } else if (x_l) {
                if (contentpos.iX > 0) {
                    m_scrollableView.m_frameView = frame->frameView();
                    return true;                
                }            
            }
        }
        
        //if the content height is > frameview height
        if (contentsize.iHeight > framerect.Height() && frame->frameView()->vScrollbar()->isEnabled()) {
            //IF we are trying to scroll down we need 
            //to see if the content position < content size - frameview size        
            //ELSE if we are trying to scroll up we need 
            //to see if the content position > 0 
            if (y_b) {        
                if (contentpos.iY < (contentsize.iHeight - framerect.Height())) {
                    m_scrollableView.m_frameView = frame->frameView();
                    return true;                
                }

            } else if (y_t) {
                if (contentpos.iY > 0) {
                    m_scrollableView.m_frameView = frame->frameView();
                    return true;                
                }            
            }
        }
    }
    
    //could not find a scrollable frame        
    m_scrollableView.m_frameView  = m_webView->mainFrame()->frameView();
    return true;
   
}

void WebPageScrollHandler::calculateScrollDirection(int absX, int absY)
{
     //assign scroll direction state according to scroll angle
    if((absX * KAngularDeviationThreshold) < (absY * 100))
        m_scrollDirectionState = ScrollDirectionY;
    else if((absY * KAngularDeviationThreshold) < (absX * 100))
        m_scrollDirectionState = ScrollDirectionX;
    else
        m_scrollDirectionState = ScrollDirectionXY;    
}

void WebPageScrollHandler::updateScrollbars(const TPoint& scrollPos, TPoint& newscrollDelta)
{
    WebFrameView* fv = m_scrollableView.activeFrameView();
    if (fv) {
        
        if (fv->vScrollbar()) {
            fv->vScrollbar()->setValue(scrollPos.iY);
        }

        if (fv->hScrollbar()) {
            fv->hScrollbar()->setValue(scrollPos.iX);
        }
        
        if ((fv->frame() == m_webView->mainFrame())) {
          m_scrollbarDrawer->drawScrollbar(m_webView, newscrollDelta);
        }
    }
}
         

bool WebPageScrollHandler::calculateScrollableElement(const TPoint& aNewPosition)
{
    WebFrame* frame = m_webView->mainFrame()->frameAtPoint(aNewPosition);
    if( !frame ) return false;
    TPoint pt = frame->frameView()->viewCoordsInFrameCoords(aNewPosition);
    Element* e = core(frame)->document()->elementFromPoint(pt.iX, pt.iY);
    Element* currElement = NULL;
    if(!e) return NULL;
    RenderObject* render = e->renderer();
    if (render && render->isScrollable()) {
        RenderLayer* layer = render->enclosingLayer();
        Element* parent = e;
        currElement = e;
        while (!currElement->isControl() && parent && parent->renderer() && parent->renderer()->enclosingLayer() == layer) {
            currElement = parent;
            Node* pn = parent;
            do {
                pn = pn->parent();
            } while (pn && !pn->isElementNode());
            parent = static_cast<Element*>(pn);
        }
        if (currElement) {
            currElement->ref();
            m_scrollableView.m_scrollingElement = currElement; 
            m_scrollableView.m_frameView = NULL;
            return true;
        }
    }
    return false;
}


void WebPageScrollHandler::scrollPageOverviewGH()
{

    TRect indicatorRect = m_webView->pageScaler()->IndicatorRect();
    TInt zoomLevel = m_webView->pageScaler()->ZoomOutLevel();
    TPoint currentPosition = m_webView->mainFrame()->frameView()->contentPos();
    TInt xInDoc = ((m_currentPosition.iX - indicatorRect.iTl.iX  - indicatorRect.Width() / 2) * zoomLevel ) / 100 + currentPosition.iX;
    TInt yInDoc = ((m_currentPosition.iY - indicatorRect.iTl.iY  - indicatorRect.Height() / 2) * zoomLevel ) / 100 + currentPosition.iY;
           
    m_webView->mainFrame()->frameView()->scrollTo(TPoint(xInDoc, yInDoc));    

}


void WebPageScrollHandler::handleScrollingGH(const TGestureEvent& aEvent)
{   
    TPoint newPos = aEvent.CurrentPos();
    m_currentPosition = newPos;
    if (m_webView->inPageViewMode()) {
        if (!m_pageOverviewScrollPeriodic->IsActive()){
            m_pageOverviewScrollPeriodic->Start( 0, KPageOverviewScrollPeriodic, 
                                                TCallBack(&pageOverviewScrollCallback, this));
            m_webView->setViewIsScrolling(true);
            m_webView->toggleRepaintTimer(false);
        }
    }
    else if (!m_webView->viewIsScrolling()) {
        setupScrolling(newPos);    
    }
}


void WebPageScrollHandler::handleTouchDownGH(const TGestureEvent& aEvent)
{
    TPoint newPos = aEvent.CurrentPos();
    m_lastMoveEventTime = 0; 
    m_lastPosition = newPos;
    m_currentPosition = newPos;
    m_webView->setViewIsScrolling(false);      
    m_webView->toggleRepaintTimer(true);
            
    if (m_decelGH) {
        m_decelGH->cancelDecel();
    }
    m_scrollableView.m_scrollingElement = NULL;
    m_scrollableView.m_frameView = NULL;
}


void WebPageScrollHandler::handleTouchUpGH(const TGestureEvent& aEvent)
{
    bool decelDoesScrollbars = false;
    TPoint newPos = aEvent.CurrentPos();

    if (m_webView->inPageViewMode()) {
        if (m_pageOverviewScrollPeriodic->IsActive()){ 
            m_pageOverviewScrollPeriodic->Cancel();
        }
        m_webView->closePageView();
        scrollPageOverviewGH();
        m_webView->setViewIsScrolling(false);
        m_webView->toggleRepaintTimer(true);
    }
    else {
        m_scrollTimer->Cancel();
        m_lastPosition = TPoint(0, 0);
        decelDoesScrollbars = startDeceleration(aEvent);
                    
        if (m_webView->viewIsScrolling()) {
            Frame* frame = m_webView->page()->focusController()->focusedOrMainFrame();
            frame->bridge()->sendScrollEvent();                            
        }
    
        if (!decelDoesScrollbars) {
            m_scrollbarDrawer->fadeScrollbar();
            m_webView->setViewIsScrolling(false);
            m_webView->toggleRepaintTimer(true);
        }
        m_scrollDirectionState = ScrollDirectionUnassigned;
        m_lastMoveEventTime = 0;
    }
}


bool WebPageScrollHandler::startDeceleration(const TGestureEvent& aEvent)
{
    bool started = false;
    TRealPoint gstSpeed = aEvent.Speed();
    if (Abs(gstSpeed.iX / gstSpeed.iY) <= KTanOfThresholdAngle) {
       gstSpeed.iX = 0;
    }
    
    if (Abs(gstSpeed.iY / gstSpeed.iX) <= KTanOfThresholdAngle) { 
       gstSpeed.iY = 0;
    }
    
    if ((Abs(gstSpeed.iX) > 0) || (Abs(gstSpeed.iY) > 0)) {
       m_decelGH->startDecel(gstSpeed, m_scrollbarDrawer); 
       started = true;
    }
    
    
    return started;
}

//  End of File
