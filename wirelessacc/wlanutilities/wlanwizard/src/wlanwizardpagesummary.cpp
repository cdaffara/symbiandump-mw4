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
* Wlan Wizard Page: Summary
*/

// System includes

#include <HbMainWindow>
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbStyleLoader>

// User includes

#include "wlanqtutils.h"
#include "wlanwizard_p.h"
#include "wlanwizardplugin.h"
#include "wlanwizardsummaryviewitem.h"
#include "wlanwizardpagesummary.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagesummaryTraces.h"
#endif


/*!
   \class WlanWizardPageSummary
   \brief Implements Wizard summary page.
 */

// External function prototypes

// Local constants

//! Path for layout of custom list widget
static const char WlanWizardPageSummaryLayout[] = ":/wizardsummarylayout/";

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor.
   
   @param [in] parent pointer to private implementation of wizard.
 */
WlanWizardPageSummary::WlanWizardPageSummary(WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent), 
    mListWidget(NULL) 
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_WLANWIZARDPAGESUMMARY_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_WLANWIZARDPAGESUMMARY_EXIT);
}

/*!
   Destructor.
 */
WlanWizardPageSummary::~WlanWizardPageSummary()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGESUMMARY_WLANWIZARDPAGESUMMARY_ENTRY);
    
    HbStyleLoader::unregisterFilePath(WlanWizardPageSummaryLayout);
    
    // Wizard framework deletes the visualization (owns mListWidget).
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGESUMMARY_WLANWIZARDPAGESUMMARY_EXIT);
}

/*!
   See WlanWizardPage::initializePage()
 */
HbWidget* WlanWizardPageSummary::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_INITIALIZEPAGE_ENTRY);
    
    // Note that from summary page it is not possible to step back
    // this means that it is not possible that this method is called twice.
    Q_ASSERT(!mListWidget);
    
    loadDocml();
    int row = 0;
    
    appendToList(
        row, 
        hbTrId("txt_occ_dblist_network_name"),
        mWizard->configuration(WlanWizardPrivate::ConfName).toString());

    appendToList( 
        row, hbTrId("txt_occ_dblist_network_mode"), networkModeText());

    appendToList(
        row, hbTrId("txt_occ_dblist_security_mode"), securityModeText());    

    addDynamicItems(row);
        
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_INITIALIZEPAGE_EXIT);
    return qobject_cast<HbWidget*>(mListWidget);
}

/*!
   Loads docml.
 */
void WlanWizardPageSummary::loadDocml()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_LOADDOCML_ENTRY);
    
    bool ok;    
    HbDocumentLoader docLoader(mWizard->mainWindow());
    
    docLoader.load(":/docml/occ_add_wlan_05.docml", &ok);
    Q_ASSERT(ok);
    
    mListWidget = qobject_cast<HbListWidget *>(docLoader.findWidget("occ_add_wlan_05"));
    Q_ASSERT(mListWidget != NULL);

    // Set own list prototype handler to disable TAP gestures and selections
    mListWidget->setItemPrototype(
        new WlanWizardSummaryListViewItem(mListWidget));
    
    // set the layout name to match the layout name in hblistviewitem.css file
    mListWidget->setLayoutName("wlanwizardsummaryitem");
    
    // Register the location of hblistviewitem.css and hblistviewitem.widgetml  
    ok = HbStyleLoader::registerFilePath(WlanWizardPageSummaryLayout);
    Q_ASSERT(ok);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_LOADDOCML_EXIT);
}

/*!
   Gets localized network mode from the WlanWizardPrivate::ConfNetworkMode
   configuration.
    
   @return localized network mode text.
 */
QString WlanWizardPageSummary::networkModeText() const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_NETWORKMODETEXT_ENTRY);
    
    QString ret;
    int mode = mWizard->configuration(
        WlanWizardPrivate::ConfNetworkMode).toInt();
        
    switch (mode) {
    case CMManagerShim::Adhoc:
        ret = hbTrId("txt_occ_dblist_network_mode_val_adhoc");
        break;

     default:
         Q_ASSERT(mode == CMManagerShim::Infra); 
         if (mWizard->configuration(WlanWizardPrivate::ConfWlanScanSSID).toBool()) {
             ret = hbTrId("txt_occ_dblist_network_mode_val_infrastructure_hi");
         } else {
             ret = hbTrId("txt_occ_dblist_network_mode_val_infrastructure_pu");
         }
         break;
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_NETWORKMODETEXT_EXIT);
    return ret;
}

