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
* Description:   Implementation of CWebKiCursor
*
*/


#include "config.h"
#include "../../bidi.h"
#include <stdlib.h>
#include "StaticObjectsContainer.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebFrameBridge.h"
#include "Frame.h"
#include "Document.h"
#include "Node.h"
#include "Element.h"
#include "HTMLImageElement.h"
#include "RenderImage.h"
#include "BrCtl.h"
#include "HTMLNames.h"
#include "HTMLElement.h"
#include "MouseEvent.h"
#include "EventNames.h"
#include "WebUtil.h"
#include "SettingsContainer.h"
#include "WebCursor.h"
#include "Page.h"
#include "FocusController.h"
#include <AknUtils.h>
#include "PlatformScrollbar.h"

#include "eikon.hrh"


const int KMiddleStep = 8;
const int KFlipAdjust =12;
const int KCursorIncremental = 17;         // 17 pixels
const int KTransparencyTime = 8000*1000; //8s
const int KTransparencyMoveThreshold = 15;

const int KAggressiveSearch = 50;

#define KCursorOffset TPoint(-3,-4)

using namespace WebCore;
using namespace EventNames;
using namespace HTMLNames;

// ============================= LOCAL FUNCTIONS ===============================

inline IntPoint nearestPointInRect(const IntPoint &point, const IntRect &rect)
{
  //Error condition
  if (rect.isEmpty()) return IntPoint(0,0);

  int y = std::max(rect.y(), std::min(rect.bottom(), point.y()));
  int x = std::max(rect.x(), std::min(rect.right(), point.x()));

  return IntPoint(x,y);
}

TInt TransparencyTimerCb(TAny* any)
    {
    WebCursor* c = static_cast<WebCursor*>(any);
    c->setTransparent(true);
    return EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebCursor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
WebCursor* WebCursor::NewL()
    {
    WebCursor* self = new (ELeave) WebCursor();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// WebCursor::WebCursor
// Constructor.
// -----------------------------------------------------------------------------
WebCursor::WebCursor() :
        m_pos(TPoint(KInitialOffset,KInitialOffset))
      , m_previousLr(0)
      , m_previousTb(0)
      , m_view(0)
      , m_visible(true)
      , m_flipcounter(0)
      , m_type(PointerCursor)
      , m_flipCounter(0)
    {
    }

// -----------------------------------------------------------------------------
// WebCursor::~WebCursor
// Deconstructor.
// -----------------------------------------------------------------------------
WebCursor::~WebCursor()
    {
    delete m_sprite;
    delete m_transarrowmask;
    if ( m_transtimer ) 
        m_transtimer->Cancel();
    delete m_transtimer;
    }

// -----------------------------------------------------------------------------
// WebCursor::ConstructL
// -----------------------------------------------------------------------------
void WebCursor::ConstructL()
    {
    m_transtimer = CPeriodic::NewL(CActive::EPriorityStandard);
    m_visible = !AknLayoutUtils::PenEnabled();
    }

// -----------------------------------------------------------------------------
// WebCursor::setCurrentView
// -----------------------------------------------------------------------------
void WebCursor::setCurrentView(WebView& view)
    {
    if (!m_view)
    {
        m_view = &view;
        TRAP_IGNORE( constructSpriteL() );
    }
    //switching between diffrent webviews, set current webview as the parent to m_sprite
    if( m_sprite->Parent() != &view)
    {
        m_view = &view;
        CCoeControl* parent = static_cast<CCoeControl*>(m_view);
        m_sprite->SetParent(parent);
    }
    m_view = &view;
    setOpaqueUntil(KTransparencyTime);
    m_transcount = 0;
    }

// -----------------------------------------------------------------------------
// WebCursor::constructSpriteL
// -----------------------------------------------------------------------------
void WebCursor::constructSpriteL()
    {

    //tot:fixme
    m_arrow = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageArrowBitmap);
    m_hand = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageFingerBitmap);
    m_ibeam = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageIBeam);
    m_selectMulti = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageSelectMulti);
    //iSmartLinkPhoneImage = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageSmartLinkPhone);
    //iSmartLinkEmailImage = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageSmartLinkEmail);
    m_wait = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageWaitArrowBitmap);
    TSize s = m_arrow.m_msk->SizeInPixels();
    m_transarrowmask = new (ELeave) CFbsBitmap();
    m_transarrowmask->Create(s,m_arrow.m_msk->DisplayMode());
    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(m_transarrowmask);
    CleanupStack::PushL(dev);
    CFbsBitGc* gc;
    User::LeaveIfError(dev->CreateContext(gc));
    gc->BitBlt(TPoint(0,0),m_arrow.m_msk);
    m_transarrowmask->LockHeap();
    TUint8* data = (TUint8*)m_transarrowmask->DataAddress();
    TUint8* end = data + s.iWidth*s.iHeight;
    // divide all pixels by 3
    while ( data<end )
        {
        *(data++) /= 3;
        }
    m_transarrowmask->UnlockHeap();
    delete gc;
    CleanupStack::PopAndDestroy();
    /////////////////////////////////
 
    CCoeControl* parent = static_cast<CCoeControl*>(m_view);
    TPoint pos = TPoint(KInitialOffset,KInitialOffset);
    m_sprite = CWebSprite::NewL(parent, pos, m_arrow.m_img, m_arrow.m_msk, ETrue);
    }

