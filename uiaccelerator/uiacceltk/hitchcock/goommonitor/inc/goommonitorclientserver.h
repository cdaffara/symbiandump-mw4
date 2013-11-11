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
* Description:  The client / server classes allowing clients to make requests to OOM monitor
*
*/


#ifndef GOOMMONITORCLIENTSERVER_H
#define GOOMMONITORCLIENTSERVER_H

#include <e32std.h>

enum TGOomMonitorClientPanic
    {
    EPanicIllegalFunction,
    EPanicRequestActive
    };

void PanicClient(const RMessagePtr2& aMessage,TGOomMonitorClientPanic aPanic);

_LIT(KGraphicsMemoryMonitorServerName, "GOomMonitorServer");

enum TGOomMonitorCmd
    {
    EGOomMonitorRequestFreeMemory,
    EGOomMonitorMemoryAllocationsComplete,
    EGOomMonitorCancelRequestFreeMemory,
    EGOomMonitorThisAppIsNotExiting,
    EGOomMonitorRequestOptionalRam,
    EGOomMonitorSetPriorityBusy,
    EGOomMonitorSetPriorityNormal,
    EGOomMonitorSetPriorityHigh,
    EGoomMonitorAppAboutToStart,
    EGoomMonitorAppUsesAbsoluteMemTargets,
    EGOomMonitorRequestHWRendering
    };

#endif // GOOMMONITORCLIENTSERVER_H
