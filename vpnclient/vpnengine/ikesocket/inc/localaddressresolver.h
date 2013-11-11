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


#ifndef C_LOCALADDRRESOLVER_H
#define C_LOCALADDRRESOLVER_H

#include <in_sock.h>
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class MIkeDebug;

/**
 *  Local address resolver.
 *
 *  This class resolves local IP addresses (IPv4 and/or IPv6) for network
 *  connection (RConnection).
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( CLocalAddressResolver ) : public CBase
    { 
public:
    /**
     * Two-phased constructor.
     * @param aSocketServer Socket server
     * @param aConnection RConnection which is used for resolving
     * @param aDebug Debug trace interface
     */
    static CLocalAddressResolver* NewL( RSocketServ& aSocketServer,
                                        RConnection& aConnection,
                                        MIkeDebug& aDebug);
    
    /**
     * Destructor.
     */
    ~CLocalAddressResolver();
    
    /**
     * Returns information about whether local IP address has been resolved.
     * @param aIpVersion IP version of local address (IPv4/IPv6)
     * @return Local IP address resolved or not.
     */
    TBool HasIPAddr( const IkeSocket::TIpVersion aIpVersion )  const;
    
    /**
     * Returns resolved local IP address.
     * @param aIpVersion IP version of local address (IPv4/IPv6)
     * @return Local IP address.
     */
    const TInetAddr& IPAddr( const IkeSocket::TIpVersion aIpVersion ) const;
    
    /**
     * Refreshes local IP addresses (IPv4 and/or IPv6). 
     * @param aIpVersion IP version of local address (IPv4/IPv6)
     * @return Local IP address.
     */
    TInt RefreshLocalAddresses();

    /**
     * Gets local IP address of interface.
     *
     * @param aIpVersion IP version of local IP address
     * @param aLocalIp Local IP address (returned)
     * @return Error status. KErrNotFound if address is not found.
     */
    TInt GetLocalAddress( const IkeSocket::TIpVersion aIpVersion,
                          TInetAddr& aLocalIp );

private:

    CLocalAddressResolver( RSocketServ& aSocketServer,
                           RConnection& aConnection,
                           MIkeDebug& aDebug );

    void ConstructL();
    
    /**
     * Mathches information of an interface and the connection. 
     * @param aInfo Information of interface
     * @return aQuery Query for interface
     */
    TBool Match( const TSoInetInterfaceInfo& aInfo,
                 const TSoInetIfQuery& aQuery) const;
    
    /**
     * Sets local IP address if not yet set. 
     * @param aAddr IP address
     */
    void SetAddressIfNotSet( const TInetAddr& aAddr );
    
    /**
     * Sets local IPv4 address if not yet set. 
     * @param aAddr IP address
     */
    void SetIPv4AddressIfNotSet( const TInetAddr& aAddr );
    
    /**
     * Sets local IPv6 address if not yet set. 
     * @param aAddr IP address
     */
    void SetIPv6AddressIfNotSet( const TInetAddr& aAddr );    

private: // data
    /**
     * IAP id of the connection.
     * Own.
     */
    TUint32             iIapId;
    
    /**
     * Socket server.
     * Not own.
     */
    RSocketServ&        iSocketServer;
    
    /**
     * Connection.
     * Not own.
     */
    RConnection&        iConnection;
    
    /**
     * Local IPv4 address.
     * Own.
     */
    TInetAddr           iIPv4Addr;
    
    /**
     * Local IPv6 address.
     * Own.
     */
    TInetAddr           iIPv6Addr;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&          iDebug;
    };
    
#endif // C_LOCALADDRRESOLVER_H


