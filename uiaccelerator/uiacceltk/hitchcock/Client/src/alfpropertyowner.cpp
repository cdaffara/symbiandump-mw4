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



#include "alf/alfpropertyowner.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfPropertyOwner::CAlfPropertyOwner()
    {
        
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfPropertyOwner::~CAlfPropertyOwner()
    {
    PropertyClear();    
    }

// ---------------------------------------------------------------------------
// Remove property
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CAlfPropertyOwner::PropertyRemove(const TDesC8& aName)
    {
    TInt index;
    
    if (!Find(aName, &index))
        {
        return EFalse;            
        }
    
    delete iProperties[index];
    iProperties.Remove(index);
    iProperties.Compress();
    
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// Clear property
// ---------------------------------------------------------------------------
//         
EXPORT_C void CAlfPropertyOwner::PropertyClear()
    {
    iProperties.ResetAndDestroy();   
    }


// We have a different registration method for each property type.

// ---------------------------------------------------------------------------
// Set integer property
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CAlfPropertyOwner::PropertySetIntegerL(const TDesC8& aName, TInt aVal, TInt aMin, TInt aMax)
    {
    CAlfPropertyInteger* property;
    TAlfPropertyType type;
    TInt index;
    
    if (aName.Length() == 0)
        {
        // Cannot have zero-length name as an argument.
        return EFalse;
        }    

    if (Find(aName, &index, &type))
        {
        // Property exists already.
        if (type != EAlfPropertyTypeInteger)
            {
            // Wrong type. We have to exit without doing anything.
            return EFalse;
            }
        
        // Modify existing property.
        property = static_cast<CAlfPropertyInteger*>(iProperties[index]);                
        property->Set(TRangedValue(aVal, aMin, aMax));        
        }
    else
        {
        // Not found, so add new item to the property list.
        property = new (ELeave) CAlfPropertyInteger();
        CleanupStack::PushL(property);
        property->ConstructL(aName);         
        CleanupStack::Pop(property);
        
        property->Set(TRangedValue(aVal, aMin, aMax));        
        
        User::LeaveIfError(iProperties.Append(property));        
        }
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Find integer property
// ---------------------------------------------------------------------------
//         
EXPORT_C TBool CAlfPropertyOwner::PropertyFindInteger(const TDesC8& aName, TInt* aValue) const
    {   
    TInt index;

    if (!FindOfType(aName, &index, EAlfPropertyTypeInteger))
        {
        return EFalse;            
        }
        
    if (aValue)
        {
        // Clamp the value.
        CAlfPropertyInteger* foundVal = static_cast<CAlfPropertyInteger*>(iProperties[index]);        
        
        *aValue = Min(foundVal->Data().iValue, foundVal->Data().iMax);
        *aValue = Max(*aValue, foundVal->Data().iMin);
        }
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Find complete ranged value
// ---------------------------------------------------------------------------
//         
TBool CAlfPropertyOwner::PropertyFindInteger(const TDesC8& aName, TRangedValue& aValue) const
    {   
    TInt index;

    if (FindOfType(aName, &index, EAlfPropertyTypeInteger))
        {
        CAlfPropertyInteger* foundVal = static_cast<CAlfPropertyInteger*>(iProperties[index]);        
        
        aValue = foundVal->Data();
        return ETrue;
        }
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Set string property
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CAlfPropertyOwner::PropertySetStringL(const TDesC8& aName, const TDesC& aValue)
    {
    CAlfPropertyString* property;
    TAlfPropertyType type;
    TInt index;
    
    if (aName.Length() == 0)
        {
        // Cannot have zero-length name as an argument.
        return EFalse;
        }    

    if (Find(aName, &index, &type))
        {
        // Property exists already.
        if (type != EAlfPropertyTypeString)
            {
            // Wrong type. We have to exit without doing anything.
            return EFalse;
            }
        
        // Modify existing property.
        property = static_cast<CAlfPropertyString*>(iProperties[index]);                
        property->SetL(aValue);        
        }
    else
        {
        // Not found, so add new item to the property list.
        property = new (ELeave) CAlfPropertyString();
        CleanupStack::PushL(property);
        property->ConstructL(aName);         
        CleanupStack::Pop(property);
        
        property->SetL(aValue);        
        
        User::LeaveIfError(iProperties.Append(property));        
        }
    
    return ETrue;
    }
    
    
// ---------------------------------------------------------------------------
// Find string property
// ---------------------------------------------------------------------------
//         
EXPORT_C TBool CAlfPropertyOwner::PropertyFindString(const TDesC8& aName, TDesC** aValue) const
    {   
    TInt index;

    if (!FindOfType(aName, &index, EAlfPropertyTypeString))
        {
        return EFalse;            
        }
    
    CAlfPropertyString* foundVal = static_cast<CAlfPropertyString*>(iProperties[index]);
    
    if (aValue)
        {
        *aValue = foundVal->Data();
        }
    
    return ETrue;
    }    
    
    
// Private methods

// ---------------------------------------------------------------------------
// Find property
// ---------------------------------------------------------------------------
//     
TBool CAlfPropertyOwner::Find(const TDesC8& aName, TInt* aIndex, TAlfPropertyType* aType) const
    {
    TIdentityRelation<CAlfProperty> matchName( CAlfProperty::Matches );
    
    if (iProperties.Count() == 0)
        {
        // Array is empty, nothing to find.
        return false;
        }
    
    // Create a temporary property with the name we want to find.
    CAlfProperty* toFind = new CAlfProperty();
    if ( !toFind )
        {
        return EFalse;
        }
        
    TRAPD( err, toFind->ConstructL(aName) );
    if ( err != KErrNone )
        {
        delete toFind;
        return EFalse;
        }

    TInt found = iProperties.Find(toFind, matchName);
    delete toFind;

    if (found == KErrNotFound)
        {
        return EFalse;
        }
       
    CAlfProperty* foundProperty = iProperties[found];       
       
    if (aIndex)
        {
        *aIndex = found;
        }
    
    if (aType)
        {
        *aType = foundProperty->Type();
        }
    
    return ETrue;        
    }

// ---------------------------------------------------------------------------
// Find property and check for the correct type
// ---------------------------------------------------------------------------
//     
TBool CAlfPropertyOwner::FindOfType(const TDesC8& aName, TInt* aIndex, TAlfPropertyType aRequiredType) const
    {
    TAlfPropertyType foundType;
    
    if (!Find(aName, aIndex, &foundType))
        {
        return EFalse;            
        }
        
    if (foundType != aRequiredType)
        {
        return EFalse;            
        }

    // Return, with find index stored in return parameter.
    return ETrue;      
    }

    
EXPORT_C TInt CAlfPropertyOwner::CopyProperty(const CAlfPropertyOwner& aOwner, const TDesC8& aPropertyName)
    {
    TInt index(0), result(KErrNone);
    TAlfPropertyType propertyType;
  
    if (aOwner.Find(aPropertyName, &index, &propertyType))
        {
        switch (propertyType)
            {
            case EAlfPropertyTypeInteger:
                {    
                TRangedValue value;
                aOwner.PropertyFindInteger(aPropertyName, value); // we know it exists
                TRAP(result, PropertySetIntegerL(aPropertyName, value.iValue, value.iMin, value.iMax))
                break;
                }
            case EAlfPropertyTypeString:
                {
                TDesC* string = 0;
                aOwner.PropertyFindString(aPropertyName, &string); // we know it exists
                TRAP(result, PropertySetStringL(aPropertyName, *string))
                break;
                }
            default:
                result = KErrNotSupported;
                break;
            }
        }
    else 
        {
        result = KErrNotFound;
        } 
        
    return result;
    }

EXPORT_C void CAlfPropertyOwner::PropertyOwnerExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    
    }

