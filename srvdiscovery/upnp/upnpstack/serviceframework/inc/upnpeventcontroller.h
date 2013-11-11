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
* Description:  Encapsulates http client for eventing
*
*/



#ifndef C_CUPNPEVENTCONTROLLER_H
#define C_CUPNPEVENTCONTROLLER_H

// INCLUDES
#include <e32cmn.h>
#include <e32std.h>
#include <e32base.h>

#include "upnphttpclientengine.h"
#include "upnphttptransaction.h"
#include "upnpsubscriberlibraryelement.h"
#include "upnpserviceimplementation.h"
#include "upnpsubscriberlibraryobserver.h"


// FORWARD DECLARATION
class CUpnpEventQueueManagerBase;
class CUpnpStateVariable;
class CUpnpSubscriberLibrary;

_LIT8(KGenaPropertyStart, 
    "<e:property><");
_LIT8(KGenaPropertyEnd,
    "></e:property>");
_LIT8(KGenaPropertysetEnd,
    "</e:propertyset>");
_LIT8(KOpenBracket, "<");
_LIT8(KCloseBracket, ">");
_LIT8(KGenaXml,
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
_LIT8(KGenaPropertysetStart,
    "<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">"); 
_LIT8( KYes, "yes" );


static const TInt KMessageTimeout = 5000000;//5 sec
const TInt KMaxEventRate = 5000000;


// CLASS DECLARATION
/**
*  An interface to Subscriber Library.
*  
*  @since Series60 2.6
*/
class MTransactionObserver
    {

public:
    /**
    * A virtual function to be implemented in derived classes.
    * It is called after completed transaction
    * @param aComletedTransaction just completed transaction
    * @since Series60 3.2
    */
    virtual void TransactionCompletedL(  CUpnpHttpTransaction* aCompletedTranscation  ) = 0;

    };



/**
*  This class handles event releated issues
*  
*
*  @since Series60 3.2
*/
NONSHARABLE_CLASS( CUpnpEventController ): public CBase, 
                                           public MUpnpHttpClientObserver,
                                           public MUpnpSubscriberLibraryObserver
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpEventController();

    /**
     * Two-phased constructor.
     */
    static CUpnpEventController* NewL( CUpnpServiceImplementation& aServiceImpl );

    /**
     * Two-phased constructor.
     */
    static CUpnpEventController* NewLC( CUpnpServiceImplementation& aServiceImpl );

public:
    /**
    * Sends inital event
    */
    void SendInitialNotification( );
    
    /**
    * Sends non-modereted event
    */
    void SendNonModeratedNotification( );
    
    /**
    * Sends moderated event
    */    
    void SendModeratedNotification();
    
    /**
    * Sends message using httpclient
    * @param aTransaction encapsulated httpmessage with event 
    */    
    void SendTransactionL( CUpnpHttpTransaction* aTranscation );
    
    /**
    * Handle subscribe/resubscribe messages
    * @param aMessage http request
    */    
    void SubscribeL( CUpnpGenaMessage* aMessage );

    /**
    * Handle unsubscribe messages
    * @param aMessage http request
    */    
    void UnSubscribeL( CUpnpGenaMessage* aMessage );
    
    /**
    * Handle unsubscribe messages
    * @return TInt number of subscribers
    */    
    TInt SubscribersAmount();
    

/*from MUpnpHttpClientObserver*/
public:
    /**
    * Imlements completed transaction handling
    * @param comleted transaction
    */    
    void ClientResponseRecivedLD( CUpnpHttpTransaction& aEndedTransaction );
    
/*from MUpnpSubscriberLibraryObserver*/
public:
    
    /**
    * 
    * @return all evented state variables
    */    
    RPointerArray<CUpnpStateVariable>& EventedStateVariables();
    
    /**
    * Resume processing after removing subscriber 
    * @param aSubscriber just removed subscriber
    * @param removed subscriber position in SubscriberLibrary
    */    
    void SubscriberRemoved( CUpnpSubscriberLibraryElement* aSubscriber ,TInt aPos );

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpEventController( CUpnpServiceImplementation& aServiceImpl );

    /**
     * Constructor for performing 2nd stage construction
     */
    void ConstructL();


private:
    /*
    * Cancel all pending transaction related wih particular subscriber
    * @param aSid SID uniqe subscriber id
    */
    void CancelInvalidSessions( const TDesC8& aSid );

    /*
    * Checks if unsubscribe message has all header
    * @param aMessage message to be checked
    * return validation status
    */   
    TUpnpErrorCode CheckHeaders( CUpnpGenaMessage* aMessage );
    
    /*
    * Generates a SID number for given, new subscriber
    * @param aSubscriber new subscriber
    */   
    void GenerateSidL( CUpnpSubscriberLibraryElement* aSubscriber );
    
    /*
    * Creates timeout header
    * @param aTimeout timeout value
    * @return header descriptor "Second-'aTimeout'"
    */   
    HBufC8* TimerHeaderLC( const TDesC8& aTimeout );
    
private:
    
    /** Http engine*/
    CUpnpHttpClientEngine* iHttpEngine;                         //owned
    
    /** Store all pending transaction*/
    RPointerArray<CUpnpHttpTransaction> iPendingTransactions;   //owned
    
    
    CUpnpServiceImplementation& iServiceImpl;                   //not owned
    
    /**Non-moderated event queue manager */
    CUpnpEventQueueManagerBase* iNonModQueue;                   //owned
    
    /**Moderated event queue manager */
    CUpnpEventQueueManagerBase* iModQueue;                      //owned
    
    /**Initial event queue manager */
    CUpnpEventQueueManagerBase* iInitialEventQueue;             //owned
    
    /**Store information about subscribers*/
    CUpnpSubscriberLibrary* iSubscriberLibrary;                 //owned
    };

#endif // C_CUPNPEVENTCONTROLLER_H
//end of file