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
* Description:  Configuration representation classes for Out of Memory Monitor
*
*/


#ifndef GOOMIDLETIMERULE_
#define GOOMIDLETIMERULE_

#include <e32base.h>

#include "goomruleconfig.h"

NONSHARABLE_CLASS(CGOomIdleTimeRule) : public CBase, public MGOomRuleConfig
    {
public:
    static CGOomIdleTimeRule* NewL(TTimeIntervalSeconds aIdleTime, TInt aPriority);
    
    TBool RuleIsApplicable(const CGOomWindowGroupList& aWindowGroupList, TInt aAppIndexInWindowGroup) const;

    inline TUint Priority() const;
    
    ~CGOomIdleTimeRule();
    
private:
    CGOomIdleTimeRule(TTimeIntervalSeconds aIdleTime, TInt aPriority);
    
private:
    TTimeIntervalSeconds iIdleTime; // The idle time after which to apply the given priority
    TInt iPriority; // The priority to apply after the given idle time
    };

#include "goomidletimerule.inl"

#endif /*GOOMIDLETIMERULE_*/
