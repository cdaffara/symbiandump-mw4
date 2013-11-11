/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging code fot the dmadpki
*
*/





#ifndef VPNLOGGER_H
#define VPNLOGGER_H

#if defined(_DEBUG)   

#include <e32base.h>
#include <flogger.h>


NONSHARABLE_CLASS(TTraceItem)
    {
public:
    inline TTraceItem(const TDesC& aTraceName);
    inline ~TTraceItem();            

    inline static void TraceCleanupOperation(TAny* aItem);
private:

    HBufC* iTraceName;
    TBool  iMethodHasLeft;
    };

 
NONSHARABLE_CLASS(CVpnDebugLogger) : public CBase
    {
public:

    inline static void InitializeDebugLoggerL(const TDesC& aFolder, const TDesC& aFileName);
    inline static void FinalizeDebugLogger();
    inline static void LogWrite(const TDesC& aText);
    inline static void LogWrite(const TDesC8& aText);
    
    inline static void LogWriteF(TRefByValue<const TDesC> aFmt, ...);
    inline static void LogWriteF(TRefByValue<const TDesC8> aFmt, ...);
    
    
    inline static void HexWrite(const TDesC8& aData);
    
    
private:
    inline void ConstructL(const TDesC& aFolder, const TDesC& aFileName);
    inline ~CVpnDebugLogger();
    
    inline static void TimeStamp(TDes& aBuffer);
    inline static CVpnDebugLogger* VpnDebugLogger();
    
    inline void WriteLogRaw(const TDesC& aLogMessage);   


    RFileLogger iFileLogger;
    TUint iCallDepth;
    
    TBuf<512> iDebugString;
    friend class TTraceItem;
    
    TUint8 iReferenceCount;
    };

#define INITIALIZE_DEBUG_LOG_L(a, b) CVpnDebugLogger::InitializeDebugLoggerL((a), (b))
#define FINALIZE_DEBUG_LOG CVpnDebugLogger::FinalizeDebugLogger();

/**
 * Trace can't be used inside LC methods.
 */
#define TRACE(a)  TTraceItem __trace(TPtrC((const TText *)L ## a))
#define DEBUG_LOG(a) CVpnDebugLogger::LogWrite(a)
#define DEBUG_LOG1(a, b) CVpnDebugLogger::LogWriteF((a), (b))
#define DEBUG_LOG2(a, b, c) CVpnDebugLogger::LogWriteF((a), (b), (c))

#define DEBUG_LOG_HEX(a) CVpnDebugLogger::HexWrite((a))

#include "vpnlogger.inl"

#else

#define INITIALIZE_DEBUG_LOG_L(a, b)
#define FINALIZE_DEBUG_LOG
#define TRACE(a)
#define DEBUG_LOG(a) 
#define DEBUG_LOG1(a, b)
#define DEBUG_LOG2(a, b, c)

#define DEBUG_LOG_HEX(a)


#endif //!defined(_DEBUG)
#endif //VPNLOGGER_H
