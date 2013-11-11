/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "minimap.h"

#include <fbs.h>
#include <gdi.h>
#include <bitstd.h>
#include <w32std.h>
#include <coecntrl.h>

#include "minimapgenerator.h"
#include "minimaptimer.h"

// MACROS

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KDefaultZoomOutPercent = 325; // 3.0x minimap zoom out by default

//const TInt KDefaultTransparency = 15; // % transparent
// This is the transparency that is the starting point in fade effect
const TInt KDefaultTransparency = 95; // % transparent
// This is the transparency that is visible when the fade effect is finished and the minimap is on
const TInt KDefaultViewingTransparency = 15; // % transparent
// Higher the KFadeEffectSpeed, the faster the fade effect. This tells directly 
// how much the percentage amount the transparency of the minimap is changed 
// every 0.03 second when the fade effect is shown.
// The suitable value depends on the performance of the hardware
// but if you consider a pure emulator, 5 is fine for the KFadeEffectSpeed
// but for example in Nokia E50 device something like 10 looks about the same.
const TInt KFadeEffectSpeed = 5;
// How often updated
const TInt KUpdateFreq = 30000;// 0.03 second

const TInt KDefaultWidthPercent = 67;
const TInt KDefaultHeightPercent = 78;
const TInt KDefaultOffsetX = 0;
const TInt KDefaultOffsetY = 3;
const CPageScaler::TPosition KDefaultPosition = CPageScaler::ETopRight;


const TInt KBorderWidthPermille = 20; // width of the border
const TInt KMaxBorderAlpha = 128; // how dark is the shadow around the minimap
// defines a box relative to minimap edges. if view center goes outside this box
// the view is recentered (50%==always center)
const TInt KHScrollAreaPercent = 33; // horizontal distance in percent
const TInt KVScrollAreaPercent = 50; // verical distance in percent
const TInt KHScrollAreaPercentWithTouch1 = 20; // horizontal distance in percent when to scrol
const TInt KVScrollAreaPercentWithTouch1 = 20; // verical distance in percent when to scrol
const TInt KHScrollAreaPercentWithTouch2 = 25; // horizontal distance in percent amount to scroll (scroll to the right only)
const TInt KVScrollAreaPercentWithTouch2 = 25; // verical distance in percent amount to scroll (scroll up only)
const TInt KUpdateDelayComplete = 45000000; // 45s
const TInt KUpdateDelayLoading = 7000000; // 7s
const TInt KUpdateDelayFullScreen = 4000000; // 4s
const TInt KUpdateCbDelayComplete = 100000; // 0.1s
const TInt KUpdateCbDelayLoading = 1000000; // 1s

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMinimap:CMinimap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMinimap::CMinimap(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality)
    : iCallback(&aCallback),
      iZoomOutLevel(KDefaultZoomOutPercent),
      iTransparency(KDefaultTransparency),
      iDisplayMode(aMode),
      iWidthPercent(KDefaultWidthPercent),
      iHeightPercent(KDefaultHeightPercent),
      iMinimapUpdating( EFalse ),
      iLowQuality( aLowQuality )
    {
    }

// -----------------------------------------------------------------------------
// CMinimap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMinimap::ConstructL()
    {
    iGenerator = CMinimapGenerator::NewL(*this);
    iUpdateCbTimer = CMinimapTimer::NewL(*this, &CMinimap::UpdateCbTimerCbL, CActive::EPriorityStandard - 1 );
    iUpdateTimer = CMinimapTimer::NewL(*this, &CMinimap::UpdateTimerCbL);
    iVisibilityTimer = CMinimapTimer::NewL(*this, &CMinimap::VisibilityTimerCbL);
    iFadeTimer = CMinimapTimer::NewL(*this, &CMinimap::UpdateTransparency);
    SetRelativePosition(KDefaultOffsetX,KDefaultOffsetY,KDefaultPosition);
    }

