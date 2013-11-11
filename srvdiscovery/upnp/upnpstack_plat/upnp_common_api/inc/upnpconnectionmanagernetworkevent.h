/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: TUpnpConnectionManagerNetworkEvent
*
*/

#ifndef T_UPNPCONNECTIONMANAGERNETWORKEVENT_H
#define T_UPNPCONNECTIONMANAGERNETWORKEVENT_H

// INCLUDE FILES
#include <e32cmn.h>
#include <in_sock.h>
#include "upnpconnmanagereventtypes.h"

/**
 * This class describes network event sent by ipc
 * @since S60 5.0
 */
class TUpnpConnectionManagerNetworkEvent
    {
public:
    
    /**
     * C++ constructor
     */
    IMPORT_C TUpnpConnectionManagerNetworkEvent();

    /**
     * C++ constructor
     */
    IMPORT_C TUpnpConnectionManagerNetworkEvent( TUpnpCmSubRsp aType );

    /**
     * C++ constructor
     */
    IMPORT_C TUpnpConnectionManagerNetworkEvent( TUpnpCmSubRsp aType, 
                                                 TInetAddr aInetAddr );

    /**
     * Sets event type
     */
    IMPORT_C void SetType( TUpnpCmSubRsp aType );

    /**
     * Sets address after address change event
     */  
    IMPORT_C void SetAddress( TInetAddr aAddress );
    
    /**
     * Get event type
     */  
    IMPORT_C TUpnpCmSubRsp& Type();

    /**
     * Get address
     */  
    IMPORT_C TInetAddr InetAddr();

private:
    
    // event type    
    TUpnpCmSubRsp iType;

    // address
    TInetAddr iInetAddr;
    };

#endif // T_UPNPCONNECTIONMANAGERNETWORKEVENT_H

// End of File
