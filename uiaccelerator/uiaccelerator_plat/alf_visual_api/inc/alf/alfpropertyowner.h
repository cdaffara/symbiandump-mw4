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
* Description:   Property object for Alfred
*
*/



#ifndef C_ALFPROPERTYOWNER_H
#define C_ALFPROPERTYOWNER_H

#include <e32base.h>

#include "alf/alfproperty.h"

// Property extension UIDs

static const TUid KUidAlfPropOwnerExtControlFocusChainChanged = {0x200110DA};


/**
 *  Value property object base class
 *
 *  Manages a list of properties.
 *  The current policy is that properties must always return a valid pointer, never NULL.
 * @todo more
 *
 */
class CAlfPropertyOwner: public CBase
    {
    public:
    /* Constructor and destructor. */
    IMPORT_C CAlfPropertyOwner();
    
    IMPORT_C ~CAlfPropertyOwner();    

    /**
     * Remove the property by name, if it exists.
     *
     * @param aName the name of the property to remove.
     * @return ETrue if found.
     */        
    IMPORT_C TBool PropertyRemove(const TDesC8& aName);
    
    /**
     * Remove all properties.
     */            
    IMPORT_C void PropertyClear();


    // We have a different registration method for each property type, eg value, string, timedvalue.

    /**
     * Set a value property.
     * This will set a property of type integer. If the property is already set,
     * it will update the property's value. If not, it will create a new property.
     * Can be overridden to provide custom handling of properties.
     *
     * @param aName the name of the property to set.
     * @param aVal The value of the property.
     * @return EFalse if value was set as a different type already, or zero-length name supplied.
     *         The property was not set.
     */    
    IMPORT_C TBool PropertySetIntegerL(const TDesC8& aName, TInt aVal, TInt aMin = INT_MIN, TInt aMax = INT_MAX);
    
    /**
     * Find the property by name.
     * Can be overridden to provide custom handling of properties.     
     *
     * @param aName the name of the property to find.
     * @param aRet Return parameter for the found value, or NULL if no value required.
     * @return ETrue if found.
     */    
    IMPORT_C TBool PropertyFindInteger(const TDesC8& aName, TInt* aRet = NULL) const;

    /**
     * Set a string property.
     * This will set a property of type string. If the property is already set,
     * it will update the property's value. If not, it will create a new property.
     * Can be overridden to provide custom handling of properties.
     *
     * @param aName the name of the property to set.
     * @param aVal The value of the property.
     * @return EFalse if value was set as a different type already, or zero-length name supplied.
     *         The property was not set.
     */    
    IMPORT_C TBool PropertySetStringL(const TDesC8& aName, const TDesC& aVal);
    
    /**
     * Find the property by name.
     * Can be overridden to provide custom handling of properties.     
     *
     * @param aName the name of the property to find.
     * @param aRet Return parameter for the found value, or NULL if no value required.
     * @return ETrue if found.
     */    
    IMPORT_C TBool PropertyFindString(const TDesC8& aName, TDesC** aRet = NULL) const;

    /**
    * Copies named property from another object
    * 
    * @param aOwner        Object which has the property to be copied
    * @param aPropertyName The name of a property
    *
    * @return System wide error codes
    */
    IMPORT_C TInt CopyProperty(const CAlfPropertyOwner& aOwner, const TDesC8& aPropertyName);
    
    private:
    
    /**
     * Find a generic property object.
     *
     * @param aName the name of the property to find.     
     * @param aIndex Return parameter for index found.
     * @param aType Return parameter for type found.     
     * @return ETrue if found.
     */
    TBool Find(const TDesC8& aName, TInt* aIndex = NULL, TAlfPropertyType* aType = NULL) const;
    
    /**
     * Find a property object of given type.
     *
     * Does type checking to ensure that we got the expected property type.
     *
     * @param aName the name of the property to find.     
     * @param aIndex Return parameter for index found.
     * @param aRequiredType Type that is expected
     * @return ETrue if found and correct type, EFalse if not found or wrong type.
     */    
    TBool FindOfType(const TDesC8& aName, TInt* aIndex, TAlfPropertyType aRequiredType) const;
    
    /**
    * !Internal util: Find complete ranged value.
    */
    TBool PropertyFindInteger(const TDesC8& aName, TRangedValue& aValue) const;

protected: // new methods    
    
    IMPORT_C virtual void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private: // data
    
    RPointerArray<CAlfProperty> iProperties;
    
    TAny* iSpare;
    };

#endif // C_ALFPROPERTYOBJECT_H

