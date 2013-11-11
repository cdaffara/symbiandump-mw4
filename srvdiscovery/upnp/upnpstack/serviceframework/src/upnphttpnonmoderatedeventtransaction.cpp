/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements transactions for moderated events
*
*/


#include "upnphttpnonmoderatedeventtransaction.h"

// -----------------------------------------------------------------------------
// CUpnpHttpNonModeratedEventTransaction::CUpnpHttpNonModeratedEventTransaction
// -----------------------------------------------------------------------------
//
CUpnpHttpNonModeratedEventTransaction::CUpnpHttpNonModeratedEventTransaction( 
                                                    CUpnpHttpMessage* aRequest,
                                                    MTransactionObserver& aObserver )
    :CUpnpHttpTransaction( aRequest ), iObserver( aObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpHttpNonModeratedEventTransaction::~CUpnpHttpNonModeratedEventTransaction
// -----------------------------------------------------------------------------
//
CUpnpHttpNonModeratedEventTransaction::~CUpnpHttpNonModeratedEventTransaction()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpNonModeratedEventTransaction::NewLC
// -----------------------------------------------------------------------------
//
CUpnpHttpNonModeratedEventTransaction* CUpnpHttpNonModeratedEventTransaction::NewLC( 
                                                    CUpnpHttpMessage* aRequest,
                                                    MTransactionObserver& aObserver )
    {
    CleanupStack::PushL( aRequest );
    CUpnpHttpNonModeratedEventTransaction* self = 
        new (ELeave) CUpnpHttpNonModeratedEventTransaction( aRequest, aObserver );
    CleanupStack::Pop( aRequest );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpNonModeratedEventTransaction::NewL
// -----------------------------------------------------------------------------
//
CUpnpHttpNonModeratedEventTransaction* CUpnpHttpNonModeratedEventTransaction::NewL( 
                                                    CUpnpHttpMessage* aRequest,
                                                    MTransactionObserver& aObserver )
    {
    CUpnpHttpNonModeratedEventTransaction* self=
            CUpnpHttpNonModeratedEventTransaction::NewLC( aRequest, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpNonModeratedEventTransaction::ProcessResponseL
// -----------------------------------------------------------------------------
//
void CUpnpHttpNonModeratedEventTransaction::ProcessResponseL()
    {
    iObserver.TransactionCompletedL( this );
    }
//end of file
