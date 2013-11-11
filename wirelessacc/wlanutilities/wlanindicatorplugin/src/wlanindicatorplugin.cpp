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
 * 
 */

#include <HbTranslator>
#include <QLocale>
#include <QtCore/qplugin.h>
#include <QtCore/QString>
#include <QtCore/QVariant> 
#include <QtCore/QMetaType>

#include <HbLabel>
#include <hbglobal.h>
#include <hbapplication.h>
#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>
#include "wlanindicatorplugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanindicatorpluginTraces.h"
#endif

Q_EXPORT_PLUGIN(WlanIndicatorPlugin)

const static char IndicatorType[] = "com.nokia.hb.indicator.connectivity.wlanindicatorplugin/1.0";


/*!
    WlanIndicatorPlugin constructor.
*/
WlanIndicatorPlugin::WlanIndicatorPlugin() :
    HbIndicatorInterface(IndicatorType, SettingCategory, InteractionActivated),
    mError(0)
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_WLANINDICATORPLUGIN_ENTRY, this);
    mIndicatorTypes << "com.nokia.hb.indicator.connectivity.wlanindicatorplugin/1.0";
    OstTraceFunctionExit1(WLANINDICATORPLUGIN_WLANINDICATORPLUGIN_ENTRY_EXIT, this);
}

/*!
    WlanIndicatorPlugin destructor.
*/
WlanIndicatorPlugin::~WlanIndicatorPlugin()
{
    OstTraceFunctionEntry1(DUP1_WLANINDICATORPLUGIN_WLANINDICATORPLUGIN_ENTRY, this);
    OstTraceFunctionExit1(DUP1_WLANINDICATORPLUGIN_WLANINDICATORPLUGIN_EXIT, this);
}

/*!
    The indicatorTypes returns type of indicator. In this case it is wlanindicatorplugin.
*/
QStringList WlanIndicatorPlugin::indicatorTypes() const
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_INDICATORTYPES_ENTRY, this);
    OstTraceFunctionExit1(WLANINDICATORPLUGIN_INDICATORTYPES_EXIT, this);

    return mIndicatorTypes;
}

/*!
    The handleClientRequest handles client request to change indicators parameters.
*/
bool WlanIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_ACCESSALLOWED_ENTRY, this);

    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    OstTraceFunctionExit1(WLANINDICATORPLUGIN_ACCESSALLOWED_EXIT, this);

    return true;
}

/*!
    The createIndicator creates indicator plugin instance.
*/
HbIndicatorInterface* WlanIndicatorPlugin::createIndicator(
    const QString &indicatorType)
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_CREATEINDICATOR_ENTRY, this);
    Q_UNUSED(indicatorType)
    
    // Install localization
    mTranslator = QSharedPointer<HbTranslator>(new HbTranslator("wlanindicatorplugin"));

    OstTraceFunctionExit1(WLANINDICATORPLUGIN_CREATEINDICATOR_EXIT, this);
    return this;
}

/*!
    The error returns indicator error to HbIndicatorPluginInterface.
*/
int WlanIndicatorPlugin::error() const
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_ERROR_ENTRY, this);
    OstTrace1( WLANINDICATORPLUGIN_ERR,ERROR,"WlanIndicatorPlugin error %u", mError);
    OstTraceFunctionExit1(WLANINDICATORPLUGIN_ERROR_EXIT, this);
    return mError;
}

/*!
    The handleInteraction is used launch WLAN list view.
*/
bool WlanIndicatorPlugin::handleInteraction(InteractionType type)
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_HANDLEINTERACTION_ENTRY, this);

    bool handled = false;
    QVariantMap data;
    switch (type) {
    case InteractionActivated: 
        // Emit a signal that will inform the client to start the wlan list view 
        emit userActivated(data);
        handled = true;
        break;
    default:
        break;
    }

    OstTraceFunctionExit1(WLANINDICATORPLUGIN_HANDLEINTERACTION_EXIT, this);

    return handled;
}

/*!
    The handleClientRequest handles client request to change indicators paramters.
*/
bool WlanIndicatorPlugin::handleClientRequest(RequestType type, const QVariant &parameter)
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_CLIENTREQUEST_ENTRY, this);

    Q_UNUSED(parameter)
    bool handled(false);
    switch (type) {
    case RequestActivate:
        if (mParameter != parameter) {
            mParameter = parameter;
            emit dataChanged();
        }     
        handled = true;
        break;
    default:
        mParameter.clear();
    }
    
    OstTraceFunctionExit1(WLANINDICATORPLUGIN_CLIENTREQUEST_EXIT, this);
    return handled;
}

/*!
    The indicatorData takes care of showing indicator's data.
*/
QVariant WlanIndicatorPlugin::indicatorData(int role) const
{
    OstTraceFunctionEntry1(WLANINDICATORPLUGIN_INDICATORDATA_ENTRY, this);

    QVariant qvariant("");
    QList<QVariant> indicatorText;
    bool validParameters = false;
        
    switch (role) {
    case PrimaryTextRole:
        qvariant = QString(hbTrId("txt_occ_dblist_indi_menu_wireless_lan"));
        break;
    case SecondaryTextRole:
        if (mParameter.isValid() && mParameter.type() == QVariant::List) {
        
            indicatorText = mParameter.toList();
            validParameters = true;
            }            
        if (validParameters && (indicatorText.size() > 0)) {
        	        
            if (indicatorText[0] == wlanNotConnected){
                    qvariant = QString(hbTrId("txt_occ_dblist_wireless_lan_val_not_connected"));
                    }

            else if (indicatorText[0] == wlanConnected){
                qvariant = indicatorText[1].toString();
                }
            }            
        else 
           {
            OstTrace0( WLANINDICATORPLUGIN_ERR,INVALID_PARAMS,"Invalid indicator parameters");
           }
        break;
        // this is the statusbar icon, which is shown only when a connection is active
    case MonoDecorationNameRole:
        OstTraceFunctionExit1(DUP1_WLANINDICATORPLUGIN_INDICATORDATA_EXIT, this);
        
        if (mParameter.isValid() && mParameter.type() == QVariant::List) {
            indicatorText = mParameter.toList();
            if ((indicatorText.size() > 0) && (indicatorText[0] == wlanConnected)) {
                qvariant = QString("qtg_status_wlan");
            }
        }
        break;
        // this is the icon in the indicator
    case DecorationNameRole:
        OstTraceFunctionExit1(DUP2_WLANINDICATORPLUGIN_INDICATORDATA_EXIT, this);
        qvariant = QString("qtg_small_wlan");
        break;
    default:
        OstTraceFunctionExit1(DUP3_WLANINDICATORPLUGIN_INDICATORDATA_EXIT, this);
        break;
    }
    return qvariant;
}

