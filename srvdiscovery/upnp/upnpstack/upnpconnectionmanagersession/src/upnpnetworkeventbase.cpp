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
#include <upnpnetworkeventbase.h>


// -----------------------------------------------------------------------------
// CUpnpNetworkEventBase::CUpnpNetworkEventBase
// 
// -----------------------------------------------------------------------------
//
CUpnpNetworkEventBase::CUpnpNetworkEventBase( TInt aError )
    : iType( EUnknownEvent ), iSubscriberError( aError )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpNetworkEventBase::CUpnpNetworkEventBase
// 
// -----------------------------------------------------------------------------
//
CUpnpNetworkEventBase::CUpnpNetworkEventBase( TUpnpCmSubRsp aType )
    : iType( aType ), iSubscriberError( KErrNone )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpNetworkEventBase::Type
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUpnpCmSubRsp CUpnpNetworkEventBase::Type()
    {
    return iType;
    }   

// -----------------------------------------------------------------------------
// CUpnpNetworkEventBase::SubscriberError
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpNetworkEventBase::SubscriberError()
    {
    return iSubscriberError;
    }
    
// End of file

