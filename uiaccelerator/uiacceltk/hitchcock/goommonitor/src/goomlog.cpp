/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logging functionality for GOOM monitor profiling
*
*/


#ifdef _DEBUG

#include <e32hal.h>
#include <hal.h>
#include <w32std.h>
#include <e32std.h>
#include <apgwgnam.h>
#include <flogger.h>

#include "goomlog.h"

_LIT8(KMemorySampleLoggingString, "%d");
_LIT8(KMemorySampleLoggingSeparator, ", ");

_LIT8(KCrLf, "\r\n");

_LIT8(KGOomLogCancel, "Sampling triggered before previous sampling has completed. Results so far: ");

_LIT(KGOomLogFile, "c:\\logs\\GOOM\\livegoommonitor.txt");
_LIT(KGOomOldLogFile, "c:\\logs\\GOOM\\goommonitor.txt");

const TInt KMaxTimeStampSize = 30;
_LIT(KTimeStampFormat, "%F    %H:%T:%S");

_LIT(KDummyWgName, "20");

_LIT8(KUidPreamble, "App UIDs:");
_LIT8(KUidFormat, " 0x%x");

const TInt KPreallocatedSpaceForAppList = 50;

const TInt KMaxUidBufferSize = 1024;

CGOomLogger* CGOomLogger::NewL(RWsSession& aWs, RFs& aFs)
    {
    CGOomLogger* self = new (ELeave) CGOomLogger(aWs, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// Start logging the available memory every n micro seconds
// Firstly a list of the app IDs is written to the log (foreground app first)
// Note that the log is created in memory (to a pre-allocated buffer) and flushed out after it is complete
// the samples are saved in CSV format so that they can easily be cut and pasted to plot graphs etc.
void CGOomLogger::StartL()
    {
    // If the log file doesn't exist then don't attempt to sample anything
    if (!iIsOpen)
        return;
    
    // If we are already active then cancel first
    if (IsActive())
        Cancel();
    
    iWriteBuffer.Zero();
    
    iStartTime.HomeTime();
    
    // Log the timestamp
    TBuf16<KMaxTimeStampSize> timeStamp;
    iStartTime.FormatL(timeStamp, KTimeStampFormat);
    TBuf8<KMaxTimeStampSize> timeStamp8;
    timeStamp8.Copy(timeStamp);
    Write(timeStamp8);
    
    // Log all of the application IDs (foreground app first, then the other apps moving towards the back)
    LogApplicationIds();

    // Then, record the free memory
    // Note that this is done to a buffer so as not to affect the timing too much
    LogFreeMemory();
    
    // Finally, set a timer to record the memory every n microseconds
    HighRes(KTimeBetweenMemorySamples);
    }

// From CTimer / CActice
void CGOomLogger::RunL()
    {
    TTime currentTime;
    currentTime.HomeTime();
    TTimeIntervalMicroSeconds loggingDuration = currentTime.MicroSecondsFrom(iStartTime);
    TTimeIntervalMicroSeconds samplingDuration = KSamplingDurationUint;
    if (loggingDuration > samplingDuration)
        // If we have passed the desired logging duration then write the data we have collected
        {
        Write(iWriteBuffer);
        }
    else
        {
        // If we haven't passed the desired logging duration then record the free memory
        // Note that this is recorded into a buffer and then logged later
        iWriteBuffer.Append(KMemorySampleLoggingSeparator);
        LogFreeMemory();
        
        // Wait before taking another memory sample
        HighRes(KTimeBetweenMemorySamples);
        }
    }

void CGOomLogger::DoCancel()
    {
    CTimer::DoCancel();
    
    Write(KGOomLogCancel);
    Write(iWriteBuffer);
    }

CGOomLogger::~CGOomLogger()
    {
    iWgIds.Close();
 // delete iWgName; // Not owned
    if (iIsOpen)
        iFile.Close();

    }

void CGOomLogger::Write(const TDesC8& aBuffer)
    {
    if (iIsOpen)
        {
        iFile.Write(aBuffer);
    
        // Add the line break
        iFile.Write(KCrLf);
        }
    }

void CGOomLogger::LogApplicationIds()
    {
    // get all window groups, with info about parents
    TInt numGroups = iWs.NumWindowGroups(0);
    TRAPD(err, iWgIds.ReserveL(numGroups));
    
    if (err != KErrNone)
        return;
    
    if (iWs.WindowGroupList(0, &iWgIds) != KErrNone)
        return;

    // Remove all child window groups, promote parents to foremost child position
    ColapseWindowGroupTree();
    
    // Go through each window group ID in the list, get the Uid of each app then log it
    // Start with the foreground application
    TInt index = 0;
    
    TUid uid;
    
    TBuf8<KMaxUidBufferSize> uidBuffer;
    
    uidBuffer = KUidPreamble;
    
    while (index < numGroups)
        {
        uid = GetUidFromWindowGroupId(iWgIds[index].iId);
        
        uidBuffer.AppendFormat(KUidFormat, uid.iUid);
        
        index++;
        }
    
    Write(uidBuffer);
    }

void CGOomLogger::LogFreeMemory()
    {
    TMemoryInfoV1Buf meminfo;
    UserHal::MemoryInfo(meminfo);
    TInt freeMem = meminfo().iFreeRamInBytes;

    // Save the free memory to a descriptor which will be written later
    iWriteBuffer.AppendFormat(KMemorySampleLoggingString(), freeMem);
    }

CGOomLogger::CGOomLogger(RWsSession& aWs, RFs& aFs) : CTimer(EPriorityStandard), iWs(aWs), iFs(aFs)
    {
    }

void CGOomLogger::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    CTimer::ConstructL();
    
    // If there is an existing log then copy it, this will be the log that can be sent to the PC
    // Without this feature then you can't get the logs off of the device because the "live" log will always be is use.
    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);
    fileMan->Copy(KGOomLogFile, KGOomOldLogFile);
    CleanupStack::PopAndDestroy(fileMan);
    
    // Create the log file, or open it if is already exists (note that the directory must already be present
    TInt err = iFile.Create(iFs, KGOomLogFile, EFileShareAny | EFileWrite);
    if (KErrNone != err)
        {
        err = iFile.Open(iFs, KGOomLogFile, EFileShareAny | EFileWrite);
        }
    
    if (KErrNone == err)
        {
        iIsOpen = ETrue;
        
        // Append all new data to the end of the file
        TInt offset = 0;
        iFile.Seek(ESeekEnd, offset);
        }
    
    // Reserve enough space to build an app list later.
    iWgIds.ReserveL(KPreallocatedSpaceForAppList);
    // Reserve enough space for CApaWindowGroupName.
    iWgName = CApaWindowGroupName::NewL(iWs);
    iWgNameBuf = HBufC::NewL(CApaWindowGroupName::EMaxLength);
    (*iWgNameBuf) = KDummyWgName;
    iWgName->SetWindowGroupName(iWgNameBuf);    // iWgName takes ownership of iWgNameBuf*/

    }