// -----------------------------------------------------------------------------
// WebCursor::setCursor
// -----------------------------------------------------------------------------
void WebCursor::setCursor(CursorTypes type)
{
    
    m_type = type;
    if (m_visible) {
        CFbsBitmap*  img = NULL;
        CFbsBitmap*  msk = NULL;
        switch( type )
            {
            case PointerCursor:
            default:
                {
                img = m_waiton ? m_wait.m_img : m_arrow.m_img;
                msk = m_waiton ? m_wait.m_img :
                    (m_transparent && m_transcount > KTransparencyMoveThreshold ? m_transarrowmask : m_arrow.m_msk);
                break;
                }
           case HandCursor:
                {
                img = m_hand.m_img;
                msk = m_hand.m_msk;
                break;
                }
           case IBeamCursor:
                {
               /*tot:fixme if ( m_view->FocusedElementType() == TBrCtlDefs::EElementSmartLinkTel )
                    {
                    spriteMem.iBitmap = iSmartLinkPhoneImage.m_img;
                    spriteMem.iMaskBitmap = iSmartLinkPhoneImage.m_msk;
                    }
                else if ( m_view->FocusedElementType() == TBrCtlDefs::EElementSmartLinkEmail )
                    {
                    spriteMem.iBitmap = iSmartLinkEmailImage.m_img;
                    spriteMem.iMaskBitmap = iSmartLinkEmailImage.m_msk;
                    }
                else*/
                    {
                    img = m_ibeam.m_img;
                    msk = m_ibeam.m_msk;
                    }
                break;
                }
           case SelectMultiCursor:
               {
                    img = m_selectMulti.m_img;
                    msk = m_selectMulti.m_msk;
                    break;
               }
            }
        TPoint pos = m_pos + KCursorOffset;
        
        m_sprite->Update(pos, img, msk);
        
    } 
    else {
        m_sprite->Hide();
    }
}

// -----------------------------------------------------------------------------
// WebCursor::cursorUpdate
// -----------------------------------------------------------------------------
void WebCursor::cursorUpdate(bool visible)
{
    if (!m_view || !m_view->brCtl() || !m_view->brCtl()->settings())
        return;

    bool tabbedNavigation = (m_view->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeTabbed);
    bool navigationNone = (m_view->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeNone); 
    bool cursorNavigation = (m_view->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeCursor);
    TBrCtlDefs::TBrCtlElementType elType = m_view->focusedElementType();
    //If cursor show mode defined inside cenrep key is no cursor shown in non-tab navigation mode,
    //then no need to update the cursor
    if (!tabbedNavigation &&
         (m_view->brCtl()->settings()->brctlSetting(TBrCtlDefs::ESettingsCursorShowMode) == TBrCtlDefs::ENoCursor))
        return;

    m_visible = visible && ((cursorNavigation && m_view->showCursor()) || 
                            (tabbedNavigation && (elType == TBrCtlDefs::EElementSelectMultiBox)));

    resetTransparency();
    CursorTypes type = PointerCursor;    
    if (m_visible) {
        if      (    elType == TBrCtlDefs::EElementNone
                  || elType == TBrCtlDefs::EElementImageBox
                  || elType == TBrCtlDefs::EElementBrokenImage )
            type = PointerCursor;
        else if (    elType == TBrCtlDefs::EElementSmartLinkTel
                  || elType == TBrCtlDefs::EElementSmartLinkEmail )
            type = IBeamCursor;
        else if (    elType == TBrCtlDefs::EElementSelectMultiBox && tabbedNavigation)
            type = SelectMultiCursor;
        else
            type = HandCursor;
    }

    setCursor(type);
}



// -----------------------------------------------------------------------------
// WebCursor::getFrameAtPoint
// -----------------------------------------------------------------------------
WebFrame* WebCursor::getFrameAtPoint(const TPoint& viewPos_)
    {
    if (!m_view)
        return 0;
    WebFrame* frameAtPoint = m_view->mainFrame()->frameAtPoint(viewPos_);
    if (!frameAtPoint)
        frameAtPoint = m_view->mainFrame();
    return frameAtPoint;
    }

