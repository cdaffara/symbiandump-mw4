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


#ifndef GOOMGLOBALCONFIG_
#define GOOMGLOBALCONFIG_

#include <e32base.h>
#include <e32hashtab.h>

NONSHARABLE_CLASS(CGOomGlobalConfig) : public CBase
    {
public:
    ~CGOomGlobalConfig();
    
    inline void AddForceCheckPriorityL(TUint aPriority);
    
    // Return ETrue if a force check has been added for this priority, return EFalse otherwise
    inline TBool ForceCheckAtPriority(TUint aPriority) const;
    
public:
    TInt iLowRamThreshold;
    TInt iGoodRamThreshold;
    TUint iMaxCloseAppBatch;
    TInt iDefaultWaitAfterPlugin;
    
    RHashSet<TUint> iForceCheckPriorities;
    };

#include "goomglobalconfig.inl"

#endif /*GOOMGLOBALCONFIG_*/
