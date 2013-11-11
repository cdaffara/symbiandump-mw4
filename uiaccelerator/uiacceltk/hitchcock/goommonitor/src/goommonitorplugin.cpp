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
* Description:  GOOM Monitor plug-in interface.
*
*/


#include <apgwgnam.h>
#include "goommonitorplugin.h"
#include "goommemorymonitor.h"
#include "goomtraces.h"

// TLS is used to store the CMemoryMonitor pointer, CMemoryMonitor
// being the main object in the GOOM monitor thread. This allows
// plugins to access the CMemoryMonitor object easily.
EXPORT_C void SetGMemoryMonitorTls(CMemoryMonitor* aMonitor)
    {
    FUNC_LOG;

    Dll::SetTls(aMonitor);
    }

CMemoryMonitor* MemoryMonitorTls()
    {
    FUNC_LOG;

    return static_cast<CMemoryMonitor*>(Dll::Tls());
    }


void GOomMonitorPluginPanic(TGOomMonitorPluginPanic aReason)
    {
    FUNC_LOG;

    _LIT(KCat, "GOomMonitorPlugin");
    User::Panic(KCat, aReason);
    }


EXPORT_C CGOomMonitorPluginBase::CGOomMonitorPluginBase()
: iMemoryMonitor(MemoryMonitorTls())
    {
    FUNC_LOG;
    RDebug::Print(_L("CGOomMonitorPluginBase::CGOomMonitorPluginBase"));

    __ASSERT_ALWAYS(iMemoryMonitor, GOomMonitorPluginPanic(EGOomMonitorPluginPanic_PluginConstructedOutsideGOomMonitorThread));
    RDebug::Print(_L("CGOomMonitorPluginBase::CGOomMonitorPluginBase EXIT"));
    }

EXPORT_C CGOomMonitorPluginBase::~CGOomMonitorPluginBase()
    {
    FUNC_LOG;
    }

EXPORT_C void CGOomMonitorPluginBase::ConstructL()
    {
    FUNC_LOG;

    // CAppGOomMonitorPlugin assumes ConstructL is empty
    }

EXPORT_C void CGOomMonitorPluginBase::ExtensionInterface(TUid /*aInterfaceId*/, TAny*& /*aImplementaion*/)
    {
    FUNC_LOG;
    }

EXPORT_C RFs& CGOomMonitorPluginBase::FsSession()
    {
    FUNC_LOG;

    return iMemoryMonitor->iFs;
    }

EXPORT_C RWsSession& CGOomMonitorPluginBase::WsSession()
    {
    FUNC_LOG;

    return iMemoryMonitor->iWs;
    }

EXPORT_C CAppGOomMonitorPlugin* CAppGOomMonitorPlugin::NewL(TUid aAppUid)
    {
    CAppGOomMonitorPlugin* self = new(ELeave) CAppGOomMonitorPlugin(aAppUid);
    // ConstructL() currently not needed
    return self;
    }

CAppGOomMonitorPlugin::CAppGOomMonitorPlugin(TUid aAppUid)
: iAppUid(aAppUid)
    {
    }

void CAppGOomMonitorPlugin::FreeRam(TInt, TBool)
    {
    SendMessageToApp(KAppGOomMonitor_FreeRam);
    }

void CAppGOomMonitorPlugin::MemoryGood(TBool)
    {
    SendMessageToApp(KAppGOomMonitor_MemoryGood);
    }

void CAppGOomMonitorPlugin::SendMessageToApp(TInt aMessage)
    {
    RWsSession& ws = WsSession();
    TInt wgId = 0;

    do
        {
        CApaWindowGroupName::FindByAppUid(iAppUid, ws, wgId);
        if (wgId>0)
            {
            TWsEvent event;
            event.SetType(aMessage);
            event.SetTimeNow();
            ws.SendEventToWindowGroup(wgId, event);
            }
        }
    while (wgId>0);
    }
