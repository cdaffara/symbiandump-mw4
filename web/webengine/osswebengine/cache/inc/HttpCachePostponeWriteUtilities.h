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

#ifndef CHTTPCACHEPOSTPONEWRITEUTILS_H
#define CHTTPCACHEPOSTPONEWRITEUTILS_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

// CONSTANTS

// MACROS
#define CACHE_POSTPONE_DEBUG_PRINT
// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class RFileWriteStream;
class RFileReadStream;
class CHttpCacheFileWriteHandler;
class CHttpCacheHandler;
class MHttpCacheWriteSource;

/*
 * CHttpCacheEntryAsyncWriteHelper is responsible for asynchronously writing out the data from a CHttpCacheStreamEntry object.
 *
 * It is used in CHttpCacheHandler::FlushAsync() to provide the async write capability.
 *
 * At creation, it begins to write the first segment of body data.  As each segment completes, the next is written.
 * Once the body data is written, the headers are written and the status from the header write is returned to the observer.
 */
NONSHARABLE_CLASS(CHttpCacheEntryAsyncWriteHelper) : public CActive
    {
public:
    virtual ~CHttpCacheEntryAsyncWriteHelper();

    static CHttpCacheEntryAsyncWriteHelper* NewL(MHttpCacheWriteSource* aSource, TRequestStatus& aStatus);
    TInt GetResult();   // KRequestPending, or a completion code.

private:
    CHttpCacheEntryAsyncWriteHelper(TRequestStatus& aStatus, MHttpCacheWriteSource* aSource, TInt aPriority = EPriorityIdle);

    virtual void DoCancel();
    virtual void RunL();

    void ConstructL();
    void WriteNextBodyBlock();

private:
    const TRequestStatus& iSignalStatus;
    MHttpCacheWriteSource* iSource;
    TInt iBodyPart;
    TBool iDone;
    };



/*
 * CSegmentedHeapBuffer is a simple segmented buffer.  You can add data progressively and storage will
 * be automatically provided.
 *
 * When creating one of these, you provide aBufferSize which is the segment size and aCompressGranularity which is the
 * multiple which is used to determine if the final block should be shrunk when Compress() is called.
 */
NONSHARABLE_CLASS(CSegmentedHeapBuffer) : public CBase
    {
public:
    virtual ~CSegmentedHeapBuffer();
    /*
     * aBufferSize is the size of each segment used for storage.
     * aCompressGranularity is used when Compress()ing the last block. If the block has at least this much free space, it is realloced to the correct size.
     */
    static CSegmentedHeapBuffer * NewL(TInt aBufferSize = 32768, TInt aCompressGranularity = 4096);

    /*
     * @since 5.0
     * @param aRemainder [out] - returns the amount of data not consumed from aDes.
     * @param aDes [in] - the data to append.
     * @return void
     * Add some data to the buffer.
     *
     * This function will append the data from aDes in chunks of iBufferSize.
     * aRemainder will normally be zero after completion of this call, however if AppendL
     * does leave, then aRemainder will tell you how many bytes from aDes have not been consumed.
     * This is intended for calling code to cope in the case where the buffer cannot be extended (KErrNoMemory)
     * to contain the whole descriptor.  Calling code can take remedial action and know exactly how much data
     * is left to handle.
     */
    void AppendL(TInt& aRemainder, const TDesC8& aDes);

    /*
     * @since 5.0
     * @param aSegment [in & out] - in -> the segment to address, out -> next segment.
     * @return Segment data descriptor.
     *
     * Access data in the buffer.
     *
     * Simply returns descriptor giving access to the desired segment.
     * Normal usage is to call with aSegment initialised to zero.
     * Repeated calls with same aSegment will result in iterator behaviour.
     * Do not call past number of available segments as returned by Count() - this will result in a panic from
     * addressing past the end of the RPointerArray.
     */
    TPtrC8 GetSegmentData(TInt& aSegment);

    /*
     * @since 5.0
     * @param aSegment [in] - the segment to free.
     * @return void
     *
     * Release data in the buffer.
     *
     * Frees the requested segment without changing segment numbering and order.
     */
    void ReleaseSegmentData(const TInt aSegment);

    TInt Length();          // amount of data stored
    TInt SpareCapacity();   // spare space in last block
    TInt Count();           // number of blocks

    void Compress();        // release space at end of last block.
    void Reset();           // release all allocated storage
private:
    void ConstructL();
    CSegmentedHeapBuffer(TInt aBufferSize, TInt aCompressGranularity);

private:
    const TInt iBufferSize;
    const TInt iCompressGranularity;
    RPointerArray<HBufC8> iBufferList;
    };
// CLASS DECLARATION


NONSHARABLE_CLASS(MHttpCacheWriteSource)
    {
public:
    virtual RFile& BodyFile() = 0;
    virtual void   BodyWriteInProgress() = 0;
    virtual void   BodyWriteComplete() = 0;
    virtual CSegmentedHeapBuffer& BodyData() = 0;
    };

// calls the given function when the preset timer expires.
NONSHARABLE_CLASS(CHttpCacheWriteTimeout) : public CActive
    {
    public:
        // Cancel and destroy
        ~CHttpCacheWriteTimeout ();

        // Two-phased constructor.
        static CHttpCacheWriteTimeout* NewL (const TInt aTimeout);

        // Two-phased constructor.
        static CHttpCacheWriteTimeout* NewLC (const TInt aTimeout);

    public:
        // New functions
        // Function for making the initial request
        void Start(TCallBack aCallbackFn, TAny *aToken);

    private:
        // C++ constructor
        CHttpCacheWriteTimeout (const TInt aTimeout);

        // Second-phase constructor
        void ConstructL ();

    private:
        // From CActive
        // Handle completion
        void RunL ();

        // How to cancel me
        void DoCancel ();

        // Override to handle leaves from RunL(). Default implementation causes
        // the active scheduler to panic.
        TInt RunError (TInt aError);

    private:
        TInt    iTimeout;
        RTimer iTimer; // Provides async timing service
        TCallBack iCallbackFn;
        TAny *iToken;
    };



#endif      // CHTTPCACHEPOSTPONEWRITEUTILS_H

// End of File
