/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Implementation of RUnicodeFile.
*      
*
*/


// INCLUDE FILES

#include "UnicodeFile.h"

// CONSTANTS

/// Little-endian Unicode file header.
LOCAL_D const TUint16 KLittleEndianUnicodeHeader = 0xFEFF;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RUnicodeFile::Open
// ---------------------------------------------------------
//
TInt RUnicodeFile::Open( RFs& aFs, const TDesC& aName, TUint aFileMode )
    {
    if( aFileMode & EFileWrite )
        {
        // This supports only read access.
        User::Leave( KErrNotSupported );
        }
    TInt err = iFile.Open( aFs, aName, aFileMode );
    if ( !err )
        {
        // Check Unicode endianness.
        TUint c = 0;
        TRAPD( err, c = GetCharL() );
        if( !err )
            {
            if ( c != KLittleEndianUnicodeHeader )
                {
                err = KErrNotSupported;
                }
            }
        }
    if( err )
        {
        iFile.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RUnicodeFile::Replace
// ---------------------------------------------------------
//
TInt RUnicodeFile::ReplaceL( RFs& aFs, const TDesC& aName, TUint aFileMode )
    {
    if( aFileMode & EFileRead )
        {
        // This supports only write access.
        User::Leave( KErrNotSupported );
        }
    TInt err = iFile.Replace( aFs, aName, aFileMode );
    if ( !err )
        {
        // Write Unicode header.
        err = iFile.Write( TPtrC8( (TUint8*)&KLittleEndianUnicodeHeader, 2 ) );
        }
    if( err )
        {
        iFile.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RUnicodeFile::Close
// ---------------------------------------------------------
//
void RUnicodeFile::Close()
    {
    iFile.Close();
    }

// ---------------------------------------------------------
// RUnicodeFile::GetCharL
// ---------------------------------------------------------
//
TUint RUnicodeFile::GetCharL()
    {
    TUint c = 0;
    TPtr8 ptr( (TUint8*)(&c), 2 );
    User::LeaveIfError( iFile.Read( ptr, 2 ) );
    if( ptr.Length() == 0 )
        {
        // Nothing is read, successfully: this must be EOF.
        c = 0;
        }
    else if( ptr.Length() == 1 )
        {
        // Odd number of bytes in Unicode file?
        User::Leave( KErrCorrupt );
        }
    return c;
    }

// ---------------------------------------------------------
// RUnicodeFile::Write
// ---------------------------------------------------------
//
TInt RUnicodeFile::Write( const TDesC16& aDes )
    {
    return iFile.Write ( TPtrC8( (const TUint8*)aDes.Ptr(), aDes.Size() ) );
    }
