/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debug utilities
*
*/



const TUint KPrintLineLength = 200;
const TUint KPrintHexPerLine = 8;

// -----------------------------------------------------------------------------
// Debug::PrintBuffer
// -----------------------------------------------------------------------------
//
inline void Debug::PrintBuffer( TUint aLength, const TUint8* aData )
    {
    TBuf<KPrintLineLength> line;
    TUint idx( 0 );
    for ( ; idx < aLength; ++idx )
        {
        if ( idx && !( idx % KPrintHexPerLine ) )
            {
            RDebug::Print( line );
            line.Zero();
            }
        line.AppendFormat( _L( "%02X " ), aData[idx] );
        }
    RDebug::Print( line );
    }

// -----------------------------------------------------------------------------
// Debug::PrintString
// -----------------------------------------------------------------------------
//
inline void Debug::PrintString( const TDesC& aString, TUint aLength, const TUint8* aData )
    {
    TBuf<KPrintLineLength> line;

    line.Append( aString );
    
    TPtrC8 buf8( aData, aLength );
    
    TBuf<KPrintLineLength> buf16;
    buf16.Copy( buf8 );

    line.Append( buf16 );
    
    RDebug::Print( line );
    }

// -----------------------------------------------------------------------------
// Debug::PrintTimestamp
// -----------------------------------------------------------------------------
//
inline void Debug::PrintTimestamp( const TDesC& aString )
    {
    TBuf<KPrintLineLength> line;
    TTime time;
    time.HomeTime();
    
    time.FormatL( line, _L( "%H:%T:%S:%C" ) );
    line.Append( _L( " " ) );
    line.Append( aString );
    
    RDebug::Print( line );
    }
