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


#ifndef C_CUPNPINITIALEVENTQUEUEMANAGER_H
#define C_CUPNPINITIALEVENTQUEUEMANAGER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "upnphttptransaction.h"
#include "upnpserviceimplementation.h"
#include "upnpeventcontroller.h"
#include "upnpeventqueuemanagerbase.h"
#include "upnpsubscriberlibrary.h"


// CLASS DECLARATION

/**
*  Implements algoritms for serving nonmoderated events
* 
*/
NONSHARABLE_CLASS( CUpnpInitialEventQueueManager ): public CUpnpEventQueueManagerBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpInitialEventQueueManager();

    /**
     * Two-phased constructor.
     */
    static CUpnpInitialEventQueueManager* NewL( CUpnpServiceImplementation& aServiceImpl,
                                                     CUpnpEventController& aEventController,
                                                     CUpnpSubscriberLibrary* aSubscriberLibrary );

    /**
     * Two-phased constructor.
     */
    static CUpnpInitialEventQueueManager* NewLC( CUpnpServiceImplementation& aServiceImpl,
                                                      CUpnpEventController& aEventController,
                                                      CUpnpSubscriberLibrary* aSubscriberLibrary );
    
   

private:    
    
    /*algoritm implemetation*/
    void SendEvent();
        
    /**
    * algoritm implemetation
    * @param aEvent buffer for message body
    * @param aVariableList list of evented variables
    * @return number of evented variables
    */
    TInt CUpnpInitialEventQueueManager::FillEventBodyL( 
                                             TDes8& aEvent, 
                                             const RPointerArray<CUpnpStateVariable>& aVariableList );
    /**
    * Creates event message
    * @param aMessage will be encapsulete into http transaction
    * @return Http transcaction with event message
    */
    CUpnpHttpTransaction* CreateTransactionL( CUpnpHttpMessage* aMessage );                                                      
    
    /**
    * 
    */
    void SendEventL();

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpInitialEventQueueManager( CUpnpServiceImplementation& iServiceImpl,
                                        CUpnpEventController& iEventController,
                                        CUpnpSubscriberLibrary* aSubscriberLibrary );
    
    /**
    * Constructor for performing 2st stage construction
    */
    void ConstructL();

private:
    
    };

#endif// C_CUPNPINITIALEVENTQUEUEMANAGER_H
//end of file
