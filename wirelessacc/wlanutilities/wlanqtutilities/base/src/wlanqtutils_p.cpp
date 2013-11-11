/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Qt Utilities private implementation.
*/

// System includes

#include <QSharedPointer>

// User includes

#include "wlanqtutilsap.h"
#include "wlanqtutilsconnection.h"
#include "wlanqtutilsiapsettings.h"
#include "wlanqtutilsconmonwrapper.h"
#include "wlanqtutilsesockwrapper.h"
#include "wlanqtutilsscan.h"

#include "wlanqtutils.h"
#include "wlanqtutils_p.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutils_pTraces.h"
#endif

/*!
    \class WlanQtUtilsPrivate
    \brief Private implementation class for WLAN Qt Utilities.

    The private interface functions are identical to those in WlanQtUtils
    class, so refer to that for descriptions. Other functions contain
    implementation documentation.
*/


// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

WlanQtUtilsPrivate::WlanQtUtilsPrivate(WlanQtUtils *q_ptr) :
    q_ptr(q_ptr),
    mSettings(new WlanQtUtilsIapSettings(this)),
    mConMonWrapper(new WlanQtUtilsConMonWrapper(this)),
    mScanWrapper(new WlanQtUtilsScan(this)),
    mEsockWrapper(new WlanQtUtilsEsockWrapper(this)),
    mIctService(),
    mScanMode(ScanModeNone),
    mIapScanList(),
    mWlanScanList(),
    mToBeTestedIapId(WlanQtUtils::IapIdNone), 
    mConnectingIapId(WlanQtUtils::IapIdNone),
    mConnection()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_WLANQTUTILSPRIVATE_ENTRY, this);

    // Make all connections.
    bool connectStatus = connect(
        mScanWrapper,
        SIGNAL(availableWlanIaps(QList< QSharedPointer<WlanQtUtilsAp> > &)),
        this,
        SLOT(updateAvailableWlanIaps(QList< QSharedPointer<WlanQtUtilsAp> > &)));
    Q_ASSERT(connectStatus);
    
    connectStatus = connect(
        mScanWrapper, 
        SIGNAL(availableWlanAps(QList< QSharedPointer<WlanQtUtilsAp> >&)), 
        this, 
        SLOT(updateAvailableWlanAps(QList< QSharedPointer<WlanQtUtilsAp> >&)));
    Q_ASSERT(connectStatus);
    
    connectStatus = connect(
        mScanWrapper,
        SIGNAL(scanFailed(int)),
        this,
        SLOT(updateScanFailed(int)));
    Q_ASSERT(connectStatus);

    connectStatus = connect(
        mEsockWrapper, 
        SIGNAL(connectionStatusFromWrapper(bool)), 
        this,
        SLOT(updateConnectionStatus(bool)));
    Q_ASSERT(connectStatus);

    connectStatus = connect(
        mConMonWrapper, 
        SIGNAL(connCreatedEventFromWrapper(uint)), 
        this,
        SLOT(addActiveConnection(uint)));
    Q_ASSERT(connectStatus);

    connectStatus = connect(
        mConMonWrapper, 
        SIGNAL(connDeletedEventFromWrapper(uint)), 
        this,
        SLOT(removeActiveConnection(uint)));
    Q_ASSERT(connectStatus);

    connectStatus = connect(
        mConMonWrapper, 
        SIGNAL(connStatusEventFromWrapper(uint, WlanQtUtils::ConnStatus)),
        this, 
        SLOT(updateActiveConnection(uint, WlanQtUtils::ConnStatus)));
    Q_ASSERT(connectStatus);

    // Retrieve initial status of active connections
    mConnection = QSharedPointer<WlanQtUtilsConnection>(
        mConMonWrapper->activeConnection());

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_WLANQTUTILSPRIVATE_EXIT, this);
}

/*!
    Destructor. 
*/

WlanQtUtilsPrivate::~WlanQtUtilsPrivate()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_WLANQTUTILSPRIVATEDESTR_ENTRY, this);
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_WLANQTUTILSPRIVATEDESTR_EXIT, this);
}

/*!
   See WlanQtUtils::scanWlans().
*/

