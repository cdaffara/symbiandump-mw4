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
 * Description:  Defines RUpnpDispatcherSession class
 *
 */

// INCLUDE FILES
#include <e32math.h>
#include "upnpmhcommon.h"
#include "upnpdispatchersession.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPStack.txt")
#endif
#include "upnpcustomlog.h"

// Number of message slots to reserve for this client server session.

#if defined(__HN_31__) || defined(__HN_32__)
const TUid KServerUid3 =
    {0x2000F881};
#else
const TUid KServerUid3 =
    {0x10005BCD};
#endif

#ifdef __WINS__
static const TUint KServerMinHeapSize = 0x100000; //  4K
static const TUint KServerMaxHeapSize = 0x1000000; // 16M
#endif

const TInt KServerMessageSlots = 10; //Number of message slots available per session.
const TInt KServerRetryCount = 2;

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RUpnpDispatcherSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RUpnpDispatcherSession::RUpnpDispatcherSession() :
    RUpnpSessionBase(), iParameter0( NULL, 0, 0 )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::Connect
// Establish connection to server.
// -----------------------------------------------------------------------------
//
TInt RUpnpDispatcherSession::Connect()
    {
    return RUpnpSessionBase::Connect( KMessageHandlerName, TVersion(
        KMessageHandlerMajorVersionNumber, KMessageHandlerMinorVersionNumber,
        KMessageHandlerBuildVersionNumber ), KServerMessageSlots,
        KServerRetryCount, KMessageHandlerFilename, KServerUid3 );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestAddLocalDevice
// Client requests server to add local device.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestAddLocalDevice( TRequestStatus& aStatus,
    TUpnpAddLocalDevice& aDevice, TPtr8& aBuffer )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( reinterpret_cast<TUint8*>( &aDevice ), sizeof(aDevice ),
        sizeof(aDevice ) );
    SendReceive( EAddLocalDevice, TIpcArgs( &iParameter0, &aBuffer ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestRemoveLocalDevice
// Client requests server to remove local device.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestRemoveLocalDevice(
    TRequestStatus& aStatus, const TDesC8& aUuid )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( (TUint8*)aUuid.Ptr( ), aUuid.Size( ), aUuid.Size( ) );
    SendReceive( ERemoveLocalDevice, TIpcArgs( &iParameter0 ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestRemoveSilentLocalDevice
// Client requests server to remove local device.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestRemoveSilentLocalDevice(
    TRequestStatus& aStatus, const TDesC8& aUuid )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( (TUint8*)aUuid.Ptr( ), aUuid.Size( ), aUuid.Size( ) );
    TIpcArgs ipcArgs;
    ipcArgs.Set( 0, &iParameter0 );
    SendReceive( ERemoveSilentLocalDevice, ipcArgs, aStatus );
    LOGS("RUpnpDispatcherSession::RequestRemoveSilentLocalDevice passed");
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestAddControlPoint
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestAddControlPoint( TRequestStatus& aStatus )
    {
    SendReceive( EAddControlPointClient, aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestRemoveControlPoint
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestRemoveControlPoint(
    TRequestStatus& aStatus )
    {
    SendReceive( ERemoveControlPointClient, aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestStopFiltering
//
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestStopFiltering( const TDesC8& aUuids )
    {
    //  Retruned error is ignored
    SendReceive( EStopFilteringDevice, TIpcArgs( &aUuids ) );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestGetDeviceListSize
// Client requests server to get device list size.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestGetDeviceListSize(
    TRequestStatus& aStatus, TUpnpGetDeviceListSize& aSize )
    {
    iParameter0.Set( reinterpret_cast<TUint8*>(&aSize), sizeof(aSize ),
        sizeof(aSize ) );
    aStatus = KRequestPending;
    SendReceive( EGetDeviceListSize, TIpcArgs( &iParameter0 ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestGetDeviceList
// Client requests server to get device list.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestGetDeviceList( TRequestStatus& aStatus,
    TPtr8& devPtr, TPtr8& servPtr ) const
    {
    LOG_FUNC_NAME;
    SendReceive( EGetDeviceList, TIpcArgs( &devPtr, &servPtr ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestSsdpSearch
// Client requests server to start SSDP search.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestSsdpSearch( TRequestStatus& aStatus,
    TDesC8& aSearch, TDesC8& aMX )
    {
    LOG_FUNC_NAME;
    SendReceive( ESsdpSearch, TIpcArgs( &aSearch, &aMX ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestActiveIap
// Client requests server active IAP.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestActiveIap( TRequestStatus& aStatus,
    TInt& aIap )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( reinterpret_cast<TUint8*>( &aIap ), sizeof(TInt),
        sizeof(TInt) );
    SendReceive( EActiveIap, TIpcArgs( &iParameter0 ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestStartSsdp
// Client requests server to start SSDP.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestStartSsdpL() const
    {
    LOG_FUNC_NAME;
    User::LeaveIfError( SendReceive( EStartSsdp, TIpcArgs() ) );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestCheckError
// 
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestCheckError( TInt& aError )
    {
    TPckgBuf<TInt> pckgTInt;
    TIpcArgs ipcArgs;
    pckgTInt = aError;
    ipcArgs.Set( 0, &pckgTInt );

    SendReceive( ECheckError, ipcArgs );

    aError = pckgTInt( );
    }

// -----------------------------------------------------------------------------
// RUpnpDispatcherSession::RequestStop
// Client requests server to cancel.
// -----------------------------------------------------------------------------
//
void RUpnpDispatcherSession::RequestStop()
    {
    LOG_FUNC_NAME;
    TInt err = SendReceive( ECancelRequest, TIpcArgs( ) );
    if ( err != KErrNone )
        {
        LOGS("RUpnpDispatcherSession::RequestStop - Request for RMessage Cancel Failed");
        }
    }

//  End of File
