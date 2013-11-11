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


#ifndef WEBSCROLLBARDRAWER_H
#define WEBSCROLLBARDRAWER_H

#include <e32base.h>
#include <coedef.h>
#include <coecntrl.h>
#include <w32std.h>
#include <gdi.h>


class WebView;
class CWebSprite;

class WebScrollbarDrawer: public CBase
{
  public:
    static WebScrollbarDrawer* NewL();
    virtual ~WebScrollbarDrawer();

    void drawScrollbar(WebView* view, TPoint& scrollPos);
    void drawScrollbar(WebView* view);
    int  getTransparency() const {return m_scrollBarTransparency;}
    void fadeScrollbar(TInt delay);
    void fadeScrollbar();
    void fade();   
    int fadeTime();
    void redrawScrollbar();
    
  private:
    WebScrollbarDrawer();
    void ConstructL(); 
    TInt InitScrollbar(WebView* view);
    TInt SetupBitmaps();
    
    void drawThumbMask(CBitmapContext* gc, TRect& rect);
    void drawThumb(CBitmapContext* gc, TRect& rect);
    TInt createBitmap(CFbsBitmap*& bm, CFbsBitGc*& gc, 
    	               CFbsBitmapDevice*& dev, TRect& rect);
    void deleteBitmap(CFbsBitmap*& bm, CFbsBitGc*& gc, 
    	              CFbsBitmapDevice*& dev);
    void removeBitmaps();
    void clearSprites();
    void calculateBitmapRects();
    TBool canRedraw();
    
  private:
    WebView*              m_webView;
    CWebSprite*           m_spriteV;
    CWebSprite*           m_spriteH;
    TRect                 m_rect;
    int                   m_scrollBarTransparency;
    CPeriodic*            m_scrollBarFader;
    
    CFbsBitmap*           m_scrollBarV;
    CFbsBitGc*            m_gcV;
    CFbsBitmapDevice*     m_devV;
    
    CFbsBitmap*           m_scrollBarVMask;
    CFbsBitGc*            m_gcVMask;
    CFbsBitmapDevice*     m_devVMask;
    
    CFbsBitmap*           m_scrollBarH;
    CFbsBitGc*            m_gcH;
    CFbsBitmapDevice*     m_devH;
    
    CFbsBitmap*           m_scrollBarHMask;
    CFbsBitGc*            m_gcHMask;
    CFbsBitmapDevice*     m_devHMask;
    
       
    bool                  m_hasVScroll;
    bool                  m_hasHScroll;
    int                   m_scrollBarWidth;
    TRect                 m_rectV;
    TRect                 m_rectVThum;
    TRect                 m_rectH;
    TRect                 m_rectHThum;
    int                   m_docWidth;
    int                   m_displayWidth;
    int                   m_docHeight;
    int                   m_displayHeight;
    float                 m_zoomFactor;
    TRgb                  m_thumColor;
    float                 m_dY;
    float                 m_dX;
};


#endif //WEBSCROLLBARDRAWER_H