void WlanQtUtilsPrivate::scanWlans()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_SCANWLANS_ENTRY, this);

    // Scanning while there is an ongoing scan is not supported
    Q_ASSERT(mScanMode == ScanModeNone);
    
    // Starting with an IAP scan, and continuing with AP scan
    mScanMode = ScanModeAvailableWlans;
    mScanWrapper->scanWlanIaps();

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_SCANWLANS_EXIT, this);
}

/*!
   See WlanQtUtils::scanWlanAps().
*/

void WlanQtUtilsPrivate::scanWlanAps()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_SCANWLANAPS_ENTRY, this);
    
    // Scanning while there is an ongoing scan is not supported
    Q_ASSERT(mScanMode == ScanModeNone);
    
    // Just forward the request to wrapper, which triggers a broadcast WLAN scan
    mScanMode = ScanModeAvailableWlanAps;
    mScanWrapper->scanWlanAps();

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_SCANWLANAPS_EXIT, this);
}

/*!
   See WlanQtUtils::scanWlanDirect().
*/

void WlanQtUtilsPrivate::scanWlanDirect(const QString &ssid)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_SCANWLANDIRECT_ENTRY, this);

    // Scanning while there is an ongoing scan is not supported
    Q_ASSERT(mScanMode == ScanModeNone);
    
    // Just forward the request to wrapper, which triggers a direct WLAN scan
    mScanMode = ScanModeDirect;
    mScanWrapper->scanWlanDirect(ssid);
    
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_SCANWLANDIRECT_EXIT, this);
}

/*!
   See WlanQtUtils::stopWlanScan().
*/

void WlanQtUtilsPrivate::stopWlanScan()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_STOPWLANSCAN_ENTRY, this);
    
    if (mScanMode != ScanModeNone) {
        int mode = mScanMode;
        // Stop the scan
        mScanMode = ScanModeNone;
        mScanWrapper->stopScan();

        // Inform that scan was cancelled
        reportScanResult(WlanQtUtils::ScanStatusCancelled, mode);
    }
    
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_STOPWLANSCAN_EXIT, this);
}

/*!
   See WlanQtUtils::availableWlans().
*/

void WlanQtUtilsPrivate::availableWlans(
    QList< QSharedPointer<WlanQtUtilsAp> > &wlanIapList,
    QList< QSharedPointer<WlanQtUtilsAp> > &wlanApList) const
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_AVAILABLEWLAN_ENTRY, this);

    wlanIapList.clear();
    wlanApList.clear();

    // Read the list of configured IAPs
    QList< QSharedPointer<WlanQtUtilsAp> > configuredIapList;
    mSettings->fetchIaps(configuredIapList);

    // Update the list of available IAPs
    foreach (QSharedPointer<WlanQtUtilsAp> iapIter, mIapScanList) {
        int iapId = iapIter->value(WlanQtUtilsAp::ConfIdIapId).toInt();
        QSharedPointer<WlanQtUtilsAp> iap(mSettings->fetchIap(iapId));
        if (iap) {
            // Only add the IAP if we (still) have the settings for it
            iap->setValue(
                WlanQtUtilsAp::ConfIdSignalStrength,
                iapIter->value(WlanQtUtilsAp::ConfIdSignalStrength));
            wlanIapList.append(iap);
        }
    }

    // Go through the scan results to find unknown APs
    for (int i = 0; i < mWlanScanList.count(); i++) {
        // Check whether an IAP with these parameters exists (in which
        // case this network is already added as an IAP in the loop above)
        if (!wlanIapExists(configuredIapList, mWlanScanList[i].data())) {
            // No IAP found in, copy the AP to caller's list of
            // unknown APs
            wlanApList.append(mWlanScanList[i]);
        }
    }

    traceIapsAndAps(wlanIapList, wlanApList);

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_AVAILABLEWLAN_EXIT, this);
}

/*!
   See WlanQtUtils::availableWlanAps().
*/

void WlanQtUtilsPrivate::availableWlanAps(
    QList< QSharedPointer<WlanQtUtilsAp> > &wlanApList) const
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_AVAILABLEWLANAPS_ENTRY, this);
    
    // Just copy the results
    wlanApList = mWlanScanList;
    
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_AVAILABLEWLANAPS_EXIT, this);
}

