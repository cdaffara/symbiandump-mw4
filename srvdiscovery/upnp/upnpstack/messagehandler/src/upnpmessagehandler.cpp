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
 * Description:  Message handler
 *
 */

// INCLUDE FILES
#include "upnpmessagehandler.h"
#define KLogFile _L("UpnpMessageHandler.txt")
#include "upnpcustomlog.h"

const TVersion KServerSupportedVersion( KMessageHandlerMajorVersionNumber,
    KMessageHandlerMinorVersionNumber, KMessageHandlerBuildVersionNumber );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::CUpnpMessageHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandler::CUpnpMessageHandler()
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::~CUpnpMessageHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandler::~CUpnpMessageHandler()
    {
    LOG_FUNC_NAME;
    iObservers.Close();
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSymbianServerBase* CUpnpMessageHandler::NewLC()
    {
    LOG_FUNC_NAME;
    CUpnpMessageHandler* messageHandler = new (ELeave) CUpnpMessageHandler();
    CleanupStack::PushL( messageHandler );
    messageHandler->ConstructL() ;
    return messageHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::ConstructL()
    {
    LOG_FUNC_NAME;
    BaseConstructL();
    iEngine = CUpnpMessageHandlerEngine::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::ServerName
// Return name of the server
// -----------------------------------------------------------------------------
//
const TDesC& CUpnpMessageHandler::ServerName() const
    {
    return KMessageHandlerName;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CUpnpMessageHandler::NewSessionL( const RMessage2& /*aMsg*/) const
    {
    LOG_FUNC_NAME;
    return CUpnpMessageHandlerSession::NewL(
        *const_cast<CUpnpMessageHandler*> ( this ), *iEngine );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::SupportedVersion
// Gets server supported version.
// -----------------------------------------------------------------------------
//
TVersion CUpnpMessageHandler::SupportedVersion() const
    {
    return KServerSupportedVersion;
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main entry point.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CUpnpMessageHandler::StartServer( KMessageHandlerThreadName,
        CUpnpMessageHandler::NewLC );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::DeviceListUpdateL
// Update device list.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::DeviceListUpdateL()
    {
    LOG_FUNC_NAME;
    // Inform every Observer (every session) that the device list has changed
    for ( TInt i(0); i < iObservers.Count(); i++ )
        {
        iObservers[i]->DeviceListChangeL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::AddObserverL
// Add observer.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::AddObserverL( MMessageHandlerObserver* aObserver )
    {
    LOG_FUNC_NAME;
    iObservers.InsertL( aObserver, iObservers.Count() );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::RemoveObserver
// Remove observer.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::RemoveObserver( MMessageHandlerObserver* aObserver )
    {
    LOG_FUNC_NAME;
    LOGS1( "CUpnpMessageHandler::RemoveObserver - Count: %d", iObservers.Count());

    for ( TInt i(0); i < iObservers.Count(); i++ )
        {
        if ( iObservers[i] == aObserver )
            {
            LOGS1( "CUpnpMessageHandler::RemoveObserver - Removed osb: %d",i);
            iObservers.Remove( i );
            iObservers.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandler::RunError( TInt aError )
    {
    LOG_FUNC_NAME;
    iStartingError = aError;
    return CUpnpSymbianServerBase::RunError( aError );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::IsSsdpStarted
// Set function for iSsdpStarted
// -----------------------------------------------------------------------------
//     
TBool CUpnpMessageHandler::IsSsdpStarted()
    {
    return iSsdpStarted;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::GetError
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandler::StartingError()
    {
    return iStartingError;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::SetStarted
// Set function for iSsdpStarted
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::SetSsdpStarted( TBool aStarted )
    {
    iSsdpStarted = aStarted;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::Sessions
// Returns client sessions
// -----------------------------------------------------------------------------
//
TDblQueIter< CSession2 >& CUpnpMessageHandler::Sessions()
    {
    return iSessionIter;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandler::IPListChange
// Notify all observers
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandler::IPListChange()
    {
#ifdef RD_UPNP_REMOTE_ACCESS
    for(TInt i(0); i < iObservers.Count(); i++)
        {
        iObservers[i]->IPListChange();
        }
#endif    
    }

// End of File
