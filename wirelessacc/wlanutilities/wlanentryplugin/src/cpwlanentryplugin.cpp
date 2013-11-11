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
* WLAN Entry plugin implementation.
*/

// System includes

#include <QApplication>
#include <HbTranslator>
#include "qsysteminfo.h"

// User includes

#include "cpwlanentryitemdata.h"
#include "cpwlanentryplugin.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlanentrypluginTraces.h"
#endif

using namespace QtMobility;

/*!
    \class CpWlanEntryPlugin
    \brief CpWlanEntryPlugin implements the WLAN Status Control Panel Plugin
    that shows the current WLAN status.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

CpWlanEntryPlugin::CpWlanEntryPlugin() :
    QObject(),
    mWlanSupported(false),
    mTranslator()
{
    OstTraceFunctionEntry0(CPWLANENTRYPLUGIN_CPWLANENTRYPLUGIN_ENTRY);
    
    // Check & store WLAN dynamic configuration
    QtMobility::QSystemInfo sysinfo;
    mWlanSupported = sysinfo.hasFeatureSupported(QSystemInfo::WlanFeature);
    
    if (mWlanSupported) {
        // Install localization
        mTranslator = QSharedPointer<HbTranslator>(
            new HbTranslator("wlanentryplugin"));
    }
    
    OstTraceFunctionExit0(CPWLANENTRYPLUGIN_CPWLANENTRYPLUGIN_EXIT);
}

/*!
    Destructor.
*/

CpWlanEntryPlugin::~CpWlanEntryPlugin()
{
    OstTraceFunctionEntry0(DUP1_CPWLANENTRYPLUGIN_CPWLANENTRYPLUGIN_ENTRY);
    OstTraceFunctionExit0(DUP1_CPWLANENTRYPLUGIN_CPWLANENTRYPLUGIN_EXIT);
}

/*!
    Plugin function for creating the entry plugin form item data.
    
    @param [in,out] itemDataHelper Control Panel item data helper.
*/

QList<CpSettingFormItemData *> CpWlanEntryPlugin::createSettingFormItemData(
    CpItemDataHelper &itemDataHelper) const
{
    OstTraceFunctionEntry0(CPWLANENTRYPLUGIN_CREATESETTINGFORMITEMDATA_ENTRY);
    
    QList<CpSettingFormItemData *> settingFormData;
    
    // The plugin is disabled if WLAN is not supported by the product.
    if (mWlanSupported) {
        settingFormData.append(new CpWlanEntryItemData(itemDataHelper));
    }

    OstTraceFunctionExit0(CPWLANENTRYPLUGIN_CREATESETTINGFORMITEMDATA_EXIT);
    return settingFormData;
}

Q_EXPORT_PLUGIN2(cpwlanentryplugin, CpWlanEntryPlugin);
