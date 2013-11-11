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
* Description:  SVG Engine header file
 *
*/


#ifndef __INC_TTokenizer__
#define __INC_TTokenizer__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

class TTokenizer
{
public:
    TTokenizer( const TPtrC& aString );

    TBool IsAtEnd();
    TInt EndIndex();

    TUint16 NextChar();
    void Advance( TInt count = 1 );

    TBool SkipWhiteSpace();
    TBool SkipInteger();
    TBool SkipDecimal();
    TBool SkipHex();

    TBool SkipUntilEnd( );
    TBool SkipUntil( TUint16 aChar );

    TBool StartsWith( const TDesC& aString );

    TPtrC SkippedString();

    TPtrC Remainder();

    static TBool IsWhiteSpace( TUint16 aChar );
    static TBool IsDigit( TUint16 aChar );
    static TBool IsHexDigit( TUint16 aChar );

    static TInt IndexOf( const TDesC& aString, TUint16 aChar );

    // return a trim string at both ends
    static TPtrC Trim( const TDesC& aString );
    
    static TBool ParseDecimal( const TDesC& aString, TReal32& aValue, TPtrC& aRemainder );



private:
    TPtrC iString;
    TInt iCurrentIndex;
    TInt iPreviousIndex;

};


#endif //__INC_TTokenizer__
