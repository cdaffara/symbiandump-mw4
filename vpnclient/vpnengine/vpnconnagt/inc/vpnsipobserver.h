/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description : P&S key monitor for communication between VPN client
*               and SIP Profile server
* Name        : vpnsipobserver.h
* Part of     : VPN client / VPN Connection Agent
* Version     : 1.0
*
*/

#ifndef VPNSIPOBSERVER_H
#define VPNSIPOBSERVER_H

#include <e32base.h>
#include "vpnconnagt.h"                   // Reference to CVPNConnAgt is needed for call back

class CVPNConnAgt;

NONSHARABLE_CLASS( CVpnSipObserver ): public CActive
    {      
public: // Constructor & destructor

    /**
     * Two-phased constructor
     */
    static CVpnSipObserver* NewL( CVPNConnAgt& aAgent );

    /**
     * Destructor
     */
    virtual ~CVpnSipObserver();
        
        
public: // New methods

    /**
     * Closes the sip connections
     * @return ETrue if connections are succesfully closed
     */
    TInt RequestDeregister();
        
    /**
     * Re-opens connection in case vpn session ends
     */
    TInt RequestRegister();
    
    
private: // From CActive

    /**
     * From CActive
     */
    void RunL();
    
    /**
     * From CActive
     */
    void DoCancel();
        
        
private: // New methods

    /**
     * Subscribes to the property
     */
    void Subscribe();
        
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * C++ constructor
     */
    CVpnSipObserver( CVPNConnAgt& aAgent );


private: // Data
   
    /**
     * Own: P&S for communication between SIP Profile Server and VPN client
     */
    RProperty iSIPProperty;
    
    /**
     * Own: Owner of this observer
     */
    CVPNConnAgt& iAgent;
    };
    
#endif    // VPNSIPOBSERVER_H
