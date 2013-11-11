/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of CHuiTextureAnimationState.
*
*/



#include <e32base.h>
#include <fbs.h>
#include <bitstd.h>
#include <uiacceltk/HuiUtil.h>
#include "huitextureanimationstate.h"

/**
 * Modifiable bitmap helper class for CHuiTextureAnimationState.
 */
NONSHARABLE_CLASS( CHuiModifiableBitmap ) : public CBase
    {
public:
    /**
     * Two-phased constructor. This creates new bitmap, 
     * this instance owns it until @c ReleaseBitmap is called.
     * @param aSize size of bitmap.
     * @param aMode display mode of bitmap.
     */
    static CHuiModifiableBitmap* NewLC( const TSize& aSize, TDisplayMode aMode );
    
    /**
     * Two-phased constructor. Use another bitmap and ownership
     * is not taken.
     * @param aBitmap bitmap to be modified.
     */
    static CHuiModifiableBitmap* NewLC( CFbsBitmap& aBitmap );
    
    /**
     * Destructor.
     */
    ~CHuiModifiableBitmap();

    /**
     * Returns graphics context. Ownership is not passed.
     * @return graphics context.
     */
    CFbsBitGc& Context();    
    
    /**
     * Releases bitmap and passes ownership to caller.
     * After this has been called, bitmap operations should not
     * be performed through this class.
     * @return bitmap.
     */
    CFbsBitmap* ReleaseBitmap();
    
    /**
     * Clears the whole bitmap with specified color.
     * @param aColor color to use.
     */
    void Clear( const TRgb& aColor );
    
    /**
     * Clears area of bitmap with specified color.
     * @param aRect area to fill.
     * @param aColor color to use.
     */
    void Clear( const TRect& aRect, const TRgb& aColor );
        
    /**
     * Copies area from another bitmap to this one.
     * @param aRect area to copy.
     * @param aBitmap source bitmap.
     */
    void BitBlt( const TRect& aRect, CHuiModifiableBitmap& aBitmap );
    
    /**
     * Combines source and mask bitmap to this 16MA bitmap.
     * @param aSrc bitmap.
     * @param aSrcMask mask bitmap.
     */
    void Combine( const CFbsBitmap& aSrc, const CFbsBitmap& aSrcMask );
    
    /**
     * Combines source and mask bitmap to this 16MA bitmap.
     * @para aPos position of bitmap.
     * @param aSrc bitmap.
     * @param aSrcMask mask bitmap.
     */
    void Combine( const TPoint& aPos, const CFbsBitmap& aSrc, const CFbsBitmap& aSrcMask );
    
    /**
     * Combines gray 256 mask to this gray 256 bitmap. 
     * It's expected that aMask contains only 0 or 255 values.
     * @param aRect rect to combine.
     * @param aMask gray 256 bitmap.
     */
    void CombineMasks( const TRect& aRect, CFbsBitmap* aMask );
    
    /**
     * Extracts alpha from 16MA source to this EGray256 bitmap.
     * @param aSource source bitmap.
     */
    void ExtractAlpha( CHuiModifiableBitmap& aSource );
    
    /**
     * Extracts color from 16MA source to this EColor64K/EColor16MU bitmap.
     * @param aSource source bitmap.
     */
    void ExtractColor( CHuiModifiableBitmap& aSource );
        
private:
    void ConstructL( const TSize& aSize, TDisplayMode aMode );
    void ConstructL( CFbsBitmap& aBitmap );

    template<class Converter>
    static void DoExtractColor( CFbsBitmap* aTarget, CFbsBitmap* aSource );
    
    struct Convert16MA_16MU
        {    
        inline static TUint32 Convert( TUint32 aPixel );
        };
    struct Convert16MA_64K
        {
        inline static TUint32 Convert( TUint32 aPixel );
        };
    
private:
    /**
     * Boolean value indicating if iBitmap is owned by this class.
     */
    TBool iOwnBitmap;
    /**
     * Bitmap instance. Ownership depends on value of iOwnBitmap.
     */
    CFbsBitmap* iBitmap;
    /**
     * Bitmap device.
     * Own.
     */
    CFbsBitmapDevice* iDevice;
    /**
     * Graphics context.
     * Own.
     */
    CFbsBitGc* iGc;
    };

