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
* Description:  Implements algoritms for modereted events
*
*/


#ifndef C_UPNPMODERATEDEVENTQUEUEMANAGER_H
#define C_UPNPMODERATEDEVENTQUEUEMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "upnphttptransaction.h"
#include "upnpserviceimplementation.h"
#include "upnpeventcontroller.h"
#include "upnpeventqueuemanagerbase.h"

#include "upnpsubscriberlibrary.h"


class CUpnpNotifyTimer;
// CLASS DECLARATION

/**
*  Implements algoritms for serving moderated events
* 
*/
NONSHARABLE_CLASS( CUpnpModeratedEventQueueManager ): public CUpnpEventQueueManagerBase, 
                                                      public MUpnpNotifyTimerObserver
    {
public:
    // Constructors and destructor

    /**
    * Destructor.
    */
    ~CUpnpModeratedEventQueueManager();

    /**
    * Two-phased constructor.
    */
    static CUpnpModeratedEventQueueManager* NewL( CUpnpServiceImplementation& aServiceImpl,
                                                  CUpnpEventController& aEventController,
                                                  CUpnpSubscriberLibrary* aSubscriberLibrary );

    /**
    * Two-phased constructor.
    */
    static CUpnpModeratedEventQueueManager* NewLC( CUpnpServiceImplementation& aServiceImpl,
                                                   CUpnpEventController& aEventController,
                                                   CUpnpSubscriberLibrary* aSubscriberLibrary );
       
/*from MUpnpNotifyTimerObserver*/
public:
    /**
    * Handles expired messages
    */
    void TimerEventL( CUpnpNotifyTimer* aTimer );     
    

private:    
    /**
    * Dalays message sending
    */ 
    void WaitBeforeSending();

    /**
    * Initiates sending process
    */
    void StartSendingEventL();
    
private:
    
    /**
    * algoritm implemetation
    */
    void SendEvent();
    
    /**
    * algoritm implemetation
    * @param aEvent buffer for message body
    * @param aVariableList list of evented variables
    * @return number of evented variables
    */
    TInt FillEventBodyL( TDes8& aEvent, const RPointerArray<CUpnpStateVariable>& aVariableList );
    
    /**
    * Creates event message
    * @param aMessage will be encapsulete into http transaction
    * @return Http transcaction with event message
    */
    CUpnpHttpTransaction* CreateTransactionL( CUpnpHttpMessage* aMessage ); 
    
    
private:

    /**
    * Constructor for performing 1st stage construction
    */
    CUpnpModeratedEventQueueManager( CUpnpServiceImplementation& iServiceImpl,
                                     CUpnpEventController& iEventController,
                                     CUpnpSubscriberLibrary* aSubscriberLibrary );
    /**
    * Constructor for performing 2st stage construction
    */    
    void ConstructL();

private:
    
    /*Dalayes message sending*/
    CUpnpNotifyTimer* iModerationTimer; //owned
    
    };

#endif // C_UPNPMODERATEDEVENTQUEUEMANAGER_H
//end of file