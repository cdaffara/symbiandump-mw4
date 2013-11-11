/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknUtils.h>
#include <e32cmn.h>
#include "WebScrollbarDrawer.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "BrCtl.h"
#include "StaticObjectsContainer.h"
#include "WebSurface.h"
#include "WebSprite.h"

#include "WebKitLogger.h"

const int     KMinScrollbarWidth = 10;
const int     KMaxScrollbarWidth = 15;
const TUint32 KMinScrollBarTransparency = 150;
const int     KScrollBarFadeInterval = 30000;
const TInt32  KScrollBarTransparencyStep = 10;
const int     KMinThumbSize = 30;

int handleFadeScrollBar(TAny* ptr);

using namespace WebCore;

WebScrollbarDrawer* WebScrollbarDrawer::NewL()
{
  WebScrollbarDrawer* self = new(ELeave) WebScrollbarDrawer();
  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop(); //self
  return self;
}


void WebScrollbarDrawer::ConstructL()
{
  m_scrollBarFader = CPeriodic::NewL(CActive::EPriorityStandard);
}



WebScrollbarDrawer::WebScrollbarDrawer(): 
                                        m_webView(NULL),
                                        m_scrollBarTransparency(KMinScrollBarTransparency),
                                        m_scrollBarWidth(KMinScrollbarWidth),
                                        m_dX(0), m_dY(0), m_spriteV(NULL), m_spriteH(NULL)
{
}



WebScrollbarDrawer::~WebScrollbarDrawer()
{
  if (m_scrollBarFader) {
      m_scrollBarFader->Cancel();
  }
  delete m_scrollBarFader;
  
  removeBitmaps();
  delete m_spriteV;
  delete m_spriteH;
}


void WebScrollbarDrawer::removeBitmaps()
{
  deleteBitmap(m_scrollBarV, m_gcV, m_devV);
  deleteBitmap(m_scrollBarVMask, m_gcVMask, m_devVMask);

  deleteBitmap(m_scrollBarH, m_gcH, m_devH);
  deleteBitmap(m_scrollBarHMask, m_gcHMask, m_devHMask);
	
}

TInt WebScrollbarDrawer::InitScrollbar(WebView* view)
{
  TInt err = KErrNone;
  m_hasVScroll = EFalse;
  m_hasHScroll = EFalse;
  m_webView = view;  
  m_scrollBarTransparency = KMinScrollBarTransparency;
  m_dY = 0.0;
  m_dX = 0.0;
  m_rectVThum = TRect();
  m_rectHThum = TRect();
  m_rect.SetRect(m_webView->brCtl()->Rect().iTl,
                 m_webView->brCtl()->Rect().Size());
  m_scrollBarWidth = Max(0.04 * Min(m_rect.Width(), m_rect.Height()), KMinScrollbarWidth);  
  calculateBitmapRects();
  
  err = SetupBitmaps();
 
  if (err == KErrNone) {
      if (!m_spriteV) {
          m_spriteV = CWebSprite::NewL(m_webView, m_rectVThum.iTl, m_scrollBarV, m_scrollBarVMask, ETrue);
      }
      else {
          m_spriteV->SetBitmap(m_scrollBarV, m_scrollBarVMask, ETrue);
          m_spriteV->SetPos(m_rectVThum.iTl);
      }
      
      if (!m_spriteH) {
          m_spriteH = CWebSprite::NewL(m_webView, m_rectHThum.iTl, m_scrollBarH, m_scrollBarHMask, ETrue);
      }
      else {    
          m_spriteH->SetBitmap(m_scrollBarH, m_scrollBarHMask, ETrue);
          m_spriteH->SetPos(m_rectHThum.iTl);
      }
      m_spriteV->Hide();
      m_spriteH->Hide();
  }
  
  return err;
}


int WebScrollbarDrawer::fadeTime()
{
    return KScrollBarFadeInterval;
}

void WebScrollbarDrawer::fadeScrollbar()
{
    fadeScrollbar(0);
}

void WebScrollbarDrawer::fadeScrollbar(TInt delay)
{
  if (m_hasVScroll || m_hasHScroll) {
    if (!m_scrollBarFader->IsActive()) {
      m_scrollBarFader->Start(delay, KScrollBarFadeInterval, 
                              TCallBack(&handleFadeScrollBar, this));
    }
  }
  else {
    m_scrollBarTransparency = 255;
    fade();
  }
}


int handleFadeScrollBar(TAny* ptr)
{
  (static_cast<WebScrollbarDrawer*>(ptr))->fade();	
  return EFalse;
}


