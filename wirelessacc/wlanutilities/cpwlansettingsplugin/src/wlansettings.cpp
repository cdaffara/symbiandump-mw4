/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

// System includes

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <psmsrvdomaincrkeys.h>
#include <psmtypes.h>

// User includes

#include "wlansettings_s60_p.h"
#include "wlansettings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansettingsTraces.h"
#endif

/*!
   \class WlanSettings
   \brief WlanSettings is a wrapper class for CWlanSettingsImpl.
*/

// External function prototypes

// Local constants

//! Device Power Saving Mode setting key
static const XQSettingsKey devicePowerSavingKey(
    XQSettingsKey::TargetCentralRepository,
    KCRUidPowerSaveMode.iUid, 
    KPsmCurrentMode);
    
// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

WlanSettings::WlanSettings() :
    QObject(),
    mSettingsManager(new XQSettingsManager(this)),
    mDevicePowerSavingMode(0),
    mDevicePowerSavingModeUpToDate(false)
{
    OstTraceFunctionEntry0( WLANSETTINGS_WLANSETTINGS_ENTRY );
    OstTraceFunctionExit0( WLANSETTINGS_WLANSETTINGS_EXIT );
}

/*!
    Destructor.
*/

WlanSettings::~WlanSettings()
{
    OstTraceFunctionEntry0( DUP1_WLANSETTINGS_WLANSETTINGS_ENTRY );

    delete d_ptr;
    
    OstTraceFunctionExit0( DUP1_WLANSETTINGS_WLANSETTINGS_EXIT );
}

/*!
    Creates the CWlanSettingsImpl object for reading/writing settings.
    \return Error code.
*/

int WlanSettings::init()
{
    OstTraceFunctionEntry0( WLANSETTINGS_INIT_ENTRY );
    
    // Listen for changes in the key value
    bool connectStatus = connect(
        mSettingsManager,
        SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this,
        SLOT(devicePowerSavingKeyChanged()));
    Q_ASSERT(connectStatus);
    mSettingsManager->startMonitoring(devicePowerSavingKey);
    
    TRAPD(error,(d_ptr = CWlanSettingsPrivate::NewL(this)));
    
    OstTraceFunctionExit0( WLANSETTINGS_INIT_EXIT );
    return error;
}

/*!
    Reads the value of the Device Power Saving Mode setting.
*/

void WlanSettings::readDevicePowerSavingKey()
{
    OstTraceFunctionEntry0( WLANSETTINGS_READDEVICEPOWERSAVINGKEY_ENTRY );
    
    mDevicePowerSavingMode = 
        mSettingsManager->readItemValue(devicePowerSavingKey).toInt();

    mDevicePowerSavingModeUpToDate = true;
    
    OstTrace1(
        TRACE_NORMAL,
        WLANSETTINGS_READDEVICEPOWERSAVINGKEY,
        "WlanSettings::readDevicePowerSavingKey;powerSaving=%d",
        mDevicePowerSavingMode );

    OstTraceFunctionExit0( WLANSETTINGS_READDEVICEPOWERSAVINGKEY_EXIT );
}

/*!
    Slot for handling updates in the Device Power Saving Mode setting.
*/

void WlanSettings::devicePowerSavingKeyChanged()
{
    OstTraceFunctionEntry0( WLANSETTINGS_DEVICEPOWERSAVINGKEYCHANGED_ENTRY );
    
    // Remember that we need to read the setting value again before
    // using it, and notify UI of the change.
    mDevicePowerSavingModeUpToDate = false;
    emit devicePowerSavingUpdated();
    
    OstTraceFunctionExit0( WLANSETTINGS_DEVICEPOWERSAVINGKEYCHANGED_EXIT );
}

/*!
    Reads the WLAN setting values.
    \return Error code.
*/

int WlanSettings::loadSettings()
{
    OstTraceFunctionEntry0( WLANSETTINGS_LOADSETTINGS_ENTRY );
    
    TRAPD(error, d_ptr->LoadDBSettingsL());
    
    OstTrace1(
        TRACE_NORMAL,
        WLANSETTINGS_LOADSETTINGS,
        "WlanSettings::loadSettings;error=%d",
        error );
    
    OstTraceFunctionExit0( WLANSETTINGS_LOADSETTINGS_EXIT );
    return error;
}

/*!
    Function to get the scan network type.
    \return ScanNetworkType which is automatic or user-defined.
*/

