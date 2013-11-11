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
*      Implementation of class RFavouritesDbNotifier
*      
*
*/


// INCLUDE FILES

#include "FavouritesDbNotifier.h"
#include "FavouritesDb.h"
#include "FavouritesMsg.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesDbNotifier::Open
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDbNotifier::Open( RFavouritesDb& aDb )
    {
    return RFavouritesHandle::Open( aDb, EFavengOpenNotifier, TIpcArgs() );
    }

// ---------------------------------------------------------
// RFavouritesDbNotifier::NotifyAllEvents
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDbNotifier::NotifyAllEvents( TRequestStatus& aStatus )
    {
    SendReceive( EFavengNotifyAllEvents, TIpcArgs(), aStatus );
    }

// ---------------------------------------------------------
// RFavouritesDbNotifier::NotifyChange
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDbNotifier::NotifyChange( TRequestStatus& aStatus )
    {
    SendReceive( EFavengNotifyChange, TIpcArgs(), aStatus );
    }

// ---------------------------------------------------------
// RFavouritesDbNotifier::Cancel
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDbNotifier::Cancel()
    {
    (void)SendReceive( EFavengNotifyCancel );
    }

//  End of File  
