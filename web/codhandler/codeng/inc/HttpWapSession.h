/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CHttpWapSession.
*      
*
*/


#ifndef HTTP_WAP_SESSION_H
#define HTTP_WAP_SESSION_H

// INCLUDES

#include <e32base.h>
#include <http/mhttpsessioneventcallback.h>
#include "HttpSessionBase.h"

// CLASS DECLARATION

/**
* WAP Session.
*/
NONSHARABLE_CLASS( CHttpWapSession )
: public CHttpSessionBase,
  public MHTTPSessionEventCallback

    {
    public:     // Constructors and destructor.

        /**
        * Two phased constructor. Leaves on failure.
        * @param aGateway WAP gateway.
        * @return The created session.
        */      
        static CHttpWapSession* NewL( const TDesC8& aGateway );
        
        /**
        * Destructor.
        */
        virtual ~CHttpWapSession();

    public:     // from CHttpSessionBase

        /**
        * Connect session.
        * @param aStatus Completes when done.
        */
        virtual void ConnectL( TRequestStatus* aStatus );

        /**
        * Disconnect session. Safe to call if not connected. Use this to
        * cancel an outstanding ConnectL.
        */
        virtual void Disconnect();

    protected:    // Constructors and destructor.

        /**
        * Constructor.
        */
        CHttpWapSession();

        /**
        * Second phase constructor. Leaves on failure.
        * @param aGateway WAP gateway.
        */      
        void ConstructL( const TDesC8& aGateway );
        

        /**
        * Handle session event.
        * @param aEvent The event that has occurred.
        */
        void MHFSessionRunL( const THTTPSessionEvent& aEvent );

        /**
        * Handle errors occured in MHFSessionRunL().
        * @param aError The leave code that MHFSessionRunL left with.
        * @param aEvent The Event that was being processed.
        * @return KErrNone.
        */
        TInt MHFSessionRunError
            ( TInt aError, const THTTPSessionEvent& aEvent );

    private:    // new methods

        /**
        * Clean up and notify parent.
        * @param aResult Result of the operation.
        */
        void Done( TInt aResult );

        /**
        * Get a shared string from the session stringpool.
        * @param aId Id of string.
        * @return Shared string from HTTP string table. No need to close.
        */
        inline RStringF StringF( HTTP::TStrings aId );

    private:    // types

        enum TState         ///< State.
            {
            EInit,          ///< Initial state (session is not open).
            EConnecting,    ///< Connecting.
            EConnected,     ///< Session is open and connected.
            EDisconnecting  ///< Disconnecting.
            };

    private:    // data

        TState iState;                  ///< State.
        TRequestStatus* iParentStatus;  ///< Parent status.
        RStringF iGateway;              ///< Gateway. Owned.
        TBool iGatewayOpen;             ///< ETrue if gateway string is open.
        CActiveSchedulerWait* iWait;    ///< Wait object (for disconnect).

    };

#include "HttpWapSession.inl"

#endif /* def HTTP_WAP_SESSION_H */
