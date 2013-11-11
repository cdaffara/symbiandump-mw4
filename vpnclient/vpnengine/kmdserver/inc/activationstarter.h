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
* Description:  active object, that monitors activating
*
*/


#ifndef C_ACTIVATIONSTARTER_H
#define C_ACTIVATIONSTARTER_H

#include <e32base.h>
#include <in_sock.h>

#include "vpnmandefs.h"
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class CIkeData;
class CVpnConnection;
class MIkeDebug;
class TVPNAddress;

/**
 *  Activation starter callback interface.
 *  Callback interface which is used by CActivationStarter object to notify
 *  about completion of activation.
 * 
 *  @lib internal (kmdserver.exe)
  */
class MActivationStarterCallback
    {
public:        
    /**
     * Notifies about completion of activation.
     * @param aStatus Completion status
     * @param aVirtualIp Internal address info
     */
    virtual void ActivationCompleted( TInt aStatus,
                                      const TVPNAddress& aVirtualIp ) = 0;
    };


/**
 *  Activating starter.
 *  Active object provides functionality for starting activating.
 *
 *  @lib internal (kmdserver.exe)
 */
class CActivationStarter : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aConnection VPN connection object
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CActivationStarter* NewL( CVpnConnection& aConnection,
                                     MActivationStarterCallback& aCallback,
                                     MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CActivationStarter();

    /**
     * Starts activating. Completion is notified via
     * MActivatingStarterCallback interface.
     * @param aIkeConf IKE policy data
     * @param aVpnIfName VPN interface name
     */
    void Activate( CIkeData& aIkeData,
                   const TDesC& aVpnIfName );

private:
    
    CActivationStarter( CVpnConnection& aConnection,
                        MActivationStarterCallback& aCallback,
                        MIkeDebug& aDebug );    
    
    /**
     * Gets VPN interface index.
     * @param aVpnIfName VPN interface name
     * @param aVpnInterfaceIndex VPN interface index (returned)
     * @return Error code
     */
    TInt GetVpnInterfaceIndex( const TDesC& aVpnIfName,
                               TUint32& aVpnInterfaceIndex );
        
// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous activating.
     */    
    void RunL();
    
    /**
     * From CActive
     * Handles cancellation of asynchronous activating.
     */    
    void DoCancel();
    
private: // data
    
    /**
     * Internal address.
     * Own.
     */
    TVPNAddress                 iInternalAddress;
    
    /**
     * IP version.
     * Own.
     */
    IkeSocket::TIpVersion       iIpVersion;
    
    /**
     * DNS server address from IKE policy data.
     * Own.
     */
    TInetAddr                   iDnsServerAddr;
    
    /**
     * VPN connection object.
     * Not own.
     */
    CVpnConnection&             iConnection;
    
    /**
     * Callback interface.
     * Not own.
     */
    MActivationStarterCallback& iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                   iDebug;
    };


#endif // C_ACTIVATIONSTARTER_H
