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
*      Implementation of class CHttpWapSession.   
*      
*
*/


// INCLUDE FILES

#include <httpfiltercommonstringsext.h>
#include "HttpWapSession.h"
#include "CodError.h"
#include "CodLogger.h"
#include "CodPanic.h"

/// WSP protocol name.
_LIT8( KCodWspProtocol, "WSP/WSP" );
/// Default WAP server port.
const TInt KCodWspRemotePort = 9201;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpWapSession::NewL()
// ---------------------------------------------------------
//
CHttpWapSession* CHttpWapSession::NewL( const TDesC8& aGateway )
    {
    CHttpWapSession* sess = new( ELeave ) CHttpWapSession();
    CleanupStack::PushL( sess );
    sess->ConstructL( aGateway );
    CleanupStack::Pop( sess );
    return sess;
    }

// ---------------------------------------------------------
// CHttpWapSession::~CHttpWapSession()
// ---------------------------------------------------------
//
CHttpWapSession::~CHttpWapSession()
    {
    CLOG(( EWapConn, 2, _L("-> CHttpWapSession::~CHttpWapSession") ));
    Disconnect();
    if ( iGatewayOpen )
        {
        iGateway.Close();
        }
    iSess.Close();
    delete iWait;
    CLOG(( EWapConn, 2, _L("<- CHttpWapSession::~CHttpWapSession") ));
    }

// ---------------------------------------------------------
// CHttpWapSession::ConnectL()
// ---------------------------------------------------------
//
void CHttpWapSession::ConnectL( TRequestStatus* aStatus )
    {
    CLOG(( EWapConn, 2, _L("-> CHttpWapSession::ConnectL") ));
    CLOG(( EWapConn, 3, _L8("  iGateway <%S> port(%d)"),\
        &(iGateway.DesC()), KCodWspRemotePort ));

    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EInit, CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );

    RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
    connInfo.SetPropertyL
        ( StringF( HTTP::EWspProxyAddress ), THTTPHdrVal( iGateway ) );
    connInfo.SetPropertyL
        ( StringF( HTTP::EWspRemotePort ), THTTPHdrVal( KCodWspRemotePort ) );
    // Connect to WAP gateway.
    iSess.SetSessionEventCallback( this );
    iSess.ConnectL();

    iState = EConnecting;

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    CLOG(( EWapConn, 2, _L("<- CHttpWapSession::ConnectL") ));
    }

// ---------------------------------------------------------
// CHttpWapSession::Disconnect()
// ---------------------------------------------------------
//
void CHttpWapSession::Disconnect()
    {
    CLOG(( EWapConn, 2, _L("-> CHttpWapSession::Disconnect iState(%d)"), \
        iState ));
    switch ( iState )
        {
        case EConnecting:
        case EConnected:
            {
            TRAPD( err, iSess.DisconnectL() );
            if ( err )
                {
                // Nothing we can do about this failure!
                CLOG(( EConn, 3, _L("  failed(%d)"), err ));
                }
            else
                {
                iState = EDisconnecting;
                CLOG(( EConn, 3, _L("  starting wait") ));
                iWait->Start();
                }
            break;
            }

        case EInit:
        case EDisconnecting:
        default:
            {
            CLOG(( EConn, 3, _L("  nothing to do") ));
            // EInit state: no need to do anything.
            // EDisconnecting: we can't do anything.
            break;
            }
        }
    CLOG(( EWapConn, 2, _L("<- CHttpWapSession::Disconnect") ));
    }

// ---------------------------------------------------------
// CHttpWapSession::CHttpWapSession()
// ---------------------------------------------------------
//
CHttpWapSession::CHttpWapSession()
: CHttpSessionBase(),
  iState( EInit ),
  iGatewayOpen( EFalse )
    {
    CLOG(( EWapConn, 2, _L("CHttpWapSession::CHttpWapSession") ));
    }

