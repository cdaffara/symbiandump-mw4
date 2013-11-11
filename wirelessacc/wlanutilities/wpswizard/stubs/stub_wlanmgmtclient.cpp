/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Management API stubbing for WPS Wizard use in emulator.
*/

#include <e32std.h>
#include <e32base.h>
#include "wlanmgmtclient.h"

// ---------------------------------------------------------
// CWlanScanRequest::NewL
// ---------------------------------------------------------
//
CWlanMgmtClient* CWlanMgmtClient::NewL()
    {
    CWlanMgmtClient* self = new CWlanMgmtClient;
    return self;
    }
    
// ---------------------------------------------------------
// CWlanScanRequest::~CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::~CWlanMgmtClient()
    {
    }

// ---------------------------------------------------------
// CWlanScanRequest::CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::CWlanMgmtClient()
    {
    }

// ---------------------------------------------------------
// CWlanScanRequest::RunProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtClient::RunProtectedSetup(
    TRequestStatus& aStatus,
    const TWlanSsid& aSsid,
    const TWlanWpsPin& /* aWpsPin */,
    CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>& aCredentials )
{
    // Complete the request immediately
    TRequestStatus *status = &aStatus;
    TInt err = KErrNone;
    TWlanProtectedSetupCredentialAttribute attr;
    
    attr.iSsid.Copy(aSsid);
    attr.iSecurityMode = EWlanIapSecurityModeAllowUnsecure;
    attr.iAuthenticationMode = EWlanAuthenticationModeOpen;
    attr.iOperatingMode = EWlanOperatingModeInfrastructure;
    attr.iWepDefaultKey = EWlanDefaultWepKey1;
    
    TRAP(err, aCredentials.AppendL(attr) );
    User::RequestComplete(status, err);
}

// ---------------------------------------------------------
// CWlanScanRequest::CancelProtectedSetup
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelProtectedSetup()
    {
    }

