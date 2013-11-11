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
* Description:   This module defines the API to EventMediator.
*
*/


#include <e32std.h>
#include "eventmediator.h"
#include "eventmediatorapi.h"
#include "eventlogger.h"
#include "clistatic.h"
#include "log.h"

/**---------------------------------------------------------
 *
 * REventMediator class constructor
 *
 *----------------------------------------------------------*/
EXPORT_C REventMediator::REventMediator() : iListenedEvents(NULL)
    {
    }

/**---------------------------------------------------------
 *
 * Connect()
 *
 * Opens a session to EventMediator server and starts the server if it
 * not yet started
 * 
 * Returns: KErrNone: OK
 *          value: error
 *
 *----------------------------------------------------------*/
EXPORT_C TInt REventMediator::Connect(void)
    {
    TInt ret = KErrNone;
    TRAP(ret, CreateListenedEventsListL());
    if (ret != KErrNone)
        {
        return ret;
        }
    
    TInt retry=2;
    for (;;)
        {
        TInt r=CreateSession(KEventMediatorServer,
                             TVersion(KEventMediatorMajorVersionNumber,
                                      KEventMediatorMinorVersionNumber,
                                      KEventMediatorBuildVersionNumber),
                             2 * KDefaultMessageSlots);
        
        if (r!=KErrNotFound && r!=KErrServerTerminated)
            return r;
        if (--retry==0)
            return r;
        r = Launcher::LaunchServer(KEventMediatorServer, KEventMediatorFile,
                                   KEventMediatorUid3);

        if (r!=KErrNone && r!=KErrAlreadyExists)
            return r;
        }
    }

void REventMediator::CreateListenedEventsListL()
    {
    iListenedEvents = new (ELeave) CArrayFixFlat<CEventListener*>(1);
    }

// ---------------------------------------------------------------------------
// Closes a session to EventMediator server
// ---------------------------------------------------------------------------
//
EXPORT_C void REventMediator::Close()
    {
    if ( iListenedEvents )
        {
        TInt nEvents( iListenedEvents->Count() );
        for (TInt i = 0; i < nEvents; i++)  
            delete iListenedEvents->At(i);
        iListenedEvents->Delete(0,iListenedEvents->Count());
        delete iListenedEvents;
        iListenedEvents = NULL;
        }
    RSessionBase::Close();
    }

EXPORT_C void REventMediator::ListenToEvent(TEventType aType, MEventObserver& aObserver)
    {
    LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, MEventObserver& aObserver)\n")));

    TInt status=KErrNone;
    CEventListener* listener=NULL;
    TRAP(status, listener=NewEventListenerL(aType,aObserver);)

    if(status==KErrNone)
        {
        LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, MEventObserver& aObserver) - calling SendReceive\n")));
            
        SendReceive(CEventMediatorSession::KEventMediatorListen,
                    TIpcArgs(aType,
                             &(listener->iDataLengthPckg),
                             &(listener->iSrvDataPtrPckg)),
                    listener->iStatus);

        LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, MEventObserver& aObserver) - SendReceive called\n")));
        
        listener->SetActive();
        }
    else
        aObserver.EventOccured(status, aType, NULL);
    }

EXPORT_C void REventMediator::ListenToEvent(TEventType aType, TDesC8& aEventSpec, MEventObserver& aObserver)
    {
    LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, TDesC8& aEventSpec, MEventObserver& aObserver)\n")));
    
    TInt status=KErrNone;
    CEventListener* listener=NULL;
    TRAP(status, listener=NewEventListenerL(aType,aObserver);)

    if(status==KErrNone)
        {
        listener->iSpecBuf = aEventSpec.Alloc();
        if (listener->iSpecBuf==NULL)
            status = KErrNoMemory;
        }

    if(status==KErrNone)
        {
        LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, TDesC8& aEventSpec, MEventObserver& aObserver) - calling SendReceive\n")));
        
        SendReceive(CEventMediatorSession::KEventMediatorListenWithSpec,
                    TIpcArgs(aType,
                             &(listener->iDataLengthPckg),
                             &(listener->iSrvDataPtrPckg),
                             listener->iSpecBuf),
                    listener->iStatus);

        LOG(Log::Printf(_L("REventMediator::ListenToEvent(TEventType aType, TDesC8& aEventSpec, MEventObserver& aObserver) - SendReceive called\n")));
        
        listener->SetActive();
        }
    else
        aObserver.EventOccured(status, aType, NULL);
    }

