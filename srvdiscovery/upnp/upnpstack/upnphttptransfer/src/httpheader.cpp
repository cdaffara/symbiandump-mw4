/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents one header in transferred file
*
*/



#include "httpheader.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpHeader::CHttpHeader()
// --------------------------------------------------------------------------
//
CHttpHeader::CHttpHeader() 
    {
    iFieldName = NULL;
    iFieldValue = NULL;
    }

// --------------------------------------------------------------------------
// CHttpHeader::~CHttpHeader()
// --------------------------------------------------------------------------
//
CHttpHeader::~CHttpHeader()
    {
    delete iFieldName;
    delete iFieldValue;
    }

// --------------------------------------------------------------------------
// CHttpHeader::ConstructL()
// --------------------------------------------------------------------------
//
void CHttpHeader::ConstructL( const TDesC8& aFieldName, 
                              const TDesC8& aFieldValue )
    {
    iFieldName = aFieldName.AllocL();
    iFieldValue = aFieldValue.AllocL();
    }

// --------------------------------------------------------------------------
// CHttpHeader::NewL()
// --------------------------------------------------------------------------
//
CHttpHeader* CHttpHeader::NewL( const TDesC8& aFieldName, 
                                const TDesC8& aFieldValue )
    {
    CHttpHeader* self = new( ELeave ) CHttpHeader();
    CleanupStack::PushL( self );
    self->ConstructL( aFieldName, aFieldValue );
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------
// CHttpHeader::FieldName()
// --------------------------------------------------------------------------
//
const TDesC8& CHttpHeader::FieldName()
    {
    return *iFieldName;
    }

// --------------------------------------------------------------------------
// CHttpHeader::FieldValue()
// --------------------------------------------------------------------------
//
const TDesC8& CHttpHeader::FieldValue()
    {
    return *iFieldValue;
    }

// end of file
