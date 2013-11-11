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
#include "minimapgenerator.h"

#include <fbs.h>
#include <gdi.h>
#include <bitstd.h>
#include <w32std.h>

#include "minimap.h"
#include "minimaptimer.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KUnscaledBitmapSize = 1024*128; // pixels, = 256kB
const TInt KBufferBitmapSize = 1024*256; // pixels, = 512kB
const TInt KMaxDocWidth = 1600; // limit the maximum width the minimap covers
const TInt KExtraUpdateHeightTop = 100; // hq update this much pixels outsize the view
const TInt KExtraUpdateHeightBottom = 300; // hq update this much pixels outsize the view
const TInt KExtraLQUpdateHeight = 0; // lq update this much pixels outsize the view

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
#ifdef __OOM__
#include <oom.h>

class CMinimapOOMCollector : public CBase, MMemoryCollector
    {
    public:
        CMinimapOOMCollector( CMinimapGenerator* aGenerator )
                : iIsCollecting(EFalse), iGenerator( aGenerator )       { MemoryManager::AddCollector( this ); }
        ~CMinimapOOMCollector()                                         { MemoryManager::RemoveCollector( this ); }
    	TBool IsCollecting()                                            { return iIsCollecting; }

    	TUint Collect(TUint aRequired = 0);
    	void Restore()                                                  { iIsCollecting = EFalse; }
    	TOOMPriority Priority()                                         { return EOOM_PriorityLow; }

    private:
        TBool iIsCollecting;
        CMinimapGenerator* iGenerator;
    };

TUint CMinimapOOMCollector::Collect( TUint /*aRequired*/ )
    {
    iIsCollecting = ETrue;
    iGenerator->CollectMemory();
    return 0;
    }
#endif

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