CHuiTextureAnimationState* CHuiTextureAnimationState::NewL(
        TInt aTextureGroupId, const TDesC& aImageFile, TInt aFrameCount )
    {
    CHuiTextureAnimationState* self = new (ELeave) CHuiTextureAnimationState;
    CleanupStack::PushL( self );
    self->ConstructL( aTextureGroupId, aImageFile, aFrameCount );
    CleanupStack::Pop( self );
    return self;
    }

CHuiTextureAnimationState::~CHuiTextureAnimationState()
    {
    HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Destroyed"), this );
    delete iFile;
    
    delete iPrevFrame;
    delete iPrevFrameMask;
    }

TInt CHuiTextureAnimationState::OwnerTextureGroupId() const
    {
    return iTextureGroupId;
    }
    
TBool CHuiTextureAnimationState::CheckIfCanProduce( 
        const TDesC& aImageFile, TInt aFrameNumber, TInt aFrameCount ) const
    {
    aFrameNumber--;
    
    TBool ok = ( aFrameCount == iFrameCount ) && 
               ( iFrame <= aFrameNumber ) &&
               !aImageFile.CompareF( *iFile );
               
    return ok; 
    }

TInt CHuiTextureAnimationState::GetNextFrameNumber() const
    {
    return ( iFrame + 1 );
    }

void CHuiTextureAnimationState::OfferNextFrameInfo( 
        const TFrameInfo& aFrameInfo )
    {
    if ( iFrame == KErrNotFound )
        {
        // First frame defines size of all frames; subsequent frames
        // are deltas to first frame.
        iSize = aFrameInfo.iOverallSizeInPixels;
        }
        
    iNextSubFrameRect = aFrameInfo.iFrameCoordsInPixels;
    iNextSubFrameBgColor = aFrameInfo.iBackgroundColor;
    
    if ( aFrameInfo.iFlags & TFrameInfo::ERestoreToPrevious )
        {
        iNextSubFrameDisposalAction = ERestoreToPrevious;
        }
    else if ( aFrameInfo.iFlags & TFrameInfo::ELeaveInPlace )
        {
        // The variable name "'ELeave'InPlace" will cause FALSE positive in CodeScanners "Leave scan"
        iNextSubFrameDisposalAction = ELeaveInPlace;
        }
    else if ( aFrameInfo.iFlags & TFrameInfo::ERestoreToBackground )
        {
        iNextSubFrameDisposalAction = ERestoreToBackgroundColour;
        }
    else
        {
        iNextSubFrameDisposalAction = EUnspecified;
        }
        
    // If EAlphaChannel flag is on, EGray256 style alpha is available.
    // If ETransparencyPossible flag is on, just EGray2.
    iNextSubFrameHasAlpha = aFrameInfo.iFlags & TFrameInfo::EAlphaChannel;
    }

TSize CHuiTextureAnimationState::OverallSize() const
    {
    return iSize;
    }

