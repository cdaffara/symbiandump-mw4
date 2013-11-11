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
*       Implementation of class RFavouritesFile
*       
*
*/


// INCLUDE FILES

#include "FavouritesFile.h"
#include "FavouritesMsg.h"
#include "FavouritesPanic.h"
#include "FavouritesDb.h"
#include <f32file.h>

// CLASS DECLARATION

/**
* Favourites file implementation.
*/
NONSHARABLE_CLASS(CFavouritesFileImpl): public CBase
    {
    public:     // Destructor

        /**
        * Destructor.
        */
        virtual inline ~CFavouritesFileImpl();

    public:     // data

        RFile iFile;    ///< The file.

    };

// ---------------------------------------------------------
// CFavouritesFileImpl::~CFavouritesFileImpl
// ---------------------------------------------------------
//
CFavouritesFileImpl::~CFavouritesFileImpl()
    {
    iFile.Close();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesFile::Close
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesFile::Close()
    {
    delete iImpl;
    iImpl = NULL;
    RFavouritesHandle::Close();
    }

// ---------------------------------------------------------
// RFavouritesFile::Open
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesFile::Open( RFavouritesDb& aDb, TInt aUid )
    {
    return DoOpen( aDb, aUid, EFavengFileOpenRead );
    }

// ---------------------------------------------------------
// RFavouritesFile::Replace
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesFile::Replace( RFavouritesDb& aDb, TInt aUid )
    {
    return DoOpen( aDb, aUid, EFavengFileOpenWrite );
    }

// ---------------------------------------------------------
// RFavouritesFile::Read
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesFile::Read( TDes8& aDes ) const
    {
    __ASSERT_DEBUG( iImpl, FavouritesPanic( EFavouritesNotReady ) );
    return iImpl->iFile.Read( aDes );
    }

// ---------------------------------------------------------
// RFavouritesFile::Write
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesFile::Write( const TDesC8& aDes )
    {
    __ASSERT_DEBUG( iImpl, FavouritesPanic( EFavouritesNotReady ) );
    return iImpl->iFile.Write( aDes );
    }

// ---------------------------------------------------------
// RFavouritesFile::Size
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesFile::Size(TInt &aSize) const
	{
	__ASSERT_DEBUG( iImpl, FavouritesPanic( EFavouritesNotReady ) );
    return iImpl->iFile.Size( aSize );
	}
	
// ---------------------------------------------------------
// RFavouritesFile::DoOpen
// ---------------------------------------------------------
//
TInt RFavouritesFile::DoOpen( RFavouritesDb& aDb, TInt aUid, TInt aFunction )
    {
    __ASSERT_DEBUG( !iImpl, FavouritesPanic( EFavouritesAlreadyOpen ) );
    TInt err;
    iImpl = new CFavouritesFileImpl();
    if ( iImpl )
        {
        err = RFavouritesHandle::Open
            ( aDb, EFavengFileOpen, TIpcArgs( aUid ) );

        if ( !err )
            {
            TPckgBuf<TInt> file;
            
            TInt rfsHandler = RFavouritesHandle::SendReceive( 
            	aFunction, TIpcArgs( aUid, &file ));
            
        	if(rfsHandler >= 0)
        		{
            	err = iImpl->iFile.AdoptFromServer( rfsHandler, file() );
        		}
        	else
        		{
        		err = rfsHandler;
        		}
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    if ( err )
        {
        Close();    // Deletes and NULL-s iImpl.
        }
    return err;
    }

// End of file
