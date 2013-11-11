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
* Description:  Implementation of CHttpCachePostponeWriteUtilities
*
*/

// INCLUDE FILES
#include "HttpCachePostponeWriteUtilities.h"
#include "HttpCacheUtil.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::CHttpCacheEntryAsyncWriteHelper
// -----------------------------------------------------------------------------
//
CHttpCacheEntryAsyncWriteHelper::CHttpCacheEntryAsyncWriteHelper(TRequestStatus& aStatus, MHttpCacheWriteSource* aSource, TInt aPriority)
    : CActive(aPriority),
      iSignalStatus(aStatus),
      iSource(aSource)
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::NewL
// -----------------------------------------------------------------------------
//
CHttpCacheEntryAsyncWriteHelper* CHttpCacheEntryAsyncWriteHelper::NewL(MHttpCacheWriteSource* aSource, TRequestStatus& aStatus)
    {
    CHttpCacheEntryAsyncWriteHelper *obj = new (ELeave) CHttpCacheEntryAsyncWriteHelper(aStatus, aSource, EPriorityHigh);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::~CHttpCacheEntryAsyncWriteHelper
// -----------------------------------------------------------------------------
//
CHttpCacheEntryAsyncWriteHelper::~CHttpCacheEntryAsyncWriteHelper()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::DoCancel
// -----------------------------------------------------------------------------
//
void CHttpCacheEntryAsyncWriteHelper::DoCancel()
    {
    TRequestStatus *stat = &(TRequestStatus&)iSignalStatus;
    User::RequestComplete(stat, KErrCancel);    // Signal cancellation to the observer.
    // cannot meaningfully do anything with the unwritten data assuming there is some.
    // we will clean it up when we are deleted.
    iSource->BodyFile().Close();
    iSource->BodyWriteComplete();
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheEntryAsyncWriteHelper::DoCancel called on object %08x"), this);
#endif
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::WriteNextBodyBlock
// -----------------------------------------------------------------------------
//
void CHttpCacheEntryAsyncWriteHelper::WriteNextBodyBlock()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheEntryAsyncWriteHelper::WriteNextBodyBlock called on object %08x for block %d"), this, iBodyPart );
#endif

    TPtrC8 bufferPtr( iSource->BodyData().GetSegmentData(iBodyPart) );
    iSource->BodyFile().Write(bufferPtr, iStatus);
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::RunL
// -----------------------------------------------------------------------------
//
void CHttpCacheEntryAsyncWriteHelper::RunL()
    {
    /* General algorithm.
     * Write out next section of body data unless done.  Then write out header data.
     */
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheEntryAsyncWriteHelper::RunL called on object %08x"), this);
#endif
    if ( iSource->BodyData().Count() > iBodyPart )
        {
        WriteNextBodyBlock();
        SetActive();
        }
    else
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:   - body write completed with status code %d"), iStatus.Int());
#endif
        // Body file writing is complete
        iSource->BodyFile().Close();
        iSource->BodyWriteComplete();
        TRequestStatus *stat = &(TRequestStatus&)iSignalStatus;
        User::RequestComplete(stat, iStatus.Int()); // signal completion to observer
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::GetResult
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEntryAsyncWriteHelper::GetResult()
    {
    return iStatus.Int();
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntryAsyncWriteHelper::ConstructL
// -----------------------------------------------------------------------------
//
void CHttpCacheEntryAsyncWriteHelper::ConstructL()
    {
    CActiveScheduler::Add(this);
    iSource->BodyWriteInProgress();
    // setup to arrive in RunL when next possible.
    SetActive();
    TRequestStatus *stat = &(TRequestStatus&)iStatus;
    User::RequestComplete(stat, KErrNone);
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::~CSegmentedHeapBuffer
// -----------------------------------------------------------------------------
//
CSegmentedHeapBuffer::~CSegmentedHeapBuffer()
    {
    Reset();
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::ConstructL
// -----------------------------------------------------------------------------
//
void CSegmentedHeapBuffer::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::NewL
// -----------------------------------------------------------------------------
//
CSegmentedHeapBuffer *CSegmentedHeapBuffer::NewL(TInt aBufferSize, TInt aCompressGranularity)
    {
    CSegmentedHeapBuffer *obj= new (ELeave) CSegmentedHeapBuffer(aBufferSize, aCompressGranularity);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);

    return obj;
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::AppendL
// -----------------------------------------------------------------------------
//
void CSegmentedHeapBuffer::AppendL(TInt& aRemainder, const TDesC8& aDes)
    {
    aRemainder = aDes.Length(); // consumed nothing yet.
    HBufC8* currentBuffer;

    // 90% of cached objects are less than 4KB.
    // Lots of them also come in two parts from the http stack.
#define HTTPSEGMENTEDBUFFER_OPTION_INCREMENT_GRADUALLY
#ifndef HTTPSEGMENTEDBUFFER_OPTION_INCREMENT_GRADUALLY
    TInt lastBuffer = iBufferList.Count()-1;
    if ( lastBuffer < 0 )
        {
        // TODO: Make the first block only equal to the size of data we need?
        // Take some traces to see what happens.
        // no blocks allocated.  May leave here if we can't get space.
        currentBuffer = HBufC8::NewLC(iBufferSize);
        iBufferList.AppendL(currentBuffer);
        CleanupStack::Pop(currentBuffer);
        lastBuffer = 0;
        }
    else
        {
        currentBuffer = iBufferList[lastBuffer];
        }

    TInt workingLen;
    TInt workingOffset=0;   // read position in source descriptor
    // here, currentBuffer always points to a buffer we can use.
    while ( aRemainder )
        {
        workingLen = iBufferSize - currentBuffer->Length(); // workingLen = amount of space left in this segment
        workingLen = (aRemainder > workingLen) ? workingLen : aRemainder;   // workingLen = smaller of (amount of data left in source) or (amount of space left in current segment)
        if ( workingLen )
            {
            // we have some space in this block to store data.
            TPtr8 ptr(currentBuffer->Des());
            ptr.Append(aDes.Mid(workingOffset ,workingLen));
            }
        aRemainder -= workingLen;   // track how much is consumed
        workingOffset += workingLen;    // remember where we get the next bit from in the source
        if ( aRemainder )
            {
            // we have more data to store, append another block.  Might result in a leave.
            currentBuffer = HBufC8::NewLC(iBufferSize);
            iBufferList.AppendL(currentBuffer);
            CleanupStack::Pop(currentBuffer);
            }
        }
#else
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CHttpCacheSegmentedBuffer::AppendL %08x adding %d bytes to %d"), this, aRemainder, this->Length());
#endif
    // because most items are small, increment buffers up to the configured segment size as data is added...
    TInt workingOffset = 0;
    TInt lastBuffer = iBufferList.Count()-1;
    if ( lastBuffer < 0 )
        {
        // special case for first allocation.
        // no blocks allocated.  May leave here if we can't get space.
        if( aRemainder <= iBufferSize)  // we can fit the first block into a single segment
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L(" First alloc %d into buffer fits inside %d"), aRemainder, iBufferSize );
#endif
            // fast path optimisation for first alloc into an empty segmented buffer.
            currentBuffer = aDes.AllocLC();
            iBufferList.AppendL( currentBuffer );
            CleanupStack::Pop( currentBuffer );
            aRemainder = 0;
            }
        else
            {
            // the segmented buffer is empty and the first block to add is bigger than the configured block size
            // fill the first segment and leave the rest for the loop
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L(" First alloc %d is bigger than %d"), aRemainder, iBufferSize );
#endif
            currentBuffer = HBufC8::NewLC( iBufferSize );
            iBufferList.AppendL( currentBuffer );
            CleanupStack::Pop( currentBuffer );
            currentBuffer->Des().Copy( aDes.Ptr(), iBufferSize );
            workingOffset = iBufferSize;    // when we add the remaining data, we start from here.
            aRemainder -= iBufferSize;
            }
        }
    else
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L(" Buffer already contains data"));
#endif
        currentBuffer = iBufferList[lastBuffer];
        }
    
    // When we get to here the following state applies.
    // currentBuffer points to an allocated and filled HBufC8
    // workingOffset tells us how far into the supplied descriptor the data we want is
    // aRemainder tells us how much data is left to copy.
    while( aRemainder )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L(" %d bytes left to add to buffer"), aRemainder);
#endif
        TInt possibleConsumptionInThisBlock = iBufferSize - currentBuffer->Length();
        if( possibleConsumptionInThisBlock == 0 )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L(" Buffer cannot be extended."));