void CHuiTextureAnimationState::ProduceNextFrameL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    HUI_DEBUG2(_L("CHuiTextureAnimationState(%x): Produce next %d"), 
        this, iFrame + 1 );
    HUI_DEBUG4(_L("CHuiTextureAnimationState(%x): pf:%x pfm:%x d:%d"), 
        this, iPrevFrame, iPrevFrameMask, iNextSubFrameDisposalAction );
    aNewFrame = NULL;
    aNewFrameMask = NULL;
    CheckSubFrameL( aSubFrame, aSubFrameMask );
    
    if ( iFrame == KErrNotFound )
        {
        // Check if previous frames can be reused.
        const TSize frameSize = OverallSize();
        if ( iPrevFrame )
            {
            if ( iPrevFrame->SizeInPixels() != frameSize ||
                 iPrevFrame->DisplayMode() != aSubFrame->DisplayMode() )
                {
                delete iPrevFrame;
                iPrevFrame = NULL;
                }
            }
        if ( iPrevFrameMask && aSubFrameMask )
            {
            if ( iPrevFrameMask->SizeInPixels() != frameSize ||
                 iPrevFrameMask->DisplayMode() != aSubFrameMask->DisplayMode() )
                {
                delete iPrevFrameMask;
                iPrevFrameMask = NULL;
                }
            }
        else
            {
            delete iPrevFrameMask;
            iPrevFrameMask = NULL;
            }

        if ( !iNextSubFrameHasAlpha || !aSubFrameMask )
            {
            HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Quick 1st"), this );
            QuickProduceFirstFrameAndGeneratePreviousL( 
                aNewFrame, aNewFrameMask, 
                aSubFrame, aSubFrameMask );
            }
        else
            {           
            HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Slow 1st"), this );
            ProduceFirstFrameAndGeneratePreviousL( 
                aNewFrame, aNewFrameMask, 
                aSubFrame, aSubFrameMask );
            }
            
        if ( !aNewFrameMask )
            {
            // So mask was not needed - get rid of previous (if any)
            delete iPrevFrameMask;
            iPrevFrameMask = NULL;
            }
        }
    else
        {
        if ( !iPrevFrame )
            {
            User::Leave( KErrNotSupported );
            }
            
        if ( !iNextSubFrameHasAlpha || 
             !aSubFrameMask || 
             !iPrevFrameMask )
            {
            HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Quick 2-"), this );
            QuickProduceNextFrameAndUpdatePreviousL(
                aNewFrame, aNewFrameMask,
                aSubFrame, aSubFrameMask );
            }
        else
            {
            HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Slow 2-"), this );
            ProduceNextFrameAndUpdatePreviousL(
                aNewFrame, aNewFrameMask,
                aSubFrame, aSubFrameMask );
            }
        }
        
    iFrame++; // Go to next frame
        
    if ( iFrame == ( iFrameCount - 1 ) )
        {
        // Reset back to the beginning
        iFrame = KErrNotFound;
        }
    HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Produce next done"), this );        
    }

void CHuiTextureAnimationState::ProceedWithoutNextFrameL(
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    // This could be optimized. However, if this method is called,
    // it means that client is anyway using the API in very performance
    // inefficient way, i.e. asking system to load random frames rather
    // than in sequence 0, 1, ..., N.
    
    CFbsBitmap* frame = NULL;
    CFbsBitmap* frameMask = NULL;
    
    ProduceNextFrameL( frame, frameMask, aSubFrame, aSubFrameMask );
    
    delete frame;
    delete frameMask;
    }

CHuiTextureAnimationState::CHuiTextureAnimationState()
    {
    HUI_DEBUG1(_L("CHuiTextureAnimationState(%x): Created"), this );
    }
    
void CHuiTextureAnimationState::ConstructL( 
        TInt aTextureGroupId, const TDesC& aImageFile, TInt aFrameCount )
    {
    iTextureGroupId = aTextureGroupId;
    iFile = aImageFile.AllocL();
    iFrame = KErrNotFound;
    iFrameCount = aFrameCount;
    }