WlanSettings::ScanNetworkType WlanSettings::scanNetworkType()
{
    OstTraceFunctionEntry0( WLANSETTINGS_SCANNETWORKTYPE_ENTRY );
    
    ScanNetworkType scanType;
    if (d_ptr->ScanInterval() == ScanNetworkAuto) {
        scanType = EScanNetworkAuto;
    } else {
        scanType = EScanNetworkUserDefined;
    }
    
    OstTrace1(
        TRACE_NORMAL,
        WLANSETTINGS_SCANNETWORKTYPE,
        "WlanSettings::scanNetworkType;scanType=%d",
        scanType );
    
    OstTraceFunctionExit0( WLANSETTINGS_SCANNETWORKTYPE_EXIT );
    return scanType;
}

/*!
    Function to get the scan interval.
    \return Scan interval  in minutes.
*/

uint WlanSettings::scanInterval()
{
    OstTraceFunctionEntry0( WLANSETTINGS_SCANINTERVAL_ENTRY );
    
    uint scanInterval = d_ptr->ScanInterval();
    
    OstTrace1(
        TRACE_NORMAL,
        WLANSETTINGS_SCANINTERVAL,
        "WlanSettings::scanInterval;scanInterval=%u",
        scanInterval );
    
    OstTraceFunctionExit0( WLANSETTINGS_SCANINTERVAL_EXIT );
    return scanInterval;
}

/*!
    Function to get Wlan Power Saving Option.
    \return True if Power Saving option is enabled, otherwise False.
*/

bool WlanSettings::isWlanPowerSavingEnabled() const
{
    OstTraceFunctionEntry0( WLANSETTINGS_ISWLANPOWERSAVINGENABLED_ENTRY );
    OstTraceFunctionExit0( WLANSETTINGS_ISWLANPOWERSAVINGENABLED_EXIT );
    return d_ptr->PowerSaving();
}

/*!
    Function to set Power Saving Option.
    \param powerSavingOption True to enable or false to disable power saving option. 
    \return Error code.
*/

int WlanSettings::setWlanPowerSaving(int powerSavingOption)
{
    OstTraceFunctionEntry0( WLANSETTINGS_SETWLANPOWERSAVING_ENTRY );
    
    d_ptr->SetPowerSaving(powerSavingOption);
    TRAPD(error, d_ptr->SaveDBSettingsL(CWlanSettingsPrivate::EWlanPowerSaving));

    OstTraceFunctionExit0( WLANSETTINGS_SETWLANPOWERSAVING_EXIT );
    return error;
}

/*!
    Function to set Scan interval.
    \param scanInterval Scan interval in minutes. 
    \return Error code.
*/

int WlanSettings::setWlanScanInterval(uint scanInterval)
{
    OstTraceFunctionEntry0( WLANSETTINGS_SETWLANSCANINTERVAL_ENTRY );
    
    d_ptr->SetScanInterval(scanInterval);
    TRAPD(error, d_ptr->SaveDBSettingsL(CWlanSettingsPrivate::EWlanScanInterval));

    OstTraceFunctionExit0( WLANSETTINGS_SETWLANSCANINTERVAL_EXIT );
    return error;
}

/*!
    Function to get Join WLAN networks option.
    \return 0 for Known, 1 for Manual.
*/

int WlanSettings::joinWlanMode() const
{
    OstTraceFunctionEntry0( WLANSETTINGS_JOINWLANMODE_ENTRY );
    OstTraceFunctionExit0( WLANSETTINGS_JOINWLANMODE_EXIT );
    return d_ptr->JoinWlanMode();
}

/*!
    Function to set Join WLAN networks option.
    \param mode 0 for Known, 1 for Manual. 
    \return Error code.
*/

int WlanSettings::setJoinWlanMode(int mode)
{
    OstTraceFunctionEntry0( WLANSETTINGS_SETJOINWLANMODE_ENTRY );
    
    TRAPD(error, d_ptr->SaveJoinWlanSettingL(mode));
    
    OstTraceFunctionExit0( WLANSETTINGS_SETJOINWLANMODE_EXIT );
    return error;
}

/*!
    Function to get Power Saving status of the device.
    \return True if Power Saving is enabled.
*/

bool WlanSettings::isDevicePowerSavingEnabled()
{
    OstTraceFunctionEntry0( WLANSETTINGS_ISDEVICEPOWERSAVINGENABLED_ENTRY );
    
    if (!mDevicePowerSavingModeUpToDate) {
        readDevicePowerSavingKey();
    }
    if (mDevicePowerSavingMode == EPsmsrvModeNormal) {
        OstTraceFunctionExit0( WLANSETTINGS_ISDEVICEPOWERSAVINGENABLED_EXIT );
        return false;
    } else {
        OstTraceFunctionExit0( DUP1_WLANSETTINGS_ISDEVICEPOWERSAVINGENABLED_EXIT );
        return true;
    }
}
