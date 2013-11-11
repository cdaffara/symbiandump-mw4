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
* Description:  Map to store attributes
*
*/

#ifndef __FEEDS_MAP_H
#define __FEEDS_MAP_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


class CFeedsMap : public CBase
    {
public: // Construct/Destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CFeedsMap* NewL();

    static CFeedsMap* NewLC();

    /**
     * Destructor.
     */
     IMPORT_C ~CFeedsMap();

private:	
    /**
     * C++ default constructor.
     */
    CFeedsMap();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:
    /**
     * Sets string Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - value of the attribute
     * @return KErrNone if successful otherwise symbian specific error
     */
     IMPORT_C TInt SetStringAttribute(TUint anAttribute, const TDesC& aValue);

    /**
     * Sets integer Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - value of the attribute
     * @return KErrNone if successful otherwise symbian specific error
     */
     IMPORT_C TInt SetIntegerAttribute(TUint anAttribute, TInt aValue);

    /**
     * Sets time Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - value of the attribute
     * @return KErrNone if successful otherwise symbian specific error
     */
     IMPORT_C TInt SetTimeAttribute(TUint anAttribute, TTime aValue);

    /**
     * Gets string Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
     IMPORT_C TInt GetStringAttribute(TUint anAttribute, TPtrC& aValue) const;

    /**
     * Gets integer Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
     IMPORT_C TInt GetIntegerAttribute(TUint anAttribute, TInt& aValue) const;

    /**
     * Gets time Attribute
     *
     *
     * @param Enum value representing an attribute
     * @param aValue - returned value of the attribute 
     * @return KErrNone if successful otherwise KErrNotFound
     */
     IMPORT_C TInt GetTimeAttribute(TUint anAttribute, TTime& aValue) const;

    /**
     * Resets Map
     *
     *
     * @return void
     */
     IMPORT_C void ResetL();

    /**
     * Checks whether Given attribute exists in map
     * 
     * 
     * @return ETrue if attribute exits in map
     */
     
     IMPORT_C TBool IsExist(TUint anAttribute) const;

    /**
     * Retuns the count of entries in the map
     *
     * @return Total number of entries in the map
     */
     IMPORT_C TInt Count()const;

    /**
     * Overloaded array operator funtion
     *
     * @return enum value corresponding to an attribute
     */
     IMPORT_C TUint operator[](TInt index)const;


private:
    // Data
    RArray<TUint> iEnums;
    HBufC* iValueString;
    RArray<TInt> iStart;
    RArray<TInt> iSize;
    };

#endif // #ifndef __FEEDS_MAP_H
