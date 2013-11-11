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


#include "goomcloseappconfig.h"
#include "goomtraces.h"

CGOomCloseAppConfig* CGOomCloseAppConfig::NewL(TInt32 aId)
    {
    FUNC_LOG;

    CGOomCloseAppConfig* self = new (ELeave) CGOomCloseAppConfig(aId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CGOomCloseAppConfig::~CGOomCloseAppConfig()
    {
    FUNC_LOG;
    }

CGOomCloseAppConfig::CGOomCloseAppConfig(TInt32 aId) : CGOomActionConfig(aId)
    {
    FUNC_LOG;
    }
