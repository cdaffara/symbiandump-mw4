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

#ifndef ANIMATIONDECODER_H
#define ANIMATIONDECODER_H

// INCLUDES
#include "AnimationDecoderWrapped.h"

// CLASS DECLARATION
/**
*  CAnimationDecoder
*
*  Proxy for implementation of file based image class.
*  @lib IHL.lib
*  @since 3.0
*  
*  This class is a proxy for CAnimationDecoderWrapped
*  
*  We needed to wrap animation decoding in a proxy because sometimes the cache gets cleared when decoding
*  is in progress; when that happens the animation gets deleted while it's in the middle (which causes all sorts
*  of crashes and memory stomping).  Now, the cache can delete the proxy while the animation is decoding; the proxy
*  will pass on the delete request to the decoder which will cleanup (delete itself) when it's safe to do so.
*/

class CAnimationDecoder
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAnimationDecoder* NewL(WebCore::ImageObserver* aObs);

        /*
        * Virtual destructor.
        */
        virtual ~CAnimationDecoder();

  public:

        /*
        * From MIHLFileImage, see base class header.
        */
        void OpenL( const TDesC8& aData, TDesC* aMIMEType, TBool aIsComplete );

        /*
        * From MIHLFileImage, see base class header.
        */
        TSize Size() const;

        /*
        * From MIHLFileImage, see base class header.
        */
        TInt AnimationFrameCount() const;
        /*
        * From MIHLFileImage, see base class header.
        */
        TInt LoadFrame( TInt aFrameIndex );

        //funtions related to Loop count of animation image.
        TInt16 getLoopCount();
        CMaskedBitmap* Destination();
        TBool isSizeAvailable() const;
        TBool decodeInProgress() const;

  private: // Private constructors

        CAnimationDecoder(CAnimationDecoderWrapped *aDecoder);
        void ConstructL( );

  private: // Data
      CAnimationDecoderWrapped *iWrapped;
    };

#endif   // CAnimationDecoder_H

// End of File
