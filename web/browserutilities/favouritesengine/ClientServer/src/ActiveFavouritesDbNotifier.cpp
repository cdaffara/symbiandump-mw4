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
*      Implementation of CActiveFavouritesDbNotifier.
*      
*
*/


// INCLUDE FILES

#include "ActiveFavouritesDbNotifier.h"
#include "FavouritesDbObserver.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::CActiveFavouritesDbNotifier
// ---------------------------------------------------------
//
EXPORT_C CActiveFavouritesDbNotifier::CActiveFavouritesDbNotifier
( RFavouritesDb& aDb, MFavouritesDbObserver& aObserver )
: CActive( EPriorityStandard ), iDb( aDb ), iObserver( &aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::~CActiveFavouritesDbNotifier
// ---------------------------------------------------------
//
EXPORT_C CActiveFavouritesDbNotifier::~CActiveFavouritesDbNotifier()
    {
    iNotifier.Close();
    Cancel();
    }

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::RunL
// ---------------------------------------------------------
//
EXPORT_C void CActiveFavouritesDbNotifier::RunL()
    {
    if ( iStatus == KErrCancel )
        {
        // Cancelled, do nothing.
        }
    else
        {
        // Otherwise, the event must be one of RDbNotifier::TEvent.
        RDbNotifier::TEvent event = 
            STATIC_CAST( RDbNotifier::TEvent, iStatus.Int() );
        if ( iStatus == RDbNotifier::EClose )
            {
            // Finish if the database is closed.
            iNotifier.Close();
            }
        else
            {
            // Reschedule automatically (as long as the database is open).
            NotifyChange();
            }
        // Call observer *after* the notifier is rescheduled: this way
        // the notifier stays alive even if observer leaves.
        iObserver->HandleFavouritesDbEventL( event );
        }
    }

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::DoCancel
// ---------------------------------------------------------
//
EXPORT_C void CActiveFavouritesDbNotifier::DoCancel()
    {
    iNotifier.Close();
    }

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::Start
// ---------------------------------------------------------
//
EXPORT_C TInt CActiveFavouritesDbNotifier::Start()
    {
    TInt err = iNotifier.Open( iDb );
    if ( !err )
        {
        NotifyChange();
        }
    return err;
    }

// ---------------------------------------------------------
// CActiveFavouritesDbNotifier::NotifyChange
// ---------------------------------------------------------
//
void CActiveFavouritesDbNotifier::NotifyChange()
    {
    if ( !IsActive() )
        {
        iNotifier.NotifyChange( iStatus );
        SetActive();
        }
    }

// End of File
