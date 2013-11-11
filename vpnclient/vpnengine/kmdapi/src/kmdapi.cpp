/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Key Management Daemon API
*
*/

// EXTERNAL INCLUDES
#include <e32std.h>
#include <e32math.h>

// INTERNAL INCLUDES
#include "kmdserver.h"
#include "clistatic.h"

// CLASS HEADER
#include "kmdapi.h"

// ---------------------------------------------------------------------------
// Opens a session to KMD server and starts the server if it is not yet
// started.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RKMD::Connect()
	{
	TInt retry=2;
	for ( ;; )
		{
		TInt r = CreateSession( KKmdServerName,
                                TVersion( KKmdServMajorVersion,
                                          KKmdServMinorVersion,
                                          KKmdServBuildVersion),
                                          KDefaultMessageSlots );

		if ( r != KErrNotFound && r != KErrServerTerminated )
		    {
		    return r;
		    }
		if ( --retry == 0 )
		    {
			return r;
		    }
		r = Launcher::LaunchServer( KKmdServerName,
		                            KKmdServerImg,
                                    KServerUid3,
                                    KMyServerInitHeapSize,
                                    KMyServerMaxHeapSize,
                                    KMyServerStackSize );
		if ( r != KErrNone && r != KErrAlreadyExists )
		    {
			return r;
		    }
		}
	}

// ---------------------------------------------------------------------------
// Start a real network connection.
// ---------------------------------------------------------------------------
//
EXPORT_C void RKMD::StartRealConnection( TUint32 aVpnIapId, 
                                         TPckg<TVpnRealConnectionParams>& aRealConfig,
                                         TRequestStatus& aStatus ) const
    {
    SendReceive( CKmdServer::KKmdStartConnection,
                 TIpcArgs( aVpnIapId, &aRealConfig ),
                 aStatus );
    }
    
// ---------------------------------------------------------------------------
// Cancels ongoing activate request.
// ---------------------------------------------------------------------------
//
EXPORT_C void RKMD::CancelStartRealConnection() const
    {
    SendReceive( CKmdServer::KKmdCancelStartConnection );
    }

// ---------------------------------------------------------------------------
// Asynchronous Activate request.
// ---------------------------------------------------------------------------
//
EXPORT_C void RKMD::Activate( TUint32 aVpnIapId,
                              const TDesC& aVpnIfName,
                              const TDesC8& aIkeConf,                               
                              TVPNAddressPckg& aVPNAddress,
                              TRequestStatus& aStatus ) const
    {
	SendReceive( CKmdServer::KKmdActivateAsync,
				 TIpcArgs( aVpnIapId,
				           &aVpnIfName,
				           &aIkeConf,
				           &aVPNAddress ),
				           aStatus );
    }

// ---------------------------------------------------------------------------
// Cancels ongoing Activate request.
// ---------------------------------------------------------------------------
//
EXPORT_C void RKMD::CancelActivate() const
    {
    SendReceive( CKmdServer::KKmdCancelActivateAsync );
    }

// ---------------------------------------------------------------------------
// Stops specified VPN connection.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RKMD::StopVpnConnection( TUint32 aVpnIapId,
                                       TKmdStopConnection::TType aType )
    { 
    return SendReceive( CKmdServer::KKmdStopConnection,
                        TIpcArgs( aVpnIapId, aType ) );
    }

// ---------------------------------------------------------------------------
// Resolve an IP address from FQDN.
// ---------------------------------------------------------------------------
//
EXPORT_C void RKMD::ResolveAddress( TUint32 aVpnIapId, 
                                    const TDesC& aFqdn,
                                    TNameEntry& aResult,
                                    TRequestStatus& aStatus ) const
    {
    SendReceive( CKmdServer::KKmdResolveAddress,
                 TIpcArgs( aVpnIapId, &aFqdn, &aResult ),
                 aStatus );    
    }
    
// ---------------------------------------------------------------------------
// Cancel outstanding ResolveAddress query.
// ---------------------------------------------------------------------------
//    
EXPORT_C void RKMD::CancelResolve() const
    {
    SendReceive( CKmdServer::KKmdCancelResolveAddress );
    }

