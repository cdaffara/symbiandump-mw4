/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  History View
*
*/



// INCLUDE FILES
#include <../bidi.h>
#include    "HistoryView.h"
#include    "HistoryController.h"
#include    "HistoryEntry.h"
#include    <coemain.h>
#include    "WebSurface.h"
#include "eikon.hrh"


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
const int KHistoryViewOffsetX = 2;
const int KAutoScrollPeriodic = 300 * 1000;

//Animation related factors
const int KResizeFactorIncr = 20;
const int KAnimFactorInc = 25;
const int KDragDelta = 7;
const int KFastScrollAnimFactorInc = 40;
const int KMaxAnimFactor = 100;
const int KMinAnimFactor = 0;
const int KMinimumScroll = 20;

#define KCenterImageBorderColor KRgbRed
#define KSideImageBorderColor KRgbBlack

// ============================= LOCAL FUNCTIONS ===============================

TBool animRepaint(TAny* aAny)
{
    HistoryView* historyView = static_cast<HistoryView*> (aAny);
    bool result = historyView->calcRepaintRect();
    return result;
}

TBool animPlaceHolderRepaint(TAny* aAny)
{
    HistoryView* historyView = static_cast<HistoryView*> (aAny);
    bool result = historyView->animatePlaceHolderPosition();
    return result;
}

