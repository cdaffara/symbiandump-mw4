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
*      Declaration of class RFavouritesBuf.
*      
*
*/


#ifndef FAVOURITES_BUF_H
#define FAVOURITES_BUF_H

//  INCLUDES

#include <e32base.h>
#include <s32buf.h>
#include <FavouritesHandle.h>

// CONSTANTS

/// Stream buffer size.
LOCAL_D const TInt KFavouritesStreamBufSize = 512;

// FORWARD DECLARATION

class RFavouritesSession;

// CLASS DECLARATION

/**
* Client side stream buffer data container.
*/
NONSHARABLE_CLASS(TFavouritesBuf)
    {

    public:     // Data

        TInt iLen;                              ///< Data length.
        TUint8 iData[KFavouritesStreamBufSize]; ///< Data buffer.

    };

/**
* Client side stream buffer. Not seekable.
*/
NONSHARABLE_CLASS(RFavouritesBuf): public RFavouritesHandle, public TStreamBuf
    {

    public:     // Constructors and destructor

        /**
        * Open the object (initially empty).
        * @param aSess Session to be used.
        * @return Error code.
        */
        TInt Open( RFavouritesSession& aSess );

        /**
        * Open this object using other object.
        * @param aHandle Handle used to open this object.
        * @param aFunction Function.
        * @param aArgs Arguments. Arg 3 used here.
        * @return Error code.
        */
        TInt Open
            ( RFavouritesHandle& aHandle, TInt aFunction, TIpcArgs& aArgs );

        /**
        * Close the object.
        */
        void Close();

    private:    // from TStreamBuf

        /**
        * Re-fill the intermediate buffer and reset the start and end points
        * of the read area.
        * @param aMaxLength The maximum amount of data required.
        * @return The amount of data available.
        */
        TInt UnderflowL( TInt aMaxLength );

        /**
        * Empty the intermediate buffer and resets the start and end points
        * of the write area.
        */
        void OverflowL();

        /**
        * Synchronise the stream buffer with the stream. In effect, this
        * ensures that buffered data is delivered to the stream.
        * Write lag flushed.
        * Read lag leaves with KErrNotSupported.
        */
        void DoSynchL();

    private:    // New methods

        /**
        * Read data from server.
        * @return The number of bytes read.
        */
        TInt IpcReadL();

        /**
        * Write data to server.
        */
        void IpcWriteL();

        /**
        * Get read lag (number of bytes transferred from server side but not
        * yet read).
        * @return Read lag.
        */
        inline TInt Lag( TRead aArea ) const;

        /**
        * Get write lag (number of bytes written but not yet transferred to
        * server side).
        * @return Write lag.
        */
        inline TInt Lag( TWrite aArea ) const;

    private:    // Data

       TFavouritesBuf iBuf; ///< Data buffer.
    };

#include "FavouritesBuf.inl"

#endif
            
// End of File
