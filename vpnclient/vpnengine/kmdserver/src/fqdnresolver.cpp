/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors the completion of FQDN resolve.
*
*/


// INTERNAL INCLUDES
#include "vpnconnection.h"

// CLASS HEADER
#include "fqdnresolver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFqdnResolver* CFqdnResolver::NewL( CVpnConnection& aConnection,
                                    MFqdnResolverCallback& aCallback )
    {
    CFqdnResolver* self = new (ELeave) CFqdnResolver( aConnection, aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFqdnResolver::~CFqdnResolver()
    {   
    Cancel();
    
    __ASSERT_DEBUG( iFqdn == NULL, User::Invariant() );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFqdnResolver::CFqdnResolver( CVpnConnection& aConnection,
                              MFqdnResolverCallback& aCallback )
 : CActive( EPriorityStandard ),
   iConnection( aConnection ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Resolves an IP address from FQDN address asynchronously.
// ---------------------------------------------------------------------------
//
void CFqdnResolver::ResolveAddress( HBufC* aFqdn )
    {
    iFqdn = aFqdn;
    iConnection.ResolveAddress( *iFqdn, iNameEntry, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous FQDN address resolving. 
// ---------------------------------------------------------------------------
//
void CFqdnResolver::RunL()
    {
    delete iFqdn;
    iFqdn = NULL;
    
    iCallback.AddressResolveCompleted( iStatus.Int(), iNameEntry );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Cancels FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CFqdnResolver::DoCancel()
    {    
    iConnection.CancelResolveAddress();
    
    delete iFqdn;
    iFqdn = NULL;    
    }
