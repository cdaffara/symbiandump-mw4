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


#ifndef GOOMRULECONFIG_
#define GOOMRULECONFIG_

#include <e32std.h>

class CGOomWindowGroupList;

NONSHARABLE_CLASS(MGOomRuleConfig)
    {
public:
    virtual TBool RuleIsApplicable(const CGOomWindowGroupList& aWindowGroupList, TInt aAppIndexInWindowGroup) const = 0;
    virtual TUint Priority() const = 0;
    };

#endif /*GOOMRULECONFIG_*/
