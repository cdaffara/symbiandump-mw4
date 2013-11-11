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
* WLAN Management API stubbing for WLAN Qt Utilities use in emulator.
*/

#include <e32std.h>
#include <e32base.h>
#include "wlanmgmtclient.h"

// ---------------------------------------------------------
// CWlanMgmtClient::NewL
// ---------------------------------------------------------
//
CWlanMgmtClient* CWlanMgmtClient::NewL()
    {
    CWlanMgmtClient* self = new CWlanMgmtClient;
    return self;
    }
    
// ---------------------------------------------------------
// CWlanMgmtClient::~CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::~CWlanMgmtClient()
    {
    }

// ---------------------------------------------------------
// CWlanMgmtClient::CWlanMgmtClient
// ---------------------------------------------------------
//
CWlanMgmtClient::CWlanMgmtClient()
    {
    }

// ---------------------------------------------------------
// CWlanMgmtClient::GetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtClient::GetScanResults(
    TRequestStatus& aStatus,
    CWlanScanInfo& aResults )
    {
    // Complete the request immediately
    TRequestStatus *status = &aStatus;
    User::RequestComplete(status, KErrNone);
    
    // Results are returned when asked per AP
    (void)aResults;
    }

// ---------------------------------------------------------
// CWlanMgmtClient::GetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtClient::GetScanResults(
    TWlanSsid& aSsid,
    TRequestStatus& aStatus,
    CWlanScanInfo& aResults )
    {
    (void)aSsid;
    (void)aResults;

    // Complete the request immediately
    TRequestStatus *status = &aStatus;
    User::RequestComplete(status, KErrNone);
    
    // Results are returned when asked per AP
    }

// ---------------------------------------------------------
// CWlanMgmtClient::CancelGetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelGetScanResults()
    {
    }

// ---------------------------------------------------------
// CWlanMgmtClient::GetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtClient::GetAvailableIaps(
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TBool aFilteredResults,
    TRequestStatus& aStatus,
    RArray<TWlanIapAvailabilityData>& aAvailableIaps )
    {
    (void)aCacheLifetime;
    (void)aMaxDelay;
    (void)aFilteredResults;
    (void)aAvailableIaps;
    
    // Complete the request immediately
    TRequestStatus *status = &aStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------
// CWlanMgmtClient::CancelGetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelGetAvailableIaps()
    {
    }
