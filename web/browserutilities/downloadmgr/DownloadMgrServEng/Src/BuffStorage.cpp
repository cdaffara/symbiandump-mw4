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
* Description:  Implements the double buffering functionality in the DownloadManager
*
*/


#include <bldvariant.hrh>

#include "FileExt.h"
#include "HttpClientApp.h"
#include "HttpDownload.h"
#include "HttpStorage.h"
#include "BuffStorage.h"

#include "HttpDownloadManagerServerEngine.h"
#include "HttpDownloadMgrLogger.h"
#include "HeaderField.h"

#include <sysutil.h>
#include <DocumentHandler.h>
#include <apmstd.h>

#include "HttpDownloadMgrLogger.h"

#ifdef __SYNCML_DM_FOTA
#include <fotaengine.h>
#endif

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
// CBuffStorage::CBuffStorage
// ---------------------------------------------------------
//
CBuffStorage::CBuffStorage( CHttpStorage* aHttpStorage )
:CActive( EPriorityHigh ), 
 iFile(aHttpStorage->iFile),
 iDownloadedSize(aHttpStorage->iDownloadedSize), 
 iBufferedSize(aHttpStorage->iBufferedSize),
 iHttpStorageBufferSize(aHttpStorage->iBufferSize),
 iHttpStorageLength(aHttpStorage->iLength),
 iBufferingEnabled(aHttpStorage->iBufferingEnabled),
 iProgressiveDownload(aHttpStorage->iProgressiveDownload)
#ifdef __SYNCML_DM_FOTA
 ,iFotaStream(aHttpStorage->iFotaStream)
#endif // __SYNCML_DM_FOTA
    {
    }

