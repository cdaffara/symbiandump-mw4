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
* Description:   This module defines the API to get services from EventMediator.
*
*/



/**  
 * @file eventmediatorapi.h
 *
 * This module defines the API to get services from EventMediator.
 *
 */

#ifndef __EVENTMEDIATOR_API_H__
#define __EVENTMEDIATOR_API_H__

#include <es_enum.h>
#include "vpnapidefs.h"
#include "vpnmandefs.h"

#define EVENTLOG_FILE_HEADER_LTH     16

/**
 *
 *  Enumeration of supported events types.
 *
 */
enum TEventType
    {
    EUnfoundEvent,
    ETestEvent,
    ESystemEvents = 10000,
    EAllInterfaceEvents,
    ESingleInterfaceEvent,
    ELogEvent,
    EKmdAddressChangeEvent,
    EKmdCertificateChangeEvent,
    EKmdRealIapConnDownEvent,
    // SIT events
    EGetProtocolVersionEvent = 20000,
    EStartVpnConnEvent,
    ECloseVpnConnEvent,
    EStartRealIapConnEvent,
    EObserveRealIapConnEvent,
    // Internal use only
    ETaskArrivedEvent = 30000,
    EFetchTaskInfoEvent,
    EGetProtocolVersionCancelEvent, 
    EStartVpnConnCancelEvent,
    ECloseVpnConnCancelEvent,
    EStartRealIapConnCancelEvent,
    EObserveRealIapConnCancelEvent,
    EProgressEvent
    };

/**
 *
 *  Enumeration of log event categories.
 *
 */
enum TLogCategory
    {
    EInfo,
    EWarning,
    EError,
    EDebug
    };

/**
 * The following definitions are used with TEventType
 * ESingleInterfaceEvent and EObserveRealIapConnEvent
 */

enum TSingleConnInterfaceState
    {
    ESingleInterfaceStateUnknown = -1, 
    ESingleInterfaceUp,
    ESingleInterfaceDown,
    ESingleInterfaceTerminated, // Authoritative stop
    };

class TSingleInterfaceNotification
    {
public:
    TConnectionInfo iConnectionInfo;
    TSingleConnInterfaceState iState;
    };

typedef TPckgBuf<TSingleInterfaceNotification> TSingleInterfaceNotificationBuf;

/**
 * Generic event data definition
 */
class TEventSpec
    {
public:
    inline TEventSpec() : iId(0) {}
    TInt iId;
    };

/**
 * Generic event data definition
 */
class TEventData
    {
public:
    inline TEventData() : iTaskStatus(KErrNone) {}
    TInt iTaskStatus;
    };

/**
 * Used by SIT to get informed about new tasks.
 */
class TTaskArrivedEventData
    {
public:
    inline TTaskArrivedEventData()
        :iEventType(EUnfoundEvent), iEventSpecId(0)
        {}
    TEventType iEventType;
    TInt iEventSpecId;
    };

/**
 * Used by SIT to fetch the event specification of a new task.
 */
typedef TTaskArrivedEventData TFetchTaskInfoEventSpec;

/**
 *
 *  MEventObserver - This class must be implemented to use ListenToEvent of EventMediatorApi
 *
 */
class MEventObserver
    {
public:
    /**
     * Called by REventMediator when a listened event has been reported.
     * @param aStatus: error code, KErrNone if successfull.
     * @param aType: the type of the event.
     * @param aData: a pointer to a descriptor containing reported data about the event.
     * aData pointer is valid only during the execution of EventOccured.
     * A copy of the descriptor must be made to store the data for later use.
     */
    virtual void EventOccured(TInt aStatus, TEventType aType, TDesC8* aData)=0;
    };

class CEventListener;

/**
 *
 *  REventMediator - API class for EventMediator 
 *
 */
