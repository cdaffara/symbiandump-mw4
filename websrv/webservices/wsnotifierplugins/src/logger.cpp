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
* Description:        
*
*/








#ifdef __CUSTOM_LOG_ENABLED__

// INCLUDE FILES
#include "Logger.h"

#include <f32file.h>

void DoLog(const TDesC& aText)
    {
    static const TInt KLogEntryMaxLength = 128;
    _LIT(KLineFeed, "\n");



    /**
     *  Log time format (see TTime) is
     *  Day-Month-Year Hours:Minutes:Seconds:Milliseconds
     *
     *  Example: 30-12-2004 23:00:55:990
     */
    _LIT(KLogTimeFormat, "%F%D-%M-%Y %H:%T:%S:%*C3");

    TBuf8<KLogEntryMaxLength> writeBuffer;
    RFs FileServer;
    RFile File;

    if(FileServer.Connect() != KErrNone)
        {
        FileServer.Close(); // just in case
        User::Panic(KLogPanicCategory(), KPanicFsConnectFailed);
        return;
        }

    // Open file for writing, if exists. Othervise create new file.
    if(File.Open(FileServer, KLogFileName(), EFileWrite) != KErrNone)
        {
        if(File.Create(FileServer, KLogFileName(), EFileWrite) != KErrNone)
            {
            FileServer.Close();
            User::Panic(KLogPanicCategory(), KPanicFileCreateFailed);
            }
        }

    TTime currentTime;
    currentTime.UniversalTime();
    TBuf<32> timeString;

    // currentTime is now in universal time. Convert it to home time.
    TLocale locale;
    TTimeIntervalSeconds universalTimeOffset(locale.UniversalTimeOffset());
    TTimeIntervalHours daylightSaving(0);
    if(locale.QueryHomeHasDaylightSavingOn())
        {
        daylightSaving = 1;
        }
    currentTime = currentTime + universalTimeOffset + daylightSaving;
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, currentTime.FormatL(timeString, KLogTimeFormat));
    leaveCode = 0; // not used
    // Add LogString to the end of file and close the file
    TInt currentSize = 0, returnCode;
    writeBuffer.Append(timeString);
    writeBuffer.Append(_L(": "));
    writeBuffer.Append(aText.Left(KLogEntryMaxLength-timeString.Length()));
    writeBuffer.Append(KLineFeed);
    File.Size(currentSize);
    returnCode = File.Write(currentSize,
                            writeBuffer,
                            writeBuffer.Length());
    File.Close();
    // Close file server session
    FileServer.Close();

    if(returnCode != KErrNone)
        {
        User::Panic(KLogPanicCategory(), KPanicFileWriteFailed);
        }
    }

#endif //__CUSTOM_LOG_ENABLED__
