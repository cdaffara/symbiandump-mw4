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
* Symbian platform specific implementation of WLAN IAP scanning.
*/

// System includes

#include <QList>
#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "wlanqtutilsscan.h"
#include "wlanqtutilsscaniap_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsscaniap_symbianTraces.h"
#endif


/*!
    \class WlanQtUtilsScanIapPrivate
    \brief Symbian platform specific implementation of WLAN IAP scanning.

    Symbian platform specific implementation of WLAN IAP scanning. 
*/


// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Static factory constructor.
    
    @param [in,out] wrapper Public implementation class reference.
    
    @return New instance of the class.
*/

WlanQtUtilsScanIapPrivate *WlanQtUtilsScanIapPrivate::NewL(WlanQtUtilsScan *wrapper)
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_NEWL_ENTRY);
    
    WlanQtUtilsScanIapPrivate *self = new (ELeave) WlanQtUtilsScanIapPrivate(wrapper);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_NEWL_EXIT);
    return self;
}

/*!
    Destructor.
*/

WlanQtUtilsScanIapPrivate::~WlanQtUtilsScanIapPrivate()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_WLANQTUTILSSCANIAPPRIVATE_ENTRY);
    
    Cancel();
    delete mWlanMgmtClient;
    mWlanMgmtClient = NULL;
    mAvailableIaps.Close();
    
    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_WLANQTUTILSSCANIAPPRIVATE_EXIT);
}

/*!
    Starts a scan to see currently available WLAN IAPs.
*/

void WlanQtUtilsScanIapPrivate::ScanIaps()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_SCANIAPS_ENTRY);
    
    // Scanning while previous scan is not complete is not supported
    Q_ASSERT(!IsActive());

    // Make the scan
    mAvailableIaps.Reset();
    mWlanMgmtClient->GetAvailableIaps(
        mCacheLifetime,
        mMaxDelay,
        EFalse,
        iStatus,
        mAvailableIaps);
    SetActive();

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_SCANIAPS_EXIT);
}

/*!
    Stops an ongoing scan.
*/

void WlanQtUtilsScanIapPrivate::StopScan()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_STOPSCAN_ENTRY);

    Cancel();

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_STOPSCAN_EXIT);
}

/*!
    Scan results handler.
*/

void WlanQtUtilsScanIapPrivate::RunL()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_RUNL_ENTRY);
    
    QT_TRYCATCH_LEAVING(
        if (iStatus != KErrNone) {
            // Scan failed
            emit q_ptr->scanFailed(WlanQtUtils::ScanStatusError);
        } else {
            // Scan succeeded
            QList< QSharedPointer<WlanQtUtilsAp> > scanResults;
            ExtractScanResults(scanResults);
            
            // Inform about the results 
            emit q_ptr->availableWlanIaps(scanResults);
        }
    );

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_RUNL_EXIT);
}

/*!
    Cancels an outstanding request.
*/

void WlanQtUtilsScanIapPrivate::DoCancel()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_DOCANCEL_ENTRY);
    
    mWlanMgmtClient->CancelGetAvailableIaps();

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_DOCANCEL_EXIT);
}

/*!
    Constructor.
    
    @param [in,out] wrapper Public implementation class reference.
*/

WlanQtUtilsScanIapPrivate::WlanQtUtilsScanIapPrivate(WlanQtUtilsScan *wrapper) :
    CActive(EPriorityStandard),
    q_ptr(wrapper),
    mWlanMgmtClient(NULL),
    mCacheLifetime(0),
    mMaxDelay(0),
    mAvailableIaps()
{
    OstTraceFunctionEntry0(DUP1_WLANQTUTILSSCANIAPPRIVATE_WLANQTUTILSSCANIAPPRIVATE_ENTRY);
    OstTraceFunctionExit0(DUP1_WLANQTUTILSSCANIAPPRIVATE_WLANQTUTILSSCANIAPPRIVATE_EXIT);
}

/*!
    Second phase constructor.
*/

void WlanQtUtilsScanIapPrivate::ConstructL()
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_CONSTRUCTL_ENTRY);
    
    CActiveScheduler::Add(this);
    mWlanMgmtClient = CWlanMgmtClient::NewL();
    
    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_CONSTRUCTL_EXIT);
}

/*
    Extracts scan results and stores them into the given AP list.
    
    @param [out] scanResults Scan result list.
*/

void WlanQtUtilsScanIapPrivate::ExtractScanResults(
    QList< QSharedPointer<WlanQtUtilsAp> > &scanResults)
{
    OstTraceFunctionEntry0(WLANQTUTILSSCANIAPPRIVATE_EXTRACTSCANRESULTS_ENTRY);

    for (int i=0; i < mAvailableIaps.Count(); i++) {
        // Create an IAP for each result
        QSharedPointer<WlanQtUtilsAp> iap(new WlanQtUtilsAp());

        // IAP ID
        iap->setValue(
            WlanQtUtilsAp::ConfIdIapId,
            (int)mAvailableIaps[i].iIapId);

        // Signal strength
        iap->setValue(
            WlanQtUtilsAp::ConfIdSignalStrength,
            (int)mAvailableIaps[i].iRssi);

        // Append the IAP to scan result list
        scanResults.append(iap);
    }

    OstTraceFunctionExit0(WLANQTUTILSSCANIAPPRIVATE_EXTRACTSCANRESULTS_EXIT);
}
