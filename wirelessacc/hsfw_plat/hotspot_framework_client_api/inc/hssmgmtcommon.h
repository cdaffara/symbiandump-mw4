/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains common data structures used by HotSpot management service.
*
*/



#ifndef HSSMGMTCOMMON_H
#define HSSMGMTCOMMON_H

// INCLUDES
#include <e32std.h>

// LOCAL CONSTANTS
// The maximum SSID length.
const TUint KHssMaxSsidLength = 32;

// The maximum BSSID length.
const TUint KHssMaxBssidLength = 6;

// DATA TYPES
// Data structure for storing the SSID of a HSS network.
typedef TBuf8<KHssMaxSsidLength> THssSsid;

// Data structure for storing the BSSID of a BSS.
typedef TBuf8<KHssMaxBssidLength> THssBssid;

// Values for possible HSS connection security modes.
enum THssConnectionSecurityMode
    {
    // Security mode open, i.e. no security
    EHssConnectionSecurityOpen,
    // Security mode WEP
    EHssConnectionSecurityWep,
    // Security mode 802d1x
    EHssConnectionSecurity802d1x,
    // Security mode WPA
    EHssConnectionSecurityWpa,
    // Security mode WPA PSK
    EHssConnectionSecurityWpaPsk
    };

// CLASS DECLARATION
/** 
* @brief Callback interface for HotSpot management notifications.
*
* These virtual methods should be inherited and implemented by the
* client wanting to observe HotSpot management events.
*
* The client has to enable notifications by calling the appropriate
* method from the management interface.
* @see MHssInterface::ActivateNotificationsL.
*/
class MHssMgmtNotifications
    {
    public:

        /**
        * New networks have been detected during scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void NewNetworksDetected( TUint /*aIapId*/) {};

        /**
        * One or more networks have been lost since the last scan.
        *
        * @param aIapId ID of the IAP.
        */
        virtual void OldNetworksLost( TUint /*aIapId*/ ) {};

    };

#endif // HSSMGMTCOMMON_H
            
// End of File