// -----------------------------------------------------------------------------
// CBuffStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBuffStorage::ConstructL()
    {
    LOGGER_ENTERFN( "ConstructL" );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CBuffStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBuffStorage* CBuffStorage::NewL( CHttpStorage* aHttpStorage )
    {
    CBuffStorage* self = new( ELeave ) CBuffStorage( aHttpStorage );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CBuffStorage::~CBuffStorage
// ---------------------------------------------------------
//
CBuffStorage::~CBuffStorage()
    {
    Cancel();

	ResetBuffers();
	
	delete iWritePtr; iWritePtr = 0;
    }



// ---------------------------------------------------------
// CBuffStorage::RunL
// ---------------------------------------------------------
//
void CBuffStorage::RunL()
    {
    LOGGER_ENTERFN( "RunL" );
    // Save the error code
    iLastWriteErrorCode = iStatus.Int();
   	
    if(iLastWriteErrorCode==KErrNone && iWritePtr)
    	{
    	// Update how much was written on the file
    	iDownloadedSize += iWritePtr->Length();
    	
    	CLOG_WRITE_2( "(%08X) CBuffStorage::RunL: Async write finished, downloaded now: %d", this, iDownloadedSize);
    	}
    else
    	{
    	CLOG_WRITE_2( "(%08X) CBuffStorage::RunL DH-iStat: %d, ", this, iStatus.Int() );
    	}
    	
    if(iWait.IsStarted())
		{
		CLOG_WRITE_1 ( "(%08X) CBuffStorage::RunL() Stopping iWait", this );
		iWait.AsyncStop();
		}
    }

void CBuffStorage::DoCancel()
	{
	CLOG_WRITE_1("(%08X) CBuffStorage::DoCancel", this);
	
	// This is ok, CActive::Cancel always waits for the async operation to finish in this case
	iLastWriteErrorCode = KErrCancel;
	}

// ---------------------------------------------------------
// CBuffStorage::ResetBuffer
// ---------------------------------------------------------
//
void CBuffStorage::ResetBuffers()
	{	
	CLOG_WRITE_1("(%08X) CBuffStorage::ResetBuffers >>", this);
	
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
    
    CLOG_WRITE_1("(%08X) CBuffStorage::ResetBuffers <<", this);
	}
    
// ---------------------------------------------------------
// CBuffStorage::WriteOutNextBodyDataL
// ---------------------------------------------------------
//
TBool CBuffStorage::WriteOutNextBodyDataL( const TDesC8& aBuf )
    {
    TBool retVal( ETrue );
    
    if( iFile )
        {
        if( iBufferingEnabled )
            {
            DoBufferingWriteL(aBuf);
        	}
        else
        	{
        	DoNonbufferingWriteL(aBuf);
        	}
        }        
#ifdef __SYNCML_DM_FOTA                
    else if( iFotaStream )
        {
        iFotaStream->WriteL( aBuf );
        }
#endif  // __SYNCML_DM_FOTA
     

    if( iHttpStorageLength != KDefaultContentLength &&
        iBufferedSize > iHttpStorageLength )
        // we don't know actually how many bytes will come down.
        {
        CLOG_WRITE_1( "(%08X) CBuffStorage::WriteOutNextBodyDataL: Length modified", this);
        iHttpStorageLength = KDefaultContentLength;
        retVal = EFalse;
        }
        
    return retVal;
    }

// ---------------------------------------------------------
// CBuffStorage::FlushBuffersL
// ---------------------------------------------------------
//
void CBuffStorage::FlushBuffersL()
	{
	if(!iFile)
		{
		return;
		}
		
	CLOG_WRITE_1("(%08X) CBuffStorage::FlushBuffersL >>", this);
	
	// Make sure async writes are finished before doing anything
	if(IsActive())
		{
		CLOG_WRITE_1("(%08X) CBuffStorage::FlushBuffersL: stalling >>", this);
	 	iWait.Start();
	 	CLOG_WRITE_1("(%08X) CBuffStorage::FlushBuffersL: stalling <<", this);
		}
	
	if(iLastWriteErrorCode != KErrNone)
		{
		CLOG_WRITE_2("(%08X) CBuffStorage::FlushBuffersL: last error = %d", this, iLastWriteErrorCode);
		TInt err = iLastWriteErrorCode;
		iLastWriteErrorCode = KErrNone;
		User::Leave(err);
		}
	
	if(iClientBuffer)
		{
		// Flush whatever is left on the client buffer
		TInt len = iClientBuffer->Length();
		
		CLOG_WRITE_2("(%08X) CBuffStorage::FlushBuffersL: %d bytes", this, len);
		if(len > 0)
			{
			// Write to file
			TPtr8 des(iClientBuffer->Des());
			
			User::LeaveIfError(iFile->Write(des));
			
			// Update how much was written to file
			iDownloadedSize += len;
			
			des.Zero();
			}
		}
	
	CLOG_WRITE_1("(%08X) CBuffStorage::FlushBuffersL <<", this);
	}

// ---------------------------------------------------------
// CBuffStorage::DoBufferingWriteL
// ---------------------------------------------------------
//        
void CBuffStorage::DoBufferingWriteL(const TDesC8& aBuf)
	{	
    if(iBufferSize != iHttpStorageBufferSize)
        {
        CLOG_WRITE_3("(%08X) CBuffStorage::DoBufferingWriteL: Allocate %d - downloaded %d", this, iHttpStorageBufferSize, iBufferedSize);
        
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
	    	
	    	CLOG_WRITE_4("(%08X) CBuffStorage::DoBufferingWriteL: Copied %d, now %d / %d", this, incomingBuffSize, buff.Length(), iBufferSize);
	    	
	    	// And break out of the loop since we're finished for this round
			break;
			}
		
		// At this point we have data that either fills or overflows the buffer so append as much as fits
		buff.Append( src, toFillUp );
		
		// Update the 'received' counter on how much we copied
		iBufferedSize+=toFillUp;
		
		CLOG_WRITE_4("(%08X) CBuffStorage::DoBufferingWriteL: Copied %d, now %d / %d", this, toFillUp, buff.Length(), iBufferSize);
		
		// Subtract how much we copied from the loop counter and advance source data pointer
		incomingBuffSize -= toFillUp;
		src+=toFillUp;
	    
	    // Now we have a full client buffer, better do something with it
	    
	    // Check if previous async write is still ongoing
	    // Done here so if somebody switched on progressive download midway through we don't mix buffers
		if(IsActive())
			{
			CLOG_WRITE_1("(%08X) CBuffStorage::DoBufferingWriteL: stalling >>", this);
		 	iWait.Start();
		 	CLOG_WRITE_1("(%08X) CBuffStorage::DoBufferingWriteL: stalling <<", this);
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
	    	CLOG_WRITE_1("(%08X) CBuffStorage::DoBufferingWriteL: Buffer full -> Progressive download", this);
	    	TInt len = buff.Length();
	    	
	    	// Progressive download -> just do a synchronous write and update counter
	    	User::LeaveIfError(iFile->Write(buff));
	    	
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
		CLOG_WRITE_3("(%08X) CBuffStorage::DoBufferingWriteL - Start async write %d bytes, filepos=%d", this, iWritePtr->Length(), currentPos);
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


// ---------------------------------------------------------
// CBuffStorage::DoNonbufferingWriteL
// ---------------------------------------------------------
//	
void CBuffStorage::DoNonbufferingWriteL(const TDesC8& aBuf)
	{
	CLOG_WRITE_2("(%08X) CBuffStorage::DoNonbufferingWriteL: %d bytes", this, aBuf.Length());
	
	if(IsActive())
		{
		CLOG_WRITE_1("(%08X) CBuffStorage::DoNonbufferingWriteL: stalling >>", this);
		iWait.Start();
		CLOG_WRITE_1("(%08X) CBuffStorage::DoNonbufferingWriteL: stalling <<", this);
		}
		
	TInt len = aBuf.Length();
	if(len)
		{
    	User::LeaveIfError( iFile->Write( aBuf ) );
    
    	if( iProgressiveDownload )
        	{
        	User::LeaveIfError( iFile->Flush() );
    		}
		
		// Update how much received from air
		iBufferedSize += len;
		
		// Update how much written to file
   		iDownloadedSize += len;
   		}
    }


//  End of File  
