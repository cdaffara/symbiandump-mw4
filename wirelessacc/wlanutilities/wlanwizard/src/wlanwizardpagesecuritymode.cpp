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
* WLAN Wizard Page: Security mode Selection.
*/

// System includes

#include <HbDocumentLoader>
#include <HbWidget>
#include <HbRadioButtonList>
#include <HbMainWindow>
#include <HbLabel>

#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanwizard_p.h"
#include "wlanwizard.h"
#include "wlanwizardpagesecuritymode.h"
#include "wlanwizardscanlist.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagesecuritymodeTraces.h"
#endif

/*!
 * Constructor method for the Sequrity mode query view object.
 * @param [in] parent pointer to parent object.
 */
WlanWizardPageSecurityMode::WlanWizardPageSecurityMode(
    WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent), 
    mWidget(NULL),
    mList(NULL),
    mLabel(NULL),
    mLoader(NULL)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_WLANWIZARDPAGESECURITYMODE_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_WLANWIZARDPAGESECURITYMODE_EXIT);
}

/*!
 * Destructor. Loader widget is deleted.
 * All document widgets are deleted by wlanwizard_p destructor.
 */
WlanWizardPageSecurityMode::~WlanWizardPageSecurityMode()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGESECURITYMODE_WLANWIZARDPAGESECURITYMODE_ENTRY);
    
    delete mLoader;
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGESECURITYMODE_WLANWIZARDPAGESECURITYMODE_EXIT);
}

/*!
 * Page initialization. If view widget is already loaded, does nothing.
 * @return pointer to widget "occ_add_wlan_02".
 */
HbWidget* WlanWizardPageSecurityMode::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_INITIALIZEPAGE_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESECURITYMODE_INITIALIZEPAGE,
        "WlanWizardPageSecurityMode::initializePage");
    
    if (mWidget==NULL) {
        
        mLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        bool ok;

        mLoader->load( ":/docml/occ_add_wlan_02_03.docml", &ok );
        Q_ASSERT(ok);
              
        // Initialize orientation
        loadDocmlSection( mWizard->mainWindow()->orientation() );

        // Load widgets
        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget("occ_add_wlan_02"));
        Q_ASSERT(mWidget != NULL);

        mList = qobject_cast<HbRadioButtonList*> (mLoader->findWidget("list"));
        Q_ASSERT(mList != NULL);

        mLabel = qobject_cast<HbLabel*> (mLoader->findWidget("dialog_6"));
        Q_ASSERT(mLabel != NULL);
        
        mLabel->setPlainText(hbTrId("txt_occ_dialog_select_network_security_mode"));

        // Connect orientation signal from the main window to orientation
        // loader.
        ok &= connect( mWizard->mainWindow(),
                 SIGNAL(orientationChanged(Qt::Orientation)),
                 this, SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
 
        // Connect signal from the radio button list indicating that an item
        // has been selected to validation handler.
        ok &= connect( mList, SIGNAL(itemSelected(int)),
                 this, SLOT(itemSelected())); 
        Q_ASSERT(ok);
    }
    
    // Create contents to the security mode radio button list.
    QStringList items;    
    populateSecModeList(items);

    // Do not override the list, if it didn't change, because setItems()
    // clears the possible selected item.
    if (items != mList->items()) {
        mList->setItems(items);
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_INITIALIZEPAGE_EXIT);
    return mWidget;
}

/*!
 * Validates the Security more selection and sets the configuration in
 * the wlanwizard.
 * @param [out] removeFromStack returns false.
 * @return depending on the security mode, returns the appropriate view
 * identifier.
 */
int WlanWizardPageSecurityMode::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_NEXTID_ENTRY);
    
    removeFromStack = false;
    
    // The configuration is selected from the mSecModes list, which is ordered
    // during the page initialization.
    mWizard->setConfiguration(
        WlanWizardPrivate::ConfSecurityMode,
        mSecModes.at(mList->selected()));

    // The configuration is selected from the mUsePsk list, which is ordered
    // during the page initialization.
    mWizard->setConfiguration(
        WlanWizardPrivate::ConfUsePsk,
        mUsePsk.at(mList->selected()));
    
    OstTraceFunctionExit0(LANWIZARDPAGESECURITYMODE_NEXTID_EXIT);
    return mPageIds.at(mList->selected());
}

/*!
 * This method is overrides the default implementation from WlanWizardPage.
 * @return Should the Next-button be enabled or not.
 */
bool WlanWizardPageSecurityMode::showPage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_SHOWPAGE_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_SHOWPAGE_EXIT);
    // Next-button is enabled whenever the radio button list has some value
    // selected.
    return mList->selected() != -1;
}

/*!
 * Is invoked when user selects a mode from the radio button list.
 * (HbRadioButtonList's itemSelected-signal)
 */
