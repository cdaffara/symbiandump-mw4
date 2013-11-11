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
*       Implementation of class CFavouritesItemImpl
*       
*
*/


// INCLUDE FILES

#include "FavouritesItemImpl.h"
#include "FavouritesPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesItemImpl::NewL
// ---------------------------------------------------------
//
CFavouritesItemImpl* CFavouritesItemImpl::NewL()
    {
    CFavouritesItemImpl* item = NewLC();
    CleanupStack::Pop( item );
    return item;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::NewLC
// ---------------------------------------------------------
//
CFavouritesItemImpl* CFavouritesItemImpl::NewLC()
    {
    CFavouritesItemImpl* item = new (ELeave) CFavouritesItemImpl();
    CleanupStack::PushL( item );
    item->ConstructL();
    return item;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::~CFavouritesItemImpl
// ---------------------------------------------------------
//
CFavouritesItemImpl::~CFavouritesItemImpl()
    {
    delete iName;
    delete iUrl;
    delete iUserName;
    delete iPassword;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::operator=
// ---------------------------------------------------------
//
CFavouritesItemImpl& CFavouritesItemImpl::operator=
( const CFavouritesItemImpl& aCopyFrom )
    {
    if( &aCopyFrom != this )
        {
        iUid = aCopyFrom.iUid;
        iParentFolder = aCopyFrom.iParentFolder;
        iType = aCopyFrom.iType;
        SetNameL( *(aCopyFrom.iName) );
        SetUrlL( *(aCopyFrom.iUrl) );
        iWapAp = aCopyFrom.iWapAp;
        SetUserNameL( *(aCopyFrom.iUserName) );
        SetPasswordL( *(aCopyFrom.iPassword) );
        iFactoryItem = aCopyFrom.iFactoryItem;
        iReadOnly = aCopyFrom.iReadOnly;
        iContextId = aCopyFrom.iContextId;
        iModified = aCopyFrom.iModified;
        iHidden = aCopyFrom.iHidden;
        }
    return *this;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::ConstructL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::ConstructL()
    {
    iUid = KFavouritesNullUid;
    iParentFolder = KFavouritesNullUid;
    iType = CFavouritesItem::EItem;
    iName = HBufC::NewL( 0 );
    iUrl = HBufC::NewL( 0 );
    iWapAp.SetDefault();
    iUserName = HBufC::NewL( 0 );
    iPassword = HBufC::NewL( 0 );
    iFactoryItem = EFalse;
    iReadOnly = EFalse;
    iContextId = KFavouritesNullContextId;
    iModified = Time::NullTTime();
    iHidden = EFalse;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetNameL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetNameL( const TDesC& aName )
    {
    HBufC* buf = HBufC::NewLC( aName.Length() );
    TPtr bufp( buf->Des() );
    User::LeaveIfError( MakeName( aName, bufp ) );
    CleanupStack::Pop( buf );
    delete iName;
    iName = buf;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::ExternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iUid );
    aStream.WriteInt32L( iParentFolder );
    aStream.WriteInt32L( STATIC_CAST( TInt, iType ) );
    BufToStreamL( *iName, aStream );
    BufToStreamL( *iUrl, aStream );
    iWapAp.ExternalizeL( aStream );
    BufToStreamL( *iUserName, aStream );
    BufToStreamL( *iPassword, aStream );
    aStream.WriteInt32L( STATIC_CAST( TInt, iReadOnly ) );
    aStream.WriteInt32L( STATIC_CAST( TInt, iFactoryItem ) );
    aStream.WriteInt32L( iContextId );
    aStream.WriteInt32L( I64HIGH( iModified.Int64() ) );
    aStream.WriteInt32L( I64LOW( iModified.Int64() ) );
    aStream.WriteInt32L( STATIC_CAST( TInt, iHidden ) );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::InternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::InternalizeL( RReadStream& aStream )
    {
    iUid = aStream.ReadInt32L();
    iParentFolder = aStream.ReadInt32L();
    iType = STATIC_CAST( CFavouritesItem::TType, aStream.ReadInt32L() );
    BufFromStreamL( iName, aStream );
    BufFromStreamL( iUrl, aStream );
    iWapAp.InternalizeL( aStream );
    BufFromStreamL( iUserName, aStream );
    BufFromStreamL( iPassword, aStream );
    iReadOnly = STATIC_CAST( TBool, aStream.ReadInt32L() );
    iFactoryItem = STATIC_CAST( TBool, aStream.ReadInt32L() );
    iContextId = aStream.ReadInt32L();
    // Need to use local variables, cannot read to TInt64 constructor directly
    // from stream. Order of argument evaluation is compiler dependent!
    TUint32 high = aStream.ReadInt32L();
    TUint32 low = aStream.ReadInt32L();
    iModified = MAKE_TINT64( high, low );
    iHidden = STATIC_CAST( TBool, aStream.ReadInt32L() );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::ExternalizeUpdateDeltaL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::ExternalizeUpdateDeltaL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iUid );
    BufToStreamL( *iName, aStream );
    aStream.WriteInt32L( I64HIGH( iModified.Int64() ) );
    aStream.WriteInt32L( I64LOW( iModified.Int64() ) );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::InternalizeUpdateDeltaL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::InternalizeUpdateDeltaL( RReadStream& aStream )
    {
    iUid = aStream.ReadInt32L();
    BufFromStreamL( iName, aStream );
    // Need to use local variables, cannot read to TInt64 constructor directly
    // from stream. Order of argument evaluation is compiler dependent!
    TUint32 high = aStream.ReadInt32L();
    TUint32 low = aStream.ReadInt32L();
    iModified = MAKE_TINT64( high, low );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::BufToStreamL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::BufToStreamL
( const TDesC& aBuf, RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( aBuf.Length() );
    aStream.WriteL( aBuf );
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::BufFromStreamL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::BufFromStreamL( HBufC*& aBuf, RReadStream& aStream )
    {
    TInt len = aStream.ReadInt32L();
    HBufC* buf = HBufC::NewLC( len );
    TPtr ptr( buf->Des() ); ptr.SetLength( len );
    aStream.ReadL( ptr, len );
    delete aBuf;
    aBuf = buf;
    CleanupStack::Pop( buf );   // Now member.
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::SetBufL
// ---------------------------------------------------------
//
void CFavouritesItemImpl::SetBufL
( HBufC*& aBuf, const TDesC& aDesc, TInt aMaxLength )
    {
    if ( aDesc.Length() > aMaxLength )
        {
        User::Leave( KErrOverflow );
        }
    HBufC* buf = aDesc.AllocL();
    delete aBuf;
    aBuf = buf;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsValid
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsValid() const
    {
    TBool ret = ETrue;
    if ( !IsValidName( *iName ) )
        {
        // Valid name is required for all (except the root folder).
        ret = EFalse;
        }
    else
        {
        switch ( iType )
            {
            case CFavouritesItem::EItem:
                if ( !iUrl->Length() )
                    {
                    // URL is required for items.
                    ret = EFalse;
                    }
                break;

            case CFavouritesItem::EFolder:
                if (
                    iUrl->Length() ||
                    iUserName->Length() ||
                    iPassword->Length()
                    )
                    {
                    // URL, Username, Password are not allowed
                    // for folders.
                    ret = EFalse;
                    }
                break;

            default:
                {
                FavouritesPanic( EFavouritesBadType );
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::MakeName
// ---------------------------------------------------------
//
TInt CFavouritesItemImpl::MakeName( const TDesC& aSource, TDes& aTarget )
    {
    TInt err( KErrNone );
    _LIT(KEikEdwinRightToLeftMark, "\x200F"	);
    // Copy source to target.
    aTarget = aSource;
    TBool rightRemove, leftRemove;
    rightRemove = EFalse;
    leftRemove = EFalse;
    
    // Remove leading/trailing spaces.
    if(aTarget.Right(1).Compare(KEikEdwinRightToLeftMark) ==0)
    {   
    	aTarget.Delete(aTarget.Length() -1,1);
    	rightRemove = ETrue;	
    }
    
    if(aTarget.Left(1).Compare(KEikEdwinRightToLeftMark) ==0)
    {
    	aTarget.Delete(0,1);
    	leftRemove = ETrue;		
    }
    
    
    aTarget.Trim();
    if(rightRemove)
    {
    	if(aTarget.Right(1).Compare(KEikEdwinRightToLeftMark) ==0)
    	{
    		aTarget.Delete(aTarget.Length() -1,1);	
    	}
    	
    }
    if(leftRemove)
    {
    	if(aTarget.Left(1).Compare(KEikEdwinRightToLeftMark) ==0)
    	{
    		aTarget.Delete(0,1);
    	}	
    }
    
    // Trim to valid length.
    if ( aTarget.Length() > KFavouritesMaxName )
        {
        aTarget.SetLength( KFavouritesMaxName );
        err = KErrOverflow;
        }
    return err;
    }

// ---------------------------------------------------------
// CFavouritesItemImpl::IsValidName
// ---------------------------------------------------------
//
TBool CFavouritesItemImpl::IsValidName( const TDesC& aName )
    {
    // All characters accepted (no exclusion of special characters.).
    // Name must contain at least non-whitespace; since this method expects
    // a trimmed name, this is equivalent of checking if the name is empty.
    // This method always gets a trimmed name (MakeName generated).
    return aName.Length();
    }

//  End of File  
