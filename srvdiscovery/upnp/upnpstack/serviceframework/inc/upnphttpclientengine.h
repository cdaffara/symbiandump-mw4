/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the CUpnpHttpClientEngine class
*
*/


#ifndef __UPNPHTTPCLIENTENGINE_H__
#define __UPNPHTTPCLIENTENGINE_H__

// INCLUDES
#include <http/rhttpsession.h>
#include <es_sock.h>
#include "upnphttpmessagesenderowner.h"
#include "upnpconnectionmanagerproxy.h"

// FORWARD DECLARATIONS
class CUpnpHttpTransaction;
class CUpnpHttpMessageSender;

// CLASS DECLARATION

/**
* MUpnpHttpClientObserver
* CUpnpHttpClientEngine passes end of transaction event with this interface.
* An instance of this class must be provided for construction of CUpnpHttpClientEngine.
*/
class MUpnpHttpClientObserver
    {
    public:
        virtual void ClientResponseRecivedLD( CUpnpHttpTransaction& aEndedTransaction ) = 0;
    };

/**
* CUpnpHTTPClientEngine
* Provides simple interface to HTTP Client API.
*/
NONSHARABLE_CLASS( CUpnpHttpClientEngine ): public CBase, public MUpnpHttpMessageSenderOwner
    {
    public:
        /**
        * Create a CUpnpHttpClientEngine object.
        * @param  iObserver:
        * @return A pointer to the created instance of CWebClientEngine
        */
        IMPORT_C static CUpnpHttpClientEngine* NewL(
            MUpnpHttpClientObserver& aObserver, TInt aActiveIap );

        /**
        * Create a CUpnpHttpClientEngine object and leave it on cleanup stack.
        * @param  iObserver:
        * @return A pointer to the created instance of CWebClientEngine
        */
        IMPORT_C static CUpnpHttpClientEngine* NewLC(
            MUpnpHttpClientObserver& aObserver, TInt aActiveIap );

        /**
        * Destroy the object
        */
        IMPORT_C ~CUpnpHttpClientEngine();

        IMPORT_C void SendL( CUpnpHttpTransaction& aTransaction );
        
        /*IMPORT_C*/ void Cancel( CUpnpHttpTransaction& aTransaction );
private:
        TInt Find( CUpnpHttpTransaction& aTransaction );


private://from MUpnpHttpMessageSenderOwner

        /*
         * Destroyes sender and removes it from senders list.
         */
        virtual void SenderFinishedLD( CUpnpHttpMessageSender* aSenderToRelease );

private:

        /**
        * Perform the second phase construction of a CWebClientEngine object.
        */
        void ConstructL( TInt aActiveIap );

        /**
        * Perform the first phase of two phase construction.
        * @param iObserver:
        */
        CUpnpHttpClientEngine( MUpnpHttpClientObserver& iObserver );

    private: 
        RHTTPSession            	iSession;
        MUpnpHttpClientObserver&    iObserver;

        RSocketServ iSocketServ;
        CUpnpConnectionManagerProxy* iConnectionManagerProxy;

        RPointerArray<CUpnpHttpMessageSender> iSenders;
    };

#endif // __UPNPHTTPCLIENTENGINE_H__

//  End of File