class REventMediator:public RSessionBase
    {
/**
 ** @internalComponent
 */
    friend class CEventListener;
public:
    /**
     * Default constructor
     */
    IMPORT_C REventMediator(void);

    /**
     * Creates session to the EventMediator server. If server is
     * not running, it is started.
     */
    IMPORT_C TInt Connect();

    /**
     * Closes the session to the VPN Manager.
     */
    IMPORT_C void Close();  

    /**
     * Sends asynchronous event notification request.
     * Called after Connect() request.
     * @param aType: the type of the event.
     * @param aObserver: object whose EventOccured method is called when an event
     * of type aType has been reported.
     */
    IMPORT_C void ListenToEvent(TEventType aType, MEventObserver& aObserver);

    /**
     * Sends asynchronous event notification request.
     * Called after Connect() request.
     * @param aType: the type of the event.
     * @param aEventSpec: specific info on listened event.
     * @param aObserver: object whose EventOccured method is called when an event
     * of type aType has been reported.
     */
    IMPORT_C void ListenToEvent(TEventType aType, TDesC8& aEventSpec, MEventObserver& aObserver);

    /**
     * Cancels all ListenToEvent requests concerning this event type.
     * Called after Connect() request.
     * @param aType: the type of the event.
     */
    IMPORT_C void CancelListening(TEventType aType);

    /**
     * Cancels ListenToEvent request geiven with a specification. 
     * Notice that this may fail.
     * Called after Connect() request.
     * @param aEventSpec: specific info on listened event.
     * @param aType: the type of the event.
     * @return error code, KErrNone if successfull.
     */
    IMPORT_C TInt CancelListening(TEventType aType, TDesC8& aEventSpec);

    /**
     * Cancels all asynchronous ListenToEvent requests.
     * Called after Connect() request.
     */
    IMPORT_C void CancelAllListening();

    /**
     * Sends synchronous request to report an event.
     * Called after Connect() request.
     * @param aType: the type of the event.
     * @param aData: a descriptor containing data about the event.
     * @return error code, KErrNone if successfull.
     */
    IMPORT_C TInt ReportEvent(TEventType aType);

    /**
     * Sends synchronous request to report an event.
     * Called after Connect() request.
     * @param aType: the type of the event.
     * @param aData: a descriptor containing data about the event.
     * @return error code, KErrNone if successfull.
     */
    IMPORT_C TInt ReportEvent(TEventType aType, TDesC8& aData);

    /**
     * Sends synchronous request to report an event.
     * Called after Connect() request.
     * @param aType: the type of the event.
     * @param aEventSpec: specific info on listened event.
     * @param aData: a descriptor containing data about the event.
     * @return error code, KErrNone if successfull.
    */
    IMPORT_C TInt ReportEvent(TEventType aType, TDesC8& aEventSpec, TDesC8& aData);

     /**
     * Sends synchronous request to report an event.
     * Called after Connect() request.
     * @param aTSrc: source of the event.
     * @param aCategory: category of the log event.
     * @param aMsgId: Id of the log message.
     * @param aDesCount: number of additional arguments (which must be of type TDesC8*).
     * @return error code, KErrNone if successfull.
     */
    IMPORT_C TInt ReportLogEvent(TUid& aSrc, TLogCategory aCategory, TUint aMsgId,TInt aDesCount,...);

     /**
     * Returns a new event specification
     * ID to be used with SIT related events.
     */
    IMPORT_C TInt NewEventSpecId();

     /**
     * Requests the Event Mediator to delete the files it owns.
     */
    IMPORT_C TInt DeletePrivateFiles();

    IMPORT_C TInt GetEventLogSize(TInt& aEventLogSize);

    IMPORT_C TInt GetEventLogHeader(TDes8& aEventLogHeader);

    IMPORT_C TInt GetEventLogData(TDes8& aEventLogData);

    IMPORT_C TInt ClearEventLog();

    /**
     * Sends a request to cancel all ListenToEvent requests concerning this
     * event type.
     * @param aType: the type of the event.
     */
    void CancelListenToEvent(TEventType aType);

    /**
     * Sends a request to cancel all ListenToEvent requests with a given
     * specification. 
     * @param aEventSpec: specific info on listened event.
     * @param aType: the type of the event.
     * @return error code, KErrNone if successfull.
     */
    TInt CancelListenToEvent(TEventType aType, TDesC8& aEventSpec);

private:
    void CreateListenedEventsListL();
    
    /**
     * Sends synchronous request to get data repeorted with an event.
     * Called after Connect() request.
     * @param aSrvPtr: pointer to the data in the servers address space.
     * The server checks whether this pointer is valid before using it.
     * @param aData: a descriptor to which the datais written.
     * @return Error status
     */
    TInt FetchData(TAny* aSrvPtr, TDes8& aDataDecriptor);

    /**
     * Called by ListenToEvent function.
     * Creates a new instance of CEventListener to receive notification about the event.
     * Adds thsi object to iListenedEvents.
     * @param aType: the type of the event.
     * @param aObserver: object whose EventOccured function is called when event is received.
     * @return the new listener.
     */
    CEventListener* NewEventListenerL(TEventType aType, MEventObserver& aObserver);

    /**
     * Deletes listener created with NewEventListener
     * @param aType: the type of the event.
     */
    void RemoveListener(CEventListener* aListener);

    /**
     * Creates EventMediator.
     */
    TInt StartEventMediator();

    /**
     * Looks up a listener from iListenedEvents.
     * @param aType: type of the event the listener is waiting for.
     * @param aIndex: the position of the listener in iListenedEvents.
     * @return ETrue if listener exists, EFalse if not.
     */
    TBool FindEventListener(const TEventType aType, TInt& aIndex);
    
    /**
     * Looks up a listener from iListenedEvents.
     * @param aType: type of the event the listener is waiting for.
     * @param aEventSpec event specification
     * @param aIndex: the position of the listener in iListenedEvents.
     * @return ETrue if listener exists, EFalse if not.
     */
    TBool FindEventListener(const TEventType aType, TDesC8& aEventSpec, TInt& aIndex);

    /**
     * Reads the variable arguments of ReportLogEvent.
     * @param aCount: Number of arguments.
     * @param aList: list of arguments.
     * @param aPointers: array to be filled with argument pointers.
     * @param aLengths: array to be filled with argument lengths.
     * @param aOverAllLength: sum of argument lengths
     * @return error code, KErrNone if successfull.
     */
    TInt ReadLogArguments(TInt aCount, VA_LIST aList,TDesC8** aPointers, TInt* aLengths, TInt& aOverAllLength);
    
private: //Data
    // list of listeners created with function NewEventListener 
    CArrayFixFlat<CEventListener*>* iListenedEvents;
    TInt64 iRandSeed;
    };

