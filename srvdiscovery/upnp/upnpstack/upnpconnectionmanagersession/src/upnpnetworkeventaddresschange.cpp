/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

// Includes
#include <upnpnetworkeventaddresschange.h>

// -----------------------------------------------------------------------------
// CUpnpNetworkEventAddressChange::CUpnpNetworkEventAddressChange
// 
// -----------------------------------------------------------------------------
//
CUpnpNetworkEventAddressChange::CUpnpNetworkEventAddressChange( TInetAddr aAddress )
    : CUpnpNetworkEventBase( EAddressChangeEvent ), iInetAddr( aAddress )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpNetworkEventAddressChange::Address
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpNetworkEventAddressChange::Address()
    {
    return iInetAddr;
    }
    

// End of file

