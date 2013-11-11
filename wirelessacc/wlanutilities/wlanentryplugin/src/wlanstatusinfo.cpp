/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Status Info wrapper for WLAN entry plugin.
*/

// System includes

#include <HbGlobal>
#include <xqsettingsmanager.h>
#include <wlandevicesettingsinternalcrkeys.h>

// User includes

#include "wlanqtutils.h"

#include "wlanstatusinfo.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanstatusinfoTraces.h"
#endif

/*!
    \class WlanStatusInfo
    \brief Class for maintaining and updating the WLAN status for the WLAN 
           Status Control Panel Plugin.
*/

// External function prototypes

// Local constants

//! Master WLAN ON/OFF setting key
static const XQSettingsKey masterWlanKey(
    XQSettingsKey::TargetCentralRepository,
    KCRUidWlanDeviceSettingsRegistryId.iUid, 
    KWlanOnOff);
    
//! WLAN  forced OFF setting key
static const XQSettingsKey wlanForceKey(
    XQSettingsKey::TargetCentralRepository,
    KCRUidWlanDeviceSettingsRegistryId.iUid, 
    KWlanForceDisable);

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

WlanStatusInfo::WlanStatusInfo(QObject *parent) :
    QObject(parent),
    mWlanQtUtils(new WlanQtUtils()),
    mSettingsManager(new XQSettingsManager(this)),
    mStatusText(""),
    mStatus(WlanStatusOff)
{
    OstTraceFunctionEntry0(WLANSTATUSINFO_WLANSTATUSINFO_ENTRY);
    
    // Listen for WLAN ON/OFF switching
    bool connectStatus = connect(
        mSettingsManager, 
        SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, 
        SLOT(updateStatus()));
    Q_ASSERT(connectStatus);
    mSettingsManager->startMonitoring(masterWlanKey);
    mSettingsManager->startMonitoring(wlanForceKey);

    // Listen for WLAN connection statuses
    connectStatus = connect(
        mWlanQtUtils,
        SIGNAL(wlanNetworkOpened(int)),
        this,
        SLOT(updateStatus()));
    Q_ASSERT(connectStatus == true);
    connectStatus = connect(
        mWlanQtUtils,
        SIGNAL(wlanNetworkClosed(int, int)),
        this,
        SLOT(updateStatus()));
    Q_ASSERT(connectStatus == true);
    
    // Set initial status
    updateStatus();
    
    OstTraceFunctionExit0(WLANSTATUSINFO_WLANSTATUSINFO_EXIT);
}

/*!
    Destructor.
*/

WlanStatusInfo::~WlanStatusInfo()
{
    OstTraceFunctionEntry0(DUP1_WLANSTATUSINFO_WLANSTATUSINFO_ENTRY);
    
    delete mWlanQtUtils;
    
    OstTraceFunctionExit0(DUP1_WLANSTATUSINFO_WLANSTATUSINFO_EXIT);
}

/*!
    Function for getting current WLAN status value (WlanStatusInfo::WlanStatus*).
*/

int WlanStatusInfo::status() const
{
    OstTraceFunctionEntry0(WLANSTATUSINFO_STATUS_ENTRY);    
    OstTraceFunctionExit0(WLANSTATUSINFO_STATUS_EXIT);
    return mStatus;
}

/*!
    Returns the current WLAN status text.
*/

QString WlanStatusInfo::statusText() const
{
    OstTraceFunctionEntry0(WLANSTATUSINFO_STATUSTEXT_ENTRY);
    OstTraceFunctionExit0(WLANSTATUSINFO_STATUSTEXT_EXIT);
    return mStatusText;
}

/*!
    Function for checking if WLAN is ON.
    WLAN is enabled if the WLAN OnOff key is true and the force disable
    wlan key is false.
    
    @return Returns true if WLAN is ON.
*/

bool WlanStatusInfo::isWlanOn() const
{
    OstTraceFunctionEntry0(WLANSTATUSINFO_ISWLANON_ENTRY);

    bool wlanOn = mSettingsManager->readItemValue(masterWlanKey).toBool();
    bool forcedOff = mSettingsManager->readItemValue(wlanForceKey).toBool();

    OstTraceExt2(
        TRACE_NORMAL,
        WLANSTATUSINFO_ISWLANON,
        "WlanStatusInfo::isWlanOn;wlanOn=%hhu;forcedOff=%hhu",
        wlanOn,
        forcedOff);
    
    OstTraceFunctionExit0(WLANSTATUSINFO_ISWLANON_EXIT);
    return wlanOn && !forcedOff;
}

/*!
    Slot used for updating the WLAN status.
*/

void WlanStatusInfo::updateStatus()
{
    OstTraceFunctionEntry0(WLANSTATUSINFO_UPDATESTATUS_ENTRY);

    // Backup old status to detect changes
    QString oldStatusText = mStatusText;

    // Figure out current WLAN status
    if (!isWlanOn()) {
        // WLAN is switched OFF.
        mStatus = WlanStatusOff;
        mStatusText = hbTrId("txt_occ_dblist_wireless_lan_val_off");
    } else if (mWlanQtUtils->connectionStatus() == WlanQtUtils::ConnStatusConnected) { 
        mStatus = WlanStatusConnected;
        mStatusText = mWlanQtUtils->iapName(mWlanQtUtils->activeIap());
    } else {
        mStatus = WlanStatusIdle;
        mStatusText = hbTrId("txt_occ_dblist_wireless_lan_val_wlan_is_on");
    }

    // Inform about update if the status really changed.
    if (oldStatusText != mStatusText) {
        emit statusUpdated();
    }
    
    OstTraceFunctionExit0(WLANSTATUSINFO_UPDATESTATUS_EXIT);
}
