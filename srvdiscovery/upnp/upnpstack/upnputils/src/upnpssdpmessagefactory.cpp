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
* Description:  Produces SSDP messages
*
*/


// INCLUDE FILES
#include "upnpssdpmessagefactory.h"
#include "upnpcons.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RUpnpSsdpMessageFactory::AdvertiseAliveL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSsdpAdvertise* RUpnpSsdpMessageFactory::AdvertiseAliveL( TDesC8& aLoc,
                                                      TDesC8& aNt, 
                                                      TDesC8& aUsn,
                                                      TDesC8& aServerDesc )
    {
    CUpnpSsdpMessage* msg = CUpnpSsdpMessage::NewL( CUpnpSsdpMessage::ERequestNotify );
    CleanupStack::PushL(msg);
    
    msg->AddHostL( (TDesC8&) UpnpSSDP::KDefaultHost);
    msg->AddCacheControlL( (TDesC8&) UpnpSSDP::KDefaultMaxAge);
    msg->AddLocationL(aLoc);
    msg->AddNotificationTypeL(aNt);
    msg->AddServerL(aServerDesc);
    msg->AddUsnL(aUsn);
    
    CleanupStack::Pop(); // msg
    
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpSsdpMessageFactory::AdvertiseByebyeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSsdpAdvertise* RUpnpSsdpMessageFactory::AdvertiseByebyeL( TDesC8& aNt,
                                                      TDesC8& aUsn )
    {
    
    CUpnpSsdpMessage* msg = CUpnpSsdpMessage::NewL(CUpnpSsdpMessage::ERequestByeBye);
    CleanupStack::PushL(msg);
    
    msg->AddNotificationTypeL(aNt); 
    msg->AddHostL( (TDesC8&) UpnpSSDP::KDefaultHost);
    msg->AddUsnL(aUsn);
    
    CleanupStack::Pop(); // msg
    
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpSsdpMessageFactory::SearchL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSsdpSearch* RUpnpSsdpMessageFactory::SearchL( TDesC8& aTarget )
    {
    CUpnpSsdpMessage* msg = CUpnpSsdpMessage::NewL(CUpnpSsdpMessage::ERequestUndefined);
    CleanupStack::PushL(msg);
    
    msg->AddPairL(UpnpSSDP::KMethodMSearch(), KNullDesC8());
    msg->AddPairL(UpnpSSDP::KHdrMx(), UpnpSSDP::KDefaultResponseDelay());
    msg->AddPairL(UpnpSSDP::KHdrSt(), aTarget);
    msg->AddPairL(UpnpSSDP::KHdrHost(), (TDesC8&) UpnpSSDP::KDefaultHost);
    msg->AddPairL(UpnpSSDP::KHdrMan(), UpnpSSDP::KNotificationDiscover());
    CleanupStack::Pop(); // msg
    
    return msg;
    
    }

// -----------------------------------------------------------------------------
// RUpnpSsdpMessageFactory::SearchL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSsdpSearch* RUpnpSsdpMessageFactory::SearchL( TDesC8& aTarget,
                                           TDesC8& aMaximumWaitTime)
    {
    CUpnpSsdpMessage* msg = CUpnpSsdpMessage::NewL(CUpnpSsdpMessage::ERequestSearch);
    CleanupStack::PushL(msg);
    
    msg->AddMxL(aMaximumWaitTime);	
    msg->AddSearchTargetStringL(aTarget);
    msg->AddHostL((TDesC8&) UpnpSSDP::KDefaultHost());
    
    CleanupStack::Pop(); // msg
    return msg;
    }

// -----------------------------------------------------------------------------
// RUpnpSsdpMessageFactory::ResponseL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSsdpSearchResponse* RUpnpSsdpMessageFactory::ResponseL( TDesC8& aSt,
                                                     TDesC8& aUsn,
                                                     TDesC8& aLoc,
                                                     TDesC8& aServerDesc )
    {
    
    CUpnpSsdpMessage* msg = CUpnpSsdpMessage::NewL(CUpnpSsdpMessage::ERequestSearchResponse);
    CleanupStack::PushL(msg);
    
    msg->AddCacheControlL( (TDesC8&) UpnpSSDP::KDefaultMaxAge);
    // Date and EXT-header will be generated automatically.
    msg->AddLocationL(aLoc);
    msg->AddServerL(aServerDesc);
    msg->AddSearchTargetStringL(aSt);
    msg->AddUsnL(aUsn);
    
    CleanupStack::Pop(); // msg
    
    return msg;
    
    }

//  End of File  
