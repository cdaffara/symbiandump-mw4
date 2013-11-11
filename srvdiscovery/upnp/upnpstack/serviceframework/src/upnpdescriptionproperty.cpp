/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines the CUpnpDescriptionProperty class
*
*/


// INCLUDE FILES
#include "upnpdescriptionproperty.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::CUpnpDescriptionProperty
// Default C++ constructor
// -----------------------------------------------------------------------------
//
CUpnpDescriptionProperty::CUpnpDescriptionProperty()
    {         
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpDescriptionProperty* CUpnpDescriptionProperty::NewL( const TDesC8& aName , const TDesC8& aValue )
    {
    CUpnpDescriptionProperty* self = new ( ELeave ) CUpnpDescriptionProperty();
    CleanupStack::PushL( self );
    self->ConstructL( aName, aValue );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpDescriptionProperty::ConstructL( const TDesC8& aName , const TDesC8& aValue )
    {
    iName = aName.AllocL();
    if ( &aValue )
        {
        iValue = aValue.AllocL();
        iValue->Des().TrimAll();        
        }
    else
        {
        iValue = HBufC8::NewMaxL(0);            
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::CUpnpDescriptionProperty
// Destructor of CUpnpDescriptionProperty class
// -----------------------------------------------------------------------------
//
CUpnpDescriptionProperty::~CUpnpDescriptionProperty()
    {
    delete iName;
    delete iValue;
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::SetNameL
// Setter for property name 
// -----------------------------------------------------------------------------
//	
void CUpnpDescriptionProperty::SetNameL( const TDesC8& aName)
    {     
    HBufC8* tmp = aName.AllocL();
    delete iName;
    iName = tmp;
    }	

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::SetValueL
// Setter for property value
// -----------------------------------------------------------------------------
//        
void CUpnpDescriptionProperty::SetValueL(const TDesC8& aValue)
    {     
    HBufC8* tmp = aValue.AllocL();
    delete iValue;
    iValue = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::Name
// Getter for property name 
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpDescriptionProperty::Name()
    {
    return *iName;
    }

// -----------------------------------------------------------------------------
// CUpnpDescriptionProperty::Value
// Getter for property value 
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpDescriptionProperty::Value()
    {
    return *iValue;
    }

// End of File