TInt autoScrollCallback( TAny* aPtr )
{
    static_cast<HistoryView*>(aPtr)->autoScroll();
    return KErrNone;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HistoryView::HistoryView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
HistoryView::HistoryView(HistoryController& historyController, bool previous)
    : m_historyController( &historyController ),
    m_resizeFactor (KMinAnimFactor),
    m_placeHolderResizeFactor(KMinAnimFactor)
{
    m_historyStackIndex = historyController.currentIndex();
    m_centerPageIndex = m_historyStackIndex;
    if (previous) {
        // we can assume that index is greater than 1 if ShowView
        // is invoked as a result of "Back" operation
        m_centerPageIndex--;
    }
}

// -----------------------------------------------------------------------------
// ?HistoryView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void HistoryView::ConstructL()
{
    CCoeControl* parent = m_historyController->historyCallback()->parent();
    SetContainerWindowL(*parent);
    m_repaintTimer = CIdle::NewL( CActive::EPriorityHigh );
    // rendering surface
    m_renderTarget = m_historyController->historyCallback()->surface();
    // Create an offscreen device and context
    m_bitmapDevice = CFbsBitmapDevice::NewL( m_renderTarget->offscreenBitmap() );
    User::LeaveIfError(m_bitmapDevice->CreateContext(m_bitmapContext));
    SetRect(parent->Rect());
    m_repaintTimer->Start(TCallBack(&animRepaint,this));
}


// -----------------------------------------------------------------------------
// HistoryView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
HistoryView* HistoryView::NewL(HistoryController& historyController, bool previous)
{
    HistoryView* self = new( ELeave ) HistoryView(historyController, previous);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


// Destructor
HistoryView::~HistoryView()
{
    delete m_bitmapDevice;
    delete m_bitmapContext;
    if (m_repaintTimer->IsActive()) {
        m_repaintTimer->Cancel();
    }
    delete m_repaintTimer;
    if (m_autoScrollPeriodic) {
        m_autoScrollPeriodic->Cancel();
        delete m_autoScrollPeriodic;
    }
}

bool HistoryView::calcRepaintRect()
{
    if (m_resizeFactor == KMaxAnimFactor) {
        m_repaintRect.SetRect(Rect().iTl,Rect().Size());
        m_historyController->updateDisplay();
        return false;
    }
    m_repaintRect.SetRect(Rect().iTl,TSize(Rect().Width()*m_resizeFactor/100,Rect().Height()*m_resizeFactor/100));
    m_resizeFactor += KResizeFactorIncr;
    m_historyController->updateDisplay();
    return true;
}

// -----------------------------------------------------------------------------
// HistoryView::OfferKeyEventL
// Processes Key Event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse HistoryView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode )
{
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    m_fastScroll = false;
    // Determine the key event and handle it
    switch (aKeyEvent.iCode)
        {
        // The arrow key events
        case EKeyRightUpArrow:        // Northeast
        case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyRightArrow:          // East
        case EKeyRightDownArrow:      // Southeast
        case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
            {
                if (aKeyEvent.iRepeats>0) {
                    m_fastScroll = true;
                }
                int historyLength = m_historyController->historyLength();
                if (m_centerPageIndex < ( historyLength -1 )) {
                    m_direction = 1;
                    //Start Animation
                    if (m_repaintTimer->IsActive()) {
                        m_repaintTimer->Cancel();
                        m_resizeFactor = KMaxAnimFactor;
                    }
                    m_repaintTimer->Start(TCallBack(&animPlaceHolderRepaint,this));
                    keyResponse = EKeyWasConsumed;
                }
                break;
            }

        case EKeyLeftUpArrow:         // Northwest
        case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyLeftArrow:           // West
        case EKeyLeftDownArrow:       // Southwest
        case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
            {
                if (aKeyEvent.iRepeats>0) {
                    m_fastScroll = true;
                }
                if (m_centerPageIndex > 0) {
                    m_direction = -1;
                    //Start Animation
                    if (m_repaintTimer->IsActive()) {
                        m_repaintTimer->Cancel();
                        m_resizeFactor = KMaxAnimFactor;
                    }
                    m_repaintTimer->Start(TCallBack(&animPlaceHolderRepaint,this));
                    keyResponse = EKeyWasConsumed;
                }
                break;
            }


            // The enter key events. aka activate key:

        case EKeyEnter:
        case EKeyDevice3:
            {
                //history
                int index = m_historyController->currentIndex();
                HistoryEntry* entryCenter = m_historyController->entryByIndex( m_centerPageIndex );
                HistoryEntry* entryCurrent = m_historyController->entryByIndex( index );
                if (entryCenter && entryCenter->ifWml()) {   // look at last known history stack info if WML
                    m_historyController->setPossibleWmlOEB( false );
                    if (m_centerPageIndex < index ) {
                        m_historyController->setPossibleWmlOEB( true );  // this is always a possibility when navigating back to wml from html
                    }
                    if ( entryCurrent && entryCurrent->ifWml() == false || ((m_centerPageIndex != index) && ((m_centerPageIndex -1 ) != index) && ((m_centerPageIndex + 1) != index)) ) {
                        loadUrl();
                    }
                    else {
                        m_historyController->historyCallback()->handleWmlBackL();
                    }
                }
                else {
                    if (m_centerPageIndex != index)
                    {
                    	loadUrl();	
                    }
                }
                keyResponse = EKeyWasConsumed;
                m_historyController->closeHistoryView();
                break;
            }

        default:
            break;
        }
    return keyResponse;
}


// -----------------------------------------------------------------------------
// HistoryView::updateDisplay
// Processes Key Event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void HistoryView::updateDisplay()
{
    m_bitmapContext->Clear();
    m_bitmapContext->Reset();

    // updateDisplay something to the bitmap
    m_centerEntry = m_historyController->entryByIndex( m_centerPageIndex );
    HistoryEntry* leftEntry = m_historyController->entryByIndex( m_centerPageIndex-1 );
    HistoryEntry* rightEntry = m_historyController->entryByIndex( m_centerPageIndex+1 );
    if (leftEntry) {
        if (!leftEntry->thumbnail()) {
            TRAP_IGNORE(leftEntry->constructThumbnailL());
        }
        if (leftEntry->thumbnail()) {
            m_bitmapContext->BitBlt( m_leftPlaceHolderRect.iTl, leftEntry->thumbnail(), TRect(TPoint(0,0), m_leftPlaceHolderRect.Size()) );
        }
        m_bitmapContext->SetPenColor( KSideImageBorderColor );
        m_bitmapContext->DrawRect(m_leftPlaceHolderRect);
    }
    // updateDisplay the next thumbnail
    if (rightEntry) {
        if (!rightEntry->thumbnail()) {
            TRAP_IGNORE(rightEntry->constructThumbnailL());
        }
        if (rightEntry->thumbnail()) {
            m_bitmapContext->BitBlt( m_rightPlaceHolderRect.iTl, rightEntry->thumbnail(), TRect(TPoint(0,0), m_rightPlaceHolderRect.Size()) );
        }
        m_bitmapContext->SetPenColor( KSideImageBorderColor );
        m_bitmapContext->DrawRect(m_rightPlaceHolderRect);
    }
    // updateDisplay the center thumbnail
    if (m_centerEntry ) {
        if (!m_centerEntry->thumbnail()) {
            TRAP_IGNORE(m_centerEntry->constructThumbnailL());
        }
        if (m_centerEntry->thumbnail()) {
            m_bitmapContext->BitBlt( m_centerPlaceHolderRect.iTl, m_centerEntry->thumbnail(), TRect(TPoint(0,0), m_centerPlaceHolderRect.Size()) );
        }
    }
    m_bitmapContext->SetPenColor( KCenterImageBorderColor );

    m_bitmapContext->DrawRect( m_centerPlaceHolderRect );

    // Reset Brush
    m_bitmapContext->SetBrushColor(TRgb(255,255,255));
    m_bitmapContext->SetBrushStyle(CGraphicsContext::ENullBrush);
  // updateState(-1);
}

// ----------------------------------------------------------------------------
// HistoryView::getCenterEntryTitle
//
// return page title of center entry
// ----------------------------------------------------------------------------
TPtrC HistoryView::getCenterEntryTitle()
{
    if (m_direction == 0) {
        if ( m_centerEntry ) {
            return m_centerEntry->pageTitle();
        }
        return KNullDesC();
    }
    if (m_repaintTimer->IsActive()) {
        if (m_direction == 1) {
            return m_historyController->entryByIndex( m_centerPageIndex + 1)->pageTitle();
        }
        else {
            return m_historyController->entryByIndex( m_centerPageIndex - 1)->pageTitle();
        }
    }
    return TPtrC();
}


// ----------------------------------------------------------------------------
// HistoryView::Draw
//
// Draws the bitmap
// for the viewable content.
// ----------------------------------------------------------------------------
void HistoryView::Draw(const TRect& ) const
{
    CWindowGc& gc = SystemGc();
    if (m_resizeFactor!=KMaxAnimFactor) {
        gc.DrawBitmap(m_repaintRect,m_renderTarget->offscreenBitmap());
        gc.DrawRect(Rect());
    }
    else {
        // put offscreen bitmap onto the screen
        gc.BitBlt( m_offscreenRect.iTl, m_renderTarget->offscreenBitmap());
    }
}

// ----------------------------------------------------------------------------
// HistoryView::SetRect
// public Class Method
// Updates the size of the offscreen bitmap  to improve performance.
// ----------------------------------------------------------------------------
void HistoryView::SizeChanged()
{
    if ( m_offscreenRect != Rect() ) {
        // Set the Offscreen rect adjusted for scrollbars
        m_offscreenRect = Rect();
        // need to resize the bitmap...
        updateOffscreenBitmapL();
        calculateLayout();
        updateDisplay();
        MakeVisible(true);
    }
}

void HistoryView::calculateLayout()
{
    TSize screenSize = m_offscreenRect.Size();
    TSize viewSize((screenSize.iWidth -2*KHistoryViewOffsetX), screenSize.iHeight);
    int centerImageWidth(viewSize.iWidth*KCenterThumbnailWidthPercent/100);
    int centerImageHeight(viewSize.iHeight*KCenterThumbnailHeightPercent/100);
    TSize centerImageSize(centerImageWidth, centerImageHeight);

    int sideImageWidth(viewSize.iWidth*KSideThumbnailWidthPercent/100);
    int sideImageHeight(viewSize.iHeight*KSideThumbnailHeightPercent/100);
    TSize sideImageSize(sideImageWidth, sideImageHeight);

    int centerImageX(KHistoryViewOffsetX+sideImageWidth);
    int centerImageY(viewSize.iHeight*(100-KCenterThumbnailHeightPercent)/(2*100));
    TPoint centerImageOrigin(centerImageX, centerImageY);

    m_centerPlaceHolderRect.SetRect(centerImageOrigin, centerImageSize);
    int sideImageY(viewSize.iHeight*(100-KSideThumbnailHeightPercent)/(2*100));
    TPoint leftThumbnailOrigin( KHistoryViewOffsetX, sideImageY );
    TPoint rightThumbnailOrigin( (int)(KHistoryViewOffsetX+sideImageWidth+centerImageWidth), sideImageY );

    m_leftPlaceHolderRect.SetRect(leftThumbnailOrigin,sideImageSize);
    m_rightPlaceHolderRect.SetRect(rightThumbnailOrigin,sideImageSize);

    int xPos(m_centerPlaceHolderRect.iBr.iX - m_centerPlaceHolderRect.Size().iWidth/2);
    int yPos(m_centerPlaceHolderRect.iBr.iY);
    int dH(Rect().iBr.iY - m_centerPlaceHolderRect.iBr.iY - 6);

    m_leftArrow[0] = TPoint( KHistoryViewOffsetX + dH, yPos + 3 );
    m_leftArrow[1] = TPoint( KHistoryViewOffsetX + dH, yPos + dH );
    m_leftArrow[2] = TPoint( m_leftArrow[0].iX - (m_leftArrow[1].iY-m_leftArrow[0].iY), (m_leftArrow[0].iY + m_leftArrow[1].iY)/2);

    m_rightArrow[0] = TPoint( viewSize.iWidth - KHistoryViewOffsetX - dH, yPos + 3 );
    m_rightArrow[1] = TPoint( viewSize.iWidth - KHistoryViewOffsetX - dH, yPos + dH );
    m_rightArrow[2] = TPoint( m_rightArrow[0].iX+(m_rightArrow[1].iY-m_rightArrow[0].iY), (m_rightArrow[0].iY+m_rightArrow[1].iY)/2 );

}

bool HistoryView::animatePlaceHolderPosition()
{
    if (m_placeHolderResizeFactor >= KMaxAnimFactor) {
        //Restore old values and end AO
        if (m_direction == 1) {
            m_centerPageIndex++;
            // delete thumbnail going out of the view
            HistoryEntry* entry = m_historyController->entryByIndex( m_centerPageIndex-2 );
            if (entry) {
                entry->deleteThumbnail();
            }
        }
        if (m_direction == -1) {
            m_centerPageIndex--;
            // delete thumbnail going out of the view
            HistoryEntry* entry = m_historyController->entryByIndex( m_centerPageIndex+2 );
            if (entry) {
                entry->deleteThumbnail();
            }
        }
        m_placeHolderResizeFactor = 0;
        m_direction = 0;
        updateDisplay();
        m_historyController->updateDisplay();
        return false;
    }
    TRect rect[4];
    int dw = 0;
    int dh = 0;
    int dx = 0;
    //left movement
    if (m_direction == -1) {
        TPoint pt(m_leftPlaceHolderRect.iTl.iX,m_leftPlaceHolderRect.iTl.iY+ m_leftPlaceHolderRect.Height()/2);
        rect[0] = TRect(pt,TSize(0,0));
        dw = m_leftPlaceHolderRect.Width()*m_placeHolderResizeFactor/200;
        dh = m_leftPlaceHolderRect.Height()*m_placeHolderResizeFactor/200;
        rect[0].Grow(dw,dh);
        rect[0].Move(dw,0);

        rect[1] = m_leftPlaceHolderRect;
        dx = (m_centerPlaceHolderRect.iTl.iX - m_leftPlaceHolderRect.iTl.iX)*m_placeHolderResizeFactor/100;
        dw = (m_centerPlaceHolderRect.Width() - m_leftPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_centerPlaceHolderRect.Height() - m_leftPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[1].Grow(dw,dh);
        rect[1].Move(dx + dw,0);

        rect[2] = m_centerPlaceHolderRect;
        dx = (m_rightPlaceHolderRect.iTl.iX - m_centerPlaceHolderRect.iTl.iX)*m_placeHolderResizeFactor/100;
        dw = (m_centerPlaceHolderRect.Width() - m_rightPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_centerPlaceHolderRect.Height() - m_rightPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[2].Shrink(dw,dh);
        rect[2].Move(dx - dw,0);

        rect[3] = m_rightPlaceHolderRect;
        dw = (m_rightPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_rightPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[3].Shrink(dw,dh);
        rect[3].Move(2*dw,0);

        HistoryEntry* entry = 0;
        //Here we clear the rectangle where we are going to repaint
        m_bitmapContext->SetPenStyle(CGraphicsContext::ENullPen);
        m_bitmapContext->SetBrushColor(TRgb(255,255,255));
        m_bitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
        m_bitmapContext->DrawRect(TRect(TPoint(0,m_centerPlaceHolderRect.iTl.iY),TSize(Rect().Width(),m_centerPlaceHolderRect.Height())));
        for (int i = 0;i<4;i++)
        {
            entry = m_historyController->entryByIndex( m_centerPageIndex-2 + i );
            if (entry && entry->thumbnail())
            {
                TRect intersectRect(TPoint(0,0),m_renderTarget->offscreenBitmap()->SizeInPixels());
                intersectRect.Intersection(rect[i]);
                if (!intersectRect.IsEmpty())
                {
                    m_bitmapContext->BitBlt( intersectRect.iTl, entry->thumbnail(), TRect(TPoint(0,0), rect[i].Size()) );
                    m_bitmapContext->SetPenStyle(CGraphicsContext::ESolidPen);
                    m_bitmapContext->SetPenColor( KSideImageBorderColor );
                    m_bitmapContext->SetBrushStyle(CGraphicsContext::ENullBrush);
                    m_bitmapContext->DrawRect(intersectRect);
                }
            }
        }
    }

    if (m_direction == 1)
    {
        rect[0] = m_leftPlaceHolderRect;
        dw = m_leftPlaceHolderRect.Width()*m_placeHolderResizeFactor/200;
        dh = m_leftPlaceHolderRect.Height()*m_placeHolderResizeFactor/200;
        rect[0].Shrink(dw,dh);
        rect[0].Move(-2*dw,0);

        rect[1] = m_centerPlaceHolderRect;
        dx = (m_centerPlaceHolderRect.iTl.iX - m_leftPlaceHolderRect.iTl.iX)*m_placeHolderResizeFactor/100;
        dw = (m_centerPlaceHolderRect.Width() - m_leftPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_centerPlaceHolderRect.Height() - m_leftPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[1].Shrink(dw,dh);
        rect[1].Move(-dx-dw,0);

        rect[2] = m_rightPlaceHolderRect;
        dx = (m_rightPlaceHolderRect.iTl.iX - m_centerPlaceHolderRect.iTl.iX)*m_placeHolderResizeFactor/100;
        dw = (m_centerPlaceHolderRect.Width() - m_rightPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_centerPlaceHolderRect.Height() - m_rightPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[2].Grow(dw,dh);
        rect[2].Move(-dx + dw,0);

        TPoint pt(m_rightPlaceHolderRect.iTl.iX + m_rightPlaceHolderRect.Width(),m_rightPlaceHolderRect.iTl.iY+ m_rightPlaceHolderRect.Height()/2);
        rect[3] = TRect(pt,TSize(0,0));
        dw = (m_rightPlaceHolderRect.Width())*m_placeHolderResizeFactor/200;
        dh = (m_rightPlaceHolderRect.Height())*m_placeHolderResizeFactor/200;
        rect[3].Grow(dw,dh);
        rect[3].Move(-dw,0);

        HistoryEntry* entry = 0;
        //Here we clear the rectangle where we are going to repaint
        m_bitmapContext->SetPenStyle(CGraphicsContext::ENullPen);
        m_bitmapContext->SetBrushColor(TRgb(255,255,255));
        m_bitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
        m_bitmapContext->DrawRect(TRect(TPoint(0,m_centerPlaceHolderRect.iTl.iY),TSize(Rect().Width(),m_centerPlaceHolderRect.Height())));
        for (int i = 0;i<4;i++)
        {
            entry = m_historyController->entryByIndex( m_centerPageIndex- 1 +i);
            if (entry && !entry->thumbnail())
            {
                TRAP_IGNORE(entry->constructThumbnailL());
            }
            if (entry && entry->thumbnail())
            {
                TRect intersectRect(TPoint(0,0),m_renderTarget->offscreenBitmap()->SizeInPixels());
                intersectRect.Intersection(rect[i]);
                if (!intersectRect.IsEmpty())
                {
                    m_bitmapContext->BitBlt( intersectRect.iTl, entry->thumbnail(), TRect(TPoint(0,0), rect[i].Size()) );
                    m_bitmapContext->SetPenStyle(CGraphicsContext::ESolidPen);
                    m_bitmapContext->SetPenColor( KSideImageBorderColor );
                    m_bitmapContext->SetBrushStyle(CGraphicsContext::ENullBrush);
                    m_bitmapContext->DrawRect(intersectRect);
                }
            }
        }
    }
    if (!m_fastScroll) {
        m_placeHolderResizeFactor += KAnimFactorInc;
    }
    else {
        m_placeHolderResizeFactor += KFastScrollAnimFactorInc;
    }
    m_historyController->updateDisplay();
    return true;
}

void HistoryView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    TRect leftRect(m_leftArrow[2].iX,m_leftArrow[0].iY, m_leftArrow[1].iX, m_leftArrow[1].iY );
    TRect rightRect(m_rightArrow[0].iX,m_rightArrow[0].iY, m_rightArrow[2].iX, m_rightArrow[1].iY );
    if (aPointerEvent.iType == TPointerEvent::EButton1Down) {
        if (m_leftPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
            m_historyViewComponent = EHistoryComponentLeftPlaceHolder;
        }
        else if (m_rightPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
            m_historyViewComponent = EHistoryComponentRightPlaceHolder;
        }
        else if (m_centerPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
            m_historyViewComponent = EHistoryComponentMiddlePlaceHolder;
        }
        else if (leftRect.Contains(aPointerEvent.iPosition)) {
            m_historyViewComponent = EHistoryComponentLeftArrow;
        }
        else if (rightRect.Contains(aPointerEvent.iPosition)) {
            m_historyViewComponent = EHistoryComponentRightArrow;
        }
        // Start repeat timer
        if (m_historyViewComponent == EHistoryComponentLeftArrow || m_historyViewComponent == EHistoryComponentRightArrow) {
            if (m_autoScrollPeriodic) {
                m_autoScrollPeriodic->Cancel();
            }
            else {
                m_autoScrollPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
            }
            m_autoScrollPeriodic->Start(KAutoScrollPeriodic, KAutoScrollPeriodic, TCallBack(&autoScrollCallback, this));
        }
    }

    if (aPointerEvent.iType == TPointerEvent::EButton1Down)  {
        m_lastpointerposition = aPointerEvent.iPosition;
    }

    if (aPointerEvent.iType == TPointerEvent::EDrag)  {
        if (Abs(m_lastpointerposition.iX - aPointerEvent.iPosition.iX) < KMinimumScroll )
            return;
        if (m_lastpointerposition.iX + KDragDelta < aPointerEvent.iPosition.iX ) {
            performTransition(-1);
        }
        else if (m_lastpointerposition.iX > aPointerEvent.iPosition.iX + KDragDelta){
            performTransition(1);
        }
        return;
    }

    if (aPointerEvent.iType == TPointerEvent::EButton1Up) {
        if (m_autoScrollPeriodic) {
            m_autoScrollPeriodic->Cancel();
        }
        if (Abs(m_lastpointerposition.iX - aPointerEvent.iPosition.iX) > KDragDelta ) return;
        TKeyEvent keyEvent;
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats = 0;
        switch (m_historyViewComponent)
        {
        case EHistoryComponentNone:
        default:
            break;
        case EHistoryComponentLeftPlaceHolder:
            // Left entry
            if (m_leftPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
                if (m_centerPageIndex > 0) {
                    m_centerPageIndex--;
                    keyEvent.iCode = EKeyDevice3; //0x0000f842;
                    keyEvent.iScanCode = EStdKeyDevice3; //0x000000a7;
                    CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKeyUp );
                }
            }
            break;
        case EHistoryComponentMiddlePlaceHolder:
            // Center entry
            if (m_centerPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
                keyEvent.iCode = EKeyDevice3; //0x0000f842;
                keyEvent.iScanCode = EStdKeyDevice3; //0x000000a7;
                CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKeyUp );
            }
            break;
        case EHistoryComponentRightPlaceHolder:
            // Right entry
            if (m_rightPlaceHolderRect.Contains(aPointerEvent.iPosition)) {
                int historyLength = m_historyController->historyLength();
                if (m_centerPageIndex < ( historyLength -1 )) {
                    m_centerPageIndex++;
                    keyEvent.iCode = EKeyDevice3; //0x0000f842;
                    keyEvent.iScanCode = EStdKeyDevice3; //0x000000a7;
                    CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKeyUp );
                }
            }
            break;
        case EHistoryComponentLeftArrow:
            // Left arrow
            if (leftRect.Contains(aPointerEvent.iPosition)) {
                keyEvent.iCode = EKeyLeftArrow;
                keyEvent.iScanCode = EStdKeyLeftArrow;
                CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKeyUp );
            }
            break;
        case EHistoryComponentRightArrow:
            // Right arrow
            if (rightRect.Contains(aPointerEvent.iPosition)) {
                keyEvent.iCode = EKeyRightArrow;
                keyEvent.iScanCode = EStdKeyRightArrow;
                CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKeyUp );
            }
            break;
        }
    }
    m_pointerEvent = aPointerEvent;
}

// ----------------------------------------------------------------------------
// HistoryView::autoScroll()
// Public Class Method
// Called v=by timer callback when user presses an arrow and does not let go
// ----------------------------------------------------------------------------
void HistoryView::autoScroll()
{
    if (m_historyViewComponent == EHistoryComponentLeftArrow) {
        TRect leftRect(m_leftArrow[2].iX,m_leftArrow[0].iY, m_leftArrow[1].iX, m_leftArrow[1].iY );
        if (leftRect.Contains(m_pointerEvent.iPosition)) {
            performTransition(-1);
        }
    }
    else if (m_historyViewComponent == EHistoryComponentRightArrow) {
        TRect rightRect(m_rightArrow[0].iX,m_rightArrow[0].iY, m_rightArrow[2].iX, m_rightArrow[1].iY );
        if (rightRect.Contains(m_pointerEvent.iPosition)) {
            performTransition(1);
        }
    }
}

// Private

// ----------------------------------------------------------------------------
// HistoryView::CreateOffscreenBitmapL
// Private Class Method
// Creates the bitmap offscreen to improve performance.  The bitmap is created
// for the viewable content.
// ----------------------------------------------------------------------------
void HistoryView::updateOffscreenBitmapL()
{
    m_renderTarget->setView( 0 );
    m_bitmapDevice->Resize( m_offscreenRect.Size() );
    m_bitmapContext->Resized();
}

// ----------------------------------------------------------------------------
// HistoryView::LoadUrl()
// Private Class Method
// Get current url from history and call LoadUrlL(url)
// ----------------------------------------------------------------------------
void HistoryView::loadUrl()
{
    const HistoryEntry* entry = m_historyController->entryByIndex( m_centerPageIndex );
    if ( entry ) {
        TPtrC8 url = entry->requestUrl();
        if (entry->postContentType().Length() && entry->formData()) {
            if ( m_historyController->historyCallback()->doHistoryPost(url, TBrCtlDefs::ECacheModeHistory,
                entry->postContentType(), entry->formData()) == KErrNone) {
                m_historyController->setCurrentIndex( m_centerPageIndex );
            }
        }
        else {
            m_historyController->setCurrentIndex( m_centerPageIndex );
            m_historyController->historyCallback()->doHistoryGet( url, TBrCtlDefs::ECacheModeHistory );
        }
    }
}

// ----------------------------------------------------------------------------
// HistoryView::performTransition()
// Private Class Method
// Start animation
// ----------------------------------------------------------------------------
void HistoryView::performTransition(int dir)
{
    if (dir == -1) {
       if (m_centerPageIndex > 0) {
           m_direction = -1;
           //Start Animation
           if (m_repaintTimer->IsActive()) {
               m_repaintTimer->Cancel();
               m_resizeFactor = KMaxAnimFactor;
           }
           m_repaintTimer->Start(TCallBack(&animPlaceHolderRepaint,this));
       }
    }
    else if (dir == 1)  {
          int historyLength = m_historyController->historyLength();
          if (m_centerPageIndex < ( historyLength -1 )) {
              m_direction = 1;
              //Start Animation
              if (m_repaintTimer->IsActive()) {
                  m_repaintTimer->Cancel();
                  m_resizeFactor = KMaxAnimFactor;
              }
              m_repaintTimer->Start(TCallBack(&animPlaceHolderRepaint,this));
          }
    }

    updateState(dir);
}

// ----------------------------------------------------------------------------
// HistoryView::performTransition()
// Private Class Method
// Start animation
// ----------------------------------------------------------------------------
void HistoryView::updateState(int dir)
{
    if (dir ==0)
    {
        if (m_centerPageIndex == 0) {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryBeginning, true);
        }
        else {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryBeginning, false);
        }
        if (m_centerPageIndex >= ( m_historyController->historyLength() - 1 )){
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryEnd, true);
        }
        else {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryEnd, false);
        }
    }
    else
    {
        if ((dir == -1) && (m_centerPageIndex <= 1)) {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryBeginning, true);
        }
        else {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryBeginning, false);
        }
        if ((dir == 1) && (m_centerPageIndex >= ( m_historyController->historyLength() - 2 ))){
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryEnd, true);
        }
        else {
            m_historyController->historyCallback()->navigationStateChanged(TBrCtlDefs::EStateHistoryEnd, false);
        }

    }
}
//  End of File
