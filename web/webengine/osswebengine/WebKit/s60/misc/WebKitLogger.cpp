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
* Description:  
*
*/


// INCLUDE FILES
#include "config.h"
#include "WebKitLogger.h"
#include <string.h>
#include <e32hal.h>

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

LogStream::LogStream() : iLoggerReady( EFalse )
    {
    iString.Zero();
    }

LogStream& LogStream::LogStream::operator<<(
    TInt aInt )
    {
    TBuf<10> num;
    num.Num( aInt );
    return *this << num;
    }

LogStream& LogStream::operator<<(
    TUint aUint )
    {
    TBuf<10> num;
    num.Num( aUint );
    return *this << num;
    }

LogStream& LogStream::operator<<(
    TReal aReal )
    {
  TBuf<32> buffer;
  buffer.Num( TReal( aReal ), TRealFormat() );
  return *this<<buffer;
    }

LogStream& LogStream::operator<<(
    const char* aString )
    {
    iString.Append( (unsigned char*)aString, (TInt)strlen( aString ) );
    return *this;
    }

LogStream& LogStream::operator<<(
    const TDesC& aString )
    {
    iString.Append( aString );
    return *this;
    }

LogStream& LogStream::operator<<(
    const TDesC8& aString )
    {
    iString.Append( aString );
    return *this;
    }

LogStream& LogStream::operator<<(
    TAny* aAny )
    {
  TBuf8<32> buffer;
  buffer.Num( (TUint)aAny, EHex );
    return *this << buffer;
    }

// LogStream& LogStream::operator<<(const LogStreamManipulator& );
void LogStream::flush()
    {
    if( !iLoggerReady )
        {
        RFileLogger::Write( _L("Browser"), _L("webkit_tot.html"), EFileLoggingModeAppend,
        _L("<html><head></head><body>") );
        iLoggerReady = ETrue;
        }
    (*this)<<"<br>";
    RFileLogger::Write( _L("Browser"), _L("webkit_tot.html"), EFileLoggingModeAppend, iString );
    iString.Zero();
    }

LogStream& flush(
    LogStream& aStream )
    {
    // flush the string to output
    aStream.flush();
    return aStream;
    }

void LogStream::mem()
    {
    flush();
    TMemoryInfoV1Buf meminfo;
    UserHal::MemoryInfo( meminfo );
    (*this)<<"Mem available:";
    (*this)<<meminfo().iFreeRamInBytes;
    }

LogStream& mem(
    LogStream& aStream )
    {
    // flush the string to output
    aStream.mem();
    return aStream;
    }

//  End of File
