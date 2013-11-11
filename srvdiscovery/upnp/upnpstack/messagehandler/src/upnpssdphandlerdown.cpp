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


// INCLUDE FILES
#include "upnpssdphandlerdown.h"
#include "upnpssdpserver.h"

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::CUpnpSsdpHandlerDown
// -----------------------------------------------------------------------------
// 
CUpnpSsdpHandlerDown::CUpnpSsdpHandlerDown( 
                               MUpnpSsdpServerObserver& aSsdpServerObserver )
                               : CUpnpSsdpHandlerBase( aSsdpServerObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::CUpnpSsdpHandlerDown
// -----------------------------------------------------------------------------
// 
CUpnpSsdpHandlerDown::~CUpnpSsdpHandlerDown()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::CUpnpSsdpHandlerDown
// -----------------------------------------------------------------------------
// 
CUpnpSsdpHandlerDown* CUpnpSsdpHandlerDown::NewL( 
                              MUpnpSsdpServerObserver& aSsdpServerObserver )
    {
    CUpnpSsdpHandlerDown* self = new ( ELeave ) 
                                 CUpnpSsdpHandlerDown( aSsdpServerObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::IsStarted
// -----------------------------------------------------------------------------
// 
TBool CUpnpSsdpHandlerDown::IsStarted()
    {
    return ETrue;    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::ConstructL
// -----------------------------------------------------------------------------
// 
void CUpnpSsdpHandlerDown::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::StartL
// -----------------------------------------------------------------------------
// 
void CUpnpSsdpHandlerDown::StartL( RSocketServ& /*aServ*/ )
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::Stop
// -----------------------------------------------------------------------------
//     
void CUpnpSsdpHandlerDown::Stop()
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::AddressChangeL
// -----------------------------------------------------------------------------
//     
void CUpnpSsdpHandlerDown::AddressChangeL( TInetAddr& /*aAddress*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::SsdpSearchL
// -----------------------------------------------------------------------------
// 
void CUpnpSsdpHandlerDown::SsdpSearchL( const TDesC8& /*aSearchString*/, TDesC8& /*aMX*/ )
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerDown::AdvertiseDeviceL
// -----------------------------------------------------------------------------
// 
void CUpnpSsdpHandlerDown::AdvertiseDeviceL( TInt /*aLive*/,  CUpnpDeviceLibraryElement& /*aElement*/ )
    {    
    }


// End of File
