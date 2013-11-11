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

#ifndef GOOMLOG_H_
#define GOOMLOG_H_

#ifdef _DEBUG

#include <apgwgnam.h>

#include "goomwindowgrouplist.h"

const TUint KTimeBetweenMemorySamples = 100000; // In microseconds
const TUint KSamplingDurationUint = 3000000;

const TUint KMaxBytesOfLoggingPerSample = 20;

NONSHARABLE_CLASS(CGOomLogger) : public CTimer
    {
public:
    static CGOomLogger* NewL(RWsSession& aWs, RFs& aFs);
    
    // Start logging the available memory every n micro seconds
    // Firstly a list of the app IDs is written to the log (foreground app first)
    // Note that the log is created in memory (to a pre-allocated buffer) and flushed out after it is complete
    // the samples are saved in CSV format so that they can easily be cut and pasted to plot graphs etc.
    void StartL();
    
// From CTimer / CActice
    void RunL();
    void DoCancel();
    
    ~CGOomLogger();
    
    void Write(const TDesC8& aBuffer);
    
protected:
    void LogApplicationIds();
    void LogFreeMemory();
    
    CGOomLogger(RWsSession& aWs, RFs& aFs);
    void ConstructL();
    
    // Duplicated functionality from GOomMonitor
    // Duplicated because it was messy to reuse it and to minimise changes to OOM monitor during development of new features
    void ColapseWindowGroupTree();
    
    TUid GetUidFromWindowGroupId(TInt aWgId);

    RWsSession& iWs;    
    
    // Used to get a list of open applications
    RArray<RWsSession::TWindowGroupChainInfo> iWgIds;
    CApaWindowGroupName* iWgName;
    HBufC* iWgNameBuf;              // owned by iWgName
    
    RFs& iFs;
    
    RFile iFile;
    TBool iIsOpen;
    
    // The time when the logging was started
    TTime iStartTime;
    
    TBuf8<(KSamplingDurationUint / KTimeBetweenMemorySamples) * KMaxBytesOfLoggingPerSample> iWriteBuffer;
    };

#endif //_DEBUG

#endif /*OOMLOG_H_*/
