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


#include "StaticImageDecoder.h"
#include "MaskedBitmap.h"
#include <imageconversion.h>
#include <eikenv.h>
#include <fbs.h>
#include <oom.h>

// CONSTANTS
// block all images that would take free ram below this amount when decoded
const TInt KFreeRamBitmapBlockLimit = 1536 * 1024;
// do the following additional checks if image would take free ram below this amount
const TInt KFreeRamBitmapCheckLimit = 7 * 1024 * 1024;
// if bitmap size is times this much bigger than data size then it is a high expansion bitmap
const TInt KHighExpansionBitmapFactor = 16;
// block high expansion bitmap above this size (they are usually backgrounds and so less important)
const TInt KLargeHighExpansionBitmapSize = 800*600*2;
// block all images that would take more than this percentage of available free ram
const TInt KMaxBitmapRamPercent = 25;

//=============================================================================
// CRawData
//=============================================================================
CRawData* CRawData::NewL( const TDesC8& aData, TDesC* aMime, CMaskedBitmap* aTarget, MAnimationDecoderObserver* aObserv )
    {
    CRawData* self = new (ELeave) CRawData();
    CleanupStack::PushL( self );
    self->ConstructL( aData, aMime, aTarget, aObserv );
    CleanupStack::Pop();
    return self;
    }

void CRawData::ConstructL( const TDesC8& aData, TDesC* aMIMEType, CMaskedBitmap* aTarget, MAnimationDecoderObserver* aObserv )
    {
    // mime type
    if (aMIMEType)
        {
        // it is safer to ignore the server supplied mime type and just recognize
        // the image type from the data headers. this does not work for all formats though
        if ( *aMIMEType==KMimeWBMP || *aMIMEType==KMimeOTA || *aMIMEType==KMimeWMF)
            {
            // convert to 8 bit
            iMime = HBufC8::NewL(aMIMEType->Length());
            iMime->des().Copy(*aMIMEType);
            }
        }

    // data
    const TUint8* src = aData.Ptr();
    iData = MemoryManager::Alloc( aData.Length() );
    Mem::Copy( iData, src, aData.Length() );
    iDataPtr.Set((TUint8*)iData, aData.Length(), aData.Length() );
    iObserver = aObserv;
    iTarget = aTarget;
    }

CRawData::~CRawData()
    {
    MemoryManager::Free( iData );
    delete iMime;
    }

//=============================================================================
// CStaticImageDecoder
//=============================================================================
CStaticImageDecoder::CStaticImageDecoder() : CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add( this );
    }

