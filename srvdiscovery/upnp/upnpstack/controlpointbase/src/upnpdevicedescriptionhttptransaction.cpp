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
* Description:  Definition of CUpnpDeviceDescriptionHttpTransaction
*
*/


#include "upnpdevicedescriptionhttptransaction.h"
#include "upnpcontrolpoint.h"

// ----------------------------------------------------------------------------
// CUpnpDeviceDescriptionHttpTransaction::~CUpnpDeviceDescriptionHttpTransaction
// Destructor.
// ----------------------------------------------------------------------------
//
CUpnpDeviceDescriptionHttpTransaction::~CUpnpDeviceDescriptionHttpTransaction()
    {
    }

// ----------------------------------------------------------------------------
// CUpnpDeviceDescriptionHttpTransaction::NewLC
// Two-phased constructor, that takes ownership of a message and guarantee it's
// destruction even when leave ocures.
// Caller shouldn't place aMsg on cleanupstack before method call
// ----------------------------------------------------------------------------
//
CUpnpDeviceDescriptionHttpTransaction* CUpnpDeviceDescriptionHttpTransaction::NewLC(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint)
    {
    CleanupStack::PushL( aRequest );
    CUpnpDeviceDescriptionHttpTransaction* self =
        new (ELeave) CUpnpDeviceDescriptionHttpTransaction( aRequest, aControlPoint );
    CleanupStack::Pop( aRequest );//now msg is safe it will be deleted from destructor
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpDeviceDescriptionHttpTransaction::CUpnpDeviceDescriptionHttpTransaction
// Constructor.
// ----------------------------------------------------------------------------
//
CUpnpDeviceDescriptionHttpTransaction::CUpnpDeviceDescriptionHttpTransaction(
    CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint ) :
    CUpnpHttpTransaction( aRequest ), iControlPoint( aControlPoint )
    {
    }

// ----------------------------------------------------------------------------
// void CUpnpDeviceDescriptionHttpTransaction::ProcessResponseL()
// Processes response by passing it to control point.
// ----------------------------------------------------------------------------
//
void CUpnpDeviceDescriptionHttpTransaction::ProcessResponseL()
    {
    iControlPoint.ProcessDeviceMessageL( Response() );
    }

//  End of File

