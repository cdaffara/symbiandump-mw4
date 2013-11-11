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
* Description:  Definition of CUpnpSoapActionHttpTransaction
*
*/

#include "upnpaction.h"
#include "upnpsoapactionhttptransaction.h"
#include "upnpcontrolpoint.h"


// ----------------------------------------------------------------------------
// CUpnpSoapActionHttpTransaction::~CUpnpSoapActionHttpTransaction
// Destructor.
// ----------------------------------------------------------------------------
//
CUpnpSoapActionHttpTransaction::~CUpnpSoapActionHttpTransaction( )
    {
    }

// ----------------------------------------------------------------------------
// CUpnpSoapActionHttpTransaction::NewLC
// Two-phased constructor, that takes ownership of a message and guarantee it's
// destruction even when leave ocures.
// Caller shouldn't place aMsg on cleanupstack before method call
// ----------------------------------------------------------------------------
//
CUpnpSoapActionHttpTransaction* CUpnpSoapActionHttpTransaction::NewLC(
    CUpnpHttpMessage* aRequest, TInt aActionId, CUpnpControlPoint& aControlPoint )
    {
    CleanupStack::PushL( aRequest );
    CUpnpSoapActionHttpTransaction* self =
        new (ELeave) CUpnpSoapActionHttpTransaction( aRequest, aActionId, aControlPoint );
    CleanupStack::Pop( aRequest );//now msg is safe it will be deleted from destructor
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpSoapActionHttpTransaction::CUpnpSoapActionHttpTransaction
// Constructor.
// ----------------------------------------------------------------------------
//
CUpnpSoapActionHttpTransaction::CUpnpSoapActionHttpTransaction(
    CUpnpHttpMessage* aRequest, TInt aActionId, CUpnpControlPoint& aControlPoint) :
    CUpnpHttpTransaction( aRequest ), iActionId( aActionId ), iControlPoint( aControlPoint )
    {
    }

// ----------------------------------------------------------------------------
// CUpnpSoapActionHttpTransaction::ProcessResponseL()
// Processes response by passing it to control point.
// ----------------------------------------------------------------------------
//
void CUpnpSoapActionHttpTransaction::ProcessResponseL()
    {
    Response()->SetSessionId( iActionId );
    iControlPoint.ProcessActionMessageL( Response() );
    }

//  End of File

