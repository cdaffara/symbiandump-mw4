/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for SfLogger to include macros needed to log information.
*
*/


/*
---------------------------------------------------------------------------------------
FILE:
SfLogger.h

DESCRIPTION:
DebugLogger.
SfLogger includes a few macros that can be used to record information 
of interest to the user. Log file is stored to c:\Logs\BrowserTelService\.

EXAMPLES:
#ifdef __DEBUGLOGGER__
	//Writes 'Some text' to the log
	WRITE_LOG(_L("Some text"));
#endif __DEBUGLOGGER__

#ifdef __DEBUGLOGGER__
	//Writes formated text to the log
	TInt num1=100;
	WRITE_LOG_FORMAT(_L("Writing a number to the log: %d"), num1);
#endif __DEBUGLOGGER__

#ifdef __DEBUGLOGGER__
	// Line begins with time only
	SET_LOG_DATE_TIME( EFalse, ETrue);		
#endif __DEBUGLOGGER__

#ifdef __DEBUGLOGGER__
	WRITE_LOG(_L8("HexDump Test"));
	const TText* hdr=_S("123456");
	const TText* mgn=_S("      ");
	const TUint8* ptr=_S8("abcdefghijklmnopqrstuxyz");
	TInt len=26;
	LOG_HEX_DUMP(hdr,mgn,ptr,len);
#endif __DEBUGLOGGER__

----------------------------------------------------------------------------------------
*/

#ifndef __SFLOGGER_H
#define __SFLOGGER_H

//#define __DEBUG_PRINT

#ifdef __DEBUG_PRINT
#include <flogger.h>

// log files are stored to KDebugLogDirFull folder
_LIT(KDebugLogDirFull,"C:\\Logs\\BrowserTelService\\");

_LIT(KDebugLogDir, "BrowserTelService");
_LIT(KDebugLogFileName, "DebugLog.txt");
_LIT(KDebugLogTitle, "- BrowserTelService Debug Log File -");
_LIT(KDebugLogCreated, "**Debug Log File Created**");
_LIT(KDebugLogClosed, "**Debug Log File Closed**");
_LIT(KFailed, "failed: leave code=%d");

#endif	// __DEBUG_PRINT

#ifdef __DEBUG_PRINT
#define WRITE_LOG(AAA,BBB) AAA.Write(BBB)
#else
#define WRITE_LOG(AAA,BBB)
#endif	// __DEBUG_PRINT

#ifdef __DEBUG_PRINT
#define WRITE_LOG_FORMAT(AAA,BBB,CCC) AAA.WriteFormat(BBB,CCC)
#else
#define WRITE_LOG_FORMAT(AAA,BBB,CCC)
#endif	// __DEBUG_PRINT

#ifdef __DEBUG_PRINT
#define SET_LOG_DATE_TIME(AAA,BBB,CCC) AAA.SetDateAndTime(BBB,CCC)
#else
#define SET_LOG_DATE_TIME(AAA,BBB,CCC)
#endif	// __DEBUG_PRINT

#ifdef __DEBUG_PRINT
#define WRITE_ALLOCATED_SIZE(AAA,BBB) {TInt bytes; User::AllocSize(bytes); WRITE_LOG_FORMAT(AAA,KBytes,bytes,BBB);};
#else
#define WRITE_ALLOCATED_SIZE()
#endif	// __DEBUG_PRINT

#endif	// __SFLOGGER_H