void CHuiTextureAnimationState::ProduceFirstFrameAndGeneratePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    if ( !aSubFrameMask )
        {
        User::Leave( KErrNotSupported );
        }
        
    const TSize frameSize = OverallSize();

    // Make scratch
    CHuiModifiableBitmap* scratch = 
        CHuiModifiableBitmap::NewLC( frameSize, EColor16MA );
    scratch->Combine( iNextSubFrameRect.iTl, *aSubFrame, *aSubFrameMask );
                        
    // Extract new frame & mask
    CHuiModifiableBitmap* frame = 
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );
    frame->ExtractColor( *scratch );

    CHuiModifiableBitmap* frameMask = 
        CHuiModifiableBitmap::NewLC( frameSize, EGray256 );
    frameMask->ExtractAlpha( *scratch );
            
    // Create prev frame & mask
    CHuiModifiableBitmap* prevFrame = 
        iPrevFrame ?
        CHuiModifiableBitmap::NewLC( *iPrevFrame ) :
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );

    CHuiModifiableBitmap* prevFrameMask = 
        iPrevFrameMask ?
            CHuiModifiableBitmap::NewLC( *iPrevFrameMask ) :
            CHuiModifiableBitmap::NewLC( frameSize, EGray256 );

    prevFrame->Clear( BitmapClearColor() );
    prevFrameMask->Clear( MaskClearColor() );
    
    switch ( iNextSubFrameDisposalAction )
        {                    
        case ELeaveInPlace:
            prevFrame->BitBlt( iNextSubFrameRect, *frame );
            prevFrameMask->BitBlt( iNextSubFrameRect, *frameMask );
            break;
        
        case ERestoreToPrevious:
        case EUnspecified:        
        case ERestoreToBackgroundColour:
        default:
            break;
        }

    // Cleanup
    iPrevFrameMask = prevFrameMask->ReleaseBitmap();
    CleanupStack::PopAndDestroy( prevFrameMask );
            
    iPrevFrame = prevFrame->ReleaseBitmap();
    CleanupStack::PopAndDestroy( prevFrame );
            
    aNewFrameMask = frameMask->ReleaseBitmap();
    CleanupStack::PopAndDestroy( frameMask );
                
    aNewFrame = frame->ReleaseBitmap();              
    CleanupStack::PopAndDestroy( frame );
    CleanupStack::PopAndDestroy( scratch ); 
    }

void CHuiTextureAnimationState::ProduceNextFrameAndUpdatePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    if ( !aSubFrameMask || !iPrevFrameMask )
        {
        User::Leave( KErrNotSupported );
        }

    const TSize frameSize = OverallSize();
            
    // Make previous state and blit new subframe on top of that
    CHuiModifiableBitmap* scratch = 
        CHuiModifiableBitmap::NewLC( frameSize, EColor16MA );
    scratch->Combine( *iPrevFrame, *iPrevFrameMask );
            
    scratch->Context().BitBltMasked(
        iNextSubFrameRect.iTl,
        aSubFrame,
        TRect( aSubFrame->SizeInPixels() ),
        aSubFrameMask,
        ETrue );
            
    // Extract new frame & mask
    CHuiModifiableBitmap* frame = 
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );
    frame->ExtractColor( *scratch );
            
    CHuiModifiableBitmap* frameMask =
        CHuiModifiableBitmap::NewLC( frameSize, EGray256 );
    frameMask->ExtractAlpha( *scratch );
            
    // Update previous frame & mask
    CHuiModifiableBitmap* prevFrame = 
        CHuiModifiableBitmap::NewLC( *iPrevFrame );
    CHuiModifiableBitmap* prevFrameMask =
        CHuiModifiableBitmap::NewLC( *iPrevFrameMask );
        
    switch ( iNextSubFrameDisposalAction )
        {                    
        case EUnspecified:
        case ERestoreToBackgroundColour:
            prevFrameMask->Clear( iNextSubFrameRect, MaskClearColor() );
            prevFrame->Clear( iNextSubFrameRect, BitmapClearColor() );
            break;
                    
        case ELeaveInPlace:
            prevFrame->BitBlt( iNextSubFrameRect, *frame );
            prevFrameMask->BitBlt( iNextSubFrameRect, *frameMask );
            break;
            
        case ERestoreToPrevious:
        default:
            break;
            }
        
    CleanupStack::PopAndDestroy( prevFrameMask );
    CleanupStack::PopAndDestroy( prevFrame );

    aNewFrame = frame->ReleaseBitmap();
    aNewFrameMask = frameMask->ReleaseBitmap();
            
    CleanupStack::PopAndDestroy( frameMask );
    CleanupStack::PopAndDestroy( frame );
                        
    CleanupStack::PopAndDestroy( scratch );
    }