/*!
   See WlanQtUtils::createIap().
*/

int WlanQtUtilsPrivate::createIap(const WlanQtUtilsAp *wlanAp)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_CREATEIAP_ENTRY, this);

    // Create the new IAP and return its ID
    int newIapId = mSettings->createIap(wlanAp);

    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_CREATEIAP,
        "WlanQtUtilsPrivate::createIap;New IAP ID=%d",
        newIapId);
        
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_CREATEIAP_EXIT, this);
    return newIapId;
}

/*!
   See WlanQtUtils::updateIap().
*/

bool WlanQtUtilsPrivate::updateIap(int iapId, const WlanQtUtilsAp *wlanAp)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATEIAP_ENTRY, this);

    bool success = mSettings->updateIap(iapId, wlanAp);
    
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATEIAP_EXIT, this);
    return success;
}

/*!
   See WlanQtUtils::deleteIap().
*/

void WlanQtUtilsPrivate::deleteIap(int iapId)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_DELETEIAP_ENTRY, this);

    mSettings->deleteIap(iapId);

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_DELETEIAP_EXIT, this);
}

/*!
   See WlanQtUtils::connectIap().
*/

void WlanQtUtilsPrivate::connectIap(int iapId, bool runIct)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_CONNECTIAP_ENTRY, this);
    
    OstTraceExt2(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_CONNECTIAP,
        "WlanQtUtilsPrivate::connectIap;IAP ID=%d;runIct=%hhu",
        iapId,
        runIct);

    Q_ASSERT(iapId != WlanQtUtils::IapIdNone);

    if (runIct) {
        // Mark this IAP for ICT testing after it has been opened
        mToBeTestedIapId = iapId;
    }
    mConnectingIapId = iapId;
    mEsockWrapper->connectIap(iapId);

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_CONNECTIAP_EXIT, this);
}

/*!
   See WlanQtUtils::disconnectIap().
*/

void WlanQtUtilsPrivate::disconnectIap(int iapId)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_DISCONNECTIAP_ENTRY, this);
    
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_DISCONNECTIAP,
        "WlanQtUtilsPrivate::disconnectIap;IAP ID=%d",
        iapId);

    if (iapId != WlanQtUtils::IapIdNone) {
        // Close our RConnection handle, if needed. Wrapper ignores call, if
        // no handle exists.
        mEsockWrapper->disconnectIap();
        
        // In order to close connection even if there are other users for the
        // IAP, close also via ConMon
        mConMonWrapper->disconnectIap(iapId);
    } // else: just ignore call

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_DISCONNECTIAP_EXIT, this);
}

/*
   See WlanQtUtils::moveIapToInternetSnap().
*/

void WlanQtUtilsPrivate::moveIapToInternetSnap(int iapId)
{
    OstTraceFunctionEntry0(WLANQTUTILSPRIVATE_MOVEIAPTOINTERNETSNAP_ENTRY);

    mSettings->moveIapToInternetSnap(iapId);

    OstTraceFunctionExit0(WLANQTUTILSPRIVATE_MOVEIAPTOINTERNETSNAP_EXIT);
}

/*!
   See WlanQtUtils::connectionStatus().
*/

WlanQtUtils::ConnStatus WlanQtUtilsPrivate::connectionStatus() const
{
    OstTraceFunctionEntry0(WLANQTUTILSPRIVATE_CONNECTIONSTATUS_ENTRY);

    WlanQtUtils::ConnStatus status = WlanQtUtils::ConnStatusDisconnected;
    
    if (mConnection) {
        status = mConnection->connectionStatus();
    }
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_CONNECTIONSTATUS,
        "WlanQtUtilsPrivate::connectionStatus;status=%{ConnStatus};",
        (TUint)status);
    
    OstTraceFunctionExit0(WLANQTUTILSPRIVATE_CONNECTIONSTATUS_EXIT);
    return status;
}

/*!
   See WlanQtUtils::activeIap().
*/

