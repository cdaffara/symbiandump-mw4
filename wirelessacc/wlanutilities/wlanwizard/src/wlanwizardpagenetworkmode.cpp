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
* WLAN Wizard Page: Network mode Selection.
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
#include "wlanwizardpagenetworkmode.h"
#include "wlanwizardscanlist.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagenetworkmodeTraces.h"
#endif

// Local constants

/*!
 * Constructor method for the network mode view object
 * @param parent [in] pointer to parent object.
 */
WlanWizardPageNetworkMode::WlanWizardPageNetworkMode(
    WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent),
    mWidget(NULL),
    mList(NULL),
    mLabel(NULL),
    mLoader(NULL)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_WLANWIZARDPAGENETWORKMODE_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_WLANWIZARDPAGENETWORKMODE_EXIT );
}

/*!
 * Destructor. Loader widget is deleted.
 * All document widgets are deleted by wlanwizard_p destructor. 
 */
WlanWizardPageNetworkMode::~WlanWizardPageNetworkMode()
{
    OstTraceFunctionEntry0( DUP1_WLANWIZARDPAGENETWORKMODE_WLANWIZARDPAGENETWORKMODE_ENTRY );
    delete mLoader;
    OstTraceFunctionExit0( DUP1_WLANWIZARDPAGENETWORKMODE_WLANWIZARDPAGENETWORKMODE_EXIT );
}

/*!
 * Page initialization. If view is already loaded, does nothing.
 * Inherited from WlanWizardPage.
 */
HbWidget* WlanWizardPageNetworkMode::initializePage()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_INITIALIZEPAGE_ENTRY );
    OstTrace0( TRACE_NORMAL, WLANWIZARDPAGENETWORKMODE_INITIALIZEPAGE,
        "WlanWizardPageNetworkMode::initializePage" );

    if (mWidget == NULL) {
        mLoader = new HbDocumentLoader(mWizard->mainWindow());

        bool ok;

        mLoader->load(":/docml/occ_add_wlan_02_03.docml", &ok);
        Q_ASSERT(ok);

        loadDocmlSection(mWizard->mainWindow()->orientation());

        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget(
            "occ_add_wlan_02"));
        Q_ASSERT(mWidget != NULL);

        mList = qobject_cast<HbRadioButtonList*> (mLoader->findWidget("list"));
        Q_ASSERT(mList != NULL);

        mLabel = qobject_cast<HbLabel*> (mLoader->findWidget("dialog_6"));
        Q_ASSERT(mLabel != NULL);

        // Connect document loading to main window orientation changes.
        ok = connect(
            mWizard->mainWindow(),
            SIGNAL(orientationChanged(Qt::Orientation)),
            this,
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
        
        // Connect a function to a radio button selection.
        ok = connect(
            mList,
            SIGNAL(itemSelected(int)),
            this,
            SLOT(itemSelected()));
        Q_ASSERT(ok);

        mLabel->setPlainText(hbTrId(
            "txt_occ_dialog_select_network_mode_and_status"));
    }
    
    QStringList items;    
    populateRadioButtonList(items);

    // Do not override the list, if it didn't change, because setItems()
    // clears the possible selected item.
    if (items != mList->items()) {
        mList->setItems(items);
    }
    
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_INITIALIZEPAGE_EXIT );
    return mWidget;
}

/*!
 * Evaluates the network mode selection and sets the configuration in
 * the wlanwizard.
 * @param [out] removeFromStack always returns false.
 * @return WlanWizardPageSecurityMode page id.
 */
int WlanWizardPageNetworkMode::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_NEXTID_ENTRY );
    removeFromStack = false;
    WlanNetworkSetting setting;
    int index = mList->selected();
    int nextPage = WlanWizardPageInternal::PageNetworkSecurity;
    
    if (mWpsSupported.at(index)) {
        nextPage = WlanWizardPageInternal::PageWpsStart;
    } else {
        setting.mode = mNetworkModes.at(index);
        setting.hidden = mIsHidden.at(index);
        setting.wpsSupported = mWpsSupported.at(index);

        mWizard->setConfiguration(
            WlanWizardPrivate::ConfNetworkMode,
            setting.mode);

        mWizard->setConfiguration(
            WlanWizardPrivate::ConfWlanScanSSID,
            setting.hidden);
        
        mWizard->setConfiguration(
            WlanWizardPrivate::ConfWpsSupported,
            setting.wpsSupported);

        if (mWizard->configurationExists(WlanWizardPrivate::ConfAvailableNetworkOptions)) {
            nextPage = selectNextPage(setting);
        }
    }
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_NEXTID_EXIT );
    return nextPage;
}

/*!
 * This method is overrides the default implementation from WlanWizardPage.
 * @return Should the Next-button be enabled or not.
 */
bool WlanWizardPageNetworkMode::showPage()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_SHOWPAGE_ENTRY );
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_SHOWPAGE_EXIT );
    // Next-button is enabled whenever the radio button list has some value
    // selected.
    return mList->selected() != -1;
}

/*!
 * This function is invoked when user selects a mode from the radio button list.
 * (HbRadioButtonList's itemSelected-signal)
 */
void WlanWizardPageNetworkMode::itemSelected()
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_ITEMSELECTED_ENTRY );
    OstTrace0( TRACE_BORDER, WLANWIZARDPAGENETWORKMODE_ITEMSELECTED,
        "WlanWizardPageNetworkMode::itemSelected" );

    mWizard->enableNextButton(true);
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_ITEMSELECTED_EXIT );
}