void CHuiTextureAnimationState::QuickProduceFirstFrameAndGeneratePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    const TSize frameSize = OverallSize();
    const TBool hasAlpha = ( aSubFrameMask != NULL );
        
    const TBool exactMatch = 
        ( TRect( frameSize ) == iNextSubFrameRect ) &&
        ( aSubFrame->SizeInPixels() == frameSize );
                            
    // Produce new frame & mask
    CHuiModifiableBitmap* frame = 
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );
    
    CHuiModifiableBitmap* frameMask = NULL;
    if ( hasAlpha )
        {           
        frameMask = CHuiModifiableBitmap::NewLC( frameSize, EGray256 );
        
        frame->Clear( BitmapClearColor() );
        if ( !exactMatch )
            {
            frameMask->Clear( MaskClearColor() );
            }
        
        frame->Context().BitBltMasked( 
            iNextSubFrameRect.iTl, 
            aSubFrame, 
            TRect( aSubFrame->SizeInPixels() ), 
            aSubFrameMask, 
            ETrue );
        frameMask->Context().BitBlt( iNextSubFrameRect.iTl, aSubFrameMask );
        }
    else
        {
        if ( !exactMatch )
            {
            frame->Clear( iNextSubFrameBgColor );
            }
        frame->Context().BitBlt( iNextSubFrameRect.iTl, aSubFrame );
        }
                
    // Create prev frame & mask
    CHuiModifiableBitmap* prevFrame = 
        iPrevFrame ?
        CHuiModifiableBitmap::NewLC( *iPrevFrame ) :
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );

    CHuiModifiableBitmap* prevFrameMask = NULL;
    if ( hasAlpha )
        {
        prevFrameMask = 
            iPrevFrameMask ?
                CHuiModifiableBitmap::NewLC( *iPrevFrameMask ) :
                CHuiModifiableBitmap::NewLC( frameSize, EGray256 );
        }

    const TRgb clearColor = 
        hasAlpha ? 
            BitmapClearColor() : 
            iNextSubFrameBgColor;
    prevFrame->Clear( clearColor );        
    if ( prevFrameMask )
        {
        prevFrameMask->Clear( MaskClearColor() );
        }
    
    switch ( iNextSubFrameDisposalAction )
        {                    
        case ELeaveInPlace:
            prevFrame->BitBlt( iNextSubFrameRect, *frame );
            if ( prevFrameMask )
                {
                prevFrameMask->BitBlt( iNextSubFrameRect, *frameMask );
                }
            break;
        
        case ERestoreToPrevious:
        case EUnspecified:        
        case ERestoreToBackgroundColour:
        default:
            break;
        }

    // Cleanup
    if ( prevFrameMask )
        {
        iPrevFrameMask = prevFrameMask->ReleaseBitmap();
        CleanupStack::PopAndDestroy( prevFrameMask );
        }       
            
    iPrevFrame = prevFrame->ReleaseBitmap();
    CleanupStack::PopAndDestroy( prevFrame );
            
    if ( frameMask )
        {
        aNewFrameMask = frameMask->ReleaseBitmap();
        CleanupStack::PopAndDestroy( frameMask );
        }
                
    aNewFrame = frame->ReleaseBitmap();                    
    CleanupStack::PopAndDestroy( frame );     
    }

