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
*      Declaration of class RFavouritesSrvDbIncremental.
*      
*
*/


#ifndef FAVOURITES_SRV_DB_INCREMENTAL_H
#define FAVOURITES_SRV_DB_INCREMENTAL_H

//  INCLUDES

#include <e32base.h>
#include <d32dbms.h>

// FORWARD DECLARATIONS

class CFavouritesSrvDb;

// CLASS DECLARATION

/**
* Server side incremental object for Favourites Engine.
*/
NONSHARABLE_CLASS(CFavouritesSrvDbIncremental): public CBase
    {
    public:     // construct / destruct

        /**
        * Constructor.
        */
        inline CFavouritesSrvDbIncremental();

        /**
        * Destructor.
        */
        inline virtual ~CFavouritesSrvDbIncremental();

    public:     // new methods

        /**
        * Start incremental recovery on the given database.
        * @param aDb The database to be recovered.
        * @param aStep Placeholder for the step counter to be returned.
        */
        void RecoverL( CFavouritesSrvDb& aDb, TInt& aStep );

        /**
        * Start incremental compaction on the given database.
        * @param aDb The database to be compacted.
        * @param aStep Placeholder for the step counter to be returned.
        * @return Error code.
        */
        void CompactL( CFavouritesSrvDb& aDb, TInt& aStep );

        /**
        * Perform next step of incremental operation.
        * @param aStep Step counter.
        * @return Error code.
        */
        void NextL( TInt& aStep );

        /**
        * Get step counter.
        * @return Step.
        */
        inline TInt Step();

    private:        // Data

        RDbIncremental iIncremental;    ///< DBMS incremental object.
        TInt iStep;                     ///< Step.
    };

#include "FavouritesSrvDbIncremental.inl"

#endif
            
// End of File
