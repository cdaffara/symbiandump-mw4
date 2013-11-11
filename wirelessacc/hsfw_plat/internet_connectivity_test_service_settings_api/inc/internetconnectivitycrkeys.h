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
* Description:  Key values for testing internet connectivity
*   
*
*/


#ifndef INTERNETCONNECTIVITYCRKEYS_H
#define INTERNETCONNECTIVITYCRKEYS_H

// INCLUDES
#include <e32cmn.h>

/**
* Internet Connectivity Test Service settings
*/
const TUid KCRUidInternetConnectivitySettings = {0x10282EC7};

/**
* CenRep key for storing test server IP address
*
* Text
*
* Default value: "http://connectivity-test.ext.nokia.com/"
*/
const TUint32 KIctsIpDestination = 0x00000001;

/**
* CenRep key for internet connectivity test permission
*
* Integer
*
* Run automatically:    0
* Never run:            1 
*
* Default value:        0
*/
const TUint32 KIctsTestPermission = 0x00000002;

// Values for internet connectivity testing permission
enum TIctsTestPermission
    {
    // Run automatically. No permission asked.
    EIctsRunAutomatically = 0,
    // Do not run internet connectivity test at all.
    EIctsNeverRun
    };

#endif // INTERNETCONNECTIVITYCRKEYS_H
     

//  End of File
