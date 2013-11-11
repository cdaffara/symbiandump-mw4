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


#include "goomapplicationconfig.h"
#include "goomconstants.hrh"
#include "goomcloseappconfig.h"
#include "goomtraces.h"

CGOomApplicationConfig* CGOomApplicationConfig::NewL(TUint aApplicationId)
    {
    FUNC_LOG;

    CGOomApplicationConfig* self = new (ELeave) CGOomApplicationConfig(aApplicationId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Add a rule
// This class takes ownership of the given rule
// This rule applies to this application itself (and not a plugin associated with this application)
// The rule would usually apply to an "application close" event
void CGOomApplicationConfig::AddRuleL(MGOomRuleConfig* aRule)
    {   
    FUNC_LOG;

   __ASSERT_ALWAYS(iCloseAppConfig, GOomMonitorPanic(KRuleConfiguredBeforeApplication));
    
    iCloseAppConfig->AddRuleL(aRule);
    }

CGOomApplicationConfig::~CGOomApplicationConfig()
    {
    FUNC_LOG;

    delete iCloseAppConfig;
    }

void CGOomApplicationConfig::ConstructL()
    {
    FUNC_LOG;

    iGoodRamThreshold = KGOomThresholdUnset;
    iLowRamThreshold = KGOomThresholdUnset;  
    iTargetFree = KGOomThresholdUnset;
    }

CGOomApplicationConfig::CGOomApplicationConfig(TUint aApplicationId) : iApplicationId(aApplicationId)
    {
    FUNC_LOG;
    }
