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
*      Declaration of class CFavouritesSession
*      
*
*/


#ifndef FAVOURITES_SRV_SESSION_H
#define FAVOURITES_SRV_SESSION_H

//  INCLUDES

#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include "FavouritesSrv.h"

// FORWARD DECLARATION
class TFavouritesHandle;
class TFavouritesSessionEntry;
class MStreamBuf;

// CLASS DECLARATION

/**
* Favourites Engine Server Session.
*/
NONSHARABLE_CLASS(CFavouritesSession) : public CSession2
    {

    public:     // construct / destruct

        /**
        * Two phase constructor. Leaves on failure.
        */
        static CFavouritesSession* NewL();

        /**
        * Destructor.
        */
        virtual ~CFavouritesSession();

    protected:  // construct

        /**
        * Constructor. Leaves on failure.
        */
        CFavouritesSession();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    public:     // From CSession2

        /**
        * Count resources.
        * @return Resource count.
        */
        virtual TInt CountResources();

        /**
        * Service message.
        * @param aMessage Message.
        */
        virtual void ServiceL( const RMessage2& aMessage );

        /**
        * Error in message processing.
        * @param aMessage Message.
        * @param aError Error.
        */
        virtual void ServiceError( const RMessage2& aMessage, TInt aError );

    public:     // New methods

        /**
        * Access the server.
        * @return The server.
        */
        inline CFavouritesSrv& Server() const;

    private:    // Entry / object management: entries.

        /**
        * Create a new entry and return its handle. Possibly allocates.
        * @return Handle for the new empty entry.
        */
        TFavouritesHandle NewHandleL();

        /**
        * Get entry for handle. Can leave with KErrBadHandle.
        * @param aHandle Handle to get entry for.
        * @return The entry for this handle.
        */
        TFavouritesSessionEntry& EntryL( TFavouritesHandle aHandle );

        /**
        * Get entry for handle. Panics server if no such handle.
        * @param aHandle Handle to get entry for.
        * @return The entry for this handle.
        */
        TFavouritesSessionEntry& Entry( TFavouritesHandle aHandle );

        /**
        * Get a new check number.
        * @return New check number.
        */
        TInt NextChkNum();

    private:    // New methods: objects.

        /**
        * Create and open a new stream hosted by aHost and return its handle.
        * @param aHost Stream buffer hosting the stream. Ownership taken.
        * @param aMessage If there is data, length is sent back in arg 3 of
        * this message.
        * @return Handle of new stream.
        */
        TFavouritesHandle NewStreamL
            ( MStreamBuf& aHost, const RMessage2& aMessage );

    private:    // new methods

        /**
        * Get directory for named database.
        * @param aName Database name.
        * @param aParse Directory path returned here.
        */
        void DatabaseDirL( const TDesC& aName, TParse& aParse );

        /**
        * Delete directory with contents. Leaves with KErrInUse if there are
        * any open files.
        * @param aParse Path.
        */
        void DeleteDirL( const TParse& aParse );

    private:    // Data

        RFs iFs;                            ///< File server session handle.
        RDbs iDbs;                          ///< DBMS session handle.
        TFavouritesSessionEntry* iEntries;  ///< Session object entries. Owned.
        TInt iNumEntries;                   ///< Number of session entries.
        TInt iChkNum;                       ///< Check number.
        TParse iParse;                      ///< Misc. usage off stack.

};

#include "FavouritesSrvSession.inl"

#endif

// End of File
