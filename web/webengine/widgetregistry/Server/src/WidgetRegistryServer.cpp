/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32base.h>
#include <w32std.h>
#include <apgcli.h>
#include "WidgetRegistryConstants.h"
#include "WidgetRegistryServer.h"
#include "WidgetRegistrySession.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetRegistryServer::CWidgetRegistryServer
// C++ default constructor can NOT contain any code, that
// might leave.
//
// -----------------------------------------------------------------------------
//
CWidgetRegistryServer::CWidgetRegistryServer() :
    CPolicyServer(0,widgetRegistryPolicy,ESharableSessions)
    {
    }

// -----------------------------------------------------------------------------
// CWidgetRegistryServer::ConstructL
// Symbian 2nd phase constructor can leave.
//
// -----------------------------------------------------------------------------
//
void CWidgetRegistryServer::ConstructL()
    {
    StartL( KWidgetRegistryName );
    iShutDown.ConstructL();
    iShutDown.Start();
    }

CWidgetRegistryServer::~CWidgetRegistryServer()
    {   
    }
// -----------------------------------------------------------------------------
// CWidgetRegistryServer::NewL
// Two-phased constructor.
//
// -----------------------------------------------------------------------------
//
 CServer2* CWidgetRegistryServer::NewLC()
    {
    CWidgetRegistryServer* self = new ( ELeave ) CWidgetRegistryServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistryServer::NewSessionL
// Creates a new widgetregistry server session
//
// -----------------------------------------------------------------------------
//
CSession2* CWidgetRegistryServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& aMessage ) const
    {
    CWidgetRegistryServerSession* session = CWidgetRegistryServerSession::NewL(
        const_cast<CWidgetRegistryServer&>( *this ),aMessage );
    return session;
    }


// -----------------------------------------------------------------------------
// CWidgetRegistryServer::AddSession
// Cancel the shutdown timer now, the new session is connected
//
// -----------------------------------------------------------------------------
//
void CWidgetRegistryServer::AddSession()
    {
    ++iSessionCount;
    iShutDown.Cancel();  // Cancel any outstanding shutdown timer
    }

// -----------------------------------------------------------------------------
// CWidgetRegistryServer::RemoveSession
// Decrement the session counter and start the shutdown timer if the last client
// has disconnected
// -----------------------------------------------------------------------------
//
void CWidgetRegistryServer::RemoveSession()
    {
    if ( --iSessionCount == 0 )
        {
        iShutDown.Start();
        }
    }

// -----------------------------------------------------------------------------
// CWidgetRegistryServer::RunServerL
// Initialize and run the server
//
// -----------------------------------------------------------------------------
//
void CWidgetRegistryServer::RunServerL()
    {
    // We need to tell the app list server about widgets, this happens
    // during installing a widget but if a widget is already installed
    // (say on a memory card) then the app list server should already
    // know about widgets
    RApaLsSession apparcSession;
    if ( KErrNone == apparcSession.Connect() )
        {
        TRAP_IGNORE(
            apparcSession.RegisterNonNativeApplicationTypeL(
                KUidWidgetLauncher, KLauncherApp()));
        apparcSession.Close();
        }

    // First create and install the active scheduler
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL(scheduler);

    CActiveScheduler::Install(scheduler);

    // create the server
    CWidgetRegistryServer::NewLC();

    // Naming the server thread after the server helps to debug panics
    User::LeaveIfError( RThread().RenameMe( KWidgetRegistryName ) );

    RProcess::Rendezvous(KErrNone);

    // Enter the wait loop
    CActiveScheduler::Start();

    // Exited - cleanup the server and scheduler

    CleanupStack::PopAndDestroy( 2, scheduler );
    }

// -----------------------------------------------------------------------------
// Server process entry-point
//
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK; // Heap checking

    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r = KErrNoMemory;

    if ( cleanup )
        {
        TRAP( r, CWidgetRegistryServer::RunServerL() );
        delete cleanup;
        }

    __UHEAP_MARKEND;
    return r;
    }

// -----------------------------------------------------------------------------
// CShutdown::RunServerL
// Initiates server exit when the timer expires
//
// -----------------------------------------------------------------------------
//
void CShutdown::RunL()
    {
    CActiveScheduler::Stop();
    }

// End of File
