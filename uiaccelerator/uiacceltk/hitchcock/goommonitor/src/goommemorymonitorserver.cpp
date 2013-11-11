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
* Description:  Main classes for Graphics Out of Memory Monitor
*
*/


#include "goommemorymonitorserver.h"
#include "goommonitorclientserver.h"
#include "goommemorymonitorsession.h"
#include "goommemorymonitor.h"
#include "goomtraces.h"

CMemoryMonitorServer* CMemoryMonitorServer::NewL(CMemoryMonitor& aMonitor)
    {
    FUNC_LOG;
		RDebug::Print(_L("GOOM: CMemoryMonitorServer::NewL"));
    CMemoryMonitorServer* self=new(ELeave) CMemoryMonitorServer(aMonitor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    RDebug::Print(_L("GOOM: CMemoryMonitorServer::NewL EXIT"));
    return self;
    }

CMemoryMonitorServer::~CMemoryMonitorServer()
    {
    FUNC_LOG;
    }

CMemoryMonitorServer::CMemoryMonitorServer(CMemoryMonitor& aMonitor)
:CServer2(CActive::EPriorityStandard), iMonitor(aMonitor)
    {
    FUNC_LOG;
    }

void CMemoryMonitorServer::ConstructL()
    {
    FUNC_LOG;
    RDebug::Print(_L("GOOM: CMemoryMonitorServer::ConstructL start server"));

    StartL(KGraphicsMemoryMonitorServerName);
    RDebug::Print(_L("GOOM: CMemoryMonitorServer::ConstructL EXIT"));
    }

CSession2* CMemoryMonitorServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
    {
    FUNC_LOG;

    return new(ELeave) CMemoryMonitorSession();
    }

TInt CMemoryMonitorServer::RunError(TInt aError)
    {
    FUNC_LOG;

    Message().Complete(aError);
    //
    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();
    return KErrNone;    // handled the error fully
    }

CMemoryMonitor& CMemoryMonitorServer::Monitor()
    {
    FUNC_LOG;

    return iMonitor;
    }

void CMemoryMonitorServer::CloseAppsFinished(TInt aBytesFree, TBool aMemoryGood)
    {
    FUNC_LOG;

    iSessionIter.SetToFirst();
    CSession2* s;
    while ((s = iSessionIter++) != 0)
        static_cast<CMemoryMonitorSession*>(s)->CloseAppsFinished(aBytesFree, aMemoryGood);
    }
