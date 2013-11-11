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
*      Declaration of class HFavouritesSrvBuf.
*      
*
*/


#ifndef FAVOURITES_SRV_BUF_H
#define FAVOURITES_SRV_BUF_H

//  INCLUDES

#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION

/**
* Server side dynamic stream buffer. Hosts streamed data on the server side.
*/
NONSHARABLE_CLASS(HFavouritesSrvBuf): public TBufBuf
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @return The constructed buffer.
        */
        static HFavouritesSrvBuf* NewL();

        /**
        * Two phased constructor. Leaves on failure.
        * @return The constructed buffer.
        */
        static HFavouritesSrvBuf* NewLC();

    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        inline HFavouritesSrvBuf();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual inline ~HFavouritesSrvBuf();

    private:    // From TBufBuf

        /**
        * Release this buffer. Deletes this.
        */
        void DoRelease();

    private:    // Data

        CBufBase* iBuf; ///< The buffer. Owned.

    };

#include "FavouritesSrvBuf.inl"

#endif
            
// End of File
