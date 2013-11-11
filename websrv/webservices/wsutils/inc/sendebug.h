/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           Central place for debug-type macros
*
*/











#ifndef SEN_DEBUG_H
#define SEN_DEBUG_H

// Note that _DEBUG is automatically defined for debug builds

/**
* To enable file logging: please comment following #ifdef line and 
* uncomment #ifdef TRUE
*/
#ifdef _DEBUG

//#ifdef TRUE // to enable logging & debug in all builds
//#ifndef TRUE // to disable logging & debug in all builds

//  INCLUDES
#include <e32svr.h>
#include <flogger.h>

// MACROS
#define _SENDEBUG   1
#define SENDEBUG(parameters)        RDebug::Print parameters
#define SENDEBUG_L(string)          RDebug::Print(_L(string))   // single param

#define LOG_CONNECT                 Log()->Connect()
#define LOG_CREATELOG(parameters)   Log()->CreateLog parameters
#define LOG_WRITE(parameters)       Log()->Write parameters
#define LOG_WRITE_L(string)         Log()->Write(_L(string))    // single param
#define LOG_WRITEFORMAT(parameters) Log()->WriteFormat parameters
#define LOG_CLOSELOG                Log()->CloseLog()
#define LOG_CLOSE                   Log()->Close()


//#define LOG_WRITEALL(parameters)    Log()->Write parameters

#define LOG_WRITEALL(parameters) \
    { \
    TInt ___iZen___ = 0; \
    TInt ___amountZen___ = 100; \
    while(___iZen___<(parameters).Length()) \
        { \
        if(( (parameters).Length()-___iZen___) < 100) \
            ___amountZen___ = (parameters).Length()-___iZen___; \
        Log()->Write((parameters).Mid(___iZen___,___amountZen___)); \
        ___iZen___+=100; \
        } \
    }


#define FILELOG(dir, file, string)          RFileLogger::Write(dir, file, EFileLoggingModeOverwriteRaw, string);
#define FILELOGAPPEND(dir, file, string)    RFileLogger::Write(dir, file, EFileLoggingModeAppendRaw, string);

#define FILELOGALL(dir, file, longstring) \
    { \
    TInt ___iZen2___ = 0; \
    TInt ___amountZen2___ = 100; \
    FILELOG(dir, file, KNullDesC8()); \
    while(___iZen2___<longstring.Length()) \
        { \
        if(( longstring.Length()-___iZen2___) < 100) \
            ___amountZen2___ = longstring.Length()-___iZen2___; \
        if(___iZen2___==0) FILELOG(dir, file, longstring.Mid(___iZen2___,___amountZen2___)) \
        else FILELOGAPPEND(dir, file, longstring.Mid(___iZen2___,___amountZen2___)) \
        ___iZen2___+=100; \
        } \
    }
#else

#define SENDEBUG(parameters)
#define SENDEBUG_L(string)

#define LOG_CONNECT
#define LOG_CREATELOG(parameters)
#define LOG_WRITE(parameters)
#define LOG_WRITE_L(string)
#define LOG_WRITEFORMAT(parameters)
#define LOG_CLOSELOG
#define LOG_CLOSE
#define LOG_WRITEALL(parameters)

#define FILELOG(dir, file, string)
#define FILELOGAPPEND(dir, file, string)
#define FILELOGALL(dir, file, parameters)

#endif // _DEBUG

#endif // SEN_DEBUG_H

// End of File