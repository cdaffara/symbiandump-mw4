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
* Description:   This module defines event mediator and the services of it.
*
*/



/**  
 * @file eventmediator.h
 *
 * This module defines event mediator and the services of it.
 *
 */

#ifndef __EVENTMEDIATOR_H__
#define __EVENTMEDIATOR_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "eventmediatorapi.h"
#include "eventmediatordefs.h"

#include "sitdeathobserver.h"

// FORWARD DECLARATIONS
class CEventMediatorSession;
class TEventContainer;
class CEventLogger;

class CSit;

class CListenerContainer;

// CLASS DECLARATION

/**
* Defines the event mediator server. 
*/
class CEventMediatorServer : public CPolicyServer, public MSitDeathListener
    {
    friend class CEventMediatorSession; // Friend class

    public: //Methods

    // Constructors and destructor

        /**
        * Static constructor
        */
        static CEventMediatorServer* NewL(void);

        /**
        * Static constructor
        */
        static CEventMediatorServer* NewLC(void);

        /**
        * Destructor
        */      
        ~CEventMediatorServer(void);

        /**
        * Creates a new session.
        */
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    // Other methods
        
        /**
         * Reports new envent to the eventmediator server.
         * @param aType: type of the event.
         * @param aSpec: additional info on event.
         * @param aData: a descriptor containing event data.
         */
        void ReportEventL(const TEventType aType, TDesC8* aSpec,
                          TDesC8* aData, TInt aStatus = KErrNone);

        /**
         * Completes the last message in error case.
         * @param aError: Cause of the call of this function.
         */
        void CompleteLastMessage(TInt aError);

        /**
         * Finds out whether the specified client thread is a SIT
         * thread or not
         */
        TBool IsClientTheSitL(const RMessage2& aMessage);

        /**
         * Starts the SIT thread if it is not running
         */
        void MakeSureSitIsRunningL();

        /**
         * Saves a pointer to the listening request that represents the
         * task arrival observation request
         */
        void SetTaskArrivalListenerL(CListenerContainer* aListener);

        /**
         * Clears the pointer to the listening request that represents the
         * task arrival observation request
         */
        void ClearTaskArrivalListener();

        /**
         * Returns the pointer to the task arrival observation request
         */
        CListenerContainer* TaskArrivalListener();

        /**
         * Notifies the server about a new task request that has arrived
         */
        void TaskRequestArrivedL(CListenerContainer* aTaskRequest);

        /**
         * Notifies the server about a new task arrival
         * observation request that has arrived from the SIT
         */
        void TaskArrivalObservationRequestArrivedL();

        /**
         * Notifies the server about a new task request event
         * specification fetching request that has arrived from
         * the SIT.
         */
        void TaskRequestEventSpecFetchingRequestArrivedL(CListenerContainer* aFetchingRequest);

        /**
         * Notifies the server about a new task cancellation
         * observation request that has arrived from the SIT.
         */
        void TaskCancellationObservationRequestArrivedL(CListenerContainer* aRequest);

        /**
         * Completes a pending task arrival observation
         * request when a new task request has arrived.
         */
        void CompleteTaskArrivalObservationRequestL(TEventType aEventType, TDesC8* aSpec);

        /**
         * Completes a pending task request event
         * specification fetching request.
         */
        void CompleteTaskRequestEventSpecFetchingRequestL(TInt aStatus, TDesC8* aEventSpec,
                                                          TDesC8* aTaskRequestEventSpec);

        /**
         * Tries to find a task request that is not yet
         * being fulfilled by the SIT.
         */
        CListenerContainer* FindWaitingTaskRequest();
        
        /**
         * Retrieves the event listener object, if any, that has the
         * specified event type and event specification ID.
         */
        CListenerContainer* FindListener(TEventType aEventType, TInt aEventSpecId);
        
        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that has the specified thread ID and is fulfilling
         * the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, const TDesC8* aEventSpec);

        /**
         * Completes the listener that matches the specified search
         * criteria.
         */
        void CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus);

        /**
         * Completes the listener that matches the specified search
         * criteria.
         */
        void CompleteListener(CListenerContainer* aListenerPtr, TInt aStatus);

        /**
         * Returns the number of "normal" (non-sit)
         * sessions that are present
         */
        TInt NormalSessionCount();

        /**
         * Completes all task requests with the specified status
         */
        void CompleteTaskRequests(TInt aStatus);
        
        /**
         * Returns a new event specification ID.
         */
        TInt NewEventSpecId();

        TPtrC EventLogFileName(void);
        
   public: // From MSitDeathListener
        void SitDied();

   protected:
        /**
         * Process any errors
         * 
         * @param aError the leave code reported
         */
        TInt RunError(TInt aError);

    private:    // Methods

    // Constructors and destructor

        /**
         * Default constructor
         */
        CEventMediatorServer(void);

        /**
         * Perform the second phase construction of a CVpnManagerServer
         * object.
         * @param aServer Pointer to the server itself.
         */
        void ConstructL(CEventMediatorServer* aServer);

    // Other methods

        /**
         * Stops the server if the session count is zero.
         */
        void StopEventMediator(void);

        /**
         * Copies data describing an event to the client.
         * @param aMessge: a message from client side sent by RConnection::FetchData
         * @return error code, KErrNone if successfull
         */
        TInt CopyEventDataL(const RMessage2& aMessage);    

        /**
         * Reduces listener count of a stored event by one and destroys it if count becomes zero.
         * @param aIndex: the index of the stored event in the list.
         */
        void MarkStoredEventListened(TInt aIndex);

        /**
         * Sets the iShuttingDown flag.
         */
        void SetShuttingDown(TBool aShuttingDown);

    private: //Data
        // List of Events that have been reported but not handled by all listeners.
        CArrayFixFlat<TEventContainer*> iStoredEvents;
        // Number of currently existing sessions.
        TInt iSessionCount;
        // Log writer
        CEventLogger* iLogger;
        // The next event specification ID
        // (used with SIT event listening requests)
        TInt iNextEventSpecId;
        // The single SIT instance
        CSit* iSit;
        // The single task arrival listener
        CListenerContainer* iTaskArrivalListener;

        // A flag that is set to true when
        // the server starts going down
        TBool iShuttingDown;
        
        TFileName iEventLogFileName;

        static const TUint iRangeCount;
        static const TInt iRanges[];
        static const TUint8 iElementIndex[];

        static const CPolicyServer::TPolicyElement iElements[];
        static const CPolicyServer::TPolicy iPolicy;

    };

