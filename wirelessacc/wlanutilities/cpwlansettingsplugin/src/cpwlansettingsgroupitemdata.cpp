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

#include <QStringList>
#include <cpsettingformitemdata.h>

// User includes

#include "cpwlansettingsgroupitemdata.h"
#include "wlansettings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlansettingsgroupitemdataTraces.h"
#endif

/*!
    \class CpWlanSettingsGroupItemData
    \brief CpWlanSettingsGroupItemData class creates all widgets required for Manage 
    WLAN Settings.
*/

// External function prototypes

// Local constants

//! To Enable the setting of a widget propery. */
static const int KEnableOption = 1;
//! Maximum widgets allowed for this Item Data. */
static const int KMaxWidgets = 4;
//! Index of Slider widget for Scan Interval. */
static const int KSliderIndex = 2;
//! Default value for user defined scan interval
static const int KDefaultUserDefinedScanInterval = 5;


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

CpWlanSettingsGroupItemData::CpWlanSettingsGroupItemData(
        CpItemDataHelper &itemDataHelper) :
    CpSettingFormItemData(HbDataFormModelItem::GroupItem, hbTrId(
            "txt_occ_subhead_wlan_settings")),
            mItemDataHelper(itemDataHelper)
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_CPWLANSETTINGSGROUPITEMDATA_ENTRY );

    initialise();
    createWlanSettingItems();

    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_CPWLANSETTINGSGROUPITEMDATA_EXIT );
}

/*!
    Destructor.
*/

CpWlanSettingsGroupItemData::~CpWlanSettingsGroupItemData()
{
    OstTraceFunctionEntry0( DUP1_CPWLANSETTINGSGROUPITEMDATA_CPWLANSETTINGSGROUPITEMDATA_ENTRY );
    OstTraceFunctionExit0( DUP1_CPWLANSETTINGSGROUPITEMDATA_CPWLANSETTINGSGROUPITEMDATA_EXIT );
}

/*!
    Initialises the member variables and reads the values of WLAN Settings.
*/

void CpWlanSettingsGroupItemData::initialise()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_INITIALISE_ENTRY );
    
    mWlanSettings.reset(new WlanSettings);
    
    bool connectStatus = connect(
        mWlanSettings.data(),
        SIGNAL(devicePowerSavingUpdated()),
        this,
        SLOT(devicePowerSavingChanged()));
    Q_ASSERT(connectStatus);

    //Error needs to be handled.
    int error = mWlanSettings->init();
    
    //Error needs to be handled.
    error = mWlanSettings->loadSettings();

    mUserDefinedScanInterval = KDefaultUserDefinedScanInterval;
    mUserDefinedItem = NULL;
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_INITIALISE_EXIT );
}

/*!
    Creates the Widgets for displaying WLAN Settings.
*/

