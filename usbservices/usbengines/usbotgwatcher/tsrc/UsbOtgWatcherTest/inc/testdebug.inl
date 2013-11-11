/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <e32debug.h>

NONSHARABLE_CLASS( TOverflowTruncate8 ) : public TDes8Overflow
    {
public:
    void Overflow( TDes8& /*aDes*/ ) {}
    }; 

NONSHARABLE_CLASS( TOverflowTruncate16 ) : public TDes16Overflow
    {
public:
    void Overflow( TDes16& /*aDes*/ ) {}
    };

TLogBuf16 TTestLogger::CreateLogBuf( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TOverflowTruncate8 overflow;   
    TLogBuf8 buf8;
    TLogBuf16 buf16;
    buf8.AppendFormatList( aFmt, list, &overflow );
    buf16.Copy( buf8 );
    return buf16;
    };

TLogBuf16 TTestLogger::CreateLogBuf( TRefByValue<const TDesC16> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TOverflowTruncate16 overflow;
    TLogBuf16 buf16;
    buf16.AppendFormatList( aFmt, list, &overflow );
    return buf16;
    };

void TTestLogger::Trace( CStifLogger& aLog, TLogBuf16 aLogBuf )
    {    
    RBuf16 buf;
    RBuf16 bufStifLog;
    
    buf.Create( KMaxLogLineLength );
    buf.Append( KStifTestPrefix );
    buf.Append( aLogBuf );
    bufStifLog.Create( KMaxLogLineLength );
    bufStifLog.Append( aLogBuf );
    
    RDebug::Print( buf );
    aLog.Log( bufStifLog );
    buf.Close();
    bufStifLog.Close();
    };