void WebScrollbarDrawer::clearSprites()
{
   if (m_spriteV) {
       m_spriteV->Hide();
       m_spriteV->Update(NULL, NULL);
       
   }
   
   if (m_spriteH) {
       m_spriteH->Hide();
       m_spriteH->Update(NULL, NULL);
       
   }
}

void WebScrollbarDrawer::fade()
{
  m_scrollBarTransparency += KScrollBarTransparencyStep;
  if (m_scrollBarTransparency >= 255) {
    m_scrollBarFader->Cancel();
    clearSprites();
    removeBitmaps();
    if (m_webView) {
      m_webView->syncRepaint(m_webView->mainFrame()->frameView()->visibleRect());
      m_webView = NULL;
    }
    m_hasVScroll = EFalse;
    m_hasHScroll = EFalse;
  }
  else {
    
    if (m_hasVScroll) {
      drawThumbMask(m_gcVMask, m_rectVThum);
      if (m_spriteV) {
          m_spriteV->Update(m_scrollBarV, m_scrollBarVMask);
      }
    }
    
    if (m_hasHScroll) {
      drawThumbMask(m_gcHMask, m_rectHThum);
      if (m_spriteH) {
          m_spriteH->Update(m_scrollBarH, m_scrollBarHMask);
      }
    }
  }
}

TBool WebScrollbarDrawer::canRedraw()
{
    return ((m_webView && m_spriteV && m_spriteH) && 
            (m_spriteV->IsShown() || m_spriteH->IsShown()));    
}

void WebScrollbarDrawer::redrawScrollbar()
{
    if (canRedraw()) {
        clearSprites();
        removeBitmaps();
        InitScrollbar(m_webView);
    }
}

void WebScrollbarDrawer::drawScrollbar(WebView* view)
{
    TPoint p = TPoint(1,1);
    drawScrollbar(view, p);
}

void WebScrollbarDrawer::drawScrollbar(WebView* view, TPoint& scrollPos) 
{
  if (!(view->brCtl()->capabilities() & TBrCtlDefs::ECapabilityDisplayScrollBar)) {
    return;  
  }

  if (m_scrollBarFader->IsActive()) {
    m_scrollBarFader->Cancel();
    clearSprites();
    removeBitmaps();
    m_hasVScroll = EFalse;
    m_hasHScroll = EFalse;
    m_webView = NULL;
  }

  if (!m_webView) {
    InitScrollbar(view);
  }
  
  /*
   * According to spec vertical scrollbar is always visible
   * even if user is not scrolling in vertical direction
   * Horizontal scrollbar is shown only if user scroll in 
   * horizontal direction. If content height < screen height
   * vertical scrollbar is not shown. The same for horizontal one.  
   */
  if (!m_hasHScroll) { //set it only once
    m_hasHScroll = (Abs(scrollPos.iX) > 1) && (m_displayWidth < m_docWidth);
    if (m_hasHScroll) {
        m_spriteH->Update(m_rectHThum.iTl, m_scrollBarH, m_scrollBarHMask);
        
    }
  }
  
  if (!m_hasVScroll) {  //set it only once
    m_hasVScroll = ( (Abs(scrollPos.iY) > 1) || (Abs(scrollPos.iX) > 1) ) && 
                     (m_displayHeight < m_docHeight);
    if (m_hasVScroll) {
        m_spriteV->Update(m_rectVThum.iTl, m_scrollBarV, m_scrollBarVMask);
    }
  }

  if (m_hasVScroll) {
    m_dY += m_zoomFactor * ((float)scrollPos.iY * (float)m_displayHeight ) / (float)m_docHeight / 100;

    if (m_rectVThum.iTl.iY + m_dY < m_rectV.iTl.iY) {
        m_dY = m_rectV.iTl.iY - m_rectVThum.iTl.iY; 
    }
    else if (m_rectVThum.iBr.iY + m_dY > m_rectV.iBr.iY) {
        m_dY = m_rectV.iBr.iY - m_rectVThum.iBr.iY;
    }
    else if ((m_rectVThum.iTl.iY + m_dY == m_rectV.iTl.iY) ||
            (m_rectVThum.iBr.iY + m_dY == m_rectV.iBr.iY)) {
        m_dY = 0.0;        
    }
    
    
    if (Abs(m_dY) > 1) { 
      m_rectVThum.Move(0, m_dY);
      m_dY = 0.0;
    }
    
    m_spriteV->SetPos(m_rectVThum.iTl);
    //m_spriteV->Update(m_rectVThum.iTl, m_scrollBarV, m_scrollBarVMask);
  }
  
  
  if (m_hasHScroll) {
    m_dX = m_zoomFactor * ((float)scrollPos.iX * (float)m_displayWidth ) / (float)m_docWidth / 100;
    
    if (m_rectHThum.iTl.iX + m_dX < m_rectH.iTl.iX) {
        m_dX = m_rectH.iTl.iX - m_rectHThum.iTl.iX; 
    }
    else if (m_rectHThum.iBr.iX + m_dX > m_rectH.iBr.iX) {
        m_dX = m_rectH.iBr.iX - m_rectHThum.iBr.iX;
    }
    else if ((m_rectHThum.iTl.iX + m_dX == m_rectH.iTl.iX) ||
             (m_rectHThum.iBr.iX + m_dX == m_rectH.iBr.iX)) {
        m_dX = 0.0;         
    }
    
    
    if (Abs(m_dX) > 1) {
      m_rectHThum.Move(m_dX, 0);
    }

    m_spriteH->SetPos(m_rectHThum.iTl);
    //m_spriteH->Update(m_rectHThum.iTl, m_scrollBarH, m_scrollBarHMask);
  }  
}


