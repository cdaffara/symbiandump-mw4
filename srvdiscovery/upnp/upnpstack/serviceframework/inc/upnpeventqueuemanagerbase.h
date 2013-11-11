/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Base class for event queue manager
*
*/


#ifndef C_CUPNPEVENTQUEUEMANAGERBASE_H
#define C_CUPNPEVENTQUEUEMANAGERBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "upnphttptransaction.h"
#include "upnpserviceimplementation.h"
#include "upnpeventcontroller.h"
#include "upnpsubscriberlibrary.h"


const TInt KFirstSubscriberInQueue(0);


// CLASS DECLARATION
/**
 *  UpnpNonModeratedEventQueueManager
 * 
 */
NONSHARABLE_CLASS( CUpnpEventQueueManagerBase ): public CBase, public MTransactionObserver
    {
public:
    // Constructors and destructor

    /**
    * Destructor.
    */
    ~CUpnpEventQueueManagerBase();

    /**
    * Two-phased constructor
    * @param aServiceImpl
    * @param aEventController 
    * $param aSubscriberLibrary 
    */
    static CUpnpEventQueueManagerBase * NewL( CUpnpServiceImplementation& aServiceImpl,
                                              CUpnpEventController& aEventController,
                                              CUpnpSubscriberLibrary* aSubscriberLibrary );

    /**
    * Two-phased constructor.
    * @param aServiceImpl
    * @param aEventController 
    * $param aSubscriberLibrary 
    */
    static CUpnpEventQueueManagerBase * NewLC( CUpnpServiceImplementation& aServiceImpl,
                                               CUpnpEventController& aEventController,
                                               CUpnpSubscriberLibrary* aSubscriberLibrary );
    
public:
    /*
    * Trigers event sending,
    * has to be implemented in delivered class
    */
    virtual void SendEvent() = 0;
    
    void UpdateQueue( TInt aPos );
    
    
/** from MTransactionObserver*/
public:  
 
    void TransactionCompletedL(  CUpnpHttpTransaction* aCompletedTranscation  );
    
    
protected:    

    /*
    * Finishes processing of the event sending queue
    */
    void SendingCompleted();
    
    /*
    * Sends the event to the next subscriber
    */
    void SendToNextL();
    
    /*
    * Prepares event message and sends it to given subscriber
    * @param aSubscriberNo subscriber's position on SubscribersLibrary list
    */
    void PrepareTransactionAndStartSendingL( TInt aSubscriberNo );

    /*
    * Removes subscriber form the SubscriberLibrary,
    * recognize it by SID releated with event message
    * @param aCompletedTransaction unsuccesfully completed transaction
    */
    void RemoveInvalidSubscriber( CUpnpHttpTransaction* aCompletedTransacation );
                                  
    /*
    * Creates xml message with all evented variables
    * @aparam aVariableList list with evented variables
    */
    HBufC8* CreateEventL( const RPointerArray<CUpnpStateVariable>& aVariableList );
       
    /*
    * Adds single variable to message body
    * @param aBufer message is appended to given buffer
    * @param aVariable variable which will be added to buffer
    */
    void AddVariableToEventBody( TDes8& aBuffer , CUpnpStateVariable * aVariable );
    
    /*
    * Clears evented variable list
    */
    void ClearVariables();
    
    /*
    * Creates event specyfic message,
    * has to be implemented in delivered class
    */
    virtual TInt FillEventBodyL ( TDes8& aEvent,     
                          const RPointerArray<CUpnpStateVariable>& aVariableList ) = 0;
    
    /*
    * Encapsulates event message into http transaction
    * has to be implemented in delivered class
    */                     
    virtual CUpnpHttpTransaction* CreateTransactionL( CUpnpHttpMessage* aMessage ) = 0;                         

protected:

    /**
    * Constructor for performing 1st stage construction
    * @param aServiceImpl
    * @param aEventController 
    * $param aSubscriberLibrary 
    */
    CUpnpEventQueueManagerBase ( CUpnpServiceImplementation& iServiceImpl,
                                 CUpnpEventController& iEventController,
                                 CUpnpSubscriberLibrary* aSubscriberLibrary );
    
    /**
    * Constructor for performing 2st stage construction
    */
    void BaseConstructL();

protected:
    
    /*Points the currently proccesed subscriber */
    TInt iCurrentSubscriberIndex;               //owned
    
    /*Current event message*/
    HBufC8* iBody;                              // owned
    
    /*Status flag */
    TBool iIsSending;
    
    /*Status flag*/
    TBool iNewValuesWaitingToBeSend;            //owned

    
    CUpnpServiceImplementation& iServiceImpl;   //not owned
    
    CUpnpEventController& iEventController;     //not owned
    
    CUpnpSubscriberLibrary* iSubscriberLibrary; //not owned
    };

#endif // C_CUPNPEVENTQUEUEMANAGERBASE_H
//end of file