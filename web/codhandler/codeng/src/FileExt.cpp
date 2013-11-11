/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Extension to RFile class to load custom data from file.
*
*/




//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "FileExt.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS


// -----------------------------------------------------------------------------
// AppendBufL
// -----------------------------------------------------------------------------
//
void AppendBufL( TPtr8& aDest, HBufC* aSrc )
    {
    TInt length = aSrc ? aSrc->Length() * 2 : KNullBuffer;

    APPEND_BUF_INT( aDest, length );

    if( length > 0 )
        {
        TPtr8 tmp8( (TUint8*)aSrc->Ptr(), length, length);
        __ASSERT_DEBUG( (aDest.MaxLength() - aDest.Length()) > tmp8.Length() , User::Panic( KNullDesC, KErrTooBig ) );
        aDest.Append( tmp8 );
        }
    }

// -----------------------------------------------------------------------------
// AppendBufL
// -----------------------------------------------------------------------------
//
void AppendBufL( TPtr8& aDest, HBufC8* aSrc )
    {
    TInt length = aSrc ? aSrc->Length() : KNullBuffer;

    APPEND_BUF_INT( aDest, length );

    if( length > 0 )
        {
        __ASSERT_DEBUG( (aDest.MaxLength() - aDest.Length()) > length, User::Panic( KNullDesC, KErrTooBig ) );
        aDest.Append( *aSrc );
        }
    }


            
//  End of File
