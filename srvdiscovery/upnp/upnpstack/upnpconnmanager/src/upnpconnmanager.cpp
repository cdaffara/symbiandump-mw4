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
* Description:  class CUpnpConnectionManager
*
*/

// INCLUDE FILES
#include "upnpconnmanager.h"
#define KLogFile _L("UPnPConnManager.txt")
#include "upnpcustomlog.h"
#include "upnpconnmanagerengine.h"
#include "upnpconnmanagersession.h"

const TVersion KServerSupportedVersion(
    KUpnpConnectionManagerMajorVersionNumber,
    KUpnpConnectionManagerMinorVersionNumber,
    KUpnpConnectionManagerBuildVersionNumber );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::CUpnpConnectionManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpConnectionManager::CUpnpConnectionManager()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::~CUpnpConnectionManager
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpConnectionManager::~CUpnpConnectionManager()
    {
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSymbianServerBase* CUpnpConnectionManager::NewLC()
    {
    LOG_FUNC_NAME;
    CUpnpConnectionManager* connManager = new (ELeave) CUpnpConnectionManager();
    CleanupStack::PushL( connManager );
    connManager->ConstructL() ;
    return connManager;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpConnectionManager::ConstructL()
    {
    LOG_FUNC_NAME;
    BaseConstructL();
    iEngine = CUpnpConnManagerEngine::NewL( iSessionIter );
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CUpnpConnectionManager::NewSessionL( const RMessage2& /*aMsg*/ ) const
    {
    LOG_FUNC_NAME;
    return CUpnpConnManagerSession::NewL( *iEngine );
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::SupportedVersion
// Gets server supported version.
// -----------------------------------------------------------------------------
//
TVersion CUpnpConnectionManager::SupportedVersion() const
    {
    return KServerSupportedVersion;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::ServerName
// Gets server supported version.
// -----------------------------------------------------------------------------
//
const TDesC& CUpnpConnectionManager::ServerName() const
    {
    return KUpnpConnectionManagerName;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpConnectionManager::RunError( TInt aError )
    {
    LOG_FUNC_NAME;
    return CUpnpSymbianServerBase::RunError( aError );
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main entry point.
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CUpnpConnectionManager::StartServer(
        KUpnpConnectionManagerThreadName, CUpnpConnectionManager::NewLC );
    }

// End of File