int WlanQtUtilsPrivate::activeIap() const
{
    OstTraceFunctionEntry0(WLANQTUTILSPRIVATE_ACTIVEIAP_ENTRY);

    int iapId = WlanQtUtils::IapIdNone;
    
    if (mConnection) {
        iapId = mConnection->iapId();
    }
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_ACTIVEIAP,
        "WlanQtUtilsPrivate::activeIap;iapId=%d",
        iapId);
    
    OstTraceFunctionExit0(WLANQTUTILSPRIVATE_ACTIVEIAP_EXIT);
    return iapId;
}

/*!
   See WlanQtUtils::iapName().
*/

QString WlanQtUtilsPrivate::iapName(int iapId) const
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_IAPNAME_ENTRY, this);
    
    QString name;
    // Read the IAP from settings and return its name
    QSharedPointer<WlanQtUtilsAp> iap = mSettings->fetchIap(iapId);
    if (iap) {
        name = iap->value(WlanQtUtilsAp::ConfIdName).toString();
    }

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_IAPNAME_EXIT, this);
    return name;
}

/*!
    This function searches for a WLAN IAP matching the given WLAN AP.

    @param [in] list List to search from.
    @param [in] ap Access point to search for.
    
    @return True, if suitable WLAN IAP found, false otherwise.
*/

bool WlanQtUtilsPrivate::wlanIapExists(
    const QList< QSharedPointer<WlanQtUtilsAp> > list,
    const WlanQtUtilsAp *ap) const
{
    bool match = false;     // Return value
    
    foreach (QSharedPointer<WlanQtUtilsAp> iap, list) {
        if (WlanQtUtilsAp::compare(iap.data(), ap) == 0) {
            // Match found
            match = true;
            break;
        }
    }
    
    return match;
}

/*!
    Reports the scanning result to the client.
        
    @param [in] status Scan status code (WlanQtUtils::ScanStatus).
    @param [in] mode Mode of the scan whose status is reported.
*/

void WlanQtUtilsPrivate::reportScanResult(int status, int mode)
{
    switch (mode) {
    case ScanModeAvailableWlans:
        OstTrace1(
            TRACE_BORDER,
            WLANQTUTILSPRIVATE_WLANSCANREADY,
            "WlanQtUtilsPrivate::reportScanResult emit wlanScanReady;status=%{ScanStatus};",
            status);
        emit q_ptr->wlanScanReady(status);
        break;

    case ScanModeAvailableWlanAps:
        OstTrace1(
            TRACE_BORDER,
            WLANQTUTILSPRIVATE_WLANSCANAPREADY,
            "WlanQtUtilsPrivate::reportScanResult emit wlanScanApReady;status=%{ScanStatus};",
            status);
        emit q_ptr->wlanScanApReady(status);
        break;

    case ScanModeDirect:
        OstTrace1(
            TRACE_BORDER,
            WLANQTUTILSPRIVATE_WLANSCANDIRECTREADY,
            "WlanQtUtilsPrivate::reportScanResult emit wlanScanDirectReady;status=%{ScanStatus};",
            status);
        emit q_ptr->wlanScanDirectReady(status);
        break;

#ifndef QT_NO_DEBUG
    default:
        // Invalid scan mode detected
        Q_ASSERT(0);
        break;
#endif        
    }
}

/*!
    This function traces the given IAPs and APs.

    @param [in] iaps IAPs to trace.
    @param [in] aps APs to trace.
*/

void WlanQtUtilsPrivate::traceIapsAndAps(
    const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
    const QList< QSharedPointer<WlanQtUtilsAp> > &aps) const
{
#ifndef OST_TRACE_COMPILER_IN_USE
    Q_UNUSED(iaps);
    Q_UNUSED(aps);
#else
    foreach (QSharedPointer<WlanQtUtilsAp> iap, iaps) {
        QString tmp(iap->value(WlanQtUtilsAp::ConfIdSsid).toString());
        TPtrC16 name(tmp.utf16(), tmp.length());
        OstTraceExt3(
            TRACE_NORMAL,
            WLANQTUTILSPRIVATE_TRACEAVAILABLEWLANAPS_IAP,
            "WlanQtUtilsPrivate::traceAvailableWlanAps IAP;name=%S;secMode=%{WlanSecMode};signalStrength=%d",
            name,
            iap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(),
            iap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt());
    }
    foreach (QSharedPointer<WlanQtUtilsAp> ap, aps) {
        QString tmp(ap->value(WlanQtUtilsAp::ConfIdSsid).toString());
        TPtrC16 ssid(tmp.utf16(), tmp.length());
        OstTraceExt3(
            TRACE_NORMAL,
            WLANQTUTILSPRIVATE_TRACEAVAILABLEWLANAPS_AP,
            "WlanQtUtilsPrivate::traceAvailableWlanAps AP;ssid=%S;secMode=%{WlanSecMode};signalStrength=%d",
            ssid,
            ap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(),
            ap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt());
    }
#endif
}