// -----------------------------------------------------------------------------
// CMinimap::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMinimap* CMinimap::NewL(MPageScalerCallback& aCallback, TDisplayMode aMode, TBool aLowQuality)
    {
    CMinimap* self = new( ELeave ) CMinimap(aCallback, aMode, aLowQuality);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CMinimap::~CMinimap()
    {
    delete iGenerator;
    delete iUpdateCbTimer;
    delete iUpdateTimer;
    delete iVisibilityTimer;
    delete iFadeTimer;
    delete iMinimapBitmapGc;
    delete iMinimapBitmapDevice;
    delete iMinimapBitmap;
    
    delete iMinimapMaskBitmapGc;
    delete iMinimapMaskBitmapDevice;
    delete iMinimapMaskBitmap;
    
    iSprite.Close();
    
#if ENABLE_MINIMAP_COLORING
    delete iMinimapColoringBitmap;
    delete iMinimapColoringBitmapMask;
    delete iMinimapColoringBitmapMaskDevice;
    delete iMinimapColoringBitmapMaskGc;
#endif
    }


// -----------------------------------------------------------------------------
// CMinimap::SetRelativePosition
//
//
// -----------------------------------------------------------------------------
//
void CMinimap::SetRelativePosition(TInt aXDist, TInt aYDist, TPosition aPos)
    {
    //negative == relative to right/bottom edge
    iXDist = (aPos==ETopRight || aPos==EBottomRight)?-aXDist-1:aXDist;
    iYDist = (aPos==EBottomLeft || aPos==EBottomRight)?-aYDist-1:aYDist;
    }


// -----------------------------------------------------------------------------
// CMinimap::SetVisibleUntil
//
//
// -----------------------------------------------------------------------------
//
void CMinimap::SetVisibleUntil(TTimeIntervalMicroSeconds32 aUntil)
    {
    iVisibilityTimer->Start(aUntil);
    if (!iVisible)
        {
        SetVisible(ETrue);
        iCallback->ScaledPageChanged(Rect(), EFalse, EFalse);
        }
        iFadeDirection = ETrue;
        UpdateTransparency();
    }

void CMinimap::UpdateTransparency()
{
    if (iFullScreenMode) return;
  
	TInt delay = KUpdateFreq;
	iFadeTimer->Cancel();
	if (iFadeDirection) 
	{
		if (iTransparency > KDefaultViewingTransparency) 
		{
			iTransparency -= KFadeEffectSpeed;
			iFadeTimer->Start(delay);
			iCallback->ScaledPageChanged(Rect(), EFalse, EFalse);
		}
		else 
		{
			iTransparency = KDefaultViewingTransparency;
		}
	}
	else 
	{
		if (iTransparency < KDefaultTransparency) 
		{
			iTransparency += KFadeEffectSpeed;
			iFadeTimer->Start(delay);
			iCallback->ScaledPageChanged(Rect(), EFalse, EFalse);
		}
		else 
		{
			iTransparency = KDefaultTransparency;
			iVisible = EFalse;
			iCallback->ScaledPageChanged(Rect(), EFalse, EFalse);
		}
	}
    }

// -----------------------------------------------------------------------------
// CMinimap::Rect
//
//
// -----------------------------------------------------------------------------
//
TRect CMinimap::Rect() const
    {
    if (iFullScreenMode)
        {
        return iContainerRect;
        }

    if (!iMinimapBitmap)
        {
        return TRect();
        }

    TSize ressize(iMinimapBitmap->SizeInPixels());

    TPoint respoint;
    respoint.iX = iXDist<0?
        iContainerRect.Width()+iXDist-ressize.iWidth+1:iXDist;
    respoint.iY = iYDist<0?
        iContainerRect.Height()+iYDist-ressize.iHeight+1:iYDist;

    return TRect(respoint, ressize);
    }

// -----------------------------------------------------------------------------
// CMinimap::MinimapRect
//
// Rectangle containing the actual minimap (excluding borders)
// -----------------------------------------------------------------------------
//
TRect CMinimap::MinimapRect() const
    {
    TSize s;
    if (iFullScreenMode)
        {
        s = CalcSize();
        //center
        TPoint p((iContainerRect.Width()-s.iWidth)/2,(iContainerRect.Height()-s.iHeight)/2);
        return TRect(p,s);
        }
    else if (iMinimapBitmap)
        {
        s = iMinimapBitmap->SizeInPixels();
        }
    TInt bw = BorderWidth();
    if (s.iWidth<=bw*2 || s.iHeight<=bw*2)
        {
        return TRect();
        }
    // borders
    s -= TSize(bw*2,bw*2);
    return TRect(TPoint(bw,1),s);
    }


// -----------------------------------------------------------------------------
// CMinimap::IndicatorRect
//
// Calculate the main view indicator on minimap
// -----------------------------------------------------------------------------
//
TRect CMinimap::IndicatorRect() const
    {
    TRect vp(iCallback->DocumentViewport());
    TRect minimapVp(ViewportOnDocument());
    vp.Move(-minimapVp.iTl); //make vp relative to minimapVp
    TRect res = FromDocCoords(vp); //translate vp to mmap coords
    // borders
    res.Move(MinimapRect().iTl);
    // so that view area is within the indicator
    res.Grow(1,1);
    // ensure it is within the bounds
    TRect mr = MinimapRect();
    if (res.iTl.iX<mr.iTl.iX)
        {
        res.iTl.iX = mr.iTl.iX;
        }
    if (res.iTl.iY<mr.iTl.iY)
        {
        res.iTl.iY = mr.iTl.iY;
        }
    if (res.iBr.iX>mr.iBr.iX)
        {
        res.iBr.iX = mr.iBr.iX;
        }
    if (res.iBr.iY>mr.iBr.iY)
        {
        res.iBr.iY = mr.iBr.iY;
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CMinimap::DocumentStarted
//
// Event from client: new document is being loaded
// -----------------------------------------------------------------------------
//
void CMinimap::DocumentStarted()
    {
    iDocumentComplete = EFalse;
    iViewportOnDocument = TRect();
    iUpdateTimer->Cancel();
    if ( iUpdateCbTimer->IsActive() )
        {
        UpdateNow();
        }
    iVisibilityTimer->Cancel();
    if ( iFadeDirection )
        {
        VisibilityTimerCbL();
        }
    iGenerator->Clear();
    TRAP_IGNORE(CheckAndCreateMinimapBitmapsL());
    iNeedsUpdate = EFalse;
    // keep bitmaps during loading to avoid constant realloc
    iGenerator->SetKeepsBitmaps(ETrue);
    iGenerator->VisitArea(iCallback->DocumentViewport());
    }

// -----------------------------------------------------------------------------
// CMinimap::DocumentChanged
//
// Event from client: changes in document
// -----------------------------------------------------------------------------
//
void CMinimap::DocumentChangedL()
    {
    TTimeIntervalMicroSeconds32 delay(
        iDocumentComplete?KUpdateCbDelayComplete:KUpdateCbDelayLoading);
    iUpdateCbTimer->Start( delay );
    }

// -----------------------------------------------------------------------------
// CMinimap::DocumentChangedCbL
//
// Event from client: changes in document
// -----------------------------------------------------------------------------
//
void CMinimap::DocumentChangedCbL()
    {
#ifdef __OOM__
    // disable updating when memory is low
    if( iGenerator->IsCollectingMemory() ) return;
#endif

    iGenerator->Invalidate();
    if( !iLowQuality )
        {
        CheckAndCreateMinimapBitmapsL();
        if (iUpdateTimer->IsActive())
            {
            // timer running, update when it completes
            iNeedsUpdate = ETrue;
            }
        else
            {
            if (iCallback->DocumentSize().iHeight>5)
                {
                iViewportOnDocument = CalcViewportOnDocument();
                iGenerator->UpdateL();
                iNeedsUpdate = EFalse;
                // don't do another update for..
                TTimeIntervalMicroSeconds32 delay(
                    iFullScreenMode?KUpdateDelayFullScreen:iDocumentComplete?KUpdateDelayComplete:KUpdateDelayLoading);
                iUpdateTimer->Start(delay);
                }
            }
        }
    else
        {
        iNeedsUpdate = ETrue;

        // don't start updating if the minimap is not visible
        if( iVisible )
            {
            if (iCallback->DocumentSize().iHeight>5)
                {
                iViewportOnDocument = CalcViewportOnDocument();
                iGenerator->UpdateL();
                iUpdateTimer->Start( 0 );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMinimap::DocumentViewportMoved
//
// Event from client: view position changed
// -----------------------------------------------------------------------------
//
void CMinimap::DocumentViewportMoved()
    {
    iViewportOnDocument = CalcViewportOnDocument();
#if ENABLE_MINIMAP_COLORING
    iGenerator->VisitArea(iCallback->DocumentViewport());
#endif
    iGenerator->ScrollL();
    if ( iVisible )
        {
        TRAP_IGNORE(iGenerator->UpdateL(ETrue));
        }
    else if( !iLowQuality )
        {
        iNeedsUpdate = ETrue;
        if (!iUpdateTimer->IsActive())
            {
            TTimeIntervalMicroSeconds32 delay(
                iDocumentComplete?KUpdateDelayComplete:KUpdateDelayLoading);
            iUpdateTimer->Start(delay);
            }
        }
    else
        iNeedsUpdate = ETrue;
    }

// -----------------------------------------------------------------------------
// CMinimap::SetMaxSize
//
// set maximum size for the minimap
// -----------------------------------------------------------------------------
//
void CMinimap::SetPercentSize(TInt aWidthPercent, TInt aHeightPercent)
    {
    iWidthPercent = aWidthPercent;
    iHeightPercent = aHeightPercent;
    TRAP_IGNORE(CheckAndCreateMinimapBitmapsL());
    }

// -----------------------------------------------------------------------------
// CMinimap::DocumentCompleted
//
// Event from client: document loading completed
// -----------------------------------------------------------------------------
//
void CMinimap::DocumentCompleted()
    {
    // this event is triggered by the minimap updating, not by 
    // the main document.
    if( iMinimapUpdating ) return;

    iNeedsUpdate = ETrue;
    iDocumentComplete = ETrue;
    
    if( iVisible || !iLowQuality )
        {
        iUpdateTimer->Start(0);
        }
    
    // we can delete the buffers now
    iGenerator->SetKeepsBitmaps(EFalse);
    }

// -----------------------------------------------------------------------------
// CMinimap::ViewportOnMinimap
//
// Area the minimap view covers in minimap coordinates
// -----------------------------------------------------------------------------
//
TRect CMinimap::ViewportOnMinimap() const
    {
    return FromDocCoords(iViewportOnDocument);
    }


// -----------------------------------------------------------------------------
// CMinimap::ViewportOnDocument
//
// Area the minimap view covers in document coordinates
// -----------------------------------------------------------------------------
//
TRect CMinimap::ViewportOnDocument() const
    {
    return iViewportOnDocument;
    }

// -----------------------------------------------------------------------------
// CMinimap::CalcSize
//
// Calculate minimap view size
// -----------------------------------------------------------------------------
//
TSize CMinimap::CalcSize() const
    {
    TSize res, max;
    TSize mms = FromDocCoords(iCallback->DocumentSize());
    if (iFullScreenMode)
        {
        max = iContainerRect.Size();
        }
    else
        {
        TSize s( iContainerRect.Size() );
        s.iWidth = s.iWidth * iWidthPercent / 100;
        s.iHeight = s.iHeight * iHeightPercent / 100;
        max = s ;
        }
    TInt bw = BorderWidth();
    res.iWidth = Min(mms.iWidth+2*bw,max.iWidth);
    res.iHeight = Min(mms.iHeight+2*bw,max.iHeight);
    return res;
    }


// -----------------------------------------------------------------------------
// CMinimap::CalcViewportOnDocument
//
// Calculate the area the minimap view covers in document coordinates
// -----------------------------------------------------------------------------
//
TRect CMinimap::CalcViewportOnDocument() const
    {
    TPoint mvp(iViewportOnDocument.iTl);

    TRect docvp(iCallback->DocumentViewport());
    TSize docs(iCallback->DocumentSize());
    TSize ms(ToDocCoords(MinimapRect().Size()));
    TPoint docc(docvp.Center());

    if (!iCallback->TouchScrolling())
        {
        // scroll if not within 1/3 center area
        // check x-direction
        if (docc.iX<mvp.iX+ms.iWidth*KHScrollAreaPercent/100 ||
            docc.iX>mvp.iX+ms.iWidth*(100-KHScrollAreaPercent)/100)
            {
            // far enough from the center, re-center the view
            mvp.iX = docc.iX-ms.iWidth/2;
            if (mvp.iX+ms.iWidth>docs.iWidth)
                {
                mvp.iX=docs.iWidth-ms.iWidth;
                }
            if (mvp.iX<0)
                {
                mvp.iX=0;
                }
            }
        // y-direction
        if (docc.iY<mvp.iY+ms.iHeight*KVScrollAreaPercent/100 ||
            docc.iY>mvp.iY+ms.iHeight*(100-KVScrollAreaPercent)/100)
            {
            // far enough from the center, re-center the view
            mvp.iY = docc.iY-ms.iHeight/2;
            if (mvp.iY+ms.iHeight>docs.iHeight)
                {
                mvp.iY=docs.iHeight-ms.iHeight;
                }
            if (mvp.iY<0)
                {
                mvp.iY=0;
                }
            }
        }
    else
        {
        // check x-direction
        TBool moved = EFalse;
        if (docc.iX<mvp.iX+ms.iWidth*KHScrollAreaPercentWithTouch1/100)
            {
            mvp.iX = docc.iX-ms.iWidth*KHScrollAreaPercentWithTouch2/100;
            moved = ETrue;
            }
        else if (docc.iX>mvp.iX+ms.iWidth*(100-KHScrollAreaPercentWithTouch1)/100)
            {
            mvp.iX = docc.iX-ms.iWidth*(100-KHScrollAreaPercentWithTouch1)/100;
            moved = ETrue;
            }
        if (moved)
            {
            if (mvp.iX+ms.iWidth>docs.iWidth)
                {
                mvp.iX=docs.iWidth-ms.iWidth;
                }
            if (mvp.iX<0)
                {
                mvp.iX=0;
                }
            }
        // y-direction
        moved = EFalse;
        if (docc.iY<mvp.iY+ms.iHeight*KVScrollAreaPercentWithTouch1/100)
            {
            mvp.iY = docc.iY-ms.iHeight*KVScrollAreaPercentWithTouch2/80;
            moved = ETrue;
            }
        else if (docc.iY>mvp.iY+ms.iHeight*(100-KVScrollAreaPercentWithTouch1)/100)
            {
            mvp.iY = docc.iY-ms.iHeight*(100-KVScrollAreaPercentWithTouch1)/100;
            moved = ETrue;
            }

        if (moved)
            {
            if (mvp.iY+ms.iHeight>docs.iHeight)
                {
                mvp.iY=docs.iHeight-ms.iHeight;
                }
            if (mvp.iY<0)
                {
                mvp.iY=0;
                }
            }
        }
    return TRect(mvp,ms);
    }



template<class GC> void CMinimap::DrawT(GC& aGc, const TRect& /*aRect*/) const
    {
    TRect r(MinimapRect());
    aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    iGenerator->Draw(aGc, r);

#if ENABLE_MINIMAP_COLORING
    // coloring

    // build up the mask
    TRect mmapVpOnDoc(ViewportOnDocument());
    iMinimapColoringBitmapMaskGc->SetBrushColor(TRgb(60, 60, 60));
    iMinimapColoringBitmapMaskGc->DrawRect(iMinimapColoringBitmap->SizeInPixels());
    iGenerator->DrawColoringMask(*iMinimapColoringBitmapMaskGc, mmapVpOnDoc);

    // paint the coloring
    aGc.AlphaBlendBitmaps(r.iTl,
                     iMinimapColoringBitmap,
                     iMinimapColoringBitmap->SizeInPixels(),
                     iMinimapColoringBitmapMask,
                     TPoint(0,0));
#endif

    // draw the position indicator
    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetPenSize(TSize(1,1));
    aGc.SetPenColor(TRgb(255,0,0));
    aGc.DrawRect(IndicatorRect());
    }

// -----------------------------------------------------------------------------
// CMinimap::Draw
//
//
// -----------------------------------------------------------------------------
//
void CMinimap::Draw(CWindowGc& aGc, const TRect& aRect) const
    {
    if (iFullScreenMode)
        {
        // clear the background
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.SetBrushColor(TRgb(220,220,255));
        aGc.SetPenColor(TRgb(220,220,255));
        aGc.DrawRect(iContainerRect);

        // get the bitmap
        DrawT(aGc,aRect);
        }
    else
        {
        TRect rectToDraw(Rect());
        // draw the minimap only if the document is larger than the screen
        TSize docsize(iGenerator->DocSize());
        if (iVisible && iMinimapBitmap && aRect.Intersects(rectToDraw)
            && (docsize.iWidth>iContainerRect.Width() || docsize.iHeight>iContainerRect.Height()))
            {
            // get the bitmap
            DrawT(*iMinimapBitmapGc,aRect);

            // set the transparency to the alpha channel
            iMinimapBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeAND);
            iMinimapBitmapGc->SetPenStyle(CGraphicsContext::ENullPen);
            iMinimapBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
            iMinimapBitmapGc->SetBrushColor(TRgb(0xff,0xff,0xff,255*(100-iTransparency)/100));
            iMinimapBitmapGc->DrawRect(MinimapRect());

            // blit to screen
            aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
            aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
            aGc.BitBlt(rectToDraw.iTl, iMinimapBitmap);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMinimap::FromDocCoords
//
// Coordinate space translation from document to minimap
// -----------------------------------------------------------------------------
//
TRect CMinimap::FromDocCoords(const TRect& aFrom) const
    {
    return TRect(FromDocCoords(aFrom.iTl),FromDocCoords(aFrom.Size()));
    }

// -----------------------------------------------------------------------------
// CMinimap::FromDocCoords
//
// Coordinate space translation from document to minimap
// -----------------------------------------------------------------------------
//
TPoint CMinimap::FromDocCoords(const TPoint& aFrom) const
    {
    TPoint res;
    res.iX = aFrom.iX*100/iZoomOutLevel;
    res.iY = aFrom.iY*100/iZoomOutLevel;
    return res;
    }

// -----------------------------------------------------------------------------
// CMinimap::FromDocCoords
//
// Coordinate space translation from document to minimap
// -----------------------------------------------------------------------------
//
TSize CMinimap::FromDocCoords(const TSize& aFrom) const
    {
    TSize res;
    res.iWidth = aFrom.iWidth*100/iZoomOutLevel;
    res.iHeight = aFrom.iHeight*100/iZoomOutLevel;
    return res;
    }

// -----------------------------------------------------------------------------
// CMinimap::ToDocCoords
//
// Coordinate space translation from minimap to document
// -----------------------------------------------------------------------------
//
TPoint CMinimap::ToDocCoords(const TPoint& aFrom) const
    {
    TPoint res;
    res.iX = aFrom.iX*iZoomOutLevel/100;
    res.iY = aFrom.iY*iZoomOutLevel/100;
    return res;
    }


// -----------------------------------------------------------------------------
// CMinimap::ToDocCoords
//
// Coordinate space translation from minimap to document
// -----------------------------------------------------------------------------
//
TSize CMinimap::ToDocCoords(const TSize& aFrom) const
    {
    TSize res;
    res.iWidth = aFrom.iWidth*iZoomOutLevel/100;
    res.iHeight = aFrom.iHeight*iZoomOutLevel/100;
    return res;
    }

// -----------------------------------------------------------------------------
// CMinimap::ToDocCoords
//
// Coordinate space translation from minimap to document
// -----------------------------------------------------------------------------
//
TRect CMinimap::ToDocCoords(const TRect& aFrom) const
    {
    return TRect(ToDocCoords(aFrom.iTl),ToDocCoords(aFrom.Size()));
    }
    
    

// -----------------------------------------------------------------------------
// CMinimap::CheckAndCreateMinimapBitmapsL
//
// Creates mask and buffer if needed
// -----------------------------------------------------------------------------
//
void CMinimap::CheckAndCreateMinimapBitmapsL()
    {
    // low quality minimap only updates when it is visible
    if( !iVisible && iLowQuality ) 
        return;

    TSize minimapSize(CalcSize());
    if (!iFullScreenMode)
        {
        TDisplayMode temp = iDisplayMode;
        iDisplayMode = EColor16MA;
        TSize oldsize;
        if (iMinimapBitmap)
            {
            oldsize = iMinimapBitmap->SizeInPixels();
            }
        if (minimapSize!=oldsize)
            {
            CheckAndCreateBitmapL( minimapSize,iMinimapBitmap,iMinimapBitmapDevice,iMinimapBitmapGc);
            if (iMinimapBitmap)
                {
                // draw shadowed border
                iMinimapBitmapGc->Clear();
                TRect r(minimapSize);
                iMinimapBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
                iMinimapBitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
                iMinimapBitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
                // wider shadow to bottom, no shadow to top
                TInt bw = BorderWidth();
                for (TInt n=1;n<=2*bw-1;n++)
                    {
                    iMinimapBitmapGc->SetPenColor(TRgb(0,0,0,n*n*KMaxBorderAlpha/(2*2*bw*bw)));
                    iMinimapBitmapGc->DrawRect(r);
                    r.iTl.iX += n%2;
                    r.iBr.iX -= n%2;
                    r.iBr.iY -= 1;
                    }
                iMinimapBitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
                }
            //create mask
            CheckAndCreateBitmapL( minimapSize, iMinimapMaskBitmap,iMinimapMaskBitmapDevice,iMinimapMaskBitmapGc);
            if (iMinimapMaskBitmap)
                {
                    //Draw mask
                iMinimapMaskBitmapGc->Clear();
                TRect r(minimapSize);
                iMinimapMaskBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
                iMinimapMaskBitmapGc->SetPenStyle( CGraphicsContext::ENullPen );
                iMinimapMaskBitmapGc->SetPenColor( TRgb( 30, 30, 30 ) );
                iMinimapMaskBitmapGc->SetBrushColor( TRgb( 30, 30, 30 ) );
                iMinimapMaskBitmapGc->DrawRect( r );
    
                iMinimapMaskBitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
                iMinimapMaskBitmapGc->SetBrushStyle(CGraphicsContext::ENullBrush);
                
                // wider shadow to bottom, no shadow to top
                TInt bw = BorderWidth();
                for (TInt n=1;n<=2*bw-1;n++)
                    {
                    TInt grade = 85*n/(2*bw);
                    iMinimapMaskBitmapGc->SetPenColor(TRgb(255-grade,255-grade,255-grade));
                    iMinimapMaskBitmapGc->DrawRect(r);
                    r.iTl.iX += n%2;
                    r.iBr.iX -= n%2;
                    r.iBr.iY -= 1;
                    }
                }
            }
        iDisplayMode = temp;
        }
#if ENABLE_MINIMAP_COLORING
    InitColoringL(MinimapRect().Size());
#endif
    }


// -----------------------------------------------------------------------------
// CMinimap::CheckAndCreateBitmapL
//
// Creates bitmap, device and mask if needed
// -----------------------------------------------------------------------------
//
TBool CMinimap::CheckAndCreateBitmapL(TSize aSize, CFbsBitmap*& aBm, CFbsBitmapDevice*& aDev, CFbsBitGc*& aGc)
    {
    if ( aSize.iWidth==0 || aSize.iHeight==0 )
        {
        // delete bitmap if there was one
        delete aGc;
        delete aDev;
        delete aBm;
        aGc = 0;
        aDev = 0;
        aBm = 0;
        return EFalse;
        }
    else
        {
        if ( aBm && aSize != aBm->SizeInPixels() )
            {
            // resize if different size
            User::LeaveIfError(aDev->Resize(aSize));
            aGc->Resized();
            }
        else if ( !aBm )
            {
            // create new
            CFbsBitmap* bm = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bm);
            User::LeaveIfError(bm->Create(aSize,iDisplayMode));
            CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bm);
            CleanupStack::PushL(dev);
            User::LeaveIfError(dev->CreateContext(aGc));
            aDev = dev;
            aBm = bm;
            CleanupStack::Pop(2);
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CMinimap::InitColoring
//
// Creates (if needed) and initializes bitmaps for mmap coloring
// -----------------------------------------------------------------------------
//
#if ENABLE_MINIMAP_COLORING
void CMinimap::InitColoringL(const TSize aSize)
    {
    if(!iMinimapColoringBitmap || aSize!=iMinimapColoringBitmap->SizeInPixels())
        {
        delete iMinimapColoringBitmap;
        iMinimapColoringBitmap = 0;
        iMinimapColoringBitmap = new (ELeave) CFbsBitmap();

        User::LeaveIfError(iMinimapColoringBitmap->Create(aSize, iDisplayMode));
        CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iMinimapColoringBitmap);
        CleanupStack::PushL(device);
        CFbsBitGc* gc;
        User::LeaveIfError(device->CreateContext(gc));

        gc->SetPenStyle(CGraphicsContext::ENullPen);
        gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
        gc->SetBrushColor(KRgbYellow);
        gc->DrawRect(aSize);

        delete gc;
        gc = 0;
        CleanupStack::PopAndDestroy(device);
        device = 0;
        }

    if(iMinimapColoringBitmapMask && aSize != iMinimapColoringBitmapMask->SizeInPixels())
        {
        User::LeaveIfError(iMinimapColoringBitmapMaskDevice->Resize(aSize));
        iMinimapColoringBitmapMaskGc->Resized();
        }
    else if(!iMinimapColoringBitmapMask)
        {
        iMinimapColoringBitmapMask = new (ELeave) CFbsBitmap();
        User::LeaveIfError(iMinimapColoringBitmapMask->Create(aSize, EGray256));
        iMinimapColoringBitmapMaskDevice = CFbsBitmapDevice::NewL(iMinimapColoringBitmapMask);
        User::LeaveIfError(iMinimapColoringBitmapMaskDevice->CreateContext(iMinimapColoringBitmapMaskGc));
        }

    iMinimapColoringBitmapMaskGc->SetPenStyle(CGraphicsContext::ENullPen);
    iMinimapColoringBitmapMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    }
#else
void CMinimap::InitColoringL(const TSize) { }
#endif

// -----------------------------------------------------------------------------
// CMinimap::SetTransparency
//
// Timer callback
// -----------------------------------------------------------------------------
//
void CMinimap::SetTransparency(TInt aPercent)
    {
    if (iTransparency!=aPercent)
        {
        iTransparency = aPercent;
        }
    }


// -----------------------------------------------------------------------------
// CMinimap::UpdateCbTimerCbL
//
// Timer callback
// -----------------------------------------------------------------------------
//
void CMinimap::UpdateCbTimerCbL()
    {
    iMinimapUpdating = ETrue;
    DocumentChangedCbL();
    iMinimapUpdating = EFalse;
    }
// -----------------------------------------------------------------------------
// CMinimap::UpdateTimerCbL
//
// Timer callback
// -----------------------------------------------------------------------------
//
void CMinimap::UpdateTimerCbL()
    {
    iMinimapUpdating = ETrue;

#ifdef __OOM__
    if( iGenerator->IsCollectingMemory() ) return;
#endif
    if (iNeedsUpdate)
        {
        iViewportOnDocument = CalcViewportOnDocument();
        CheckAndCreateMinimapBitmapsL();
        iGenerator->UpdateL();
        }
    iNeedsUpdate = EFalse;
    iMinimapUpdating = EFalse;
    }

// -----------------------------------------------------------------------------
// CMinimap::VisibilityTimerCbL
//
// Timer callback
// -----------------------------------------------------------------------------
//
void CMinimap::VisibilityTimerCbL()
    {
    iFadeDirection = EFalse;
    iSprite.Close();
    UpdateTransparency();
    }

// -----------------------------------------------------------------------------
// CMinimap::ScaledPage
//
// Scaled Page
// -----------------------------------------------------------------------------
//
 CFbsBitmap* CMinimap::ScaledPage() const
    {
    return iGenerator->ScaledPage();
    }

// -----------------------------------------------------------------------------
// CMinimap::ContainerRect
//
// Rectangle used to detemine the paint area
// -----------------------------------------------------------------------------
//
TRect CMinimap::ContainerRect() const
    {
    return iContainerRect;
    }


// -----------------------------------------------------------------------------
// CMinimap::SetContainerRect
//
// Set the rectangle used to detemine the paint area
// -----------------------------------------------------------------------------
//
void CMinimap::SetContainerRect(const TRect& aRect)
    {
    iContainerRect = aRect;
    iViewportOnDocument = CalcViewportOnDocument();
    TRAP_IGNORE(CheckAndCreateMinimapBitmapsL());
    }


// -----------------------------------------------------------------------------
// CMinimap::FullScreenMode
//
// Paint full area
// -----------------------------------------------------------------------------
//
TBool CMinimap::FullScreenMode() const
    {
    return iFullScreenMode;
    }

// -----------------------------------------------------------------------------
// CMinimap::SetFullScreenMode
//
// Paint full area
// -----------------------------------------------------------------------------
//
void CMinimap::SetFullScreenMode(TBool aFullScreenMode)
    {
    iFullScreenMode = aFullScreenMode;
    iViewportOnDocument = CalcViewportOnDocument();
    TRAP_IGNORE(CheckAndCreateMinimapBitmapsL());
    if (iFullScreenMode) 
        {
        iVisibilityTimer->Cancel();
        }
    else 
        {
        iVisibilityTimer->StartOrContinue();
        }
    }

// -----------------------------------------------------------------------------
// CMinimap::ConstructSprite
//
//
// -----------------------------------------------------------------------------
//
void CMinimap::ConstructSprite()
    {
    CCoeControl& view = iCallback->PageControlView();
    iSprite = RWsSprite(view.ControlEnv()->WsSession());
    RWindowTreeNode *window =  (RDrawableWindow* )view.DrawableWindow();
    iSprite.Construct(*window,Rect().iTl,ESpriteNoChildClip);

    TSpriteMember spriteMem;
    spriteMem.iBitmap = iMinimapBitmap;
    spriteMem.iMaskBitmap = iMinimapMaskBitmap;
    spriteMem.iInvertMask = ETrue;

    iSprite.AppendMember(spriteMem);
    iSprite.Activate();
    }

// -----------------------------------------------------------------------------
// CPageOverview::SetVisible
//
//
// -----------------------------------------------------------------------------
//
void CMinimap::SetVisible(TBool aVisible)
    {
    if (aVisible && !iVisible)
        {
        iVisible = aVisible;
        UpdateNow();
        if(!iFullScreenMode)
            {
            ConstructSprite();
            }
        }
    iVisible = aVisible;
    
    if(!aVisible && !iFullScreenMode)
        {
        iSprite.Close();
        }
    
    }

// -----------------------------------------------------------------------------
// CPageOverview::UpdateNow
//
// Force update
// -----------------------------------------------------------------------------
//
void CMinimap::UpdateNow()
    {
    if (iUpdateCbTimer->IsActive())
        {
        iUpdateCbTimer->Cancel();
        iUpdateTimer->Cancel();
        DocumentChangedCbL();
        }
    else
        {
        iUpdateTimer->Cancel();
        UpdateTimerCbL();
        }
    }


TInt CMinimap::BorderWidth() const
    {
    return KBorderWidthPermille*Max(iContainerRect.Width(),iContainerRect.Height())/1000;
    }

#ifdef __OOM__
void CMinimap::DeleteMinimapBitmap()
    {
    // disable updating
    if (iUpdateCbTimer->IsActive())
        {
        iUpdateCbTimer->Cancel();
        iUpdateTimer->Cancel();
        }

    // delete the minimap
    delete iMinimapBitmap;
    iMinimapBitmap = 0;
    delete iMinimapBitmapDevice;
    iMinimapBitmapDevice = 0;
    delete iMinimapBitmapGc;
    iMinimapBitmapGc = 0;
    }
#endif
//  End of File
