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
* Description:   This module contains eventmediator and the services of it.
*
*/



/**  
 * @file eventmediator.cpp
 *
 * This module contains eventmediator and the services of it.
 *
 */
#include <es_sock.h>
#include "eventmediator.h"
#include "eventlogger.h"
#include "eventmediatordefs.h"
#include "sit.h"
#include "log_em.h"

#define FIRST_ARGUMENT 0
#define SECOND_ARGUMENT 1
#define THIRD_ARGUMENT 2
#define FOURTH_ARGUMENT 3

// ============================= CEventMediatorServer =============================

const TUint CEventMediatorServer::iRangeCount = 2;
    
const TInt CEventMediatorServer::iRanges[iRangeCount] = 
    {
    CEventMediatorSession::KEventMediatorListen,
    CEventMediatorSession::KEventMediatorClearEventLog+1
    };

const TUint8 CEventMediatorServer::iElementIndex[iRangeCount] = 
    {
    0,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement CEventMediatorServer::iElements[] =
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CEventMediatorServer::iPolicy =
    {
    0,              // All connect attempts are checked
    iRangeCount,    // Count of ranges
    iRanges,        // 0...9, 9...
    iElementIndex,  // Only range 1000-1008 are checked
    iElements       // The list of policy elements
    };


CEventMediatorServer::CEventMediatorServer(void)
    : CPolicyServer(EPriorityNormal,iPolicy), iStoredEvents(1)
    {
    }

CEventMediatorServer* CEventMediatorServer::NewL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::NewL - begin\n")));
    CEventMediatorServer* server = CEventMediatorServer::NewLC();
    CleanupStack::Pop(); // server
    LOG(Log::Printf(_L("CEventMediatorServer::NewL - end\n")));
    return server;
    }

CEventMediatorServer* CEventMediatorServer::NewLC()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::NewLC - begin\n")));
    CEventMediatorServer* server = new (ELeave) CEventMediatorServer();
    CleanupStack::PushL(server); 
    server->ConstructL(server);
    LOG(Log::Printf(_L("CEventMediatorServer::NewLC - end\n")));
    return server;
    }

void CEventMediatorServer::ConstructL(CEventMediatorServer* aServer)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    fs.CreatePrivatePath(EDriveC);
    
    TPath privateDir;
    User::LeaveIfError(fs.PrivatePath(privateDir));
    iEventLogFileName.Copy(privateDir);
    iEventLogFileName.Append(KEventLogFile);
    
    CleanupStack::PopAndDestroy(); // fs
    
    aServer->iLogger = CEventLogger::NewL(this);
    iSit = new (ELeave) CSit(this);
    StartL(KEventMediatorServer);
    }

CEventMediatorServer::~CEventMediatorServer(void)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::~CEventMediatorServer\n")));
    // Delete stored events
    TInt nEvents = this->iStoredEvents.Count();
    for (TInt i = 0; i < nEvents; i++)  
        {
        delete iStoredEvents.At(i)->iData;
        delete iStoredEvents.At(i);
        }
    iStoredEvents.Delete(0, iStoredEvents.Count());

    // Delete log writer
    delete iLogger;

    delete iSit;
    }

// ----------------------------------------------------------------------------
// CEventMediatorServer::StopEventMediator
// Stops Event Mediator service if there are no sessions left.
// ----------------------------------------------------------------------------
//
void CEventMediatorServer::StopEventMediator(void)
    {
    if (iSessionCount == 0) 
        {
        LOG(Log::Printf(_L("CEventMediatorServer::StopEventMediator - session count 0, stopping scheduler and thus the server\n")));
        CActiveScheduler::Stop();   
        }   
    }

TInt CEventMediatorServer::RunError(TInt aError)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::RunError - error = %d\n"), aError));
    Message().Complete(aError);

    // The leave will result in an early return from CServer::RunL(),
    // skipping the call to request another message. So we issue the
    // request here in order to keep the server running.
    ReStart();

    // Handled the error fully
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
// CEventMediatorServer::NewSessionL
// Creates a new session and returns the handle to the session.
// ----------------------------------------------------------------------------
//
CSession2* CEventMediatorServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& aMessage) const
    {
    // New sessions are not accepted if the server is shutting down
    // (it's just waiting for the last session (from SIT) to die)
    if (iShuttingDown)
        {
        User::Leave(KErrServerTerminated);
        }
    
    CSession2* session = CEventMediatorSession::NewL(CONST_CAST(CEventMediatorServer*, this), aMessage);
    return session;
    }

