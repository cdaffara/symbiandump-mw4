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



//File only compiled with one of these debug flags enabled.
#if defined(TFXSERVER_TIME_LOG) || defined(WSSERVERDRAWER_TIME_LOG)

#include "debuglogger.h"
#include <e32debug.h>

// ======== MEMBER FUNCTIONS ========

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
CDebugLogger::CDebugLogger()
	{
	}
	
CDebugLogger::~CDebugLogger() 
	{
	iEntries.ResetAndDestroy();
	}

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------	  
void CDebugLogger::Log(const TDesC16& aMsg) 
	{
	CLogEntry *entry = new CLogEntry(aMsg,0);
	if(entry)
		{
		if(KErrNone != iEntries.Append(entry))
			{
			delete entry;
			}
		}
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	  
void CDebugLogger::Log1(const TDesC16& aMsg, TInt aArg1) 
	{
	CLogEntry *entry = new CLogEntry(aMsg, 1, aArg1);
	if(entry)
		{
		if(KErrNone != iEntries.Append(entry))
			{
			delete entry;
			}
		}
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	  
void CDebugLogger::Log2(const TDesC16& aMsg, TInt aArg1, TInt aArg2) 
	{
	CLogEntry *entry = new CLogEntry(aMsg, 2, aArg1, aArg2);
	if(entry)
		{
		if(KErrNone != iEntries.Append(entry))
			{
			delete entry;
			}
		}
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	  
void CDebugLogger::Log3(const TDesC16& aMsg, TInt aArg1, TInt aArg2, TInt aArg3) 
	{
	CLogEntry *entry = new CLogEntry(aMsg, 3, aArg1, aArg2, aArg3);
	if(entry)
		{
		iEntries.Append(entry);
		}
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	  
void CDebugLogger::Log4(const TDesC16& aMsg, TInt aArg1, TInt aArg2, TInt aArg3, TInt aArg4) 
	{
	CLogEntry *entry = new CLogEntry(aMsg, 4, aArg1, aArg2, aArg3, aArg4);
	if(entry)
		{
		iEntries.Append(entry);
		}
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	  
void CDebugLogger::WriteToFile(const TDesC& aLogDir, const TDesC& aLogFile) 
	{
	const TInt count = iEntries.Count();
	/*lint -e1032 -e1023 */
	/* 	this is to prevent lint errors that says that writeformat is ambigious and cant be used
		without an object, writeformat is not ambigious - it has one static and one non-static version */
	for(TInt i = 0; i < count; i++)
		{
		switch(iEntries[i]->iNbrArgs)
			{
			case 0:
				RFileLogger::Write(aLogDir,aLogFile,EFileLoggingModeAppend,*(iEntries[i]->iMsg));
				break;
			case 1:
				RFileLogger::WriteFormat(aLogDir,aLogFile,EFileLoggingModeAppend,*(iEntries[i]->iMsg),iEntries[i]->iArg1);
				break;
			case 2:
				RFileLogger::WriteFormat(aLogDir,aLogFile,EFileLoggingModeAppend,*(iEntries[i]->iMsg),iEntries[i]->iArg1,iEntries[i]->iArg2);
				break;
			case 3:
				RFileLogger::WriteFormat(aLogDir,aLogFile,EFileLoggingModeAppend,*(iEntries[i]->iMsg),iEntries[i]->iArg1,iEntries[i]->iArg2,iEntries[i]->iArg3);
				break;
			case 4:
				RFileLogger::WriteFormat(aLogDir,aLogFile,EFileLoggingModeAppend,*(iEntries[i]->iMsg),iEntries[i]->iArg1,iEntries[i]->iArg2,iEntries[i]->iArg3,iEntries[i]->iArg4);
				break;
			default:
				break;
			}
		}
	/*lint +e1032 +e1023 */
	iEntries.ResetAndDestroy();
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
TUint32 CDebugLogger::TimeMs()
	{
	TTime now;
	now.UniversalTime();
	return (TUint32)(now.Int64()/1000);
	}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------	
CLogEntry::CLogEntry(const TDesC16& aMsg, TInt aNbrArg, TInt aArg1, TInt aArg2, TInt aArg3, TInt aArg4) :  iNbrArgs(aNbrArg), 
					iArg1(aArg1), iArg2(aArg2), iArg3(aArg3), iArg4(aArg4) 
	{
	iMsg = HBufC16::New(aMsg.Length());
	if(iMsg)
	    {
    	(*iMsg) = aMsg;
	    }
	}	  

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------		
CLogEntry::~CLogEntry()
	{
	delete iMsg;
	}


#endif // TFXSERVER_TIME_LOG

