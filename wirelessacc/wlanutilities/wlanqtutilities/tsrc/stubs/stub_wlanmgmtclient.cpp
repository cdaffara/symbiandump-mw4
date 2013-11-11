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
* WLAN Management API stubbing for WLAN Qt Utilities.
*/

#include <e32std.h>
#include <e32base.h>
#include "wlanmgmtclient.h"

#include "wlanqtutilstestcontext.h"
#include "wlanqtutilsap.h"

extern WlanQtUtilsTestContext testContext;

// Request status for canceling stubbed GetScanResults async request
static TRequestStatus* iGetScanResultsStatus = 0;

// Request status for canceling stubbed GetAvailableIaps async request
static TRequestStatus* iGetAvailableIapsStatus = 0;

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
    (void)aResults;
    
    if (testContext.mScan.mCompleteWlanApScan) {
        // Complete the request immediately
        TRequestStatus *status = &aStatus;
        User::RequestComplete(
            status,
            testContext.mScan.mApScanRetValue);
    } else {
        iGetScanResultsStatus = &aStatus;
    }
    
    // Results are returned when asked per AP
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

    if (testContext.mScan.mCompleteWlanApScan) {
        // Complete the request immediately
        TRequestStatus *status = &aStatus;
        User::RequestComplete(
            status,
            testContext.mScan.mApScanRetValue);
    } else {
        iGetScanResultsStatus = &aStatus;
    }
    
    // Results are returned when asked per AP
    }

// ---------------------------------------------------------
// CWlanMgmtClient::CancelGetScanResults
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelGetScanResults()
    {
    if (iGetScanResultsStatus) {
        User::RequestComplete(iGetScanResultsStatus, KErrCancel);
        iGetScanResultsStatus = NULL;
    }
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

    if (testContext.mScan.mCompleteWlanIapScan) {
        // Form results as defined in context
        aAvailableIaps.Reset();
        for (int i=0; i < testContext.mScan.mWlanScanIapResultList.count(); i++) {
            QSharedPointer<WlanQtUtilsAp> iap(testContext.mScan.mWlanScanIapResultList[i]);
            TWlanIapAvailabilityData data;
            data.iIapId = (TUint)iap->value(WlanQtUtilsAp::ConfIdIapId).toInt();
            data.iRssi = (TUint)iap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt();
            aAvailableIaps.AppendL(data);
        }
        
        // Complete the request immediately
        TRequestStatus *status = &aStatus;
        User::RequestComplete(
            status,
            testContext.mScan.mIapScanRetValue);
    } else {
        iGetAvailableIapsStatus = &aStatus;
    }
    }

// ---------------------------------------------------------
// CWlanMgmtClient::CancelGetAvailableIaps
// ---------------------------------------------------------
//
void CWlanMgmtClient::CancelGetAvailableIaps()
    {
    if (iGetAvailableIapsStatus) {
        User::RequestComplete(iGetAvailableIapsStatus, KErrCancel);
        iGetAvailableIapsStatus = 0;
    }
    }