EXPORT_C void REventMediator::CancelListening(TEventType aType)
    {
    LOG(Log::Printf(_L("REventMediator::CancelListening(TEventType aType)\n")));
    
    TInt index = -1;
    while ( FindEventListener( aType, index ) )
    	{
    	CEventListener* listener = iListenedEvents->At( index );
    	listener->Cancel();
    	RemoveListener( listener );
    	}
    }

EXPORT_C TInt REventMediator::CancelListening(TEventType aType, TDesC8& aEventSpec)
    {
    LOG(Log::Printf(_L("REventMediator::CancelListening(TEventType aType, TDesC8& aEventSpec)\n")));
    
    TInt index = -1;
    while ( FindEventListener( aType, aEventSpec, index ) )
    	{
    	CEventListener* listener = iListenedEvents->At( index );
    	listener->Cancel();
    	RemoveListener( listener );
    	}
    	
	return KErrNone;    
    }

EXPORT_C void REventMediator::CancelAllListening()
    {
    LOG(Log::Printf(_L("REventMediator::CancelAllListening()\n")));
    
    while ( iListenedEvents->Count() )
    	{
    	CEventListener* listener = iListenedEvents->At( 0 );
        listener->Cancel();
        RemoveListener( listener );    	            	    	
    	}
    }

EXPORT_C TInt REventMediator::ReportEvent(TEventType aType)
    {
    return SendReceive(CEventMediatorSession::KEventMediatorReportEvent, TIpcArgs(aType, 0, NULL));
    }

EXPORT_C TInt REventMediator::ReportEvent(TEventType aType, TDesC8& aData)
    {
    return SendReceive(CEventMediatorSession::KEventMediatorReportEvent, TIpcArgs(aType, aData.Length(), &aData));
    }

EXPORT_C TInt REventMediator::ReportEvent(TEventType aType, TDesC8& aEventSpec, TDesC8& aData)
    {
    return SendReceive(CEventMediatorSession::KEventMediatorReportEventWithSpec, TIpcArgs(aType, aData.Length(), &aData, &aEventSpec));
    }

EXPORT_C TInt REventMediator::ReportLogEvent(TUid& aSrc, TLogCategory aCategory, TUint aMsgId, TInt aDesCount,...)
    {
    VA_LIST list;
    TInt err;
    TLogEvent event(aSrc, aCategory, aMsgId,aDesCount);
    TPckg<TLogEvent> eventPckg(event);
    HBufC8* desBuf=NULL;
    TInt desBufLength;
    // alloc array for descripor pointers
    TDesC8** pointers = new TDesC8*[aDesCount];
    TInt* lengths = new TInt[aDesCount];
    TInt lengthsDesLth = aDesCount*sizeof(TInt); // length of a descriptor containing lengths array.

    // read arguments to array
    VA_START(list, aDesCount);
    err = ReadLogArguments( aDesCount, list, pointers,lengths, desBufLength);
    VA_END(list);

    if(err==KErrNone)
        {
        // Read arguments to one descriptor
        desBuf = HBufC8::New(eventPckg.Length()+lengthsDesLth+desBufLength);
        if(desBuf==NULL)
            err=KErrNoMemory;
        else
            {
            TPtr8 desPtr= desBuf->Des();
            desPtr.Append(eventPckg);
            desPtr.Append((TUint8*)lengths,lengthsDesLth);
            for(TInt i=0; i < aDesCount; i++)
                {
                desPtr.Append(*(pointers[i]));
                TInt fillerLth = 4 -(lengths[i] % 4);
                TChar filler(0);
                if (fillerLth > 0 && fillerLth < 4)
                    {
                    desPtr.AppendFill(filler , fillerLth);
                    }
                }
            err=ReportEvent(ELogEvent,desPtr);
            }
        }
    delete[] pointers;
    delete[] lengths;
    delete desBuf;
    return err;
    }

