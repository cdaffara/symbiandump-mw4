/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The client side connection to the FeedsSever.
 *
*/


#ifndef __FEEDSENTITY_H
#define __FEEDSENTITY_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFeedsMap;
class CPackedFeed;
class CPackedFolder;
class CPacked;
class RFeedsInterface;

enum TFeedsEntityType
    {
    EFeed,
    EFolder,
    EFeedsItem,
    EFeedsEnclosure,
    EMiniItem
    };

// DATA TYPES
enum TFeedItemStatus
    {
    EItemStatusUndefined,
    EItemStatusNew,
    EItemStatusUnread,
    EItemStatusRead
    };

// CLASS DECLARATION
class CFeedsEntity : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CFeedsEntity* NewL(RFeedsInterface* aFeedsInterface);

    /**
     * Two-phased constructor.
     * Only called from RFolderItem.  aPackedFolder is adopted by this method.
     */
    static CFeedsEntity* NewL(CPackedFolder* aPackedFolder,
    		RFeedsInterface* aFeedsInterface);

    /**
     * Two-phased constructor.
     * Only called from RFolderItem.  aPackedFolder is adopted by this method.
     */
    static CFeedsEntity* NewL(CPackedFeed* aPackedFeed,
    		RFeedsInterface* aFeedsInterface);

    /**
     * Two-phased constructor.
     */
    static CFeedsEntity* NewFolderL(const TDesC& aTitle, TInt aEntryId, TInt aStatusCode);

    /**
     * Two-phased constructor.
     */
    static CFeedsEntity* NewFeedL(const TDesC& aTitle, const TDesC& aUrl,
            TInt aEntryId, TInt aFeedId, const TTime& aTimestamp, TInt aStatusCode, TInt aFreq,
    		const TInt& aUnreadCount=0);
    		
    /**
     * Two-phased constructor.
     */
     CFeedsEntity* CFeedsEntity::NewMiniItemL();
    		

    /**
     * C++ default constructor.  aPackedFolder is adopted by this method. 
     */
    CFeedsEntity(CPackedFolder* aPackedFolder);

    /**
     * C++ default constructor.  aPackedFolder is adopted by this method. 
     */
    CFeedsEntity(CPackedFeed* aPackedFeed);

    /**
     * C++ default constructor.
     */
    CFeedsEntity();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const CPackedFolder& aPackedFolder);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const CPackedFeed& aPackedFeed);

    /**
     * Destructor
     */
    ~CFeedsEntity();

    /**
     * Sets string Attribute
     *
     *
     * @param aAttribute Enum representing attribute
     * @param aAttributeValue - Attribute value
     * @return void
     */
    void SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue);

    /**
     * Changes Attributes/Property
     *
     *
     * @param aNewValues Map containing new Values
     * @return wheather operation was successful or failed (ETrue/EFalse)
     */
    IMPORT_C TInt ChangeValueL(const CFeedsMap& aNewValues);

    /**
     * Gets string Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
    IMPORT_C TInt GetStringValue(TUint anAttribute, TPtrC& aValue) const;

    /**
     * Gets integer Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
    IMPORT_C TInt GetIntegerValue(TUint anAttribute, TInt& aValue) const;

    /**
     * Gets time Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
    IMPORT_C TInt GetTimeValue(TUint anAttribute, TTime& aValue) const;

    /**
     * Gets type of CFeedsEntity (Where this represents folder/feeds/items/enclosures)
     *
     *
     * @return TFeedsEntityType enum
     */

    IMPORT_C TFeedsEntityType GetType() const;

    /**
     * Gets ID of feeds entity (This ID is actually unique integer value)
     *
     *
     * @return TInt ID
     */
    IMPORT_C TInt GetId() const;

    /**
     * Returns Parent of this feeds entity (NULL if it is root folder)
     *
     *
     * @return CFeedsEntity* (parent)
     */
    IMPORT_C CFeedsEntity* GetParent() const;

    /**
     * Returns RPointerArray containing all children of this CFeedsEntity
     *
     *
     * @return Array containing children.
     */
    IMPORT_C const RPointerArray<CFeedsEntity>& GetChildren() const;

    /**
     * Initialises FeedsInterface pointer
     *
     *
     * @param aFeedIterface 
     * @return void
     */
    void SetFeedInterface(RFeedsInterface* aFeedInterface)
    	{
    	iFeedInterface = aFeedInterface;
    	}

    // Friend classes declration.
    friend class RFeedsInterface;
    friend class CTransaction;

private: // Private data members.
    TInt iId;
    TInt iFeedId;
    TFeedsEntityType iType;
    CFeedsEntity* iParent;
    RPointerArray<CFeedsEntity> iChildren;
    CFeedsMap* iAttrProp;
    CPacked* iPacked;
    RFeedsInterface* iFeedInterface;
    };

#endif	//__FEEDSENTITY_H__



