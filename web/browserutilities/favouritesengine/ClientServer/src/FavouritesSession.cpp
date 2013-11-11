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
*      Implementation of class RFavouritesSession
*      
*
*/


// INCLUDE FILES

#include "FavouritesSession.h"
#include "FavouritesSrv.h"
#include "FavouritesVersion.h"
#include "FavouritesDef.h"
#include "FavouritesMsg.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesSession::Version
// ---------------------------------------------------------
//
EXPORT_C TVersion RFavouritesSession::Version()
    {
    return TVersion
        ( KFavouritesMajorVersion, KFavouritesMinorVersion, KFavouritesBuild );
    }

// ---------------------------------------------------------
// RFavouritesSession::Connect
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesSession::Connect()
    {
    TInt err = DoConnect();
    if ( err == KErrNotFound )
        {
        err = FavouritesSrv::Start();
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            ;
            }
        else
            {
            // Deal with race conditions.
            err = DoConnect();
            }
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesSession::ResourceMark
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesSession::ResourceMark()
    {
    (void)SendReceive( TFavouritesMsg( 0, EFavengResourceMarkStart ) );
    }

// ---------------------------------------------------------
// RFavouritesSession::ResourceCheck
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesSession::ResourceCheck()
    {
    (void)SendReceive( TFavouritesMsg( 0, EFavengResourceMarkEnd ) );
    }

// ---------------------------------------------------------
// RFavouritesSession::ResourceCount
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesSession::ResourceCount()
    {
    return SendReceive( TFavouritesMsg( 0, EFavengResourceCount ) );
    }

// ---------------------------------------------------------
// RFavouritesSession::DeleteDatabase
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesSession::DeleteDatabase( const TDesC& aName )
    {
    return SendReceive( TFavouritesMsg( 0, EFavengDeleteDatabase ),
        TIpcArgs( &aName ));
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// RFavouritesSession::__DbgSetAllocFail
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesSession::__DbgSetAllocFail
( RAllocator::TAllocFail aType, TInt aRate )
    {
    TInt type = STATIC_CAST( TInt, aType );
    (void) SendReceive( TFavouritesMsg( 0, EFavengSetHeapFailure ),
        TIpcArgs( type, aRate ) );
    }

#else /* not _DEBUG */

// ---------------------------------------------------------
// RFavouritesSession::__DbgSetAllocFail
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesSession::__DbgSetAllocFail
( RAllocator::TAllocFail /*aType*/, TInt /*aRate*/ )
    {
    }

#endif /* def _DEBUG */

// ---------------------------------------------------------
// RFavouritesSession::DoConnect
// ---------------------------------------------------------
//
TInt RFavouritesSession::DoConnect()
    {
    return CreateSession( KFavouritesSrvName, Version() );
    }

//  End of File  
