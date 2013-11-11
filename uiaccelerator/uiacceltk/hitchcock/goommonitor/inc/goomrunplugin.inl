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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMRUNPLUGININL_H_
#define GOOMRUNPLUGININL_H_

#include "goomactionconfig.h"
#include "goompluginwaiter.h"
#include "goomrunpluginconfig.h"

inline CGOomActionConfig& CGOomRunPlugin::GetConfig()
    {
    return iConfig;
    }


inline TBool CGOomRunPlugin::IsRunning()
    {
    if(iPluginWaiter)
        {
        return iPluginWaiter->IsActive();
        }
    
    return EFalse;
    }

inline void CGOomRunPlugin::WaitCompleted()
    {
    MemoryFreed(KErrNone);
    }


#endif /*GOOMRUNPLUGININL_H_*/