EXPORT_C TInt REventMediator::NewEventSpecId()
    {
    TInt specId = 0;
    TPckg<TInt> specIdDes(specId);

    SendReceive(CEventMediatorSession::KEventMediatorNewEventSpecId, TIpcArgs(&specIdDes));

    return specId;
    }

void REventMediator::CancelListenToEvent(TEventType aType)
    {
    LOG(Log::Printf(_L("REventMediator::CancelListenToEvent(TEventType aType)\n")));
    
    SendReceive(CEventMediatorSession::KEventMediatorCancel, TIpcArgs(aType));
    }

TInt REventMediator::CancelListenToEvent(TEventType aType, TDesC8& aEventSpec)
    {
    LOG(Log::Printf(_L("REventMediator::CancelListenToEvent(TEventType aType, TDesC8& aEventSpec)\n")));
    
    return SendReceive(CEventMediatorSession::KEventMediatorCancelWithSpec, TIpcArgs(aType, NULL, NULL, &aEventSpec));
    }

TInt REventMediator::FetchData(TAny* aSrvPtr, TDes8& aDataPtr)
    {
    LOG(Log::Printf(_L("REventMediator::FetchData()\n")));
    
    TRequestStatus status;
    SendReceive(CEventMediatorSession::KEventMediatorFetchData,
                TIpcArgs(aSrvPtr, &aDataPtr), status);
    User::WaitForRequest(status);

    LOG_1("REventMediator::FetchData() - SendReceive called, status: %d\n",status.Int());
    return status.Int();
    }

CEventListener* REventMediator::NewEventListenerL(TEventType aType, MEventObserver& aObserver)
    {
    CEventListener* listener=NULL;

    listener = new (ELeave) CEventListener(aType, aObserver,this);
    CleanupStack::PushL(listener);
    iListenedEvents->AppendL(listener);
    CleanupStack::Pop();
    return listener;
    }

void REventMediator::RemoveListener(CEventListener* aListener)
    {
    TInt index=-1;
    for(TInt i=0; i<iListenedEvents->Count(); i++)
        {
        if(iListenedEvents->At(i)==aListener)
        	{
        	index=i;
        	break;
        	}            
        }
    if (index >=0)
        {
        delete iListenedEvents->At(index);
        iListenedEvents->Delete(index);
        }
    }

TBool REventMediator::FindEventListener(const TEventType aType, TInt& aIndex)
    {        
    for( TInt i=0; i<iListenedEvents->Count(); i++ )
    	{
	    if( iListenedEvents->At(i)->iType == aType )
	    	{
	    	aIndex = i;
	    	return ETrue;
	    	}            
	    }
	return EFalse;
    }

TBool REventMediator::FindEventListener(const TEventType aType, TDesC8& aEventSpec, TInt& aIndex)
    {
    for( TInt i=0; i<iListenedEvents->Count(); i++ )
        {
        if( iListenedEvents->At(i)->iType == aType &&
        	iListenedEvents->At(i)->iSpecBuf &&
            iListenedEvents->At(i)->iSpecBuf->Des().Compare( aEventSpec ) == 0 )
        	{
        	aIndex = i;
        	return ETrue;
        	}            
        }
	return EFalse;
    }

TInt REventMediator::ReadLogArguments(TInt aCount, VA_LIST aList, TDesC8** aPointers, TInt* aLengths, TInt& aOverAllLength)
    {
    if(aPointers==NULL || aLengths==NULL)
        return KErrNoMemory;
    aOverAllLength=0;
    for (TInt i=0; i<aCount; i++)
        {
        aPointers[i] = (TDesC8*) VA_ARG(aList, TDesC8*);
        aLengths[i] = aPointers[i]->Length();
        aOverAllLength += aLengths[i];
        TInt fillerLth = 4 - (aLengths[i] % 4);
        if (fillerLth > 0 && fillerLth < 4)
            {
            aOverAllLength += fillerLth;
            }

        }
    return KErrNone;
    }

EXPORT_C TInt REventMediator::DeletePrivateFiles()
    {
    return SendReceive (CEventMediatorSession::KEventMediatorDeletePrivateFiles, TIpcArgs());
    }

    
