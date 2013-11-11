/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* Server component for VPN Manager, used by VPN API. 
*
*/

#include <e32svr.h>
#include <e32math.h>

#include "vpnmanagerserver.h"
#include "vpnmanagersession.h"
#include "vpnapiservant.h"


const TUint CVpnManagerServer::iVpnManRangeCount = 3;
	
const TInt CVpnManagerServer::iVpnManRanges[iVpnManRangeCount] = 
	{
	0,
    EVpnImportPolicy,
    EVpnUpdatePolicyData+1
	};

const TUint8 CVpnManagerServer::iVpnManElementIndex[iVpnManRangeCount] = 
	{
	CPolicyServer::ENotSupported,
	0,
	CPolicyServer::ENotSupported
	};

const CPolicyServer::TPolicyElement CVpnManagerServer::iVpnManElements[] =
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CVpnManagerServer::iVpnManPolicy =
    {
    0, 						// All connect attempts are checked
    iVpnManRangeCount,     	// Count of ranges
    iVpnManRanges,  		// 0-999, 1000-1008, 1009...
    iVpnManElementIndex,  	// Only range 1000-1008 are checked
    iVpnManElements 		// The list of policy elements
    };

CVpnManagerServer::CVpnManagerServer(TInt aPriority) : CPolicyServer(aPriority,iVpnManPolicy)
    {
    }

CVpnManagerServer::~CVpnManagerServer()
    {
    delete iVpnApiServant;
    iFs.Close();
    }

CVpnManagerServer* CVpnManagerServer::NewL()
    {
    CVpnManagerServer* server = CVpnManagerServer::NewLC();
    CleanupStack::Pop(); // server
    return server;
    }

CVpnManagerServer* CVpnManagerServer::NewLC()
    {
    CVpnManagerServer* server = new (ELeave) CVpnManagerServer(EPriorityNormal);
    CleanupStack::PushL(server); 
    server->ConstructL();
    return server;
    }

void CVpnManagerServer::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
    iVpnApiServant = CVpnApiServant::NewL(iFs);       
    StartL(KVpnManagerServer);
    }

CSession2* CVpnManagerServer::NewSessionL(
    const TVersion &aVersion,
    const RMessage2& /*aMessage*/) const
    {
    // Check that the client is requesting a session with the right version
    if (!User::QueryVersionSupported(TVersion(KVpnManagerMajorVersionNumber,
                                              KVpnManagerMinorVersionNumber,
                                              KVpnManagerBuildVersionNumber),
                                     aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    // Make new session
    return CVpnManagerSession::NewL(*const_cast<CVpnManagerServer*>(this),
                                    *iVpnApiServant);
    }

void CVpnManagerServer::IncrementSessions()
    {
    iSessionCount++;
    }

void CVpnManagerServer::DecrementSessions()
    {
    iSessionCount--;
    
    // Terminate the server when there are no clients left
    if (iSessionCount <= 0)
        {
        CActiveScheduler::Stop();
        }    
    }

TInt CVpnManagerServer::RunError(TInt aError)
    {
    Message().Complete(aError);

    // The leave will result in an early return from CServer::RunL(),
    // skipping the call to request another message. So we issue the
    // request here in order to keep the server running.
    ReStart();

    // Handled the error fully
    return KErrNone;    
    }
