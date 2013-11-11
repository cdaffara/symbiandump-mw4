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


#ifndef SERVER_HTTP_CONNECTION_H
#define SERVER_HTTP_CONNECTION_H


// INCLUDES
#include "HttpConnection.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CInternetConnectionManager;

// CLASS DECLARATION


/**
*  A class that holds the shared connection to a access point.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CServerHttpConnection: public CHttpConnection
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CServerHttpConnection* NewL( TUint32 iDefaultAccessPoint );
        
        /**
        * Destructor.
        */        
        virtual ~CServerHttpConnection();
        
        
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
        
        
    public:  // from CHttpConnection
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


    public:  // New methods.
        /**
        * Sets the new access point.
        *
        * @since 3.0
        * @param aAccessPoint The access point.
        * @return Void
        */
        void SetAccessPointL( TUint32 aAccessPoint );


    private:
        /**
        * C++ default constructor.
        */
        CServerHttpConnection( TUint32 iDefaultAccessPoint );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:
        TLeakTracker                 iLeakTracker;
        
        CInternetConnectionManager*  iConMgr;
        TUint32                      iDefaultAccessPoint;
    };

#endif      // SERVER_HTTP_CONNECTION_H
            
// End of File