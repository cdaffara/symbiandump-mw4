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


#include "upnphttpmoderatedeventtransaction.h"

// -----------------------------------------------------------------------------
// CUpnpHttpModeratedEventTransaction::CUpnpHttpModeratedEventTransaction
// -----------------------------------------------------------------------------
//
CUpnpHttpModeratedEventTransaction::CUpnpHttpModeratedEventTransaction( 
                                                    CUpnpHttpMessage* aRequest,
                                                    MTransactionObserver& aObserver )
    :CUpnpHttpTransaction( aRequest ), iObserver( aObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpHttpModeratedEventTransaction::~CUpnpHttpModeratedEventTransaction
// -----------------------------------------------------------------------------
//
CUpnpHttpModeratedEventTransaction::~CUpnpHttpModeratedEventTransaction()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpModeratedEventTransaction::NewLC
// -----------------------------------------------------------------------------
//
CUpnpHttpModeratedEventTransaction* CUpnpHttpModeratedEventTransaction::NewLC( 
                                            CUpnpHttpMessage* aRequest ,
                                            MTransactionObserver& aObserver )
    {
    CleanupStack::PushL( aRequest );
    CUpnpHttpModeratedEventTransaction* self = 
        new (ELeave) CUpnpHttpModeratedEventTransaction( aRequest, aObserver );
    CleanupStack::Pop( aRequest );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpHttpModeratedEventTransaction::NewL
// -----------------------------------------------------------------------------
//
CUpnpHttpModeratedEventTransaction* 
CUpnpHttpModeratedEventTransaction::NewL( CUpnpHttpMessage* aRequest,
                                          MTransactionObserver& aObserver )
    {
    CUpnpHttpModeratedEventTransaction* self=
            CUpnpHttpModeratedEventTransaction::NewLC( aRequest, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpModeratedEventTransaction::ProcessResponseL
// -----------------------------------------------------------------------------
//
void CUpnpHttpModeratedEventTransaction::ProcessResponseL()
    {
    iObserver.TransactionCompletedL( this );
    }
//end of file