/*!
 * Loads the document orientation information from occ_add_wlan_02_03.docml.
 * This is called each time phone orientation changes.
 * @param [in] orientation indicates whether the phone is in portrait or
 * landscape mode.
 */
void WlanWizardPageNetworkMode::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_LOADDOCMLSECTION_ENTRY );
    OstTrace1( TRACE_NORMAL, WLANWIZARDPAGENETWORKMODE_LOADDOCML,
        "WlanWizardPageNetworkMode::loadDocml - Orientation;orientation=%x",
        ( TUint )( orientation ) );

    WlanWizardPageInternal::loadDocmlSection(
        mLoader,
        orientation,
        ":/docml/occ_add_wlan_02_03.docml", 
        "portrait_section",
        "landscape_section");
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_LOADDOCMLSECTION_EXIT );
}

/*!
 * A support function to map the radio button list to a generic network
 * mode list. This enables the changing of button order without it
 * affecting the entire class.
 * In case previous scan has revealed multiple results with the same ssid, only
 * available network mode options are shown in the radio button list.
 * @param [out] list is the list of captions used for the radio buttons.
 * @param [in] item is the caption to be added.
 * @param [in] mode is the network mode to be added.
 * @param [in] isHidden is the visibility status of the network.
 * @param [in] wpsSupported is the possibility for wps support.
 */
void WlanWizardPageNetworkMode::addToList(
    QStringList &list,
    const QString &item,
    int mode,
    bool isHidden,
    bool wpsSupported)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_ADDTOLIST_ENTRY );
        
    if (mWizard->configurationExists(WlanWizardHelper::ConfAvailableNetworkOptions)) {
        WlanWizardScanList networkOptions = mWizard->configuration(
            WlanWizardHelper::ConfAvailableNetworkOptions).value<WlanWizardScanList> ();

        QList<WlanNetworkSetting> modes = networkOptions.getNetModes();

        for (int i = 0; i < modes.size(); i++) {
            if ( (modes[i].mode == mode 
                && modes[i].hidden == isHidden
                && modes[i].wpsSupported == wpsSupported)
                || (modes[i].wpsSupported && wpsSupported) ) {
                list << item;
                mNetworkModes.append(mode);
                mIsHidden.append(isHidden);
                mWpsSupported.append(wpsSupported);
            }
        }
    } else { 
        list << item;
        mNetworkModes.append(mode);
        mIsHidden.append(isHidden);
        // If there are no known network options, WPS is not allowed to be
        // selected.
        mWpsSupported.append(false);
    }
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_ADDTOLIST_EXIT );
}

/*!
 * This function checks the sets up the radio button list for user input.
 * @param [out] list is the list of captions used for the radio buttons.
 */
void WlanWizardPageNetworkMode::populateRadioButtonList(QStringList &list)
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_POPULATERADIOBUTTONLIST_ENTRY );
    list.clear();
    mNetworkModes.clear();
    mIsHidden.clear();
    mWpsSupported.clear();

    addToList(
        list,
        hbTrId("txt_occ_dblist_val_infrastructure_public"),
        CMManagerShim::Infra,
        false,
        false);

    addToList(
        list,
        hbTrId("txt_occ_list_infrastructure_hidden"),
        CMManagerShim::Infra,
        true,
        false);

    addToList(
        list,
        hbTrId("txt_occ_list_adhoc_1"),
        CMManagerShim::Adhoc,
        false,
        false);
    
    if (mWizard->configurationExists(WlanWizardHelper::ConfAvailableNetworkOptions)) {
        // addToList with wpsSupported=true is only called, when there are available
        // scan result options. If no network options exist, the user can not get to
        // wps wizard from this view. Also, mode and isHidden are "don't care".
        addToList(list, hbTrId("txt_occ_list_wifi_protected_setup"), NetworkModeNone, false, true);
    }
    OstTraceFunctionExit0( WLANWIZARDPAGENETWORKMODE_POPULATERADIOBUTTONLIST_EXIT );
}

/*!
 * This function selects the next page. Security mode may be skipped in case
 * scan results exist and only single option remains.
 * @param [in] setting indicates the selected network mode.
 * @return Security mode query page identifier unless it can be skipped.
 */
int WlanWizardPageNetworkMode::selectNextPage(const WlanNetworkSetting &setting) const
{
    OstTraceFunctionEntry0( WLANWIZARDPAGENETWORKMODE_SELECTNEXTPAGE_ENTRY );
    
    int nextPage = WlanWizardPageInternal::PageNetworkSecurity;
    
    WlanWizardScanList networkOptions = mWizard->configuration(
        WlanWizardHelper::ConfAvailableNetworkOptions).value<WlanWizardScanList>();

    if (networkOptions.secModes(setting) == SingleResult) {
        WlanSecuritySetting secMode = networkOptions.getSecMode(setting);
        mWizard->setConfiguration(WlanWizardHelper::ConfSecurityMode, secMode.mode);
        mWizard->setConfiguration(WlanWizardHelper::ConfUsePsk, secMode.usePsk);
        nextPage = secMode.nextPageId;
    }
    
    OstTraceFunctionExit0( DUP1_WLANWIZARDPAGENETWORKMODE_SELECTNEXTPAGE_EXIT );
    return nextPage;
}
