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
* WLAN Wizard Page: Processing settings, connection and running ICT.
*/

// System includes
#include <HbMainWindow>
#include <HbDocumentLoader>
#include <HbWidget>
#include <HbLabel>
#include <HbProgressBar>
#include <HbParameterLengthLimiter>
#include <wlanerrorcodes.h>

// User includes
#include "wlanwizardplugin.h"
#include "wlanqtutils.h"
#include "wlanwizard_p.h"
#include "wlanwizardpageprocessingsettings.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardpageprocessingsettingsTraces.h"
#endif

/*!
   \class WlanWizardPageProcessingSettings
   \brief Implements Processing settings wizard page.
   
   In this page 
   - possible wizard plugin settings are stored
   - connection to the wlan ap is established
   - ICT (Internet connectivity test) is run 
 */

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
   Constructor.
   
   @param [in] parent pointer to private implementation of wizard.
 */
WlanWizardPageProcessingSettings::WlanWizardPageProcessingSettings(
    WlanWizardPrivate* parent) :
    WlanWizardPageInternal(parent), 
    mDocLoader(NULL),
    mWidget(NULL), 
    mLabel(NULL), 
    mNextPageId(WlanWizardPageInternal::PageSummary)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANWIZARDPAGEPROCESSINGSETTINGS_ENTRY);
    
    WlanQtUtils* utils = mWizard->wlanQtUtils();

    bool ok;
    ok = connect(
        utils,
        SIGNAL(wlanNetworkOpened(int)), 
        this,
        SLOT(wlanNetworkOpened(int)));
    Q_ASSERT(ok);
    
    ok = connect(
        utils,
        SIGNAL(wlanNetworkClosed(int,int)), 
        this,
        SLOT(wlanNetworkClosed(int,int)));
    Q_ASSERT(ok);
    
    ok = connect(
        utils,
        SIGNAL(ictResult(int,int)), 
        this,
        SLOT(ictResult(int,int)));
    Q_ASSERT(ok);

    Q_UNUSED(ok);
    Q_ASSERT(ok);
    
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANWIZARDPAGEPROCESSINGSETTINGS_EXIT);
}

/*!
   Destructor.
 */
WlanWizardPageProcessingSettings::~WlanWizardPageProcessingSettings()
{
    OstTraceFunctionEntry0(DUP1_WLANWIZARDPAGEPROCESSINGSETTINGS_WLANWIZARDPAGEPROCESSINGSETTINGS_ENTRY);
    
    // signals are automatically disconnected
    delete mDocLoader;
    
    // Wizard framework deletes the visualization (owns mWidget).
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGEPROCESSINGSETTINGS_WLANWIZARDPAGEPROCESSINGSETTINGS_EXIT);
}

/*!
   See WlanWizardPage::initializePage()
 */
HbWidget* WlanWizardPageProcessingSettings::initializePage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_INITIALIZEPAGE_ENTRY);
    
    if (!mWidget) {
        mDocLoader = new HbDocumentLoader(mWizard->mainWindow());
        
        bool ok;
        mDocLoader->load(":/docml/occ_add_wlan_06.docml", &ok);
        Q_ASSERT(ok);
        loadDocmlSection(mWizard->mainWindow()->orientation());
        
        mWidget = qobject_cast<HbWidget*> (mDocLoader->findWidget("occ_add_wlan_06"));
        Q_ASSERT(mWidget != NULL);

        mLabel = qobject_cast<HbLabel*> (mDocLoader->findWidget("dialog"));
        Q_ASSERT(mLabel != NULL);

        ok = connect(
            mWizard->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(loadDocmlSection(Qt::Orientation)));
        Q_ASSERT(ok);
    }
    
    mLabel->setPlainText(
        HbParameterLengthLimiter(
            "txt_occ_dialog_checking_connection_to_1").arg(
                mWizard->configuration(WlanWizardPrivate::ConfName).toString()));

    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_INITIALIZEPAGE_EXIT);
    return mWidget;
}

/*!
   See WlanWizardPage::nextId()
   
   @param [out] removeFromStack return value is always true
   
   @return WlanWizardPageInternal::PageSummary
 */
