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
* Description:  SVG Implementation source file
 *
*/


#include "SVGTokenizer.h"

TTokenizer::TTokenizer( const TPtrC& aString )
{
    iString.Set( Trim( aString ) );
    iCurrentIndex = 0;
    iPreviousIndex = 0;
}

TBool TTokenizer::IsAtEnd()
{
    return iCurrentIndex >= EndIndex();
}

TInt TTokenizer::EndIndex()
{
    return iString.Length();
}

TUint16 TTokenizer::NextChar()
{
    if ( IsAtEnd() )
        return '\0';
    else
        return iString[iCurrentIndex];
}

void TTokenizer::Advance( TInt aCount )
{
    iCurrentIndex += aCount;
    if ( iCurrentIndex > EndIndex() )
        iCurrentIndex = EndIndex();
    else if ( iCurrentIndex < 0 )
        iCurrentIndex = 0;

    iPreviousIndex = iCurrentIndex;
}

TBool TTokenizer::SkipInteger()
{
    if ( iCurrentIndex >= EndIndex() )
        return EFalse;

    if ( NextChar() == '-' || NextChar() == '+' )
    {
        TInt index = iCurrentIndex;
        iCurrentIndex++;
        if ( SkipInteger() )
        {
            iPreviousIndex = index;
            return ETrue;
        }
        else
        {
            iCurrentIndex = index;
            iPreviousIndex = iCurrentIndex;
            return EFalse;
        }
    }

    iPreviousIndex = iCurrentIndex;
    TInt index = iCurrentIndex;

    while ( index < EndIndex() && IsDigit( iString[index] ) )
    {
        index++;
    }
    if ( IsAtEnd() )
        return EFalse;

    iCurrentIndex = index;
    return ETrue;
}

TBool TTokenizer::SkipDecimal()
{
    if ( iCurrentIndex >= EndIndex() )
        return EFalse;

    if ( NextChar() == '-' || NextChar() == '+' )
    {
        TInt index = iCurrentIndex;
        iCurrentIndex++;
        if ( NextChar() != '-' && NextChar() != '+' && SkipDecimal() )
        {
            iPreviousIndex = index;
            return ETrue;
        }
        else
        {
            iCurrentIndex = index;
            iPreviousIndex = iCurrentIndex;
            return EFalse;
        }
    }

    if ( NextChar() == '.' )
    {
        TInt index = iCurrentIndex;
        Advance( 1 ); // skip '.'
        if ( !IsDigit( NextChar() ) )
        {
            // Only found '.' and no digits following
            Advance( -1 );
            return EFalse;
        }
        else
        {
            // '.xxx'
            SkipInteger();
            iPreviousIndex = index;
            return ETrue;
        }
    }
    else if ( IsDigit( NextChar() ) )
    {
        SkipInteger();
        if ( NextChar() != '.' )
        {
            // 'xxx'
            return ETrue;
        }
        else
        {
            TInt index = iPreviousIndex;//iCurrentIndex;
            Advance( 1 ); // skip '.'

            SkipInteger();
            iPreviousIndex = index;
            return ETrue;
        }

    }
    return EFalse;
}

TBool TTokenizer::SkipHex()
{
    if ( iCurrentIndex >= EndIndex() )
        return EFalse;

    iPreviousIndex = iCurrentIndex;
    TInt index = iCurrentIndex;

    while ( index < EndIndex() && IsHexDigit( iString[index] ) )
    {
        index++;
    }
    if ( IsAtEnd() )
        return EFalse;

    iCurrentIndex = index;
    return ETrue;
}

TBool TTokenizer::SkipWhiteSpace()
{
    if ( iCurrentIndex >= EndIndex() )
        return EFalse;

    iPreviousIndex = iCurrentIndex;
    TInt index = iCurrentIndex;

    while ( index < EndIndex() && IsWhiteSpace( iString[index] ) )
    {
        index++;
    }
    if ( IsAtEnd() )
        return EFalse;

    iCurrentIndex = index;
    return ETrue;
}