void CHuiTextureAnimationState::QuickProduceNextFrameAndUpdatePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask )
    {
    const TSize frameSize = OverallSize();
    const TBool hasAlpha = aSubFrameMask != NULL;

    // Produce frame & frame mask
    CHuiModifiableBitmap* frame = 
        CHuiModifiableBitmap::NewLC( frameSize, aSubFrame->DisplayMode() );
    frame->Context().BitBlt( TPoint(), iPrevFrame );
    
    CHuiModifiableBitmap* frameMask = NULL;
    if ( iPrevFrameMask )
        {
        frameMask =
            CHuiModifiableBitmap::NewLC( frameSize, EGray256 );        
        frameMask->Context().BitBlt( TPoint(), iPrevFrameMask );
        }
    
    if ( hasAlpha )
        {
        frame->Context().SetBrushStyle( CGraphicsContext::ENullBrush );
        frame->Context().BitBltMasked( 
            iNextSubFrameRect.iTl, 
            aSubFrame, 
            TRect( aSubFrame->SizeInPixels() ), 
            aSubFrameMask, 
            ETrue );

        if ( iPrevFrameMask )
            {                
            // Merge masks:
            // prev: 255 sub: 255 frame: 255
            // prev: 255 sub: 0   frame: 255
            // prev: 0   sub: 255 frame: 255
            // prev: 0   sub: 0   frame: 0
            // => OR style behaviour works
            frameMask->CombineMasks( iNextSubFrameRect, aSubFrameMask );
            }
        }
    else
        {
        frame->Context().BitBlt( iNextSubFrameRect.iTl, aSubFrame );
        
        if ( iPrevFrameMask )
            {
            frameMask->Clear( iNextSubFrameRect, MaskOpaqueColor() );
            }
        }
    
    // Update previous frame & mask
    CHuiModifiableBitmap* prevFrame = 
        CHuiModifiableBitmap::NewLC( *iPrevFrame );
    CHuiModifiableBitmap* prevFrameMask = NULL;
    if ( iPrevFrameMask )
        {
        prevFrameMask =
            CHuiModifiableBitmap::NewLC( *iPrevFrameMask );
        }
        
    switch ( iNextSubFrameDisposalAction )
        {   
        case EUnspecified:                         
        case ERestoreToBackgroundColour:
            if ( prevFrameMask )
                {
                prevFrameMask->Clear( iNextSubFrameRect, MaskClearColor() );
                prevFrame->Clear( iNextSubFrameRect, BitmapClearColor() );
                }
            else
                {
                prevFrame->Clear( iNextSubFrameRect, iNextSubFrameBgColor );
                }    
            break;
                    
        case ELeaveInPlace:
            prevFrame->BitBlt( iNextSubFrameRect, *frame );
            if ( prevFrameMask )
                {
                prevFrameMask->BitBlt( iNextSubFrameRect, *frameMask );
                }
            break;
            
        case ERestoreToPrevious:
        default:
            break;
        }

    if ( prevFrameMask )
        {
        CleanupStack::PopAndDestroy( prevFrameMask );
        }
    CleanupStack::PopAndDestroy( prevFrame );

    aNewFrame = frame->ReleaseBitmap();
    if ( frameMask )
        {
        aNewFrameMask = frameMask->ReleaseBitmap();
        CleanupStack::PopAndDestroy( frameMask );
        }
    CleanupStack::PopAndDestroy( frame );        
    }

inline TRgb CHuiTextureAnimationState::BitmapClearColor()
    {
    return TRgb(0,0,0); // black
    }
    
inline TRgb CHuiTextureAnimationState::MaskClearColor()
    {
    return TRgb::Gray256( 0 ); // transparent for EGray256 masks
    }

inline TRgb CHuiTextureAnimationState::MaskOpaqueColor()
    {
    return TRgb::Gray256( 255 ); // opaque for EGray256 masks
    }

void CHuiTextureAnimationState::CheckSubFrameL( 
        CFbsBitmap* aSubFrame, 
        CFbsBitmap* aSubFrameMask )
    {
    if ( !aSubFrame )
        {
        User::Leave( KErrNotSupported );
        }    
            
    // aSubFrame display mode should be either 64K or 16MU,
    // but let's check it.
    const TDisplayMode subFrameMode = aSubFrame->DisplayMode();
    if ( ( subFrameMode != EColor64K && subFrameMode != EColor16MU ) ||
         aSubFrame->IsCompressedInRAM() )
        {
        User::Leave( KErrNotSupported );
        }
        
    // aSubFrameMask should be EGray256
    if ( aSubFrameMask )
        {
        if ( ( aSubFrameMask->DisplayMode() != EGray256 ) ||
             aSubFrame->IsCompressedInRAM() )
            {
            User::Leave( KErrNotSupported );
            }
        }
    }



//
// CHuiModifiableBitmap implementation
//


CHuiModifiableBitmap* CHuiModifiableBitmap::NewLC( 
        const TSize& aSize, TDisplayMode aMode )
    {
    CHuiModifiableBitmap* self = new (ELeave) CHuiModifiableBitmap;
    CleanupStack::PushL( self );
    self->ConstructL( aSize, aMode );
    return self;
    }
    
