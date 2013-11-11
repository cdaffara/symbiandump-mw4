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
* Description:  Declaration of CUpnpServiceSubscriptionHttpTransaction
*
*/

#include "upnpservicesubscriptionhttptransaction.h"
#include "upnpcontrolpoint.h"

// -----------------------------------------------------------------------------
// CUpnpServiceSubscriptionHttpTransaction::CUpnpServiceSubscriptionHttpTransaction
// -----------------------------------------------------------------------------
//
CUpnpServiceSubscriptionHttpTransaction::CUpnpServiceSubscriptionHttpTransaction(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint) :
    CUpnpHttpTransaction( aRequest ), iControlPoint( aControlPoint ),
    iServiceSessionId( aRequest->SessionId() )
    {
    
    }
// -----------------------------------------------------------------------------
// CUpnpServiceSubscriptionHttpTransaction::~CUpnpServiceSubscriptionHttpTransaction
// -----------------------------------------------------------------------------
//
CUpnpServiceSubscriptionHttpTransaction::~CUpnpServiceSubscriptionHttpTransaction()
    {
    
    }
// -----------------------------------------------------------------------------
// CUpnpServiceSubscriptionHttpTransaction::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpServiceSubscriptionHttpTransaction* CUpnpServiceSubscriptionHttpTransaction::NewLC(
                                   CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint)
    {
    CleanupStack::PushL( aRequest );
    CUpnpServiceSubscriptionHttpTransaction* self = 
        new (ELeave)CUpnpServiceSubscriptionHttpTransaction( aRequest, aControlPoint );
    CleanupStack::Pop( aRequest );
    CleanupStack::PushL( self );
    self->BaseConstructL( );
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpServiceSubscriptionHttpTransaction::ProcessResponseL
// -----------------------------------------------------------------------------
//
void CUpnpServiceSubscriptionHttpTransaction::ProcessResponseL()
    {
    CUpnpHttpMessage* msg = Response();
    msg->SetSessionId( iServiceSessionId );
    iControlPoint.SubscriptionResponseReceivedL( *msg );
    }

//end of file