// CLASS DECLARATION

/**
* Defines the session to the VPN manager server. 
*/
class CEventMediatorSession : public CSession2
    {
    public: //Message types
        enum
            {
            KEventMediatorListen,
            KEventMediatorListenWithSpec,
            KEventMediatorCancel,
            KEventMediatorCancelWithSpec,
            KEventMediatorCancelAll,
            KEventMediatorReportEvent,
            KEventMediatorReportEventWithSpec,
            KEventMediatorFetchData,
            KEventMediatorReportLogEvent,
            KEventMediatorNewEventSpecId,
            KEventMediatorDeletePrivateFiles,
            KEventMediatorGetEventLogSize,
            KEventMediatorGetEventLogHeader,
            KEventMediatorGetEventLogData,
            KEventMediatorClearEventLog
            };
    
    public: // Methods

    // Constructors and destructor

        /**
        * Static constructor
        */
        static CEventMediatorSession* NewL(CEventMediatorServer* aServer, const RMessage2& aMessage);
        
        /**
        * Default destructor
        */
        ~CEventMediatorSession(void);

        /**
        * Wrapper function which Dispatches and executes the client's service calls
        * (See Message type definition).
        */
        void ServiceL(const RMessage2& aMessage);

        /**
         * Checks if client is listening events of the given type and completes message 
         * sent by clients ListenToEvent function. The length of the event data and
         * the pointer to that data are written to the message.
         * @param aType: type of the event.
         * @param aSpec: additional info on event.
         * @param aData: event data.
         * @return number of listeners for the event was listened.
         */
        TInt CheckEventL(const TEventType aType, const TDesC8* aSpec, const TDesC8* aData, TInt aStatus);

        /**
         * Tries to find a task request that is not being fulfilled
         * already
         */
        CListenerContainer* FindWaitingTaskRequest();

        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that is fulfilling the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, TInt aEventSpecId);
        
        /**
         * Retrieves the event listener object, if any, that is using
         * the SIT that is fulfilling the specified event.
         */
        CListenerContainer* FindListener(TEventType aEventType, const TDesC8* aEventSpec);

        /**
         * Completes the specified listener if it is owned by the
         * session. Returns ETrue is the listener was found and
         * completed, EFalse otherwise.
         */
        void CompleteListener(TEventType aEventType, const TDesC8* aEventSpec, TInt aStatus);

        void CompleteTaskRequests(TInt aStatus);

        TBool IsASitSession();
        
    private: //Methods
    // Constructors and destructor

        /**
        * Constructor
        */
        CEventMediatorSession(CEventMediatorServer* aServer);

        /**
         * Starts listening events of requesteed type by coping the message to iListenedEvents.
         * @param aMessage: aMessage sent by clients ListenToEvent function.
         * @return: error code, KErrNone if successfull.
         */
        TInt ListenToEventWithSpecL(const RMessage2& aMessage);

        /**
         * Starts listening events of requesteed type by coping the message to iListenedEvents.
         * @param aMessage: aMessage sent by clients ListenToEvent function.
         * @return: error code, KErrNone if successfull.
         */
        TInt ListenToEventL(const RMessage2& aMessage);

        /**
         * Reports the event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportEventL(const RMessage2& aMessage);

        /**
         * Reports a log event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportLogEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportLogEventL(const RMessage2& aMessage);

        /**
         * Reports the event contained in the message to the event mediator server.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return: error code, KErrNone if successfull.
         */
        void ReportEventWithSpecL(const RMessage2& aMessage);

        /**
         * Cancels listning of one event type.
         * @param aMessage: aMessage sent by clients CancelListenToEvent function.
         */
        void CancelListening(const RMessage2& aMessage);

        /**
         * Cancels listning of one event type.
         * @param aMessage: aMessage sent by clients CancelListenToEvent function.
         */
        void CancelListeningWithSpecL(const RMessage2& aMessage);

        /**
         * Cancels all listening.
         * @param aMessage: aMessage sent by clients CancelAllListening function.
         */
        void CancelAll();

        /**
         * Copies event data from the server to the client.
         * @param aMessage: aMessage sent by clients FetchData function.
         * @return: error code, KErrNone if successfull.
         */
        TInt FetchDataL(const RMessage2& aMessage);

        /**
         * Looks up a message from iListenedEvents.
         * @param aType: type of the event the message is listening to.
         * @param aIndex: the position of the message in iListenedEvents.
         * @return ETrue if message exists, EFalse if not.
         */
        TBool FindListenerMsg(const TEventType aType,TInt& index);

        /**
         * Looks up a message from iListenedEvents.
         * @param aType: type of the event the message is listening to.
         * @param aIndex: the position of the message in iListenedEvents.
         * @return ETrue if message exists, EFalse if not.
         */
        TBool FindListenerMsg(const TEventType aType,const TDesC8* aSpec,TInt& index);

        /**
         * Searches for a task request type event listening request
         */
        TBool FindTaskRequestListenerMsg(TInt& index);
        
        /**
         * Reads data that was reported with the event from client. Allocates a buffer
         * for data and gives ownership of that buffer to caller.
         * @param aMessage: aMessage sent by clients ReportEvent function.
         * @return pointer to the newly created buffer containig data.
         */
        HBufC8* ReadEventDataFromClientL(const RMessage2& aMessage);

        /**
         * Reads specification describing the event from client. Allocates a buffer
         * for secification and gives ownership of that buffer to caller.
         * @param aMessage: aMessage sent with event specification.
         * @return pointer to the newly created buffer containig data.
         */
        HBufC8* ReadSpecificationFromClientL(const RMessage2& aMessage);

        /**
         * Completes the specified listener and deletes it from the
         * listener list
         */
        void CompleteListener(TInt aIndex, TInt aStatus);

        /**
         * Returns to the client a new event specification ID
         */
        void NewEventSpecIdL(const RMessage2& aMessage);

        TInt DeletePrivateFiles();
        void DeletePrivateFilesL();
        
        TInt GetEventLogSize(const RMessage2& aMessage);
        TInt GetEventLogHeader(const RMessage2& aMessage);
        TInt GetEventLogData(const RMessage2& aMessage);
        TInt ClearEventLog();
        
    private: //Data
        // List of messages listening to events
        CArrayFixFlat<CListenerContainer*> iListenedEvents;
        // Event mediator server
        CEventMediatorServer* iServer;

        RFs     iFs;
        RFile   iEventLogFile;
        TBool   iEventLogFileOpen;
        // Is this session from a SIT or not

    public:        
        TBool iIsSitSession;
    };