void CEventMediatorServer::ReportEventL(const TEventType aType, TDesC8* aSpec,
                                        TDesC8* aData, TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::ReportEventL - event type = %d\n"), aType));
    TInt listenerCount = 0;
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        // Some listeners listen this event with specification
        listenerCount += session->CheckEventL(aType, aSpec, aData, aStatus); 
        // Some without specification, all events are good for them
        if (aSpec != NULL)
            {
            listenerCount += session->CheckEventL(aType, NULL, aData, aStatus);
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }
    if (listenerCount)
        {
        // Need to save the event data as it may/will be fetched later
        TEventContainer* container = new (ELeave) TEventContainer(listenerCount, aData);
        CleanupStack::PushL(container);
        iStoredEvents.AppendL(container);
        CleanupStack::Pop();
        }
    // Write event to log
    if (aType == ELogEvent && iLogger)
        {
        LOG(Log::Printf(_L("CEventMediatorServer::ReportEventL - calling iLogger->LogEvent\n")));
        iLogger->LogEvent(*aData);
        }

    // If there are no listeners, delete data
    if (listenerCount == 0)
        {
        delete aData;
        }
    }

TInt CEventMediatorServer::CopyEventDataL(const RMessage2& aMessage)
    {
    TBool found = EFalse;
    TInt i = 0;
    TInt err = KErrNone;
    while (!found && i < iStoredEvents.Count())
        {
        if (iStoredEvents.At(i)->iData == aMessage.Ptr0())
            {
            found = ETrue;
            }
        else
            {
            i++;
            }
        }
    if (found)
        {
        aMessage.WriteL(SECOND_ARGUMENT, *(iStoredEvents.At(i)->iData));
        MarkStoredEventListened(i);
        }
    else
        {
        err = KErrNotFound;
        }
    return err;
    }

void CEventMediatorServer::MarkStoredEventListened(TInt aIndex)
    {
    iStoredEvents.At(aIndex)->iListenerCount--;
    if (iStoredEvents.At(aIndex)->iListenerCount == 0)
        {
        delete iStoredEvents.At(aIndex)->iData;
        delete iStoredEvents.At(aIndex);
        iStoredEvents.Delete(aIndex);
        }
    }

TBool CEventMediatorServer::IsClientTheSitL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::IsClientTheSitL\n")));
    TBool isClientTheSit = EFalse;
    
    RThread clientThread;
    User::LeaveIfError(aMessage.Client(clientThread));
    
    if (clientThread.Id() == iSit->ThreadId())
        {
        LOG(Log::Printf(_L("CEventMediatorServer::IsClientTheSitL - YES\n")));
        isClientTheSit = ETrue;
        }

    clientThread.Close();

    return isClientTheSit;
    }
    
void CEventMediatorServer::MakeSureSitIsRunningL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::MakeSureSitIsRunningL\n")));
    // If the SIT has not yet been started
    // or has died, try to start it
    iSit->StartL();
    }

void CEventMediatorServer::SetTaskArrivalListenerL(CListenerContainer* aListener)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::SetTaskArrivalListenerL\n")));
    if (aListener)
        {
        // Task arrival observation requests must
        // come from the SIT
        if (!IsClientTheSitL(aListener->Message()))
            {
            User::Leave(KErrNotSupported);
            }

        // Only one task arrival observation request
        // is allowed to be present at the same time
        if (iTaskArrivalListener && (aListener != iTaskArrivalListener))
            {
            User::Leave(KErrNotSupported);
            }
        }

    iTaskArrivalListener = aListener;
    }

void CEventMediatorServer::ClearTaskArrivalListener()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::ClearTaskArrivalListener\n")));
    iTaskArrivalListener = NULL;
    }

CListenerContainer* CEventMediatorServer::TaskArrivalListener()
    {
    return iTaskArrivalListener;
    }

void CEventMediatorServer::TaskRequestArrivedL(CListenerContainer* aTaskRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskRequestArrivedL\n")));
    CompleteTaskArrivalObservationRequestL(aTaskRequest->Type(), aTaskRequest->Specification());
    }

void CEventMediatorServer::TaskArrivalObservationRequestArrivedL()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskArrivalObservationRequestArrivedL\n")));
    // Go through all pending event listening requests to see if any
    // one of those should be passed to the SIT (i.e. if the task
    // arrival observation request should be completed immediately)
    
    CListenerContainer* taskRequest = FindWaitingTaskRequest();
    
    if (taskRequest)
        {
        CompleteTaskArrivalObservationRequestL(taskRequest->Type(), taskRequest->Specification());
        }
    }