CHuiModifiableBitmap* CHuiModifiableBitmap::NewLC( 
        CFbsBitmap& aBitmap )
    {
    CHuiModifiableBitmap* self = new (ELeave) CHuiModifiableBitmap;
    CleanupStack::PushL( self );
    self->ConstructL( aBitmap );
    return self;    
    }

CHuiModifiableBitmap::~CHuiModifiableBitmap()
    {
    delete iGc;
    delete iDevice;
    if ( iOwnBitmap )
        {
        delete iBitmap;
        }
    }

CFbsBitGc& CHuiModifiableBitmap::Context()
    {
    return *iGc;
    }
    
CFbsBitmap* CHuiModifiableBitmap::ReleaseBitmap()
    {
    delete iGc;
    iGc = NULL;
    delete iDevice;
    iDevice = NULL;
    CFbsBitmap* bitmap = iBitmap;
    iBitmap = NULL;
    return bitmap;
    }

void CHuiModifiableBitmap::Clear( const TRgb& aColor )
    {
    iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iGc->SetBrushColor( aColor );
    iGc->Clear();
    iGc->SetBrushStyle( CGraphicsContext::ENullBrush );
    }

void CHuiModifiableBitmap::Clear( const TRect& aRect, const TRgb& aColor )
    {
    iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iGc->SetBrushColor( aColor );
    iGc->Clear( aRect );
    iGc->SetBrushStyle( CGraphicsContext::ENullBrush );    
    }

void CHuiModifiableBitmap::BitBlt( 
        const TRect& aRect, CHuiModifiableBitmap& aBitmap )
    {
    if ( iGc && aBitmap.iBitmap && iBitmap )
        {
        // Copy area from one bitmap to another. Same size expected.
        iGc->BitBlt( aRect.iTl, aBitmap.iBitmap, aRect );
        }
    }

void CHuiModifiableBitmap::Combine( 
        const CFbsBitmap& aSrc, const CFbsBitmap& aSrcMask )
    {
    if ( iBitmap )
        {
        Combine( TPoint(), aSrc, aSrcMask );
        }
    }

void CHuiModifiableBitmap::Combine( 
        const TPoint& aPos, const CFbsBitmap& aSrc, const CFbsBitmap& aSrcMask )
    {
    if ( iBitmap )
        {
        // Clear with transparent
        iGc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        iGc->SetBrushColor( TRgb( 0, 0, 0, 0 ) );
        iGc->Clear();
        iGc->SetBrushStyle( CGraphicsContext::ENullBrush );    
        iGc->SetDrawMode( CGraphicsContext::EDrawModePEN );
        
        // Blit masked
        iGc->BitBltMasked( 
            aPos, 
            &aSrc, 
            TRect( aSrc.SizeInPixels() ), 
            &aSrcMask, 
            ETrue );
        }
    }

void CHuiModifiableBitmap::CombineMasks( 
        const TRect& aRect, 
        CFbsBitmap* aMask )
    {    
    if ( iBitmap )
        {
        // this is EGray256, aMask is EGray256
        TRect targetRect( aRect );
        targetRect.Intersection( iBitmap->SizeInPixels() );
        targetRect.Intersection( TRect( aRect.iTl, aMask->SizeInPixels() ) );
        
        if ( targetRect.IsEmpty() )
            {
            // nothing to draw
            return;
            }
    
        // Starting target & source point and size to draw
        const TPoint targetPoint = targetRect.iTl;
        const TPoint sourcePoint = targetRect.iTl - aRect.iTl;
        const TSize size = targetRect.Size();

        // Go through all pixels using OR operation.
        // As aMask contains only 0 or 255 values, it is
        // sufficient to combine these masks.
        TBitmapUtil source(aMask);
        TBitmapUtil target(iBitmap);
    
        source.Begin( sourcePoint );
        target.Begin( targetPoint );
    
        for( TInt y = 0; y < size.iHeight; ++y )
            {
            source.SetPos( sourcePoint + TPoint(0,y) );
            target.SetPos( targetPoint + TPoint(0,y) );
            
            for( TInt x = 0; x < size.iWidth; ++x )
                {
                TUint32 pixel = source.GetPixel();
                if ( pixel )
                    {
                    target.SetPixel( target.GetPixel() | pixel );
                    }
                
                target.IncXPos();
                source.IncXPos();
                }
            }
            
        target.End();
        source.End();        
        }    
    }