/*!
    Slot for handling WLAN IAP scan result event from wrapper. Results are
    stored in a member variable.

    @param [in] availableIaps Available WLAN IAP's found in scan.
*/

void WlanQtUtilsPrivate::updateAvailableWlanIaps(
    QList< QSharedPointer<WlanQtUtilsAp> > &availableIaps)
{
    OstTraceFunctionEntry0(WLANQTUTILSPRIVATE_UPDATEAVAILABLEWLANIAPS_ENTRY);

    // Store the new IAP list
    mIapScanList = availableIaps;

    // Continue with AP scan (which should come immediately from WLAN's scan cache)
    mScanWrapper->scanWlanAps();
    
    OstTraceFunctionExit0(WLANQTUTILSPRIVATE_UPDATEAVAILABLEWLANIAPS_EXIT);
}

/*!
    Slot for handling WLAN AP scan result event from wrapper. Results are
    stored in member variable (possible duplicates are removed).

    @param [in] availableWlanList WLAN networks found in scan.
*/

void WlanQtUtilsPrivate::updateAvailableWlanAps(
    QList< QSharedPointer<WlanQtUtilsAp> > &availableWlanList)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATEAVAILABLEWLANAPS_ENTRY, this);

    // Old results are removed
    mWlanScanList.clear();
    
    // Copy available WLANs to scan result list (duplicates are removed)
    for (int i = 0; i < availableWlanList.count(); i++) {
        bool duplicate = false;
        for (int j = 0; j < mWlanScanList.count(); j++) {
            if (WlanQtUtilsAp::compare(
                availableWlanList[i].data(),
                mWlanScanList[j].data()) == 0) {
                duplicate = true;
                break;
            }
        }
        if (duplicate == false) {
            mWlanScanList.append(availableWlanList[i]);
        }
    }

    int mode = mScanMode;
    // Scan is complete
    mScanMode = ScanModeNone;

    // The information is forwarded to the client
    reportScanResult(WlanQtUtils::ScanStatusOk, mode);

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATEAVAILABLEWLANAPS_EXIT, this);
}

/*!
    Slot for handling WLAN scan failure event from wrapper. Result is
    reported to client.

    @param [in] status Scan status code (WlanQtUtils::ScanStatus).
*/

void WlanQtUtilsPrivate::updateScanFailed(int status)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATESCANFAILED_ENTRY, this);

    int mode = mScanMode;
    // Scan is now done
    mScanMode = ScanModeNone;
    reportScanResult(status, mode);
    
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATESCANFAILED_EXIT, this);
}

/*!
    Slot for handling connection setup status event from wrapper.

    @param [in] isOpened Was connection setup successful?
*/