void CEventMediatorServer::TaskRequestEventSpecFetchingRequestArrivedL(CListenerContainer* aFetchingRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskRequestEventSpecFetchingRequestArrivedL\n")));
    TFetchTaskInfoEventSpec taskRequestInfo;
    TPckg<TFetchTaskInfoEventSpec> taskRequestInfoDes(taskRequestInfo);
    taskRequestInfoDes.Copy(*(aFetchingRequest->Specification()));

    // Find the task request whose event specification
    // we should return to the SIT TH
    CListenerContainer* taskRequest = FindListener(taskRequestInfo.iEventType, taskRequestInfo.iEventSpecId);

    if (taskRequest && !taskRequest->BeingFulfilled())
        {
        CompleteTaskRequestEventSpecFetchingRequestL(KErrNone, aFetchingRequest->Specification(),
                                                     taskRequest->Specification());
        taskRequest->MarkAsBeingFulfilled();
        }
    else
        {
        CompleteTaskRequestEventSpecFetchingRequestL(KErrNotFound, aFetchingRequest->Specification(), NULL);
        }
    }

void CEventMediatorServer::TaskCancellationObservationRequestArrivedL(CListenerContainer* aRequest)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::TaskCancellationObservationRequestArrivedL\n")));
    // Try to find a task request event type that corresponds to the received
    // event type. This will only succeed if the received event type is
    // one that is used to listen to the cancellation of a task request.
    TEventType taskRequestEventType = CSit::FindTaskRequestEventType(aRequest->Type());

    // If a corresponding task request type was found...
    if (taskRequestEventType != EUnfoundEvent)
        {
        // Try to find the listener container of the task request
        CListenerContainer* taskRequest = FindListener(taskRequestEventType, aRequest->Specification());

        // The listener container for the task request was not found
        // (i.e. the task request has been cancelled or the
        // corresponding client session has been closed), so we
        // complete the cancellation observation request right away
        if (taskRequest == NULL)
            {
            ReportEventL(aRequest->Type(), aRequest->Specification(), NULL);
            }
        }
    }

void CEventMediatorServer::CompleteTaskArrivalObservationRequestL(TEventType aEventType, TDesC8* aSpec)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskArrivalObservationRequestL\n")));
    if (iTaskArrivalListener)
        {
        // In SIT events, the event specification
        // begins with the event specification ID
        TEventSpec* sitEventSpec = (TEventSpec*)(aSpec->Ptr());
        
        TTaskArrivedEventData eventData;
        eventData.iEventType = aEventType;
        eventData.iEventSpecId = sitEventSpec->iId;
        TPckg<TTaskArrivedEventData> eventDataDes(eventData);
        
        HBufC8* eventDataCopy = eventDataDes.AllocL();
        CleanupStack::PushL(eventDataCopy);

        ReportEventL(ETaskArrivedEvent, NULL, eventDataCopy);
        
        CleanupStack::Pop(); // eventDataCopy, freed elsewhere
        }
    }

void CEventMediatorServer::CompleteTaskRequestEventSpecFetchingRequestL(TInt aStatus, TDesC8* aEventSpec,
                                                                        TDesC8* aTaskRequestEventSpec)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskRequestEventSpecFetchingRequestL\n")));
    // The event specification of the task request
    // is returned to the SIT TH as event data
    if (aTaskRequestEventSpec)
        {
        HBufC8* eventData = aTaskRequestEventSpec->AllocL();
        CleanupStack::PushL(eventData);

        ReportEventL(EFetchTaskInfoEvent, aEventSpec, eventData, aStatus);

        CleanupStack::Pop(); // eventData, freed elsewhere
        }
    else
        {
        ReportEventL(EFetchTaskInfoEvent, aEventSpec, NULL, aStatus);
        }
    }
    
CListenerContainer* CEventMediatorServer::FindWaitingTaskRequest()
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindWaitingTaskRequest();
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }

CListenerContainer* CEventMediatorServer::FindListener(TEventType aEventType,    
                                                       TInt aEventSpecId)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindListener(aEventType, aEventSpecId);
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }
    
CListenerContainer* CEventMediatorServer::FindListener(TEventType aEventType,    
                                                       const TDesC8* aEventSpec)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    CListenerContainer* listener = NULL;
    
    while (session != NULL)
        {
        listener = session->FindListener(aEventType, aEventSpec);
        if (listener != NULL)
            {
            break;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return listener;
    }

void CEventMediatorServer::CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus)
    {
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        session->CompleteListener(aEventType, aEventSpec, aStatus);
        session = (CEventMediatorSession*) iSessionIter++;
        }
    }

