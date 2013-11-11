/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Domain API implementation
*
*/

#include "ictsclientinterface.h"
#include "ictsengine.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIctsClientInterface::CIctsClientInterface
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIctsClientInterface::CIctsClientInterface()
    {
    iEngine =NULL;
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::ConstructL
// ---------------------------------------------------------------------------
//
void CIctsClientInterface::ConstructL( TUint32 aIapId, 
                                       TUint32 aNetworkId,
                                       MIctsObserver& aClient )
    {
    DEBUG("CIctsClientInterface::ConstructL");
    iEngine = CIctsEngine::NewL( aIapId, aNetworkId, aClient );
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CIctsClientInterface* CIctsClientInterface::NewL( TUint32 aIapId,
                                                           TUint32 aNetworkId,
                                                           MIctsObserver& aClient 
                                                           )
    {
    CIctsClientInterface* self = new (ELeave) CIctsClientInterface();
    CleanupStack::PushL( self );    
    self->ConstructL( aIapId, aNetworkId, aClient );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::~CIctsClientInterface()
// ---------------------------------------------------------------------------
//
CIctsClientInterface::~CIctsClientInterface()
    {
    delete iEngine;
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::StartL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIctsClientInterface::StartL()
    {
    DEBUG("CIctsClientInterface::StartL()");
    iEngine->StartL();
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::CancelStartL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIctsClientInterface::CancelStartL()
    {
    DEBUG("CIctsClientInterface::CancelStartL()");
    iEngine->CancelStartL();
    }
    
// ---------------------------------------------------------------------------
// CIctsClientInterface::StartPolling
// ---------------------------------------------------------------------------
//
EXPORT_C void CIctsClientInterface::StartPolling( TInt aTime, TInt aInterval )
    {
    DEBUG("CIctsClientInterface::StartPolling()");
    iEngine->StartPolling( aTime, aInterval );
    }

// ---------------------------------------------------------------------------
// CIctsClientInterface::StopPolling
// ---------------------------------------------------------------------------
//
EXPORT_C void CIctsClientInterface::StopPolling()
    {
    DEBUG("CIctsClientInterface::StopPolling()");
    iEngine->StopPolling();
    }

// End of File
