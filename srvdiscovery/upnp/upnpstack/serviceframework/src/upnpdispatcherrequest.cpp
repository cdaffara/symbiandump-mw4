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
* Description:  Defines CUpnpdispacherrequest class
*
*/


// INCLUDE FILES
#include <badesca.h>
#include "upnpdispatcherrequest.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::CUpnpDispatcherRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherRequest::CUpnpDispatcherRequest( TMessageHandlerRqst aRequest )
: iRequest(aRequest)
    {
    }
// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherRequest* CUpnpDispatcherRequest::NewLC( TMessageHandlerRqst aRequest )
    {
    CUpnpDispatcherRequest* self = new (ELeave) CUpnpDispatcherRequest( aRequest );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherRequest::ConstructL()
    {
    const TInt KDefaultGranurality(8);
    iArguments = new (ELeave) CDesC8ArrayFlat( KDefaultGranurality );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::~CUpnpDispatcherRequest
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherRequest::~CUpnpDispatcherRequest()
    {
    delete iArguments;
    delete iServices;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::AddArgumentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherRequest::AddArgumentL(const TDesC8& aArgument)
    {
    iArguments->AppendL( aArgument );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::AddServices
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherRequest::AddServices(CDesC8ArrayFlat* aArgument)
    {
    delete iServices;
    iServices = aArgument;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::Arguments
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesC8ArrayFlat& CUpnpDispatcherRequest::Arguments()
    {
    return *iArguments;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::Services
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesC8ArrayFlat& CUpnpDispatcherRequest::Services()
    {
    return *iServices;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherRequest::Request
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TMessageHandlerRqst CUpnpDispatcherRequest::Request()
    {
    return iRequest;
    }

//  End of File  
