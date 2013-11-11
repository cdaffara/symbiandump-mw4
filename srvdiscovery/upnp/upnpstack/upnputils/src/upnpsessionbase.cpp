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
* Description:  Implementation of RUpnpSessionBase
*
*/

#include "upnpsessionbase.h"
// ======== LOCAL FUNCTIONS ========
#include "upnpcustomlog.h"

// -----------------------------------------------------------------------------
// StartServer
// Static method to start the server.
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
// -----------------------------------------------------------------------------
//
static TInt StartServer( TUid aUid3, const TDesC& aFileName )
    {
    const TUidType serverUid( KNullUid, KNullUid, aUid3 );
    RProcess server;
    TInt r = server.Create( aFileName, KNullDesC, serverUid );
    if ( r != KErrNone )
        {
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 ); // abort startup
        }
    else
        {
        server.Resume(); // logon OK - start the server
        }
    User::WaitForRequest( stat ); // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

// -----------------------------------------------------------------------------
// RUpnpSessionBase::Connect
// Connect to the server, attempting to start it if necessary
// -----------------------------------------------------------------------------
EXPORT_C TInt RUpnpSessionBase::Connect( const TDesC& aServerName, TVersion aServerVersion,
    TInt aServerMessageSlots, TInt aServerStartRetryCount,
    const TDesC& aServerFileName, TUid aServerUid3 )
    {
    TInt retry = aServerStartRetryCount;
    for ( ;; )
        {
        TInt r = CreateSession( aServerName, aServerVersion, aServerMessageSlots );
        if ( r != KErrNotFound && r != KErrServerTerminated )
            {
            return r;
            }
        if ( --retry == 0 )
            {
            return r;
            }
        r = StartServer( aServerUid3, aServerFileName );
        if ( r != KErrNone && r != KErrAlreadyExists )
            {
            return r;
            }
        }
    }
