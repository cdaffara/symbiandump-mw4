/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* Provides set of static functions, e.g. for creating particular task handler.
*
*/



#ifndef __TASK_HANDLER_CREATOR_H__
#define __TASK_HANDLER_CREATOR_H__

#include "sit.h"

class MTaskHandlerManager;
class CTaskHandler;

class TaskHandlerCreator
    {
public:
    static TBool EventRequiresSit(TEventType aEventType);
    static TEventType FindTaskRequestEventType(TEventType aCancelEventType);
    static TEventType FindCancelEventType(TEventType aTaskRequestEventType);
    static TBool IsTaskCancellationObservationRequest(TEventType aEventType);
    static CTaskHandler* CreateTaskHandlerL(MTaskHandlerManager* aManager,
                                            const TTaskArrivedEventData& aEventSpec);
    };

#endif // __TASK_HANDLER_CREATOR_H__
