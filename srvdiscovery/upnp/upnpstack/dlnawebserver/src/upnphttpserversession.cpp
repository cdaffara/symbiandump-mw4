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
* Description:  CUpnpHttpServerSession is class that is used to handle single 
*                 connections to HttpServer
*
*/

#include "upnphttpserversession.h"
#include "upnphttpserverruntime.h"
#include "upnphttpserver.h"

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::CUpnpHttpServerSession
// -----------------------------------------------------------------------------
//
CUpnpHttpServerSession::CUpnpHttpServerSession()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::~CUpnpHttpServerSession
// -----------------------------------------------------------------------------
//
CUpnpHttpServerSession::~CUpnpHttpServerSession()
    {
    delete iServerRuntime;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::NewLC
// -----------------------------------------------------------------------------
//
CUpnpHttpServerSession* CUpnpHttpServerSession::NewLC(
    TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver )
    {
    CUpnpHttpServerSession* self = new (ELeave) CUpnpHttpServerSession();
    CleanupStack::PushL( self );
    self->ConstructL( aUnusedIapId, aObserver );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpServerSession* CUpnpHttpServerSession::NewL(
    TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver )
    {
    CUpnpHttpServerSession* self = CUpnpHttpServerSession::NewLC( aUnusedIapId, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerSession::ConstructL( 
    TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver )
    {
    iServerRuntime = CUpnpHttpServerRuntime::NewL( aUnusedIapId, aObserver );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::StartL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServerSession::StartL( const TInt aPort )
    {    
    iServerRuntime->StartServerL( aPort ) ;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServerSession::Stop()
    {
    iServerRuntime->DeleteServer();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::GetAddress
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServerSession::GetAddress( TInetAddr& aAddr )
    {    
    iServerRuntime->HttpServer().ServerAddress( aAddr );
    aAddr.SetPort( iServerRuntime->HttpServer().ServerPort() );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::DefaultRuntime
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpServerRuntime& CUpnpHttpServerSession::DefaultRuntime()
    {
    return *iServerRuntime;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::SendMessageL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpHttpServerSession::SendMessageL( CUpnpHttpMessage* aMessage )
    {
    return iServerRuntime->HttpServer().SendMessageL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerSession::HWAddressL
// -----------------------------------------------------------------------------
//    
EXPORT_C TSockAddr* CUpnpHttpServerSession::HWAddressL()
    {
    return iServerRuntime->HttpServer().HWAddressL();
    }

