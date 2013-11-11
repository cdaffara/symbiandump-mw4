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
*      Inline method definitions of class RFavouritesFilter.   
*      
*
*/


#ifndef FAVOURITES_FILTER_INL
#define FAVOURITES_FILTER_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesFilter::TFavouritesFilter()
// ---------------------------------------------------------
//
TFavouritesFilter::TFavouritesFilter()
: iParentFolder( KFavouritesNullUid ),
  iType( CFavouritesItem::ENone ),
  iName( NULL ),
  iContextId( KFavouritesNullContextId )
    {
    }

// ---------------------------------------------------------
// TFavouritesFilter::TFavouritesFilter()
// ---------------------------------------------------------
//
TFavouritesFilter::TFavouritesFilter
        (
        TInt aParentFolder,
        CFavouritesItem::TType aType,
        const TDesC* aName,
        TInt32 aContextId
        )
: iParentFolder( aParentFolder ),
  iType( aType ),
  iName( aName ),
  iContextId( aContextId )
    {
    }

#endif

// End of File