int WlanWizardPageProcessingSettings::nextId(bool &removeFromStack) const
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_NEXTID_ENTRY);
    
    removeFromStack = true;
    
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_NEXTID_EXIT);
    return mNextPageId;
}


/*!
   See WlanWizardPage::showPage()
   
   @return false. Next button is dimmed when the page is displayed.
 */
bool WlanWizardPageProcessingSettings::showPage()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_SHOWPAGE_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_SHOWPAGE_EXIT);
    return false;
}

/*!
   Loads docml at initialization phase and when HbMainWindow sends orientation()
   signal.
   
   @param [in] orientation to be loaded.
 */
void WlanWizardPageProcessingSettings::loadDocmlSection(Qt::Orientation orientation)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_LOADDOCMLSECTION_ENTRY);
    
    WlanWizardPageInternal::loadDocmlSection(
        mDocLoader,
        orientation,
        ":/docml/occ_add_wlan_06.docml", 
        "portrait_section",
        "landscape_section");
    
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_LOADDOCMLSECTION_EXIT);
}

/*!
   This method is connected to WlanQtUtils::wlanNetworkClosed(int, int) signal.
   to get disconnected status events of currently established connection.
   
   In case of failure, movement to generic error page is triggered.
   
   @param [in] iapId IAP ID
   @param [in] reason Symbian Error code.
 */
void WlanWizardPageProcessingSettings::wlanNetworkClosed(int iapId, int reason)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKCLOSED_ENTRY);
    
    OstTraceExt2(
        TRACE_BORDER,
        WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKCLOSED, 
        "WlanWizardPageProcessingSettings::wlanNetworkClosed;iapId=%d;reason=%d",
        iapId,
        reason);
        
    if (iapId != mWizard->configuration(WlanWizardHelper::ConfIapId).toInt()) {
        OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKCLOSED_EXIT);
        return;
    }

    mWizard->setConfiguration(WlanWizardPrivate::ConfIctStatus, false);
    mWizard->setConfiguration(WlanWizardPrivate::ConfConnected, false);
    
    if (mWizard->isCurrentPage(mWidget)) {
        QString errorText;
        switch (reason) {
        case KErrWlanOpenAuthFailed:
        case KErrWlanSharedKeyAuthRequired:
        case KErrWlanWpaAuthRequired:
        case KErrWlanWpaAuthFailed:
        case KErrWlan802dot1xAuthFailed:
        case KErrWlanIllegalEncryptionKeys:
        case KErrWlanPskModeRequired:
        case KErrWlanEapModeRequired:
            errorText = hbTrId("txt_occ_dialog_authentication_unsuccessful");
            break;
            
        case KErrWlanIllegalWpaPskKey:
            errorText = hbTrId("txt_occ_dialog_incorrect_wpa_preshared_key_pleas");
            break;
            
        case KErrWlanSharedKeyAuthFailed:
            errorText = hbTrId("txt_occ_dialog_incorrect_wep_key_please_check_the");
            break;

        default:
            // Handles also KErrNone
            // Get plugin specific localized error text if any
            WlanWizardPlugin *plugin = mWizard->wlanWizardPlugin();
            
            if (plugin){
                errorText = plugin->errorString(reason);
            }
            if (errorText.length() == 0) {
                errorText = hbTrId("txt_occ_dialog_connection_failed");
            }
            break;
        }
        mNextPageId = WlanWizardPageInternal::PageGenericError;
        mWizard->setConfiguration(
            WlanWizardHelper::ConfGenericErrorString, errorText);
        
        mWizard->setConfiguration(
            WlanWizardHelper::ConfGenericErrorPageStepsBackwards, 
            WlanWizardPage::OneStepBackwards);
        
        mWizard->nextPage();
    }
    
    OstTrace0( 
        TRACE_BORDER,
        WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKCLOSED_DONE,
        "WlanWizardPageProcessingSettings::wlanNetworkClosed - Done");
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKCLOSED_EXIT);
}

