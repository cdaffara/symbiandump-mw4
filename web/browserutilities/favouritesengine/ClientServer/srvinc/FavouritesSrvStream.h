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
*      Declaration of class HFavouritesSrvStream.
*      
*
*/


#ifndef FAVOURITES_SRV_STREAM_H
#define FAVOURITES_SRV_STREAM_H

//  INCLUDES

#include <e32base.h>
#include <s32buf.h>

// FORWARD DECLARATION

class RMessage2;

// CLASS DECLARATION

/**
* Server side stream object.
*/
NONSHARABLE_CLASS(HFavouritesSrvStream)
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        * @param aHost Stream buffer hosting the stream. Ownership taken.
        */
        inline HFavouritesSrvStream( MStreamBuf& aHost );

        /**
        * Destructor. Releases the stream buffer.
        */
        inline ~HFavouritesSrvStream();

        /**
        * Read data from host to client.
        * @param aMessage Message.
        */
        TInt ReadL( const RMessage2& aMessage );

        /**
        * Write data from client to host.
        * @param aMessage Message.
        */
        void WriteL( const RMessage2& aMessage );

        /**
        * Get the stream buffer.
        */
        inline MStreamBuf& Host();

    private:    // Data

        MStreamBuf& iHost;  ///< Stream buffer hosting the stream.
    };

#include "FavouritesSrvStream.inl"

#endif
            
// End of File
