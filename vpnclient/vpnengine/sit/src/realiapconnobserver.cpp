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
* Description:   Task handler for listening to the real interface 
*
*/

// @file realiapconnobserver.cpp

#include "realiapconnobserver.h"
#include "log.h"

CRealIapConnObserver* CRealIapConnObserver::NewL(MTaskHandlerManager* aManager,
                                                 const TTaskArrivedEventData& aTaskInfo)
    {
    CRealIapConnObserver* self = new (ELeave) CRealIapConnObserver(aManager, aTaskInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CRealIapConnObserver::CRealIapConnObserver(MTaskHandlerManager* aManager,
                                           const TTaskArrivedEventData& aTaskInfo)
    : CTaskHandler(aManager, aTaskInfo, EObserveRealIapConnCancelEvent, &iEventSpecDes)
    {
    }

void CRealIapConnObserver::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iEventMediator.Connect());
    }
    
CRealIapConnObserver::~CRealIapConnObserver()
    {
    Cancel();
    iEventMediator.Close();
    }

void CRealIapConnObserver::StartTaskHandling()
    {
    GotoState(KStateStartObserving);
    }

void CRealIapConnObserver::ChangeStateL()
    {
    switch (NextState())
        {
        case KStateStartObserving:
            StateStartObservingL();
            break;

        case KStateFinishObserving:
            StateFinishObserving();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }

void CRealIapConnObserver::CancelOngoingOperation()
    {
    TPckg<TUint32> connInfoDes(iEventSpecDes().iVpnIapId);
        
    switch (CurrState())
        {
        case KStateStartObserving:
            // Cancel connection observation
            iEventMediator.CancelListening(EKmdRealIapConnDownEvent, connInfoDes);
            break;

        case KStateFinishObserving:
            // There's no ongoing external request to cancel
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }
    }
    
void CRealIapConnObserver::StateStartObservingL()
    {
    SetCurrState(KStateStartObserving);

    TPckg<TUint32> connInfoDes(iEventSpecDes().iVpnIapId);
    iEventMediator.ListenToEvent(EKmdRealIapConnDownEvent, connInfoDes, *this);
    }

void CRealIapConnObserver::EventOccured(TInt aStatus, TEventType aType, TDesC8* aData)
    {
    LOG(Log::Printf(_L("CRealIapConnObserver::EventOccured\n")));
    
    if (aType == EKmdRealIapConnDownEvent)
        {
        // An erroneous aStatus would indicate that the event listening
        // itself would have failed somehow. We interpret that so that
        // in that case we just wouldn't get a notice if the real IAP
        // connection goes down. If it would go down, application
        // data would cease to move and the application or the user
        // would notice that at some point and close the VPN connection.
        if (aStatus == KErrNone)
            {
            LOG(Log::Printf(_L("CRealIapConnObserver::EventOccured (KMD informed that real IAP is down\n")));
            
            TEventData eventData;
            TPckg<TEventData> eventDataDes(eventData);
            eventDataDes.Copy(*aData);

            iStatus = eventData.iTaskStatus;
            
            GotoState(KStateFinishObserving);
            }
        }
    else
        {
        // Call the base class implementation to handle the event
        CTaskHandler::EventOccured(aStatus, aType, aData);
        }
    }

void CRealIapConnObserver::StateFinishObserving()
    {
    SetCurrState(KStateFinishObserving);
    
    TaskComplete(iStatus.Int());
    }

void CRealIapConnObserver::ReportResult(TInt aStatus)
    {
    // Report the connection shutdown with status
    // information to the Event Mediator

    TObserveRealIapConnEventData eventData;

    eventData.iTaskStatus = aStatus;

    TPckg<TObserveRealIapConnEventData> eventDataDes(eventData);
    ReportEvent(EObserveRealIapConnEvent, iEventSpecDes, eventDataDes);
    }
