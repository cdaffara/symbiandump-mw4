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

#ifndef _MEMORYLOGGING_H
#define _MEMORYLOGGING_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>
#include <f32file.h>

#ifdef OOM_LOGGING
// CONSTANTS

_LIT( KMEMLogDir,               "WebCore");
_LIT( KMEMLogFile,              "Memory.log");
_LIT( KChunkLogFile,             "freechunks.log");
_LIT8( KFuncMemLogBegin, "BEGIN: %S @ %S/%d InSize -> %d" );
_LIT8( KFuncMemLogEnd, "END: Peek -> %d Diff -> %d Accumulated Peek -> %d" );
_LIT8( KTab, "\t" );

// MACROS

#define MEM_LOG(a)		{ _LIT8(temp, a); RFileLogger::Write(KMEMLogDir, KMEMLogFile, EFileLoggingModeAppend, temp); }
#define MEM_LOGF		FPrint
#define C_LOG(a)      { _LIT8(temp, a); RFileLogger::Write(KMEMLogDir, KChunkLogFile, EFileLoggingModeAppend, temp); }
#define C_LOGF        FPrint2

// FUNCTION DECLARATIONS

inline void FPrint(const TRefByValue<const TDesC8> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMEMLogDir, KMEMLogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void FPrint2(const TRefByValue<const TDesC8> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMEMLogDir, KChunkLogFile, EFileLoggingModeAppend, aFmt, list);
}

// CLASS DECLARATION

/**
*  A utility class to record the memory usage inside a function
*  @lib memman.lib
*  @since 3.1
*/
class FunctionLogger
	{
	public:
		inline FunctionLogger( const TDesC8& func, const TDesC8& file, TInt line, TInt size );
		inline ~FunctionLogger();
			
		TInt _peek;
		TInt _used;
		TInt _accum;
		TInt _inSize;
		static RPointerArray<FunctionLogger> loggers;
	};

// MEMBER FUNCTIONS

//-----------------------------------------------------------------------------
// FunctionLogger::FunctionLogger
//-----------------------------------------------------------------------------
inline FunctionLogger::FunctionLogger( const TDesC8& func, const TDesC8& file, TInt line, TInt size ) 
	: _peek( 0 ), _used( 0 ), _accum( 0 )
	{
		TBuf8<512>	buf;
			
		// indent
		for( TInt i=0; i<loggers.Count(); ++i )	buf.Append( KTab );
		
		buf.Append( KFuncMemLogBegin );
		
		// write the logger info
		MEM_LOGF( buf, &func, &file, line, size );
		_inSize = size;
			
		// insert this logger to the head of loggers array
		loggers.Insert( this, 0 );
	}

//-----------------------------------------------------------------------------
// FunctionLogger::~FunctionLogger
//-----------------------------------------------------------------------------
inline FunctionLogger::~FunctionLogger()
	{
		TBuf8<512>	buf;
			
		// indent
		for( TInt i=0; i<loggers.Count()-1; ++i )	buf.Append( KTab );
		
		buf.Append( KFuncMemLogEnd );
		
		_accum += _peek;
			
		MEM_LOGF( buf, _peek, _used, _accum );
		
		
		// remove the logger
		loggers.Remove( 0 );
		
		// update its parents' _accum
		if( loggers.Count() > 0 )
			for( TInt i=0; i<loggers.Count(); ++i )
				loggers[ i ]->_accum += _peek;
	}
#else // OOM_LOGGING
#define MEM_LOG(a)
#define MEM_LOGF
#define C_LOG(a)
#define C_LOGF
#endif

#endif
// END OF FILE