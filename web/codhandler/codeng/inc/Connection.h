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
*      Declaration of class CConnection.   
*      
*
*/


#ifndef CONNECTION_H
#define CONNECTION_H

// INCLUDES

#include <e32base.h>
#include <es_sock.h>
#include <commdbconnpref.h>

// CLASS DECLARATION

/**
* Connection Active Object.
*/
NONSHARABLE_CLASS( CConnection ) : public CActive
    {
    public:     // Constructors and destructor.

        /**
        * Two phased constructor. Leaves on failure.
        * @return The created connection factory.
        */      
        static CConnection* NewL();

        /**
        * Destructor.
        */
        ~CConnection();

    public:     // new methods

        /**
        * Connect to network, create new connection if required.
        * @param aIap IAP.
        * @param aStatus Completes when done.
        */
        void ConnectL( TUint32 aIap, TRequestStatus* aStatus );

        /**
        * Connect to network, attach-only.
        * @param aIap IAP.
        */
        void AttachL( TUint32 aIap );

        /**
        * Close connection. If a request is outstanding, it is cancelled.
        */
        void Close();

        /**
        * Check if connected.
        * @param aIap IAP of active connection returned here.
        * @return ETrue if connected, EFalse otherwise.
        */
        TBool IsConnected( TUint32& aIap );

        /**
        * Get socket server handle.
        * @return Socket server handle.
        */
        inline RSocketServ& SockServ();

        /**
        * Get connection handle.
        * @return Connection handle.
        */
        inline RConnection& Conn();

    private:    // types

        enum TState         ///< State.
            {
            EInit,          ///< Initial state (idle).
            EConnecting,    ///< Connecting in progress.
            EConnected      ///< Connected.
            };

    private:    // Constructors and destructor.

        /**
        * Constructor.
        */
        CConnection();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:  // from CActive

        /**
        * Cancel protocol implementation.
        */
        virtual void DoCancel();

        /**
        * Outstanding request completed.
        */
        virtual void RunL();

        /**
        * Handle error.
        * @param aError Error code.
        * @return KErrNone.
        */
        virtual TInt RunError( TInt aError );

    private:    // new methods

        /**
        * Implementation of close.
        */
        void DoClose();

        /**
        * Done.
        */
        void Done();

    private:    // data

        RSocketServ iSockServ;          ///< Socket Server session handle.
        RConnection iConn;              ///< Connection handle.
        TState iState;                  ///< State.
        TRequestStatus* iParentStatus;  ///< Parent status.
        TCommDbConnPref iConnPref;      ///< Connection preferences.

    };

#include "Connection.inl"

#endif /* def CONNECTION_H */
