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
* Description:  Panic codes for GOOM monitor
*
*/


#ifndef GOOMPANIC_
#define GOOMPANIC_

#include <e32def.h>

enum TGOomMonitorPanic
    {
    KRuleConfiguredBeforeApplication,
    KCloseAppActionIsResued,
    KAppConfigAddedTwice,
    KSameAppClosureConfiguredTwice,
    KGOomDefaultAppCloseNotConfigured,
    KPluginConfigAddedTwice,
    KRuleConfiguredBeforePlugin,
    KGOomDefaultAppNotConfigured,
    KGOomDefaultPluginNotConfigured,
    KNoCoeEnvFound,
    KInvalidWgName,
    KGOomInvalidPriority,
    KAppCloseActionEqualPriorities,
    KEGLProfilingExtensionNotAvailable
    };

void GOomMonitorPanic(TGOomMonitorPanic aReason);
void GOomConfigParserPanic(TInt aReason);

#endif /*OOMPANIC_*/
