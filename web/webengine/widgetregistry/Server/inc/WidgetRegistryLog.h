/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Log macro layer over flogger.h
*
*
*/


#ifndef WIDGETREGISTRYLOG_H
#define WIDGETREGISTRYLOG_H

#ifdef _DEBUG

#include <bautils.h>
#include <eikenv.h>
#include <flogger.h>

// add to class decl

#define LOG_MEMBER_VARS \
 RFileLogger iLog; TInt iLogOpenCount; TFileLoggingMode iLogMode;

// use remainder in impl

#define LOG_NAMES( cstr_log_dir, cstr_log_file ) \
 _LIT( KLogDir, cstr_log_dir ); \
 _LIT( KLogFile, cstr_log_file );

#define LOG_CONSTRUCTL \
 iLogOpenCount = 0; \
 User::LeaveIfError( iLog.Connect() ); \
 iLog.CreateLog( KLogDir, KLogFile, EFileLoggingModeOverwrite )

#define LOG_DESTRUCT \
  if ( iLogOpenCount ) iLog.CloseLog(); \
  iLog.Close();

#define LOG_OPEN \
 if ( !iLogOpenCount++ ) \
  iLog.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend )

#define LOG_CLOSE \
 if ( iLogOpenCount && !(--iLogOpenCount) ) iLog.CloseLog()

#define LOG_FLUSH \
 do { \
  if ( iLogOpenCount ) { \
   iLog.CloseLog(); \
   iLog.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend ); \
  } \
 } while ( 0 )

#define LOG( cstr ) \
 iLog.Write( _L(cstr) )

#define LOG_DES( des ) \
 iLog.Write( des )

#define LOG1( cstr_fmt, arg1 ) \
 iLog.WriteFormat( _L(cstr_fmt), arg1 )

#define LOG2( cstr_fmt, arg1, arg2 ) \
 iLog.WriteFormat( _L(cstr_fmt), arg1, arg2 )

#define LOG3( cstr_fmt, arg1, arg2, arg3 ) \
 iLog.WriteFormat( _L(cstr_fmt), arg1, arg2, arg3 )

#define LOG4( cstr_fmt, arg1, arg2, arg3, arg4 ) \
 iLog.WriteFormat( _L(cstr_fmt), arg1, arg2, arg3, arg4 )

#define LOG_CODE( code ) code

#else // not _DEBUG

#define LOG_NAMES( cstr_log_dir, cstr_log_file )
#define LOG_MEMBER_VARS
#define LOG_CONSTRUCTL
#define LOG_DESTRUCT
#define LOG_OPEN
#define LOG_CLOSE
#define LOG_FLUSH
#define LOG( cstr )
#define LOG_DES( des )
#define LOG1( cstr_fmt, arg1 )
#define LOG2( cstr_fmt, arg1, arg2 )
#define LOG3( cstr_fmt, arg1, arg2, arg3 )
#define LOG4( cstr_fmt, arg1, arg2, arg3, arg4 )
#define LOG_CODE( code )

#endif // _DEBUG

#endif  // WIDGETREGISTRY_H