TInt CEventMediatorServer::NormalSessionCount()
    {
    TInt normalSessionCount = 0;
    
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        if (!(session->IsASitSession()))
            {
            normalSessionCount++;
            }
        session = (CEventMediatorSession*) iSessionIter++;
        }

    return normalSessionCount;
    }

TInt CEventMediatorServer::NewEventSpecId()
    {
    return ++iNextEventSpecId;
    }

TPtrC CEventMediatorServer::EventLogFileName(void)
    {
    TPtrC name(iEventLogFileName);
    return name;
    }
    
void CEventMediatorServer::SitDied()
    {
    LOG(Log::Printf(_L("CEventMediatorServer::SitDied\n")));
    CompleteTaskRequests(KErrDied);
    }
    
void CEventMediatorServer::CompleteTaskRequests(TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorServer::CompleteTaskRequests\n")));    
    CEventMediatorSession* session;
    iSessionIter.SetToFirst();
    session = (CEventMediatorSession*) iSessionIter++;

    while (session != NULL)
        {
        session->CompleteTaskRequests(aStatus);
        session = (CEventMediatorSession*) iSessionIter++;
        }
    }

void CEventMediatorServer::SetShuttingDown(TBool aShuttingDown)
    {
    iShuttingDown = aShuttingDown;
    }

// ============================= CEventMediatorSession =============================     

CEventMediatorSession* CEventMediatorSession::NewL(CEventMediatorServer* aServer, const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::NewL - begin\n")));
    CEventMediatorSession* self;
    self = new (ELeave) CEventMediatorSession(aServer);
    CleanupStack::PushL(self);

    // Marks the session as a "SIT session"
    // if the client is the SIT thread
    self->iIsSitSession = aServer->IsClientTheSitL(aMessage);

    aServer->iSessionCount++;

    if ( !self->IsASitSession() )
        {
        User::LeaveIfError(self->iFs.Connect()); // For EventLog
        self->iEventLogFileOpen = EFalse;
        }

    CleanupStack::Pop(); // self

    LOG(Log::Printf(_L("CEventMediatorSession::NewL - end\n")));
    return self;
    }

CEventMediatorSession::CEventMediatorSession(CEventMediatorServer* aServer)
    : iListenedEvents(2), iServer(aServer)
    {
    }

CEventMediatorSession::~CEventMediatorSession(void)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::~CEventMediatorSession\n")));
    CancelAll();
    if (iServer)
        {        
        TInt normalSessionCount = iServer->NormalSessionCount();

        // If this too is a normal session and is dying,
        // decrement the normal session count by one
        if (!IsASitSession())
            {
            normalSessionCount--;
            
            if (iEventLogFileOpen)
                {
                iEventLogFile.Close();
                iEventLogFileOpen = EFalse;
                }
            iFs.Close(); // For EventLog
            
            }

        if (normalSessionCount == 0)
            {
            // If "normal" (non-sit) sessions are no longer present,
            // we complete the task arrival observation request, thus
            // causing the SIT to terminate and close its connection
            // to this server. This should be the last connection whose
            // closing will cause this server to terminate.
            // NOTE. KErrCancel cannot be used here as the Event Mediator 
            // does not report it to the caller
            LOG(Log::Printf(_L("CEventMediatorSession::~CEventMediatorSession - normal session count = 0\n")));
            iServer->CompleteListener(ETaskArrivedEvent, NULL, KErrAbort);

            // Set the server state to "shutting down". This will
            // cause the server to discard any new connect requests
            // with KErrServerTerminated.
            iServer->SetShuttingDown(ETrue);
            }

        if (iServer->iSessionCount)
            {
            iServer->iSessionCount--;
            }

        iServer->StopEventMediator();
        }
    }

void CEventMediatorSession::ServiceL(const RMessage2& aMessage)
    {
    TInt status = 0;

    switch (aMessage.Function())
        {
        case KEventMediatorListen:
            ListenToEventL(aMessage);
            break;

        case KEventMediatorListenWithSpec:
            ListenToEventWithSpecL(aMessage);
            break;

        case KEventMediatorCancel:
            CancelListening(aMessage);        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorCancelWithSpec:
            CancelListeningWithSpecL(aMessage);        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorCancelAll:
            CancelAll();        
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorReportEvent:
            ReportEventL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorReportEventWithSpec:
            ReportEventWithSpecL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorFetchData:
            status = FetchDataL(aMessage);
            aMessage.Complete(status);
            break;

        case KEventMediatorReportLogEvent:
            ReportLogEventL(aMessage);
            aMessage.Complete(KErrNone);
            break;

        case KEventMediatorNewEventSpecId:
            NewEventSpecIdL(aMessage);
            aMessage.Complete(KErrNone);
            break;
        case KEventMediatorDeletePrivateFiles:
            status = DeletePrivateFiles();
            aMessage.Complete(status);
            break;

        case KEventMediatorGetEventLogSize:
            status = GetEventLogSize(aMessage);
            aMessage.Complete(status);
            break;

        case KEventMediatorGetEventLogHeader:
            status = GetEventLogHeader(aMessage);
            aMessage.Complete(status);
            break;
        case KEventMediatorGetEventLogData:
            status = GetEventLogData(aMessage);
            aMessage.Complete(status);
            break;
        case KEventMediatorClearEventLog:
            status = ClearEventLog();
            aMessage.Complete(status);
            break;
        default:
            aMessage.Complete(KErrGeneral);
            break;
        }
    }