TInt WebScrollbarDrawer::createBitmap(CFbsBitmap*& bm, CFbsBitGc*& gc, 
		                               CFbsBitmapDevice*& dev, TRect& rect)
{
  TInt err = KErrNone;
  bm = new CFbsBitmap();
  if (!bm) {
      err = KErrNoMemory;
  }
  if (err == KErrNone) {
      err = bm->Create(rect.Size(), EColor256);      
  }
  if (err == KErrNone) {
      TRAP(err, dev = CFbsBitmapDevice::NewL(bm));
  }
  if (err == KErrNone && dev != NULL) {
      err = dev->CreateContext(gc);
  }
  if (err != KErrNone) {
      delete dev;
  }
  return err;
}


void WebScrollbarDrawer::deleteBitmap(CFbsBitmap*& bm, CFbsBitGc*& gc, 
		                               CFbsBitmapDevice*& dev)
{
  if (bm) {
    delete bm;
    bm = NULL;
  }
  
  if (gc) {
    delete gc;
    gc = NULL;
  }
	
  if (dev) {
    delete dev;
    dev = NULL;
  }
}

TInt WebScrollbarDrawer::SetupBitmaps()
{
  // if rect is empty don't create bitmap
  TInt err = (m_rectVThum != TRect(0,0,0,0)) ? KErrNone : KErrNotReady;
  if (!m_scrollBarV && err == KErrNone) {
    err = createBitmap(m_scrollBarV, m_gcV, m_devV, m_rectVThum);
  }
  
  if (!m_scrollBarVMask && err == KErrNone) {
    err = createBitmap(m_scrollBarVMask, m_gcVMask, m_devVMask, m_rectVThum);
  }
  
  if (err == KErrNone) {
      drawThumb(m_gcV, m_rectVThum);
      drawThumbMask(m_gcVMask, m_rectVThum);
  }

  // if rect is empty don't create bitmap
  if (err == KErrNotReady || KErrNone) {
      err = (m_rectHThum != TRect(0,0,0,0)) ? KErrNone : KErrNotReady;
  }
  
  if (!m_scrollBarH && err == KErrNone) {
    err = createBitmap(m_scrollBarH, m_gcH, m_devH, m_rectHThum);
  }
    
  if (!m_scrollBarHMask && err == KErrNone) {
    err = createBitmap(m_scrollBarHMask, m_gcHMask, m_devHMask, m_rectHThum);
  }
  
  if (err == KErrNone) {
      drawThumb(m_gcH, m_rectHThum);
      drawThumbMask(m_gcHMask, m_rectHThum);
  }
  
  if (err == KErrNotReady) {
      err = KErrNone;
  }
  return err;
}