void WlanQtUtilsPrivate::updateConnectionStatus(bool isOpened)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATECONNECTIONSTATUS_ENTRY, this);

    if (isOpened == false) {
        // Opening failed, update connection status and inform UI
        if (mConnection) {
            mConnection->setConnectionStatus(
                WlanQtUtils::ConnStatusDisconnected);
        }
        
        // Include the status code from connection wrapper
        int status = mEsockWrapper->lastStatusCode();
        OstTraceExt2(
            TRACE_BORDER,
            WLANQTUTILSPRIVATE_WLANNETWORKCLOSED,
            "WlanQtUtilsPrivate::emit wlanNetworkClosed;IAP ID=%d;status=%d",
            mConnectingIapId,
            status);
        emit q_ptr->wlanNetworkClosed(mConnectingIapId, status);
    } else {
        // Opening succeeded, update connection status and inform UI
        if (mConnection) {
            mConnection->setConnectionStatus(WlanQtUtils::ConnStatusConnected);
        }
        
        OstTrace1(
            TRACE_BORDER,
            WLANQTUTILSPRIVATE_WLANNETWORKOPENED,
            "WlanQtUtilsPrivate::emit wlanNetworkOpened;IAP ID=%d",
            mConnectingIapId);
        emit q_ptr->wlanNetworkOpened(mConnectingIapId);

        // Start ICT, if needed
        if (mConnectingIapId == mToBeTestedIapId) {
            QSharedPointer<WlanQtUtilsAp> iap(mSettings->fetchIap(mConnectingIapId));
            
            mIctService = QSharedPointer<IctsWlanLoginInterface>(
                new IctsWlanLoginInterface(this));
            
            // IctsWlanLoginInterface instance is created for each connectivity test
            // Note: Queued connection is required since mIctService is deleted
            // when signal comes
            bool connectStatus = connect(
                mIctService.data(),
                SIGNAL(ictsResult(int)),
                this,
                SLOT(updateIctResult(int)),
                Qt::QueuedConnection); 
            Q_ASSERT(connectStatus);            
            connectStatus = connect(
                mIctService.data(),
                SIGNAL(hotspotCase()),
                this,
                SLOT(updateIctHotspotCase()),
                Qt::QueuedConnection);
            Q_ASSERT(connectStatus);
            
            mIctService->start(
                mToBeTestedIapId,
                iap->value(WlanQtUtilsAp::ConfIdNetworkId).toInt());
        }
    }
    // IAP is no more in connecting state
    mConnectingIapId = WlanQtUtils::IapIdNone;

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATECONNECTIONSTATUS_EXIT, this);
}

/*!
    Slot for handling internet connectivity test result event from wrapper.
    Tested IAP is stored to Internet SNAP, if test was successful. If the
    IAP needs Hotspot authentication, it remains ungategorized.

    @param [in] result Result of internet connectivity test and hotspot
                       authentication (IctsWlanLoginInterface::ictsResultType).
*/

void WlanQtUtilsPrivate::updateIctResult(int ictsResult)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATEICTRESULT_ENTRY, this);

    Q_ASSERT(mToBeTestedIapId != WlanQtUtils::IapIdNone);
    WlanQtUtils::IctStatus result = WlanQtUtils::IctFailed;
    
    if (ictsResult == IctsWlanLoginInterface::IctsPassed) {
        // Move the tested IAP to Internet SNAP since the test passed
        Q_ASSERT(mToBeTestedIapId != WlanQtUtils::IapIdNone);
        mSettings->moveIapToInternetSnap(mToBeTestedIapId);
        result = WlanQtUtils::IctPassed;
    } else if (ictsResult == IctsWlanLoginInterface::IctsHotspotPassed) {
        Q_ASSERT(mToBeTestedIapId != WlanQtUtils::IapIdNone);
        result = WlanQtUtils::IctHotspotPassed;
    } else if (ictsResult == IctsWlanLoginInterface::IctsCanceled) {
        Q_ASSERT(mToBeTestedIapId != WlanQtUtils::IapIdNone);
        result = WlanQtUtils::IctCancelled;
    } else {
        // ICTS failed - IAP remains to be uncategorized and mIctService is deleted.
        mIctService.clear();
    }
    
    // Inform UI
    OstTraceExt2(
        TRACE_NORMAL,
        WLANQTUTILSPRIVATE_ICTRESULT,
        "WlanQtUtilsPrivate::emit ictResult;iapId=%d;result=%{IctStatus}",
        mToBeTestedIapId,
        result);

    emit q_ptr->ictResult(mToBeTestedIapId, result);

    // This IAP is now tested
    mToBeTestedIapId = WlanQtUtils::IapIdNone;

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATEICTRESULT_EXIT, this);
}

/*!
    Slot for setting IAP to be tested as hotspot IAP.
*/

void WlanQtUtilsPrivate::updateIctHotspotCase()
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATEICTHOTSPOTCASE_ENTRY, this);

    mSettings->setAsHotspotIap(mToBeTestedIapId);
  
    Q_ASSERT(mToBeTestedIapId != WlanQtUtils::IapIdNone);
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_UPDATEICTHOTSPOTCASE,
        "WlanQtUtilsPrivate::updateIctHotspotCase set as hotspot IAP;iapId=%d",
        mToBeTestedIapId);
        
    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATEICTHOTSPOTCASE_EXIT, this);
}

