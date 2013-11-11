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



#include "alf/alfproperty.h"

CAlfProperty::CAlfProperty()
    {
    
    }

TInt CAlfProperty::CompareByName(const CAlfProperty& aA, const CAlfProperty& aB)
    {
    return aA.Name().CompareC(aB.Name());
    }

TBool CAlfProperty::Matches(const CAlfProperty& aA, const CAlfProperty& aB)
    {
    if (CAlfProperty::CompareByName(aA, aB) == 0)
        {
        return ETrue;
        }
        
    return EFalse;
    }

void CAlfProperty::ConstructL(const TDesC8& aName)
    {
    iName = aName.AllocL();        
    }

const TDesC8& CAlfProperty::Name() const
    {
    ASSERT(iName);
    return *iName;
    }

TAlfPropertyType CAlfProperty::Type() const
    {
    return iType;    
    }

CAlfProperty::~CAlfProperty()
    {
    delete iName;        
    }


// Integer property class

CAlfPropertyInteger::CAlfPropertyInteger()
    {
    iType = EAlfPropertyTypeInteger;
    }

CAlfPropertyInteger::~CAlfPropertyInteger()
    {
        
    }

TRangedValue CAlfPropertyInteger::Data() const
    {
    return iValue;    
    };

void CAlfPropertyInteger::Set(const TRangedValue& aValue)
    {
    iValue = aValue;
    }


// Timed value property class

CAlfPropertyTimedValue::CAlfPropertyTimedValue()
    {
    iType = EAlfPropertyTypeTimedValue;
    }

CAlfPropertyTimedValue::~CAlfPropertyTimedValue()
    {
    }

TAlfTimedValue& CAlfPropertyTimedValue::Data()
    {
    return iTimedValue;    
    }

void CAlfPropertyTimedValue::Set(TAlfTimedValue& aTimedValue)
    {
    iTimedValue = aTimedValue;
    }
    
    
// String property class

CAlfPropertyString::CAlfPropertyString()
    {
    iType = EAlfPropertyTypeString;
    iString = NULL;
    }

CAlfPropertyString::~CAlfPropertyString()
    {
    delete iString;
    }

TDesC* CAlfPropertyString::Data() const
    {
    ASSERT(iString);
    
    return iString;    
    }

void CAlfPropertyString::SetL(const TDesC& aString)
    {
    if (iString)
        {
        delete iString;
        iString = NULL;
        }
    
    iString = aString.AllocL();
    }
