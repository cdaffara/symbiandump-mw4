/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements double buffering storage functionality for OMA downloads
*
*/


#include <bldvariant.hrh>

#include "FileExt.h"
#include "FileSaver.h"
#include "CodBuffStorage.h"
#include "CodLogger.h"
#include "HeaderField.h"
#include <sysutil.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <f32file.h>


#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// ---------------------------------------------------------
// CCodBuffStorage::CCodBuffStorage
// ---------------------------------------------------------
//
CCodBuffStorage::CCodBuffStorage( CFileSaver* aSaver )
:CActive( EPriorityHigh ), 
 iFile(&(aSaver->iFile)),
 iDownloadedSize(aSaver->iSize), 
 iBufferedSize(aSaver->iBufferedSize),
 iHttpStorageBufferSize(aSaver->iBufferSize),
 iHttpStorageLength(aSaver->iLength),
 iProgressiveDownload(aSaver->iProgressiveDownload)
    {
    }

// -----------------------------------------------------------------------------
// CCodBuffStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCodBuffStorage::ConstructL()
    {
   
    CLOG(( ECodStorage, 0, _L("-> CCodBuffStorage::ConstructL") ));
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCodBuffStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCodBuffStorage* CCodBuffStorage::NewL( CFileSaver* aHttpStorage )
    {
    CCodBuffStorage* self = new( ELeave ) CCodBuffStorage( aHttpStorage );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CCodBuffStorage::~CCodBuffStorage
// ---------------------------------------------------------
//
CCodBuffStorage::~CCodBuffStorage()
    {
    Cancel();

    ResetBuffers();

    delete iWritePtr; iWritePtr = 0;
    }



// ---------------------------------------------------------
// CCodBuffStorage::RunL
// ---------------------------------------------------------
//
void CCodBuffStorage::RunL()
    {
    
    CLOG(( ECodStorage, 0, _L("-> CCodBuffStorage::RunL") ));
    // Save the error code
    iLastWriteErrorCode = iStatus.Int();

    if(iLastWriteErrorCode==KErrNone && iWritePtr)
        {
        // Update how much was written on the file
        iDownloadedSize += iWritePtr->Length();
        
        CLOG(( ECodStorage, 2, _L("(%08X) CCodBuffStorage::RunL: Async write finished, downloaded now: %d"), \
                 this, iDownloadedSize ));
        }
    else
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::RunL DH-iStat: %d,"), \
               this, iStatus.Int()  ));
        }
  
    if(iWait.IsStarted())
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::RunL Stopping iWait"), \
               this ));
        iWait.AsyncStop();
        }
    }

void CCodBuffStorage::DoCancel()
    {
    CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoCancel"), \
        this ));

    // This is ok, CActive::Cancel always waits for the async operation to finish in this case
    iLastWriteErrorCode = KErrCancel;
    }

// ---------------------------------------------------------
// CCodBuffStorage::ResetBuffer
// ---------------------------------------------------------
//
void CCodBuffStorage::ResetBuffers()
    {
    CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::ResetBuffers >>"), \
        this ));


    if(IsActive())
        {
        // Make sure async writes are finished
        iWait.Start();
        }

    // Cleanup
    delete iBuff1; iBuff1 = NULL;
    delete iBuff2; iBuff2 = NULL;
    iClientBuffer = NULL;
    iBufferSize = 0;
    
    CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::ResetBuffers <<"), \
           this ));

    }
    
    
// ---------------------------------------------------------
// CCodBuffStorage::WriteOutNextBodyDataL
// ---------------------------------------------------------
//
TBool CCodBuffStorage::WriteOutNextBodyDataL( const TDesC8& aBuf )
    {
    TBool retVal( ETrue );
    
    if( iFile )
        {
        
        if(iDownloadedSize <= 0)
            {
            TInt len = aBuf.Length();
            User::LeaveIfError( iFile->Write( aBuf ) );
            User::LeaveIfError( iFile->Flush() );
    	    // Update how much written to file
   		    iDownloadedSize += len;
            }
        else
            {
            DoBufferingWriteL(aBuf);    
            }
        }        
  

    if( iHttpStorageLength != KDefaultContentLength &&
        iBufferedSize > iHttpStorageLength )
        // we don't know actually how many bytes will come down.
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::WriteOutNextBodyDataL: Length modified"), \
               this ));

        iHttpStorageLength = KDefaultContentLength;
        retVal = EFalse;
        }
        
    return retVal;
    }

// ---------------------------------------------------------
// CCodBuffStorage::FlushBuffersL
// ---------------------------------------------------------
//
void CCodBuffStorage::FlushBuffersL()
    {
    if(!iFile)
        {
        return;
        }

    CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL >>"), \
           this ));


    // Make sure async writes are finished before doing anything
    if(IsActive())
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL  stalling >>"), \
                this ));

        iWait.Start();
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL  stalling <<"), \
               this ));

        }

     if(iLastWriteErrorCode != KErrNone)
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL: last error = %d"), \
               this, iLastWriteErrorCode ));

        TInt err = iLastWriteErrorCode;
        iLastWriteErrorCode = KErrNone;
        User::Leave(err);
        }

    if(iClientBuffer)
        {
        // Flush whatever is left on the client buffer
        TInt len = iClientBuffer->Length();

        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL: %d bytes"), \
            this, len ));


        if(len > 0)
            {
            // Write to file
            TPtr8 des(iClientBuffer->Des());

            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL before iFile->Write(des)"),this ));
            User::LeaveIfError(iFile->Write(des));
            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL after iFile->Write(des)"), this ));

             // Update how much was written to file
            iDownloadedSize += len;

            des.Zero();
            }
        }

    CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::FlushBuffersL  <<"), \
        this ));

   }

