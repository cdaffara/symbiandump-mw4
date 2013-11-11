/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main classes for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMMEMORYMONITORSESSION_H
#define GOOMMEMORYMONITORSESSION_H

#include <e32base.h>

class CMemoryMonitorServer;
class CMemoryMonitor;

NONSHARABLE_CLASS(CMemoryMonitorSession) : public CSession2
    {
public:
    CMemoryMonitorSession();
    void CreateL();
    void CloseAppsFinished(TInt aBytesFree, TBool aMemoryGood);

private:
    ~CMemoryMonitorSession();
    CMemoryMonitorServer& Server();
    CMemoryMonitor& Monitor();
    void ServiceL(const RMessage2& aMessage);

private:
    RMessagePtr2 iRequestFreeRam;
    TInt iFunction;
    TInt iMinimumMemoryRequested;
    TBool iUseAbsoluteTargets;
    TUint iClientId;
    };

#endif /*GOOMMEMORYMONITORSESSION_H*/
