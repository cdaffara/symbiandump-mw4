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
* Description:  upnpconnectionmanager / client side
 *
*/

#include "upnpconnectionmanagernetworkevent.h"
#include "upnpconnmanagercommon.h"

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent
// C++ default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent() 
    : iType( EUnknownEvent )
    {
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent
// C++ constructor
// -----------------------------------------------------------------------------
//    
EXPORT_C TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent( 
    TUpnpCmSubRsp aType )
    : iType( aType )
    {
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent
// C++ constructor
// -----------------------------------------------------------------------------
//    
EXPORT_C TUpnpConnectionManagerNetworkEvent::TUpnpConnectionManagerNetworkEvent( 
    TUpnpCmSubRsp aType, TInetAddr aInetAddr )
    : iType( aType ), iInetAddr( aInetAddr )
    {
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::SetType
// -----------------------------------------------------------------------------
//    
EXPORT_C void TUpnpConnectionManagerNetworkEvent::SetType( TUpnpCmSubRsp aType )
    {
    iType = aType;
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::SetAddress
// -----------------------------------------------------------------------------
//    
EXPORT_C void TUpnpConnectionManagerNetworkEvent::SetAddress( TInetAddr aAddress )
    {
    iInetAddr = aAddress;	
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::Type
// -----------------------------------------------------------------------------
//    
EXPORT_C TUpnpCmSubRsp& TUpnpConnectionManagerNetworkEvent::Type()
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// TUpnpConnectionManagerNetworkEvent::InetAddr
// -----------------------------------------------------------------------------
//   
EXPORT_C TInetAddr TUpnpConnectionManagerNetworkEvent::InetAddr()
    {
    return iInetAddr;
    }

// End of file
