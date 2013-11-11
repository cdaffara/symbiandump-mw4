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
* Description: 
*      Logging macros for DD Recognizer.   
*      
*
*/


#ifndef DD_RECOG_LOGGER_H
#define DD_RECOG_LOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

#ifdef _DEBUG

/// DD logging directory.
_LIT( KDdLogDir,                  "Dd" );
/// DD log file name.
_LIT( KDdLogFile,                  "DdRecog.txt" );
/// Format string: enter function.
_LIT( KDdLogEnterFn,               "-> %S" );
/// Format string: leave function.
_LIT( KDdLogLeaveFn,               "<- %S" );
/// Format string: time.
_LIT( KDdLogTimeFormatString,      "%H:%T:%S:%*C2" );
/// Format string: timestamp.
_LIT( KDdLogTimeStampFormatString, "%S %S" );

/// Write log: enter function.
#define CLOG_ENTERFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDdLogDir,                 \
        KDdLogFile,                \
        EFileLoggingModeAppend,     \
        KDdLogEnterFn,             \
        &temp                       \
        );                          \
    }

/// Write log: leave function.
#define CLOG_LEAVEFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDdLogDir,                 \
        KDdLogFile,                \
        EFileLoggingModeAppend,     \
        KDdLogLeaveFn,             \
        &temp                       \
        );                          \
    }

/// Write log: string 'a'.
#define CLOG_WRITE( a )             \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::Write              \
        (                           \
        KDdLogDir,                 \
        KDdLogFile,                \
        EFileLoggingModeAppend,     \
        temp                        \
        );                          \
    }

/// Write log: formatted.
#define CLOG_WRITE_FORMAT( a, b )   \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KDdLogDir,                 \
        KDdLogFile,                \
        EFileLoggingModeAppend,     \
        temp,                       \
        b                           \
        );                          \
    }

/// Write log: timestamp.
#define CLOG_WRITE_TIMESTAMP( a )                                   \
    {                                                               \
    _LIT( temp, a );                                                \
    TTime time;                                                     \
    time.HomeTime();                                                \
    TBuf<32> timeBuf;                                               \
    TRAPD( err, time.FormatL( timeBuf, KDdLogTimeFormatString ) ); \
    if ( !err )                                                     \
        {                                                           \
        RFileLogger::WriteFormat                                    \
            (                                                       \
            KDdLogDir,                                             \
            KDdLogFile,                                            \
            EFileLoggingModeAppend,                                 \
            KDdLogTimeStampFormatString,                           \
            &temp,                                                  \
            &timeBuf                                                \
            );                                                      \
        }                                                           \
    }

#else /* not defined _DEBUG */

/// Empty definition (disable log).
#define CLOG_ENTERFN( a )

/// Empty definition (disable log).
#define CLOG_LEAVEFN( a )

/// Empty definition (disable log).
#define CLOG_WRITE( a )

/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a, b )

/// Empty definition (disable log).
#define CLOG_WRITE_TIMESTAMP( a )

#endif /* def _DEBUG */

#endif /* def DD_RECOG_LOGGER_H */
