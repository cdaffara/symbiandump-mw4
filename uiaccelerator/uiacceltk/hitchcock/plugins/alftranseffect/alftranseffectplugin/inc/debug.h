/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef DEBUG_H
#define DEBUG_H

#include <e32debug.h>

#ifdef _DEBUG //No debugging unless in Debug mode...
	//#define DEBUG_ON_TARGET //define to get debug output also on target.
	#ifdef __WINS__
		//This is the config on emulator.
		//#define DEBUG_PRINTS 
		//#define DEBUGPRINTS_AS_FLOG

	#elif defined(DEBUG_ON_TARGET)
		//This is the config for target, when debuging on target is on.
		#define DEBUG_PRINTS
		//#define DEBUGPRINTS_AS_FLOG

	#endif
#endif
//#define FLOG_PRINTS //This can be used to use FLOGs without support for DEBUG_PRINTS.. probably not useful...



#if defined(FLOG_PRINTS) || defined(DEBUGPRINTS_AS_FLOG)

	#include <flogger.h>
	_LIT(KLogFile, "AlfFxServerLog.txt");
	_LIT(KLogDir, "AlfGfxtest");

	#define FLOG(x) RFileLogger::Write(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x))
	#define FLOG1(x,a1) RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x),a1)
	#define FLOG2(x,a1,a2) RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x),a1,a2)
	#define FLOG3(x,a1,a2,a3) RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x),a1,a2,a3)
	#define FLOG4(x,a1,a2,a3,a4) RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x),a1,a2,a3,a4)
	#define FLOG5(x,a1,a2,a3,a4,a5) RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,_LIT(x),a1,a2,a3,a4,a5)

#else

	#define FLOG(x)
	#define FLOG1(x,a1)
	#define FLOG2(x,a1,a2)
	#define FLOG3(x,a1,a2,a3)
	#define FLOG4(x,a1,a2,a3,a4) 
	#define FLOG5(x,a1,a2,a3,a4,a5) 

#endif

#if defined(DEBUG_PRINTS) && !defined(DEBUGPRINTS_AS_FLOG)
	// Prints via RDebug
	#define DEBUG_PRINT(x) RDebug::Printf("AFX:  "##x)
	#define DEBUG_PRINT1(x, a1) RDebug::Printf("AFX:  "##x, a1)
	#define DEBUG_PRINT2(x, a1, a2) RDebug::Printf("AFX:  "##x, a1, a2)
	#define DEBUG_PRINT3(x, a1, a2, a3) RDebug::Printf("AFX:  "##x, a1, a2, a3)
	#define DEBUG_PRINT4(x, a1, a2, a3, a4) RDebug::Printf("AFX:  "##x, a1, a2, a3, a4)
	#define DEBUG_PRINT5(x, a1, a2, a3, a4, a5) RDebug::Printf("AFX:  "##x, a1, a2, a3, a4, a5)

#elif defined(DEBUGPRINTS_AS_FLOG)
	// Prints via flogger
	#define DEBUG_PRINT(x) FLOG(x)
	#define DEBUG_PRINT1(x, a1) FLOG1(x, a1)
	#define DEBUG_PRINT2(x, a1, a2) FLOG2(x, a1, a2)
	#define DEBUG_PRINT3(x, a1, a2, a3) FLOG3(x, a1, a2, a3)
	#define DEBUG_PRINT4(x, a1, a2, a3, a4) FLOG4(x, a1, a2, a3, a4)
	#define DEBUG_PRINT5(x, a1, a2, a3, a4, a5) FLOG5(x, a1, a2, a3, a4, a5)

#else 
	/*lint -e960 */
	// No prints
	#define DEBUG_PRINT(x) 
	#define DEBUG_PRINT1(x, a1) 
	#define DEBUG_PRINT2(x, a1, a2) 
	#define DEBUG_PRINT3(x, a1, a2, a3) 
	#define DEBUG_PRINT4(x, a1, a2, a3, a4) 
	#define DEBUG_PRINT5(x, a1, a2, a3, a4, a5)
	/*lint +e960 */
	
#endif // DEBUG_PRINTS

#endif // DEBUG_H
