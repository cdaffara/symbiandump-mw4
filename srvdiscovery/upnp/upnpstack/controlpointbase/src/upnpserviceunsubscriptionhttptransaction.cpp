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
* Description:  Declaration of CUpnpServiceUnsubscriptionHttpTransaction
*
*/

#include "upnpserviceunsubscriptionhttptransaction.h"
#include "upnpcontrolpoint.h"

// -----------------------------------------------------------------------------
// CUpnpServiceUnsubscriptionHttpTransaction::CUpnpServiceUnsubscriptionHttpTransaction
// -----------------------------------------------------------------------------
//
CUpnpServiceUnsubscriptionHttpTransaction::CUpnpServiceUnsubscriptionHttpTransaction(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint) :
    CUpnpHttpTransaction( aRequest ), iControlPoint( aControlPoint )
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CUpnpServiceUnsubscriptionHttpTransaction::~CUpnpServiceUnsubscriptionHttpTransaction
// -----------------------------------------------------------------------------
//
CUpnpServiceUnsubscriptionHttpTransaction::~CUpnpServiceUnsubscriptionHttpTransaction()
    {
    }
// -----------------------------------------------------------------------------
// CUpnpServiceUnsubscriptionHttpTransaction::NewLC
// Two-phased constructor 
// -----------------------------------------------------------------------------
//
CUpnpServiceUnsubscriptionHttpTransaction* CUpnpServiceUnsubscriptionHttpTransaction::NewLC(
                                CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint)
    {
    CleanupStack::PushL( aRequest );
    CUpnpServiceUnsubscriptionHttpTransaction* self = 
            new (ELeave)CUpnpServiceUnsubscriptionHttpTransaction( aRequest, aControlPoint );
        CleanupStack::Pop( aRequest );
        CleanupStack::PushL( self );
        self->BaseConstructL( );
        return self;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceUnsubscriptionHttpTransaction::ProcessResponseL
// -----------------------------------------------------------------------------
//
void CUpnpServiceUnsubscriptionHttpTransaction::ProcessResponseL()
    {
    iControlPoint.UnSubscriptionResponseReceived();
    }

//end of file    