// ---------------------------------------------------------
// CHttpWapSession::ConstructL()
// ---------------------------------------------------------
//
void CHttpWapSession::ConstructL( const TDesC8& aGateway ) 
    {
    __ASSERT_ALWAYS( aGateway.Length(), CodPanic(  ECodInvalidArguments ) );
    iSess.OpenL( KCodWspProtocol );
    iSess.StringPool().OpenL( HttpFilterCommonStringsExt::GetTable() );
    iGateway = iSess.StringPool().OpenFStringL( aGateway );
    // Track whether open - cannot close unopened RStringF.
    iGatewayOpen = ETrue;
    iWait = new (ELeave) CActiveSchedulerWait;
    }

// ---------------------------------------------------------
// CHttpWapSession::MHFSessionRunL()
// ---------------------------------------------------------
//
void CHttpWapSession::MHFSessionRunL( const THTTPSessionEvent& aEvent )
    {
    CLOG(( EWapConn, 0, _L("-> CHttpWapSession::MHFSessionRunL event(%d)"), \
                                                            aEvent.iStatus ));

    switch ( aEvent.iStatus )
        {
        case THTTPSessionEvent::EConnectedOK:
        case THTTPSessionEvent::EConnectedWithReducedCapabilities:
            {
            CLOG(( EWapConn, 3, _L("  EConnected...") ));
            if ( iState == EConnecting )
                {
                // Connecting: we are done.
                iState = EConnected;
                Done( KErrNone );
                }
            else if ( iState == EDisconnecting )
                {
                // Connecting was interrupted with a Disconnect call but it
                // seems we have already connected by that time. Now we are
                // not Done() yet: wait more for the disconnect to finish.
                CLOG(( EWapConn, 3, \
                    _L("  late connect event, keep waiting for disconnect") ));
                iState = EConnected;
                }
            else
                {
                // Unexpected state for these events.
                CLOG(( EWapConn, 0, \
                    _L("  connect event in unexpected state") ));
                CodPanic( ECodOffState );
                }
            break;
            }

        case THTTPSessionEvent::EDisconnected:
            {
            CLOG(( EWapConn, 3, _L("  EDisconnected") ));
            iState = EInit;
            Done( KErrCodWapConnectionDropped );
            break;
            }

        case THTTPSessionEvent::EConnectionTimedOut:
            {
            CLOG(( EWapConn, 3, _L("  EConnectionTimedOut") ));
            iState = EInit;
            Done( KErrCodHttpTimedOut );
            break;
            }

        default:
            {
            if( aEvent.iStatus < 0 )
                {
                // Negative value indicates error.
                iState = EInit;
                Done( KErrCodWapConnectionDropped );
                }
            break;
            }
        }
    CLOG(( EWapConn, 0, _L("<- CHttpWapSession::MHFSessionRunL") ));
    }

// ---------------------------------------------------------
// CHttpWapSession::MHFSessionRunError()
// ---------------------------------------------------------
//
TInt CHttpWapSession::MHFSessionRunError
( TInt LOG_ONLY( aError ), const THTTPSessionEvent& /*aEvent*/ )
    {
    CLOG(( EWapConn, 2, \
        _L("CHttpWapSession::MHFSessionRunError (%d)"), aError ));
    // This method should never be called - MHFSessionRunL does not leave.
    CodPanic( ECodInternal );
    return KErrNone;
    }

// ---------------------------------------------------------
// CHttpWapSession::Done()
// ---------------------------------------------------------
//
void CHttpWapSession::Done( TInt aResult )
    {
    CLOG(( EConn, 2, _L("CHttpWapSession::Done aResult(%d)"), aResult ));
    if ( iWait->IsStarted() )
        {
        // This must be Disconnect: we are waiting for it.
        CLOG(( EConn, 3, _L("  stopping wait") ));
        iWait->AsyncStop();
        }
    if ( iParentStatus )
        {
        // This must be ConnectL: parent is waiting for it.
        CLOG(( EConn, 3, _L("  completing parent") ));
        User::RequestComplete( iParentStatus, aResult );
        iParentStatus = NULL;
        }
    }