EXPORT_C TInt REventMediator::GetEventLogSize(TInt& aEventLogSize)
    {
    TPckg<TInt> eventLogSizePckg(aEventLogSize);

    return SendReceive (CEventMediatorSession::KEventMediatorGetEventLogSize,
                        TIpcArgs(&eventLogSizePckg));
    }

EXPORT_C TInt REventMediator::GetEventLogHeader(TDes8& aEventLogHeader)
    {
    return SendReceive (CEventMediatorSession::KEventMediatorGetEventLogHeader,
                        TIpcArgs(&aEventLogHeader));
    }

EXPORT_C TInt REventMediator::GetEventLogData(TDes8& aEventLogData)
    {
    return SendReceive (CEventMediatorSession::KEventMediatorGetEventLogData,
                        TIpcArgs(&aEventLogData));
    }

EXPORT_C TInt REventMediator::ClearEventLog()
    {
    return SendReceive (CEventMediatorSession::KEventMediatorClearEventLog, TIpcArgs());
    }


void CEventListener::DoCancel()
    {
    if (iSpecBuf)
        iSession->CancelListenToEvent(iType, *iSpecBuf);
    else
        iSession->CancelListenToEvent(iType);
    }

void CEventListener::RunL()  // Should it leave? no
    {
    LOG(Log::Printf(_L("REventMediator::RunL()\n")));
    
    if (iState == EStateListening)
        {
        HandleListeningComplete();
        }
    }

void CEventListener::HandleListeningComplete()
    {
    LOG(Log::Printf(_L("REventMediator::HandleListeningComplete()\n")));
    
    // Event has occured, iSpecBuf is useless.
    delete iSpecBuf;
    iSpecBuf = NULL;

    TInt status=iStatus.Int();

    LOG(Log::Printf(_L("REventMediator::HandleListeningComplete() - status = %d\n"), status));    
    
    if (status==KErrNone)
        {
        TInt dataLength = iDataLengthPckg();
        // Some events might not have data
        if (dataLength)
            {
            iDataBuf = HBufC8::New(dataLength);
            if (iDataBuf)
                {
                LOG(Log::Printf(_L("REventMediator::HandleListeningComplete() - going to call FetchData()\n")));
                    
                iDataPtr.Set(iDataBuf->Des());
                TInt err = iSession->FetchData(iSrvDataPtrPckg(), iDataPtr);
                iState = EStateFetchingData;
                // Event reporting to the observer and listener 
                // cleanup are handled in HandleFetchingComplete
                HandleFetchingComplete(err);
                return;
                }
            else 
                status = KErrNoMemory;
            }
        iObserver.EventOccured(status, iType, NULL);
        }
    else if (status!=KErrCancel) // Cancels are not reported
        {
        iObserver.EventOccured(status, iType, NULL);
        }
    
    // Listened event has occured, so this listener is done
    iSession->RemoveListener(this);
    }

void CEventListener::HandleFetchingComplete(TInt aError)
    {
    LOG(Log::Printf(_L("REventMediator::HandleFetchingComplete()\n")));
    
    LOG(Log::Printf(_L("REventMediator::HandleFetchingComplete() - status = %d\n"), aError));
    
    if (aError==KErrNone)
        {
        iObserver.EventOccured(aError, iType, iDataBuf);
        }
    else if (aError!=KErrCancel) // Cancels are not reported
        {
        iObserver.EventOccured(aError, iType, NULL);
        }
    
    // Listened event has occured, so this listener is done
    iSession->RemoveListener(this);  
    }
    
CEventListener::CEventListener(TEventType aType,MEventObserver& aObserver, REventMediator* aSession) : 
        CActive(0), iType(aType), iObserver(aObserver), iSession(aSession),
        iDataPtr(NULL, 0), iState(EStateListening)
    {
    CActiveScheduler::Add(this);
    }

CEventListener::~CEventListener()
    {
    Cancel();
    delete iSpecBuf;
    iSpecBuf = NULL;
    delete iDataBuf;
    iDataBuf = NULL;
    }

