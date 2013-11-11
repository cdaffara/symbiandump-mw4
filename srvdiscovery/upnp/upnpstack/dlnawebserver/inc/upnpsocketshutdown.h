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


#ifndef C_CUPNPSOCKETSHUTDOWN_H
#define C_CUPNPSOCKETSHUTDOWN_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>

class CUpnpTcpSession;
class CUpnpNotifyTimer;


// CLASS DECLARATION


NONSHARABLE_CLASS (CUpnpSocketShutdown) : CActive, MUpnpNotifyTimerObserver
{
    public:
        
    /**
        * Constructor function.
        * @since Series60 3.1
        * @return A new CUpnpSocketShutdown instance.
        **/
    static CUpnpSocketShutdown* NewL( CUpnpTcpSession&, RSocket&, TThreadPriority );

    /**
        * Destructor.
        */
    ~CUpnpSocketShutdown();

    /**
        * HandleError
        * Error servicing
        * @since Series60 3.1
        * @param aError Error code
        */
    void HandleError( TInt aError );
        
    /**
        * ShutdownImmediate
        * Shutdown with EImmediate
        * @since Series60 3.1
        */
    void ShutdownImmediate();
    
    /**
        * ShutdownStop
        * Shutdown with ENormal or EStopOutput
        * @since Series60 3.1
        */
    void ShutdownStop();
        
    /**
        * MarksTimeout
        * Marks if timeout occured on session
        * @since Series60 3.2
        */
    void MarksTimeout();

    private:
    
    /**
        * Constructor
        */
    CUpnpSocketShutdown( CUpnpTcpSession&, RSocket&, TThreadPriority );

    /**
        * ConstructL
        */
    void ConstructL();
    
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
        * Callback function for timer expirations.
        * @since Series60 2.6
        * @param aTimer Timer that has expired.
        **/
    void TimerEventL( CUpnpNotifyTimer* aTimer );

    /**
        * TInternalState
        * Internal states.
        * @since Series60 3.1
        */
    enum TInternalState 
        {
        EUnknown,
        EShuttingDownStopOutput,
        EShuttingDownNormal,
        EWaitingForInputError,
        EInputErrorReceived,
        ENotConnected
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
        * Internal state variable.
        */
    TInternalState iInternalState;

    // Timeout timer for input errors after shutdown normal and stop input 
    CUpnpNotifyTimer* iInputTimeoutTimer;
        
    /**
        * Determines if timeout occured on session
        */
    TBool iSessionTimeouted;

};


#endif

