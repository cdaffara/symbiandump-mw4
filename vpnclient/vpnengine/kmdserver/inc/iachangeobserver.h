/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors internal address change
*
*/


#ifndef C_IACHANGEOBSERVER_H
#define C_IACHANGEOBSERVER_H

#include <e32base.h>
#include "eventmediatorapi.h"

// FORWARD DECLARATIONS
class CInternalAddress;
class MIkeDebug;
class MIkePluginSessionIf;
class TInetAddr;

/**
 *  IA change observer.
 *  Active object that provides functionality for observing internal address
 *  change.
 *
 *  @lib internal (kmdserver.exe)
  */
class CIaChangeObserver : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aVpnIapId VPN IAP id
     * @param aDnsServerAddr DNS server address from IKE policy
     * @param aIkePluginSession IKE plugin session
     * @param aDebug Debug trace interface
     */
    static CIaChangeObserver* NewL( TUint32 aVpnIapId,
                                    const TInetAddr& aDnsServerAddr,
                                    MIkePluginSessionIf& aIkePluginSession,
                                    MIkeDebug& aDebug );

    /**
     * Destructor.
     */
    ~CIaChangeObserver();

    /**
     * Starts observing internal address change.
     */
    void StartObserving();
    
private:
    
    CIaChangeObserver( TUint32 aVpnIapId,
                       const TInetAddr& aDnsServerAddr,
                       MIkePluginSessionIf& aIkePluginSession,
                       MIkeDebug& aDebug );
    
    void ConstructL();

// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous notification request.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of asynchronous notification request.
     */    
    void DoCancel();
        
private: // data
    
    /**
     * Event mediator.
     * Own.
     */
    REventMediator             iEventMediator;

    /**
     * VPN IAP Id.
     * Own.
     */            
    TUint32                    iVpnIapId;
    
    /**
     * DNS server address.
     * Own.
     */
    TInetAddr                   iDnsServerAddr;

    /**
     * Internal address.
     * Own.
     */
    TVPNAddress                iInternalAddress;
    
    /**
     * IKE plugin session.
     * Not own.
     */
    MIkePluginSessionIf&        iIkePluginSession;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;            
    };


#endif // C_IACHANGEOBSERVER_H
