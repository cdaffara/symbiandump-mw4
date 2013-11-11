/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Local address resolver
*
*/


// INCLUDE FILES
#include "localaddressresolver.h"
#include "ikedebug.h"
#include "ikesocketassert.h"

using namespace IkeSocket;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CLocalAddressResolver* CLocalAddressResolver::NewL( RSocketServ& aSocketServer,
                                                    RConnection& aConnection,
                                                    MIkeDebug& aDebug )
    {
    CLocalAddressResolver* self = 
        new ( ELeave ) CLocalAddressResolver( aSocketServer,
                                              aConnection,
                                              aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLocalAddressResolver::~CLocalAddressResolver()
    {
    DEBUG_LOG( _L("CLocalAddressResolver::~CLocalAddressResolver") );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLocalAddressResolver::CLocalAddressResolver( RSocketServ& aSocketServer,
                                              RConnection& aConnection,
                                              MIkeDebug& aDebug )
 : iSocketServer( aSocketServer ),
   iConnection( aConnection ),
   iDebug( aDebug )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CLocalAddressResolver::ConstructL()
    {
    DEBUG_LOG( _L("CLocalAddressResolver::ConstructL") );
    }

// ---------------------------------------------------------------------------
// Return information whether local IP address has been resolved or not.
// ---------------------------------------------------------------------------
//
TBool CLocalAddressResolver::HasIPAddr( const TIpVersion aIpVersion )  const
    {
    IKESOCKET_ASSERT( aIpVersion == EIPv4 || aIpVersion == EIPv6 );
    
    TBool hasIPAddr( EFalse );
    
    if ( aIpVersion == EIPv4 &&
         !iIPv4Addr.IsUnspecified() )
        {
        hasIPAddr = ETrue;
        }
    else if ( aIpVersion == EIPv6 &&
              !iIPv6Addr.IsUnspecified() )
        {
        hasIPAddr = ETrue;
        }
    return hasIPAddr;
    }

// ---------------------------------------------------------------------------
// Returns local IP address.
// ---------------------------------------------------------------------------
//
const TInetAddr& CLocalAddressResolver::IPAddr( const TIpVersion aIpVersion ) const
    {
    IKESOCKET_ASSERT( aIpVersion == EIPv4 || aIpVersion == EIPv6 );
        
    if ( aIpVersion == EIPv4)
        {
        return iIPv4Addr;
        }
    
    return iIPv6Addr;
    }

// ---------------------------------------------------------------------------
// Refreshes local IP addresses for the connection
// ---------------------------------------------------------------------------
//
TInt CLocalAddressResolver::RefreshLocalAddresses()
    {
    // Get IAP Id of the connection
    _LIT( KIapId, "IAP\\Id" );    
    TInt ret = iConnection.GetIntSetting( KIapId, iIapId ) ;
    
    if ( ret == KErrNone )
        {
        RSocket socket;
        ret = socket.Open( iSocketServer,
                           KAfInet,
                           KSockDatagram,
                           KProtocolInetUdp,
                           iConnection );

        // Initialize IP addresses
        iIPv4Addr.Init( KAfInet );
        iIPv6Addr.Init( KAfInet );

        if ( ret == KErrNone )
            {
            // Begin enumeration of network interfaces.
            TInt err = socket.SetOpt( KSoInetEnumInterfaces,
                                      KSolInetIfCtrl );
            if ( err == KErrNone )
                {                
                // Return details of the first interface in the enumeration.
                TPckgBuf<TSoInetInterfaceInfo> details;
                err = socket.GetOpt( KSoInetNextInterface,
                                     KSolInetIfCtrl,
                                     details );
                
                while ( err == KErrNone )
                    {
                    // Query IAP for the interface.
                    TPckgBuf<TSoInetIfQuery> query;
                    query().iName = details().iName;
                    err = socket.GetOpt( KSoInetIfQueryByName,
                                         KSolInetIfQuery,
                                         query );
                    
                    if ( err == KErrNone )
                        {
                        // Match information of the interface with the
                        // connection.
                        if ( Match( details(), query() ) )
                            {
                            // Match found. Set local IP address if not yet
                            // set.
                            SetAddressIfNotSet( details().iAddress );
                            }
                        }
                    // Return details of the next interface. If the interface
                    // has multiple addresses then details for each address
                    // are returned separately in the enumeration.
                    err = socket.GetOpt( KSoInetNextInterface,
                                         KSolInetIfCtrl,
                                         details );
                    }
                }
            socket.Close();
            }
        }
    
    DEBUG_LOG1( _L("CLocalAddressResolver::RefreshLocalAddresses, ret=%d"), ret );
    return ret;
    }   

// ---------------------------------------------------------------------------
// Gets local IP address.
// ---------------------------------------------------------------------------
//
TInt CLocalAddressResolver::GetLocalAddress( const TIpVersion aIpVersion,
                                             TInetAddr& aLocalIp )
    {
    IKESOCKET_ASSERT( aIpVersion == EIPv4 || aIpVersion == EIPv6 );
    DEBUG_LOG1( _L("CLocalAddressResolver::GetLocalAddress, aIpVersion=%d"),
            aIpVersion);

    TInt ret = RefreshLocalAddresses();
    
    if ( ret )
        {
        return ret;
        }
    
    if ( HasIPAddr( aIpVersion ) )
        {
        aLocalIp = IPAddr( aIpVersion );
        }
    else 
        {
        ret = KErrNotFound;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Mathches information of an interface and the connection.
// ---------------------------------------------------------------------------
//
TBool CLocalAddressResolver::Match(
    const TSoInetInterfaceInfo& aInfo,
    const TSoInetIfQuery& aQuery ) const
    {
    TBool match = EFalse;
    if ( !aInfo.iAddress.IsUnspecified() && 
         !aInfo.iAddress.IsLoopback() &&
         !aInfo.iAddress.IsLinkLocal() &&
         aQuery.iZone[1] == iIapId ) // Match IAPs of interface and connection
        {
        match = ETrue;
        }
    return match;
    }

// ---------------------------------------------------------------------------
// Sets local IP address if not set.
// ---------------------------------------------------------------------------
//
void CLocalAddressResolver::SetAddressIfNotSet( const TInetAddr& aAddr )
    {
    TInetAddr addr = aAddr;
    
    if ( addr.Family() == KAfInet ) // IPv4 address
        {
        SetIPv4AddressIfNotSet( addr );
        }
    else if ( addr.Family() == KAfInet6 )
        {
        if ( addr.IsV4Mapped() ) // IPv4 mapped address
            {
            addr.ConvertToV4();
            SetIPv4AddressIfNotSet( addr );            
            }
        else // IPv6 address
            {
            SetIPv6AddressIfNotSet( addr );
            }        
        }
    }

// ---------------------------------------------------------------------------
// Sets local IPv4 address if not set.
// ---------------------------------------------------------------------------
//
void CLocalAddressResolver::SetIPv4AddressIfNotSet( const TInetAddr& aAddr )
    {
    if ( iIPv4Addr.IsUnspecified() )
        {
#ifdef _DEBUG    
        TBuf<100> txt_addr;
        aAddr.Output( txt_addr );
        DEBUG_LOG1( _L("CLocalAddressResolver::SetIPv4AddressIfNotSet, address=%S"),
                &txt_addr );
#endif
        
        iIPv4Addr = aAddr;
        }
    }

// ---------------------------------------------------------------------------
// Sets local IPv6 address if not set.
// ---------------------------------------------------------------------------
//
void CLocalAddressResolver::SetIPv6AddressIfNotSet( const TInetAddr& aAddr )
    {
    if ( iIPv6Addr.IsUnspecified() )
        {
#ifdef _DEBUG    
        TBuf<100> txt_addr;
        aAddr.Output( txt_addr );
        DEBUG_LOG1( _L("CLocalAddressResolver::SetIPv6AddressIfNotSet, address=%S"),
                &txt_addr );
#endif

        iIPv6Addr = aAddr;
        }
    }
   