TInt CEventMediatorSession::ListenToEventL(const RMessage2& aMessage)
    {
    CListenerContainer* listener = new (ELeave) CListenerContainer(aMessage, NULL, iServer);
    CleanupStack::PushL(listener);

    iListenedEvents.AppendL(listener);
    CleanupStack::Pop(); // listener

    listener->AnalyzeRequestL();

    return KErrNone;
    }

TInt CEventMediatorSession::ListenToEventWithSpecL(const RMessage2& aMessage)
    {
    HBufC8* specBuf = NULL;

    // Read specification
    specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);

    // Ownership of specBuf is given to listener
    CListenerContainer* listener = new (ELeave) CListenerContainer(aMessage, specBuf, iServer);
    CleanupStack::Pop(); // specBuf
    CleanupStack::PushL(listener);
        
    iListenedEvents.AppendL(listener);
    CleanupStack::Pop(); // listener

    listener->AnalyzeRequestL();

    return KErrNone;
    }

void CEventMediatorSession::CancelListening(const RMessage2& aMessage)
    {
    TInt index;
    
    while (FindListenerMsg((TEventType)aMessage.Int0(), index))
        {
        CompleteListener(index, KErrCancel);
        }
    }

void CEventMediatorSession::CancelListeningWithSpecL(const RMessage2& aMessage)
    {
    HBufC8* specBuf = NULL;
    TInt index;
    
    // Read specification
    specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);

    // Cancel listeners
    while (FindListenerMsg((TEventType)aMessage.Int0(), specBuf, index))
        {
        CompleteListener(index, KErrCancel);
        }
    CleanupStack::PopAndDestroy(); // specBuf
    }

void CEventMediatorSession::CancelAll()
    {
    TInt nEvents = iListenedEvents.Count();
    for (TInt i = 0; i < nEvents; i++)  
        {
        iListenedEvents.At(i)->Complete(KErrCancel);
        delete iListenedEvents.At(i);
        iListenedEvents.At(i) = NULL;
        }
    iListenedEvents.Reset();
    }

void CEventMediatorSession::ReportEventL(const RMessage2& aMessage)
    {
    TEventType eventType = (TEventType)aMessage.Int0();
    LOG(Log::Printf(_L("CEventMediatorSession::ReportEventL - event type = %d\n"), eventType));
    // Read data
    HBufC8* dataBuf = ReadEventDataFromClientL(aMessage);
    CleanupStack::PushL(dataBuf);
    // Report event to server
    iServer->ReportEventL(eventType, NULL, dataBuf);
    CleanupStack::Pop();
    }

void CEventMediatorSession::ReportEventWithSpecL(const RMessage2& aMessage)
    {
    TEventType eventType=(TEventType) aMessage.Int0();
    LOG(Log::Printf(_L("CEventMediatorSession::ReportEventWithSpecL - event type = %d\n"), eventType));
    // Read data
    HBufC8* dataBuf = ReadEventDataFromClientL(aMessage);
    CleanupStack::PushL(dataBuf);
    // Read specification
    HBufC8* specBuf = ReadSpecificationFromClientL(aMessage);
    CleanupStack::PushL(specBuf);
    // Report event to server
    iServer->ReportEventL(eventType, specBuf, dataBuf);
    CleanupStack::PopAndDestroy(); // specBuf
    CleanupStack::Pop(); // dataBuf 
    }

