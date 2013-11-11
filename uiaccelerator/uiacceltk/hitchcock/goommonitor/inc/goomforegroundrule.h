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


#ifndef GOOMFOREGROUNDRULE_
#define GOOMFOREGROUNDRULE_

#include <e32base.h>
#include "goomruleconfig.h"

// A class to modify the priority if a particular application is in the foreground
NONSHARABLE_CLASS(CGOomForegroundRule) : public CBase, public MGOomRuleConfig
    {
public:
    // If the specified target app is in the foreground then apply the specified priority
    CGOomForegroundRule(TInt aTargetAppId, TInt aPriority);
    
    TBool RuleIsApplicable(const CGOomWindowGroupList& aWindowGroupList, TInt aAppIndexInWindowGroup) const;

    inline TUint Priority() const;
        
private:
    TInt iTargetAppId;
    TInt iPriority; // The priority to apply if the specified app is in the foreground
    };

#include "goomforegroundrule.inl"

#endif /*GOOMFOREGROUNDRULE_*/
