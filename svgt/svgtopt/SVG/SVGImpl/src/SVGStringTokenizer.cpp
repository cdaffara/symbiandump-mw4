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


#include "SVGStringTokenizer.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TStringTokenizer::TStringTokenizer( const TDesC& aSrc, const TDesC& aDelim ) : iSrc( aSrc ),
                                                                               iDelim( aDelim )
    {
    SkipDeliminator( iDelim );
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TPtrC TStringTokenizer::NextToken()
    {
    return NextToken( iDelim );
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TPtrC TStringTokenizer::NextToken( const TDesC& aDelim )
    {
    iSrc.Mark();
    TChar tmpchar = iSrc.Peek();
    while ( !isDeliminator( tmpchar, aDelim ) && !iSrc.Eos() )
        {
        iSrc.Inc();
        tmpchar = iSrc.Peek();
        }
    TPtrC token = iSrc.MarkedToken();
    SkipDeliminator( aDelim );
    return token;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool TStringTokenizer::HasMoreTokens()
    {
    return !iSrc.Eos();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool TStringTokenizer::isDeliminator( TChar aChar, const TDesC& aDelim )
    {
    TUint16 ch = ( TUint16 ) ( TUint ) aChar;
    
    TInt32 delimLength = aDelim.Length();
    for ( TInt32 i = 0; i < delimLength; i++ )
        {
        if ( ch == aDelim[i] )
            return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void TStringTokenizer::SkipDeliminator( const TDesC& aDelim )
    {
    TChar tmpchar = iSrc.Peek();
    while ( isDeliminator( tmpchar, aDelim ) && !iSrc.Eos() )
        {
        iSrc.Inc();
        tmpchar = iSrc.Peek();
        }
    }

