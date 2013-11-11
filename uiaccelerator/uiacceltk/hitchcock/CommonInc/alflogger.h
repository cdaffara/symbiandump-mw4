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
* Description:  
*       Macro definition file for logging.
*
*/




#ifndef ALFLOGGER_H
#define ALFLOGGER_H

#include "alfloggingconfiguration.h"
#include <e32std.h>
#include <flogger.h>
#include <e32svr.h>

_LIT( KLogsDir,                  "alf");
_LIT( KLogFileName,              "alf.log");

/*
 We have 3 possible logging methods:

 set ALF_LOGGING_METHOD to 0 to get no logging at all
 set ALF_LOGGING_METHOD to 1 to get logging via RDebug::Print
 set ALF_LOGGING_METHOD to 2 to get logging to log file

 Switching on/off is done in the Configuration file (AlfLoggingConfiguration.h)
 */

#if ( defined (_ALF_LOGGING) )
    #if ( defined (_ALF_LOGGING_TO_FILE))
        #define ALF_LOGGING_METHOD 2 
    #else
        #define ALF_LOGGING_METHOD 1
    #endif
#else
    #define ALF_LOGGING_METHOD 0
#endif


#if (ALF_LOGGING_METHOD==0)
    #define __ALFLOGSTRING(C)
    #define __ALFLOGSTRING1(C, X)
    #define __ALFLOGSTRING2(C, X, Y)
    #define __ALFLOGSTRING3(C, X, Y, Z)
    #define __ALFLOGSTRING4(C, X, Y, Z, W)
#endif
#if (ALF_LOGGING_METHOD==1)
    #define __ALFLOGSTRING(C)            RDebug::Print(_L(C));
    #define __ALFLOGSTRING1(C, X)        RDebug::Print(_L(C),X);
    #define __ALFLOGSTRING2(C, X, Y)     RDebug::Print(_L(C),X, Y);
    #define __ALFLOGSTRING3(C, X, Y, Z)  RDebug::Print(_L(C),X, Y, Z);
    #define __ALFLOGSTRING4(C,X,Y,Z,W)   RDebug::Print(_L(C),X, Y, Z, W);
#endif
#if (ALF_LOGGING_METHOD==2)
    #define __ALFLOGSTRING(C)            {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, tempLogDes());}
    #define __ALFLOGSTRING1(C, X)        {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X);}
    #define __ALFLOGSTRING2(C, X, Y)     {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y);}
    #define __ALFLOGSTRING3(C, X, Y, Z)  {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z);}
    #define __ALFLOGSTRING4(C,X,Y,Z,W)   {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z,W);}
#endif

// Logging for effects components
#if ( defined (_ALF_FXLOGGING) )
    #if ( defined (_ALF_LOGGING_TO_FILE))
        #define ALF_FXLOGGING_METHOD 2 
    #else
        #define ALF_FXLOGGING_METHOD 1
    #endif
#else
    #define ALF_FXLOGGING_METHOD 0
#endif

#if (ALF_FXLOGGING_METHOD==0)
    #define __ALFFXLOGSTRING(C)
    #define __ALFFXLOGSTRING1(C, X)
    #define __ALFFXLOGSTRING2(C, X, Y)
    #define __ALFFXLOGSTRING3(C, X, Y, Z)
    #define __ALFFXLOGSTRING4(C, X, Y, Z, W)

#endif
#if (ALF_FXLOGGING_METHOD==1)
    #define __ALFFXLOGSTRING(C)            RDebug::Print(_L(C));
    #define __ALFFXLOGSTRING1(C, X)        RDebug::Print(_L(C),X);
    #define __ALFFXLOGSTRING2(C, X, Y)     RDebug::Print(_L(C),X, Y);
    #define __ALFFXLOGSTRING3(C, X, Y, Z)  RDebug::Print(_L(C),X, Y, Z);
    #define __ALFFXLOGSTRING4(C,X,Y,Z,W)   RDebug::Print(_L(C),X, Y, Z, W);
#endif
#if (ALF_FXLOGGING_METHOD==2)
    #define __ALFFXLOGSTRING(C)            {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, tempLogDes());}
    #define __ALFFXLOGSTRING1(C, X)        {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X);}
    #define __ALFFXLOGSTRING2(C, X, Y)     {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y);}
    #define __ALFFXLOGSTRING3(C, X, Y, Z)  {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z);}
    #define __ALFFXLOGSTRING4(C, X, Y, Z, W)   {_LIT(tempLogDes,C);RFileLogger::WriteFormat(KLogsDir(),KLogFileName(), EFileLoggingModeAppend, TRefByValue<const TDesC>(tempLogDes()),X,Y,Z,W);}
#endif

#endif // ALFLOGGER_H

// End of File
