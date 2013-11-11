/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Utility for debug prints to file
*
*/



#ifndef C_DEBUGLOGGER_H
#define C_DEBUGLOGGER_H

//file only included with one of these debug flags enabled.
#if defined(TFXSERVER_TIME_LOG) || defined(WSSERVERDRAWER_TIME_LOG)

#include <e32base.h>
#include <flogger.h>

/**
 *  Class that holds the data for one log entry, the data consists
 *  of one string and up to 4 integers
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CLogEntry) : public CBase
	{
public:
	CLogEntry(const TDesC16& aMsg, TInt aNbrArg, TInt aArg1=0, TInt aArg2=0, TInt aArg3=0, TInt aArg4=0);
	~CLogEntry();

//data
	HBufC16 *iMsg;
	TInt iNbrArgs;
	TInt iArg1,iArg2,iArg3,iArg4;
	};

/**
 *  Class that stores LogEntrys and later writes them to a file.
 *  This can be used to get output on target without taking to much time.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS(CDebugLogger) : public CBase
	{
public:
	CDebugLogger();
	~CDebugLogger();
	void Log(const TDesC16& aMsg);
	void Log1(const TDesC16& aMsg, TInt aArg1);
	void Log2(const TDesC16& aMsg, TInt aArg1, TInt aArg2);
	void Log3(const TDesC16& aMsg, TInt aArg1, TInt aArg2, TInt aArg3);
	void Log4(const TDesC16& aMsg, TInt aArg1, TInt aArg2, TInt aArg3, TInt aArg4);
	void WriteToFile(const TDesC& aLogDir, const TDesC& aLogFile);
	TUint32 TimeMs();
private: //data
	RPointerArray<CLogEntry> iEntries;
	};
	

#endif // TFXSERVER_TIME_LOG

#endif //C_DEBUGLOGGER_H

