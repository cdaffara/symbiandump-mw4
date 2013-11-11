/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "taskhandlercreator.h"
#include "protocolversiongetter.h"
#include "realiapconnobserver.h"
#include "vpnconncloser.h"
#include "vpnconnstarter.h"

CTaskHandler* TaskHandlerCreator::CreateTaskHandlerL(MTaskHandlerManager* aManager,
                                                     const TTaskArrivedEventData& aEventSpec)
    {
    CTaskHandler* taskHandler = NULL;

    switch (aEventSpec.iEventType)
        {
        case EGetProtocolVersionEvent:
            taskHandler = CProtocolVersionGetter::NewL(aManager, aEventSpec);
            break;
            
        case EStartVpnConnEvent:
            taskHandler = CVpnConnStarter::NewL(aManager, aEventSpec);
            break;

        case ECloseVpnConnEvent:
            taskHandler = CVpnConnCloser::NewL(aManager, aEventSpec);
            break;

        case EObserveRealIapConnEvent:
            taskHandler = CRealIapConnObserver::NewL(aManager, aEventSpec);
            break;

        default:
            break;
        }

    return taskHandler;
    }

TBool TaskHandlerCreator::EventRequiresSit(TEventType aEventType)
    {
    TBool eventRequiresSit = EFalse;
    
    switch (aEventType)
        {
        case EGetProtocolVersionEvent:
        case EStartVpnConnEvent:
        case ECloseVpnConnEvent:
        case EObserveRealIapConnEvent:
            eventRequiresSit = ETrue;
            break;

        default:
            break;
        }

    return eventRequiresSit;
    }

TEventType TaskHandlerCreator::FindTaskRequestEventType(TEventType aCancelEventType)
    {
    TEventType taskRequestEventType = EUnfoundEvent;
    
    switch (aCancelEventType)
        {
        case EGetProtocolVersionCancelEvent:
            taskRequestEventType = EGetProtocolVersionEvent;
            break;
            
        case EStartVpnConnCancelEvent:
            taskRequestEventType = EStartVpnConnEvent;
            break;
            
        case ECloseVpnConnCancelEvent:
            taskRequestEventType = ECloseVpnConnEvent;
            break;
            
        case EObserveRealIapConnCancelEvent:
            taskRequestEventType = EObserveRealIapConnEvent;
            break;

        default:
            break;
        }

    return taskRequestEventType;
    }
    
TEventType TaskHandlerCreator::FindCancelEventType(TEventType aTaskRequestEventType)
    {
    TEventType cancelEventType = EUnfoundEvent;
    
    switch (aTaskRequestEventType)
        {
        case EGetProtocolVersionEvent:
            cancelEventType = EGetProtocolVersionCancelEvent;
            break;
            
        case EStartVpnConnEvent:
            cancelEventType = EStartVpnConnCancelEvent;
            break;
            
        case ECloseVpnConnEvent:
            cancelEventType = ECloseVpnConnCancelEvent;
            break;
            
        case EObserveRealIapConnEvent:
            cancelEventType = EObserveRealIapConnCancelEvent;
            break;

        default:
            break;
        }

    return cancelEventType;
    }

TBool TaskHandlerCreator::IsTaskCancellationObservationRequest(TEventType aEventType)
    {
    TBool isTaskCancellationObservationRequest = EFalse;
    
    switch (aEventType)
        {
        case EGetProtocolVersionCancelEvent:
        case EStartVpnConnCancelEvent:
        case ECloseVpnConnCancelEvent:
        case EStartRealIapConnCancelEvent:
        case EObserveRealIapConnCancelEvent:
            isTaskCancellationObservationRequest = ETrue;
            break;

        default:
            break;
        }

    return isTaskCancellationObservationRequest;
    }