void CEventMediatorSession::ReportLogEventL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::ReportLogEventL\n")));
    // Read event
    TLogEvent event;
    TPckg<TLogEvent> eventPckg(event);
    aMessage.ReadL(FIRST_ARGUMENT, eventPckg);

    // Create one buffer to contain put everything in a normal buffer and
    TInt lengthsDesLth = event.iDesCount * sizeof(TInt);
    TInt position = eventPckg.Length();
    TInt dataLength = position + lengthsDesLth + aMessage.Int2();
    HBufC8* dataBuf = HBufC8::NewLC(dataLength);
    TPtr8 dataPtr = dataBuf->Des();

    // Copy event to buffer
    dataPtr.Append(eventPckg);
    // Read lengths to buffer
    TPtr8 tmpPtr(&dataPtr[position], 0, dataLength - position);
    aMessage.ReadL(SECOND_ARGUMENT, tmpPtr);
    // Read descriptors to the buffer
    position= dataPtr.Length();
    tmpPtr.Set(&dataPtr[position], 0, dataLength - position);
    aMessage.ReadL(THIRD_ARGUMENT, tmpPtr);

    // Report event to server
    iServer->ReportEventL(ELogEvent, NULL, dataBuf);
    CleanupStack::Pop();
    }

TInt CEventMediatorSession::FetchDataL(const RMessage2& aMessage)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::FetchDataL\n")));
    return iServer->CopyEventDataL(aMessage);
    }

TInt CEventMediatorSession::CheckEventL(const TEventType aType, const TDesC8* aSpec,
                                        const TDesC8* aData, TInt aStatus)
    {
    TInt index;
    TInt listenerCount = 0;
    TInt dataLth = 0;
    // Some events don't not have data
    if (aData)
        {
        dataLth = aData->Length();
        }
    TPckg<TInt> lengthpckg(dataLth);
    TPckgC<const TAny*> ptrpckg(aData);

    while (FindListenerMsg(aType, aSpec, index))
        {
        RMessage2& listener = iListenedEvents.At(index)->Message();

        if (aStatus == KErrNone)
            {
            // Write info about data
            listener.WriteL(SECOND_ARGUMENT, lengthpckg);
            listener.WriteL(THIRD_ARGUMENT, ptrpckg);

            // Complete listener
            listener.Complete(KErrNone);
            }
        else
            {
            listener.Complete(aStatus);
            }
        
        delete iListenedEvents.At(index);
        iListenedEvents.Delete(index);
        listenerCount++;    
        }

    return listenerCount;
    }

TBool CEventMediatorSession::FindListenerMsg(const TEventType aType, TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->Type() == aType)
            {
            index=i;
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CEventMediatorSession::FindListenerMsg(const TEventType aType, const TDesC8* aSpec, TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->HandlesEvent(aType, aSpec))
            {
            index = i;
            return ETrue;
            }
        }
    return EFalse;
    }

TBool CEventMediatorSession::FindTaskRequestListenerMsg(TInt& index)
    {
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (CSit::EventRequiresSit(iListenedEvents.At(i)->Type()))
            {
            index = i;
            return ETrue;
            }
        }
    return EFalse;
    }
    
HBufC8* CEventMediatorSession::ReadSpecificationFromClientL(const RMessage2& aMessage)
    {
    HBufC8* specBuf;
    // Read specification descriptor length from client, create specification buffer
    const TAny* desPtr = aMessage.Ptr3();
    if (desPtr == NULL)
        {
        return NULL;
        }
    TInt specLength = aMessage.GetDesLength(FOURTH_ARGUMENT);
    // Create spcification buffer
    specBuf = HBufC8::NewLC(specLength);
    TPtr8 ptr = specBuf->Des();
    // Read specification
    aMessage.ReadL(FOURTH_ARGUMENT, ptr);
    CleanupStack::Pop(); // specBuf
    return specBuf;
    }

HBufC8* CEventMediatorSession::ReadEventDataFromClientL(const RMessage2& aMessage)
    {
    HBufC8* dataBuf = NULL;
    TInt desLength = aMessage.Int1();
    
    if (desLength != 0) // Some events have no data 
        {
        dataBuf = HBufC8::NewLC(desLength);
        TPtr8 ptr = dataBuf->Des();
        aMessage.ReadL(THIRD_ARGUMENT, ptr);
        CleanupStack::Pop(); // dataBuf
        }
    return dataBuf;
    }

CListenerContainer* CEventMediatorSession::FindWaitingTaskRequest()
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->WaitingForFulfilling())
            {
            listener = iListenedEvents.At(i);
            break;
            }
        }

    return listener;
    }

CListenerContainer* CEventMediatorSession::FindListener(TEventType aEventType,
                                                        TInt aEventSpecId)
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->Type() == aEventType)
            {
            TEventSpec* eventSpec = (TEventSpec*)(iListenedEvents.At(i)->Specification()->Ptr());

            if (eventSpec->iId == aEventSpecId)
                {
                listener = iListenedEvents.At(i);
                break;
                }
            }
        }

    return listener;
    }
    
