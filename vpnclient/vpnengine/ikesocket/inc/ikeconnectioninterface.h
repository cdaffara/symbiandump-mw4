/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE socket connection interface
*
*/


#ifndef C_IKECONNECTIONINTERFACE_H
#define C_IKECONNECTIONINTERFACE_H

#include <e32base.h>
#include <in_sock.h>
#include "ikesocketdefs.h"

using namespace IkeSocket;

// FORWARD DECLARATIONS
class MIkeDebug;
class MIkeDataInterface;
class RSocketServer;

/**
 *  IKE socket connection interface.
 *
 *  This interface defines functionality for managing VPN AP's real network
 *  connection.
 * 
 *  Data interface (MIkeDataInterface) needs to be opened via this class. Data
 *  interface is closed automatically, when instance of connection interface
 *  (this interface) is deleted.
 *
 *  @lib ikesocket.lib
 */

class CIkeConnectionInterface : protected CActive
    {
public:    
     /**
     * Two-phased constructor.
     * @param aDebug Debug trace interface
     */
    IMPORT_C static CIkeConnectionInterface* NewL( MIkeDebug& aDebug );
    
    /**
    * Destructor.
    */
    virtual ~CIkeConnectionInterface();    
    
    /**
     * Opens data interface.
     *
     * @param aIkeMajorVersion IKE major version
     * @param aIpVersion IP protocol version
     * @return Data interface
     */
    virtual MIkeDataInterface& OpenDataInterfaceL( const TIkeMajorVersion aIkeMajorVersion,
                                                   const TIpVersion aIpVersion ) = 0;
    
    /**
     * Establishes connection for specified SNAP (destination) or IAP.
     *
     * @param aIapId IAP id
     * @param aSnapId SNAP id
     * @param aStatus Completion status (returned)
     * @param aForcedRoaming Indicates whether forced roaming is enabled or
     *                       disabled for SNAP
     */
    virtual void StartConnection( const TUint32 aIapId,
                                  const TUint32 aSnapId,
                                  TRequestStatus& aStatus,
                                  const TBool aForcedRoaming = EFalse ) = 0;
    
    /**
     * Cancels connection establishment.
     *
     */
    virtual void CancelStartConnection() = 0;
        
    /**
     * Closes the connection.
     *
     */
    virtual void StopConnection() = 0;
    
    /**
     * Resolves an IP address from FQDN address.
     *
     * @param aFQDN Fully Qualified Domain Name
     * @param aNameEntry Result of name resolution (returned)
     * @param aStatus Completion status (returned)
     */
    virtual void ResolveFQDNAddress( const TDesC& aFQDN,
                                     TNameEntry& aNameEntry,
                                     TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels FQDN address resolving.
     *
     */
    virtual void CancelResolveFQDNAddress() = 0;
    
    /**
     * Starts listening disconnect notification.
     *
     * @param aStatus Disconnection status (returned)
     */
    virtual void NotifyDisconnect( TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels notifying of disconnect indication.
     *
     */
    virtual void CancelNotifyDisconnect() = 0;
    
    /**
     * Returns IAP id.
     *
     * @return IAP id
     */
    virtual TUint32 IapId() const = 0;
    
    /**
     * Returns NET id.
     *
     * @return NET id
     */
    virtual TUint32 NetId() const = 0;

    /**
     * Returns SNAP id.
     *
     * @return SNAP id
     */
    virtual TUint32 SnapId() const = 0;
    
    /**
     * Gets local IP address of interface.
     *
     * @param aIpVersion IP version of local IP address
     * @param aLocalIp Local IP address (returned)
     * @return Error status. KErrNotFound if address is not found.
     */
    virtual TInt GetLocalAddress( const TIpVersion aIpVersion,
                                  TInetAddr& aLocalIp ) = 0;
    
protected:
    
    CIkeConnectionInterface( TInt aPriority );
    
    };


#endif // C_IKECONNECTIONINTERFACE_H
