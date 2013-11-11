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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#include <goommonitorplugin.hrh>
#include "goomrunplugin.h"
#include "goomtraces.h"
#include "goommemorymonitor.h"
#include "goommonitorplugin.h"
#include "goomactionlist.h"

CGOomRunPlugin* CGOomRunPlugin::NewL(TUint aPluginId, CGOomRunPluginConfig& aConfig, MGOomActionObserver& aStateChangeObserver, CGOomMonitorPlugin& aPlugin)
    {
    FUNC_LOG;

    CGOomRunPlugin* self = new (ELeave) CGOomRunPlugin(aPluginId, aConfig, aStateChangeObserver, aPlugin);
    CleanupStack::PushL(self);
    self->ConstructL(aConfig);
    CleanupStack::Pop(self);
    return self;
    }

// Run the GOOM plugin in order to free memory
// Call the CGOomAction::MemoryFreed when it is done
void CGOomRunPlugin::FreeMemory(TInt aBytesRequested, TBool aUseSwRendering)
    {
    FUNC_LOG;
    TRACES1("CGOomRunPlugin::FreeMemory: iPluginId = 0x%x", iPluginId);
    TRACES1("CGOomRunPlugin::FreeMemory: aBytesRequested = %d", aBytesRequested);

    // Ask the plugin to free some memory, should actually ask the difference 
    // between existing and required amount..
    TInt clientId = iStateChangeObserver.ClientId();
    TAny* anyp = (TAny*) &clientId;
    iPlugin.ExtensionInterface(TUid::Uid(KGoomClientSecureId), anyp);
    
    if(aUseSwRendering && iConfig.IsSwRendSupported())
        {
		TInt flags = KGOomUseSwRendering;
        iPlugin.FreeRam(aBytesRequested, flags);
        iFreeMemoryWithSwRenderingCalled = ETrue;
        TRACES("CGOomRunPlugin::FreeMemory: UseSwRendering TRUE");
        }
    else
        {
        iPlugin.FreeRam(aBytesRequested, 0);
        iFreeMemoryWithSwRenderingCalled = EFalse;
        TRACES("CGOomRunPlugin::FreeMemory: UseSwRendering FALSE");
        }

    iFreeMemoryCalled = ETrue;

    // Wait for the required time before we signal completion.
    if(iPluginWaiter)
        {
        iPluginWaiter->Start();
        }
    }

// Call the memory good function on the plugin but...
// only if there is an outstanding FreeMemory request
void CGOomRunPlugin::MemoryGood()
    {
    FUNC_LOG;

    if (iFreeMemoryCalled)
        {
		TInt flags = 0;
		if(iFreeMemoryWithSwRenderingCalled)
			flags = KGOomUseSwRendering;
        iPlugin.MemoryGood(flags);
        iFreeMemoryCalled = EFalse;
        iFreeMemoryWithSwRenderingCalled = EFalse;
        }
    }

CGOomRunPlugin::~CGOomRunPlugin()
    {
    FUNC_LOG;

    delete iPluginWaiter;
    }

CGOomRunPlugin::CGOomRunPlugin(TUint aPluginId, CGOomRunPluginConfig& aConfig, MGOomActionObserver& aStateChangeObserver, CGOomMonitorPlugin& aPlugin) : CGOomAction(aStateChangeObserver), iPluginId(aPluginId), iPlugin(aPlugin), iConfig(aConfig)
    {
    FUNC_LOG;
    }

void CGOomRunPlugin::ConstructL(CGOomRunPluginConfig& aPluginConfig)
    {
    FUNC_LOG;

    TInt waitDuration = CMemoryMonitor::GlobalConfig().iDefaultWaitAfterPlugin;

    if(aPluginConfig.iSyncMode == ECheckRam)
        {
        if (aPluginConfig.WaitAfterPluginDefined())
            {
            // If the wait duration for this plugin is overridden then use the overridden value
            waitDuration = aPluginConfig.WaitAfterPlugin();
            }
         iPluginWaiter = CGOomPluginWaiter::NewL(waitDuration, *this);
        }
    }

TUint CGOomRunPlugin::Id()
    {
    return iPluginId;
    }
