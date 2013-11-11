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
* Description:  
*
*/


#ifndef C_CUPNPRETRYWRITE_H
#define C_CUPNPRETRYWRITE_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>


static const TInt KWriteErrorsMax = 5;
static const TInt KWaitWithSocketWrite = 1000000;


// CLASS DECLARATION
/**
* An interface for retry write
*
*  @since Series60 3.1
*/
class MUpnpRetryWriteObserver
{
	public:

        /**
        * RetryWriteSucceed
        * A callback function for succeed writing.
        * @since Series60 3.1
        */
		virtual void RetryWriteSucceed() = 0;

        /**
        * RetryWriteFailL
        * A callback function for failed writing.
        * @since Series60 3.1
		* @param aError Error code
        */
		virtual void RetryWriteFailL( TInt aError ) = 0;
};


NONSHARABLE_CLASS (CUpnpRetryWrite) : CActive
{
	public:
		
	    /**
	    * Constructor function.
	    * @since Series60 3.1
	    * @return A new CUpnpRetryWrite instance.
	    **/
		static CUpnpRetryWrite* NewL( CUpnpTcpSession&, RSocket&, MUpnpRetryWriteObserver*, TThreadPriority );

        /**
        * Destructor.
        */
		~CUpnpRetryWrite();

        /**
        * IssueWriteRetry
        * Starts retry procedure
        * @since Series60 3.1
        */
		void IssueWriteRetry();
		
        /**
        * IsStarted
        * Checks if the retrying procedure is started
        * @since Series60 3.1
        * @return ETrue when retrying procedure is started, else return EFalse
        */
		TBool IsStarted();
		
	private:
        /**
        * Constructor
        */
		CUpnpRetryWrite(CUpnpTcpSession&, RSocket&, MUpnpRetryWriteObserver*, TThreadPriority );

        /**
        * ConstructL
        */
		void ConstructL();
	
        /**
        * WriteToSocket
        * Starts writing null descriptor to the socket
        * @since Series60 3.1
        */
		void WriteToSocket();

        /**
        * PassErrorToObserverL
        * Retruns the error code to the observer
        * @since Series60 3.1
		* @param aError Returned error code
        */
		void PassErrorToObserverL( TInt aError );
	
        /**
        * RunL
        * Active object state machine.
        * @since Series60 2.0
        */
		void RunL();

        /**
        * DoCancel
        * Cancel outstanding request(s) and reset the iActive flag.
        * @since Series60 2.0
        */
		void DoCancel();

        /**
        * RunError
        * RunError in case RunL leaves.
        * @since Series60 2.0
        */
		TInt RunError( TInt aError );
			
	
        /**
        * TInternalState
        * Internal states.
        * @since Series60 3.1
        */
    enum TInternalState
        {
        EUnknown,
        EWaiting,
        EWriting
        };
	
	private:

	    /**
	    * Reference to session that owns the writer.
	    * Session is request writing and is notified when it's finished or
	    * errors occure, not owned.
	    */
		CUpnpTcpSession& iSession;

	    /**
	    * Socket used for writing data, not owned.
	    */
		RSocket& iSocket;
	
	    /**
	    * Interface observer object.
	    */
		MUpnpRetryWriteObserver* iObserver;

	    /**
	    * Internal state variable.
	    */
		TInternalState iInternalState;
	
	    /**
	    * Internal timer.
	    */
		RTimer iWriteTimer;

	    /**
	    * Errors coumter.
	    */
		TInt iWriteErrorsCount;
		
};



#endif

