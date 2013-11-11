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
* Description:  HTTP header
*
*/



// INCLUDE FILES
#include "upnphttpheader.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::CUpnpHttpHeader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpHttpHeader::CUpnpHttpHeader()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpHttpHeader::ConstructL( const TDesC8& aName, const TDesC8& aValue )
    {
    iName = HBufC8::NewL(aName.Length());
    iName->Des().SetLength(0);
    iName->Des().Append(aName);
    
    iValue = HBufC8::NewL(aValue.Length());
    iValue->Des().SetLength(0);
    iValue->Des().Append(aValue);
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpHeader* CUpnpHttpHeader::NewL(const TDesC8& aName, const TDesC8& aValue)
    {
    CUpnpHttpHeader* self = new (ELeave) CUpnpHttpHeader();
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue);
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::~CUpnpHttpHeader
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpHeader::~CUpnpHttpHeader()
    {
    if (iName)
        {
        delete iName;
        iName = NULL;
        }
    
    if (iValue)
        {
        delete iValue;
        iValue = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::Name
// Returns name of HTTPHeader.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpHttpHeader::Name()
    {
    if ( iName != NULL )
        {
        return *iName;
        }
    else
        {
        return ( TDesC8& ) KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::Value
// Returns value of HTTPHeader.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpHttpHeader::Value()
    {
    if ( iName != NULL )
        {
        return *iValue;
        }
    else
        {
        return ( TDesC8& ) KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::SetNameL
// Set name of HTTPHeader.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpHeader::SetNameL( const TDesC8& aName )
    {
    if ( iName )
        {
        delete iName;
        iName = NULL;
        }
    
    iName = HBufC8::NewL( aName.Length() );
    iName->Des().SetLength( 0 );
    iName->Des().Append(aName);
    }

// -----------------------------------------------------------------------------
// CUpnpHttpHeader::SetValueL
// Set value of HTTPHeader.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpHeader::SetValueL( const TDesC8& aValue )
    {
    if ( iValue )
        {
        delete iValue;
        iValue = NULL;
        }
    
    iValue = HBufC8::NewL( aValue.Length() );
    iValue->Des().SetLength( 0 );
    iValue->Des().Append( aValue );
    }

//  End of File
