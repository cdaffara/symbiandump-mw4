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
*      Declaration of class RFavouritesSrvDbNotifier.
*      
*
*/


#ifndef FAVOURITES_SRV_DB_NOTIFIER_H
#define FAVOURITES_SRV_DB_NOTIFIER_H

//  INCLUDES

#include <e32base.h>
#include <d32dbms.h>

// FORWARD DECLARATIONS

class CFavouritesSrvDb;

// CLASS DECLARATION

/**
* Server side change notifier for the Favourites Engine.
*/
NONSHARABLE_CLASS(CFavouritesSrvDbNotifier): public CActive
    {
    public:     // construct / destruct

        /**
        * Two phase constructor. Leaves on failure.
        * @param aDb The database to be watched.
        */
        static CFavouritesSrvDbNotifier* NewL( CFavouritesSrvDb& aDb );

        /**
        * Destructor.
        */
        virtual ~CFavouritesSrvDbNotifier();

    protected:  // construct

        /**
        * Constructor. Leaves on failure.
        */
        CFavouritesSrvDbNotifier();

        /**
        * Second phase constructor. Leaves on failure.
        * @param aDb The database to be watched.
        */
        void ConstructL( CFavouritesSrvDb& aDb );

    protected:  // from CActive

        /**
        * Invoked when the outstanding request completes.
        */
        void RunL();

        /**
        * Implementation of the Cancel protocol;
        * cancel any outstanding request.
        */
        void DoCancel();

    public:     // new methods

        /**
        * Request notification about all database events.
        * @param aMessage Message.
        */
        void NotifyAllEvents( const RMessage2& aMessage );

        /**
        * Request notification about database changes.
        * @param aMessage Message.
        */
        void NotifyChange( const RMessage2& aMessage );

    private:        // Data

        RDbNotifier iNotifier;  ///< DBMS notifier object.
        RMessage2 iMessage;     ///< The message.
    };

#endif
            
// End of File