void CpWlanSettingsGroupItemData::createWlanSettingItems()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_CREATEWLANSETTINGITEMS_ENTRY );
    
    mJoinWlanItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_join_wlan_networks"));
    // name the object for test automation purposes
    mJoinWlanItem->setObjectName("switchToWlan");
    
    QStringList joinWlanItems;
    
    joinWlanItems.append(
        hbTrId("txt_occ_setlabel_join_wlan_networks_val_known"));
    
    joinWlanItems.append(
        hbTrId("txt_occ_setlabel_join_wlan_networks_val_manual"));

    mJoinWlanItem->setContentWidgetData("items", joinWlanItems);

    mItemDataHelper.addConnection(
        mJoinWlanItem,
        SIGNAL(currentIndexChanged (int)),
        this,
        SLOT(joinWlanItemChanged (int)));

    mJoinWlanItem->setContentWidgetData(
        "currentIndex",
        mWlanSettings->joinWlanMode());

    this->appendChild(mJoinWlanItem);

    mScanNetworkItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_scan_for_networks"));
    // name the object for test automation purposes
    mScanNetworkItem->setObjectName("scanNetworkItem");
    
    QStringList scanNetworkItems;
    
    scanNetworkItems.append(
        hbTrId("txt_occ_setlabel_scan_for_networks_val_automatic"));
    
    scanNetworkItems.append(
        hbTrId("txt_occ_setlabel_scan_for_networks_val_userdefine"));

    mScanNetworkItem->setContentWidgetData("items", scanNetworkItems);

    mItemDataHelper.addConnection(
        mScanNetworkItem,
        SIGNAL(currentIndexChanged (const QString &)),
        this,
        SLOT(scanItemChanged (const QString &)));

    this->appendChild(mScanNetworkItem);

    WlanSettings::ScanNetworkType scanType = mWlanSettings->scanNetworkType();

    mScanNetworkItem->setContentWidgetData("currentIndex", scanType);

    //In case of PSM mode for device user-defined value is not 
    //allowed,make it automatic forcefully.
    if (mWlanSettings->isDevicePowerSavingEnabled()) {
        mScanNetworkItem->setContentWidgetData(
            "currentIndex",
            WlanSettings::EScanNetworkAuto);
        mScanNetworkItem->setContentWidgetData("enabled", false);
    } else if (scanType == WlanSettings::EScanNetworkUserDefined) {
        createScanIntervalItem();
    }
    
    mPowerSavingItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_power_saving"));
    // name the object for test automation purposes
    mPowerSavingItem->setObjectName("powerSaving");
    
    QStringList powerSavingItems;
    
    powerSavingItems.append(
        hbTrId("txt_occ_setlabel_power_saving_val_disabled"));
    
    powerSavingItems.append(
        hbTrId("txt_occ_setlabel_power_saving_val_enabled"));

    mPowerSavingItem->setContentWidgetData("items", powerSavingItems);

    mPowerSavingItem->setContentWidgetData(
        "currentIndex",
        mWlanSettings->isWlanPowerSavingEnabled());

    mItemDataHelper.addConnection(
        mPowerSavingItem,
        SIGNAL(currentIndexChanged (int)),
        this,
        SLOT(powerSavingItemChanged (int)));

    this->appendChild(mPowerSavingItem);

    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_CREATEWLANSETTINGITEMS_EXIT );
}

/*!
    Creates the slider widget for displaying scan interval value.
*/

void CpWlanSettingsGroupItemData::createScanIntervalItem()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_CREATESCANINTERVALITEM_ENTRY );
    
    //Add Scan Interval Slider, if it doesn't already exist.
    if (this->childCount() < KMaxWidgets) {
        mUserDefinedItem = new CpSettingFormItemData(
            HbDataFormModelItem::SliderItem,
            hbTrId("txt_occ_setlabel_scan_interval_minutes"));
    
        mUserDefinedItem->setContentWidgetData("maximum", WlanSettings::ScanNetworkMax);
        mUserDefinedItem->setContentWidgetData("minimum", WlanSettings::ScanNetworkMin);
        mUserDefinedItem->setContentWidgetData("toolTipVisible", KEnableOption);
        mUserDefinedItem->setContentWidgetData("tracking", KEnableOption);
    
        mItemDataHelper.addConnection(
            mUserDefinedItem,
            SIGNAL(valueChanged (int)),
            this,
            SLOT(scanValueChanged (int)));
        
        mItemDataHelper.addConnection(
            mUserDefinedItem,
            SIGNAL(sliderReleased ()),
            this,
            SLOT(scanSliderReleased ()));
        
        mItemDataHelper.addConnection(
            mUserDefinedItem,
            SIGNAL(sliderPressed ()),
            this,
            SLOT(scanSliderPressed ()));

        int interval = mWlanSettings->scanInterval();
        if (interval == WlanSettings::ScanNetworkAuto) {
            interval = mUserDefinedScanInterval;
        }
        mUserDefinedItem->setContentWidgetData("value", interval);
    
        this->insertChild(KSliderIndex, mUserDefinedItem);
    }

    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_CREATESCANINTERVALITEM_EXIT );
}

/*!
    Removes the slider widget, if it exists.
*/

void CpWlanSettingsGroupItemData::removeScanIntervalItem()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_REMOVESCANINTERVALITEM_ENTRY );
    
    if (KMaxWidgets == this->childCount()) {
        this->removeChild(KSliderIndex);
        mUserDefinedItem = NULL;    //removeChild() will delete the item.
    }

    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_REMOVESCANINTERVALITEM_EXIT );
}

/*!
    Slot for handling pressed signal of scan interval slider widget.
*/

