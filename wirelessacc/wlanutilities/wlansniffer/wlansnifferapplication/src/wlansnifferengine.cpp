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
* WLAN Sniffer application engine. 
*/

// System includes

#include <QTimerEvent>

#include <xqserviceutil.h>
#include <xqsettingsmanager.h>
#include <wlandevicesettingsinternalcrkeys.h>
#include <startupdomainpskeys.h>

// User includes

#include "wlanqtutils.h"
#include "wlansnifferservice.h"
#include "wlansnifferengine.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansnifferengineTraces.h"
#endif

/*!
    \class WlanSnifferEngine
    \brief WLAN Sniffer application engine.

    This class implements the WLAN Sniffer application engine.
    The purpose of the engine implementation is to gather all non-UI
    implementation base functionality into one place that can be utilized
    from multiple places (i.e. multiple views and windows).  
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

//! Offline mode setting key
static const XQSettingsKey offlineKey(
    XQSettingsKey::TargetPublishAndSubscribe,
    KPSUidStartup.iUid, 
    KPSGlobalSystemState);

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] parent Parent object.
 */

WlanSnifferEngine::WlanSnifferEngine(QObject *parent) :
    QObject(parent),
    mService(new WlanSnifferService(this)),
    mSettingsManager(new XQSettingsManager(this)),
    mScanTimerId(0),
    mScanEnabled(false),
    mEmbedded(false),
    mWlanQtUtils(new WlanQtUtils())
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_WLANSNIFFERENGINE_ENTRY);

    // Subscribe for WLAN status change indications
    bool connectStatus = connect(
        mSettingsManager, 
        SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, 
        SLOT(updateSetting(XQSettingsKey, QVariant)));
    Q_ASSERT(connectStatus);
    mSettingsManager->startMonitoring(masterWlanKey);
    mSettingsManager->startMonitoring(wlanForceKey);

    // Connect WLAN Sniffer service signals
    connectStatus = connect(
        mService,
        SIGNAL(toListView(QString)),
        this,
        SIGNAL(toListView(QString)));
    Q_ASSERT(connectStatus == true);
    connectStatus = connect(
        mService,
        SIGNAL(returnValueDelivered()),
        this,
        SIGNAL(exitTriggered()));    
    Q_ASSERT(connectStatus == true);
    
    // Store WLAN Sniffer service embedded status
    mEmbedded = XQServiceUtil::isEmbedded();
    
    OstTraceFunctionExit0(WLANSNIFFERENGINE_WLANSNIFFERENGINE_EXIT);
}

/*!
    Destructor.
 */

WlanSnifferEngine::~WlanSnifferEngine()
{
    OstTraceFunctionEntry0(DUP1_WLANSNIFFERENGINE_WLANSNIFFERENGINE_ENTRY);
    OstTraceFunctionExit0(DUP1_WLANSNIFFERENGINE_WLANSNIFFERENGINE_EXIT);
}

/*!
    Getter function for WLAN Qt Utilities instance owned by
    this class. The whole WLAN Sniffer application uses the same
    instance, and thus the reference is needed also in other classes.
    
    @return WLAN Qt Utilities object.
 */

WlanQtUtils *WlanSnifferEngine::wlanQtUtils() const
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_WLANQTUTILS_ENTRY);
    
    // The reference must never be null.
    Q_ASSERT(mWlanQtUtils.data());
    
    OstTraceFunctionExit0(WLANSNIFFERENGINE_WLANQTUTILS_EXIT);
    return mWlanQtUtils.data();
}

/*!
    Getter for WLAN Sniffer service embedded property.
    
    @return TRUE if WLAN Sniffer is used as an embedded service.
 */

bool WlanSnifferEngine::isEmbedded() const
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_ISEMBEDDED_ENTRY);
    OstTraceFunctionExit0(WLANSNIFFERENGINE_ISEMBEDDED_EXIT);
    return mEmbedded;
}

/*!
    Starts periodic WLAN Scanning.
    It is allowed to call this function also when the scanning is already ON.
    If so, the timer is not restarted but the scan period stays untouched. 
 */

