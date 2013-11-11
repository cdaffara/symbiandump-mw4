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
* Description:   Defines CHuiTextureAnimationState, which is used by
*                CHuiTextureManager when loading animated textures.
*
*/


#ifndef C_HUITEXTUREANIMATIONSTATE_H
#define C_HUITEXTUREANIMATIONSTATE_H

#include <e32base.h>
#include <gdi.h>
#include <imageconversion.h>

class CFbsBitmap;

/**
 * This holds animation state. One instance is required per animation.
 * To produce next frame, you have to have animation state containing
 * previous frame information. Thus, if animation state is needed, 
 * you should load frames in sequence (0, 1, ..., N).
 */
NONSHARABLE_CLASS( CHuiTextureAnimationState ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param aTextureGroupId texture group id.
     * @param aImageFile image file.
     * @param aFrameCount amount of frames in file.
     */
    static CHuiTextureAnimationState* NewL( 
        TInt aTextureGroupId, const TDesC& aImageFile, TInt aFrameCount );
        
    /**
     * Destructor.
     */
    ~CHuiTextureAnimationState();
   
    /**
     * Returns owner texture group id.
     * @return texture group id.
     */
    TInt OwnerTextureGroupId() const;
    
    /** 
     * Checks if frame defined by aImageFile and aFrameNumber can be
     * composed with this state instance.
     * @param aImageFile image file name.
     * @param aFrameNumber frame number.
     * @param aFrameCount amount of frames in the file.
     */
    TBool CheckIfCanProduce( 
        const TDesC& aImageFile, 
        TInt aFrameNumber, 
        TInt aFrameCount ) const;

    /**
     * Gets next frame number.
     * @return next frame number.
     */
    TInt GetNextFrameNumber() const;
    
    /**
     * Offers next frame information. This method will store
     * information in order to be able to produce next frame
     * in @c ProduceNextFrameL.
     * @param aFrameInfo frame info.
     */
    void OfferNextFrameInfo( const TFrameInfo& aFrameInfo );
    
    /**
     * Returns overall size.
     */
    TSize OverallSize() const;

    /**
     * Produces next frame.
     * @param aNewFrame new frame.
     * @param aNewFrameMask new frame mask.
     * @param aSubFrame sub frame.
     * @param aSubFrameMask sub frame mask.
     */
    void ProduceNextFrameL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );
        
    /** 
     * Proceed without generating next frame.
     * @param aSubFrame sub frame.
     * @param aSubFrameMask sub frame mask.
     */
    void ProceedWithoutNextFrameL(
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );
        
private:
    CHuiTextureAnimationState();
    void ConstructL( 
        TInt aTextureGroupId, const TDesC& aImageFile, 
        TInt aFrameCount );
   
    /**
     * Produces first frame and generates previous.
     */
    void ProduceFirstFrameAndGeneratePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );

    /**
     * Produces next frame and updates previous.
     */
    void ProduceNextFrameAndUpdatePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );

    /**
     * Produces first frame and generates previous. Quick
     * version assumes that mask contains only values 0 and 255.
     */
    void QuickProduceFirstFrameAndGeneratePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );

    /**
     * Produces next frame and updates previous. Quick
     * version assumes that mask contains only values 0 and 255.
     */
    void QuickProduceNextFrameAndUpdatePreviousL(
        CFbsBitmap*& aNewFrame,
        CFbsBitmap*& aNewFrameMask,
        CFbsBitmap* aSubFrame,
        CFbsBitmap* aSubFrameMask );

    inline static TRgb BitmapClearColor();
    inline static TRgb MaskClearColor();
    inline static TRgb MaskOpaqueColor();
    
    /**
     * Checks sub frame and its mask for validity.
     * Leaves if invalid.
     */
    static void CheckSubFrameL( 
        CFbsBitmap* aSubFrame, 
        CFbsBitmap* aSubFrameMask );
    
private:
    //
    // Information about current animation
    //
    
    /**
     * Group id.
     */
    TInt iTextureGroupId;
    
    /**
     * File name.
     * Own.
     */
    HBufC* iFile;
    
    /**
     * Frame number. -1 if no frame yet produced.
     */
    TInt iFrame;

    /**
     * Frame count. Should be greater than one.
     */
    TInt iFrameCount;
        
    /**
     * Size of frames.
     */
    TSize iSize;

    //
    // Information about next frame
    //
    
    /**
     * Sub frame rectangle.
     * Updated in OfferNextFrameInfo.
     */
    TRect iNextSubFrameRect;
    
    /**
     * Background color.
     * Updated in OfferNextFrameInfo.
     */
    TRgb iNextSubFrameBgColor;

    /**
     * Boolean value indicating if complex blending is needed,
     * i.e. subframe has alpha channel (instead of just on/off transparency).
     * Updated in OfferNextFrameInfo.
     */
    TBool iNextSubFrameHasAlpha;

    /**
     * Enumeration for disposal actions
     */
    enum TDisposalAction
        {
        EUnspecified,
        ERestoreToPrevious,
        ERestoreToBackgroundColour,
        ELeaveInPlace
        };
        
    /**
     * Disposal action for next frame.
     * Updated in OfferNextFrameInfo.
     */
    TDisposalAction iNextSubFrameDisposalAction;
                
    //
    // Previous frame information
    //
    
    /**
     * Previous frame.
     * Own.
     */
    CFbsBitmap* iPrevFrame;
    
    /**
     * Previous frame mask.
     * Own.
     */
    CFbsBitmap* iPrevFrameMask;    
    };

#endif // C_HUITEXTUREANIMATIONSTATE_H

