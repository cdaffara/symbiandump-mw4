/** @file
 * Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implementation of CUpnpSymbianServerBase
 *
 */

#include <upnpsymbianserverbase.h>
#include "upnpuheapmark.h"
#include "upnpcustomlog.h"

#include <ecom/ecom.h>

const TInt KUpnpSymbianServerShutdownInterval = 2000000;

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::StartServerL
// Create and start the server.
// -----------------------------------------------------------------------------
//
void CUpnpSymbianServerBase::StartServerL( const TDesC& aThreadName,
    TServerFactoryMethodLC aServerFactoryMethodLC )
    {
    User::LeaveIfError( User::RenameThread( aThreadName ) );
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server
    CUpnpSymbianServerBase* serverObject = aServerFactoryMethodLC();
    LOGS( "UpnpSymbianServer *** Ready to accept connections" );

    RProcess::Rendezvous( KErrNone );

    // Start handling requests
    CActiveScheduler::Start();

    LOGS( "UpnpSymbianServer *** Active scheduler stopped" );
    LOGS( "UpnpSymbianServer *** Prepared for shutdown" );

    CleanupStack::PopAndDestroy( serverObject );
    REComSession::FinalClose();
    CleanupStack::PopAndDestroy( activeScheduler );
    LOGS( "UpnpSymbianServer *** Shutdown complete" );
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::StartServer
// Create and start the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSymbianServerBase::StartServer( const TDesC& aThreadName,
        TServerFactoryMethodLC aServerFactoryMethodLC )
    {
    __UPNP_UHEAP_MARK;

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !( cleanupStack ) )
        {
        User::Panic( aThreadName, ECreateTrapCleanup );
        }

    TRAPD( err, StartServerL( aThreadName, aServerFactoryMethodLC) );
    if ( err != KErrNone )
        {
        //there's no need to panic server
        //f.e. KErrAlreadyExist is valid in case of simultanous start
        }

    delete cleanupStack;
    cleanupStack = NULL;

    __UPNP_UHEAP_MARKEND;
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::CUpnpSymbianServerBase
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSymbianServerBase::CUpnpSymbianServerBase()
: CServer2( EPriorityNormal )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::~CUpnpSymbianServerBase
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSymbianServerBase::~CUpnpSymbianServerBase()
    {
    delete iShutdownTimer;
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::BaseConstructL
// Second phase base constructor. It must be called by derived class.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::BaseConstructL()
    {
    StartL( ServerName() );
    iShutdownTimer = CUpnpNotifyTimer::NewL( this );
    SuggestShutdown();
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::PanicClient(
        const RMessage2& aMessage, TInt aPanic ) const
    {
    LOG_FUNC_NAME;
    aMessage.Panic( ServerName(), aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::PanicServer
// Panic server.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::PanicServer( TInt aPanic ) const
    {
    User::Panic( ServerName(), aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::IncrementSessionCount
// Increment sessions.
// -----------------------------------------------------------------------------
//
void CUpnpSymbianServerBase::IncrementSessionCount()
    {
    LOG_FUNC_NAME;
    iSessionCount++;
    iShutdownTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::DecrementSessions
// Decrement sessions.
// -----------------------------------------------------------------------------
//
void CUpnpSymbianServerBase::DecrementSessionCount()
    {
    LOG_FUNC_NAME;
    iSessionCount--;
    ASSERT( iSessionCount >= 0 );
    SuggestShutdown();
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::SuggestShutdown
// Checks every stop condidion and starts closing server timeout if all of them
// are fulfilled.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::SuggestShutdown()
    {
    if ( (iSessionCount <= 0) && CanBeStopped() )
        {
        iShutdownTimer->Start( KUpnpSymbianServerShutdownInterval );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::TimerEventL
// End of shutdown timer event
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    ASSERT( iShutdownTimer == aTimer );
    ASSERT( (iSessionCount <= 0) && CanBeStopped() );
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::RunL
// Invoked when server receives message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSymbianServerBase::RunL()
    {
    switch( Message().Function() )
        {
        case RMessage2::EConnect:
        IncrementSessionCount();
        break;
        case RMessage2::EDisConnect:
        DecrementSessionCount();
        break;
        }
    CServer2::RunL();
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSymbianServerBase::RunError( TInt aError )
    {
    LOG_FUNC_NAME;
    if ( aError == KErrBadDescriptor )
        {
        // A bad descriptor error implies a badly programmed client, so panic it;
        // otherwise report the error to the client
        LOGS( "RunError - BadClient" );
        PanicClient( Message(), EBadDescriptor );
        }
    else if ( aError != KErrCorrupt )
        {
        LOGS( "RunError - Faulty Message" );
        if ( !Message().IsNull() )
            {
            Message().Complete( aError );
            }
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    // Handled the error fully
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::NewSessionL
// Create new session or leaves with KErrNotSupported if version is unsupported.
// -----------------------------------------------------------------------------
//
EXPORT_C CSession2* CUpnpSymbianServerBase::NewSessionL(
        const TVersion& aVersion, const RMessage2& aMessage ) const
    {
    LOG_FUNC_NAME;
    // Check we're the right version
    if ( !User::QueryVersionSupported( SupportedVersion(), aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    return NewSessionL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpSymbianServerBase::CanBeStopped
// Default implementation for transient servers - no additional stopping conditions
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpSymbianServerBase::CanBeStopped() const
    {
    return ETrue;
    }

// End Of File
