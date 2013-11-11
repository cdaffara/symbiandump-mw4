/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the CUpnpHttpMessageSender class
 *
*/


#ifndef __UPNPHTTPMESSAGESENDER_H__
#define __UPNPHTTPMESSAGESENDER_H__

#include <http.h>
#include <upnpnotifytimer.h>

class MUpnpHttpMessageSenderOwner;
class CUpnpHttpTransaction;

/**
 *  Class that manage sending process of a single transaction (
 *  request http message and response http message).
 *
 */
NONSHARABLE_CLASS( CUpnpHttpMessageSender ) : public CBase,
                                              public MHTTPTransactionCallback,
                                              public MHTTPDataSupplier,
                                              public MUpnpNotifyTimerObserver
    {
public:
    /**
     * Two-phased constructor. Create a CUpnpHttpMessageSender object.
     * @param aUpnpTransaction unsent upnp transaction object
     * @param aSession opened http session that will be used to send the message
     * @param aOwner object that should be notified when sender finish its job
     *                  and issue destruction of this (sender) object
     * @return A pointer to the created instance of CUpnpHttpMessageSender
     */
    static CUpnpHttpMessageSender* NewL( CUpnpHttpTransaction& aUpnpTransaction,
                                         RHTTPSession aSession,
                                         MUpnpHttpMessageSenderOwner& aOwner
                                         );

    /**
     * Two-phased constructor. Create a CUpnpHttpMessageSender object.
     * @param aUpnpTransaction unsent upnp transaction object
     * @param aSession opened http session that will be used to send the message
     * @param aOwner object that should be notified when sender finish its job
     *                  and issue destruction of this (sender) object
     * @return A pointer to the created instance of CUpnpHttpMessageSender
     */
    static CUpnpHttpMessageSender* NewLC( CUpnpHttpTransaction& aUpnpTransaction,
                                          RHTTPSession aSession,
                                          MUpnpHttpMessageSenderOwner& aOwner
                                          );

    /**
     * Destructor.
     */
    ~CUpnpHttpMessageSender();

    /**
     * Start a new HTTP transaction maintained by the object. It will start
     * asynchronous sending of request message.
     */
    void StartTransactionL();

    /**
     * Returns upnp transaction object that is maintained by the object.
     * @return upnp transaction object that is maintained by the object
     */
    CUpnpHttpTransaction& UpnpTransaction();
    
    
    void CancelTransaction();

private:    //from MHTTPTransactionCallback
    /**
     * Called by framework to notify about transaction events.
     * @param aTransaction: Transaction, where the event occured.
     * @param aEvent:       Occured event.
     */
    virtual void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    /**
     * Called by framework when *leave* occurs in handling of transaction event.
     * @param aError:       The leave code that occured.
     * @param aTransaction: The transaction that was being processed when leave occured.
     * @param aEvent:       The event that was being processed when leave occured.
     * @return KErrNone,    if the error was handled. Otherwise the value of aError, or
     *                      some other error value. Returning error value causes causes
     *                      HTTP-CORE 6 panic.
     */
    virtual TInt MHFRunError( TInt aError,
            RHTTPTransaction aTransaction,
            const THTTPEvent& aEvent );

private:    //from MHTTPDataSupplier
    /*
     * Methods from MHTTPDataSupplier are used to supply body of request from
     * our UpnpMessage to Symian RHTTPRequest
     */
    virtual TBool GetNextDataPart( TPtrC8& aDataChunk );

    virtual void ReleaseData();

    virtual TInt OverallDataSize();

    virtual TInt Reset();

private:    //from MNotifyTimerObserver
    /**
     * From MNotifyTimerObserver Function which indicate that message is timeout.
     * @since Series ?XX ?SeriesXX_version
     * @param aTimer Timer.
     */
    virtual void TimerEventL( CUpnpNotifyTimer* aTimer );

private:
    /**
     * Constructor
     * @param aUpnpTransaction unsent upnp transaction object
     * @param aSession opened http session that will be used to send the message
     * @param aOwner object that should be notified when sender finish its job
     *                  and issue destruction of this (sender) object
     */
    CUpnpHttpMessageSender( CUpnpHttpTransaction& aUpnpTransaction,
                            RHTTPSession aSession,
                            MUpnpHttpMessageSenderOwner& aOwner
                            );
                                                        
    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Opens RHTTPTransaction with request method and uri, subscribe
     * for transaction's events
     */
    void PrepareRequestTransactionL();

    /**
     * Sets http headers from UpnpHttpTransaction to RHTTPTransaction
     */
    void PrepareRequestHeadersL();

    /**
     * Copies http headers from RHTTPTransaction to UpnpHttpTransaction
     */
    void CopyResponseHeadersL();

    /**
     * Sets http message body from UpnpHttpTransaction to RHTTPTransaction
     */
    void PrepareRequestBody();

    /**
     * Starts timer of request sending with TcpTimeout value.
     */
    void StartRequestTimer();

    /**
     * Called when transaction succeeded.
     * Closes RHTTPTransaction, and forward result message to observer.
     */
    void TransactionSucceeded();

    /**
     * Do NOT use this method directly, but via TransactionFailed
     */
    void DoTransactionFailedL( TInt aStatus, TInt aError );
    
    /**
     * Do NOT use this method directly, but via TransactionSucceded
     */
    void DoTransactionSucceededL();
    
    /**
     * Called in case of every fail of transaction.
     * Closes RHTTPTransaction, creates error message (408 timeout), and forward it
     * to observer.
     */
    void TransactionFailed( TInt aStatus, TInt aError = 0 );

    /**
     * Called when transaction got another part of body data.
     * Data is concatenated to iBody and when the last part of body is received
     * request message is created witin transaction.
     */
    void GotResponseBodyDataL( MHTTPDataSupplier& aResponseBodySupplier );

    /**
     * Helper method that allocate descriptor object with destination uri of message
     * passed as a parameter.
     */
    static HBufC8* DestinationUriL( CUpnpHttpMessage* aMessage );

private:
    /**
     * Represents UPnP transaction
     */
    CUpnpHttpTransaction& iUpnpTransaction;

    /**
     * Timer of transaction
     */
    CUpnpNotifyTimer* iTimer;

    /**
     * Represents HTTP Transaction used to realize UPnP transaction
     */
    RHTTPTransaction iTransaction;

    /**
     * Error number to be sent if transaction failed
     */    
    TInt iCurrentErrorNumber;
    
    /**
     * Represents HTTP Session object
     */
    RHTTPSession iSession;

    /**
     * Body of received message
     */
    HBufC8* iBody;

    /*
     * Owner of an instance.
     */
    MUpnpHttpMessageSenderOwner& iOwner;

    };

#endif /*__UPNPHTTPMESSAGESENDER_H__*/

//  End of File

