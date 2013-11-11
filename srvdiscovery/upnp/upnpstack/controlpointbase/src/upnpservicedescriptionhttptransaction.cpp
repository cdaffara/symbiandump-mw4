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
* Description:  Definition of CUpnpServiceDescriptionHttpTransaction
*
*/


#include "upnpservicedescriptionhttptransaction.h"
#include "upnpcontrolpoint.h"

// ----------------------------------------------------------------------------
// CUpnpServiceDescriptionHttpTransaction::~CUpnpServiceDescriptionHttpTransaction
// Destructor.
// ----------------------------------------------------------------------------
//
CUpnpServiceDescriptionHttpTransaction::~CUpnpServiceDescriptionHttpTransaction()
    {
    }

// ----------------------------------------------------------------------------
// CUpnpServiceDescriptionHttpTransaction::NewLC
// Two-phased constructor, that takes ownership of a message and guarantee it's
// destruction even when leave ocures.
// Caller shouldn't place aMsg on cleanupstack before method call
// ----------------------------------------------------------------------------
//
CUpnpServiceDescriptionHttpTransaction* CUpnpServiceDescriptionHttpTransaction::NewLC(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint )
    {
    CleanupStack::PushL( aRequest );
    CUpnpServiceDescriptionHttpTransaction* self =
        new (ELeave) CUpnpServiceDescriptionHttpTransaction( aRequest, aControlPoint );
    CleanupStack::Pop( aRequest );//now msg is safe it will be deleted from destructor
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpServiceDescriptionHttpTransaction::CUpnpServiceDescriptionHttpTransaction
// Constructor.
// ----------------------------------------------------------------------------
//
CUpnpServiceDescriptionHttpTransaction::CUpnpServiceDescriptionHttpTransaction(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint) :
    CUpnpHttpTransaction( aRequest ), iControlPoint( aControlPoint )
    {
    }

// ----------------------------------------------------------------------------
// CUpnpServiceDescriptionHttpTransaction::ProcessResponseL()
// Processes response by passing it to control point.
// ----------------------------------------------------------------------------
//
void CUpnpServiceDescriptionHttpTransaction::ProcessResponseL()
    {
    iControlPoint.ProcessServiceMessageL( Response() );
    }

//  End of File

