/*
* ==============================================================================
*    Copyright (c) 2006, Nokia Corporation
*    All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions
*   are met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in
*        the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the Nokia Corporation nor the names of its
*        contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*   DAMAGE.
*
* ==============================================================================
*/

/*
 *  This class is wrapped by a proxy CAnimationDecoder
 *  
 *  We needed to wrap animation decoding in a proxy because sometimes the cache gets cleared when decoding
 *  is in progress; when that happens the animation gets deleted while it's in the middle (which causes all sorts
 *  of crashes and memory stomping).  Now, the cache can delete the proxy while the animation is decoding; the proxy
 *  will pass on the delete request to the decoder which will cleanup (delete itself) when it's safe to do so.
 *  
 */

#ifndef ANIMATIONDECODERWRAPPED_H_
#define ANIMATIONDECODERWRAPPED_H_

// INCLUDES

#include <e32base.h>
#include <imageconversion.h> // TFrameInfo

// FORWARD DECLARATIONS
class CMaskedBitmap;
class CBufferedImageDecoder;
class CSynDecodeThread;
namespace WebCore {
    class ImageObserver;
}
// CONSTANTS

_LIT(KMimeJPEG, "image/jpeg");
_LIT(KMimeJPG, "image/jpg");
_LIT(KMimeGIF, "image/gif");
_LIT(KMimePNG, "image/png");
_LIT(KMimeTIFF, "image/tiff");
_LIT(KMimeBMP, "image/bmp");
_LIT(KMimeWBMP, "image/vnd.wap.wbmp");
_LIT(KMimeWMF, "application/x-msmetafile");
_LIT(KMimeMBM, "image/x-epoc-mbm");
_LIT(KMimeOTA, "image/vnd.nokia.ota-bitmap");
_LIT(KMimeICO, "image/x-icon");
_LIT(KMimeDRM, "application/vnd.oma.drm.content");

//const TDisplayMode KMaxDepth = EColor64K;

// FIXME: we should move this back to EColor64K after Symbian fix their Gif image decoder bug.
const TDisplayMode KMaxDepth = EColor16M;

// CLASS DECLARATION
/**
*  CAnimationDecoderWrapped
*
*  Implementation of file based image class.
*  @lib IHL.lib
*  @since 3.0
*/

class CAnimationDecoderWrapped  : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAnimationDecoderWrapped* NewL(WebCore::ImageObserver* aObs);

        /*
        * Virtual destructor.
        */
        virtual ~CAnimationDecoderWrapped();

  public:

        /*
        * From MIHLFileImage, see base class header.
        */
        void OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete );
        void OpenAndDecodeSyncL( const TDesC8& aData );

        /*
        * From MIHLFileImage, see base class header.
        */
        void AddDataL( const TDesC8& aData, TBool aIsComplete );

        /*
        * From MIHLFileImage, see base class header.
        */
        TSize Size() const { return iFrameInfo.iOverallSizeInPixels; }

        /*
        * From MIHLFileImage, see base class header.
        */
        TDisplayMode DisplayMode() const { return KMaxDepth; }

        /*
        * From MIHLFileImage, see base class header.
        */
        TDisplayMode MaskDisplayMode() const;

        /*
        * From MIHLFileImage, see base class header.
        */
        TRgb BackgroundColor() const { return iFrameInfo.iBackgroundColor; }

        /*
        * From MIHLFileImage, see base class header.
        */
        TBool IsAnimation() const { return iAnimation; }

        /*
        * From MIHLFileImage, see base class header.
        */
        TInt AnimationFrameCount() const { return iAnimationFrameCount; }

        /*
        * From MIHLFileImage, see base class header.
        */
        TTimeIntervalMicroSeconds32 AnimationFrameDelay( TInt aAnimationFrameIndex ) const;

        /*
        * From MIHLFileImage, see base class header.
        */
        TInt LoadFrame( TInt aFrameIndex );

        /*
        * From MIHLFileImage, see base class header.
        */
        TBool IsBusy() const { return ( iImageState != EInactive ); }


        /*
        * From MIHLFileImage, see base class header.
        */
        void CancelLoad() { Cancel(); }

        TBool IsOpen() const { return iDecoder!=0; }

        //funtions related to Loop count of animation image.
        TInt16 getLoopCount(){return iCurLoopCount;};
        void resetLoopCount(){iCurLoopCount = iLoopCount;};
        void decreamentLoopCount(){ iCurLoopCount--;};
        CMaskedBitmap* Destination();
        TBool isSizeAvailable() const { return iSizeAvailable; }
        TBool decodeInProgress() const { return iDecodeInProgress; }

        /**
         * Mark the object invalid (to be deleted); used when it gets cleared from the cache
         */
        void Invalidate();
        
  private: // From base class CActive

        /*
        * From CActive, see base class header.
        */
        void DoCancel();

        /*
        * From CActive, see base class header.
        */
        void RunL();

        /*
        * From CActive, see base class header.
        */
        TInt RunError( TInt aError );

  private: // Private methods

        void StartDecodingL();
        void StartLoadL();
        void StartLoadNormalBitmap( TInt aFrameIndex );
        void StartLoadAnimationBitmapL( TInt aFrameIndex );
        void CompleteLoadL();
        void BuildAnimationFrameL();
        void ErrorCleanup();
        void SelfComplete( TInt aError = KErrNone );
        HBufC8* DecodeDRMImageContentL(const TDesC8& aData);

  private: // Private constructors

        CAnimationDecoderWrapped(WebCore::ImageObserver* aObs);
        void ConstructL( );

  private: // Private data types

        enum TImageState
            {
            EInactive,
            EStartLoad,
            ECompleteLoad,
            };

  private: // Data

        // Image status & state
        TRequestStatus* iImageStatus;
        TImageState iImageState;

        // Own: Image decoder
        CBufferedImageDecoder* iDecoder; // owned

        TFrameInfo iFrameInfo;
        TBool iAnimation;
        TInt iAnimationFrameCount;

        // Ref: Destination bitmap
        CMaskedBitmap* iDestination;
        TInt iFrameIndex;

        // Own: Temporary animation bitmap
        CMaskedBitmap* iAnimationBitmap; // owned
        TInt iAnimationFrameIndex;

        WebCore::ImageObserver* iObserver; // not owned

        TBool iRawDataComplete;

        //Loop count for animation image. Default to -1 -- loop forever
        TInt16 iLoopCount;
        TInt16 iCurLoopCount;

        TBool iSizeAvailable;
        TInt iSyncBitmapHandle;
        TBool iSyncMaskHandle;
        TBool iDecodeInProgress;
        static CSynDecodeThread* iSyncDecodeThread; // sync decoder thread
        HBufC8* iDrmContent;
        
        // Is invalid because the cache got cleared in the middle of decoding
        TBool iIsInvalid;
        TBool iCanBeDeleted;
    };


#endif /* ANIMATIONDECODERWRAPPED_H_ */

// End of File
