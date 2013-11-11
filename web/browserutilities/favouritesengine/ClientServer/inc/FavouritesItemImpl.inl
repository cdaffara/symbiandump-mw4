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
*      Implementation of inline methods of class CFavouritesItemImpl.
*
*
*/


#ifndef FAVOURITES_ITEM_IMPL_INL
#define FAVOURITES_ITEM_IMPL_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesItemImpl::CFavouritesItemImpl
// ---------------------------------------------------------
//
CFavouritesItemImpl::CFavouritesItemImpl()
    {
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Uid() const
// ---------------------------------------------------------
//
TInt CFavouritesItemImpl::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::ParentFolder
// ---------------------------------------------------------
//
TInt CFavouritesItemImpl::ParentFolder() const
    {
    return iParentFolder;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Type
// ---------------------------------------------------------
//
CFavouritesItem::TType CFavouritesItemImpl::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Name
// ---------------------------------------------------------
//
const TDesC& CFavouritesItemImpl::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Url
// ---------------------------------------------------------
//
const TDesC& CFavouritesItemImpl::Url() const
    {
    return *iUrl;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::WapAccessPoint
// ---------------------------------------------------------
//
TFavouritesWapAp CFavouritesItemImpl::WapAp() const
    {
    return iWapAp;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::UserName
// ---------------------------------------------------------
//
const TDesC& CFavouritesItemImpl::UserName() const
    {
    return *iUserName;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Password
// ---------------------------------------------------------
//
const TDesC& CFavouritesItemImpl::Password() const
    {
    return *iPassword;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::ContextId
// ---------------------------------------------------------
//
TInt32 CFavouritesItemImpl::ContextId() const
    {
    return iContextId;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsItem
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsItem() const
    {
    return iType == CFavouritesItem::EItem;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsFolder
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsFolder() const
    {
    return iType == CFavouritesItem::EFolder;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsFactoryItem
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsFactoryItem() const
    {
    return iFactoryItem;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsReadOnly
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsReadOnly() const
    {
    return iReadOnly;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::Modified
// ---------------------------------------------------------
//
TTime CFavouritesItemImpl::Modified() const
    {
    return iModified;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsHidden
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsHidden() const
    {
    return iHidden;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetParentFolder
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetParentFolder( TInt aId )
    {
    iParentFolder = aId;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetType
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetType( CFavouritesItem::TType aType )
    {
    if ( aType != CFavouritesItem::ENone )
        {
        iType = aType;
        }
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetUrlL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetUrlL( const TDesC& aUrl )
    {
    SetBufL( iUrl, aUrl, KFavouritesMaxUrl );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetWapAccessPoint
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetWapAp( const TFavouritesWapAp& aAccessPoint )
    {
    iWapAp = aAccessPoint;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetUserNameL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetUserNameL( const TDesC& aUserName )
    {
    SetBufL( iUserName, aUserName, KFavouritesMaxUserName );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetPasswordL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetPasswordL( const TDesC& aPassword )
    {
    SetBufL( iPassword, aPassword, KFavouritesMaxPassword );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetContextId
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetContextId( TInt32 aContextId )
    {
    iContextId = aContextId;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetUid
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetUid( TInt aUid )
    {
    iUid = aUid;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetFactoryItem
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetFactoryItem( TBool aFactoryItem )
    {
    iFactoryItem = aFactoryItem;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetReadOnly
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetReadOnly( TBool aReadOnly )
    {
    iReadOnly = aReadOnly;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetModified
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetModified( TTime aModified )
    {
    iModified = aModified;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetHidden
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetHidden( TBool aHidden )
    {
    iHidden = aHidden;
    }
        

#endif
            
// End of File
