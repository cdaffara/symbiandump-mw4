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


#include "goomaction.h"
#include "goomactionlist.h"
#include "goomtraces.h"

CGOomAction::~CGOomAction()
    {
    FUNC_LOG;
    }

// To be called by the derived class after the memory has been freed (or if it fails)
void CGOomAction::MemoryFreed(TInt)
    {
    FUNC_LOG;
    
    iState = EGOomIdle;
    iStateChangeObserver.StateChanged();
    }

CGOomAction::CGOomAction(MGOomActionObserver& aStateChangeObserver) : iStateChangeObserver(aStateChangeObserver)
    {
    FUNC_LOG;
    }
