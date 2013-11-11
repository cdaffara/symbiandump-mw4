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
* Description:  CUpnpTcpSessionReader is a class responsible for 
*                   asynchronous reading from socket owned by CUpnpTcpSession.
*
*/



#ifndef C_CUPNPTCPSESSIONREADER_H
#define C_CUPNPTCPSESSIONREADER_H


#include <e32base.h>
#include <in_sock.h>
#include "upnpnotifytimer.h"

class CUpnpTcpSession;

/**
 *  CUpnpTcpSessionReader is responsible for asynchronous reading.
 *  Reader starta reading in asynchronous mode. Issues reading and then waits 
 *  for the operation to be completed. When it is completed then received
 *  data is forwarded to upper layer - CUpnpTcpSession and the new read 
 *  operation is issues. This continues in loop until the Cancel() is invoked
 *  or object is destroyed.
 *  @lib DLNAWebServer.lib
 *  @since Series60 2.6
 */
NONSHARABLE_CLASS (CUpnpTcpSessionReader) : public CActive, MUpnpNotifyTimerObserver
    {

public: // Constructors and destructors
    
    /**
    * CUpnpTcpSessionReader factory method.
    * @since Series60 2.6
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @return new CUpnpTcpSessionReader object's instance.
    */
    static CUpnpTcpSessionReader* NewL( CUpnpTcpSession& aSession,
                                        RSocket& aSocket,
                                        TThreadPriority aPriority,
                                        TInt aBufferSize );
    
    /**
    * Virtual destructor.
    * @since Series60 2.6
    */                             
    virtual ~CUpnpTcpSessionReader();
    
public: // New functions
    
    /**
    * Starts reading in asynchronous mode.
    * @since Series60 2.6
    */
    void Start();
  
    /**
    * Cancel the timers
    * @since Series60 3.2
    */
    void CancelTimers();

protected: // Functions from CActive
   
    /**
    * Cancels issued reading request.
    * Standard active object function
    * @since Series60 2.6
    */
    void DoCancel();
    
     /**
    * Function called when the read request is finished.
    * Standard active object function
    * @since Series60 2.6
    */
    void RunL();
    
    /**
    * Trap RunL leaves
    * Standard active object function
    * @since Series60 2.0
    */
    TInt RunError( TInt aError );

private: // Constructors and destructors

    /**
    * CUpnpTcpSessionReader constructor.
    * @since Series60 2.6
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    */
    CUpnpTcpSessionReader( CUpnpTcpSession& aSession,
                           RSocket& aSocket,
                           TThreadPriority aPriority,
                           TInt aBufferSize  );
    
    /**
    * By default Symbian 2nd phase constructor is private.
    * @since Series60 2.6
    */
    void ConstructL();

private: // New functions

    /**
    * Issues reading in asynchronous mode.
    * @since Series60 2.6
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
    
private: 
    
    /**
    * Socket from which the data will be read, not owned.
    */
    RSocket& iSocket;

    /**
    * Reference to session that owns this reader.
    * Session is notified when reader read some data and when 
    * errors occure, not owned.
    */
    CUpnpTcpSession& iSession;

    /**
    * Buffer that is used to receive data to session.
    * Own.
    */
    RBuf8 iReceiveBuffer;

    /**
    * Length of data that has been received.
    */
    TSockXfrLength iLen;
    	
  	// Read buffer size
  	TInt iBufferSize;
    
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

    };


#endif // C_CUPNPTCPSESSIONREADER_H

// End Of File
