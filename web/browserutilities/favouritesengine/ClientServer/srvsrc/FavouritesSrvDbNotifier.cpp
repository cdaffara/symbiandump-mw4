/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class CFavouritesSrvDbNotifier
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvDbNotifier.h"
#include "FavouritesSrvDb.h"
#include "FavouritesPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::NewL
// ---------------------------------------------------------
//
CFavouritesSrvDbNotifier* CFavouritesSrvDbNotifier::NewL
( CFavouritesSrvDb& aDb )
    {
    CFavouritesSrvDbNotifier* notif = new (ELeave) CFavouritesSrvDbNotifier();
    CleanupStack::PushL( notif );
    notif->ConstructL( aDb );
    CleanupStack::Pop( notif );
    return notif;
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::~CFavouritesSrvDbNotifier
// ---------------------------------------------------------
//
CFavouritesSrvDbNotifier::~CFavouritesSrvDbNotifier()
    {
    Cancel();
    iNotifier.Close();
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::CFavouritesSrvDbNotifier
// ---------------------------------------------------------
//
CFavouritesSrvDbNotifier::CFavouritesSrvDbNotifier()
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::ConstructL
// ---------------------------------------------------------
//
void CFavouritesSrvDbNotifier::ConstructL( CFavouritesSrvDb& aDb )
    {
    User::LeaveIfError( iNotifier.Open( aDb.Database() ) );
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::RunL
// ---------------------------------------------------------
//
void CFavouritesSrvDbNotifier::RunL()
    {
    iMessage.Complete( iStatus.Int() );
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::DoCancel
// ---------------------------------------------------------
//
void CFavouritesSrvDbNotifier::DoCancel()
    {
    iNotifier.Cancel();
    iMessage.Complete( KErrCancel );
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::NotifyAllEvents
// ---------------------------------------------------------
//
void CFavouritesSrvDbNotifier::NotifyAllEvents( const RMessage2& aMessage )
    {
    __ASSERT_DEBUG( !IsActive(), FavouritesPanic( EFavouritesInternal ) );
    iMessage = aMessage;
    iNotifier.NotifyUnlock( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CFavouritesSrvDbNotifier::NotifyChange
// ---------------------------------------------------------
//
void CFavouritesSrvDbNotifier::NotifyChange( const RMessage2& aMessage )
    {
    __ASSERT_DEBUG( !IsActive(), FavouritesPanic( EFavouritesInternal ) );
    iMessage = aMessage;
    iNotifier.NotifyChange( iStatus );
    SetActive();
    }

//  End of File  