CStaticImageDecoder* CStaticImageDecoder::NewL()
    {
    CStaticImageDecoder* self = new (ELeave) CStaticImageDecoder();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CStaticImageDecoder::~CStaticImageDecoder()
    {

    iQueue.Reset();
    delete iDecoder;
    iContext = 0;

    if (IsActive())
        {
        Cancel();
        }
    }

void CStaticImageDecoder::ConstructL()
    {
    iDecoder = CBufferedImageDecoder::NewL(CEikonEnv::Static()->FsSession());
    }

TBool CStaticImageDecoder::LoadNextImage()
    {
  if( iQueue.Count() == 0 ) return EFalse;

    // load the data, FIFO
    CRawData* data = iQueue[0];

  TRAP_IGNORE(
        if( data->iMime )
            iDecoder->OpenL( data->iDataPtr, *(data->iMime), CImageDecoder::EOptionNone );
        else
            iDecoder->OpenL( data->iDataPtr, CImageDecoder::EOptionNone );
    )

    // set the context
  if(iDecoder->ValidDecoder()  && iDecoder->IsImageHeaderProcessingComplete())
    {
    if( iDecoder->FrameCount() == 1 )
      {
            iContext = data;
            TFrameInfo frmInfo = iDecoder->FrameInfo( 0 );

            // check memory before creating bitmaps
            if( CheckBitmapMemoryConsumption( frmInfo ) )
                {
                if (frmInfo.iFlags & TFrameInfo::ETransparencyPossible)
                    {
                    TDisplayMode mskMode = (frmInfo.iFlags & TFrameInfo::EAlphaChannel) ? EGray256 : EGray2;
                    iContext->iTarget->Create( frmInfo.iOverallSizeInPixels, GetBestDisplayMode(frmInfo.iFrameDisplayMode), mskMode );
                    }
                else
                    {
                    iContext->iTarget->Create( frmInfo.iOverallSizeInPixels, GetBestDisplayMode(frmInfo.iFrameDisplayMode) );
                    }

          LoadOneFrame();
                return ETrue;
                }
      }
    else
      {
      // animated images need a dedicated decoder
            data->iObserver->StartAnimationDecoder();

      iQueue.Remove( 0 );
      Reset();

      StartLoading();
            return ETrue;
      }
    }

    // something must be wrong with the current image
    data->iObserver->decoderError( -1 );
    iQueue.Remove(0);

    // decode next image
    Reset();
    StartLoading();

    return ETrue;
    }

void CStaticImageDecoder::LoadOneFrame()
    {
    // static image has only one frame;
    const TFrameInfo& frameInfo = iDecoder->FrameInfo( 0 );
    CFbsBitmap& bmp = iContext->iTarget->BitmapModifyable();
    if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
        {
        CFbsBitmap& msk = iContext->iTarget->MaskModifyable();
        iDecoder->Convert( &iStatus, bmp, msk, 0 );
        }
    else
        {
        iDecoder->Convert( &iStatus, bmp, 0 );
        }

    // the first frame, partial information is available
    iContext->iObserver->PartialImage();

    SetActive();
    iState = EOneFrameReady;
    }

TBool CStaticImageDecoder::DecodeL( CRawData* aData )
    {
    // queue the data, FIFO
    iQueue.AppendL( aData );
    StartLoading();
    return ETrue;
    }

void CStaticImageDecoder::StartLoading()
    {
    if( !IsActive() )
        {
        // initalize decoding
      TRequestStatus* status = &iStatus;
        User::RequestComplete( status, 0 );
        SetActive();
        iState = EStartLoading;
        }
    }

void CStaticImageDecoder::RunL()
    {
    if( iState == EStartLoading )
        {
        LoadNextImage();
        }
    else if( iState == EOneFrameReady )
        {
        if( iStatus.Int() != KErrNone )
            {
            iContext->iObserver->decoderError( iStatus.Int() );

            iQueue.Remove(0);

            // decode next image
            Reset();
            if( iQueue.Count() > 0 ) LoadNextImage();
            }
        else
            {
            // pass bitmap's ownership to the observer
            iContext->iObserver->ImageReady();

            iQueue.Remove(0);

            // decode next image
            Reset();
            if( iQueue.Count() > 0 ) LoadNextImage();
            }
        }
    }

void CStaticImageDecoder::DoCancel()
    {
    // TODO: maybe we should delete all the intermediate data?
    }

void CStaticImageDecoder::Reset()
    {
    // decoder reset
    iDecoder->Cancel();
    iDecoder->Reset();
    iState = EIdle;

    // context reset
    iContext = 0;
    }

void CStaticImageDecoder::StopObserving( MAnimationDecoderObserver* aObserv )
    {
    // fast check
    if( iQueue.Count() == 0 ) return;

    // is the image currently being decoded?
    if( iContext && iContext->iObserver == aObserv )
        {
        // stop decoding
        if( IsActive() ) Cancel();
        }

    // remove image data from the queue
    TInt idx = KErrNotFound;
    for( TInt i=0; i<iQueue.Count(); ++i )
        if( iQueue[i]->iObserver == aObserv )
            idx = i;
    if( idx == KErrNotFound ) return;

    CRawData* data = iQueue[idx];
    iQueue.Remove( idx );

    // decode next image
    Reset();
    if( iQueue.Count() > 0 ) StartLoading();
    }

TDisplayMode CStaticImageDecoder::GetBestDisplayMode( TDisplayMode /*aMode*/ ) const
    {
    return EColor64K;
    }

TBool CStaticImageDecoder::CheckBitmapMemoryConsumption( const TFrameInfo& aFrameInfo ) const
    {
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo( info );
    TInt freeRamInBytes = 10*1024*1024;
    TInt dataSize = iContext->iDataPtr.Length();
    if( UserHal::MemoryInfo( info ) == KErrNone )
        freeRamInBytes = info().iFreeRamInBytes;
    TInt bitmapBytes = aFrameInfo.iOverallSizeInPixels.iWidth*aFrameInfo.iOverallSizeInPixels.iHeight*2;
    TInt freeRamAfterLoad = freeRamInBytes-bitmapBytes;
    if (freeRamAfterLoad < KFreeRamBitmapBlockLimit)
        {
        // block image decoding when we are below block limit
        return EFalse;
        }
    if (freeRamAfterLoad < KFreeRamBitmapCheckLimit)
        {
        // low on memory, do additional checks
        if (dataSize*KHighExpansionBitmapFactor < bitmapBytes && bitmapBytes>KLargeHighExpansionBitmapSize )
            {
            // bitmaps with high expansion factor are commonly used as background image etc. and are of less importance
            // block large ones
            return EFalse;
            }
        if (freeRamInBytes*KMaxBitmapRamPercent/100 < bitmapBytes)
            {
            // block all bitmaps that would consume more than this percent of available memory
            return EFalse;
            }
        }
    return ETrue;
    }

// END OF FILE
