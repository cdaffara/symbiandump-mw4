/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia. All
* rights are reserved. Copying, including reproducing, storing
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia. This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Debug macros and declarations. 
*
*/

#ifndef DEBUG_H
#define DEBUG_H

//////////////////////////////////////////////////////////////////////////////
// Here are parameters need to be modified. 
// Component name 
#define MODULE_NAME "USBOTGWATCHER"
//////////////////////////////////////////////////////////////////////////////


// For handling unicode strings as L##MACRO is interpreted as LMACRO
#define WIDEN2( x ) L##x
#define WIDEN( x ) WIDEN2( x )

_LIT( KUsbPanicModule, ""WIDEN( MODULE_NAME ) );

#ifdef _DEBUG

// Comment this line out if using real-time debugging
//#define LOG_TO_FILE 
// ===========================================================================
#if defined ( LOG_TO_FILE )
// ===========================================================================
// Add corresponding libabry in mmp file
// LIBRARY    flogger.lib    
// The log file is located at  
// c:\logs\usb\MODULE_NAME.txt

#include <flogger.h>

#define DESC( x ) TPtrC( ( const TText* ) ( x ) )
#define DESC8( x ) TPtrC8( ( const TUint8* ) ( x ) )
_LIT( KUsbLogDir, "USB" );
_LIT( KUsbLogFile, ""WIDEN(MODULE_NAME)L".txt" );

// s: c string. 
#define LOG(s) RFileLogger::WriteFormat( KUsbLogDir, \
                KUsbLogFile, EFileLoggingModeAppend, \
                DESC8( "["MODULE_NAME"] "##s ) )

// s: c string contains "%x"
// v: varaible corresponding to %x
#define LOG1(s, v) RFileLogger::WriteFormat( KUsbLogDir, \
                KUsbLogFile, EFileLoggingModeAppend, \
                DESC8( "["MODULE_NAME"] "##s ), v)
// s: c string contains "%x%y"
// v1: varaible corresponding to %x
// v2: varaible corresponding to %y
#define LOG2(s, v1, v2) RFileLogger::WriteFormat( KUsbLogDir, \
                KUsbLogFile, EFileLoggingModeAppend, \
                DESC8( "["MODULE_NAME"] "##s ), v1, v2)

// s: c string contains "%x%y%z"
// v1: varaible corresponding to %x
// v2: varaible corresponding to %y
// v3: varaible corresponding to %z
#define LOG3(s, v1, v2, v3) RFileLogger::WriteFormat( KUsbLogDir, \
            KUsbLogFile, EFileLoggingModeAppend, \
            DESC8( "["MODULE_NAME"] "##s ), v1, v2, v3 )

class TFuncLogger
    {
public:
    TFuncLogger(const TDesC8& aFuncName)
        {
        iFuncName.Set( aFuncName );
        RFileLogger::WriteFormat( KUsbLogDir, KUsbLogFile, EFileLoggingModeAppend, 
                DESC8( "["MODULE_NAME"] >>%s" ), iFuncName.Ptr() );
        }
    ~TFuncLogger()
        {
        RFileLogger::WriteFormat( KUsbLogDir, KUsbLogFile, 
                EFileLoggingModeAppend, 
                DESC8( "["MODULE_NAME"] <<%s" ), iFuncName.Ptr() );
        }
private:
    TPtrC8 iFuncName;
    };
// ===========================================================================
#else //LOG_TO_FILE not defined
// ===========================================================================
#include <e32debug.h>

// Paramters same as above.
#define LOG( str ) { RDebug::Printf( "["MODULE_NAME"] %s", str  ); }

#define LOG1( s, v ) { RDebug::Printf( "["MODULE_NAME"] "##s , v ); }

#define LOG2( s, v1,v2 ) { RDebug::Printf( "["MODULE_NAME"] "##s , v1, v2 ); }

#define LOG3( s, v1, v2, v3 ) { RDebug::Printf( "["MODULE_NAME"] "##s , \
        v1, v2, v3); }

class TFuncLogger
    {
public:
    TFuncLogger(const TPtrC8& aFuncName) : iFuncName( aFuncName )
        {
        RDebug::Printf( "["MODULE_NAME"] >>%s", iFuncName.Ptr());
        }
    ~TFuncLogger()
        {
        RDebug::Printf( "["MODULE_NAME"] <<%s", iFuncName.Ptr());
        }
private:
    TPtrC8 iFuncName;
    };
#endif // LOG_TO_FILE

#define LEAVE( exp )  {volatile TInt err_ = exp; \
        LOG3( "LEAVE(%d) @file: %s, line: %d", err_, __FILE__, __LINE__ );\
        User::Leave( err_ );}

#define LEAVEIFERROR( exp ) {volatile TInt err__ = exp; \
        if(err__ < 0) LEAVE(err__);}

#define PANIC( exp ) {volatile TInt err_ = exp; \
        LOG3( "PANIC(%d) @file: %s, line: %d", err_, __FILE__, __LINE__ );\
        User::Panic( KUsbPanicModule, err_ );} 

#define LOG_FUNC TFuncLogger __instrument(TPtrC8((TUint8*)__PRETTY_FUNCTION__));

#define ASSERT_PANIC( exp, code ) {if(!(exp)) PANIC(code)}

#else // _DEBUG not defined 
// ===========================================================================

#define LOG( s )
#define LOG1( s, v ) 
#define LOG2( s, v1, v2 )
#define LOG3( s, v1, v2, v3 )
#define LOG_FUNC_ENTRY 
#define LOG_FUNC_EXIT 
#define LEAVE( exp ) User::Leave( exp );
#define LEAVEIFERROR( exp ) User::LeaveIfError( exp );
#define PANIC( err ) // in non-debug builds PANICs are silent
#define LOG_FUNC
#define ASSERT_PANIC( exp, code )
// ===========================================================================
#endif // _DEBUG
// ===========================================================================
#endif // DEBUG_H

// End of File
