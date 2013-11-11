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
* WLAN Wizard Page: Scan processing.
*/

// System includes

#include <HbParameterLengthLimiter>
#include <HbMainWindow>
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbLabel>
#include <HbProgressBar>

#include <cmmanagerdefines_shim.h>
#include <wlanqtutils.h>
#include <wlanqtutilsap.h>

// User includes

#include "wlanwizard_p.h"
#include "wlanwizardpagescanning.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpagescanningTraces.h"
#endif

// Local constants

/*!
 * Constructor method for the scanning view object.
 * @param [in] parent pointer to parent object.
 */
WlanWizardPageScanning::WlanWizardPageScanning(WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent),
    mWidget(NULL),
    mLabel(NULL),
    mLoader(NULL),
    mNextPageId(WlanWizardPage::PageNone),
    mScanResultsAvailable(false),
    mScanStatus(WlanQtUtils::ScanStatusOk)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_WLANWIZARDPAGESCANNING_ENTRY);
    
    WlanQtUtils* utils = mWizard->wlanQtUtils();

    // Connect normal scan completion signal from wlanQtUtils to result
    // handler. Connect here instead of initializePage, since this signal may
    // need to be caught event if the window is not active.
    bool ok = connect(
        utils,
        SIGNAL(wlanScanApReady(int)),
        this,
        SLOT(wlanScanResultPreCheck(int)));
    Q_ASSERT(ok);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_WLANWIZARDPAGESCANNING_EXIT);
}

/*!
 * Destructor. Loader widget is deleted.
 * All document widgets are deleted by wlanwizard_p destructor.
 */
WlanWizardPageScanning::~WlanWizardPageScanning()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGESCANNING_WLANWIZARDPAGESCANNING_ENTRY);
    
    delete mLoader;
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGESCANNING_WLANWIZARDPAGESCANNING_EXIT);
}

/*!
 * Page initialization. If view is already loaded, does nothing. Inherited from
 * WlanWizardPage.
 * @return pointer to widget "occ_add_wlan_06".
 */
HbWidget* WlanWizardPageScanning::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_INITIALIZEPAGE_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESCANNING_INITIALIZEPAGE,
        "WlanWizardPageScanning::initializePage");

    // Next page id is reset with each initialization.    
    mNextPageId = WlanWizardPage::PageNone;

    if (mWidget == NULL) {
        mLoader = new HbDocumentLoader(mWizard->mainWindow());

        bool ok;

        mLoader->load(":/docml/occ_add_wlan_06.docml", &ok);
        Q_ASSERT(ok);

        // Initialize orientation
        loadDocmlSection(mWizard->mainWindow()->orientation());

        // Load widgets
        mWidget = qobject_cast<HbWidget*> (mLoader->findWidget(
            "occ_add_wlan_06"));
        Q_ASSERT(mWidget != NULL);

        mLabel = qobject_cast<HbLabel*> (mLoader->findWidget("dialog"));
        Q_ASSERT(mLabel != NULL);

        WlanQtUtils* utils = mWizard->wlanQtUtils();

        // Connect orientation signal from the main window to orientation
        // loader.
        ok = connect(
            mWizard->mainWindow(),
            SIGNAL(orientationChanged(Qt::Orientation)),
            this,
            SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);

        // Connect direct scan completion signal from wlanQtUtils to result
        // handler.
        ok = connect(
            utils,
            SIGNAL(wlanScanDirectReady(int)),
            this,
            SLOT(wlanScanDirectReady(int)));
        Q_ASSERT(ok);
    }

    mLabel->setPlainText(HbParameterLengthLimiter(
        "txt_occ_dialog_searching").arg(mWizard->configuration(
            WlanWizardPrivate::ConfSsid).toString()));

    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_INITIALIZEPAGE_EXIT);
    return mWidget;
}

/*!
 * Sends the user to the next page.
 * @param [out] removeFromStack is always true: this page is removed from the 
 * stacked widget after "Next" button is pressed.
 * @return the id value of the next page.
 */
int WlanWizardPageScanning::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_NEXTID_ENTRY);
    
    removeFromStack = true;

    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_NEXTID_EXIT);
    return mNextPageId;
}

/*!
 * This function reimplements WlanWizardPage::previousTriggered. When going
 * back from scanning view, the ongoing scan operation is also stopped. Since
 * there is no need to react to following the acknowledgement signal from
 * wlanQtUtils, the corresponding handler slots are disconnected before the
 * opration and immediately connected again.
 * @return one step backwards value.
 */
