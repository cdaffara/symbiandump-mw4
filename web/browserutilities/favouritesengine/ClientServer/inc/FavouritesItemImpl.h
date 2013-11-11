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
*      Declaration of class CFavouritesItemImpl.
*      
*
*/


#ifndef FAVOURITES_ITEM_IMPL_H
#define FAVOURITES_ITEM_IMPL_H

//  INCLUDES

#include <e32base.h>
#include <s32strm.h>
#include <FavouritesItem.h>
#include <FavouritesLimits.h>
#include <FavouritesWapAp.h>

// CLASS DECLARATION

/**
* Implementation class behind proxy CFavouritesItem.
*/
NONSHARABLE_CLASS(CFavouritesItemImpl): public CBase
    {

    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed item.
        */
        static CFavouritesItemImpl* NewL();

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed item.
        */
        static CFavouritesItemImpl* NewLC();

        /**
        * Destructor.
        */
        virtual ~CFavouritesItemImpl();

        /**
        * Assignment operator.
        * @param aCopyFrom Assign from this.
        */
        CFavouritesItemImpl& operator=( const CFavouritesItemImpl& aCopyFrom );

    protected:      // Constructors
        
        /**
        * C++ constructor.
        */
        inline CFavouritesItemImpl();

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    public:     // Query

        /**
        * Get unique id of the item.
        * @return The unique id.
        */
        inline TInt Uid() const;

        /**
        * Get the uid of the parent folder of this item.
        * @return The unique id of the parent folder.
        */
        inline TInt ParentFolder() const;

        /**
        * Get type of the item (item or folder).
        * @return The type of the item. It is either EFolder or EItem
        * (never ENone).
        */
        inline CFavouritesItem::TType Type() const;

        /**
        * Get the name of this item.
        * @return The name of this item.
        */
        inline const TDesC& Name() const;

        /**
        * Get the URL of this item.
        * @return The URL of this item.
        */
        inline const TDesC& Url() const;

        /**
        * Get WAP Access Point id associated with this item.
        * @return The WAP Access Point id.
        */
        inline TFavouritesWapAp WapAp() const;

        /**
        * Get the username associated with this item.
        * @return The username.
        */
        inline const TDesC& UserName() const;

        /**
        * Get password associated with this item.
        * @return The password.
        */
        inline const TDesC& Password() const;

        /**
        * Get context id associated with this item.
        * @return The context id.
        */
        inline TInt32 ContextId() const;

        /**
        * Check if this is an item (not folder).
        * @return ETrue if this is an item.
        */
        inline TBool IsItem() const;

        /**
        * Check if this is a folder.
        * @return ETrue if this is a folder.
        */
        inline TBool IsFolder() const;

        /**
        * Check if this is a factory item.
        * @return ETrue if this is a factory item.
        */
        inline TBool IsFactoryItem() const;

        /**
        * Check if this is a read-only in database. Note that ETrue value does
        * not prevent modifying this CFavouritesItemImpl object.
        * @return ETrue if this is read-only in database.
        */
        inline TBool IsReadOnly() const;

        /**
        * Get last modification time (of database entry), universal time.
        * This can be zero if:
        * - Last modification time is not available in database (old
        *   databases); or
        * - Engine does not support this feature.
        * Note that zero means Modified().Int64() == 0, and not
        * Time::NullTTime().
        */
        inline TTime Modified() const;

        /**
        * Check if this is hidden.
        * @return ETrue if this is hidden.
        */
        inline TBool IsHidden() const;
        
    public:     // update

        /**
        * Set parent folder.
        * @param aId The unique id of the parent folder.
        */
        inline void SetParentFolder( TInt aId );

        /**
        * Set item type (item or folder).
        * @param aType The type to be set. ENone cannot be set (if that is
        * specified, it is ignored).
        */
        inline void SetType( CFavouritesItem::TType aType );

        /**
        * Set name of the item. Leading or trailing whitespace is trimmed.
        * Length limit is KFavouritesMaxName (leaves with KErrOverflow).
        * @param aName The name to be set.
        */
        void SetNameL( const TDesC& aName );

        /**
        * Set URL of the item.
        * Length limit is KFavouritesMaxURL (leaves with KErrOverflow).
        * @param aUrl The URL to be set.
        */
        inline void SetUrlL( const TDesC& aUrl );

        /**
        * Set WAP Access Point of the item.
        * @param aAccessPoint The WAP Access point to be set.
        */
        inline void SetWapAp( const TFavouritesWapAp& aAccessPoint );

        /**
        * Set username of the item.
        * Length limit is KFavouritesMaxUserName (leaves with KErrOverflow).
        * @param aUserName The username to be set.
        */
        inline void SetUserNameL( const TDesC& aUserName );

        /**
        * Set password.
        * Length limit is KFavouritesMaxPassword (leaves with KErrOverflow).
        * @param aPassword The password to be set.
        */
        inline void SetPasswordL( const TDesC& aPassword );

        /**
        * Set context id of the item.
        * @param aContextId The context id to be set.
        */
        inline void SetContextId( TInt32 aContextId );
        
        /**
        * Set hidden value of the item.
        * @param aHidden The hidden value to be set.
        */
        inline void SetHidden( TBool aHidden );

    public:     // Streaming

        /**
        * Externalize into a stream.
        * @param aStream The stream to externalize to.
        */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * Internalize from a stream.
        * @param aStream The stream to externalize from.
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize update-delta into a stream.
        * This is a performance oprtimization to reduce data transfer between
        * client and server. When database is updated (using an item), only
        * the changed data is synced back to the item (name, uid,
        * last-mod-time).
        * @param aStream The stream to externalize to.
        */
        void ExternalizeUpdateDeltaL( RWriteStream& aStream ) const;

        /**
        * Internalize update-delta from a stream.
        * This is a performance oprtimization to reduce data transfer between
        * client and server. When database is updated (using an item), only
        * the changed data is synced back to the item (name, uid,
        * last-mod-time).
        * @param aStream The stream to externalize from.
        */
        void InternalizeUpdateDeltaL( RReadStream& aStream );

    private:    // Streaming utils

        /**
        * Externalize descriptor member to a stream.
        * @param aBuf Buffer.
        * @param aStream The stream to externalize to.
        */
        void BufToStreamL( const TDesC& aBuf, RWriteStream& aStream ) const;

        /**
        * Internalize HBufC* member from a stream.
        * @param aBuf Buffer.
        * @param aStream The stream to internalize from.
        */
        void BufFromStreamL( HBufC*& aBuf, RReadStream& aStream );

    private:

        /**
        * Set buffer from string. Leaves for exceeding aMaxLength.
        * @param aBuf Buffer.
        * @param aDesc Descriptor to set from.
        * @param aMaxLength Max length.
        */
        void SetBufL( HBufC*& aBuf, const TDesC& aDesc, TInt aMaxLength );

        /**
        * Set unique id of the item.
        * @param aUid The unique id to be set.
        */
        inline void SetUid( TInt aUid );

        /**
        * Set factory item flag of the item.
        * @param aFactoryItem The factory item flag to be set.
        */
        inline void SetFactoryItem( TBool aFactoryItem );

        /**
        * Set read-only flag of the item.
        * @param aReadOnly The read-only flag to be set.
        */
        inline void SetReadOnly( TBool aReadOnly );

        /**
        * Set last modification time of the item.
        * @param aReadOnly The last modification time to be set.
        */
        inline void SetModified( TTime aModified );

        /**
        * Validate the item.
        * - For all kinds: valid name required.
        * - For items: URL must be present.
        * - For folders: URL, Username, Password must be empty;
        *                WAP Access Point must not be specified.
        * @return ETrue if the item matches the above criteria.
        */
        TBool IsValid() const;

        /**
        * Create a bookmark name from a string.
        * Leading / trailing whitespace is removed. Note that the resulting
        * name may be zero length (if aSource is made up from whitespace only).
        * In this case the return value is KErrNone, though the name is not
        * valid.
        * @param aSource The string to create name from.
        * @param aTarget Resulting name is placed here. Previous content is
        * overwritten. Must be large enough to hold the result;
        * the result's length equals to or less than aSource length (i.e.
        * aSource.Length() is enough for aTarget).
        * @return
        * - KErrOverflow if name was trimmed (because it was too long);
        * - KErrNone otherwise.
        */
        static TInt MakeName( const TDesC& aSource, TDes& aTarget );

        /**
        * Check if aName is a valid bookmark name.
        * @return aName Name to check. Not expecting trailing/leading
        * whitespace or whitespace other than spaces (i.e. should be a name
        * that comes from MakeName()).
        * @return ETrue if the name is valid.
        */
        static TBool IsValidName( const TDesC& aName );

    private:    // Data

        TInt iUid;                      ///< Uid of the item.
        TInt iParentFolder;             ///< Uid of the parent folder.
        CFavouritesItem::TType iType;   ///< Type (EFolder or EItem).
        HBufC* iName;                   ///< Name, owned. Never NULL.
        HBufC* iUrl;                    ///< URL, owned. Never NULL.
        TFavouritesWapAp iWapAp;        ///< WAP Access Point.
        HBufC* iUserName;               ///< Username, owned. Never NULL.
        HBufC* iPassword;               ///< Password, owned. Never NULL.
        TBool iReadOnly;                ///< Read-only flag.
        TBool iFactoryItem;             ///< Factory item flag.
        TInt32 iContextId;              ///< Context id.
        TTime iModified;                ///< Last modification time.
        TBool iHidden;                  ///< hidden flag.

    private:    // Friend classes

        friend class RFavouritesDb;
        friend class CFavouritesSrvDb;
        friend class RFavouritesSrvTable;
        friend class CFavouritesItem;

    };

#endif

#include "FavouritesItemImpl.inl"

// End of File
