/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CFavouritesDbTestObserver
*
*/


// INCLUDE FILES

#include <favouritesdb.h>
#include "favouritesdbtestobserver.h"
//#include "FavouritesDbTester.h"
#include "activefavouritesdbnotifier.h "

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesDbTestObserver::CFavouritesDbTestObserver
// ---------------------------------------------------------
//
CFavouritesDbTestObserver::CFavouritesDbTestObserver
( CFavouritesEngineTest& aTester )
: CActive( EPriorityLow ), iTester( &aTester )
    {
    // Observer has lower priority than the notifier; to make sure we don't
    // miss notification.
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::~CFavouritesDbTestObserver
// ---------------------------------------------------------
//
CFavouritesDbTestObserver::~CFavouritesDbTestObserver()
    {
    Cancel();
    delete iNotifier;
//    iTester->iDb.Close();
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::RunL
// ---------------------------------------------------------
//
void CFavouritesDbTestObserver::RunL()
    {
    CActiveScheduler::Stop();
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::DoCancel
// ---------------------------------------------------------
//
void CFavouritesDbTestObserver::DoCancel()
    {
    if ( iNotifier )
        {
        iNotifier->Cancel();
        }
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::HandleFavouritesDbEventL
// ---------------------------------------------------------
//
void CFavouritesDbTestObserver::HandleFavouritesDbEventL
( RDbNotifier::TEvent aEvent )
    {
    iLastEvent = aEvent;
    iEventCount++;
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::Start
// ---------------------------------------------------------
//
void CFavouritesDbTestObserver::Start()
    {
    iStep = -1; // First call to Next will increment it to 1.
    iLastEvent = 666;
    iEventCount = 0;
    Next();
    }

// ---------------------------------------------------------
// CFavouritesDbTestObserver::Next
// ---------------------------------------------------------
//
void CFavouritesDbTestObserver::Next()
    {
    iStep++;
    iStatus = KRequestPending;
    SetActive();
	TRequestStatus *status = &iStatus;
	User::RequestComplete( status, KErrNone );  // Invoke RunL()
    }

