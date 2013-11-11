/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpHttpFileTransferReader is a class responsible for 
*                   asynchronous reading from socket owned by CUpnpTcpSession.
*
*/



#ifndef C_CUPNPHTTPFILETRANSFERREADER_H
#define C_CUPNPHTTPFILETRANSFERREADER_H


#include <e32base.h>
#include <in_sock.h>
#include "upnpnotifytimer.h"

//constants
static const TInt KMaxRetryErrors = 5;
static const TInt KRetryWaitTime = 1000000;
static const TInt KTwoSec = 2000000;
static const TInt KOneSec = 1000000;

//FORWARD DECLARATIONS
class CUpnpTcpSession;
class CUpnpHttpFileTransferBuffer;
class CUpnpHttpFileAccess;


/**
 *  CUpnpHttpFileTransferReader is responsible for asynchronous reading.
 *  Reader starta reading in asynchronous mode. Issues reading and then waits 
 *  for the operation to be completed. When it is completed then received
 *  data is forwarded to upper layer - CUpnpTcpSession and the new read 
 *  operation is issues. This continues in loop until the Cancel() is invoked
 *  or object is destroyed.
 *  @lib DLNAWebServer.lib
 *  @since Series60 3.2
 */
NONSHARABLE_CLASS (CUpnpHttpFileTransferReader) : public CActive, MUpnpNotifyTimerObserver
    {

public:
    
    /**
    * CUpnpHttpFileTransferReader factory method.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @param aReadPortion - size of the buffer to read from the socket
    * @param aBufferSize - the buffer size
    * @return new CUpnpHttpFileTransferReader object's instance.
    */
    static CUpnpHttpFileTransferReader* NewL( CUpnpTcpSession& aSession,
                                        RSocket& aSocket,
                                        TThreadPriority aPriority,                                        
                                        TInt aReadPortion,
                                        TInt aBufferSize);
    
    /**
    * Virtual destructor.
    * @since Series60 3.2
    */                             
    virtual ~CUpnpHttpFileTransferReader();
    
    /**
    * Appends first portion of data to the buffer.
    * @since Series60 3.2
    */    
    void AppendL(const TDesC8& aBeginning);
    
    /**
    * Calculates the buffer size as exponent of 2
    * and proper for read portions
    * @since Series60 3.2
    */    
    static TInt CalculateBufferSize(TInt aReadPortion, TInt aBufferSize);            
    /**
    * Starts reading in asynchronous mode.
    * @since Series60 3.2
    */
    virtual void StartL();

    /**
    * Cancel the timers
    * @since Series60 3.2
    */
    void CancelTimers();

    /**
    * Resets some member values to start reading data
    * @since Series60 3.2
    */
    
    virtual void Reset();
    
    /**
    * Checks if the reader is started meaning in use
    * @since Series60 3.2
    * @return TBool
    */
    TBool Activated();
     
    /**
    * Sets the state of the reader to activated
    * @since Series60 3.2    
    * @param state - ETrue/EFalse
    */
    void SetActivated(TBool aValue);

  

protected: // Functions from CActive
   
    /**
    * Cancels issued reading request.
    * Standard active object function
    * @since Series60 3.2
    */
    void DoCancel();
    
     /**
    * Function called when the read request is finished.
    * Standard active object function
    * @since Series60 3.2
    */
    virtual void RunL();
    
    /**
    * Trap RunL leaves
    * Standard active object function
    * @since Series60 3.2
    */
    TInt RunError( TInt aError );

	// Constructors and destructors

    /**
    * CUpnpHttpFileTransferReader constructor.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @param aReadPortion - size of the buffer to read from the socket
    * @param aBufferSize - the buffer size
    */
    CUpnpHttpFileTransferReader( CUpnpTcpSession& aSession,
                           RSocket& aSocket,
                           TThreadPriority aPriority,
                           TInt aReadPortion,
                           TInt aBufferSize);
    

        

protected: // New functions

    /**
    * Issues reading in asynchronous mode.
    * @since Series60 3.2
    */
    void IssueRead();
    
    /**
    * Callback function for timer expirations.
    * @since Series60 2.6
    * @param aTimer Timer that has expired.
    **/
	void TimerEventL( CUpnpNotifyTimer* /*aTimer*/ );

    /**
    * Starts the cancel timer.
    * @since Series60 3.2
    */
    void StartCancelTimer();

	/**
    * Base constructor to be used also in derived classes.
    * @since Series60 3.2
    */
    void BaseConstructL();
    
    /**
    * Initiates the buffer
    * @since Series60 3.2      
    */
    void InitiateBufferL();
	
	/**
    * Remaing number of bytes to read.
    * @since Series60 3.2    
    * @return TInt
    */
    virtual TInt ReadBufferSize();   
    
    /**
    * Handles buffer of data
    * @since Series60 3.2    
    * @return TBool
    */
    virtual TBool HandleL();   
    
    /**
    * Handles leave from RunL
    * @since Series60 3.2    
    * @param aError
    */
    void HandleError();   
    
    /**
    * Finish the session
    * @since Series60 3.2
    */
    virtual void Finish();  
    
    
    /**
    * Reads from socket
    * @since Series60 3.2    
    */
    virtual void ReadFromSocket();
    
    /**
    * Remaing number of bytes to read.
    * @since Series60 3.2    
    * @return TInt
    */
    virtual TInt RemainingBytes();
    
    /**
    * Returns bytes to be read unless FIN received, then returns 0
    * @since Series60 3.2    
    * @return TInt
    */
    TInt TcpFinFoundRemainingBytes();
    
    /**
    * Returns how many bytes will still fit into the buffer
    * @since Series60 3.2    
    * @return TInt
    */
    TInt AvailableSpace();
    
    /**
    * If content length is less than single buffer size
    * transaction is completed and session is closed
    * @since Series60 3.2    
    * @return TInt
    */    
    void HandleOneBufferShortTransferL();
    
     
    
protected: // Data
      
    //Socket from which the data will be read, not owned.    
    RSocket& iSocket;   
    
    // Reference to session that owns this reader.   
    CUpnpTcpSession& iSession;
    
    //Pointer that points to receiving buffer.
    TPtr8 iReceivePtr;       
    
    // Buffer that is used to receive data to session.   
    RBuf8 iCacheBuffer;
        	
    // Read buffer size
    TInt iReadBufferSize;

    //size of buffered data
    TInt iBufferSize;  	

    //processing has finished
    TInt iIsFinished;

    //whether upload reader is activated
    TBool iIsActivated;

    /**
    * Timer used for retry process.
    */
    CUpnpNotifyTimer *iRetryTimer;

    /**
    * Timer used for cancel reciving process.
    */
    CUpnpNotifyTimer *iCancelTimer;

    /**
    * Variable indicating when the cancel was called from cancel timer.
    */
    TBool iCancelFromTimer;

    /**
    * Count of reading errors.
    */
    TInt iRetryErrorCount;

    //length of data read from socket
    TSockXfrLength iLen;

    };


#endif // C_CUPNPHTTPFILETRANSFERREADER_H

// End Of File
