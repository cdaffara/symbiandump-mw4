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
*      Implementation of class CConnection.   
*      
*
*/


// INCLUDE FILES

#include "Connection.h"
#include "CodPanic.h"
#include "CodUtil.h"
#include "CodError.h"
#include "CodLogger.h"
#include <commdbconnpref.h>
#include <cdblen.h>
#include <es_enum.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnection::NewL()
// ---------------------------------------------------------
//
CConnection* CConnection::NewL()
    {
    CConnection* conn = new (ELeave) CConnection();
    CleanupStack::PushL( conn );
    conn->ConstructL();
    CleanupStack::Pop( conn );
    return conn;
    }

// ---------------------------------------------------------
// CConnection::~CConnection()
// ---------------------------------------------------------
//
CConnection::~CConnection()
    {
    Cancel();
    DoClose();
    }

// ---------------------------------------------------------
// CConnection::ConnectL()
// ---------------------------------------------------------
//
void CConnection::ConnectL( TUint32 aIap, TRequestStatus* aStatus )
    {
    CLOG(( EConn, 2, _L("-> CConnection::ConnectL iap(%d)"), aIap ));
    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );

    iStatus = KErrCodCannotConnect;
    if ( iState == EConnected )
        {
        // Already connected; do nothing.
        CLOG(( EConn, 4, _L("  already connected") ));
        iStatus = KErrNone;
        }
    else if ( iState == EInit )
        {
        // Not connected. Create connection.
        CLOG(( EConn, 4, _L("  starting connection") ));
        // Make this part atomic by pushing closes on the stack.
        User::LeaveIfError( iSockServ.Connect() );
        CleanupClosePushL<RSocketServ>( iSockServ );
        User::LeaveIfError( iConn.Open( iSockServ ) );
        CleanupClosePushL<RConnection>( iConn );
        iConnPref.SetDirection( ECommDbConnectionDirectionOutgoing );
        if ( aIap )
            {
            // Use specified IAP.
            iConnPref.SetIapId( aIap );
            iConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
            }
        else
            {
            // aIap == 0 -> user select.
            iConnPref.SetDialogPreference( ECommDbDialogPrefPrompt );
            }
        iConn.Start( iConnPref, iStatus );
        iState = EConnecting;
        SetActive();
        CleanupStack::Pop( 2 ); // closing iConn and iSockServ
        // End of atomic part.
        }
    else
        {
        // Not expecting this to be called in other states.
        CLOG(( EConn, 0, _L("CConnection::ConnectL: unexpected state (%d)"), \
                                                                    iState ));
        __ASSERT_ALWAYS( EFalse, CodPanic( ECodInternal ) );
        }

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    if ( !IsActive() )
        {
        // Unless we have an outstanding connect request (iConn.Start),
        // we are done.
        Done();
        }

    CLOG(( EConn, 2, _L("<- CConnection::ConnectL") ));
    }

// ---------------------------------------------------------
// CConnection::AttachL()
// ---------------------------------------------------------
//
void CConnection::AttachL( TUint32 aIap )
    {
    CLOG(( EConn, 2, _L("-> CConnection::AttachL iap(%d)"), aIap ));

    iStatus = KErrCodCannotConnect;
    if ( iState == EConnected )
        {
        // Already connected; do nothing.
        CLOG(( EConn, 4, _L("  already connected") ));
        iStatus = KErrNone;
        }
    else if ( iState == EInit )
        {
        // Not connected. Create connection, or attach to existing.
        CLOG(( EConn, 4, _L("  not connected") ));
        if ( aIap )
            {
            // Make this part atomic by pushing closes on the stack.
            User::LeaveIfError( iSockServ.Connect() );
            CleanupClosePushL<RSocketServ>( iSockServ );
            User::LeaveIfError( iConn.Open( iSockServ ) );
            CleanupClosePushL<RConnection>( iConn );
            TConnectionInfoBuf connInfo;
            TUint count;
            User::LeaveIfError( iConn.EnumerateConnections( count ) );
            CLOG(( EConn, 4, _L("  %d existing connections"), count ));
            TUint i;
            for ( i = count; i; i-- )
                {
                // Note: GetConnectionInfo expects 1-based index.
                User::LeaveIfError( iConn.GetConnectionInfo( i, connInfo ) );
                CLOG(( EConn, 4, _L("  conn(%d) iap(%d)"), \
                                                i, connInfo().iIapId ));
                if ( connInfo().iIapId == aIap )
                    {
                    CLOG(( EConn, 4, _L("  attach"), i, count ));
                    User::LeaveIfError( iConn.Attach
                        ( connInfo, RConnection::EAttachTypeNormal ) );
                    iState = EConnected;
                    iStatus = KErrNone;
                    break;
                    }
                }
            CleanupStack::Pop( 2 ); // closing iConn and iSockServ
            // End of atomic part.
            }
        if ( iState == EInit )
            {
            CLOG(( EConn, 4, _L("  ended unconnected") ));
            iConn.Close();
            iSockServ.Close();
            }
        }
    else
        {
        // Not expecting this to be called in other states.
        CLOG(( EConn, 0, _L("CConnection::AttachL: unexpected state (%d)"), \
                                                                    iState ));
        __ASSERT_ALWAYS( EFalse, CodPanic( ECodInternal ) );
        }
    User::LeaveIfError( iStatus.Int() );

    CLOG(( EConn, 2, _L("<- CConnection::AttachL") ));
    }