/*!
    Slot for updating active connection status from wrapper.

    @param [in] connectionId ID of the new connection.
*/

void WlanQtUtilsPrivate::addActiveConnection(uint connectionId)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_ADDACTIVECONNECTION_ENTRY, this);
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_ADDACTIVECONNECTION,
        "WlanQtUtilsPrivate::addActiveConnection;connectionId=%u",
        connectionId);

    Q_ASSERT(mConnection == NULL);
    mConnection = QSharedPointer<WlanQtUtilsConnection>(
        mConMonWrapper->connectionInfo(connectionId));

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_ADDACTIVECONNECTION_EXIT, this);
}

/*!
    Slot for updating active connection status from wrapper.

    @param [in] connectionId ID of the deleted connection.
*/

void WlanQtUtilsPrivate::removeActiveConnection(uint connectionId)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_REMOVEACTIVECONNECTION_ENTRY, this);
    OstTrace1(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_REMOVEACTIVECONNECTION,
        "WlanQtUtilsPrivate::removeActiveConnection;connectionId=%u",
        connectionId);

    Q_ASSERT(mConnection);
    if (mConnection->connectionId() == connectionId) {
        int closedIapId = mConnection->iapId();
        mConnection.clear();

        if (mConnectingIapId != closedIapId) {
            // Connection is closed, inform UI. wlanNetworkClosed is sent
            // from here instead of updateActiveConnection(closed),
            // because, in some cases, connection may be removed without
            // any connection status updates.
            // Note: reason parameter is not accurate here, because it is
            // only relevant for connections opened by this dll
            // (updateConnectionStatus)
            OstTraceExt2(
                TRACE_BORDER,
                DUP1_WLANQTUTILSPRIVATE_WLANNETWORKCLOSED,
                "WlanQtUtilsPrivate::emit wlanNetworkClosed;iapID=%d;status=%d",
                closedIapId,
                KErrNone);
            emit q_ptr->wlanNetworkClosed(closedIapId, KErrNone);
        }
        // else: connection creation started by thid dll, but creation failed
        // -> wlanNetworkClosed is sent from updateConnectionStatus
    }

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_REMOVEACTIVECONNECTION_EXIT, this);
}

/*!
    Slot for updating active connection status from wrapper.

    @param [in] connectionId ID of the updated connection.
    @param [in] connectionStatus New status of the connection.
*/

void WlanQtUtilsPrivate::updateActiveConnection(
    uint connectionId,
    WlanQtUtils::ConnStatus connectionStatus)
{
    OstTraceFunctionEntry1(WLANQTUTILSPRIVATE_UPDATEACTIVECONNECTION_ENTRY, this);
    OstTraceExt2(
        TRACE_BORDER,
        WLANQTUTILSPRIVATE_UPDATEACTIVECONNECTION,
        "WlanQtUtilsPrivate::updateActiveConnection;connectionId=%u;connectionStatus=%{ConnStatus}",
        connectionId,
        (uint)connectionStatus);

    Q_ASSERT(mConnection);
    if (mConnection->connectionId() == connectionId
        && mConnection->connectionStatus() != connectionStatus) {
        // Update connection status and inform UI, if necessary
        mConnection->setConnectionStatus(connectionStatus);
        if (connectionStatus == WlanQtUtils::ConnStatusConnected) {
            OstTrace1(
                TRACE_BORDER,
                DUP1_WLANQTUTILSPRIVATE_WLANNETWORKOPENED,
                "WlanQtUtilsPrivate::emit wlanNetworkOpened;iapId=%d",
                mConnection->iapId());
            emit q_ptr->wlanNetworkOpened(mConnection->iapId());
        }
        // Do not inform UI about connection closing here. It is done in
        // removeActiveConnection(), because that may occur without any
        // connection status updates.
    } // else: connection status did not change

    OstTraceFunctionExit1(WLANQTUTILSPRIVATE_UPDATEACTIVECONNECTION_EXIT, this);
}