int WlanWizardPageScanning::previousTriggered()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_PREVIOUSTRIGGERED_ENTRY);
    
    WlanQtUtils* utils = mWizard->wlanQtUtils();
    
    disconnect(
        utils,
        SIGNAL(wlanScanApReady(int)),
        this,
        SLOT(wlanScanResultPreCheck(int)));
    
    disconnect(
        utils,
        SIGNAL(wlanScanDirectReady(int)),
        this,
        SLOT(wlanScanDirectReady(int)));
    
    utils->stopWlanScan();

    bool ok = connect(
        utils,
        SIGNAL(wlanScanApReady(int)),
        this,
        SLOT(wlanScanResultPreCheck(int)));
    Q_ASSERT(ok);
    
    ok = connect(
        utils,
        SIGNAL(wlanScanDirectReady(int)),
        this,
        SLOT(wlanScanDirectReady(int)));
    Q_ASSERT(ok);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_PREVIOUSTRIGGERED_EXIT);
    return OneStepBackwards;
    
}

/*!
 * This method is overrides the default implementation from WlanWizardPage.
 * It indicates whether the Next-button should be enabled or not. It also resets
 * the ConfProcessSettings-configuration to false - meaning that if no scan
 * results are available, no iap is created.
 * @return always false - the scanning proceeds to next window
 * automatically or not at all.
 */
bool WlanWizardPageScanning::showPage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_SHOWPAGE_ENTRY);

    mWizard->setConfiguration(WlanWizardHelper::ConfProcessSettings, false);

    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_SHOWPAGE_EXIT);
    return false;
}

/*!
 * Reimplements the default function in WlanWizardPage. Indicates to the
 * framework that startOperation() function must be executed.
 * @return true
 * @see WlanWizardPage
 */
bool WlanWizardPageScanning::requiresStartOperation()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_REQUIRESSTARTOPERATION_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_REQUIRESSTARTOPERATION_EXIT);
    return true;
}

/*!
 * Wlan scan is performed as a "post-initialization" which is executed in a
 * separate scheduler loop. If scan results are available, starts reading them.
 */
void WlanWizardPageScanning::startOperation()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_STARTOPERATION_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESCANNING_STARTOPERATION,
        "WlanWizardPageScanning::startOperation - start AP scan if results"
        " are available.");

    if (mScanResultsAvailable) {
        mScanResultsAvailable = false;
        wlanScanApReady();
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_STARTOPERATION_EXIT);
}

/*!
 * Loads the document orientation information from occ_add_wlan_06.docml
 * This is called each time phone orientation changes.
 * @param [in] orientation indicates whether the phone is in portrait or
 * landscape mode.
 */
void WlanWizardPageScanning::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_LOADDOCMLSECTION_ENTRY);
    
    OstTrace1(
        TRACE_NORMAL,
        WLANWIZARDPAGESCANNING_LOADDOCML,
        "WlanWizardPageScanning::loadDocml - orientation ;orientation=%x",
        (TUint)orientation);

    WlanWizardPageInternal::loadDocmlSection(
        mLoader,
        orientation,
        ":/docml/occ_add_wlan_06.docml", 
        "portrait_section",
        "landscape_section");
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_LOADDOCMLSECTION_EXIT);
}

/*!
 * Checks whether the view is active. If it is, execute wlanScanApReady. If
 * not, set mScanResultsAvailable to true.
 * @param [in] scanStatus indicates whether the scan was succesful, cancelled of
 * failed. The argument is saved to a member variable.
 */
void WlanWizardPageScanning::wlanScanResultPreCheck(int scanStatus)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_WLANSCANRESULTPRECHECK_ENTRY);
    
    mScanStatus = scanStatus;
    
    if (!mWidget) {
        OstTrace1(
            TRACE_BORDER,
            WLANWIZARDPAGESCANNING_WLANSCANRESULTPRECHECK,
            "WlanWizardPageScanning::wlanScanResultPreCheck no widget;this=%x",
            this);

        mScanResultsAvailable = true;
    } else {
        if (mWizard->isCurrentPage(mWidget)) {
            OstTrace0(
                TRACE_BORDER,
                DUP1_WLANWIZARDPAGESCANNING_WLANSCANRESULTPRECHECK,
                "WlanWizardPageScanning::wlanScanResultPreCheck go to results");

            wlanScanApReady();
        } else {
            OstTrace0(
                TRACE_BORDER,
                DUP2_WLANWIZARDPAGESCANNING_WLANSCANRESULTPRECHECK,
                "WlanWizardPageScanning::wlanScanResultPreCheck"
                " not current widget");

            mScanResultsAvailable = true;
        }
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_WLANSCANRESULTPRECHECK_EXIT);
}