/**
 *
 *  CEventListener - A class used by REventListener to Store information about the listened events.
 *
 */
NONSHARABLE_CLASS(CEventListener) : public CActive
    {
/**
 ** @internalComponent
 */
    friend class REventMediator;
    enum TListenerState { EStateListening = 0, EStateFetchingData};
public:
    /**
     * standard deconstructor
     */
    ~CEventListener();
private:
    /**
     * Constructor.
     * @param aType: type of the event that is listened.
     * @param aObserver: object whose EventOccured is called when event has occured.
     * @param aSession: pointer to the owning REventMediator object.
     */
    CEventListener(TEventType aType, MEventObserver& aObserver, REventMediator* aSession);

    /**
     * Implement RunL of class CActive.
     */
    void RunL();

    /**
     * Handles the completion of event listening request
     */
    void HandleListeningComplete();

    /**
     * Handles the completion of event data fetching request
     * @param Error status
     */
    void HandleFetchingComplete(TInt aError);

    /**
     * Implements DoCancel of class CActive.
     */
    void DoCancel();

private: //data
    // buffer to receive the length of the data descriptor of an reported event.
    TPckgBuf<TInt> iDataLengthPckg;
    // buffer to receive the pointer to the data descriptor in servers address space.
    TPckgBuf<TAny*> iSrvDataPtrPckg;
    // type of the event that is listened.
    TEventType iType;
    // the observer given in constructor.
    MEventObserver& iObserver;
    // the session given in constructor
    REventMediator* iSession;

    // possible extra specifications on listened events
    HBufC8* iSpecBuf;

    // Event data
    HBufC8* iDataBuf;

    // Event data as a modifiable descriptor
    TPtr8 iDataPtr;

    // Internal state
    TListenerState iState;
    };

#endif // __EVENTMEDIATOR_API_H__
