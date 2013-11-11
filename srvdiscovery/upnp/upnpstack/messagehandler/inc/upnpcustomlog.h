/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares Logger Functions
*
*/





#ifndef C_UPNPCUSTOMLOG_H
#define C_UPNPCUSTOMLOG_H

#include <flogger.h>
#include <e32svr.h>

// UPNP_LOG :
//1 - logging, UDEB
//0 - no logging, UREL

//UPNP_FLOGGING = file logging, default in UDEB
//UPNP_CLOGGING = console logging


#ifndef _DEBUG
    // UREL
    #define UPNP_LOG  0   // No logging in UREL builds

#else

    // UDEB
    #define UPNP_LOG  1
    #define UPNP_FLOGGING //File logger
    //#define UPNP_CLOGGING //console logger

    _LIT(KEventLogFile, "EventActionLog.txt");
    // If log file is not defined in CPP file, use UPnPStackLog as default
    #ifndef KLogFile
        #define KLogFile _L("UPnPStack.txt")
    #endif

#endif

_LIT(KLogFolder,"upnp");
const TInt KMaxLogLen = 256;
//_LIT(KLogFile, LOG_FILE);

// Writing some descriptor into log
// LOGT(own_desc);
//
// Writing data into log
// LOGS("My log string");
//
// Writing data with one decimal parameter
// LOGS1("Log with decimal parameter %i", desimalValue);
//
// Writing data with one decimal and hexa parameter
// LOGS2("Log with decimal and hexa parameters %i %x", desimalValue, hexaValue);
//
// Writing data with one descriptor and decimal parameters
// LOGS2("String %S and decimal value %i", &stringData, decimalValue);
//
// Writing User::AllocSize
// LOGM;

#if UPNP_LOG == 1

#ifdef UPNP_FLOGGING

static void DoOutput(TDesC8& aData)
    {
    if(!aData.Length())
        {
        return;
        }
    RFileLogger logger;
    logger.Connect();

    logger.CreateLog( KLogFolder(), KLogFile, EFileLoggingModeAppend);
    logger.Write( aData.Mid(0,(aData.Length()<KMaxLogLen)? aData.Length():KMaxLogLen) );

    logger.CloseLog();
    logger.Close();
    }

static void DebugStringNarrowL(const char* aFmt, ...)
    {
    VA_LIST args;
    VA_START(args, aFmt);

    TPtrC8 fmt(reinterpret_cast<const unsigned char *>(aFmt));

    HBufC8* buf = HBufC8::NewLC(KMaxLogLen);

    buf->Des().FormatList(fmt, args);
    DoOutput(*buf);

    VA_END(args);
    CleanupStack::PopAndDestroy(buf);
    }

static void DebugStringWideL(const char* aFmt, ...)
    {
    VA_LIST args;
    VA_START(args, aFmt);

    TPtrC8 fmt(reinterpret_cast<const unsigned char *>(aFmt));

    HBufC* fmt16 = HBufC::NewLC(fmt.Length());
    fmt16->Des().Copy(fmt);

    HBufC* buf = HBufC::NewLC(KMaxLogLen);
    TPtrC pointer = fmt16->Des() ;
    buf->Des().FormatList(pointer, args);

    HBufC8* buf8 = HBufC8::NewLC(buf->Length());
    buf8->Des().Copy(*buf);

    DoOutput(*buf8);

    VA_END(args);

    CleanupStack::PopAndDestroy(buf8);
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(fmt16);
    }


static void DebugBufferL(const TDesC8& aBuf)
    {
    DebugStringNarrowL("\"%S\"", &aBuf);
    }

static void DebugBufferL(const TDesC& aBuf)
    {
    DebugStringWideL("\"%S\"", &aBuf);
    }

