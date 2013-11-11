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


#ifndef GOOMCLOSEAPPCONFIG_
#define GOOMCLOSEAPPCONFIG_

#include "goomactionconfig.h"

/*
 * 
 */
/**
 *  The GOOM action of closing an application in order to free up memory.
 *
 *  @lib goommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomCloseAppConfig) : public CGOomActionConfig
    {
public:
    /**
     * Two-phased constructor.
     * @param aId The Uid of the applicaton
     */
    static CGOomCloseAppConfig* NewL(TInt32 aId);
    
    ~CGOomCloseAppConfig();

    /**
     * Calculates the priority of a close app action, based on its position on the window group list
     * 
     * @param aWindowGroupList a fully constructed, collapsed window group list
     * @param aAppIndexInWindowGroup the position in the window group list
     */
    inline TUint CalculateCloseAppPriority(const CGOomWindowGroupList& aWindowGroupList, TUint aAppIndexInWindowGroup);
    
    TInt iCloseTimeout;
    TInt iWaitAfterClose;
    
private:
    
    CGOomCloseAppConfig(TInt32 aId);
    };

#include "goomcloseappconfig.inl"

#endif /*GOOMCLOSEAPPCONFIG_*/
