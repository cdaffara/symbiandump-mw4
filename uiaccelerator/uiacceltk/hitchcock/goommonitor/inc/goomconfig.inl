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


#ifndef GOOMCONFIGINL_H_
#define GOOMCONFIGINL_H_

inline CGOomGlobalConfig& CGOomConfig::GlobalConfig()
    {
    return iGlobalConfig;
    }

void CGOomConfig::SetDefaultLowRamThreshold(TInt aLowRamThreshold)
    {
    iGlobalConfig.iLowRamThreshold = aLowRamThreshold;
    }

void CGOomConfig::SetDefaultGoodRamThreshold(TInt aGoodRamThreshold)
    {
    iGlobalConfig.iGoodRamThreshold = aGoodRamThreshold;
    }

void CGOomConfig::SetMaxCloseAppBatch(TUint aMaxCloseAppBatch)
    {
    iGlobalConfig.iMaxCloseAppBatch = aMaxCloseAppBatch;
    }
void CGOomConfig::SetDefaultWaitAfterPlugin(TInt aMilliseconds)
    {
    iGlobalConfig.iDefaultWaitAfterPlugin = aMilliseconds;
    }

#endif /*OOMCONFIGINL_H_*/
