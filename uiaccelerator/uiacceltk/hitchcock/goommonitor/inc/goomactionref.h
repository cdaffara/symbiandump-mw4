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
* Description:  Classes for executing Graphics OOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMACTIONREF_H_
#define GOOMACTIONREF_H_

#include <e32base.h>
#include "goomactionconfig.h"

class CGOomRunPlugin;
class CGOomAction;

/**
 *  Encapsulates a reference to an action. 
 *  
 *  Objects of this T-class are instantiated at memory freeing time in preference to the  
 *  CGOomAction derived objects, so that we do not instantiate anything on the heap at a time
 *  when the device is, by-definition, low on memory.
 *
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
class TActionRef
    {
public:
    
    enum TActionType
        {
        EAppPlugin,
        ESystemPlugin,
        EAppClose
        };

    //constructor for Plugin actions
    TActionRef(TActionType aType, TInt aPriority, TGOomSyncMode aSyncMode, TInt aRamEstimate, CGOomRunPlugin& aRunPlugin, TUint aWgIndexOfTargetApp);

    //constructor for AppClose actions
    TActionRef(TActionType aType, TInt aPriority, TGOomSyncMode aSyncMode, TInt aRamEstimate, TInt aWgId, TUint aWgIndex, TInt aCloseTimeout = 0, TInt aWaitAfterClose = 0);
    
    TActionType Type() const;
    TUint Priority() const;
    void SetPriority(TUint);
    TGOomSyncMode SyncMode() const;
    TInt RamEstimate() const;
    TInt WgId() const;
    TInt WgIndex() const;
    CGOomRunPlugin& RunPlugin();
    TInt CloseTimeout() const;
    TInt WaitAfterClose() const;
    TBool IsRunning();

private: //data
    
    TActionType iType;
    TUint iPriority;
    TGOomSyncMode iSyncMode; //needed as we don't have reference to the config 
    TInt iRamEstimate; //needed as we don't have reference to the config 
    TInt iWgId; //For AppClose
    TInt iWgIndex;
    CGOomRunPlugin* iRunPlugin; //For Plugins. Not owned
    TInt iCloseTimeout; //For AppClose
    TInt iWaitAfterClose; //For AppClose
public:
    CGOomAction* iAppPlugin; // because of stupid desing, not owned
    };

#endif /*GOOMACTIONREF_H_*/