CListenerContainer* CEventMediatorSession::FindListener(TEventType aEventType,
                                                        const TDesC8* aEventSpec)
    {
    CListenerContainer* listener = NULL;
    
    for (TInt i = 0; i < iListenedEvents.Count(); i++)
        {
        if (iListenedEvents.At(i)->HandlesEvent(aEventType, aEventSpec))
            {
            listener = iListenedEvents.At(i);
            break;
            }
        }

    return listener;
    }

void CEventMediatorSession::CompleteListener(TEventType aEventType,
                                             const TDesC8* aEventSpec,
                                             TInt aStatus)
    {
    TInt index;
    while (FindListenerMsg(aEventType, aEventSpec, index))
        {
        CompleteListener(index, aStatus);
        }
    }
        
void CEventMediatorSession::CompleteListener(TInt aIndex, TInt aStatus)
    {
    iListenedEvents.At(aIndex)->Complete(aStatus);
    delete iListenedEvents.At(aIndex);
    iListenedEvents.Delete(aIndex);
    }

void CEventMediatorSession::CompleteTaskRequests(TInt aStatus)
    {
    LOG(Log::Printf(_L("CEventMediatorSession::CompleteTaskRequests\n")));
    TInt index;
    while (FindTaskRequestListenerMsg(index))
        {
        CompleteListener(index, aStatus);
        }
    }

TBool CEventMediatorSession::IsASitSession()
    {
    return iIsSitSession;
    }

void CEventMediatorSession::NewEventSpecIdL(const RMessage2& aMessage)
    {
    TInt newEventSpecId = iServer->NewEventSpecId();
    TPckg<TInt> newEventSpecIdDes(newEventSpecId);
    aMessage.WriteL(FIRST_ARGUMENT, newEventSpecIdDes);
    }

TInt CEventMediatorSession::DeletePrivateFiles()
    {
    TRAPD(err, DeletePrivateFilesL());
    if ( err )
        {
        LOG(Log::Printf(_L("DeletePrivateFilesL() leave error %d\n"), err));
        return err;
        }

    return KErrNone;
    }

void CEventMediatorSession::DeletePrivateFilesL()
    {
    LOG(Log::Printf(_L("DeletePrivateFilesL() called\n")));

    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);

    TPath privateDir;
    User::LeaveIfError(iFs.PrivatePath(privateDir));

    TInt err = fileMan->RmDir(privateDir);
    if (err != KErrNone && err != KErrPathNotFound && err != KErrNotFound)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(); //fileMan
    }

TInt CEventMediatorSession::GetEventLogSize(
    const RMessage2& aMessage)
    {
    if ( iEventLogFileOpen )
        {
        iEventLogFile.Close();
        iEventLogFileOpen = EFalse;
        }

    TInt err = iEventLogFile.Open(iFs, iServer->EventLogFileName(), EFileRead | EFileShareAny);
    if ( err )
        return err;

    TInt size(0);
    err = iEventLogFile.Size(size);
    if ( err )
        {
        iEventLogFile.Close();
        return err;
        }
    
    TPckg<TInt> sizePckg(size);
    err = aMessage.Write(FIRST_ARGUMENT, sizePckg);
    if ( err )
        {
        iEventLogFile.Close();
        return err;
        }

    iEventLogFileOpen = ETrue;
    
    return KErrNone;
    }

TInt CEventMediatorSession::GetEventLogHeader(
    const RMessage2& aMessage)
    {
    TInt err(0);
    
    if ( !iEventLogFileOpen )
        {
        err = iEventLogFile.Open(iFs, iServer->EventLogFileName(), EFileRead | EFileShareAny);
        if ( err )
            return err;
        iEventLogFileOpen = ETrue;
        }
    
    TInt position = 0;
    err = iEventLogFile.Seek(ESeekStart, position);     
    if (err != KErrNone)
        return err;

    TBuf8<EVENTLOG_FILE_HEADER_LTH> fileHeaderBuf;
    err = iEventLogFile.Read(fileHeaderBuf, EVENTLOG_FILE_HEADER_LTH);
    if (err != KErrNone)
        return err;

    TRAP(err, aMessage.WriteL(FIRST_ARGUMENT, fileHeaderBuf));
    if ( err )
        return err;

    return KErrNone;
    }
    