// -----------------------------------------------------------------------------
// WebCursor::reset
// -----------------------------------------------------------------------------
void WebCursor::reset()
    {
    m_view->setFocusedElementType(TBrCtlDefs::EElementNone);
    cursorUpdate(true);
    m_nodeRect = TRect();
    }

// -----------------------------------------------------------------------------
// WebCursor::setWaitCursor
// -----------------------------------------------------------------------------
void WebCursor::setWaitCursor(bool waiton)
    {
    m_waiton = waiton;
    if (m_waiton)
        setCursor(PointerCursor);
    else
        cursorUpdate(m_visible);
    }

// -----------------------------------------------------------------------------
// WebCursor::offsetCursor
// -----------------------------------------------------------------------------
void WebCursor::offsetCursor(const TPoint& offset)
    {
    m_pos -= offset;
    m_nodeRect.Move(-offset.iX,-offset.iY);
    }

// -----------------------------------------------------------------------------
// WebCursor::setTransparent
// -----------------------------------------------------------------------------
void WebCursor::setTransparent(bool transparent)
{
    if (transparent)
        m_transtimer->Cancel();
    if (m_transparent != transparent) {
        m_transparent = transparent;
        // only pointer cursor is transparent
        if (m_transparent && m_view->focusedElementType() == TBrCtlDefs::EElementNone)
            setCursor(PointerCursor);
        else
            cursorUpdate(m_visible);
    }
}

// -----------------------------------------------------------------------------
// WebCursor::setOpaqueUntil
// -----------------------------------------------------------------------------
void WebCursor::setOpaqueUntil(int microsecs)
    {
    setTransparent(false);
    m_transtimer->Cancel();
    
    if (m_visible)
        {
        m_transtimer->Start(microsecs,0,TCallBack(TransparencyTimerCb,this));
        }
    }

void WebCursor::stopTransparencyTimer()
    {
    if (m_transtimer && m_transtimer->IsActive())
        {
        m_transtimer->Cancel();
        }
    }


// -----------------------------------------------------------------------------
// WebCursor::increaseTransparencyMoveCount
// -----------------------------------------------------------------------------
void WebCursor::increaseTransparencyMoveCount()
    {
    ++m_transcount;
    }

// -----------------------------------------------------------------------------
// WebCursor::resetTransparency
// -----------------------------------------------------------------------------
void WebCursor::resetTransparency()
    {
    m_transcount = 0;
    setOpaqueUntil(KTransparencyTime);
    }

//-------------------------------------------------------------------------------
// WebCursor::innerRect()
//-------------------------------------------------------------------------------
void WebCursor::innerRect(TRect& rect)
{
  TPoint c = rect.Center();
  int z = m_view->scalingFactor();
  int dx = KCursorIncremental * z / 100;
  int dy = KCursorIncremental * z / 100;

  dx = Max(dx, 2 * rect.Width() / 5);
  dy = Max(dy, 2 * rect.Height() / 5);

  rect.Shrink(dx, dy);

  if (!rect.IsNormalized()) {
    rect.SetRect(c, c);
  }
}

//-------------------------------------------------------------------------------
// WebCursor::frameHasContentToScroll()
//-------------------------------------------------------------------------------
bool WebCursor::frameHasContentToScroll(WebFrame* frame, TPoint& delta)
{
    WebFrameView* rfv = frame->frameView();
    TPoint contentpos = rfv->contentPos();
    TSize contentsize = rfv->contentSize();
    bool hasContentToScrollY = false;
    bool hasContentToScrollX = false;

    if (delta.iY > 0)
        {
        hasContentToScrollY = (contentpos.iY < (contentsize.iHeight - rfv->rect().Height()));
        }
    else if (delta.iY < 0)
        {
        hasContentToScrollY = (contentpos.iY > 0);
        }

    if (delta.iX > 0)
        {
        hasContentToScrollX = (contentpos.iX < (contentsize.iWidth - rfv->rect().Width()));
        }
    else if (delta.iX < 0)
        {
        hasContentToScrollX = (contentpos.iX > 0);
        }

    return hasContentToScrollY || hasContentToScrollX;

}