/*!
   Handles ictResult(int, bool) signal from WlanQtUtils and calls
   WlanWizardHelper::nextPage() to trigger movement to next page.
   
   @param [in] iapId IAP ID
   @param [in] reason ICT result, Symbian error code
 */
void WlanWizardPageProcessingSettings::ictResult(int iapId, int reason)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_ICTRESULT_ENTRY);
    
    OstTraceExt2( 
        TRACE_BORDER, 
        WLANWIZARDPAGEPROCESSINGSETTINGS_ICTRESULT, 
        "WlanWizardPageProcessingSettings::ictResult;iapId=%d;result=%d", 
        iapId,
        reason);
    
    if (iapId != mWizard->configuration(WlanWizardHelper::ConfIapId).toInt()) {
        OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_ICTRESULT_EXIT);
        return;
    }
    
    // Cast of result .
    WlanQtUtils::IctStatus status = 
        static_cast<WlanQtUtils::IctStatus>(reason);

    if(status == WlanQtUtils::IctCancelled) {
        
        mNextPageId = WlanWizardPageInternal::PageNone;
        mWizard->cancelTriggered();
    }
    else {
        mWizard->setConfiguration(WlanWizardPrivate::ConfConnected, true);
    }
    mWizard->setConfiguration(WlanWizardPrivate::ConfIctStatus, status);
        
    mWizard->nextPage();
    
    OstTrace0(
        TRACE_BORDER,
        WLANWIZARDPAGEPROCESSINGSETTINGS_ICTRESULT_DONE, 
        "WlanWizardPageProcessingSettings::ictResult - Done");
    
    OstTraceFunctionExit0(DUP1_WLANWIZARDPAGEPROCESSINGSETTINGS_ICTRESULT_EXIT);
}

/*!
   Handles wlanNetworkOpened(int) signal from WlanQtUtils.
   
   @param [in] iapId IAP ID
 */
void WlanWizardPageProcessingSettings::wlanNetworkOpened(int iapId)
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKOPENED_ENTRY);
    
    Q_UNUSED(iapId);
    OstTrace1(
        TRACE_BORDER, 
        WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKOPENED, 
        "WlanWizardPageProcessingSettings::wlanNetworkOpened;iapId=%d", 
        iapId);
    
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_WLANNETWORKOPENED_EXIT);
}

/*!
   See WlanWizardPage::requiresStartOperation().
   
   Indicates to framework that startOperation() needs to called.
 */
bool WlanWizardPageProcessingSettings::requiresStartOperation()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_REQUIRESSTARTOPERATION_ENTRY);
    OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_REQUIRESSTARTOPERATION_EXIT);
    return true;
}

/*!
   See WlanWizardPage::startOperation().
   
   Starts the page operation.
 */
void WlanWizardPageProcessingSettings::startOperation()
{
    OstTraceFunctionEntry0(WLANWIZARDPAGEPROCESSINGSETTINGS_STARTOPERATION_ENTRY);
    
    if (mWizard->handleIap()) {
        mNextPageId = WlanWizardPageInternal::PageSummary;
        
        // in case wlan connection is already established disconnect it
        // note that connectIap() disconnects implicitly connection when 
        // same IAP is already connected.
        WlanQtUtils* utils = mWizard->wlanQtUtils();
        int connectedIapId = utils->activeIap();
        int iapId = mWizard->configuration(WlanWizardHelper::ConfIapId).toInt();
        if (connectedIapId != iapId){
            utils->disconnectIap(connectedIapId);
        }
        mWizard->wlanQtUtils()->connectIap( iapId, true);        
    } else {
        mNextPageId = WlanWizardPageInternal::PageGenericError;
        mWizard->setConfiguration(
            WlanWizardHelper::ConfGenericErrorString, 
            hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret"));
        
        mWizard->setConfiguration(
            WlanWizardHelper::ConfGenericErrorPageStepsBackwards, 
            WlanWizardPage::OneStepBackwards);
        mWizard->nextPage();
    }
    
	OstTraceFunctionExit0(WLANWIZARDPAGEPROCESSINGSETTINGS_STARTOPERATION_EXIT);
}