TInt CEventMediatorSession::GetEventLogData(
    const RMessage2& aMessage)
    {
    TInt err(0);

    if ( !iEventLogFileOpen )
        {
        err = iEventLogFile.Open(iFs, iServer->EventLogFileName(), EFileRead | EFileShareAny);
        if ( err )
            return err;
        iEventLogFileOpen = ETrue;
        }
    
    TInt size(0);
    err = iEventLogFile.Size(size);
    if ( err )
        return err;

    if ( size < EVENTLOG_FILE_HEADER_LTH )
        return KErrNotFound;

    HBufC8* eventLogFileBuf = NULL;
    TRAP(err, eventLogFileBuf = HBufC8::NewL(size));
    if ( err )
        {
        return err;
        }
    
    TPtr8 eventLogDataPtr(eventLogFileBuf->Des());
    TInt position(0);
    err = iEventLogFile.Seek(ESeekStart, position);
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }
    err = iEventLogFile.Read(eventLogDataPtr); // iLogFileSize);
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }
    
    TRAP( err, aMessage.WriteL(FIRST_ARGUMENT, eventLogDataPtr));
    if ( err )
        {
        delete eventLogFileBuf;
        return err;
        }

    delete eventLogFileBuf;
    eventLogFileBuf = NULL;
    
    return KErrNone;
    }
    
TInt CEventMediatorSession::ClearEventLog()
    {
    if ( iEventLogFileOpen )
        {
        iEventLogFile.Close();
        iEventLogFileOpen = EFalse;
        }

    TInt err = iFs.Delete(iServer->EventLogFileName());

    return err;
    }


    
// ============================= CEventMediatorServer =============================    

CListenerContainer::CListenerContainer(const RMessage2& aMessage, TDesC8* aSpec,
                                       CEventMediatorServer* aServer)
    : iSpec(aSpec), iMessage(aMessage), iServer(aServer)
    {
    iEventType = Type();
    }

void CListenerContainer::AnalyzeRequestL()
    {
    LOG(Log::Printf(_L("CListenerContainer::AnalyzeRequestL\n")));
    if (CSit::EventRequiresSit(iEventType))
        {
        LOG(Log::Printf(_L("CListenerContainer::AnalyzeRequestL - event type = %d, requires SIT\n"), iEventType));
        iServer->MakeSureSitIsRunningL();
        iServer->TaskRequestArrivedL(this);
        }

    // If this event listening request is the one
    // made by the SIT task arrival observer...
    if (iEventType == ETaskArrivedEvent)
        {
        iServer->SetTaskArrivalListenerL(this);
        iServer->TaskArrivalObservationRequestArrivedL();
        }

    // If this event listening request is one
    // made by a SIT TH to fetch a task...
    if (iEventType == EFetchTaskInfoEvent)
        {
        iServer->TaskRequestEventSpecFetchingRequestArrivedL(this);
        }

    // If the event listening request is one
    // made by a SIT TH to listen to the cancellation
    // of the task request it is handling
    if (CSit::IsTaskCancellationObservationRequest(iEventType))
        {
        iServer->TaskCancellationObservationRequestArrivedL(this);
        }
    }
    
CListenerContainer::~CListenerContainer()
    {
    delete iSpec;

    if (iServer->TaskArrivalListener() == this)
        {
        iServer->ClearTaskArrivalListener();
        }
    }

TBool CListenerContainer::HandlesEvent(TEventType aEventType, const TDesC8* aEventSpec)
    {
    if (iEventType == aEventType)
        {
        // Specs are same if both are NULL
        if (iSpec == NULL && aEventSpec == NULL)
            {
            return ETrue;
            }
        // or data in buffers are identical
        else if (iSpec != NULL && aEventSpec != NULL && (*iSpec) == (*aEventSpec))
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
void CListenerContainer::Complete(TInt status)
    {
    // If there's a SIT fulfilling this event listening
    // request (i.e. if this is a task request) and this
    // task request is being cancelled...
    if (status == KErrCancel)
        {
        // See if we can find a task request cancellation observation
        // event type that corresponds to this event type
        TEventType cancelEventType = CSit::FindCancelEventType(iEventType);
        
        if (cancelEventType != EUnfoundEvent)
            {
            // Complete the task request cancellation
            // observation, if found
            iServer->CompleteListener(cancelEventType, iSpec, KErrNone);
            }
        }
    
    iMessage.Complete(status);
    }

TBool CListenerContainer::WaitingForFulfilling()
    {
    if (CSit::EventRequiresSit(iEventType) && !iBeingFulfilledBySit)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CListenerContainer::MarkAsBeingFulfilled()
    {
    iBeingFulfilledBySit = ETrue;
    }

TBool CListenerContainer::BeingFulfilled()
    {
    return iBeingFulfilledBySit;
    }