#endif
            // block cannot extend, alloc a new one
            // the new one is either the correct length, or iBufferSize if aRemainder is too big.
            TInt spaceToAlloc = aRemainder < iBufferSize ? aRemainder : iBufferSize;
            currentBuffer = HBufC8::NewLC( spaceToAlloc );
            iBufferList.AppendL( currentBuffer );
            CleanupStack::Pop( currentBuffer );
            possibleConsumptionInThisBlock = spaceToAlloc;            
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L(" New buffer of %d bytes allocated"), spaceToAlloc);
#endif
            // fill the block as far as possible
            currentBuffer->Des().Append( aDes.Mid( workingOffset, possibleConsumptionInThisBlock ));
            workingOffset += possibleConsumptionInThisBlock;
            aRemainder -= possibleConsumptionInThisBlock;
            }
        else
            {
            // block can extend
            if( possibleConsumptionInThisBlock >= aRemainder )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteFormatLog(0, _L(" Current buffer can be extended to hold all data."));
#endif
                // we can realloc this buffer big enough to hold all the remaining data.
                currentBuffer = currentBuffer->ReAllocL( currentBuffer->Length() + aRemainder );
                CleanupStack::PushL( currentBuffer );
                iBufferList.Remove(iBufferList.Count()-1);
                iBufferList.AppendL( currentBuffer );
                CleanupStack::Pop( currentBuffer );
                // copy the data
                currentBuffer->Des().Append( aDes.Mid( workingOffset, aRemainder ));
                aRemainder = 0;
                }
            else
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteFormatLog(0, _L(" Buffer cannot be extended to hold all data, consuming %d bytes."), possibleConsumptionInThisBlock);
#endif
                // this buffer cannot extend to hold all the data.
                // take as much as we can - we will allocate a new buffer next time around.
                currentBuffer = currentBuffer->ReAllocL( currentBuffer->Length() + possibleConsumptionInThisBlock );
                CleanupStack::PushL( currentBuffer );
                iBufferList.Remove(iBufferList.Count()-1);
                iBufferList.AppendL( currentBuffer );
                CleanupStack::Pop( currentBuffer );
                // copy the data
                currentBuffer->Des().Append( aDes.Mid( workingOffset, possibleConsumptionInThisBlock ));
                // set up variables for next time around
                workingOffset += possibleConsumptionInThisBlock;
                aRemainder -= possibleConsumptionInThisBlock;
                }
            }
        }
