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
 * Description:  Declaration of CUpnpSymbianServerBase
 *
 */

#ifndef CUPNPSYMBIANSERVERBASE_H
#define CUPNPSYMBIANSERVERBASE_H

#include <e32base.h>
#include "upnpnotifytimer.h"

class CUpnpSymbianServerBase;

typedef CUpnpSymbianServerBase* (*TServerFactoryMethodLC)();

/**
 * Base class for all symbian server classes used in upnp stack project
 * This implementation provides:
 * - proper server starting,
 * - 2 seconds stoping timeout,
 * - stopping server if ( no connected clients && CanBeStopped() )
 *   @see CanBeStopped()
 *   @see SuggestShutdown()
 * - session connection version checking,
 * - basic panics utility functions,
 * - basic errors handling
 * Note that it should be used with client part implementation that use
 * RUpnpSessionBase class.
 *
 *  @lib upnpipserversutils
 */
class CUpnpSymbianServerBase : public CServer2, public MUpnpNotifyTimerObserver
    {
public:
    enum TUpnpSymbianServerPanics
        {
        ECreateTrapCleanup,
        EBadRequest,
        EBadDescriptor,
        EFirstUnusedPanicNumber //this should be always last enum as this is
        //used by derived server to start their panic enums
        };

public:
    /**
     * Fully initialise thread, and start the server.
     * Method is intended to be called from E32Main method without any earlier
     * initialisation.
     * @param aThreadName name to which thread will be renamed.
     * @param aServerFactoryMethodLC two phase constructor of server object that
     *          leaves it on cleanup stack.
     * @return error code
     */
    IMPORT_C static TInt StartServer( const TDesC& aThreadName,
            TServerFactoryMethodLC aServerFactoryMethodLC );

    /**
     * Destructor
     */
    IMPORT_C ~CUpnpSymbianServerBase();

    /**
     * Panic the client.
     * @param aMessage the message channel to the client.
     * @param aReason the reason code for the panic.
     */
    IMPORT_C void PanicClient( const RMessage2& aMessage, TInt aReason ) const;

protected:
    /**
     * Constructor.
     */
    IMPORT_C CUpnpSymbianServerBase();

    /**
     * Second phase base constructor. It must be called by derived class.
     */
    IMPORT_C void BaseConstructL();

    /**
     * Panic the server.
     * @param aPanic the panic code.
     */
    IMPORT_C void PanicServer( TInt aPanic ) const;

private:
    /**
     * Returns new CSession2 object.
     * This method don't have to bother about version checking
     * as this is done by implementation of
     * CUpnpSymbianServerBase::NewSessionL
     *      (const TVersion& aVersion, const RMessage2& aMessage)
     * that is wrapping this method.
     */
    virtual CSession2* NewSessionL( const RMessage2& aMessage ) const = 0;

    /**
     * Returns version that is supported by this server.
     */
    virtual TVersion SupportedVersion() const = 0;

    /**
     * Return name of a server.
     * @return name of a server.
     */
    virtual const TDesC& ServerName() const = 0;

    /**
     * Derived classes can implement in the method additional condidions
     * of stopping server process that will be checked in SuggestShutdown
     * method and befor actual server stopping. Default implementation
     * provides no additional condidions.
     *
     * @return ETrue if server proccess can be stopped at the moment
     */
    IMPORT_C virtual TBool CanBeStopped() const;

protected:
    /**
     * Method should be called when derived class wants to suggest
     * closing of the media server. This method checks every stop
     * condidion and starts closing server timeout if all of them are
     * fulfilled.
     */
    IMPORT_C void SuggestShutdown();

protected:
    //from MUpnpNotifyTimerObserver
    /**
     * Invoked by shutdown timer when event TimerEventL occured.
     */
    IMPORT_C virtual void TimerEventL( CUpnpNotifyTimer* aTimer );

protected:
    //from CServer2
    /**
     * This method wraps pure virtual NewSessionL(const RMessage2&)
     * and leaves with KErrNotSupported if aVersion is different
     * from version provided by pure virtual SupportedVersion.
     */
    IMPORT_C CSession2* NewSessionL(
            const TVersion& aVersion, const RMessage2& aMessage ) const;

protected:
    //from CActive

    /**
     * Invoked by active object framework to handle ending events
     */
    IMPORT_C void RunL();

    /**
     * Process any errors.
     * @param aError the leave code reported.
     * @result return KErrNone if leave is handled.
     */
    IMPORT_C TInt RunError( TInt aError );

private:
    /**
     * Internal method that initialises thread, and starts the server.
     * Precondition is that cleanup stack is correctly initialised
     * and method is called from within a TRAP.
     * @param aThreadName name to which thread will be renamed.
     * @param aServerFactoryMethodLC two phase constructor of server object that
     *          leaves it on cleanup stack.
     */
    static void StartServerL( const TDesC& aThreadName,
        TServerFactoryMethodLC aServerFactoryMethodLC );

    /**
     * Increment the count of the active sessions for this server.
     */
    void IncrementSessionCount();

    /**
     * Decrement the count of the active sessions for this server.
     */
    void DecrementSessionCount();

private:
    /** @var iSessionCount the number of session owned by this server */
    TInt iSessionCount;

    /** shutdown timer */
    CUpnpNotifyTimer* iShutdownTimer;
    };

#endif // CUPNPSYMBIANSERVERBASE_H
