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


#ifndef GOOMACTIONCONFIG_
#define GOOMACTIONCONFIG_

#include <e32cmn.h>
#include <e32std.h>
#include <w32std.h>

class MGOomRuleConfig;
class CGOomWindowGroupList;

enum TGOomSyncMode
    {
    EContinue,    // Continue with the next action regardless of anything else (exluding max_batch_size)
    EEstimate,    // Continue with the next action if we estimate that we need to free more memory
    ECheckRam,    // Wait for this action to complete, then check the free RAM before continuing
    EContinueIgnoreMaxBatchSize // Continue with the next action regardless of anything else (including max_batch_size)
    };

NONSHARABLE_CLASS(CGOomActionConfig) : public CBase
    {
public:
    // Add a rule
    // This class takes ownership of the given rule
    void AddRuleL(MGOomRuleConfig* aRule);    // Add the configuration for a rule (e.g. an idle time rule)
            
    virtual ~CGOomActionConfig();
    
    // Set the priority of this action
    // This priority will be applied if none of the attached rules can be applied
    inline void SetDefaultPriority(TUint aPriority);
    
protected:
    
    // Return the priority for this action for the idle time of the target app
    TUint Priority(const CGOomWindowGroupList& aWindowGroupList, TInt aAppIndexInWindowGroup) const;
    
    void ConstructL();

    CGOomActionConfig(TInt32 aId);
    
public:
    
    TGOomSyncMode iSyncMode;
    
    TInt iRamEstimate;    // The estimated RAM saving after performing this action
    
    TInt32 iId;        // The ID of the affected component (e.g. an application ID for app closure, or a plugin ID for a plugin event)
    
    TUint iDefaultPriority;
    
protected:    
    
    RPointerArray<MGOomRuleConfig> iRules;
    };

#include "goomactionconfig.inl"

#endif /*GOOMACTIONCONFIG_*/