void WlanWizardPageSecurityMode::itemSelected()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_ITEMSELECTED_ENTRY);
    
    OstTrace0(
        TRACE_BORDER,
        WLANWIZARDPAGESECURITYMODE_ITEMSELECTED,
        "WlanWizardPageSecurityMode::itemSelected");

    mWizard->enableNextButton(true);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_ITEMSELECTED_EXIT);
}

/*!
 * Loads the document orientation information from occ_add_wlan_02_03.docml
 * This is called each time phone orientation changes.
 * @param [in] orientation indicates whether the phone is in portrait or
 * landscape mode.
 */
void WlanWizardPageSecurityMode::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_LOADDOCMLSECTION_ENTRY);
    
    OstTrace1(
        TRACE_NORMAL,
        WLANWIZARDPAGESECURITYMODE_LOADDOCML,
        "WlanWizardPageSecurityMode::loadDocml - orientation;orientation=%x",
        (TUint)orientation);
    
    WlanWizardPageInternal::loadDocmlSection(
        mLoader,
        orientation,
        ":/docml/occ_add_wlan_02_03.docml", 
        "portrait_section",
        "landscape_section");
    
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_LOADDOCMLSECTION_EXIT);
}

/*!
 * Support function that creates the contents of the security mode list. 
 * @param [out] list is the list of captions used for the radio buttons.
 */
void WlanWizardPageSecurityMode::populateSecModeList(QStringList &list)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_POPULATESECMODELIST_ENTRY);
    
    mSecModes.clear();
    mPageIds.clear();
    mUsePsk.clear();
    
    // Create the radio button list to correspond to correct security mode
    // identifiers and page identifiers.
    // Populate the list according to network mode selection.
    addToList(
        list,
        hbTrId("txt_occ_list_open"), 
        CMManagerShim::WlanSecModeOpen,
        WlanWizardPage::PageProcessSettings,
        false);

    addToList(
        list,
        hbTrId("txt_occ_list_wep_1"),
        CMManagerShim::WlanSecModeWep,
        WlanWizardPageInternal::PageKeyQuery,
        true);

    // In case of Ad-hoc network, exclude wpa, eap and 802.1X modes.
    if (mWizard->configuration(WlanWizardPrivate::ConfNetworkMode).toInt()
        != CMManagerShim::Adhoc) {

        addToList(
            list,
            hbTrId("txt_occ_list_wpa_with_password"),
            CMManagerShim::WlanSecModeWpa,
            WlanWizardPageInternal::PageKeyQuery,
            true);

        addToList(
            list,
            hbTrId("txt_occ_list_wpa_with_eap"),
            CMManagerShim::WlanSecModeWpa,
            WlanWizardPage::PageEapStart,
            false);

        addToList(
            list,
            hbTrId("txt_occ_list_8021x_1"),
            CMManagerShim::WlanSecMode802_1x,
            WlanWizardPage::PageEapStart,
            false);
    }

    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_POPULATESECMODELIST_EXIT);
}

/*!
 * A support function to map the radio button list to a generic network
 * mode list. This enables the changing of button order without it
 * affecting the entire class.
 * @param [out] list is a reference to a local list that will be placed as an
 * argument to the radio button list.
 * @param [in] item is a reference to the text value that is appended to the
 * list
 * @param [in] mode is the security mode associated with the item
 * @param [in] page is the id of the next wizard page when this item is selected.
 * @param [in] psk indicates whether password needs to be queried. 
 */
void WlanWizardPageSecurityMode::addToList(
    QStringList &list,
    const QString &item,
    int mode,
    int page,
    bool psk)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESECURITYMODE_ADDTOLIST_ENTRY);
    
    if (mWizard->configurationExists(WlanWizardHelper::ConfAvailableNetworkOptions)) {
        WlanNetworkSetting netMode;
        netMode.mode = mWizard->configuration(WlanWizardPrivate::ConfNetworkMode).toInt();
        netMode.hidden = mWizard->configuration(WlanWizardPrivate::ConfWlanScanSSID).toBool();
        netMode.wpsSupported = mWizard->configuration(WlanWizardPrivate::ConfWpsSupported).toBool();
        WlanWizardScanList networkOptions = mWizard->configuration(
            WlanWizardHelper::ConfAvailableNetworkOptions).value<WlanWizardScanList>();

        for (int i = 0; i < networkOptions.secModes(netMode); i++) {
            WlanSecuritySetting secMode = networkOptions.getSecMode(netMode, i);
            
            if (secMode.mode == mode && secMode.usePsk == psk) {
                list << item;
                mSecModes.append(mode);
                mPageIds.append(page);
                mUsePsk.append(psk);
            }
        }
    }
    else {
        list << item;
        mSecModes.append(mode);
        mPageIds.append(page);
        mUsePsk.append(psk);
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESECURITYMODE_ADDTOLIST_EXIT);
}
