/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Custom plug-in implementations offer their own API documentation
*                where detailed characteristics about how methods declared in this
*                class are implemented.
*
*/








#ifndef SEN_SERVICE_DISPATCHER_H
#define SEN_SERVICE_DISPATCHER_H

#include <flogger.h>
#include <e32property.h>
#include <e32std.h>
#include "senlogger.h"
#include "sendebug.h"

typedef RSenPointerMap<TInt, TInt> RTransactionsMap;

// FORWARD DECLARATIONS
class CSenConnectionChunk;
class CSenAsyncOperation;
class CSenServiceConnectionImpl;
    
struct TMessage
{
    CSenConnectionChunk* iSenConnectionChunk;   // Not Owned
    CSenAsyncOperation* iSenAsyncOperation; // Not Owned
};
        
struct TThreadMessage
{
    TMessage iMessage;
    TInt iVrtalTrnsnID;
};

#ifdef EKA2
NONSHARABLE_CLASS(CSenServiceDispatcher) : public CBase
#else
class CSenServiceDispatcher : public CBase
#endif
    {
    private:
    protected:
    public:

        // Constructors and destructor:
        static CSenServiceDispatcher* NewL(   RSenServiceConnection&  aConnection,
                                                        TInt aConnectionID);

        static CSenServiceDispatcher* NewLC(  RSenServiceConnection&  aConnection,
                                                        TInt aConnectionID);


        static TInt DispatcherThreadL(CSenServiceDispatcher* aThis);

        static TInt ExecuteL(CSenServiceDispatcher* aThis);

        TInt GetDispactherThreadId();

        TInt AddToTheQueue(TThreadMessage aThreadMessage);

        TBool RemoveFromQueue(TInt aTransactionID);

        TInt AddToTheTransMap(TInt* pVrtlaTxnId,TInt* pActlTxnId);

        TInt RemoveFromTransMap(TInt* pVrtlaTxnId);

        TInt UpdateTransMap(TInt* pVrtlaTxnId,TInt* pActlTxnId);

        TInt GetActualTransactionID(TInt* pVrtlaTxnId);

        TInt GetVirtualTransactionID(TInt* pActlTxnId);

        void SetOwnerThreadId(TInt aOwnerThreadID);

        void ResetQueue();

        TMessage GetMessageFromQueue(TInt aTransactionID);
        
        virtual ~CSenServiceDispatcher();

    private:

        /**
        * C++ default constructor.
        */
        CSenServiceDispatcher(RSenServiceConnection&  aConnection);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TInt aConnectionID);

        RTransactionsMap& TransactionMap();
        
        void OpenDispatcherLogL();

        void CloseDispatcherLogL();
        
    private:

        RThread                             iDispatcherThread;
        
        //Message queue to dispacth messages in FIFO order.
        RArray<TThreadMessage>              iMessageQueue;
        TInt                                iMessagesQueueCounter;
        
        //Map of virtual and actual transaction ID's.
        RTransactionsMap*                   ipTransactionsMap;
        
        //Flag for dispatching messages. Stops dispatching messages
        //when set to FALSE.
        TBool                               iDispatchMessages;
        
        //Synchronizer critical section. Used to lock while dispatching
        //messages.
        RCriticalSection                    iCsSynchronizer;
        
        //Crtitical senction for message queue. 
        RCriticalSection                    iCsMessageQueue;
        
        //Crtitical senction for transaction map. 
        RCriticalSection                    iCsTransctnsMap;
        
        TThreadId                           iOwnerThreadId;
        RSenServiceConnection&              iConnection;    // not owned        
        TInt                                iConnectionID;

    };

#ifdef EKA2
NONSHARABLE_CLASS(CSenUnderTakerWaiter) : public CActive
#else
class CSenUnderTakerWaiter : public CActive
#endif
    {
    public:
        // Constructors and destructor
        static CSenUnderTakerWaiter* NewL(CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                                          TInt aDispatcherThreadID );
        static CSenUnderTakerWaiter* NewLC(CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                                           TInt aDispatcherThreadID);

        void StartWaiter();
        void StopWaiter();

        ~CSenUnderTakerWaiter();

    private:
        CSenUnderTakerWaiter();
        void ConstructL(CSenServiceConnectionImpl* aSenServiceConnectionImpl,
                        TInt iDispatcherThreadID );
        void RunL();
        void DoCancel();
    private: // Data
        CSenServiceConnectionImpl*          iSenServiceConnectionImpl;  // not owned        
        RUndertaker                         iUnderTaker;
        TInt                                iDyingThreadNumber ;
        TInt                                iDispatcherThreadID ;
    };

#endif  //SEN_SERVICE_DISPATCHER_H

// End of File
