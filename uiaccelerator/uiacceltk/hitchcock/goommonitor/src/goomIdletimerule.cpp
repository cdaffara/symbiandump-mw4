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


#include "goomidletimerule.h"
#include "goomwindowgrouplist.h"
#include "goomtraces.h"

CGOomIdleTimeRule* CGOomIdleTimeRule::NewL(TTimeIntervalSeconds aIdleTime, TInt aPriority)
    {
    FUNC_LOG;

    CGOomIdleTimeRule* self = new (ELeave) CGOomIdleTimeRule(aIdleTime, aPriority);
    return self;
    }

TBool CGOomIdleTimeRule::RuleIsApplicable(const CGOomWindowGroupList& aWindowGroupList, TInt aAppIndexInWindowGroup) const
    {
    FUNC_LOG;

    TBool applicable = EFalse;
    if (aAppIndexInWindowGroup >= 0)
        {
        applicable = (aWindowGroupList.IdleTime(aAppIndexInWindowGroup) >= iIdleTime);
        }
    return applicable;
    }

CGOomIdleTimeRule::~CGOomIdleTimeRule()
    {
    FUNC_LOG;
    }

CGOomIdleTimeRule::CGOomIdleTimeRule(TTimeIntervalSeconds aIdleTime, TInt aPriority) : iIdleTime(aIdleTime), iPriority(aPriority)
    {
    FUNC_LOG;
    }
    
