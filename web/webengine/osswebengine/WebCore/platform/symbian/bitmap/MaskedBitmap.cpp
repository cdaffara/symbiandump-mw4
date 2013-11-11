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
* Description:  Default implementation of bitmap class.
*
*/


// INCLUDE FILES
#include "MaskedBitmap.h"
#include "BitmapTiler.h"
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>


// ======================== STATIC FACTORY FUNCTIONS ===========================
#define GET_PIX_GRAY2(buf, x, y, w)     (((TUint32*)((TUint8*)buf + y*w))[x>>5] & (1<<(x&0x1f)))


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMaskedBitmap::CopyBitmap
// -----------------------------------------------------------------------------
TInt BitmapUtil::CopyBitmap( const CFbsBitmap& aSource, CFbsBitmap& aDestination )
    {
    TSize size( aSource.SizeInPixels() );
    TDisplayMode displayMode( aSource.DisplayMode() );
    TInt err( aDestination.Create( size, displayMode ) );
    if( !err )
        {
        err = BitmapUtil::CopyBitmapData( aSource, aDestination, size, displayMode );
        if( err )
            {
            aDestination.Reset();
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::CopyBitmapData
// -----------------------------------------------------------------------------
TInt BitmapUtil::CopyBitmapData( const CFbsBitmap& aSource, CFbsBitmap& aDestination,
                                   const TSize& aSize, const TDisplayMode& aDisplayMode )
    {
    HBufC8* scanLine = HBufC8::New( aSource.ScanLineLength( aSize.iWidth, aDisplayMode ) );
    if( scanLine )
        {
        TPtr8 scanPtr( scanLine->Des() );
        TPoint pp;
        for( pp.iY = 0; pp.iY < aSize.iHeight; ++pp.iY )
            {
            aSource.GetScanLine( scanPtr, pp, aSize.iWidth, aDisplayMode );
            aDestination.SetScanLine( scanPtr, pp.iY );
            }

        delete scanLine;
        return KErrNone;
        }
    return KErrNoMemory; // scanLine alloc failed
    }
    
// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CMaskedBitmap::CMaskedBitmap()
    {
    }


CMaskedBitmap::CMaskedBitmap(CFbsBitmap* aBitmap, CFbsBitmap* aMask)
: iBitmap(aBitmap),iMask(aMask)
    {
    }

// ----------------------------------------------------------------------------
//
// Second phase constructors. Can leave.
// -----------------------------------------------------------------------------
void CMaskedBitmap::ConstructL()
    {
    iBitmap = new (ELeave) CFbsBitmap;
    iMask = new (ELeave) CFbsBitmap;
    }

// -----------------------------------------------------------------------------
//
// Two-phased constructors.
// -----------------------------------------------------------------------------
CMaskedBitmap* CMaskedBitmap::NewL()
    {
    CMaskedBitmap* self = new( ELeave ) CMaskedBitmap;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMaskedBitmap::~CMaskedBitmap()
    {
    delete iBitmap;
    delete iMask;
    delete iScaledBitmap;
    }


// -----------------------------------------------------------------------------
// CMaskedBitmap::CreateCopyMode
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::CreateCopyingDisplayMode( const TSize& aSize, const CMaskedBitmap& aCopyFrom )
    {
    if (aCopyFrom.HasMask())
        {
        return Create(aSize,aCopyFrom.Bitmap().DisplayMode(),aCopyFrom.Mask().DisplayMode());
        }
    else
        {
        return Create(aSize,aCopyFrom.Bitmap().DisplayMode());
        }
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Create
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::Create( const TSize& aSize, TDisplayMode aDisplayMode )
    {
    Reset();
    return iBitmap->Create( aSize, aDisplayMode );
    }


// -----------------------------------------------------------------------------
// CMaskedBitmap::Create
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::Create( const TSize& aSize, TDisplayMode aBitmapDisplayMode,
                           TDisplayMode aMaskDisplayMode )
    {
    Reset();
    TInt err( iBitmap->Create( aSize, aBitmapDisplayMode ) );
    if( err )
        {
        return err;
        }
    err = iMask->Create( aSize, aMaskDisplayMode );
    if( err )
        {
        iBitmap->Reset();
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Copy
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::Copy( const CFbsBitmap& aBitmap, TBool aDuplicate )
    {
    Reset();

    TInt err( KErrNone );
    TInt bitmapHandle( aBitmap.Handle() );
    if( bitmapHandle )
        {
        if( aDuplicate )
            {
            err = iBitmap->Duplicate( bitmapHandle );
            }
        else
            {
            err = BitmapUtil::CopyBitmap( aBitmap, *iBitmap );
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Copy
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::Copy( const CFbsBitmap& aBitmap, const CFbsBitmap& aMask, TBool aDuplicate )
    {
    Reset();

    TInt bitmapHandle( aBitmap.Handle() );
    TInt maskHandle( aMask.Handle() );
    if( bitmapHandle && maskHandle &&
        aBitmap.SizeInPixels() != aMask.SizeInPixels() )
        {
        return KErrArgument;
        }

    TInt err( KErrNone );
    if( bitmapHandle )
        {
        if( aDuplicate )
            {
            err = iBitmap->Duplicate( bitmapHandle );
            }
        else
            {
            err = BitmapUtil::CopyBitmap( aBitmap, *iBitmap );
            }
        }

    if( !err && maskHandle )
        {
        if( aDuplicate )
            {
            err = iMask->Duplicate( maskHandle );
            }
        else
            {
            err = BitmapUtil::CopyBitmap( aMask, *iMask );
            }
        if( err )
            {
            iBitmap->Reset();
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Copy
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::Copy( const CMaskedBitmap& aBitmap, TBool aDuplicate )
    {
    return Copy( aBitmap.Bitmap(), aBitmap.Mask(), aDuplicate );
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Reset
// -----------------------------------------------------------------------------
void CMaskedBitmap::Reset()
    {
    iBitmap->Reset();
    iMask->Reset();
    }


// -----------------------------------------------------------------------------
// CMaskedBitmap::IsCreated
// -----------------------------------------------------------------------------
TBool CMaskedBitmap::IsCreated() const
    {
    return ( iBitmap->Handle() != 0 );
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Bitmap
// -----------------------------------------------------------------------------
const CFbsBitmap& CMaskedBitmap::Bitmap() const
    {
    return *iBitmap;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::HasMask
// -----------------------------------------------------------------------------
TBool CMaskedBitmap::HasMask() const
    {
    return ( iMask->Handle() != 0 );
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Mask
// -----------------------------------------------------------------------------
const CFbsBitmap& CMaskedBitmap::Mask() const
    {
    return *iMask;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::BitBlt
// -----------------------------------------------------------------------------
void CMaskedBitmap::BitBlt( CFbsBitGc& aContext, const TPoint& aPoint ) const
    {
    TSize s(iBitmap->SizeInPixels());
    if (!(s.iWidth>0 && s.iHeight>0))
        {
        return;
        }
    if( iBitmap->Handle() )
        {
        if( iMask->Handle() )
            {
            aContext.BitBltMasked( aPoint, iBitmap, s, iMask, iInvertMask );
            }
        else
            {
            aContext.BitBlt( aPoint, iBitmap, s );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::BitBlt
// -----------------------------------------------------------------------------
void CMaskedBitmap::BitBlt( CFbsBitGc& aContext, const TPoint& aPoint, const TRect& aSource ) const
    {
    if (!(aSource.Width()>0 && aSource.Height()>0))
        {
        return;
        }
    if( iBitmap->Handle() )
        {
        if( iMask->Handle() )
            {

            aContext.BitBltMasked( aPoint, iBitmap, aSource, iMask, iInvertMask );
            }
        else
            {
            aContext.BitBlt( aPoint, iBitmap, aSource );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::DrawBitmap
// -----------------------------------------------------------------------------
void CMaskedBitmap::DrawBitmap( CFbsBitGc& aContext, const TRect& aTarget ) const
    {
    TSize s(iBitmap->SizeInPixels());
    if (!(aTarget.Width()>0 && aTarget.Height()>0 && s.iWidth>0 && s.iHeight>0))
        {
        return;
        }
    if( iBitmap->Handle() )
        {
        // ### FIXME DrawBitmapMasked is too buggy to use 2.8/week52, so no transparency with scaling
        if( iMask->Handle() )
            {
            aContext.DrawBitmapMasked( aTarget, iBitmap, s, iMask, iInvertMask );
            }
        else
            {
            aContext.DrawBitmap( aTarget, iBitmap );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::DrawBitmap
// -----------------------------------------------------------------------------
void CMaskedBitmap::DrawBitmap( CFbsBitGc& aContext, const TRect& aTarget, const TRect& aSource) const
    {
    if (!(aSource.Width()>0 && aSource.Height()>0 && aTarget.Width()>0 && aTarget.Height()>0))
        {
        return;
        }
    if( iBitmap->Handle() )
        {
        // ### FIXME DrawBitmapMasked is too buggy to use 2.8/week52, so no transparency with scaling
        if( iMask->Handle() )
            {
            aContext.DrawBitmapMasked( aTarget, iBitmap, aSource, iMask, iInvertMask );
            }
        else
            {
            aContext.DrawBitmap( aTarget, iBitmap, aSource );
            }
        }
    }

// Internal interface
// -----------------------------------------------------------------------------
// CMaskedBitmap::SetFilename
// -----------------------------------------------------------------------------
CFbsBitmap& CMaskedBitmap::BitmapModifyable()
    {
    return *iBitmap;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::SetFilename
// -----------------------------------------------------------------------------
CFbsBitmap& CMaskedBitmap::MaskModifyable()
    {
    return *iMask;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::SetFrameIndex
// -----------------------------------------------------------------------------
void CMaskedBitmap::SetFrameIndex( TInt aFrameIndex )
    {
    iFrameIndex = aFrameIndex;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::SetFrameDelay
// -----------------------------------------------------------------------------
void CMaskedBitmap::SetFrameDelay( TTimeIntervalMicroSeconds32 aFrameDelay )
    {
    iFrameDelay = aFrameDelay;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::FrameIndex
// -----------------------------------------------------------------------------
TInt CMaskedBitmap::FrameIndex() const
    {
    return iFrameIndex;
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::FrameDelay
// -----------------------------------------------------------------------------
TTimeIntervalMicroSeconds32 CMaskedBitmap::FrameDelay() const
    {
    return iFrameDelay;
    }

// Private methods

// -----------------------------------------------------------------------------
// CMaskedBitmap::SizeInPixels
// -----------------------------------------------------------------------------
TSize CMaskedBitmap::SizeInPixels() const
    {
    return iBitmap->SizeInPixels();
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::Resize
// -----------------------------------------------------------------------------
void CMaskedBitmap::Resize(TSize aSize)
    {
    iBitmap->Resize(aSize);
    if (iMask)
        {
        iMask->Resize(aSize);
        }
    }

// -----------------------------------------------------------------------------
// CMaskedBitmap::TileInBitmapRect
// -----------------------------------------------------------------------------
void CMaskedBitmap::TileInBitmapRect( CFbsBitGc& gc, const TRect& bmpRect, const TPoint& srcPt )
    {
    if (!HasMask()) 
        {
        TRAP_IGNORE( 
            CFbsBitGc* copy = CFbsBitGc::NewL();
            CleanupStack::PushL( copy );
            copy->Activate( (CFbsDevice*) gc.Device() );
            copy->CopySettings( gc );
            copy->UseBrushPattern(iBitmap);
            copy->SetPenStyle(CGraphicsContext::ENullPen);
            copy->SetBrushStyle(CGraphicsContext::EPatternedBrush);
            copy->SetBrushOrigin(srcPt);
            copy->DrawRect(bmpRect);
            copy->DiscardBrushPattern();
            CleanupStack::PopAndDestroy( copy );
        );
        }
    else 
        {
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
        TPoint off(srcPt.iX, srcPt.iY);
        for (TInt x = off.iX; x<bmpRect.iBr.iX; x+=SizeInPixels().iWidth) 
            {
            for (TInt y = off.iY; y<bmpRect.iBr.iY; y+=SizeInPixels().iHeight) 
                {
                TRect rect(TPoint(x,y), SizeInPixels());
                BitBlt(gc, rect.iTl);
                }
            }
        }
    }

void CMaskedBitmap::TileInBitmapRect( CFbsBitmap* trgBmp, const TRect& aTrgRect, const TPoint& aOffset )
    {
    trgBmp->LockHeap();

    TSize sz = trgBmp->SizeInPixels();
    TUint32* buf = (TUint32*)(trgBmp->DataAddress());

    // bitmap with 32-bit depth doesn't need padding for a scanline.
    TInt padding = trgBmp->DisplayMode() == EColor16MU ? 0 : (sz.iWidth & 1);

    TInt32 stride = padding;
    stride += sz.iWidth - aTrgRect.Width();

    // calculate the offset in image space
    TRect srcRect( TPoint(0,0), iBitmap->SizeInPixels() );
    const TSize& imgSz = srcRect.Size();
    TPoint offset( aOffset.iX % imgSz.iWidth, aOffset.iY % imgSz.iHeight );

    // prepare to fetch scanline
    TBool fastAccess = EFalse;
    if( !iBitmap->IsCompressedInRAM() && ( !HasMask() || !iMask->IsCompressedInRAM() ) )
        fastAccess = ETrue;

    CScanLineFetcher* fetcher = 0;

    // possible OOM
    TRAPD( err,
        if( fastAccess )
            fetcher = CFastScanLineFetcher::NewL( this, offset.iX, offset.iY );
        else
            fetcher = CCompressedScanLineFetcher::NewL( this, offset.iX, offset.iY );
        );
    if( err )
        {
        trgBmp->UnlockHeap();
        return;
        }

    if( trgBmp->DisplayMode() == EColor16MU )
        {
        // starting address in trgBmp
        buf += (TUint32)( aTrgRect.iTl.iX + aTrgRect.iTl.iY * ( sz.iWidth + padding ) );

        if( HasMask() )
            {
            if( iMask->DisplayMode() == EGray2 )
                {
                // 1-bit mask
                TScanLineTiler<TSrcScanLineMasked, TUint32*> tiler( aTrgRect.Width(), offset.iX );
                TileBitmap<TSrcScanLineMasked, TUint32*>( buf, stride, *fetcher, tiler, aTrgRect.Height(),
                    srcRect.Width(), srcRect.Height(), offset );
                }
            else
                {
                // alpha blending
                TScanLineTiler<TSrcScanLineAlpha, TUint32*> tiler( aTrgRect.Width(), offset.iX );
                TileBitmap<TSrcScanLineAlpha, TUint32*>( buf, stride, *fetcher, tiler, aTrgRect.Height(),
                    srcRect.Width(), srcRect.Height(), offset );
                }
            }
        else
            {
            // direct drawing
            TScanLineTiler<TSrcScanLine, TUint32*> tiler( aTrgRect.Width(), offset.iX );
            TileBitmap<TSrcScanLine, TUint32*>( buf, stride, *fetcher, tiler, aTrgRect.Height(),
                srcRect.Width(), srcRect.Height(), offset );
            }
        }
    else if( trgBmp->DisplayMode() == EColor64K )
        {
        // starting address in trgBmp
        TUint16* buf16 = (TUint16*)buf;
        buf16 += (TUint32)( aTrgRect.iTl.iX + aTrgRect.iTl.iY * ( sz.iWidth + padding ) );
        if( HasMask() )
            {
            if( iMask->DisplayMode() == EGray2 )
                {
                // 1-bit mask
                TScanLineTiler<TSrcScanLineMasked, TUint16*> tiler( aTrgRect.Width(), offset.iX );
                TileBitmap<TSrcScanLineMasked, TUint16*>( buf16, stride, *fetcher, tiler, aTrgRect.Height(),
                    srcRect.Width(), srcRect.Height(), offset );
                }
            else
                {
                // alpha blending
                TScanLineTiler<TSrcScanLineAlpha, TUint16*> tiler( aTrgRect.Width(), offset.iX );
                TileBitmap<TSrcScanLineAlpha, TUint16*>( buf16, stride, *fetcher, tiler, aTrgRect.Height(),
                    srcRect.Width(), srcRect.Height(), offset );
                }
            }
        else
            {
            // direct drawing
            TScanLineTiler<TSrcScanLine, TUint16*> tiler( aTrgRect.Width(), offset.iX );
            TileBitmap<TSrcScanLine, TUint16*>( buf16, stride, *fetcher, tiler, aTrgRect.Height(),
                srcRect.Width(), srcRect.Height(), offset );
            }
        }

    delete fetcher;
    trgBmp->UnlockHeap();
    }


TBool CMaskedBitmap::IsFullyTransparent()
    {
    if (!iMask || !iMask->Handle())
        {
        return EFalse;
        }

    if( iMask->DisplayMode() != EGray2 )
        {
        return EFalse; // fully transparent png? rare, dont bother
        }

    TUint32* msk = iMask->DataAddress();
    TSize mskSz = iMask->SizeInPixels();
    TInt mskScanLineW = iMask->ScanLineLength( mskSz.iWidth, iMask->DisplayMode() );

    TUint32 trgMask;

    iMask->LockHeap();

    for( TInt y=0; y<mskSz.iHeight; ++y )
        {
        for( TInt x=0; x<mskSz.iWidth; ++x )
            {
            trgMask = GET_PIX_GRAY2(msk, x, y, mskScanLineW);

            if (trgMask)
                {
                return EFalse;
                }
            }
        }

    iMask->UnlockHeap();
    return ETrue;
    }

void CMaskedBitmap::CompressInBackground( )
    {
    
		if ( iBitmap && iBitmap->Handle() ) 
			iBitmap->CompressInBackground();
		
        if ( iMask && iMask->Handle() )
            iMask->CompressInBackground();
        
    }

CMaskedBitmap* CMaskedBitmap::ScaleImageToSize( TSize newSize )
{
    if (iScaledBitmap && iScaledBitmap->SizeInPixels() == newSize)
        return iScaledBitmap;

    delete iScaledBitmap;
    iScaledBitmap = NULL;
    iScaledBitmap = CMaskedBitmap::NewL();

    CFbsBitmap& bmp = iScaledBitmap->BitmapModifyable();
    bmp.Create(newSize, iBitmap->DisplayMode());

    // scale the image quickly
    CFbsBitGc* bitGc;
    CFbsBitmapDevice* bitDevice = CFbsBitmapDevice::NewL( &bmp );
    CleanupStack::PushL( bitDevice );
    User::LeaveIfError( bitDevice->CreateContext( bitGc ) );
    CleanupStack::PushL( bitGc );
    bitGc->DrawBitmap(TRect(TPoint(0,0), newSize), iBitmap, iBitmap->SizeInPixels());
    CleanupStack::PopAndDestroy(2);

    CFbsBitmap& msk = iScaledBitmap->MaskModifyable();
    if( HasMask() ) {
        msk.Create(newSize, iMask->DisplayMode());

        // scale the image quickly
        CFbsBitGc* bitGc;
        CFbsBitmapDevice* bitDevice = CFbsBitmapDevice::NewL( &msk );
        CleanupStack::PushL( bitDevice );
        User::LeaveIfError( bitDevice->CreateContext( bitGc ) );
        CleanupStack::PushL( bitGc );
        bitGc->DrawBitmap(TRect(TPoint(0,0), newSize), iMask, iMask->SizeInPixels());
        CleanupStack::PopAndDestroy(2);
    }

    return iScaledBitmap;
}

//  End of File
