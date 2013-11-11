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
* Description:  IKE protocol plugin interface
*
*/


#ifndef M_IKEPLUGINIF_H
#define M_IKEPLUGINIF_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MIkeDataInterface;
class MIkeDebug;
class MIkePluginSessionIf;
class MKmdEventLoggerIf;

/**
 *  IKE protocol plugin interface.
 *
 *  IKE protocol plugin interface provides functionality for creating IKE
 *  protocol plugin sessions.
 *
 *  @lib internal (kmdserver.exe)
 */
class MIkePluginIf
    {        
public:    

    /**
     * Destructor.
     */
    virtual ~MIkePluginIf() {};
    
    /**
     * Creates IKE protocol plugin session.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aDataInterface IKE data interface
     * @return IKE protocol plugin session interface. Ownership transferred.
     */
    virtual MIkePluginSessionIf* CreateSessionL( TUint32 aVpnIapId,
                                                 TUint32 aVpnNetId,
                                                 TUint32 aVpnInterfaceIndex,
                                                 MIkeDataInterface& aIkeDataInterface ) = 0;    
    };


/**
 * Method prototype to create new protocol plugin
 */
typedef MIkePluginIf* (*CreateIkePluginL)(MKmdEventLoggerIf&, MIkeDebug&);

#endif // M_IKEPLUGINIF_H
