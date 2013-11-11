/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "FileExt.h"
#include "HeaderField.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================
void Trim( TPtrC8& aTrim )
    {
    if( aTrim.Length() == 0 )
        {
        return;
        }

    TInt start = 0;

    for( ; start < aTrim.Length() && TChar(aTrim[start]).IsSpace(); ++start ){};

    TInt end = aTrim.Length()-1;
    for( ; end >= start && TChar(aTrim[end]).IsSpace(); --end ){};

    aTrim.Set( aTrim.Mid( start, (end+1)-start ) );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHeaderField::CHeaderField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHeaderField::CHeaderField( TInt aFieldIndex )
    :iFieldIndex( aFieldIndex )
    {
    }

// -----------------------------------------------------------------------------
// CHeaderField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHeaderField::ConstructL( const TDesC8* aFieldname,
                               const TDesC8* aFieldRawData )
    {
    if( aFieldname && aFieldRawData )
        {
        TPtrC8 name( *aFieldname );
        TPtrC8 rawData( *aFieldRawData );

        Trim( name );
        Trim( rawData );

        ReallocateStringL( iFieldName, name );
        ReallocateStringL( iFieldRawData, rawData );
        }
    }

// -----------------------------------------------------------------------------
// CHeaderField::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHeaderField* CHeaderField::NewL( const TDesC8* aFieldname,
                                  const TDesC8* aFieldRawData,
                                  TInt aFieldIndex )
    {
    CHeaderField* self = new( ELeave ) CHeaderField( aFieldIndex );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFieldname, aFieldRawData );
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHeaderField::~CHeaderField()
    {
    delete iFieldName;
    delete iFieldRawData;
    }

// -----------------------------------------------------------------------------
// CHeaderField::LoadHeaderInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHeaderField::LoadHeaderInfoL( RFile& aInFile )
    {
    ReadHBufCL( aInFile, iFieldName );
    ReadHBufCL( aInFile, iFieldRawData );
    }

// -----------------------------------------------------------------------------
// CHeaderField::Int
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHeaderField::Int() const
    {
    TLex8 temp( *iFieldRawData );
    TInt value( 0 );

    temp.Val( value );

    return value;
    }

//  End of File  
