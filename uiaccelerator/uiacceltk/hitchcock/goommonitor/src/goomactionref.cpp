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


#include "goomactionref.h"
#include "goomrunplugin.h"

TActionRef::TActionRef(TActionType aType, TInt aPriority, TGOomSyncMode aSyncMode, TInt aRamEstimate, CGOomRunPlugin& aRunPlugin, TUint aWgIndexOfTargetApp)
    : iType(aType), iPriority(aPriority), iSyncMode(aSyncMode), iRamEstimate(aRamEstimate), iWgIndex(aWgIndexOfTargetApp), iRunPlugin(&aRunPlugin), iAppPlugin(0)
    {   
    }

TActionRef::TActionRef(TActionType aType, TInt aPriority, TGOomSyncMode aSyncMode, TInt aRamEstimate, TInt aWgId, TUint aWgIndex, TInt aCloseTimeout, TInt aWaitAfterClose)
: iType(aType), iPriority(aPriority), iSyncMode(aSyncMode), iRamEstimate(aRamEstimate), iWgId(aWgId), iWgIndex(aWgIndex), iRunPlugin(NULL), iCloseTimeout(aCloseTimeout), iWaitAfterClose(aWaitAfterClose), iAppPlugin(0)
    {   
    }

    
TActionRef::TActionType TActionRef::Type() const
    {
    return iType;
    }

TUint TActionRef::Priority() const
    {
    return iPriority;
    }

void TActionRef::SetPriority(TUint aPriority)
    {
    iPriority = aPriority;
    }

TGOomSyncMode TActionRef::SyncMode() const
    {
    return iSyncMode;
    }
    
TInt TActionRef::RamEstimate() const
    {
    return iRamEstimate;
    }

TInt TActionRef::WgId() const
    {
    return iWgId;
    }

TInt TActionRef::CloseTimeout() const
    {
    return iCloseTimeout;
    }

TInt TActionRef::WaitAfterClose() const
    {
    return iWaitAfterClose;
    }

TInt TActionRef::WgIndex() const
    {
    return iWgIndex;
    }

CGOomRunPlugin& TActionRef::RunPlugin()
    {
    return *iRunPlugin;
    }

TBool TActionRef::IsRunning()
    {
    if (iRunPlugin)
        {
        return iRunPlugin->IsRunning();
        }
    else if (iAppPlugin)
        {
        return iAppPlugin->IsRunning();
        }
    return EFalse;
    }         