/*!
   Gets localized security mode from the WlanWizardPrivate::ConfSecurityMode
   configuration.
   
   @return localized security mode text.
 */
QString WlanWizardPageSummary::securityModeText() const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_SECURITYMODETEXT_ENTRY);
    
    QString ret;
    int mode = mWizard->configuration(WlanWizardPrivate::ConfSecurityMode).toInt();
    switch (mode) {
    case CMManagerShim::WlanSecMode802_1x:
        ret = hbTrId("txt_occ_dblist_security_mode_val_8021x");
        break;
        
    case CMManagerShim::WlanSecModeWep:
        ret = hbTrId("txt_occ_dblist_security_mode_val_wep");
        break;
        
    case CMManagerShim::WlanSecModeWpa:
        if (mWizard->configuration(WlanWizardPrivate::ConfUsePsk).toBool()) {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpawpa2psk");
        } else {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpawpa2_with_eap");
        }
        break;
        
    case CMManagerShim::WlanSecModeWpa2:
        if (mWizard->configuration(WlanWizardPrivate::ConfUsePsk).toBool()) {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpa2_with_passwor");
        } else {
            ret = hbTrId("txt_occ_dblist_security_mode_val_wpa2_with_eap");
        }
        break;
        
    default:
        Q_ASSERT(mode == CMManagerShim::WlanSecModeOpen);
        ret = hbTrId("txt_occ_dblist_security_mode_val_open");
        break;
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_SECURITYMODETEXT_EXIT);
    return ret;
}

/*!
   Adds dynamic content to the view.
   
   @param [in,out] row row to where insert happens.
 */
void WlanWizardPageSummary::addDynamicItems(int &row)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_ADDDYNAMICITEMS_ENTRY);
    
    if (mWizard->isEapEnabled()) {
        appendToListPluginInfo(WlanWizardPlugin::SummaryEapOuterType, row);
        appendToListPluginInfo(WlanWizardPlugin::SummaryEapInnerType, row);
        appendToListPluginInfo(WlanWizardPlugin::SummaryEapFastProvisioningMode, row);
    }

    QString value;

    switch (mWizard->configuration(WlanWizardPrivate::ConfIctStatus).toInt()) {
    case WlanQtUtils::IctPassed:
        value = hbTrId("txt_occ_dblist_destination_val_internet");
        appendToList(row, hbTrId("txt_occ_dblist_destination"), value);
        break;
        
    case WlanQtUtils::IctFailed:
        value = hbTrId("txt_occ_dblist_destination_val_uncategorized");
        appendToList(row, hbTrId("txt_occ_dblist_destination"), value);
        break;
        
    default:
        Q_ASSERT(WlanQtUtils::IctHotspotPassed ==
            mWizard->configuration(WlanWizardPrivate::ConfIctStatus).toInt());
        // Add nothing to list.
        break;
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_ADDDYNAMICITEMS_EXIT);
}

/*!
   Appends new item to the end of the summary list
   
   @param [in,out] row number to where the item is inserted
   @param [in] title title of the item
   @param [in] value value of the item
 */
void WlanWizardPageSummary::appendToList(
    int &row, 
    const QString title, 
    const QString value)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_APPENDTOLIST_ENTRY);
    
    HbListWidgetItem *item = new HbListWidgetItem();
    item->setText(title);
    item->setSecondaryText(value);
    mListWidget->insertItem(row++, item);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_APPENDTOLIST_EXIT);
}

/*!
   Appends new item to the end of the summary list
   
   @param [in] id Plugin summary id
   @param [in,out] row number to where the item is inserted
 */
void WlanWizardPageSummary::appendToListPluginInfo( 
    WlanWizardPlugin::Summary id,
    int &row)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESUMMARY_APPENDTOLISTPLUGININFO_ENTRY);
    
    QString title;
    QString value;
    WlanWizardPlugin *plugin = mWizard->wlanWizardPlugin();
    Q_ASSERT(plugin);
    if (plugin->summary(id, title, value)) {
        appendToList(row, title, value );
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESUMMARY_APPENDTOLISTPLUGININFO_EXIT);
}