void CGOomLogger::ColapseWindowGroupTree()
    {
    // start from the front, wg count can reduce as loop runs
    for (TInt ii=0; ii<iWgIds.Count();)
        {
        RWsSession::TWindowGroupChainInfo& info = iWgIds[ii];
        if (info.iParentId > 0)        // wg has a parent
            {
            // Look for the parent position
            TInt parentPos = ii;        // use child pos as not-found signal
            TInt count = iWgIds.Count();
            for (TInt jj=0; jj<count; jj++)
                {
                if (iWgIds[jj].iId == info.iParentId)
                    {
                    parentPos = jj;
                    break;
                    }
                }

            if (parentPos > ii)  // parent should be moved forward
                {
                iWgIds[ii] = iWgIds[parentPos];
                iWgIds.Remove(parentPos);
                }
            else if (parentPos < ii)  // parent is already ahead of child, remove child
                iWgIds.Remove(ii);
            else                    // parent not found, skip
                ii++;
            }
        else    // wg does not have a parent, skip
            ii++;
        }
    }

TUid CGOomLogger::GetUidFromWindowGroupId(TInt aWgId)
    {
    // get the app's details
    TPtr wgPtr(iWgNameBuf->Des());
    
    TUid uid;
    
    TInt err = iWs.GetWindowGroupNameFromIdentifier(aWgId, wgPtr);
    
    if (KErrNone != err)
        // If there is an error then set the UID to 0;
        {
        uid.iUid = 0;
        }
    else
        {
        iWgName->SetWindowGroupName(iWgNameBuf);
        uid = iWgName->AppUid(); // This UID comes from the app, not the mmp!
        }
    
    return uid;
    }

#endif //_DEBUG
