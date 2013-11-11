/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Printing log messages
*
*/



#if !defined(__LOGCOMMON_H__)
#define __LOGCOMMON_H__

#if !defined(_DEBUG)

#define LOG(a)
#define LOG_(a)
#define LOG_1(s, v)
#define LOG8_(a)
#define LOG8_1(s, v)
#define STACK_LEFT

#else // _DEBUG

#define LOG(a) a
#define LOG_(a) Log::Printf( _L(a) )
#define LOG_1(s, v) Log::Printf( _L(s), v )
#define LOG8_(a) Log::Printf( _L8(a) )
#define LOG8_1(s, v) Log::Printf( _L8(s), v )
#define STACK_LEFT { RThread __r; TThreadStackInfo __i; __r.StackInfo(__i); TInt maxSize = __i.iBase - __i.iLimit; TInt currentSize = __i.iBase - (TInt)&__r; Log::Printf( _L("Max stack size: %d, current size: %d"), maxSize, currentSize); }

#include <flogger.h>
_LIT(KLogFolder,"vpn");

class Log
	{
public:
	static inline void Write(const TDesC16& aDes);
	static inline void Write(const TDesC8& aDes);
	static inline void Printf(TRefByValue<const TDesC> aFmt, ...);
	static inline void Printf(TRefByValue<const TDesC8> aFmt, ...);
	static inline void HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);
	};

inline void Log::Write(const TDesC16& aDes)
    {
    RFileLogger logger;
    if (logger.Connect() != KErrNone)
    	return;
    logger.CreateLog(KLogFolder(), KLogFile(), EFileLoggingModeAppend);
    TInt maxLen = (KLogBufferSize < 100) ? KLogBufferSize : 100;
    TInt start(0);
    while (start < aDes.Length())
    	{
    	TInt length = (start + maxLen >= aDes.Length()) ?
				(aDes.Length() - start) : maxLen;
		TInt addLength = length;
		const TDesC16 &tmp = aDes.Mid(start, length);
		
		TInt linefeedPos = tmp.Find(_L("\n"));
		if (linefeedPos != KErrNotFound)
			{
			length -= (length - linefeedPos);
			addLength = length + 1; // To skip linefeed
			}
		
		logger.Write(tmp.Left(length));
		start += addLength;
    	}
    logger.Close();
    }

inline void Log::Write(const TDesC8& aDes)
    {
    RFileLogger logger;
    if (logger.Connect() != KErrNone)
    	return;
    logger.CreateLog(KLogFolder(), KLogFile(), EFileLoggingModeAppend);
    TInt maxLen = (KLogBufferSize < 100) ? KLogBufferSize : 100;
    TInt start(0);
    while (start < aDes.Length())
    	{
    	TInt length = (start + maxLen >= aDes.Length()) ?
				(aDes.Length() - start) : maxLen;
		TInt addLength = length;
		const TDesC8 &tmp = aDes.Mid(start, length);
		
		TInt linefeedPos = tmp.Find(_L8("\n"));
		if (linefeedPos != KErrNotFound)
			{
			length -= (length - linefeedPos);
			addLength = length + 1; // To skip linefeed
			}
		
		logger.Write(tmp.Left(length));
		start += addLength;
    	}
    logger.Close();
    }

inline void Log::Printf(TRefByValue<const TDesC> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
    const TInt KLogTimeFormatLength = 16;
    const TInt KLogLineLength = 256; 	
    TBuf<KLogLineLength> buf;
    buf.FormatList( aFmt, list ); 
    
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		_LIT( KLogTimeFormat, "%02d:%02d,%02d,%06d;");
		logger.SetDateAndTime( ETrue, EFalse );
		logger.CreateLog( KLogFolder(), KLogFile(), EFileLoggingModeAppend );		
		TBuf<KLogTimeFormatLength> timeStamp;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		timeStamp.Format( KLogTimeFormat, 
                dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();    
    	
	//RFileLogger::WriteFormat(KLogFolder(), KLogFile(), EFileLoggingModeAppend, aFmt, list);
    }

inline void Log::Printf(TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KLogFolder(), KLogFile(), EFileLoggingModeAppend, aFmt, list);
    }

inline void Log::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
    {
	RFileLogger::HexDump(KLogFolder(), KLogFile(), EFileLoggingModeAppend, aHeader, aMargin, aPtr, aLen);
    }


#endif // !defined(_DEBUG)

#endif //__LOGCOMMON_H__