void WlanSnifferEngine::startWlanScanning()
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_STARTWLANSCANNING_ENTRY);

    if (!mScanEnabled) {
        OstTrace0(
            TRACE_NORMAL,
            WLANSNIFFERENGINE_STARTWLANSCANNING,
            "WlanSnifferEngine::startWlanScanning Periodic WLAN scanning starting");

        mScanEnabled = true;
        // Connect response signal
        bool connectStatus = connect(
            mWlanQtUtils.data(),
            SIGNAL(wlanScanReady(int)),
            this,
            SLOT(handleWlanScanReady(int)));
        Q_ASSERT(connectStatus);

        // Start the first scan. Scan timer is started when scan result
        // signal is received.
        mWlanQtUtils->scanWlans();
    }
    
    OstTraceFunctionExit0(WLANSNIFFERENGINE_STARTWLANSCANNING_EXIT);
}

/*!
    Stops periodic WLAN Scanning.
    It is allowed to call this function also when periodic scanning is OFF.
 */

void WlanSnifferEngine::stopWlanScanning()
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_STOPWLANSCANNING_ENTRY);
    
    if (mScanEnabled) {
        OstTrace0(
            TRACE_NORMAL,
            WLANSNIFFERENGINE_STOPWLANSCANNING,
            "WlanSnifferEngine::stopWlanScanning Periodic WLAN scanning stopped");

        mScanEnabled = false;
        // Disconnect response signal
        disconnect(
            mWlanQtUtils.data(),
            SIGNAL(wlanScanReady(int)),
            this,
            SLOT(handleWlanScanReady(int)));

        // Stop the scan
        mWlanQtUtils->stopWlanScan();

        // Stop periodic scan timer
        if (mScanTimerId != 0) {
            killTimer(mScanTimerId);
            mScanTimerId = 0;
        }
    }
    
    OstTraceFunctionExit0(WLANSNIFFERENGINE_STOPWLANSCANNING_EXIT);
}

/*!
    Function for getting the master WLAN status.

    @return Master WLAN status: true if enabled, otherwise false.
*/

bool WlanSnifferEngine::masterWlan() const
{
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_MASTERWLAN_ENTRY, this);

    // Read WLAN ON/OFF setting from Cenrep
    int wlanEnabled = mSettingsManager->readItemValue(masterWlanKey).toInt();

    OstTrace1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_MASTERWLAN,
        "WlanSnifferEngine::masterWlan;enabled=%d",
        wlanEnabled);

    OstTraceFunctionExit1(WLANSNIFFERENGINE_MASTERWLAN_EXIT, this);
    return wlanEnabled ? true : false;
}

/*!
    Function for switching the master WLAN status ON or OFF.

    @param [in] enabled If set to true, WLAN is switched ON, and vice versa.
*/

void WlanSnifferEngine::setMasterWlan(bool enabled)
{
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_SETMASTERWLAN_ENTRY, this);

    OstTraceExt1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_SETMASTERWLAN,
        "WlanSnifferEngine::setMasterWlan;enabled=%hhu",
        enabled);
    
    // Store WLAN ON/OFF setting to Cenrep
    int wlanEnabled = enabled ? 1 : 0;
    bool writeStatus = mSettingsManager->writeItemValue(masterWlanKey, wlanEnabled);
    Q_ASSERT(writeStatus);

    OstTraceFunctionExit1(WLANSNIFFERENGINE_SETMASTERWLAN_EXIT, this);
}

/*!
    Function for getting the force disable WLAN status.
     
    @return forced WLAN status: true if disabled, otherwise false.
*/

bool WlanSnifferEngine::forceDisableWlan() const
{
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_FORCEDISABLEWLAN_ENTRY, this);
    
    // Read force disable WLAN ON/OFF setting from Cenrep
    int wlanDisabled = mSettingsManager->readItemValue(wlanForceKey).toInt();

    OstTrace1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_FORCEDISABLEWLAN,
        "WlanSnifferEngine::forceDisableWlan;wlanDisabled=%d",
        wlanDisabled);

    OstTraceFunctionEntry1(WLANSNIFFERENGINE_FORCEDISABLEWLAN_EXIT, this);
    return wlanDisabled ? true : false;
}

/*!
    Function for getting the offline mode status.

    @return Offline mode status: true if offline, otherwise false.
*/

bool WlanSnifferEngine::offlineMode() const
{
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_OFFLINEMODE_ENTRY, this);
    
    // Read offline mode setting from Cenrep
    int systemState = mSettingsManager->readItemValue(offlineKey).toInt();

    bool offline = false;
    if (ESwStateNormalRfOff == systemState) {
        offline = true;
    }
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_OFFLINEMODE,
        "WlanSnifferEngine::offlineMode;offline=%hhu",
        offline);

    OstTraceFunctionExit1(WLANSNIFFERENGINE_OFFLINEMODE_EXIT, this);
    return offline; 
}

