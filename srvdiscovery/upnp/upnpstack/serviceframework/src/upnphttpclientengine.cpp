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
* Description:  Definition of the CUpnpHttpClientEngine class
*
*/


// INCLUDE FILES
#include <http.h>
#include <commdbconnpref.h>
#include <upnphttpmessage.h>

#include "upnphttpclientengine.h"
#include "upnphttpmessagesender.h"
#include "upnphttptransaction.h"

#ifdef _DEBUG
#define KLogFile _L("HttpClientEngine.txt")
#endif
#include "upnpcustomlog.h"

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::NewL
// Creates instance of CUpnpHTTPClientEngine.
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpClientEngine* CUpnpHttpClientEngine::NewL(
    MUpnpHttpClientObserver& aObserver, TInt aActiveIap )
    {
    CUpnpHttpClientEngine* self = CUpnpHttpClientEngine::NewLC(aObserver, aActiveIap );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::NewLC
// Creates instance of CUpnpHTTPClientEngine.
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpClientEngine* CUpnpHttpClientEngine::NewLC(
    MUpnpHttpClientObserver& aObserver, TInt aActiveIap )
    {
    CUpnpHttpClientEngine* self = new (ELeave) CUpnpHttpClientEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aActiveIap );
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::~CUpnpHTTPClientEngine
// Destructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpClientEngine::~CUpnpHttpClientEngine()
    {
    iSenders.ResetAndDestroy();
    iSession.Close();
    delete iConnectionManagerProxy;
    iSocketServ.Close();
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::SendL
// Sends request message, client can expect asynchronous response callbacks,
// and response body in transaction object.
// ----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpClientEngine::SendL( CUpnpHttpTransaction& aTransaction )
    {
    CUpnpHttpMessageSender* messageSender = CUpnpHttpMessageSender::NewLC(
        aTransaction, iSession, *this);
    iSenders.AppendL( messageSender );
    CleanupStack::Pop( messageSender );
    messageSender->StartTransactionL();
    LOGS1H( 0, "SendL ses id: %d", aTransaction.Request()->SessionId() );
    }

void CUpnpHttpClientEngine::Cancel( CUpnpHttpTransaction& aTransaction )
    {
    TInt pos( Find( aTransaction ) );
    if ( pos != KErrNotFound )
        {
        iSenders[ pos ]->CancelTransaction();
        LOGSH( 0, "Cancel ses" );
        }
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::ConstructL
// Second phase construction.Openes session and starts connection of active IAP.
// ----------------------------------------------------------------------------
//
void CUpnpHttpClientEngine::ConstructL( TInt aActiveIap )
    {
    iSession.OpenL();	// Open default protocol ("HTTP/TCP")

    TCommDbConnPref pref;
    pref.SetIapId( aActiveIap );
    pref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );

    User::LeaveIfError( iSocketServ.Connect() );
    iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( iSocketServ );
    User::LeaveIfError( iConnectionManagerProxy->EnsureStart() );

    RStringPool strP = iSession.StringPool();
    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();
    connInfo.SetPropertyL (
        strP.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable() ),
        THTTPHdrVal (iSocketServ.Handle())
        );
    TInt connPtr = reinterpret_cast<TInt>( &iConnectionManagerProxy->ConnectionL() );
    connInfo.SetPropertyL (
        strP.StringF(HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
        THTTPHdrVal (connPtr)
        );
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::CUpnpHTTPClientEngine
// First phase constructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpClientEngine::CUpnpHttpClientEngine( MUpnpHttpClientObserver& aObserver )
:   iObserver( aObserver )
    {
    }

// ----------------------------------------------------------------------------
// CUpnpHTTPClientEngine::SenderFinishedLD
// Callback method indicatind that sender finished its job.
// Methods notifies observer about response, and destroyes sender.
// ----------------------------------------------------------------------------
//
void CUpnpHttpClientEngine::SenderFinishedLD( CUpnpHttpMessageSender* aSenderToRelease )
    {
    TInt index = iSenders.Find( aSenderToRelease );
    ASSERT( KErrNotFound != index );
    LOGS1H( 0, "CUpnpHTTPClientEngine::RemoveSender: %d", index );
    iObserver.ClientResponseRecivedLD( aSenderToRelease->UpnpTransaction() );
    iSenders.Remove( index );
    delete aSenderToRelease;
    }

TInt CUpnpHttpClientEngine::Find( CUpnpHttpTransaction& aTransaction )
    {
    for ( TInt i(0) ; i < iSenders.Count() ; i++ )
        {
        if ( &aTransaction  == &( iSenders[i]->UpnpTransaction() ) )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

//end of file

