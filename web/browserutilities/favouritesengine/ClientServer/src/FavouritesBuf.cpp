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
*      Implementation of class RFavouritesBuf
*      
*
*/


// INCLUDE FILES

#include "FavouritesBuf.h"
#include "FavouritesMsg.h"
#include "FavouritesPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesBuf::Open
// ---------------------------------------------------------
//
TInt RFavouritesBuf::Open( RFavouritesSession& aSess )
    {
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData );   // Empty.
    return RFavouritesHandle::Open( aSess, EFavengOpenStream, TIpcArgs() );
    }

// ---------------------------------------------------------
// RFavouritesBuf::Open
// ---------------------------------------------------------
//
TInt RFavouritesBuf::Open
( RFavouritesHandle& aHandle, TInt aFunction, TIpcArgs& aArgs )
    {
    iBuf.iLen = 0;
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData );   // Initially empty.
    TPckg<TFavouritesBuf> buf( iBuf );
    aArgs.Set( 3, &buf );
    TInt err = RFavouritesHandle::Open( aHandle, aFunction, aArgs );
    if ( !err && iBuf.iLen > 0 )
        {
        // If aFunction provided data, it is already here.
        SetEnd( ERead, iBuf.iData + iBuf.iLen );
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesBuf::Close()
// ---------------------------------------------------------
//
void RFavouritesBuf::Close()
    {
    // Resolve RFavouritesHandle::Close() vs. TStreamBuf::Close() ambiguity.
    // Mind the order: TStreamBuf::Close calls SynchL -> needs an open buf.
    TStreamBuf::Close();
    RFavouritesHandle::Close();
    }

// ---------------------------------------------------------
// RFavouritesBuf::UnderflowL()
// ---------------------------------------------------------
//
TInt RFavouritesBuf::UnderflowL( TInt /*aMaxLength*/ )
    {
    __ASSERT_DEBUG( Avail( ERead ) == 0, \
        FavouritesPanic( EFavouritesInternal ) );
    return IpcReadL();
    }

// ---------------------------------------------------------
// RFavouritesBuf::OverflowL()
// ---------------------------------------------------------
//
void RFavouritesBuf::OverflowL()
    {
    __ASSERT_DEBUG( Avail( EWrite ) == 0, \
        FavouritesPanic( EFavouritesInternal ) );
    IpcWriteL();
    }

// ---------------------------------------------------------
// RFavouritesBuf::DoSynchL()
// ---------------------------------------------------------
//
void RFavouritesBuf::DoSynchL()
    {
    if ( Lag( ERead ) )
        {
        // Read lag unexpected, we do not support seeking.
        User::Leave( KErrNotSupported );
        }
    IpcWriteL();    // Flush write lag.
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData );
    }

// ---------------------------------------------------------
// RFavouritesBuf::IpcReadL()
// ---------------------------------------------------------
//
TInt RFavouritesBuf::IpcReadL()
    {
	TPtr8 des( iBuf.iData, KFavouritesStreamBufSize );
    TInt len = User::LeaveIfError
        ( SendReceive( EFavengStreamRead, TIpcArgs( &des ) ) );
    SetBuf( EWrite, iBuf.iData, iBuf.iData );
    SetBuf( ERead, iBuf.iData, iBuf.iData + len );
    return len;
    }

// ---------------------------------------------------------
// RFavouritesBuf::IpcWriteL()
// ---------------------------------------------------------
//
void RFavouritesBuf::IpcWriteL()
    {
    if ( Lag( EWrite ) )
        {
        TPtrC8 ptr( iBuf.iData, Lag( EWrite ) );
        User::LeaveIfError
            ( SendReceive( EFavengStreamWrite, TIpcArgs( &ptr ) ) );
        }
    SetBuf( ERead, iBuf.iData, iBuf.iData );
    SetBuf( EWrite, iBuf.iData, iBuf.iData + KFavouritesStreamBufSize );
    }

//  End of File  
