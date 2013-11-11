/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data supplier for responses loaded from cache
*
*/


// INCLUDE FILES
#include "HttpCacheDataSupplier.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheDataSupplier::CHttpCacheDataSupplier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheDataSupplier::CHttpCacheDataSupplier(
    HBufC8* aBody ) 
    : m_body( aBody )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheDataSupplier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheDataSupplier* CHttpCacheDataSupplier::NewL(
    HBufC8* aBody )
    {
    CHttpCacheDataSupplier* self = new(ELeave) CHttpCacheDataSupplier( aBody );
    return self;
    }

// Destructor
CHttpCacheDataSupplier::~CHttpCacheDataSupplier()
    {
    ReleaseData();
    }

// -----------------------------------------------------------------------------
// CHttpCacheDataSupplier::GetNextDataPart
// Return the next chunk of response body from the Cache
// -----------------------------------------------------------------------------
//
TBool CHttpCacheDataSupplier::GetNextDataPart(
    TPtrC8 &aDataChunk )
    {
    aDataChunk.Set( *m_body );
    return ETrue;
    }

//  End of File