void CHuiModifiableBitmap::ExtractAlpha( CHuiModifiableBitmap& aSource )
    {
    if ( iBitmap && aSource.iBitmap )
        {
        // this is EGray256, aSource is 16MA
        
        TBitmapUtil source(aSource.iBitmap);
        TBitmapUtil target(iBitmap);
    
        source.Begin( TPoint() );
        target.Begin( TPoint() );
    
        const TSize size( iBitmap->SizeInPixels() );
        for( TInt y = 0; y < size.iHeight; ++y )
            {
            source.SetPos( TPoint( 0, y) );
            target.SetPos( TPoint( 0, y) );
            
            for( TInt x = 0; x < size.iWidth; ++x )
                {
                target.SetPixel( source.GetPixel() >> 24 );
                
                target.IncXPos();
                source.IncXPos();
                }
            }
            
        target.End();
        source.End();        
        }
    }
   
void CHuiModifiableBitmap::ExtractColor( CHuiModifiableBitmap& aSource )
    {
    // this is color, aSource is 16MA
    
    if ( iBitmap && aSource.iBitmap )
        {
        TDisplayMode mode = iBitmap->DisplayMode();
        if ( mode == EColor64K )
            {
            DoExtractColor<Convert16MA_64K>( iBitmap, aSource.iBitmap );
            }
        else if ( mode == EColor16MU )
            {
            DoExtractColor<Convert16MA_16MU>( iBitmap, aSource.iBitmap );
            }
        }
    }
    
void CHuiModifiableBitmap::ConstructL( const TSize& aSize, TDisplayMode aMode )
    {
    iOwnBitmap = ETrue;
    iBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError( iBitmap->Create( aSize, aMode ) );
    
    iDevice = CFbsBitmapDevice::NewL( iBitmap );
    iDevice->CreateContext( iGc );
    User::LeaveIfNull( iGc );        
    }

void CHuiModifiableBitmap::ConstructL( CFbsBitmap& aBitmap )
    {
    iOwnBitmap = EFalse;
    iBitmap = &aBitmap;
    
    iDevice = CFbsBitmapDevice::NewL( iBitmap );
    iDevice->CreateContext( iGc );
    User::LeaveIfNull( iGc );        
    }

template<typename Converter>
void CHuiModifiableBitmap::DoExtractColor( 
        CFbsBitmap* aTarget, CFbsBitmap* aSource )
    {
    TBitmapUtil source(aSource);
    TBitmapUtil target(aTarget);
    
    source.Begin( TPoint() );
    target.Begin( TPoint() );
    
    const TSize size( aTarget->SizeInPixels() );
    for(TInt y = 0; y < size.iHeight; ++y)
        {
        source.SetPos(TPoint(0, y));
        target.SetPos(TPoint(0, y));
            
        for(TInt x = 0; x < size.iWidth; ++x)
            {
            target.SetPixel( Converter::Convert( source.GetPixel() ) );
                
            target.IncXPos();
            source.IncXPos();
            }
        }
            
    target.End();
    source.End();
    }

inline TUint32 CHuiModifiableBitmap::Convert16MA_16MU::Convert( TUint32 aPixel )
    {
    return aPixel | 0xFF000000;
    }
    
inline TUint32 CHuiModifiableBitmap::Convert16MA_64K::Convert( TUint32 aPixel )
    {
	const TUint32 b = (aPixel&0x000000ff)>>3;
	const TUint32 g = (aPixel&0x0000fc00)>>5;
	const TUint32 r = (aPixel&0x00f80000)>>8;
	return (r|g|b);
    }