void CpWlanSettingsGroupItemData::scanSliderPressed()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_SCANSLIDERPRESSED_ENTRY );

    int interval = mWlanSettings->scanInterval();
    if (interval == WlanSettings::ScanNetworkAuto) {
        interval = mUserDefinedScanInterval;
    }
    mUserDefinedItem->setContentWidgetData(
        "text",
        QString("%1").arg(interval));
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_SCANSLIDERPRESSED_EXIT );
}

/*!
    Slot for handling value changed signal of scan interval slider widget.
*/

void CpWlanSettingsGroupItemData::scanValueChanged(int value)
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_SCANVALUECHANGED_ENTRY );
    
    mUserDefinedItem->setContentWidgetData("text",QString("%1").arg(value));
    mUserDefinedScanInterval = value;
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_SCANVALUECHANGED_EXIT );
}

/*!
    Slot for handling Released signal of scan interval slider widget.
    Current value will be commited to database.
*/

void CpWlanSettingsGroupItemData::scanSliderReleased()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_SCANSLIDERRELEASED_ENTRY );
    
    mWlanSettings->setWlanScanInterval(mUserDefinedScanInterval);
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_SCANSLIDERRELEASED_EXIT );
}

/*!
    Slot for handling item changed signal of Scan for network combo box widget.
*/

void CpWlanSettingsGroupItemData::scanItemChanged(const QString &text)
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_SCANITEMCHANGED_ENTRY );
    
    if (hbTrId("txt_occ_setlabel_scan_for_networks_val_userdefine") == text) {
        // User defined mode
        mWlanSettings->setWlanScanInterval(mUserDefinedScanInterval);
        createScanIntervalItem();
    } else {
        // Automatic mode
        removeScanIntervalItem();
        mWlanSettings->setWlanScanInterval(WlanSettings::ScanNetworkAuto);
    }
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_SCANITEMCHANGED_EXIT );
}

/*!
    Slot for handling item changed signal of Power saving combo box widget.
*/

void CpWlanSettingsGroupItemData::powerSavingItemChanged(int index)
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_POWERSAVINGITEMCHANGED_ENTRY );
    
    mWlanSettings->setWlanPowerSaving(index);
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_POWERSAVINGITEMCHANGED_EXIT );
}

/*!
    Slot for handling item changed signal of Join WLAN networks combo box widget.
*/

void CpWlanSettingsGroupItemData::joinWlanItemChanged(int index)
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_JOINWLANITEMCHANGED_ENTRY );
    
    mWlanSettings->setJoinWlanMode(index);
    
    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_JOINWLANITEMCHANGED_EXIT );
}

/*!
    Slot for handling device power saving mode update signal.
*/

void CpWlanSettingsGroupItemData::devicePowerSavingChanged()
{
    OstTraceFunctionEntry0( CPWLANSETTINGSGROUPITEMDATA_DEVICEPOWERSAVINGCHANGED_ENTRY );
    
    // Disconnect the handling of currentIndexChanged, because
    // we will programmatically update the index in this function.
    mItemDataHelper.removeConnection(
        mScanNetworkItem,
        SIGNAL(currentIndexChanged (const QString &)),
        this,
        SLOT(scanItemChanged (const QString &)));
    
    if (mWlanSettings->isDevicePowerSavingEnabled()) {
        // If device power saving mode is activated, force the scan
        // setting to automatic, and disable the setting combo box
        mScanNetworkItem->setContentWidgetData(
            "currentIndex",
            WlanSettings::EScanNetworkAuto);
        mScanNetworkItem->setContentWidgetData("enabled", false);
        removeScanIntervalItem();
    } else {
        // If device power saving mode is deactivated, enable the
        // setting combo box, and make sure the setting is in the
        // same mode it was before power saving mode was activated
        mScanNetworkItem->setContentWidgetData("enabled", true);
        if (mWlanSettings->scanNetworkType() ==
            WlanSettings::EScanNetworkUserDefined) {
            mScanNetworkItem->setContentWidgetData(
                "currentIndex",
                WlanSettings::EScanNetworkUserDefined);
            createScanIntervalItem();
        }
    }

    // Reconnect the handling of currentIndexChanged since we are
    // done with the updates.
    mItemDataHelper.addConnection(
        mScanNetworkItem,
        SIGNAL(currentIndexChanged (const QString &)),
        this,
        SLOT(scanItemChanged (const QString &)));

    OstTraceFunctionExit0( CPWLANSETTINGSGROUPITEMDATA_DEVICEPOWERSAVINGCHANGED_EXIT );
}
