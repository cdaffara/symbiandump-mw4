/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors link disconnection
*
*/


#ifndef C_DISCONNECTIONOBSERVER_H
#define C_DISCONNECTIONOBSERVER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CIkeConnectionInterface;

/**
 *  Disconnection observer callback interface.
 *  Callback interface which is used by CDisconnectionObserver object to notify
 *  about link disconnection.
 * 
 *  @lib internal (kmdserver.exe)
  */
class MDisconnectionObserverCallback
    {
public:        
    /**
     * Notifies about disconnection.
     * @param aStatus Completion status
     */
    virtual void DisconnectIndication( TInt aStatus ) = 0;
    };

/**
 *  Disconnection observer.
 *  Active object provides functionality for notifying link disconnection.
 *
 *  @lib internal (kmdserver.exe)
  */
class CDisconnectionObserver : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aIkeConnectionInterface IKE connection interface
     * @param aCallback Callback interface
     */
    static CDisconnectionObserver* NewL( CIkeConnectionInterface& aIkeConnectionInterface,
                                         MDisconnectionObserverCallback& aCallback );

    /**
     * Destructor.
     */
    ~CDisconnectionObserver();

    /**
     * Starts observing Link disconnection. Link disconnection is notified via
     * MDisconnectionObserverCallback interface.
     */
    void StartObserving();
    
private:
    
    CDisconnectionObserver( CIkeConnectionInterface& aIkeConnectionInterface,
                            MDisconnectionObserverCallback& aCallback );

// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous notification request.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of asynchronous notification request.
     */    
    void DoCancel();
        
private: // data
    
    /**
     * IKE connection interface.
     * Not own.
     */
    CIkeConnectionInterface&        iIkeConnectionInterface;
    
    /**
     * Callback interface.
     * Not own.
     */
    MDisconnectionObserverCallback& iCallback;
    };


#endif // C_DISCONNECTIONOBSERVER_H