TInt StartAsyncUpdateCb(
    TAny* aPtr )
    {
    TRAP_IGNORE(static_cast<CMinimapGenerator*>(aPtr)->StartAsyncBufferUpdateL());
    return EFalse;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMinimapGenerator::CMinimapGenerator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMinimapGenerator::CMinimapGenerator(CMinimap& aMinimap)
    : iMinimap(&aMinimap)
    {
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::ConstructL()
    {
    iScaler = CMinimapScaler::NewL(*this);
    iAsyncUpdateStarted = CIdle::NewL(CActive::EPriorityIdle);
#ifdef __OOM__
    iOOMCollector = new (ELeave) CMinimapOOMCollector( this );
#endif
  }

// -----------------------------------------------------------------------------
// CMinimapGenerator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMinimapGenerator* CMinimapGenerator::NewL(CMinimap& aMinimap)
    {
    CMinimapGenerator* self = new( ELeave ) CMinimapGenerator(aMinimap);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CMinimapGenerator::~CMinimapGenerator()
    {
    DeleteUnscaledBitmap();
    DeleteBufferBitmap();
    delete iScaler;
    delete iAsyncUpdateStarted;
    iValidLQRegion.Close();
    iValidHQRegion.Close();
    iVisitedRegion.Close();
#ifdef __OOM__
    delete iOOMCollector;
#endif
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::ScalingCompletedL
//
// This callback is called when a scaling operation completes
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::ScalingCompletedL(CFbsBitmap& aResult, const TRect& aTargetRect)
    {
    if (!iBufferBitmap)
        {
        return;
        }
    TRect target(aTargetRect);
    TRect bufrect(BufferRect());
    // maybe the buffer has been scrolled out while scaling was going on?
    if (target.Intersects(bufrect))
        {
        // update the valid reqion
        iValidHQRegion.AddRect(target);
        iValidHQRegion.ClipRect(bufrect);
        iValidHQRegion.Tidy();

        // blit the newly scaled area to correct position in buffer
        target.Move(-iBufferPos);
        iBufferBitmapGc->BitBlt(target.iTl, &aResult);
        }
    // search for next stripe to update
    TBool more = StartAsyncBufferUpdateL();

    // delete unscaled bitmap if nothing more to do
    if (!more && !iKeepsBitmaps)
        {
        DeleteUnscaledBitmap();
        }

    // signal the update
    iMinimap->Callback().ScaledPageChanged(iMinimap->Rect(), !more /*aReady*/, EFalse);
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::Invalidate
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::Invalidate()
    {
    TSize docSize(iMinimap->Callback().DocumentSize());
    // if doc width changes, assume larger changes and invalidate low quality buffer too
    if (docSize.iWidth!=iDocSize.iWidth || (!iMinimap->IsDocumentComplete() && iMinimap->FullScreenMode()))
        {
        iValidLQRegion.Clear();
//        iVisitedRegion.Clear();
        }
    iDocSize = docSize;
    // otherwise only invalidate high quality areas to avoid lq<->hq flicker
    iValidHQRegion.Clear();
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::UpdateL
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::UpdateL(TBool aScrolling)
    {
    TBool changes = FastBufferUpdateL();

    if( !iMinimap->LowQuality() )
        {
        if (changes && iScaler->IsActive())
            {
            // FastBufferUpdateL uses the same bitmap, scaling op is no longer valid, have to cancel
            iScaler->Cancel();
            }
        // if scaler is already active no need to restart it
        if (iMinimap->IsDocumentComplete() && !iAsyncUpdateStarted->IsActive()
    #ifdef __OOM__
            && !iOOMCollector->IsCollecting()
    #endif
            )
            {
            iAsyncUpdateStarted->Start(TCallBack( &StartAsyncUpdateCb, this));
            }
        }

    if (changes)
        {
        iMinimap->Callback().ScaledPageChanged(iMinimap->Rect(), ETrue, aScrolling);
        }
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::ScrollL
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::ScrollL()
    {
    CalcBufferPosition();
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::Clear
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::Clear()
    {
    iScaler->Cancel();
    iValidLQRegion.Clear();
    iValidHQRegion.Clear();
    iVisitedRegion.Clear();
    if (iBufferBitmapGc)
        {
        iBufferBitmapGc->Clear();
        }
    if (iUnscaledBitmapGc)
        {
        iUnscaledBitmapGc->Clear();
        }
    iBufferPos = TPoint(0,0);
    iDocSize = TSize();
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::CalcBufferPosition
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::CalcBufferPosition()
    {
    if (!iBufferBitmap)
        {
        return;
        }
    TRect vpr(iMinimap->ViewportOnMinimap());
    TPoint vpc(vpr.Center());
    TRect bufrect(BufferRect());
    TSize bufsize(bufrect.Size());
    TPoint newPos(bufrect.iTl);
    TSize mmdocSize(iMinimap->FromDocCoords(iDocSize));

    // check if view is outside center 1/3 of the buffer
    if (vpc.iY<iBufferPos.iY+bufsize.iHeight/3 ||
        vpc.iY>iBufferPos.iY+bufsize.iHeight*2/3 )
        {
        // recalc new pos
        newPos.iY = vpc.iY - bufsize.iHeight/2;
        if (newPos.iY+bufsize.iHeight>mmdocSize.iHeight)
            newPos.iY=mmdocSize.iHeight-bufsize.iHeight;
        if (newPos.iY<0)
            newPos.iY=0;
        }

    // scroll the buffer if needed
    TInt scrollAmount = newPos.iY-iBufferPos.iY;
    if (scrollAmount!=0)
        {
        // check if we need to move bitmap
        if (scrollAmount>0 && scrollAmount<bufsize.iHeight)
            {
            // scroll buffer down
            TRect from (TPoint(0,scrollAmount), TSize(bufsize.iWidth, bufsize.iHeight-scrollAmount));
            // copy area that remains in buffer to a new position
            iBufferBitmapGc->CopyRect(TPoint(0,-scrollAmount), from);
            }
        else if (scrollAmount<0 && (-scrollAmount)<bufsize.iHeight)
            {
            // scroll buffer up
            TRect from (TPoint(0,0), TSize(bufsize.iWidth, bufsize.iHeight+scrollAmount));
            // copy area that remains in buffer to a new position
            iBufferBitmapGc->CopyRect(TPoint(0,-scrollAmount), from);
            }
        // invalidate the areas that are out from the buffer due to scrolling
        iValidLQRegion.ClipRect(bufrect);
        iValidHQRegion.ClipRect(bufrect);
        }
    iBufferPos = newPos;
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::FastBufferUpdate
//
//
// -----------------------------------------------------------------------------
//
TBool CMinimapGenerator::FastBufferUpdateL()
    {
    if (!CheckAndCreateBitmapsL())
        {
        return EFalse;
        }

    TBool changes(EFalse);

    TRect vp(iMinimap->ViewportOnMinimap());
    TRect bufrect(BufferRect());
    TSize unscaledSize(iUnscaledBitmap->SizeInPixels());

    TSize targetSize(iMinimap->FromDocCoords(unscaledSize));

    // divide to unscaled bitmap height stripes
    TInt ypos = ((vp.iTl.iY - KExtraLQUpdateHeight)/targetSize.iHeight)*targetSize.iHeight;
    TInt yend(vp.iBr.iY + KExtraLQUpdateHeight);
    if (ypos<0)
        {
        ypos = 0;
        }
    if (yend>bufrect.iBr.iY)
        {
        yend = bufrect.iBr.iY;
        }
    // loop over the view area, checking if this stripe needs an update
    while (ypos<yend)
        {
        TRect target(TPoint(0,ypos),targetSize);
        TRect from(iMinimap->ToDocCoords(target).iTl,unscaledSize);

        // check if this area is already valid
        RRegion tempR;
        tempR.AddRect(target);
        tempR.ClipRect(bufrect);
        tempR.SubRegion(iValidLQRegion);
        tempR.SubRegion(iValidHQRegion);
        if (!tempR.IsEmpty())
            {
            //  mark area valid
            iValidLQRegion.AddRect(target);
            iValidLQRegion.ClipRect(bufrect);
            iValidLQRegion.Tidy();

            // if not, get the bitmap from client
            iMinimap->Callback().DrawDocumentPart(*iUnscaledBitmapGc,iUnscaledBitmap,from);
            // scale down
            target.Move(-iBufferPos);
            iBufferBitmapGc->DrawBitmap(target,iUnscaledBitmap,TRect(unscaledSize));

            changes = ETrue;
            }
        tempR.Close();

        ypos += targetSize.iHeight;
        }
    return changes;
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::StartAsyncBufferUpdateL
//
//
// -----------------------------------------------------------------------------
//
TBool CMinimapGenerator::StartAsyncBufferUpdateL()
    {

    if (!CheckAndCreateBitmapsL())
        {
        return EFalse;
        }

    TRect vp(iMinimap->ViewportOnMinimap());
    TRect bufrect(BufferRect());
    TSize unscaledSize(iUnscaledBitmap->SizeInPixels());

    TSize targetSize(iMinimap->FromDocCoords(unscaledSize));

    // divide to unscaled bitmap height stripes
    TInt ypos(((vp.iTl.iY - KExtraUpdateHeightTop)/targetSize.iHeight)*targetSize.iHeight);
    TInt yend(vp.iBr.iY + KExtraUpdateHeightBottom);
    if (ypos<bufrect.iTl.iY)
        {
        ypos = bufrect.iTl.iY;
        }
    if (yend>bufrect.iBr.iY)
        {
        yend = bufrect.iBr.iY;
        }
    // loop over the view area, searching for a stripe that needs an update
    while (ypos<yend)
        {
        // update this area
        TRect target(TPoint(0,ypos),targetSize);
        // from here
        TRect from(iMinimap->ToDocCoords(target).iTl,unscaledSize);

        // check if this area is already valid in high quality region
        RRegion tempR;
        tempR.AddRect(target);
        tempR.ClipRect(bufrect);
        tempR.SubRegion(iValidHQRegion);
        if (!tempR.IsEmpty())
            {
            tempR.Close();
            // if not, get the bitmap from client
            iMinimap->Callback().DrawDocumentPart(*iUnscaledBitmapGc,iUnscaledBitmap,from);
            // scale asynchronously
            iScaler->StartScalingL(*iUnscaledBitmap,target);
            // update started, get out
            return ETrue;
            }
        tempR.Close();

        ypos += targetSize.iHeight;
        }
    // nothing to do
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::CheckAndCreateBitmapsL
//
//
// -----------------------------------------------------------------------------
//
TBool CMinimapGenerator::CheckAndCreateBitmapsL()
    {
#ifdef __OOM__
    if( iOOMCollector->IsCollecting() ) return EFalse;
#endif
    TSize docSize(iDocSize);
    // minmap won't cover ridiculously wide document fully in horizontal
    // direction to avoid stripes from getting too wide/low
    if (docSize.iWidth > KMaxDocWidth)
        {
        docSize.iWidth = KMaxDocWidth;
        }
    TSize mmdocSize(iMinimap->FromDocCoords(docSize));
    TSize bufsize;
    TSize unscaledsize;
    if (mmdocSize.iWidth!=0 && docSize.iWidth!=0)
        {
        bufsize = TSize(mmdocSize.iWidth, Min(mmdocSize.iHeight,KBufferBitmapSize/mmdocSize.iWidth));
        unscaledsize = TSize(docSize.iWidth, Min(docSize.iHeight,KUnscaledBitmapSize/docSize.iWidth));
        if (!iUnscaledBitmap || unscaledsize != iUnscaledBitmap->SizeInPixels())
            {
            // cancel scaling since we might delete the bitmap
            iScaler->Cancel();
            }
        }
    TRAPD(err,
        if (iMinimap->CheckAndCreateBitmapL(bufsize,iBufferBitmap,iBufferBitmapDevice,iBufferBitmapGc))
			{
            iMinimap->CheckAndCreateBitmapL(unscaledsize,iUnscaledBitmap,iUnscaledBitmapDevice,iUnscaledBitmapGc);
			}
	)

    if( err != KErrNone )
        {
        DeleteUnscaledBitmap();
        DeleteBufferBitmap();
        }

    return iBufferBitmap!=0 && iUnscaledBitmap!=0;
    }



// -----------------------------------------------------------------------------
// CMinimapGenerator::DeleteUnscaledBitmap
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::DeleteUnscaledBitmap()
    {
    delete iUnscaledBitmapGc;
    delete iUnscaledBitmapDevice;
    delete iUnscaledBitmap;
    iUnscaledBitmapGc = 0;
    iUnscaledBitmapDevice = 0;
    iUnscaledBitmap = 0;
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::DeleteBufferBitmap
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::DeleteBufferBitmap()
    {
    delete iBufferBitmapGc;
    delete iBufferBitmapDevice;
    delete iBufferBitmap;
    iBufferBitmapGc = 0;
    iBufferBitmapDevice = 0;
    iBufferBitmap = 0;
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::BufferRect
//
//
// -----------------------------------------------------------------------------
//
TRect CMinimapGenerator::BufferRect() const
    {
    return TRect(iBufferPos, iBufferBitmap->SizeInPixels());
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::SetKeepsBitmaps
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::SetKeepsBitmaps(TBool aKeepsBitmaps)
    {
    iKeepsBitmaps = aKeepsBitmaps;
    // delete bitmap if no scaling active
    if (!iKeepsBitmaps && !iScaler->IsActive())
        {
        DeleteUnscaledBitmap();
        }
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::KeepsBitmaps
//
//
// -----------------------------------------------------------------------------
//
TBool CMinimapGenerator::KeepsBitmaps() const
    {
    return iKeepsBitmaps;
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::DrawT
//
// Template to support both CWindowGc and CFbsBitGc
// -----------------------------------------------------------------------------
//
template<class GC> void CMinimapGenerator::DrawT(GC& aGc, const TRect& aTo) const
    {
    if (!iBufferBitmap)
        {
        return;
        }

    TRect vp(iMinimap->ViewportOnMinimap());

    TRect from(vp.iTl-iBufferPos, aTo.Size());
    from.Intersection(TRect(iBufferBitmap->SizeInPixels()));

    if (aTo.Size()!=from.Size())
        {
        aGc.SetBrushColor(TRgb(0xff,0xff,0xff));
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        aGc.SetPenStyle(CGraphicsContext::ENullPen);
        aGc.DrawRect(aTo);
        }

    if (!from.IsEmpty())
        {
        aGc.BitBlt(aTo.iTl, iBufferBitmap, from);
        }
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::Draw
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::Draw(CFbsBitGc& aGc, const TRect& aTo) const
    {
    DrawT(aGc,aTo);
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::Draw
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::Draw(CWindowGc& aGc, const TRect& aTo) const
    {
    DrawT(aGc,aTo);
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::DrawColoringMaskT
//
// Template to support both CWindowGc and CFbsBitGc
// -----------------------------------------------------------------------------
//
template<class GC> void CMinimapGenerator::DrawColoringMaskT(GC& aGc, const TRect& aMinimapViewportOnDoc) const
    {
    RRegion temp;
    temp.Copy(iVisitedRegion);
    temp.ClipRect(aMinimapViewportOnDoc);
    aGc.SetBrushColor(TRgb(0, 0, 0));

    for(TInt i = 0; i<temp.Count(); i++)
        {
        TRect vp(temp[i]);
        TRect minimapVp(iMinimap->ViewportOnDocument());
        vp.Move(-minimapVp.iTl); //make vp relative to minimapVp
        TRect res = iMinimap->FromDocCoords(vp); //translate vp to mmap coords
        // borders
        res.Move(1,1);
        // so that view area is within the indicator
        res.Grow(1,1);

        aGc.DrawRect(res);
        }
    temp.Close();
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::DrawColoringMask
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::DrawColoringMask(CFbsBitGc& aGc, const TRect& aMinimapViewportOnDoc) const
    {
    DrawColoringMaskT(aGc,aMinimapViewportOnDoc);
    }


// -----------------------------------------------------------------------------
// CMinimapGenerator::DrawColoringMask
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::DrawColoringMask(CWindowGc& aGc, const TRect& aMinimapViewportOnDoc) const
    {
    DrawColoringMaskT(aGc,aMinimapViewportOnDoc);
    }

// -----------------------------------------------------------------------------
// CMinimapGenerator::VisitArea
//
//
// -----------------------------------------------------------------------------
//
void CMinimapGenerator::VisitArea(const TRect &aArea)
    {
    iVisitedRegion.AddRect(aArea);
    iVisitedRegion.Tidy();
    }

// -----------------------------------------------------------------------------
// CMinimap::ScaledPage
//
// Scaled Page
// -----------------------------------------------------------------------------
//
CFbsBitmap* CMinimapGenerator::ScaledPage() const
    {
    return iBufferBitmap;
    }

#ifdef __OOM__
TBool CMinimapGenerator::IsCollectingMemory()
    {
    return iOOMCollector->IsCollecting();
    }

void CMinimapGenerator::CollectMemory()
    {
    if( iScaler->IsActive() ) iScaler->Cancel();
    DeleteUnscaledBitmap();
    DeleteBufferBitmap();

    iMinimap->DeleteMinimapBitmap();
    }
#endif

// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
