/*
* Copyright (c) 2005, 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file defines logging macros for USB UI Notifiers
*
*/


#ifndef USBUINOTIFDEBUG_H
#define USBUINOTIFDEBUG_H

#ifdef _DEBUG

#include <e32svr.h>
#include <e32std.h>

// ===========================================================================
#ifdef __WINS__     // File logging for WINS
// ===========================================================================
#define FLOG(a)   { FPrint(a); }
#define FTRACE(a) { a; }

#include <f32file.h>
#include <flogger.h>

_LIT(KLogFile,"usbuinotiflog.txt");
_LIT(KLogDir,"USBUI");

// Declare the FPrint function
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend,
        aFmt, list);
    }


// ===========================================================================
#else               // Target HW
// ===========================================================================

// ===========================================================================
#ifdef _USBUINOTIF_FILE_LOGGING_ // File logging for target HW
// ===========================================================================

#define FLOG(a)   { FPrint(a); }
#define FTRACE(a) { a; }

#include <f32file.h>
#include <flogger.h>

_LIT(KLogFile,"usbuinotiflog.txt");
_LIT(KLogDir,"USBUI");

// Declare the FPrint function
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend,
        aFmt, list);
    }

// ===========================================================================
#else    // RDebug logging for target HW
// ===========================================================================
#define FLOG(a) { RDebug::Print(a);  }
#define FTRACE(a) { a; }

//Declare the FPrint function with variable number of arguments.
inline void FPrint(const TRefByValue<const TDesC16> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    TInt tmpInt = VA_ARG(list, TInt);
    TInt tmpInt2 = VA_ARG(list, TInt);
    TInt tmpInt3 = VA_ARG(list, TInt);
    VA_END(list);
    RDebug::Print(aFmt, tmpInt, tmpInt2, tmpInt3);
    }

//Declare the FPrint function with variable number of arguments.
inline void FPrint(TRefByValue<const TDesC8> aFmt, ...)
    {

    VA_LIST list;
    VA_START(list, aFmt);
    TBuf8<256> buf8;
    buf8.AppendFormatList(aFmt, list);

    TBuf16<256> buf16(buf8.Length());
    buf16.Copy(buf8);

    TRefByValue<const TDesC> tmpFmt(_L("%S"));
    RDebug::Print(tmpFmt, &buf16);
    }
#endif //_USBUINOTIF_FILE_LOGGING_    
#endif // __WINS__

// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG(a)
#define FTRACE(a)

#endif // _DEBUG


#endif // USBUINOTIFDEBUG_H

// End of File
