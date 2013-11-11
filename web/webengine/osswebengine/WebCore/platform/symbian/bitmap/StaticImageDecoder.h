/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __STATICIMAGEDECODER_H_
#define __STATICIMAGEDECODER_H_

#include <e32base.h>
#include <e32std.h>
#include "AnimationDecoder.h"

class CFbsBitmap;
class CMaskedBitmap;
class TFrameInfo;
class CBufferedImageDecoder;

struct CRawData : public CBase
    {
    static CRawData* NewL( const TDesC8& aData, TDesC* aMime, 
           CMaskedBitmap* aTarget, MAnimationDecoderObserver* aObserv );
    virtual ~CRawData();

    CRawData() : iData(0), iDataPtr(0,0), iMime(0), iTarget(0), iObserver(0) {}
    void ConstructL(const TDesC8& aData, TDesC* aMime, 
        CMaskedBitmap* aTarget, MAnimationDecoderObserver* aObserv );
    
    TAny*           iData;
    TPtr8           iDataPtr;
    HBufC8*         iMime;
    CMaskedBitmap*  iTarget;
    MAnimationDecoderObserver* iObserver;
    };


class CStaticImageDecoder : public CActive
    {
    public:
        enum TDecodingState
            {
            EIdle,
            EStartLoading,
            EOneFrameReady
            };
                            
        // constructor
        static CStaticImageDecoder* NewL();
        virtual ~CStaticImageDecoder();
        
        
        // decode the image asynchorously
        TBool DecodeL( CRawData* aData  );
        void StopObserving( MAnimationDecoderObserver* aObserv );
        
        // from CActive
        void RunL();
        void DoCancel();
        
    private:
        CStaticImageDecoder();
        TBool LoadNextImage();
        void  LoadOneFrame();
        void StartLoading();
        TDisplayMode GetBestDisplayMode(TDisplayMode) const;
        TBool CheckBitmapMemoryConsumption( const TFrameInfo& aFrameInfo ) const;
        
        void ConstructL();
        void Reset();
      
        CBufferedImageDecoder*  iDecoder;   // owned
        TDecodingState          iState;
        RPointerArray<CRawData> iQueue;
        CRawData*               iContext;   // current decoding data
    };

#endif //!__STATICIMAGEDECODER_H_