/*!
 * Processes the direct scan results.
 * @param [in] scanStatus indicates whether the scan was succesful, canceled or
 * failed. In case the scan was not succesful, always proceed to manual wizard.
 */
void WlanWizardPageScanning::wlanScanDirectReady(int scanStatus)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_WLANSCANDIRECTREADY_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESCANNING_WLANSCANDIRECTREADY,
        "WlanWizardPageScanning::wlanScanDirectReady");

    WlanQtUtils* utils = mWizard->wlanQtUtils();

    Q_ASSERT(utils);

    QList<WlanScanResult> filteredResults;

    if (scanStatus == WlanQtUtils::ScanStatusOk) {
        QList< QSharedPointer<WlanQtUtilsAp> > directScanResults;
        // Read the directed scan results from Qt Utils
        utils->availableWlanAps(directScanResults);

        // Identify open and hidden networks.
        getFinalScanResults(directScanResults, mWlanApList, filteredResults);
    }

    // Process the scan results. If scan status is not ok, feed an empty
    // result table to the selection function.
    selectNextPageActions(filteredResults);
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_WLANSCANDIRECTREADY_EXIT);
}

/*!
 * All wlan channels have been scanned for public networks. After processing the
 * results a direct scan is initiated. This function also checks whether the
 * scan was succesful, canceled or failed. In case the scan was not succesful,
 * always proceed to manual wizard.
 */
void WlanWizardPageScanning::wlanScanApReady()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_WLANSCANAPREADY_ENTRY);
    
    OstTrace0(
        TRACE_NORMAL,
        WLANWIZARDPAGESCANNING_WLANSCANAPREADY,
        "WlanWizardPageScanning::wlanScanApReady");

    if (mScanStatus == WlanQtUtils::ScanStatusOk) {
        WlanQtUtils* utils = mWizard->wlanQtUtils();
        Q_ASSERT(utils);

        QList<QSharedPointer<WlanQtUtilsAp> > openScanResults;

        // Fetch the list of scan results.
        utils->availableWlanAps(openScanResults);

        // The name of the network that we are looking for.
        QString ssid =
            mWizard->configuration(WlanWizardPrivate::ConfSsid).toString();

        // Store matching scan results.
        getSsidMatchList(ssid, openScanResults);

        // Next directed scan.
        utils->scanWlanDirect(ssid);
    }
    else {
        mWizard->clearConfiguration(WlanWizardHelper::ConfAvailableNetworkOptions);
        mNextPageId = WlanWizardPageInternal::PageNetworkMode;
        mWizard->nextPage();
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_WLANSCANAPREADY_EXIT);
}

/*!
 * This function stores all the APs that match ssid parameter to mWlanApList
 * member list.
 * @param [in] ssid is the name of the network we are interested in.
 * @param [in] matchList is the list of all APs that were revealed in the scan.
 */
void WlanWizardPageScanning::getSsidMatchList(
    QString ssid,
    const QList<QSharedPointer<WlanQtUtilsAp> > &matchList)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_GETSSIDMATCHLIST_ENTRY);
    
    mWlanApList.clear();
    QSharedPointer<WlanQtUtilsAp> item;

    for (int i = 0; i < matchList.size(); i++) {
        item = matchList.at(i);
        if (item->value(WlanQtUtilsAp::ConfIdSsid).toString() == ssid) {
            mWlanApList.append(item);
        }
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_GETSSIDMATCHLIST_EXIT);
}

/*
 * Find out which of the direct scan results are open networks by comparing
 * result to open scan results. Return finalResults.
 * @param [in] directScanResults are the results of a directed scan.
 * @param [in] openScanResults are the results of a scan inquiring any APs.
 * @param [out] finalResults is a list of APs with visibility flag included.
 */