// ---------------------------------------------------------
// CCodBuffStorage::DoBufferingWriteL
// ---------------------------------------------------------
//        
void CCodBuffStorage::DoBufferingWriteL(const TDesC8& aBuf)
    {
    if((iBufferSize != iHttpStorageBufferSize )|| (iClientBuffer == NULL))
        {
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: Allocate %d - downloaded %d"), \
        this, iHttpStorageBufferSize, iBufferedSize ));

        
        // Make sure nothing is leaked
        delete iBuff1; iBuff1 = 0;
        delete iBuff2; iBuff2 = 0;
        iBufferSize = 0;
        
        // Allocate buffers
        iBuff1 = HBufC8::NewL(iHttpStorageBufferSize);
        iBuff2 = HBufC8::NewL(iHttpStorageBufferSize);

        // Mark that we now have buffers that are iBufferSize big
        iBufferSize = iHttpStorageBufferSize;

        // Set client to use buffer 1
        iClientBuffer = iBuff1;
        
        }

    // Get incoming data pointer and size
    const TUint8* src = aBuf.Ptr();
    TInt incomingBuffSize = aBuf.Length();
    
    // Process the data in blocks  
    while(incomingBuffSize)
        {
        // Get pointer to current client buffer data (at start of loop since iClientBuffer can change)
        TPtr8 buff = iClientBuffer->Des();
        TInt buffLength = buff.Length();

        // How much space in the buffer
        TInt toFillUp = iBufferSize - buffLength;

        // Check if we can just fit everything in the current buffer
        if(incomingBuffSize < toFillUp)
            {  
            // Dump data on the current client buffer since ample space exists
            buff.Append(src, incomingBuffSize);

            // Update the 'received' counter on how much we got from air
            iBufferedSize += incomingBuffSize;

            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: Copied %d, now %d / %d"), \
                   this, incomingBuffSize, buff.Length(), iBufferSize ));


            // And break out of the loop since we're finished for this round
            break;
            }
            
        // At this point we have data that either fills or overflows the buffer so append as much as fits
        buff.Append( src, toFillUp );

        // Update the 'received' counter on how much we copied
        iBufferedSize+=toFillUp;

        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: Copied %d, now %d / %d"), \
               this, toFillUp, buff.Length(), iBufferSize ));


        // Subtract how much we copied from the loop counter and advance source data pointer
        incomingBuffSize -= toFillUp;
        src+=toFillUp;

        // Now we have a full client buffer, better do something with it
    
        // Check if previous async write is still ongoing
        // Done here so if somebody switched on progressive download midway through we don't mix buffers
        if(IsActive())
            {
            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: stalling >>"), \
                   this ));

            iWait.Start();
            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: stalling <<"), \
                   this ));

             }

        // In case of async writes we have to check if there was error previously
        if(iLastWriteErrorCode != KErrNone)
            {
            // Clear the error code and leave with it
            TInt err = iLastWriteErrorCode;
            iLastWriteErrorCode = KErrNone;
            User::Leave(err);
            }

        if(iProgressiveDownload)
            {
            CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL: Buffer full -> Progressive download"), \
                   this ));

            TInt len = buff.Length();

            // Progressive download -> just do a synchronous write and update counter
            User::LeaveIfError(iFile->Write(buff));
            
            // Update received counter
            //iBufferedSize += toFillUp;

            // Update how much was written to file (sync write)
            iDownloadedSize += len;

            // Clear out client buffer
            buff.Zero();
            
            // Continue for next write
            continue;
            }

        // We must keep the descriptor alive until async operations have finished
        if(!iWritePtr)
            {
            iWritePtr = new (ELeave) TPtr8(iClientBuffer->Des());
            }
        else
            {
            iWritePtr->Set(iClientBuffer->Des());
            }

#ifdef _DEBUG
        // Print out file position so we can check alignments
        TInt currentPos(0);
        iFile->Seek(ESeekCurrent, currentPos);
        
        CLOG(( ECodStorage, 2, _L("(%08X)  CCodBuffStorage::DoBufferingWriteL - Start async write %d bytes, filepos=%d"), \
               this, iWritePtr->Length(), currentPos ));

#endif

        // Start the async write and set AO active
        iFile->Write( *iWritePtr, iStatus ) ;
        SetActive();

        // Swap buffer pointers (we can't use the same buffer here 
        // since it is not known when the buffer can be used again)
        if(iClientBuffer == iBuff1)
            {
            iClientBuffer = iBuff2;
            }
        else
            {
            iClientBuffer = iBuff1;
            }

        // Make sure the new client buffer is empty
        iClientBuffer->Des().Zero();
        }
    }
