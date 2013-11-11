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
*      Declaration of class TCodProgress.   
*      
*
*/


#ifndef COD_PROGRESS_H
#define COD_PROGRESS_H

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class MCodLoadObserver;

// CLASS DECLARATION

/**
* Progress tracking.
*/
NONSHARABLE_CLASS( TCodProgress )
    {
    public:     // types
    
        enum TPhase     ///< Progress phases.
            {
            ELoad = 0,  ///< Load phase.
            ENotify,    ///< Notify phase.
            ENumPhases  ///< End marker, do not use.
            };

    public:     // constructor

        /**
        * Constructor.
        * @param aObserver Observer.
        */
        inline TCodProgress( MCodLoadObserver* aObserver );

    public:     // new methods

        /**
        * Set or unset observer (pass NULL to unset).
        * Panics if already set; unsetting more is allowed.
        * @param aObserver Observer to set or NULL to unset.
        */
        void SetObserver( MCodLoadObserver* aObserver );
        
        /**
        * Setup progress.
        * aLoad Expected load bytes.
        * aNotify Expected notify bytes.
        */
        void Setup( TInt aLoad, TInt aNotify );

        /**
        * Start phase.
        * @param aPhase Phase.
        */
        void StartPhaseL( TPhase aPhase );

        /**
        * Data update (incremental).
        * @param aBytes Bytes received.
        */
        void IncrementL( TInt aBytes );

        /**
        * Data update (absolute).
        * @param aBytes Toal bytes received (in this phase).
        */
        void SetAmountL( TInt aBytes );

        /**
        * Set to complete.
        */
        void DoneL();

        /**
        * Get final value.
        * @return Final value.
        */
        inline TInt FinalValue();

        /**
        * Get current value.
        * @return Current value.
        */
        inline TInt CurrentValue();

    private:    // new methods
    
        /**
        * Progress update, notify observer.
        */
        inline void NotifyObserverL();

    private:    // data

        TPhase iPhase;                  ///< Current phase.
        TInt iMax[ENumPhases];          ///< Expected values.
        TInt iCur[ENumPhases];          ///< Realized values.
        MCodLoadObserver* iObserver;    ///< Observer. Not owned.
        
    };

#include "CodProgress.inl"

#endif /* def COD_PROGRESS_H */