//-------------------------------------------------------------------------------
// WebCursor::determineScrollingFrame()
//-------------------------------------------------------------------------------
WebFrame*  WebCursor::determineScrollingFrame(int border1, int border2, int pos,
                                              WebFrame* fr1, WebFrame* fr2, TPoint& delta)
{
  WebFrame* ret = NULL;

  if (pos < border1 && pos < border2) return ret; //"magic" line has not been reached
  if (pos >= border1 && pos >= border2) {
      if (border1 > border2) {
          ret = frameHasContentToScroll(fr2, delta) ? fr2 : fr1;
      }
      else {
          ret = frameHasContentToScroll(fr1, delta) ? fr1 : fr2;
      }
  }
  else if (border1 <= pos && pos <= border2) {//"magic" line of fr1 was hit first
      ret = fr1;
  }
  else if (border2 <= pos && pos <= border1) {//"magic" line of fr2 was hit first
      ret = fr2;
  }
  return ret;
}


//-------------------------------------------------------------------------------
// WebCursor::calculateScrollableFrameView()
// TODO: Investigate diagonal events, especially in the light of IFrames.
//-------------------------------------------------------------------------------
WebFrame* WebCursor::calculateScrollableFrameView(TPoint& pos, TPoint& delta, TRect& fRect, bool autoscroll)
{
  WebFrame* frame = getFrameAtPoint(pos);
  WebFrame* ret_frame = NULL;
  WebFrameView* mfView = m_view->mainFrame()->frameView();
  TPoint docContPos = mfView->toViewCoords(mfView->contentPos());
  int z = m_view->scalingFactor();
  m_incrLimit.iX = Abs(delta.iX) * 2 * z / 100;
  m_incrLimit.iY = Abs(delta.iY) * 2 * z / 100;


  for (; frame; frame = frame->parentFrame())  {
    bool hScroll = frame->frameView()->hScrollbar()->isEnabled();
    bool vScroll = frame->frameView()->vScrollbar()->isEnabled();
          // Dont scroll a frameset
    if (frame->parentFrame() && frame->parentFrame()->isFrameSet()) {
      continue;
    }
    else if (frame->parentFrame() && !hScroll && !vScroll) {
      continue;
    }
    else if (!frame->parentFrame()) { //main view
      hScroll = true;
      vScroll = true;
    }


    WebFrame*  pf = frame->parentFrame() ? frame->parentFrame() : m_view->mainFrame();
    WebFrameView* pfv = pf->frameView();
    WebFrameView* ppfv = pf->parentFrame() ? pf->parentFrame()->frameView() : NULL;
    TRect pfRect = pfv->rect();
    if (ppfv) {// parent frame is not a main frame
        pfRect = TRect(ppfv->frameCoordsInViewCoords(pfRect.iTl),
                          ppfv->frameCoordsInViewCoords(pfRect.iBr));
    }
    else { 
        pfRect.SetRect(0, 0, pfRect.iBr.iX * z / 100, pfRect.iBr.iY * z / 100);
    }
    
    WebFrameView* fv = frame->frameView();
    TRect framerect = fv ->rect();
    if (frame->parentFrame()) {// frame is not a main frame
       framerect = TRect(pfv->frameCoordsInViewCoords(framerect.iTl),
                         pfv->frameCoordsInViewCoords(framerect.iBr));
    }
    else {
        framerect.SetRect(0, 0, framerect.iBr.iX * z / 100, framerect.iBr.iY * z / 100);
    }
    
    TRect pfInnerRect = pfRect;
    innerRect(pfInnerRect);
    TRect fInnerRect = framerect;
    innerRect(fInnerRect);


    // See which "magic" line we crossed first
    if (delta.iY > 0) {  //scroll down.
        ret_frame = determineScrollingFrame(pfInnerRect.iBr.iY, fInnerRect.iBr.iY, pos.iY, pf, frame, delta);
        m_incrLimit.iY = Min (m_incrLimit.iY, Abs(framerect.iBr.iY - pos.iY));
    }
    else if (delta.iY < 0) { //scroll up.
        ret_frame = determineScrollingFrame(-pfInnerRect.iTl.iY, -fInnerRect.iTl.iY, -pos.iY, pf, frame, delta);
        m_incrLimit.iY = Min (m_incrLimit.iY, Abs(pos.iY - framerect.iTl.iY));
    }

    if (delta.iX > 0) {
        ret_frame = determineScrollingFrame(pfInnerRect.iBr.iX, fInnerRect.iBr.iX, pos.iX, pf, frame, delta);
        m_incrLimit.iX = Min (m_incrLimit.iX, Abs(framerect.iBr.iX - pos.iX));
    }
    else if (delta.iX < 0) {
        ret_frame = determineScrollingFrame(-pfInnerRect.iTl.iX, -fInnerRect.iTl.iX, -pos.iX, pf, frame, delta);
        m_incrLimit.iX = Min (m_incrLimit.iX, Abs(pos.iX - framerect.iTl.iX));
    }

    fRect = (ret_frame == frame) ? framerect : pfRect;

     if (ret_frame) {
        if (!ret_frame->parentFrame()) {
                m_incrLimit.iX = Abs(delta.iX) * 2 * z / 100;
                m_incrLimit.iY = Abs(delta.iY) * 2 * z / 100;
        }

        if (!frameHasContentToScroll(ret_frame, delta)) {
            ret_frame = NULL;
            m_incrLimit.iX = KCursorIncremental * z / 100;
            m_incrLimit.iY = KCursorIncremental * z / 100;
        }

        if (ret_frame) {
            m_incrLimit.iX = Max(m_incrLimit.iX, KMiddleStep);
            m_incrLimit.iY = Max(m_incrLimit.iY, KMiddleStep);
            break;
        }
    }
    else {
         m_incrLimit.iX = Abs(delta.iX) * 2 * z / 100;
         m_incrLimit.iY = Abs(delta.iY) * 2 * z / 100;
    }
  }

  return ret_frame;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// WebCursor::scrollingDirection()
//-------------------------------------------------------------------------------
void WebCursor::scrollingDirection(int dir, int& lr, int& tb)
{
     switch (dir)
       {
         case EKeyUpArrow:             // North
           tb = -1;
           break;

         case EKeyRightUpArrow:        // Northeast
         case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
           tb = -1;
           lr = +2;                    // Make it run closer to 45 degrees
           break;

         case EKeyRightArrow:          // East
           lr = +1;
           break;

         case EKeyRightDownArrow:      // Southeast
         case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
           tb = +1;
           lr = +2;
           break;

         case EKeyDownArrow:           // South
           tb = +1;
           break;

         case EKeyLeftDownArrow:       // Southwest
         case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
           tb = +1;
           lr = -2;
           break;

         case EKeyLeftArrow:           // West
           lr = -1;
           break;

         case EKeyLeftUpArrow:         // Northwest
         case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
           tb = -1;
           lr = -2;
           break;

         default:
           break;
       }
    }


//-------------------------------------------------------------------------------
// WebCursor::scrollAndMoveCursor
//
// Method that scrolls and moves the cursor based on the navigation algorithm
// TODO: Investigate "autoscroll" and diagonal events from real HW and the
//       diagonal event simulator wedge.
//-------------------------------------------------------------------------------
void WebCursor::scrollAndMoveCursor(int dir, int scrollRange, bool autoscroll)
{
    if (!m_view)
        return;

    int lr = 0;
    int tb = 0;
    int deltaX;
    int deltaY;

    scrollingDirection(dir, lr, tb);

    deltaX = lr * scrollRange/2;
    deltaY = tb * scrollRange;

    TPoint delta = TPoint(deltaX, deltaY);
    WebFrame* frame = NULL;
    WebFrameView* fv = NULL;
    TPoint oldpos;
    TRect inRect = TRect();
    bool needScroll = false;

    frame = calculateScrollableFrameView(m_pos, delta, inRect, autoscroll);
    needScroll = (frame != NULL);
    if (!frame) {
      frame = m_view->mainFrame();
    }
    fv = frame->frameView();
    oldpos = fv->contentPos();

    if (autoscroll || m_view->pageView()) {
        // scroll
        fv->scrollTo(oldpos + delta);

        if (!m_view->pageView()) {
            TPoint cp = position();
            if (fv->contentPos() == oldpos) {
                // not scrolling, move the cursor instead
                cp.iX += deltaX;
                cp.iY += deltaY;
            } else {
                // move cursor slowly out of the way
                cp.iX += deltaX/6;
                cp.iY += deltaY/6;
            }
            // limit the movement, don't scroll out of the view area
            if (lr != 0) {
              cp.iX = (lr < 0) ? Max(inRect.iTl.iX + 8, cp.iX) : Min(inRect.iBr.iX - 8, cp.iX);
            }
            if (tb != 0) {
              cp.iY = (tb < 0) ? Max(inRect.iTl.iY + 8, cp.iY) : Min(inRect.iBr.iY - 8, cp.iY);
            }
            if (cp.iX == (inRect.iTl.iX + 8) || (cp.iX == inRect.iBr.iX - 8) ||
                (cp.iY == inRect.iTl.iY + 8) || (cp.iY == inRect.iBr.iY - 8)) {
            }
            setPosition(cp);
            reset();
        }
        return;
    }
    if (!handleSelectElementScrolling(m_view, tb)) {
        // link snapping
      moveCursor( lr,tb,scrollRange);
    }

    TPoint newpos(oldpos);
    newpos += TPoint(lr*scrollRange/2, tb*scrollRange);
    newpos = fv->nearestPointInFrame(newpos);

    int adjustedScrollRange = Abs(lr*(newpos.iX - oldpos.iX)) + Abs(tb*(newpos.iY - oldpos.iY));

    if (lr==0)
        increaseTransparencyMoveCount();
    else
        resetTransparency();

    if (adjustedScrollRange) {
        TPoint p = position();
        if ( needScroll ) {
            innerRect(inRect);
            int deltax = (lr != 0 ) ? ( (lr > 0) ? (p.iX - inRect.iBr.iX) : (inRect.iTl.iX - p.iX ) ) : 0 ;
            int deltay = (tb != 0 ) ? ( (tb > 0) ? (p.iY - inRect.iBr.iY ) : (inRect.iTl.iY - p.iY) ) : 0 ;
            deltax = Min(adjustedScrollRange, deltax);
            deltay = Min(adjustedScrollRange, deltay);
            TPoint offset(fv->toViewCoords(TPoint(lr*deltax,tb*deltay)));
            offsetCursor(offset);
            fv->scrollTo(oldpos+TPoint(lr*deltax , tb*deltay));
        } else {
            if (m_view->pageView())
                m_view->brCtl()->DrawNow();
        }
    }
}

// -----------------------------------------------------------------------------
// WebCursor::simpleScroll
// -----------------------------------------------------------------------------
void WebCursor::simpleScroll(int dir, int scrollRange, bool autoscroll)
{
    if (!m_view)
        return;

    int lr = 0;
    int tb = 0;

    scrollingDirection(dir, lr, tb);

    int deltaX = lr * scrollRange/2;
    int deltaY = tb * scrollRange;

    TPoint delta = TPoint(deltaX, deltaY);
    WebFrameView* fv = m_view->mainFrame()->frameView();
    TPoint oldpos = fv->contentPos();

    if (autoscroll || m_view->pageView()) {
        // just simple scroll
        fv->scrollTo(oldpos + delta);
    }

    if (!m_view->pageView()) {
        TPoint cp = position();
        if (fv->contentPos() == oldpos) {
            // not scrolling, move the cursor instead
            cp.iX += deltaX;
            cp.iY += deltaY;
        } else {
            // move cursor slowly out of the way
            cp.iX += deltaX/6;
            cp.iY += deltaY/6;
        }
        setPosition(cp);
        reset();
    }
}

// -----------------------------------------------------------------------------
// WebCursor::updatePositionAndElemType
// -----------------------------------------------------------------------------
void WebCursor::updatePositionAndElemType(const TPoint& pt)
{
    m_pos = pt;
    m_sprite->SetPos(pt);
    WebFrame* frame = getFrameAtPoint(pt);
    TBrCtlDefs::TBrCtlElementType elType;
    TRect r;
    TPoint point(frame->frameView()->viewCoordsInFrameCoords(m_pos));
    if (m_view && navigableNodeUnderCursor(*frame, point, elType, r)) {
        m_view->setFocusedElementType(elType);
    }
    else {
        m_view->setFocusedElementType(TBrCtlDefs::EElementNone);
    }
}

// -----------------------------------------------------------------------------
// WebCursor::moveCursor
// -----------------------------------------------------------------------------
void WebCursor::moveCursor(int lr,int tb, int scrollRange)
    {
    TPoint cp(m_pos);
    int z = m_view->scalingFactor();

    //Flipping is set to true only if the user make 3 consective moves in opposite direction
    //ie if the user went in up down up direction then we apply the flipping logic
    if ( ( lr*m_previousLr == -1 || tb*m_previousTb == -1) )
        {
        m_flipcounter++ ;
        }
    else m_flipcounter = 0;

    m_previousLr = lr;
    m_previousTb = tb;

    if (m_view->focusedElementType() == TBrCtlDefs::EElementAnchor && !m_nodeRect.IsEmpty() && m_flipcounter <= 1 ) {
        TSize nodeSizeInDocCoords(m_view->mainFrame()->frameView()->toDocCoords(m_nodeRect.Size()));
        if ( tb == 0 && nodeSizeInDocCoords.iWidth < scrollRange) {
            cp.iX = (lr == -1) ? Min(cp.iX , Max(m_nodeRect.iTl.iX, cp.iX - m_incrLimit.iX)) :
                                 Max(cp.iX, Min(m_nodeRect.iBr.iX, cp.iX + m_incrLimit.iX));
        }
        if ( lr == 0 && nodeSizeInDocCoords.iHeight < scrollRange) {
            cp.iY = (tb == -1) ? Min(cp.iY, Max(m_nodeRect.iTl.iY, cp.iY - m_incrLimit.iY)) :
                                 Max(cp.iY, Min(m_nodeRect.iBr.iY, cp.iY + m_incrLimit.iY));
        }
    }

    m_incrLimit.iX -= Abs(m_pos.iX - cp.iX);
    m_incrLimit.iY -= Abs(m_pos.iY - cp.iY);

    cp.iX += lr*Min(KCursorIncremental*z/100, m_incrLimit.iX);
    cp.iY += tb*Min(KCursorIncremental*z/100, m_incrLimit.iY);

    TRect rec(TPoint(0,0), m_view->offscreenRect().Size());
    // limit the movement, don't scroll out of the view area
    cp.iX = Min(rec.Width(), Max( 0 , cp.iX));
    cp.iY = Min(rec.Height(), Max( 0 , cp.iY));

    TPoint rPos[] = { cp, cp };
    rPos[0] -= TPoint(lr*KMiddleStep*z/100,tb*KMiddleStep*z/100);

    // user is trying hard to focus a small element
    if ( m_flipcounter > 1 )
        {
        int flipDistance = KFlipAdjust * z/(100 * (m_flipcounter - 1));
        rPos[0] = m_pos + TPoint(lr * flipDistance/2,tb * flipDistance/2);
        rPos[1] = m_pos + TPoint(lr * flipDistance,tb * flipDistance);
        }

    m_pos = rPos[0];

    for ( int i = 0;i < 2; i++ )
        {
        WebFrame* frame = getFrameAtPoint(rPos[i]);
        if ( !frame ) continue;

        m_view->page()->focusController()->setFocusedFrame(core(frame));
        TPoint pt = frame->frameView()->viewCoordsInFrameCoords(rPos[i]);

        // now let's do the probing
        TBrCtlDefs::TBrCtlElementType elType = TBrCtlDefs::EElementNone;
        TRect nr = TRect();

        bool test = navigableNodeUnderCursor(*frame, pt, elType, nr);

        TRect rt(frame->frameView()->frameCoordsInViewCoords(nr.iTl),m_view->mainFrame()->frameView()->toViewCoords(nr.Size()));
        rt.Intersection(rec);

        if (i == 0 && ( !test || (m_view->focusedElementType() == elType  && rt == m_nodeRect)) ) continue;

        if ( i == 1 && !test && m_flipCounter <= 1 && !m_nodeRect.IsEmpty())
            {
            //Go to webcore to figure out a possible node
            TRect searchRect = calcSearchRect(lr,tb, scrollRange);
            TPoint cp(m_pos);
            WebFrame* f = getFrameAtPoint(cp);
            if ( !f ) continue;

            cp = f->frameView()->viewCoordsInFrameCoords(cp);
            bool found = determineCursorPosition(*f, elType, nr, searchRect, cp, false);
            if ( found )
                {
                m_pos = f->frameView()->frameCoordsInViewCoords(cp);

                m_nodeRect = TRect(f->frameView()->frameCoordsInViewCoords(nr.iTl),m_view->mainFrame()->frameView()->toViewCoords(nr.Size()) );
                m_nodeRect.Intersection(rec);
                m_view->setFocusedElementType(elType);
                return;
                }
            }

        m_pos = frame->frameView()->frameCoordsInViewCoords(pt);
        m_nodeRect = rt;
        m_view->setFocusedElementType(elType);
        return;
        }
    }


bool WebCursor::navigableNodeUnderCursor(WebFrame& webFrame, TPoint& aPoint, TBrCtlDefs::TBrCtlElementType& aElType, TRect& aFocusRect)
{
    Frame* coreFrame = core(&webFrame);
    if (!coreFrame->renderer() )
        return false;

    Element* node = coreFrame->document()->elementFromPoint(aPoint.iX, aPoint.iY);
    m_elementUnderCursor = node;
    if (node) {
        Node* retNode = 0;
        bool ret = coreFrame->bridge()->getTypeFromElement(node, aElType, aFocusRect, retNode);
        m_elementUnderCursor = static_cast<Element *>(retNode);
        return ret;
    }

    return false;
}

// -----------------------------------------------------------------------------
// WebCursor::determineCursorPosition
// -----------------------------------------------------------------------------
bool WebCursor::determineCursorPosition( WebFrame& webFrame, TBrCtlDefs::TBrCtlElementType& aElType,
                                         TRect& aFocusRect, TRect& aSearchRect,
                                         TPoint &aCursorPosition, bool aInitialize)
{
    // FIXME: cursor navigation not implemented
    IntRect searchRt(aSearchRect);
    IntPoint pt(aCursorPosition.iX,aCursorPosition.iY);
    bool found = false;

    if (aInitialize && core(&webFrame)->document()->focusedNode()) {
       IntRect nodeRect = core(&webFrame)->document()->focusedNode()->getRect();
       if (nodeRect.intersects(searchRt)) {
          IntRect rt = nodeRect;
          rt.intersect(searchRt);
          pt = IntPoint( rt.x() + rt.width()>>1, rt.y() + rt.height()>>1 );
          found = true;
       }
    }

    found = decideCursorPosition(webFrame, searchRt, pt);
    TPoint newPos(pt.x(),pt.y());

    bool retest = false;
    if (found)
       retest = navigableNodeUnderCursor(webFrame, newPos,aElType,aFocusRect);

    if (!retest)
        return false;

    aCursorPosition = newPos;
    return retest;
}


// -----------------------------------------------------------------------------
// WebCursor::calcSearchRect
// -----------------------------------------------------------------------------
TRect WebCursor::calcSearchRect(int lr, int tb, int scrollRange)
{
    TRect searchRect;
    TPoint pt(m_nodeRect.iTl);
    TSize nodeSz(m_view->mainFrame()->frameView()->toDocCoords(m_nodeRect.Size()));

    WebFrame* focused = getFrameAtPoint(m_pos);
    if (!focused)
        return searchRect;
    pt = focused->frameView()->viewCoordsInFrameCoords(pt);

    if (focused) {
        TSize sz( (lr == 0) ? nodeSz.iWidth: scrollRange , (tb == 0) ? nodeSz.iHeight:scrollRange);
        pt.iX += (tb != 0) ? 0 : (lr == -1) ? -scrollRange: nodeSz.iWidth ;
        pt.iY += (lr != 0) ? 0 : (tb == -1) ? -scrollRange: nodeSz.iHeight ;
        searchRect = TRect(pt,sz);

        TBrCtlDefs::TBrCtlElementType elType = m_view->focusedElementType();
        if (    elType == TBrCtlDefs::EElementInputBox
             || elType == TBrCtlDefs::EElementTextAreaBox
             || elType == TBrCtlDefs::EElementRadioButtonUnSelected
             || elType == TBrCtlDefs::EElementButton
             || elType == TBrCtlDefs::EElementCheckBoxUnChecked
             || elType == TBrCtlDefs::EElementCheckBoxChecked
             || elType == TBrCtlDefs::EElementRadioButtonSelected )
          increaseSearchRect(lr, tb, searchRect);
    }
    return searchRect;
}

// -----------------------------------------------------------------------------
// WebCursor::increaseSearchRect
// -----------------------------------------------------------------------------
void WebCursor::increaseSearchRect(int lr,int tb,TRect& aRect)
{
    int width(aRect.Width());
    int height(aRect.Height());

    // enlarge the search rect for aggressive probing, the origninal ccb implementation is faulty.
    if (lr) {
        if (lr > 0)
            aRect.SetWidth(width + width*KAggressiveSearch/100);   // right
        else
            aRect.iTl.iX -= width*KAggressiveSearch/100;            // left
    } else if (tb) {
        if (tb > 0)
            aRect.SetHeight(height + height*KAggressiveSearch/100); // down
        else
            aRect.iTl.iY -= height*KAggressiveSearch/100;           // up
    }
    m_flipCounter = 0;
}

// -----------------------------------------------------------------------------
// WebCursor::decideCursorPosition
// -----------------------------------------------------------------------------
bool WebCursor::decideCursorPosition(WebFrame& webFrame, const IntRect& searchRect, IntPoint& cursorPosition)
{
    int distance = -1;
    bool found = false;
    IntPoint cp = cursorPosition;

    const int minWidth = 5;
    const int minHeight = 5;

    Vector<WebCore::IntRect>& recList = *(webFrame.bridge()->focusableRectList());

    if (recList.isEmpty())
        return false;

    for (int i = 0;i < recList.size();i++) {
        IntRect nr = recList[i];

        //Resize the rect so that the cursor is comfortably inside the box
        nr.inflate(-2);

        if (!nr.intersects(searchRect)) continue;

        IntRect intersectRect = nr;
        intersectRect.intersect(searchRect);

        if ( (nr.width() != intersectRect.width() && intersectRect.width() < minWidth)
           || (nr.height() != intersectRect.height() && intersectRect.height() < minHeight) )
            continue;

        intersectRect.inflate(-1);
        found = true;

        IntPoint np = nearestPointInRect(cp,intersectRect);
        int disFromCursor = (np.x() - cp.x())*(np.x() - cp.x())
                           + (np.y() - cp.y())*(np.y() - cp.y());

        if (distance == -1 || distance > disFromCursor) {
            distance = disFromCursor;
            cursorPosition = np;
        }
    }

    return found;
}

// -----------------------------------------------------------------------------
// WebCursor::getFrameUnderCursor
// -----------------------------------------------------------------------------
WebFrame* WebCursor::getFrameUnderCursor()
{
  return getFrameAtPoint(m_pos);
}



// END OF FILE
