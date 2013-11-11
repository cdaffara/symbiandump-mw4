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
* Description:  Declaration of class CFavouritesDbTestObserver.
*
*/


#ifndef FAVOURITES_DB_TEST_OBSERVER_H
#define FAVOURITES_DB_TEST_OBSERVER_H

//  INCLUDES

#include <e32base.h>
#include <favouritesdbobserver.h>
#include "favouritesenginetest.h"

// FORWARD DECLARATION

class CActiveFavouritesDbNotifier;

// CLASS DECLARATION

/**
* Favourites Engine test observer.
*/
class CFavouritesDbTestObserver: public CActive, public MFavouritesDbObserver
    {
    public:     // Constructors and destructor
        
        /**
        * Constructor.
        * @param aTester Tester object.
        */
        CFavouritesDbTestObserver( CFavouritesEngineTest& aTester );

        /**
        * Destructor.
        */
        virtual ~CFavouritesDbTestObserver();

    protected:  // from CActive

        /**
        * Run a step of the observer test.
        */
        void RunL();

        /**
        * Cancel protocol implementation.
        */
        void DoCancel();

    public:     // from MFavouritesDbObserver

        /**
        * Act accordingly to database events.
        * @param aEvent Database event.
        */
        void HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent );

    public:     // new methods

        /**
        * Start the test.
        */
        void Start();

        /**
        * Continue the test.
        */
        void Next();

    private:    // data

        CFavouritesEngineTest* iTester;           ///< Tester; not owned.
        CActiveFavouritesDbNotifier* iNotifier; ///< Database notifier.
        TInt iStep;                             ///< Test step.
        TInt iLastEvent;                        ///< Last event occurred.
        TInt iEventCount;                       ///< Count of events.

    };

#endif
            
// End of File
