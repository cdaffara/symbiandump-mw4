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
*      Implementation of class TFavouritesSessionEntry and
*                              CFavouritesSessionEntryList
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvSessionEntry.h"
#include "FavouritesSrvDb.h"
#include "FavouritesSrvDbNotifier.h"
#include "FavouritesSrvDbIncremental.h"
#include "FavouritesSrvStream.h"
#include "FavouritesSrvFile.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesSessionEntry::Release()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::Release()
    {
    switch( iType )
        {
        case ENone:
            {
            break;
            }

        case EDatabase:
            {
            delete STATIC_CAST( CFavouritesSrvDb*, iObject );
            break;
            }

        case ENotifier:
            {
            delete STATIC_CAST( CFavouritesSrvDbNotifier*, iObject );
            break;
            }

        case EIncremental:
            {
            delete STATIC_CAST( CFavouritesSrvDbIncremental*, iObject );
            break;
            }

        case EStream:
            {
            delete STATIC_CAST( HFavouritesSrvStream*, iObject );
            break;
            }

        case EFile:
            {
            delete STATIC_CAST( CFavouritesSrvFile*, iObject );
            break;
            }

        default:
            {
            FavouritesPanic( EFavouritesInternal );
            }
        }
    SetNull();
    }

//  End of File  
