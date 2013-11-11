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
*      Implementation of class HFavouritesSrvStream
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvStream.h"
#include "FavouritesBuf.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// HFavouritesSrvStream::ReadL()
// ---------------------------------------------------------
//
TInt HFavouritesSrvStream::ReadL( const RMessage2& aMessage )
    {
    TUint8 buf[KFavouritesStreamBufSize];
    TInt len = iHost.ReadL( buf, KFavouritesStreamBufSize );
    if ( len )
        {
        aMessage.WriteL( 0, TPtrC8( buf, len ) );
        }
    return len;
    }

// ---------------------------------------------------------
// HFavouritesSrvStream::WriteL()
// ---------------------------------------------------------
//
void HFavouritesSrvStream::WriteL( const RMessage2& aMessage )
    {
    TBuf8<KFavouritesStreamBufSize> buf;
    aMessage.ReadL( 0, buf );
    iHost.WriteL( buf.Ptr(), buf.Size() );
    }

//  End of File  