// ---------------------------------------------------------
// CConnection::Close()
// ---------------------------------------------------------
//
void CConnection::Close()
    {
    CLOG(( EConn, 2, _L("-> CConnection::Close") ));
    Cancel();
    DoClose();
    CLOG(( EConn, 2, _L("<- CConnection::Close") ));
    }

// ---------------------------------------------------------
// CConnection::IsConnected()
// ---------------------------------------------------------
//
TBool CConnection::IsConnected( TUint32& aIap )
    {
    TBool connected( EFalse );
    if( iState == EConnected )
        {
        TBuf<KCommsDbSvrMaxColumnNameLength * 2 + 1> iapId;
        _LIT( KFormatIapId, "%S\\%S" );
        TPtrC iap( IAP );
        TPtrC id( COMMDB_ID );
        iapId.Format( KFormatIapId, &iap, &id );
        TInt err = iConn.GetIntSetting( iapId, aIap );
        connected = err ? EFalse : ETrue;
        }
    return connected;
    }

// ---------------------------------------------------------
// CConnection::CConnection()
// ---------------------------------------------------------
//
CConnection::CConnection()
: CActive( CActive::EPriorityStandard ),
  iState( EInit )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CConnection::ConstructL()
// ---------------------------------------------------------
//
void CConnection::ConstructL()
    {
    if ( !CodUtil::ApCountL() )
        {
        // It's futile to try making a connection without an AP, so leave if
        // we don't have any.
        User::Leave( KErrCodNoAccessPoint );
        }
    }

// ---------------------------------------------------------
// CConnection::DoCancel()
// ---------------------------------------------------------
//
void CConnection::DoCancel()
    {
    CLOG(( EConn, 2, _L("-> CConnection::DoCancel") ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    // RConnection has no cancel method matching the async RConnection::Start.
    // The only way to cancel is to close the RConnection.
    // (Our request will complete with KErrCancel, as expected.)
    DoClose();
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::DoCancel") ));
    }

// ---------------------------------------------------------
// CConnection::RunL()
// ---------------------------------------------------------
//
void CConnection::RunL()
    {
    CLOG(( EConn, 2, _L("-> CConnection::RunL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().
    iState = EConnected;
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::RunL") ));
    }

// ---------------------------------------------------------
// CConnection::RunError()
// ---------------------------------------------------------
//
TInt CConnection::RunError( TInt LOG_ONLY( aError ) )
    {
    CLOG(( EConn, 2, _L("-> CConnection::RunError aError(%d)"), aError ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    DoClose();
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::RunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CConnection::DoClose()
// ---------------------------------------------------------
//
void CConnection::DoClose()
    {
    CLOG(( EConn, 2, _L("CConnection::DoClose") ));
    iConn.Close();
    iSockServ.Close();
    iState = EInit;
    }

// ---------------------------------------------------------
// CConnection::Done()
// ---------------------------------------------------------
//
void CConnection::Done()
    {
    CLOG(( EConn, 2, _L("CConnection::Done iStatus(%d)"), iStatus.Int() ));
    __ASSERT_DEBUG( iParentStatus, CodPanic( ECodInternal ) );
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    }
