/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares ControlPoint Discovery class.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "upnpcompvariant.hrh"

#include <upnpdevice.h>
#ifdef RD_UPNP_REMOTE_ACCESS
#include "upnpradasync.h"
#endif
#include "upnpcpbdiscoveryagent.h"
#include "upnpcpbhttpmessagecontroller.h"

#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDiscoveryAgent* CUpnpCpbDiscoveryAgent::NewL(
    MUpnpCpbHttpMessageController& aMessanger )
    {
    LOG_FUNC_NAME;
    CUpnpCpbDiscoveryAgent* self = new (ELeave) CUpnpCpbDiscoveryAgent(aMessanger);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::~CUpnpCpbDiscoveryAgent
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDiscoveryAgent::~CUpnpCpbDiscoveryAgent()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    delete iRadaClient;
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::SearchL
// Search intresting Device Types from network
// -----------------------------------------------------------------------------
//
void CUpnpCpbDiscoveryAgent::SearchL( const TDesC8& aSearchString )
    {
    iMessanger.SearchL( aSearchString );

    #ifdef RD_UPNP_REMOTE_ACCESS
    if(iRadaClient)
        {
        iRadaClient->GetDevicesL();
        }
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::CUpnpCpbDiscoveryAgent
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbDiscoveryAgent::CUpnpCpbDiscoveryAgent( MUpnpCpbHttpMessageController& aMessanger)
    : iMessanger( aMessanger )
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpCpbDiscoveryAgent::ConstructL()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iRadaClient = CUpnpRadaSync::NewL(this);
    #endif
    }

#ifdef RD_UPNP_REMOTE_ACCESS
// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::EnableRadaDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpCpbDiscoveryAgent::EnableRadaDeviceL( TRequestStatus& aStatus )
{
   iRadaClient->EnableRadaL( aStatus );
}
// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::EnableRadaDeviceL
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbDiscoveryAgent::EnableRadaDeviceL(  )
{
    return iRadaClient->EnableRadaL();
}
// -----------------------------------------------------------------------------
// CUpnpCpbDiscoveryAgent::DisableRadaDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpCpbDiscoveryAgent::DisableRadaDeviceL(  )
{
    iRadaClient->DisableRadaL();
}

void CUpnpCpbDiscoveryAgent::StartIPFilteringL( )
{
    iRadaClient->StartIPFilteringL( );
}

void CUpnpCpbDiscoveryAgent::StopIPFiltering( )
{
    iRadaClient->StopIPFiltering( );
}
#endif

//  End of File

