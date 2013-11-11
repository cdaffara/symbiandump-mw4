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


#ifndef GOOMMEMORYMONITORSERVER_H
#define GOOMMEMORYMONITORSERVER_H

#include <e32base.h>

class CMemoryMonitor;

NONSHARABLE_CLASS(CMemoryMonitorServer) : public CServer2
    {
public:
    static CMemoryMonitorServer* NewL(CMemoryMonitor& aMonitor);
    ~CMemoryMonitorServer();

    CMemoryMonitor& Monitor();
    void CloseAppsFinished(TInt aBytesFree, TBool aMemoryGood);

private:
    CMemoryMonitorServer(CMemoryMonitor& aMonitor);
    void ConstructL();
    CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
    TInt RunError(TInt aError);

private:
    CMemoryMonitor& iMonitor;
    };

#endif /*GOOMMEMORYMONITORSERVER_H*/
