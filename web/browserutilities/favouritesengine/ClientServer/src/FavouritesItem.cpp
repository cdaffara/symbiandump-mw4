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
*       Implementation of class CFavouritesItem
*       
*
*/


// INCLUDE FILES

#include "FavouritesItem.h"
#include "FavouritesItemImpl.h"
#include "FavouritesPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem* CFavouritesItem::NewLC()
    {
    CFavouritesItem* item = new (ELeave) CFavouritesItem();
    CleanupStack::PushL( item );
    item->ConstructL();
    return item;
    }

// ---------------------------------------------------------
// CFavouritesItem::NewL
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem* CFavouritesItem::NewL()
    {
    CFavouritesItem* item = CFavouritesItem::NewLC();
    CleanupStack::Pop();    // item
    return item;
    }

// ---------------------------------------------------------
// CFavouritesItem::~CFavouritesItem
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem::~CFavouritesItem()
    {
    delete iImpl;
    }

// ---------------------------------------------------------
// CFavouritesItem::operator=
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem& CFavouritesItem::operator=
( const CFavouritesItem& aCopyFrom )
    {
    if(&aCopyFrom != this)
        {
        *iImpl = *aCopyFrom.iImpl;
        }

    return *this;
    }

// ---------------------------------------------------------
// CFavouritesItem::Uid() const
// ---------------------------------------------------------
//
EXPORT_C TInt CFavouritesItem::Uid() const
    {
    return iImpl->Uid();
    }

// ---------------------------------------------------------
// CFavouritesItem::ParentFolder
// ---------------------------------------------------------
//
EXPORT_C TInt CFavouritesItem::ParentFolder() const
    {
    return iImpl->ParentFolder();
    }

// ---------------------------------------------------------
// CFavouritesItem::Type
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem::TType CFavouritesItem::Type() const
    {
    return iImpl->Type();
    }

// ---------------------------------------------------------
// CFavouritesItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TPtrC CFavouritesItem::Name() const
    {
    return TPtrC( iImpl->Name() );
    }

// ---------------------------------------------------------
// CFavouritesItem::Url
// ---------------------------------------------------------
//
EXPORT_C const TPtrC CFavouritesItem::Url() const
    {
    return TPtrC( iImpl->Url() );
    }

// ---------------------------------------------------------
// CFavouritesItem::WapAccessPoint
// ---------------------------------------------------------
//
EXPORT_C TFavouritesWapAp CFavouritesItem::WapAp() const
    {
    return iImpl->WapAp();
    }

// ---------------------------------------------------------
// CFavouritesItem::UserName
// ---------------------------------------------------------
//
EXPORT_C const TPtrC CFavouritesItem::UserName() const
    {
    return TPtrC( iImpl->UserName() );
    }

// ---------------------------------------------------------
// CFavouritesItem::Password
// ---------------------------------------------------------
//
EXPORT_C const TPtrC CFavouritesItem::Password() const
    {
    return TPtrC( iImpl->Password() );
    }

// ---------------------------------------------------------
// CFavouritesItem::ContextId
// ---------------------------------------------------------
//
EXPORT_C TInt32 CFavouritesItem::ContextId() const
    {
    return iImpl->ContextId();
    }

// ---------------------------------------------------------
// CFavouritesItem::IsItem
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::IsItem() const
    {
    return iImpl->IsItem();     // TODO keep this?
    }

// ---------------------------------------------------------
// CFavouritesItem::IsFolder
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::IsFolder() const
    {
    return iImpl->IsFolder();   // TODO keep this?
    }

// ---------------------------------------------------------
// CFavouritesItem::IsFactoryItem
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::IsFactoryItem() const
    {
    return iImpl->IsFactoryItem();
    }

// ---------------------------------------------------------
// CFavouritesItem::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::IsReadOnly() const
    {
    return iImpl->IsReadOnly();
    }

// ---------------------------------------------------------
// CFavouritesItem::Modified
// ---------------------------------------------------------
//
EXPORT_C TTime CFavouritesItem::Modified() const
    {
    return iImpl->Modified();
    }


// ---------------------------------------------------------
// CFavouritesItem::IsHidden
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::IsHidden() const
    {
    return iImpl->IsHidden();
    }

// ---------------------------------------------------------
// CFavouritesItem::ClearL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::ClearL()
    {
    CFavouritesItemImpl* impl = CFavouritesItemImpl::NewL();
    delete iImpl;
    iImpl = impl;
    }

// ---------------------------------------------------------
// CFavouritesItem::SetParentFolder
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetParentFolder( TInt aId )
    {
    iImpl->SetParentFolder( aId );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetType
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetType( CFavouritesItem::TType aType )
    {
    iImpl->SetType( aType );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetNameL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetNameL( const TDesC& aName )
    {
    iImpl->SetNameL( aName );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetUrlL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetUrlL( const TDesC& aUrl )
    {
    iImpl->SetUrlL( aUrl );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetWapAccessPoint
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetWapAp( const TFavouritesWapAp& aAccessPoint )
    {
    iImpl->SetWapAp( aAccessPoint );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetUserNameL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetUserNameL( const TDesC& aUserName )
    {
    iImpl->SetUserNameL( aUserName );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetPasswordL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetPasswordL( const TDesC& aPassword )
    {
    iImpl->SetPasswordL( aPassword );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetContextId
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItem::SetContextId( TInt32 aContextId )
    {
    iImpl->SetContextId( aContextId );
    }

// ---------------------------------------------------------
// CFavouritesItem::SetHidden
// ---------------------------------------------------------
//
EXPORT_C TBool CFavouritesItem::SetHidden(TBool aHidden) const
    {
    iImpl->SetHidden( aHidden );
    }

// ---------------------------------------------------------
// CFavouritesItem::CFavouritesItem
// ---------------------------------------------------------
//
CFavouritesItem::CFavouritesItem()
    {
    }

// ---------------------------------------------------------
// CFavouritesItem::ConstructL
// ---------------------------------------------------------
//
void CFavouritesItem::ConstructL()
    {
    iImpl = CFavouritesItemImpl::NewL();
    }

// ---------------------------------------------------------
// CFavouritesItem::ExternalizeL
// ---------------------------------------------------------
//
void CFavouritesItem::ExternalizeL( RWriteStream& aStream ) const
    {
    iImpl->ExternalizeL( aStream );
    }

// ---------------------------------------------------------
// CFavouritesItem::InternalizeL
// ---------------------------------------------------------
//
void CFavouritesItem::InternalizeL( RReadStream& aStream )
    {
    iImpl->InternalizeL( aStream );
    }

//  End of File  