#endif
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L(" exiting AppendL. Segmented buffer now contains %d bytes"), this->Length());
#endif
    // will only exit here if we consumed all data
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::Length
// -----------------------------------------------------------------------------
//
TInt CSegmentedHeapBuffer::Length()
    {
    // all segments are same size except last one
    TInt len = 0;
    TInt count = iBufferList.Count();
    if ( count )
        {
        len = iBufferSize * (count - 1);    // all segments are same size except last one
        len += iBufferList[count-1]->Length();
        }
    return len;
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::SpareCapacity
// -----------------------------------------------------------------------------
//
TInt CSegmentedHeapBuffer::SpareCapacity()
    {
    TInt len = 0;
    TInt count = iBufferList.Count();
    if ( count )
        {
        len = iBufferSize - (iBufferList[count-1]->Length());
        }

    // return how much is left in last segment.
    return len;
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::Count
// -----------------------------------------------------------------------------
//
TInt CSegmentedHeapBuffer::Count()
    {
    return iBufferList.Count(); // number of segments
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::GetSegmentData
// -----------------------------------------------------------------------------
//
TPtrC8 CSegmentedHeapBuffer::GetSegmentData(TInt& aSegment)
    {
    return iBufferList[aSegment++]->Des();
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::ReleaseSegmentData
// -----------------------------------------------------------------------------
//
void CSegmentedHeapBuffer::ReleaseSegmentData(const TInt aSegment)
    {
    HBufC8* buf = iBufferList[aSegment];
    iBufferList[aSegment] = 0;  // don't want to reshuffle contents
    delete buf;
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::Compress
// -----------------------------------------------------------------------------
//
void CSegmentedHeapBuffer::Compress()
    {
    TInt count = iBufferList.Count();
    if ( count )
        {
        HBufC8* buf = iBufferList[count-1];
        // first, see if we can shrink by at least one iCompressGranularity
        if ( iBufferSize - buf->Length() >= iCompressGranularity )
            {
            // calculate new size
            TInt newsize = (iCompressGranularity * (1 + (buf->Length() / iCompressGranularity)));
            HBufC8 *newbuf = buf->ReAlloc(newsize);
            // we should be realloc'ing in place since we're shrinking this piece, but you never know.
            // don't care if realloc operation fails since it will leave original data alone.
            if ( newbuf )
                {
                iBufferList[count-1] = newbuf;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
//  CSegmentedHeapBuffer::Reset
// -----------------------------------------------------------------------------
//
void CSegmentedHeapBuffer::Reset()
    {
    iBufferList.ResetAndDestroy();  // call delete on all buffers
    }

// -----------------------------------------------------------------------------
// CSegmentedHeapBuffer::CSegmentedHeapBuffer
// -----------------------------------------------------------------------------
//
CSegmentedHeapBuffer::CSegmentedHeapBuffer(TInt aBufferSize, TInt aCompressGranularity) : iBufferSize(aBufferSize), iCompressGranularity(aCompressGranularity)
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::CHttpCacheWriteTimeout
// -----------------------------------------------------------------------------
//
CHttpCacheWriteTimeout::CHttpCacheWriteTimeout( const TInt aTimeout )
    : CActive(EPriorityIdle),
      iTimeout(aTimeout) // Standard priority
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::NewLC
// -----------------------------------------------------------------------------
//
CHttpCacheWriteTimeout* CHttpCacheWriteTimeout::NewLC(const TInt aTimeout)
    {
    CHttpCacheWriteTimeout* self = new ( ELeave ) CHttpCacheWriteTimeout(aTimeout);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::NewL
// -----------------------------------------------------------------------------
//
CHttpCacheWriteTimeout* CHttpCacheWriteTimeout::NewL(const TInt aTimeout)
    {
    CHttpCacheWriteTimeout* self = CHttpCacheWriteTimeout::NewLC(aTimeout);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::ConstructL
// -----------------------------------------------------------------------------
//
void CHttpCacheWriteTimeout::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
    CActiveScheduler::Add(this); // Add to scheduler
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::~CHttpCacheWriteTimeout
// -----------------------------------------------------------------------------
//
CHttpCacheWriteTimeout::~CHttpCacheWriteTimeout()
    {
    Cancel(); // Cancel any request, if outstanding
    iTimer.Close(); // Destroy the RTimer object
    // Delete instance variables if any
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::DoCancel
// -----------------------------------------------------------------------------
//
void CHttpCacheWriteTimeout::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::Start
// -----------------------------------------------------------------------------
//
void CHttpCacheWriteTimeout::Start(TCallBack aCallbackFn, TAny *aToken)
    {
    Cancel(); // Cancel any request, just to be sure

    iToken = aToken;
    iCallbackFn = aCallbackFn;

    iTimer.After(iStatus, iTimeout); // Set for later
    SetActive(); // Tell scheduler a request is active
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::RunL
// -----------------------------------------------------------------------------
//
void CHttpCacheWriteTimeout::RunL()
    {
    iCallbackFn.iFunction(iToken);
    }

// -----------------------------------------------------------------------------
// CHttpCacheWriteTimeout::RunError
// -----------------------------------------------------------------------------
//
TInt CHttpCacheWriteTimeout::RunError(TInt aError)
    {
    return aError;
    }

//  End of File
