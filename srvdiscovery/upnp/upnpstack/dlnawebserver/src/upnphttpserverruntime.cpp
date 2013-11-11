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
* Description:  CUpnpHttpServerRuntime is a single runtime.
*
*/

#include "upnphttpserverruntime.h"
#include "upnpconnectionmanagerproxy.h"
#include "upnphttpserver.h"
#include "upnpsettings.h"

static const TUint KSocketServerConnectionSlots = 60;

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::CUpnpHttpServerRuntime
// -----------------------------------------------------------------------------
//
CUpnpHttpServerRuntime::CUpnpHttpServerRuntime( TUint /*aUnusedIapId*/,
                                                MUpnpHttpServerObserver& aObserver ):
    iObserver( aObserver )                                               
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::~CUpnpHttpServerRuntime
// -----------------------------------------------------------------------------
//
CUpnpHttpServerRuntime::~CUpnpHttpServerRuntime()
    {
    delete iHttpServer;
    CloseSocketServer();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::NewLC
// -----------------------------------------------------------------------------
//
CUpnpHttpServerRuntime* CUpnpHttpServerRuntime::NewLC( 
    TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver )
    {
    CUpnpHttpServerRuntime* self = new (ELeave) CUpnpHttpServerRuntime( aUnusedIapId, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::NewL
// -----------------------------------------------------------------------------
//
CUpnpHttpServerRuntime* CUpnpHttpServerRuntime::NewL( 
    TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver )
    {
    CUpnpHttpServerRuntime* self = CUpnpHttpServerRuntime::NewLC( aUnusedIapId, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerRuntime::ConstructL()
    {    
    // connecting to needed servers
    OpenSocketServerL();
    GetIapAndStartServerL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::GetIapAndStartServer
// -----------------------------------------------------------------------------
//    
void CUpnpHttpServerRuntime::GetIapAndStartServerL()
    {       
    // Gets IAP
    CUpnpConnectionManagerProxy *proxy = CUpnpConnectionManagerProxy::NewLC( iSocketServ );
    User::LeaveIfError( proxy->EnsureStart() );
    TInt iap = proxy->ActiveIap();    
    
    TInt bufferSize = CUpnpSettings::GetFileBufferSizeL();
    // creating Http server
    iHttpServer = CUpnpHttpServer::NewL( &iObserver, &iSocketServ, iap );
    iHttpServer->SetFileReadBufferSize( bufferSize );
    iHttpServer->SetFileWriteBufferSize( bufferSize );
    
    CleanupStack::PopAndDestroy( proxy );
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::SetCreator
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServerRuntime::SetCreator( MUpnpHttpServerTransactionCreator& aCreator )
    {
    iTransactionCreator = &aCreator;
    iHttpServer->SetTransactionCreator( &aCreator );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::HttpServer
// -----------------------------------------------------------------------------
//
CUpnpHttpServer& CUpnpHttpServerRuntime::HttpServer()
    {
    return *iHttpServer; 
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::DeleteServer
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerRuntime::DeleteServer()
    {
    if ( iHttpServer )
        {
        // server should be stopped to notify failed transfers
        iHttpServer->Stop();
        delete iHttpServer;
        iHttpServer = NULL;   
        CloseSocketServer();
        }    
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::StartServerL
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerRuntime::StartServerL(  const TInt aPort )
    {
    OpenSocketServerL();  
    if ( !iHttpServer )
        {
        GetIapAndStartServerL();
        }
    iHttpServer->SetTransactionCreator( iTransactionCreator );  
    iHttpServer->StartL( aPort );
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::OpenSocketServerL
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerRuntime::OpenSocketServerL()
    {
    if ( !iIsSocketOpened )
        {
        User::LeaveIfError( iSocketServ.Connect( KSocketServerConnectionSlots ) );
        iIsSocketOpened = ETrue;
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpServerRuntime::CloseSocketServer
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerRuntime::CloseSocketServer()
    {
    iSocketServ.Close();
    iIsSocketOpened = EFalse;
    }