static void DebugTimeL()
    {
    TTime time;
    time.UniversalTime();

    TBuf<64> dateTimeString;

    _LIT(KTimeString, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
    time.FormatL(dateTimeString, KTimeString);
    DebugBufferL(dateTimeString);

    }


// ------------ Content Directory logging specific ---------------
_LIT(KInfoFormatString, "INFO#%S#%S#%d#%S");
_LIT(KErrFormatString, "ERROR#%S#%S#%d#%S");

static void LOGCD(const char* aInFunction, const char* aCalledFunction, TInt aErrorCode, const char* aComment)
{
    // Transforming parameters from 'const char *' into 'HBufC'
    TPtrC8 InFunction8(reinterpret_cast<const unsigned char *>(aInFunction));
    HBufC* InFunction16 = HBufC::NewLC(128);
    InFunction16->Des().Copy(InFunction8);

    TPtrC8 CalledFunction8(reinterpret_cast<const unsigned char *>(aCalledFunction));
    HBufC* CalledFunction16 = HBufC::NewLC(128);
    CalledFunction16->Des().Copy(CalledFunction8);

    TPtrC8 Comment8(reinterpret_cast<const unsigned char *>(aComment));
    HBufC* Comment16 = HBufC::NewLC(128);
    Comment16->Des().Copy(Comment8);

    // preparing main 16bit buffer
    HBufC* buf = HBufC::NewLC(KMaxLogLen);

    // formating all together in 16bit descriptor
    if(aErrorCode < 0)
    {
        buf->Des().Format(KErrFormatString, &*InFunction16, &*CalledFunction16, aErrorCode, &*Comment16);
    }
    else
    {
        buf->Des().Format(KInfoFormatString, &*InFunction16, &*CalledFunction16, aErrorCode, &*Comment16);
    }

    // converting 16bit descriptor into 8bit one
    HBufC8* buf8 = HBufC8::NewLC(buf->Length());
    buf8->Des().Copy(*buf);

    // main file logging
    DoOutput(*buf8);

    // cleaning
    CleanupStack::PopAndDestroy(buf8);
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(Comment16);
    CleanupStack::PopAndDestroy(CalledFunction16);
    CleanupStack::PopAndDestroy(InFunction16);
}


    #define LOGTIME DebugTimeL()
    #define LOG(x) DebugStringNarrowL x
    #define LOG8(x) DebugStringNarrowL x
    #define LOG16(x) DebugStringWideL x

    #define LOGT(A)
    #define LOGS(A)      LOG16((A))
    #define LOGS1(A,B)   LOG16((A,B))
    #define LOGS2(A,B,C) LOG16((A,B,C))
    #define LOGM

    #define LOGTH(H,A)       LOGT(A)
    #define LOGSH(H,A)       LOGS(A)
    #define LOGS1H(H,A,B)    LOGS1(A,B)
    #define LOGS2H(H,A,B,C)  LOGS2(A,B,C)

    #define ACTIONSEND(A)
    #define ACTIONRECV(A,B)

static void LOGSQL(const char* aInFunction, const char* aCalledFunction, TInt aErrorCode, const TDesC* aCommand)
{
    LOGCD(aInFunction, aCalledFunction, aErrorCode, "SQL Command:");

    // log whole sql command separately
    LOGS1("%S", aCommand);
}



#else     // UPNP_CLOGGING = console logging
    #define LOGS(A)      RDebug::Print(_L(A));
    #define LOGS1(A,B)   RDebug::Print(_L(A),B);
    #define LOGS2(A,B,C) RDebug::Print(_L(A),B,C);

    #define LOGT(A)
    #define LOGM
    #define ACTIONSEND(A)
    #define ACTIONRECV(A,B)

    #define LOGTH(H,A)       LOGT(A)
    #define LOGSH(H,A)       LOGS(A)
    #define LOGS1H(H,A,B)    LOGS1(A,B)
    #define LOGS2H(H,A,B,C)  LOGS2(A,B,C)
    #define LOGCD(A,B,C,D)
    #define LOGSQL(A,B,C,D)

    #define LOGTIME
    #define LOG(x)
    #define LOG8(x)
    #define LOG16(x)


#endif //UPNP_LOG == 1

#else // UPNP_LOG == 0 or invalid
    #define LOGT(A)
    #define LOGS(A)
    #define LOGS1(A,B)
    #define LOGS2(A,B,C)
    #define LOGM
    #define ACTIONSEND(A)
    #define ACTIONRECV(A,B)

    #define LOGTH(H,A)
    #define LOGSH(H,A)
    #define LOGS1H(H,A,B)
    #define LOGS2H(H,A,B,C)

    #define LOGCD(A,B,C,D)
    #define LOGSQL(A,B,C,D)

    #define LOGTIME
    #define LOG(x)
    #define LOG8(x)
    #define LOG16(x)

#endif // UPNP_LOG

#define LOG_FUNC_NAME LOGS( __PRETTY_FUNCTION__ )

#endif // C_UPNPCUSTOMLOG_H

// End Of File