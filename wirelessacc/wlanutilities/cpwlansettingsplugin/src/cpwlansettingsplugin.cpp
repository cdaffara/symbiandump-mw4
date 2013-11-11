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

#include <HbTranslator>
#include <QScopedPointer>
#include <cpitemdatahelper.h>
#include <cpsettingformitemdata.h>

// User includes

#include "cpwlansettingsplugin.h"
#include "cpwlansettingsgroupitemdata.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlansettingspluginTraces.h"
#endif

/*!
    \class CpWlanSettingsPlugin
    \brief CpWlanSettingsPlugin implements the WLAN Settings Control Panel Plugin
    which will allow viewing/editing of Manage WLAN Settings.
*/

// External function prototypes

// Local constants

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/

CpWlanSettingsPlugin::CpWlanSettingsPlugin() :
    QObject(),
    mTranslator(new HbTranslator("cpwlansettingsplugin"))
{
    OstTraceFunctionEntry0( CPWLANSETTINGSPLUGIN_CPWLANSETTINGSPLUGIN_ENTRY );
    OstTraceFunctionExit0( CPWLANSETTINGSPLUGIN_CPWLANSETTINGSPLUGIN_EXIT );
}

/*!
    Destructor.
*/

CpWlanSettingsPlugin::~CpWlanSettingsPlugin()
{
    OstTraceFunctionEntry0( DUP1_CPWLANSETTINGSPLUGIN_CPWLANSETTINGSPLUGIN_ENTRY );
    OstTraceFunctionExit0( DUP1_CPWLANSETTINGSPLUGIN_CPWLANSETTINGSPLUGIN_EXIT );
}

/*!
    Plugin function for creating the WLAN Settings form item data.
*/

QList<CpSettingFormItemData*> CpWlanSettingsPlugin::createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const
{
    OstTraceFunctionEntry0( CPWLANSETTINGSPLUGIN_CREATESETTINGFORMITEMDATA_ENTRY );
    
    QList<CpSettingFormItemData*> settingFormData;
    settingFormData.append(new CpWlanSettingsGroupItemData(itemDataHelper));
    
    OstTraceFunctionExit0( CPWLANSETTINGSPLUGIN_CREATESETTINGFORMITEMDATA_EXIT );
    return settingFormData;
    
}

Q_EXPORT_PLUGIN2(cpwlansettingsplugin, CpWlanSettingsPlugin);