void WebScrollbarDrawer::calculateBitmapRects()
{
  m_zoomFactor =  ((float)m_webView->scalingFactor() / 100);
  int delta = 0;
  int posX = m_zoomFactor * m_webView->mainFrame()->frameView()->contentPos().iX;
  int posY = m_zoomFactor * m_webView->mainFrame()->frameView()->contentPos().iY;
  m_docWidth = m_zoomFactor * m_webView->mainFrame()->frameView()->contentSize().iWidth;
  m_displayWidth = m_rect.Width();
  m_docHeight = m_zoomFactor * m_webView->mainFrame()->frameView()->contentSize().iHeight;
  m_displayHeight = m_rect.Height();
    
  m_docHeight = (m_displayHeight > m_docHeight) ? m_displayHeight : m_docHeight;
  m_docWidth = (m_displayWidth > m_docWidth) ? m_displayWidth : m_docWidth;
  
  bool shouldHaveHScroll = (m_displayWidth < m_docWidth); 
  bool shouldHaveVScroll = (m_displayHeight < m_docHeight);
  TPoint vThumbTl;
  TPoint hThumbTl;
  
  if (AknLayoutUtils::LayoutMirrored()) {
    if (shouldHaveHScroll) { //horizontal scroll bar
      delta = shouldHaveVScroll ? m_scrollBarWidth : 0; //to ajust thumb and scrollbar rect
      m_rectH.SetRect(0 + delta, m_rect.Height() - m_scrollBarWidth, m_rect.Width(), m_rect.Height());
      hThumbTl.SetXY(m_scrollBarWidth + (posX * m_rectH.Width()) / m_docWidth, 
	                 m_rect.Height() - m_scrollBarWidth);
    }
    
    if (shouldHaveVScroll) {
      delta = shouldHaveHScroll ? m_scrollBarWidth : 0; //to ajust thumb and scrollbar rect
      m_rectV.SetRect(0, 0, m_scrollBarWidth, m_rect.Height() - delta);
      vThumbTl.SetXY(0, (posY * m_rectV.Height()) / m_docHeight);
    }
  }
  else {
    if (shouldHaveHScroll) {
      delta = shouldHaveVScroll ? m_scrollBarWidth : 0; //to ajust thumb and scrollbar rect
      m_rectH.SetRect(0, m_rect.Height() - m_scrollBarWidth, 
                    m_rect.Width() - delta, m_rect.Height());
      hThumbTl.SetXY((posX * m_rectH.Width()) / m_docWidth, 
                     m_rect.Height() - m_scrollBarWidth);
    }   
    
    if (shouldHaveVScroll) {
      delta = shouldHaveHScroll ? m_scrollBarWidth : 0; //to ajust thumb and scrollbar rect
      m_rectV.SetRect(m_rect.Width() - m_scrollBarWidth, 0, 
   		              m_rect.Width(), m_rect.Height() - delta);
      vThumbTl.SetXY(m_rect.Width() - m_scrollBarWidth, 
                     ((posY * m_rectV.Height()) / m_docHeight));
    }
  }
  
  if (shouldHaveHScroll) {
    int thW = ((float)m_rectH.Width() * m_displayWidth) / m_docWidth;
    thW = (thW < KMinThumbSize) ? KMinThumbSize : thW;
    m_rectHThum.SetRect(hThumbTl, TSize(thW, m_scrollBarWidth));       
    if (m_rectHThum.iBr.iX > m_rectH.iBr.iX) {
      m_rectHThum.Move(m_rectH.iBr.iX - m_rectHThum.iBr.iX, 0);
    }
  }
  
  if (shouldHaveVScroll) {
    int thH = ((float)m_rectV.Height() * m_displayHeight) / m_docHeight;
    thH = (thH < KMinThumbSize) ? KMinThumbSize : thH;
    m_rectVThum.SetRect(vThumbTl, TSize(m_scrollBarWidth, thH));
    if (m_rectVThum.iBr.iY > m_rectV.iBr.iY) {
      m_rectVThum.Move(0, m_rectV.iBr.iY - m_rectVThum.iBr.iY);
    }
  }
}


void WebScrollbarDrawer::drawThumb(CBitmapContext* gc, TRect& rect)
{
  TRect r(TPoint(0, 0), rect.Size());
  gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  gc->SetPenStyle(CGraphicsContext::ESolidPen);
  gc->SetPenSize(TSize(2,2));
  gc->SetPenColor(TRgb(242, 242, 242));
  gc->SetBrushColor(TRgb(10, 10, 10));
  gc->DrawRoundRect(r, TSize(7, 7));
 
}



void WebScrollbarDrawer::drawThumbMask(CBitmapContext* gc, TRect& rect)
{
  TRect r(TPoint(0, 0), rect.Size());
  TRgb  penMaskColor = TRgb(m_scrollBarTransparency, m_scrollBarTransparency, m_scrollBarTransparency);
  TRgb  brushMaskColor = TRgb(m_scrollBarTransparency, m_scrollBarTransparency, m_scrollBarTransparency);;

  gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  gc->SetPenStyle(CGraphicsContext::ESolidPen);
  gc->SetBrushColor(brushMaskColor);
  gc->SetPenSize(TSize(2,2));
  gc->SetPenColor(TRgb(5, 5, 5));
  gc->DrawRoundRect(r, TSize(7, 7));
}

