/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CUpnpHttpServerRuntime is a single runtime.
*
*/

#ifndef UPNPHTTPSERVERRUNTIME_H
#define UPNPHTTPSERVERRUNTIME_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MUpnpHttpServerTransactionCreator;
class CUpnpHttpServer;
class MUpnpHttpServerObserver;

// CLASS DECLARATION
/**
 * CUpnpHttpServerRuntime single runtime contains a set of n HTTP transactions.
 * It provides method for setting transaction creator.
 * 
 * @lib dlnawebserver.lib
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CUpnpHttpServerRuntime ) : public CBase
    {
public:

    /**
     * Sets transaction creator to http server.
     * 
     * @since S60 5.1
     * @param aCreator reference to transaction creator
     */
    IMPORT_C void SetCreator( MUpnpHttpServerTransactionCreator& aCreator );

    
    // For internal usage only    
    static CUpnpHttpServerRuntime* NewL( TUint aUnusedIapId, 
                                         MUpnpHttpServerObserver& aObserver );

    static CUpnpHttpServerRuntime* NewLC( TUint aUnusedIapId, 
                                          MUpnpHttpServerObserver& aObserver );
    ~CUpnpHttpServerRuntime();    
    CUpnpHttpServer& HttpServer();
    void DeleteServer();    
    void StartServerL( const TInt aPort );    
private:

    CUpnpHttpServerRuntime( TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver );
    void ConstructL();    
    void GetIapAndStartServerL();
    void OpenSocketServerL();        
    void CloseSocketServer();
    
private:    // data
    // Socket server.
    RSocketServ iSocketServ;        
    // Http server, owned
    CUpnpHttpServer* iHttpServer;    
    // Http server observer.
    MUpnpHttpServerObserver& iObserver;      
    // Pointer to transaction creator, not owned    
    MUpnpHttpServerTransactionCreator* iTransactionCreator;          
    // Determine if socket server is opened.
    TBool iIsSocketOpened;
    };

#endif // UPNPHTTPSERVERRUNTIME_H
