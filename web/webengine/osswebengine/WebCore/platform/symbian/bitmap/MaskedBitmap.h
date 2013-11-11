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


#ifndef MASKEDBITMAP_H
#define MASKEDBITMAP_H

// INCLUDES
#include <e32base.h>
#include <gdi.h>

// FORWARD DECLARATIONS

class CFbsBitGc;

class BitmapUtil
    {
    public:
        static TInt CopyBitmap( const CFbsBitmap& aSource, CFbsBitmap& aDestination );

        static TInt CopyBitmapData( const CFbsBitmap& aSource, CFbsBitmap& aDestination,
                   const TSize& aSize, const TDisplayMode& aDisplayMode );
    };
    
// CLASS DECLARATION
/**
*  Class encapsulating bitmap and the mask (originally CIHLBitmap)
*  @since 3.0
*/
class CMaskedBitmap : public CBase
  {
    public:  // Constructors and destructor

        /**
         * Initialize with bitmaps
         */
        CMaskedBitmap(CFbsBitmap* aBitmap, CFbsBitmap* aMask);

        /**
        * Two-phased constructors
        */
        static CMaskedBitmap* NewL();

        /**
        * Virtual destructor.
        */
        virtual ~CMaskedBitmap();

  public: // From base class MIHLBitmap

        /**
        * From MIHLBitmap, see base class header.
        */
    TInt Create( const TSize& aSize, TDisplayMode aDisplayMode );

        /**
        * From MIHLBitmap, see base class header.
        */
    TInt Create( const TSize& aSize, TDisplayMode aBitmapDisplayMode,
                     TDisplayMode aMaskDisplayMode );

        /**
        * From MIHLBitmap, see base class header.
        */
        TInt CreateCopyingDisplayMode( const TSize& aSize, const CMaskedBitmap& aCopyFrom );

        /**
        * From MIHLBitmap, see base class header.
        */
    TInt Copy( const CFbsBitmap& aBitmap, TBool aDuplicate );

        /**
        * From MIHLBitmap, see base class header.
        */
    TInt Copy( const CFbsBitmap& aBitmap, const CFbsBitmap& aMask, TBool aDuplicate );

        /**
        * From MIHLBitmap, see base class header.
        */
        TInt Copy( const CMaskedBitmap& aBitmap, TBool aDuplicate );

        /**
        * From MIHLBitmap, see base class header.
        */
    void Reset();

        /**
        * From MIHLBitmap, see base class header.
        */
    TBool IsCreated() const;

        /**
        * From MIHLBitmap, see base class header.
        */
    const CFbsBitmap& Bitmap() const;

        /**
        * From MIHLBitmap, see base class header.
        */
    TBool HasMask() const;

        /**
        * From MIHLBitmap, see base class header.
        */
    const CFbsBitmap& Mask() const;

        /**
        * From MIHLBitmap, see base class header.
        */
        void BitBlt( CFbsBitGc& aContext, const TPoint& aPoint ) const;

        /**
        * From MIHLBitmap, see base class header.
        */
        void BitBlt( CFbsBitGc& aContext, const TPoint& aPoint, const TRect& aSourceRect ) const;

        void DrawBitmap( CFbsBitGc& aContext, const TRect& aTarget) const;

        void DrawBitmap( CFbsBitGc& aContext, const TRect& aTarget, const TRect& aSource) const;

        TSize SizeInPixels() const;

        void Resize(TSize aSize);
        
        void CompressInBackground( );

        CMaskedBitmap* ScaleImageToSize( TSize newSize );

  public: // Internal interface

        /**
        * Non-const bitmap reference.
        * @since 3.0
    * @return Non-const bitmap reference.
    */
    CFbsBitmap& BitmapModifyable();

        /**
        * Non-const bitmap reference.
        * @since 3.0
    * @return Non-const bitmap reference.
    */
    CFbsBitmap& MaskModifyable();

        /**
        * Set image/frame index to file
    * which was source of this bitmap.
        * @since 3.0
    * @param aFrameIndex Image/frame index to Filename() file.
    */
    void SetFrameIndex( TInt aFrameIndex );

        /**
        * Set animation delay of this bitmap.
        * @since 3.0
    * @param aDelay Animation delay.
    */
    void SetFrameDelay( TTimeIntervalMicroSeconds32 aFrameDelay );

        /**
        * Return image/frame index to file
    * which was source of this bitmap.
        * @since 3.0
    * @return Image/frame index to Filename() file.
    */
    TInt FrameIndex() const;

        /**
        * Return animation delay of this bitmap.
        * @since 3.0
    * @return Animation delay.
    */
    TTimeIntervalMicroSeconds32 FrameDelay() const;

        /**
        * fast tiling: tile this bitmap to another bitmap, usually the offscreen buffer
        * @since 3.0
    * @return.
    */
    void TileInBitmapRect( CFbsBitmap* trgBmp, const TRect& bmpRect, const TPoint& srcPt );

    void TileInBitmapRect( CFbsBitGc& trgGc, const TRect& bmpRect, const TPoint& srcPt );

       /**
        * True if the image has no drawable pixels
        */
       TBool IsFullyTransparent();

       /**
        * Invert the mask
        */
       void SetInvertMask(TBool aBool) { iInvertMask = aBool; }

       /**
       * Invert the mask
       */
       TBool InvertMask() const { return iInvertMask; }

  private: // Private constructors

        CMaskedBitmap();
    void ConstructL();

    private: // Data

    // Own: Bitmap pointers
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;

    // Frame index according to iFilename file
    TInt iFrameIndex;

    // Frame animation delay
    TTimeIntervalMicroSeconds32 iFrameDelay;

        TBool iInvertMask;

    CMaskedBitmap* iScaledBitmap;

  };

#endif // MASKEDBITMAP_H

// End of File
