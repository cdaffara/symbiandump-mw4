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
* Description:  Extension to RFile class to load custom data from file.
*
*/



#ifndef RFILEEXT_H
#define RFILEEXT_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// CONSTANTS

// MACROS
#define MIN( a, b ) ( (a) < (b) ? (a) : (b) )
#define MAX( a, b ) ( (a) > (b) ? (a) : (b) )


#define APPEND_BUF_INT( buf, a ) { buf.Append( (TUint8*)&(a), sizeof(a) ); }

#define READ_INT_L( fs, a ) { TPtr8 buf( (TUint8*)&(a), sizeof(a) ); \
                              User::LeaveIfError( fs.Read( buf ) ); \
                              if( buf.Length() != sizeof(a) ) \
                                {   \
                                User::Leave( KErrEof ); \
                                }   \
                          }

#define READ_CUST_L( fs, a ) { TPtr8 buff( (TUint8*)&(a), sizeof(a) ); \
                               fs.Read( buff, sizeof(a) ); \
                               if( buff.Length() == 0 ) \
                                    { \
                                    User::Leave( KErrEof ); \
                                    } \
                            }

// indicates that the passed HBufC(8)* pointer is/was NULL
const TInt KNullBuffer = -1;

inline void ReadHBufCL( RFile& aInFile, HBufC* &aBuf )
    {
    TInt length;

    delete aBuf;
    aBuf = NULL;

    // read length of the buffer
    READ_INT_L( aInFile, length );

    if( length > 10*1024 )
        // we surely didn't have so huge block
        {
        User::Leave( KErrCorrupt );
        }

    if( length != KNullBuffer )
        {
        aBuf = HBufC::NewL( length / 2 );   // byte -> dbyte
        if( length )
            {
            TPtr8 buf( (TUint8*)aBuf->Ptr(), length );

            User::LeaveIfError( aInFile.Read( buf, length ) );

            if( buf.Length() == length )
                {
                aBuf->Des().SetLength( length / 2 );
                }
            else
                // couldn't read enough byte -> eof
                {
                delete aBuf;
                aBuf = NULL;

                User::Leave( KErrEof );
                }
            }
        }
    }

inline void ReadHBufCL( RFile& aInFile, HBufC8* &aBuf )
    {
    TInt length;

    delete aBuf;
    aBuf = NULL;

    // read length of the buffer
    READ_INT_L( aInFile, length );

    if( length > 10*1024 )
        // we surely didn't have so huge block
        {
        User::Leave( KErrCorrupt );
        }

    if( length != KNullBuffer )
        {
        aBuf = HBufC8::NewL( length );   // byte -> dbyte
        if( length )
            {
            TPtr8 buf( (TUint8*)aBuf->Ptr(), length );

            User::LeaveIfError( aInFile.Read( buf, length ) );

            if( buf.Length() == length )
                {
                aBuf->Des().SetLength( length );
                }
            else
                // couldn't read enough byte -> eof
                {
                delete aBuf;
                aBuf = NULL;

                User::Leave( KErrEof );
                }
            }
        }
    }

/**
 * Appends data and data length info to buffer
 * @param aDest destinaion buffer
 * @param aSrc  source of data
 * @return None. Leaves on error.
 */
void AppendBufL( TPtr8& aDest, HBufC* aSrc );

/**
 * Appends data and data length info to buffer
 * @param aDest destinaion buffer
 * @param aSrc  source of data
 * @return None. Leaves on error.
 */
void AppendBufL( TPtr8& aDest, HBufC8* aSrc );

inline void ReallocateStringL( HBufC* &aBuff, const TDesC& aValue, TInt aMaxSize = 0 )
    {
    delete aBuff;
    aBuff = NULL;
    TInt min;
    TInt max;

    if( aMaxSize )
        {
        min = MIN( aValue.Length(), aMaxSize );
        max = MAX( aValue.Length(), aMaxSize );
        }
    else
        {
        min = aValue.Length();
        max = aValue.Length();
        }
    
    aBuff = HBufC::NewL( max );
    aBuff->Des().Copy( aValue );
    aBuff->Des().SetLength( min );
    }

inline void ReallocateStringL( HBufC8* &aBuff, const TDesC8& aValue, TInt aMaxSize = 0 )
    {
    delete aBuff;
    aBuff = NULL;
    TInt min;
    TInt max;

    if( aMaxSize )
        {
        min = MIN( aValue.Length(), aMaxSize );
        max = MAX( aValue.Length(), aMaxSize );
        }
    else
        {
        min = aValue.Length();
        max = aValue.Length();
        }
    
    aBuff = HBufC8::NewL( max );
    aBuff->Des().Copy( aValue );
    aBuff->Des().SetLength( min );
    }

inline void ReallocateStringL( HBufC8* &aBuff, const TDesC& aValue, TInt aMaxSize = 0 )
    {
    delete aBuff;
    aBuff = NULL;
    TInt min;
    TInt max;

    if( aMaxSize )
        {
        min = MIN( aValue.Length(), aMaxSize );
        max = MAX( aValue.Length(), aMaxSize );
        }
    else
        {
        min = aValue.Length();
        max = aValue.Length();
        }
    
    aBuff = HBufC8::NewL( max );
    aBuff->Des().Copy( aValue );
    aBuff->Des().SetLength( min );
    }

inline void ReallocateStringL( HBufC* &aBuff, const TDesC8& aValue, TInt aMaxSize = 0 )
    {
    delete aBuff;
    aBuff = NULL;
    TInt min;
    TInt max;

    if( aMaxSize )
        {
        min = MIN( aValue.Length(), aMaxSize );
        max = MAX( aValue.Length(), aMaxSize );
        }
    else
        {
        min = aValue.Length();
        max = aValue.Length();
        }
    
    aBuff = HBufC::NewL( max );
    aBuff->Des().Copy( aValue );
    aBuff->Des().SetLength( min );
    }


#endif      // RFILEEXT_H
            
// End of File
