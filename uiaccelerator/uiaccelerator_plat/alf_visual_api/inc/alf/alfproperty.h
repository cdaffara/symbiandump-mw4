/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Property value for Alfred
*
*/



#ifndef C_ALFPROPERTYVALUE_H
#define C_ALFPROPERTYVALUE_H

#include <e32base.h>
#include <libc/limits.h> // For max and min int sizes.

#include <alf/alftimedvalue.h>


/** Property types. */
enum TAlfPropertyType
    {
    /** Property has no value type. */
    EAlfPropertyTypeUndefined,
    
    /** Property is a string. */
    EAlfPropertyTypeString,
    
    /** Property is a TRangedValue. */    
    EAlfPropertyTypeInteger,
    
    /** Property is a CAlfTimedValue. */
    EAlfPropertyTypeTimedValue
    };

/**
 * A ranged value with specifiable min/max bounds.
 */
class TRangedValue
{
public:
    TRangedValue()
    {
    iValue = 0;
    iMin = INT_MIN;
    iMax = INT_MAX;
    }
    
    TRangedValue(TInt aValue, TInt aMin, TInt aMax)
    :
    iValue(aValue),
    iMin(aMin),
    iMax(aMax)
    {
    }
    
    TInt iValue;
    TInt iMin;
    TInt iMax;
};



/**
 * Property base class
 *
 * A generic property value, it can be derived to create new property types
 * that can be accessed by name.
 *
 * @todo This class should be able to represent a generic property without any type.
 *
 * @see CAlfPropertyOwner
 *
 */
NONSHARABLE_CLASS(CAlfProperty) : public CBase
    {
public:

    CAlfProperty();

    ~CAlfProperty();

    /**
     * Constructs a new empty property.
     *
     * @param aName the name of this property.
     */
    void ConstructL(const TDesC8& aName);

    /**
     * Returns the type of this property.
     *
     * @return Property type.
     * @see TAlfPropertyType
     */
    TAlfPropertyType Type() const;

    /**
     * Returns the name of this property.
     *
     * @return Property name.
     */
    const TDesC8& Name() const;
    
    /**
     * Compare names for sort operations.
     * Does a letter-by-letter compare of property names.
     *
     * @param aA First property.
     * @param aA Second property.     
     * @return difference between names.
     */
    static TInt CompareByName(const CAlfProperty& aA, const CAlfProperty& aB);

    /**
     * Do names match.
     * Compares the names of two properties.
     * @param aA First property.
     * @param aA Second property.     
     * @return ETrue on a match.
     */
    static TBool Matches(const CAlfProperty& aA, const CAlfProperty& aB);
    
protected:
    HBufC8* iName;   // Owned.        
    TAlfPropertyType iType;    
    };

/**
 *  Integer value property base class
 *
 *  A single property containing an integer value and its range.
 *
 */
NONSHARABLE_CLASS(CAlfPropertyInteger) : public CAlfProperty
    {
    public:
    /* Constructors and destructor. */

    CAlfPropertyInteger();

    ~CAlfPropertyInteger();    
    
    void Set(const TRangedValue& aValue);
    
    TRangedValue Data() const;
        
    private:
    TRangedValue iValue;        
    };

/**
 *  Timed value property base class
 *
 *  A single property containing an integer value and its range.
 *
 */
NONSHARABLE_CLASS(CAlfPropertyTimedValue) : public CAlfProperty
    {
    public:
    /* Constructors and destructor. */

    CAlfPropertyTimedValue();

    ~CAlfPropertyTimedValue();    
    
    void Set(TAlfTimedValue& aValue);
    
    TAlfTimedValue& Data();
        
    private:
    TAlfTimedValue iTimedValue;         
    };
    
    
/**
 *  String property base class
 *
 *  A single property containing a string.
 *
 */
NONSHARABLE_CLASS(CAlfPropertyString) : public CAlfProperty
    {
    public:
    /* Constructors and destructor. */

    CAlfPropertyString();

    ~CAlfPropertyString();    
    
    void SetL(const TDesC& aValue);
    
    TDesC* Data() const;
        
    private:
    HBufC* iString;         
    };


#endif // C_ALFPROPERTYVALUE_H
