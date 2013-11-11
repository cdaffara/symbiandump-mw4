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


#ifndef GOOMAPPLICATIONCONFIGINL_H_
#define GOOMAPPLICATIONCONFIGINL_H_

#include "goompanic.h"

inline CGOomCloseAppConfig* CGOomApplicationConfig::GetAppCloseConfig()
    {
    return iCloseAppConfig;
    }

inline void CGOomApplicationConfig::SetAppCloseConfig(CGOomCloseAppConfig* aActionConfig)
    {
    __ASSERT_ALWAYS(iCloseAppConfig == NULL, GOomMonitorPanic(KSameAppClosureConfiguredTwice));
    
    iCloseAppConfig = aActionConfig;
    }


inline TUint CGOomApplicationConfig::Id()
    {
    return iApplicationId;
    }

#endif /*GOOMAPPLICATIONCONFIGINL_H_*/
