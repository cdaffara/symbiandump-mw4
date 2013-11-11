/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  alfperfappdeamon implementation.
*
*/


// INCLUDE FILES
#include <eikstart.h>
#include <apgwgnam.h>
#include "alfperfappdaemon.h"

#include "../../alfdebugextension/inc/alfdebug.h"

/**
 * Reset inactity period in microseconds.
 */
const TInt KAlfPerfAppActivityPollPeriod = 2500000;

// ============================ MEMBER FUNCTIONS ===============================

//
// Implementation of CAlfPerfAppDaemonApplication
//

CApaDocument* CAlfPerfAppDaemonApplication::CreateDocumentL()
    {
    return new (ELeave) CAlfPerfAppDaemonDocument( *this );
    }

TUid CAlfPerfAppDaemonApplication::AppDllUid() const
    {
    return KAlfPerfAppDaemonUid;
    }

void CAlfPerfAppDaemonApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = NULL;
    aAppServer = new (ELeave) CAlfPerfAppDaemonServer;
    }

//
// Implementation of CAlfPerfAppDaemonDocument
//

CAlfPerfAppDaemonDocument::CAlfPerfAppDaemonDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

CEikAppUi* CAlfPerfAppDaemonDocument::CreateAppUiL()
    {
    return new (ELeave) CAlfPerfAppDaemonAppUi;
    }

void CAlfPerfAppDaemonDocument::UpdateTaskNameL(CApaWindowGroupName* aWgName)
    {
    CAknDocument::UpdateTaskNameL( aWgName );
    aWgName->SetHidden( ETrue );
    }

//
// Implementation of CAlfPerfAppDaemonAppUi
//

CAlfPerfAppDaemonAppUi::CAlfPerfAppDaemonAppUi()
    {
    }

CAlfPerfAppDaemonAppUi::~CAlfPerfAppDaemonAppUi()
    {
    delete iDebug;
    delete iFactory;
    iLibrary.Close();
    }

CAlfDebugExtension* CAlfPerfAppDaemonAppUi::Debug()
    {
    return iDebug;
    }

void CAlfPerfAppDaemonAppUi::ConstructL()
    {
    CAknAppUi::BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    
    RWsSession& wsSession = iEikonEnv->WsSession();
    wsSession.ComputeMode( RWsSession::EPriorityControlDisabled );
    RProcess().SetPriority( EPriorityHigh );
    iEikonEnv->SetSystem( ETrue );
    
    if ( iLibrary.Load( KAlfDebugExtensionLibraryName ) == KErrNone )
        {
        TInt res = iLibrary.Lookup( KAlfDebugExtensionLibraryEntryOrdinal )();
        iFactory = (CAlfDebugFactory*)res;
        if ( iFactory )
            {
            iDebug = iFactory->CreateDebugExtensionL( NULL );
            }
        }
    }

// -----------------------------------------------------------------------------
// Common commands are handled here.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppDaemonAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            Exit();
            break;

        default:
            break;
        }
    }

//
// Implementation of CAlfPerfAppDaemonServer
//

CAlfPerfAppDaemonServer::CAlfPerfAppDaemonServer()
    {
    }

CApaAppServiceBase* CAlfPerfAppDaemonServer::CreateServiceL( TUid aServiceType ) const
    {
    if ( aServiceType == KAlfPerfAppDaemonUid )
        {
        return new (ELeave) CAlfPerfAppDaemonService;
        }
    else
        {
        User::Leave( KErrNotSupported );
        return NULL;
        }
    }

//
// Implementation of CAlfPerfAppDaemonService
//

CAlfPerfAppDaemonService::CAlfPerfAppDaemonService()
    {
    }

CAlfPerfAppDaemonService::~CAlfPerfAppDaemonService()
    {
    if ( iActivityPoll )
        {
        iActivityPoll->Cancel();
        delete iActivityPoll;
        }
    }

void CAlfPerfAppDaemonService::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EAlfPerfAppIpcGetMeasurements:
            {
            CAlfPerfAppDaemonAppUi* appui =
                static_cast<CAlfPerfAppDaemonAppUi*>( 
                    CEikonEnv::Static()->EikAppUi() );
            CAlfDebugExtension* debug = appui->Debug();
            if ( debug )
                {
                TAlfDebugServerMeasurements measurements;
                TInt err = debug->GetMeasurements( measurements );
                if ( err == KErrNone )
                    {
                    TPckgC<TAlfDebugServerMeasurements> pckg( measurements );
                    err = aMessage.Write( 0, pckg );
                    }
                aMessage.Complete( err );
                }
            else
                {
                aMessage.Complete( KErrNotSupported );
                }
            }
            break;

        case EAlfPerfAppIpcEnableActivityPoll:
            {
            const TBool enable = ( aMessage.Int0() != 0 );
            if ( enable )
                {
                User::ResetInactivityTime();
                if ( !iActivityPoll )
                    {
                    iActivityPoll = CPeriodic::NewL( CActive::EPriorityHigh );
                    }
                iActivityPoll->Cancel();
                iActivityPoll->Start(
                    KAlfPerfAppActivityPollPeriod,
                    KAlfPerfAppActivityPollPeriod,
                    TCallBack( ActivityPollCallBack, this ) );
                }
            else
                {
                if ( iActivityPoll )
                    {
                    iActivityPoll->Cancel();
                    }
                delete iActivityPoll;
                iActivityPoll = NULL;
                }

            aMessage.Complete( KErrNone );
            }
            break;

        default:
            CAknAppServiceBase::ServiceL( aMessage );
            break;
        }
    }

TInt CAlfPerfAppDaemonService::ActivityPollCallBack( TAny* /*aAny*/ )
    {
    User::ResetInactivityTime();
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

LOCAL_C CApaApplication* NewApplication()
    {
    return new CAlfPerfAppDaemonApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