void WlanWizardPageScanning::getFinalScanResults(
    const QList<QSharedPointer<WlanQtUtilsAp> > &directScanResults,
    const QList<QSharedPointer<WlanQtUtilsAp> > &openScanResults,
    QList<WlanScanResult> &finalResults)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_GETFINALSCANRESULTS_ENTRY);
    
    finalResults.clear();

    // The key assumption of this algorithm is that both direct and public scan
    // reveal the same set of AP:s with the exception of hidden AP:s.
    // In other words: Direct scan result set is the whole set with each AP 
    // marked as hidden by default. If an AP has been revealed also by public
    // scan, it is changed to visible.
    for (int i = 0; i < directScanResults.size(); i++) {
        WlanScanResult resultItem;

        resultItem.scanResult = directScanResults.at(i);
        
        // Initialize as hidden.
        resultItem.networkHidden = true;
         
        for (int j = 0; j < openScanResults.size(); j++) {
            if (WlanQtUtilsAp::compare(
                openScanResults.at(j).data(), 
                directScanResults.at(i).data()) == 0) {
                // Mark open, if a public scan revealed the same AP
                resultItem.networkHidden = false;
            }
        }

        finalResults.append(resultItem);
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_GETFINALSCANRESULTS_EXIT);
}

/*!
 * This function processes the scan results in case multiple results were
 * acquired with matching SSIDs.
 * @param [in] finalResults is the list of APs revealed by the last scans.
 * @return the identifier of the next page.
 */
int WlanWizardPageScanning::processMultipleScanResults(
    const QList<WlanScanResult> &finalResults)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_PROCESSMULTIPLESCANRESULTS_ENTRY);
    
    WlanWizardScanList networkOptions;
    int nextPage;

    // Build the list of results to be placed in the configuration.
    networkOptions.build(finalResults);
    
    QVariant tmp;
    tmp.setValue(networkOptions);

    mWizard->setConfiguration(
        WlanWizardHelper::ConfAvailableNetworkOptions, tmp);

    if (networkOptions.wpsSelected()) {
        // In case all available network mode options support wps, go directly
        // to wps-wizard.
        nextPage = WlanWizardPageInternal::PageWpsStart;
    } else if (networkOptions.netModes() > SingleResult) {
        // If there are more than one network mode choices, go to manual selection
        // of network mode.
        nextPage = WlanWizardPageInternal::PageNetworkMode;
    } else {
        // if only a single network mode option is available, the mode query
        // may be skipped and the next page is set to security mode query.
        WlanNetworkSetting setting = networkOptions.getNetModes().at(0);
        mWizard->setConfiguration(WlanWizardHelper::ConfNetworkMode, setting.mode);
        mWizard->setConfiguration(WlanWizardHelper::ConfWlanScanSSID, setting.hidden);
        mWizard->setConfiguration(WlanWizardHelper::ConfWpsSupported, setting.wpsSupported);
        nextPage = WlanWizardPageInternal::PageNetworkSecurity;
    }
    
    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_PROCESSMULTIPLESCANRESULTS_EXIT);
    return nextPage;
}

/*!
 * This function decides what to do with the scan results.
 * @param [in] finalResults is the list of APs revealed by the last scans.
 */
void WlanWizardPageScanning::selectNextPageActions(
    const QList<WlanScanResult> &finalResults)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGESCANNING_SELECTNEXTPAGEACTIONS_ENTRY);

    mWizard->clearConfiguration(WlanWizardHelper::ConfAvailableNetworkOptions);

    if (finalResults.isEmpty()) {
        // In case of no results at all were found.
        mNextPageId = WlanWizardPageInternal::PageNetworkMode;
    } else if (finalResults.size() == SingleResult) {
        // In case there is only a single result.
        WlanScanResult item = finalResults.first();
        mNextPageId = mWizard->getNextPageId(
            item.scanResult->value(WlanQtUtilsAp::ConfIdName).toString(),
            item.scanResult->value(WlanQtUtilsAp::ConfIdSsid).toByteArray(),
            item.scanResult->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt(),
            item.scanResult->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(),
            item.scanResult->value(WlanQtUtilsAp::ConfIdWpaPskUse).toBool(),
            item.networkHidden,
            item.scanResult->value(WlanQtUtilsAp::ConfIdWpsSupported).toBool());
    } else {
        // In case more than one result is available.
        mWizard->setConfiguration(WlanWizardHelper::ConfProcessSettings, true);
        mNextPageId = processMultipleScanResults(finalResults);
    }

    mWizard->nextPage();

    OstTraceFunctionExit0(WLANWIZARDPAGESCANNING_SELECTNEXTPAGEACTIONS_EXIT);
}
