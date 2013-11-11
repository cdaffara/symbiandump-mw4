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
* Description:  A class that holds the shared connection to a access point.
*
*/


#ifndef SESSION_HTTP_CONNECTION_H
#define SESSION_HTTP_CONNECTION_H


// INCLUDES
#include <es_sock.h>
//#include <AknInputBlock.h>

#include "HttpConnection.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFeedsServerSession;

// CLASS DECLARATION


/**
*  A class that holds the shared connection with client app.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CSessionHttpConnection: public CHttpConnection//, public MAknInputBlockCancelHandler
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CSessionHttpConnection* NewL( CFeedsServerSession& aFeedsSession );
        
        /**
        * Destructor.
        */        
        virtual ~CSessionHttpConnection();
        
        
    public: // From MConnectionCallback        
        /**
        * It propagates the connection callback to the browser client.
        *
        * @since 3.0
        * @param aConnectionPtr Connection ptr
        * @param aSockSvrHandle Socket server handle
        * @param aNewConn ETrue, if this is a new connection
        * @param aBearerType Bearer type
        * @return OS wide error code
        */
        virtual TInt CreateConnection(TInt* aConnectionPtr, TInt* aSockSvrHandle, 
                TBool* aNewConn, TApBearerType* aBearerType);
        
        
    public:  // From CHttpConnection.
        /**
        * Returns whether or not the connection is active.
        *
        * @since 3.0
        * @return void.
        */
        TBool IsConnected();

        /**
        * Closes the connection.
        *
        * @since 3.0
        * @return void.
        */
        void Disconnect();

        /**
        *
        */
        void CancelAnyConnectionAttempt();

    public:  // New Methods.
        /**
        * Set the connection.
        *
        * @since 3.1
        * @return ETrue if connection being set, EFalse otherwise.
        */
        TBool SetConnection( TDesC& aName, TInt aBearerType );


    private:
        /**
        * C++ default constructor.
        */
        CSessionHttpConnection( CFeedsServerSession& aFeedsSession );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Reset connection name
        */
        void Reset();

        /**
        * Have we received valid connection information from the client?
        */
        TBool ConnectionInformationProvidedByClient() const;
        
    private:
        TLeakTracker                 iLeakTracker;
        
        CFeedsServerSession&    iFeedsSession; 

        RSocketServ             iSocketServ;
        RConnection             iConnection;   ///< got from client apps, no need to disconnect upon session closing
        HBufC*                  iConnName;
        TApBearerType           iBearerType;
        TInt                    iConnStage;
        TInt                    iConnectionError;

        CActiveSchedulerWait    iWait;
    };

#endif      // SESSION_HTTP_CONNECTION_H
            
// End of File