// CLASS DECLARATION

/**
* Container for one reported event from witch it can be read by all listening sessions. 
*/
class TEventContainer
    {
    public: //Methods
    // Constructors and destructor

        /**
        * Inline constructor
        */
        inline TEventContainer(TInt aCount, TDesC8* aData) : iData(aData), iListenerCount(aCount){};

    public: //Data
        // Data descriptor reported with the event.
        TDesC8*     iData;
        // number of listeners that need this event
        TInt        iListenerCount;
    };


// CLASS DECLARATION

/**
* Container for one reported event from which it can be read by all listening sessions
*/
class CListenerContainer : public CBase
    {
    public: //Methods
    // Constructors and destructor
        CListenerContainer(const RMessage2& aMessage, TDesC8* aSpec,
                           CEventMediatorServer* aServer);
        ~CListenerContainer();

        inline TEventType Type() {return (TEventType) iMessage.Int0();}
        inline RMessage2& Message() {return iMessage;}
        inline TDesC8* Specification() {return iSpec;}
        void Complete(TInt status);

        void AnalyzeRequestL();
        TBool WaitingForFulfilling();
        void MarkAsBeingFulfilled();
        TBool BeingFulfilled();

        TBool HandlesEvent(TEventType aEventType, const TDesC8* aEventSpec);

    private: //Data
        // Data descriptor reported with the event.
        TDesC8*     iSpec;
        RMessage2   iMessage;
        TEventType  iEventType;
        // Pointer to the event mediator server
        CEventMediatorServer* iServer;
        // A flag that tells whether the request
        // is being fulfilled by the SIT or not
        TBool iBeingFulfilledBySit;
    };

#endif
