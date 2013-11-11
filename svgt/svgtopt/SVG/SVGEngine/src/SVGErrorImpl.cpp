/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine source file
 *
*/


#include "SVGErrorImpl.h"


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
CSvgErrorImpl* CSvgErrorImpl::NewL()
    {
    CSvgErrorImpl* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
CSvgErrorImpl* CSvgErrorImpl::NewLC()
    {
    CSvgErrorImpl* self = new ( ELeave ) CSvgErrorImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgErrorImpl::~CSvgErrorImpl()
    {
    if(iDescription)
		{
		delete iDescription;
		iDescription = NULL;
		}
    }

// ---------------------------------------------------------------------------
// return Etrue if an error is indicated by this object.
// ---------------------------------------------------------------------------
TBool CSvgErrorImpl::HasError()
{
    return iErrorCode != ESvgNoError;
}

// ---------------------------------------------------------------------------
// return Etrue if the error indicated by this object is only a warning.
// ---------------------------------------------------------------------------
TBool CSvgErrorImpl::IsWarning()
{
    return iIsWarning;
}

// ---------------------------------------------------------------------------
// Set the state that the error indicated by this object is only a warning.
// ---------------------------------------------------------------------------
void CSvgErrorImpl::SetIsWarning( TBool aBool )
{
    iIsWarning = aBool;
}

// ---------------------------------------------------------------------------
// Get the error code.
// ---------------------------------------------------------------------------
TSvgErrorCode CSvgErrorImpl::ErrorCode()
{
    return iErrorCode;
}

// ---------------------------------------------------------------------------
// Set the error code.
// ---------------------------------------------------------------------------
void CSvgErrorImpl::SetErrorCode( TSvgErrorCode aErrorCode )
{
    iErrorCode = aErrorCode;
}
// ---------------------------------------------------------------------------
// Get the error description.
// ---------------------------------------------------------------------------
TDesC& CSvgErrorImpl::Description()
{
    return *iDescription;
}

// ---------------------------------------------------------------------------
// Set the given string as the description
// ---------------------------------------------------------------------------
void CSvgErrorImpl::SetDescription( const TDesC& aDescription )
{
   delete iDescription;
   iDescription = NULL;
   TRAPD( error, iDescription = aDescription.AllocL() );
   if ( error != KErrNone )
      {
   	 // ignore trap error
	  }
}

// ---------------------------------------------------------------------------
// Set the given strings as the description
// ---------------------------------------------------------------------------
void CSvgErrorImpl::SetDescription( const TDesC& aText1, const TDesC& aText2 )
{
    delete iDescription;
    iDescription = NULL;
    TRAPD( error, iDescription = HBufC::NewL( aText1.Length() + aText2.Length() ) );
    if  ( error == KErrNone )
        {
        iDescription->Des().Append( aText1 );
        iDescription->Des().Append( aText2 );
        }
}

// ---------------------------------------------------------------------------
// Append the given string to the description
// ---------------------------------------------------------------------------
void CSvgErrorImpl::AppendDescription( const TDesC& aDescription )
{
    iDescription = iDescription->ReAlloc( iDescription->Length() + aDescription.Length() );
    iDescription->Des().Append( aDescription );
}

TInt CSvgErrorImpl::SystemErrorCode()
{
    return iSystemErrorCode;
}

void CSvgErrorImpl::SetSystemErrorCode( TInt aErrorCode )
{
    iSystemErrorCode = aErrorCode;
}

// ---------------------------------------------------------------------------
// Second phase of construction
// ---------------------------------------------------------------------------
void CSvgErrorImpl::ConstructL()
{
    iDescription = HBufC::NewL( 1 );
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CSvgErrorImpl::CSvgErrorImpl()
{
    SetIsWarning( ETrue );
    SetErrorCode( ESvgNoError );
    SetSystemErrorCode( KErrNone );
}