/*!
    Function for completing the running WLAN Sniffer service.
    This function must only be ran if a WLAN Sniffer service is running. 
 */

void WlanSnifferEngine::completeService()
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_COMPLETESERVICE_ENTRY);
    
    mService->complete();

    OstTraceFunctionExit0(WLANSNIFFERENGINE_COMPLETESERVICE_EXIT);
}

/*!
    This function (from QObject) handles timers. Initiates a new WLAN scan.

    @param [in] event Timer event.
 */

void WlanSnifferEngine::timerEvent(QTimerEvent *event)
{
    OstTraceFunctionEntry0(WLANSNIFFERENGINE_TIMEREVENT_ENTRY);
    
    int timerId = event->timerId();
        
    OstTrace1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_TIMEREVENT,
        "WlanSnifferEngine::timerEvent;timerId=%d",
        timerId);
    
    // WLAN scan timer is the only used timer
    Q_ASSERT(timerId == mScanTimerId);
    
    // Request a new scan. Timer events come periodically.
    mWlanQtUtils->scanWlans();
    
    OstTraceFunctionExit0(WLANSNIFFERENGINE_TIMEREVENT_EXIT);
}

/*!
    Slot for updating settings.

    @param [in] key The changed key setting.
    @param [in] value The new value of the setting.
*/

void WlanSnifferEngine::updateSetting(
    const XQSettingsKey &key,
    const QVariant &value)
{   
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_UPDATESETTING_ENTRY, this);
    
    // Check that the key is WLAN ON/OFF, or wlanForceDisable since 
    // they are the only supported keys
    Q_ASSERT(
        key.target() == masterWlanKey.target() ||
        key.target() == wlanForceKey.target());
    Q_ASSERT(
        key.uid() == masterWlanKey.uid() ||
        key.uid() == wlanForceKey.uid());
    Q_ASSERT(
        key.key() == masterWlanKey.key() ||
        key.key() == wlanForceKey.key());

    bool ok;
    bool boolean = value.toInt(&ok) ? true : false;
    Q_ASSERT(ok);
    
    // The updated key can be either the KWlanOnOff or the KWlanForceDisable
    if (key.key() == wlanForceKey.key()) {
        // Inform about WlanForceDisable status change
        OstTraceExt1(
            TRACE_NORMAL,
            WLANSNIFFERENGINE_UPDATESETTING_WLANFORCEDISABLE,
            "WlanSnifferEngine::emit forceWlanStatus;forcedWlanDisabled=%hhu",
            boolean);
        emit forceDisableWlanStatus(boolean);
    } else {
        // Inform about WLAN ON/OFF status change
        OstTraceExt1(
            TRACE_NORMAL,
            WLANSNIFFERENGINE_UPDATESETTING_WLANONOFF,
            "WlanSnifferEngine::emit masterWlanStatus;wlanEnabled=%hhu",
            boolean);
        emit masterWlanStatus(boolean);
    }
    
    OstTraceFunctionExit1(WLANSNIFFERENGINE_UPDATESETTING_EXIT, this);
}

/*!
    Slot for handling Wlan scan result.

    @param [in] status Scan status code.
*/

void WlanSnifferEngine::handleWlanScanReady(int status)
{   
    OstTraceFunctionEntry1(WLANSNIFFERENGINE_HANDLEWLANSCANREADY_ENTRY, this);

    OstTrace1(
        TRACE_NORMAL,
        WLANSNIFFERENGINE_HANDLEWLANSCANREADY,
        "WlanSnifferEngine::handleWlanScanReady;status=%d",
        status);
    
    // Forward result signal only, if there was no error
    if (status == WlanQtUtils::ScanStatusOk) {
        emit wlanScanReady();
    }
    
    // Start timer for the next scan, if not running already
    if (mScanTimerId == 0) {
        mScanTimerId = startTimer(scanTimerInterval);
        // The timer start must succeed
        Q_ASSERT(mScanTimerId != 0);
    }
    
    OstTraceFunctionExit1(WLANSNIFFERENGINE_HANDLEWLANSCANREADY_EXIT, this);
}