TBool TTokenizer::SkipUntil( TUint16 aChar )
{
    if ( IsAtEnd() )
        return EFalse;

    iPreviousIndex = iCurrentIndex;

    TPtrC remaining( &iString.Ptr()[iCurrentIndex], iString.Length() - iCurrentIndex );
    TInt index = IndexOf( remaining, aChar );
    if ( index == -1 )
        return EFalse;

    iCurrentIndex += index;
    return ETrue;
}

TBool TTokenizer::SkipUntilEnd( )
{
    if ( IsAtEnd() )
        return EFalse;

    iPreviousIndex = iCurrentIndex;

    TPtrC remaining( &iString.Ptr()[iCurrentIndex], iString.Length() - iCurrentIndex );
    TInt index = remaining.Length();

    iCurrentIndex += index;
    return ETrue;
}

TBool TTokenizer::StartsWith( const TDesC& aString )
{
    if ( IsAtEnd() || aString.Length() > iString.Length() - iCurrentIndex )
        return EFalse;

    TPtrC remaining( &iString.Ptr()[iCurrentIndex], aString.Length() );
    return remaining == aString;
}

TPtrC TTokenizer::SkippedString()
{
    return TPtrC( &iString.Ptr()[iPreviousIndex], iCurrentIndex - iPreviousIndex );
}

TPtrC TTokenizer::Remainder()
{
    if ( iString.Length() == 0 )
        return TPtrC( NULL, 0 );
    else
        return TPtrC( &iString.Ptr()[iCurrentIndex], iString.Length() -  iCurrentIndex );
}
//------------------------------------------------------
// Helping Functions
//------------------------------------------------------
TBool TTokenizer::IsWhiteSpace( TUint16 aChar )
{
    return ( aChar == ' ' ) || ( aChar == '\t' ) || ( aChar == '\n' );
}

TBool TTokenizer::IsDigit( TUint16 aChar )
{
    return ( aChar >= '0' && aChar <= '9' );
}

TBool TTokenizer::IsHexDigit( TUint16 aChar )
{
    return ( aChar >= '0' && aChar <= '9' ) ||
           ( aChar >= 'a' && aChar <= 'f' ) ||
           ( aChar >= 'A' && aChar <= 'F' );
}

TInt TTokenizer::IndexOf( const TDesC& aString, TUint16 aChar )
{
    TInt length = aString.Length();
    for ( TInt i = 0; i < length; i++ )
    {
        if ( aString[i] == aChar )
            return i;
    }
    return -1;
}

TPtrC TTokenizer::Trim( const TDesC& aString )
{
    TInt startIndex = 0;
    for ( ; startIndex < aString.Length() && IsWhiteSpace( aString[startIndex] ); startIndex++ )
    {
    }

    TInt endIndex = aString.Length();
    for ( ; endIndex > 0 && IsWhiteSpace( aString[endIndex-1] ); endIndex-- )
    {
    }

    return TPtrC( &aString.Ptr()[startIndex], endIndex - startIndex );
}

TBool TTokenizer::ParseDecimal( const TDesC& aString, TReal32& aValue, TPtrC& aRemainder )
    {
    TTokenizer tokenizer( aString );
    tokenizer.SkipWhiteSpace();
    
    // blank, setting to zero
    if ( tokenizer.IsAtEnd() )
        {
        return EFalse;
        }
    
    if ( tokenizer.SkipDecimal() )
        {
        // Decimal number, extract it
        TPtrC decimalString = tokenizer.SkippedString();
        TLex lex( decimalString );
        // Specify the decimal seperator, instead of using
        // locale specific seperator.
        if(lex.Val( aValue, '.' )== KErrNone)
            {
            tokenizer.SkipWhiteSpace();
            aRemainder.Set( tokenizer.Remainder() );
            return ETrue;
            }
         }
    return EFalse;
    }
