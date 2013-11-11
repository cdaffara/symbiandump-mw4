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
* Description:  Configuration representation classes for Graphics Out of Memory Monitor
*
*/


#include "goomrunpluginconfig.h"
#include "goomwindowgrouplist.h"
#include "goomtraces.h"

CGOomRunPluginConfig* CGOomRunPluginConfig::NewL(TUint aPluginId, TGOomPluginType aPluginType)
    {
    FUNC_LOG;

    CGOomRunPluginConfig* self = new (ELeave) CGOomRunPluginConfig(aPluginId, aPluginType);
    return self;
    }

TUint CGOomRunPluginConfig::CalculatePluginPriority(const CGOomWindowGroupList& aWindowGroupList)
    {
    FUNC_LOG;

    // Call the Priority function on the CActionConfig base class
    // This function will check if any rules match the current system state and then adjust the priority if they do
    // Rules may apply to system plugins or application plugins
    return Priority(aWindowGroupList, aWindowGroupList.GetIndexFromAppId(iTargetAppId));
    }


CGOomRunPluginConfig::~CGOomRunPluginConfig()
    {
    FUNC_LOG;
    delete iNextConfig;
    }
    

CGOomRunPluginConfig::CGOomRunPluginConfig(TUint aPluginId, TGOomPluginType aPluginType) : CGOomActionConfig(aPluginId), iPluginId(aPluginId), iPluginType(aPluginType)
    {
    FUNC_LOG;

    iSyncMode = EContinueIgnoreMaxBatchSize;
    }
