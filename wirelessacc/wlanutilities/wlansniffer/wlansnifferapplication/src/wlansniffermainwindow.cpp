/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Sniffer main window. 
*/

// System includes

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"

#include "wlanwizard.h"

#include "wlansnifferengine.h"
#include "wlansnifferlistview.h"
#include "wlansniffermainwindow.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansniffermainwindowTraces.h"
#endif

/*!
    \class WlanSnifferMainWindow
    \brief WLAN Sniffer main window. 
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] engine WLAN Sniffer application engine.
 */

WlanSnifferMainWindow::WlanSnifferMainWindow(WlanSnifferEngine *engine) :
    mWizard(0),
    mListView(0),
    mEngine(engine)
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_WLANSNIFFERMAINWINDOW_ENTRY);

    // Add the list view to the main window
    addListView();
    bool connectStatus = connect(
        mEngine,
        SIGNAL(toListView(QString)),
        this,
        SLOT(toListView(QString)));
    Q_ASSERT(connectStatus == true);
    
    // When using WLAN Sniffer service, the view show is called after the service
    // request arrives.
    if (!mEngine->isEmbedded()) {
        // Show the list view
        toListView(hbTrId("txt_occ_title_wireless_lan"));
    }

    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_WLANSNIFFERMAINWINDOW_EXIT);
}

/*!
    Destructor.
 */

WlanSnifferMainWindow::~WlanSnifferMainWindow()
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_WLANSNIFFERMAINWINDOWDESTR_ENTRY);
    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_WLANSNIFFERMAINWINDOWDESTR_EXIT);
}

/*!
    Shows the WLAN Sniffer list view.
    
    @param [in] title View title.
 */

void WlanSnifferMainWindow::toListView(const QString &title)
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_TOLISTVIEW_ENTRY);
    
    // Show the list view
    mListView->setTitle(title);
    setCurrentView(mListView);
    show();

    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_TOLISTVIEW_EXIT);
}

/*!
    Creates and adds the WLAN List View to main window.
 */

void WlanSnifferMainWindow::addListView()
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_ADDLISTVIEW_ENTRY);
    
    mListView = new WlanSnifferListView(mEngine, this);
    addView(mListView);

    bool connectStatus = connect(
        mListView,
        SIGNAL(wizardTriggered(const WlanQtUtilsAp *)),
        this,
        SLOT(startWlanWizard(const WlanQtUtilsAp *)));
    
    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_ADDLISTVIEW_EXIT);
}

/*!
    Starts WLAN Wizard for new WLAN IAP creation.
    
    @param [in] ap WLAN Access Point to create, or NULL for adding WLAN manually.
 */

void WlanSnifferMainWindow::startWlanWizard(const WlanQtUtilsAp *ap)
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_STARTWLANWIZARD_ENTRY);
    
    if (mWizard == NULL) {
        // Stop WLAN scanning for the duration of WLAN Wizard
        mEngine->stopWlanScanning();
        
        mWizard = QSharedPointer<WlanWizard>(new WlanWizard(this));
        bool connectStatus = connect(
            mWizard.data(),
            SIGNAL(finished(int,bool)),
            this,
            SLOT(handleWlanWizardComplete(int,bool)),
            Qt::QueuedConnection);
        Q_ASSERT(connectStatus == true);
        
        connectStatus = connect(
            mWizard.data(),
            SIGNAL(cancelled()),
            this,
            SLOT(handleWlanWizardCancelled()),
            Qt::QueuedConnection);
        Q_ASSERT(connectStatus == true);
        
        // Create an IAP for a specific AP
        if (ap) {
            mWizard->setParameters(
                ap->value(WlanQtUtilsAp::ConfIdName).toString(),
                ap->value(WlanQtUtilsAp::ConfIdSsid).toByteArray(),
                ap->value(WlanQtUtilsAp::ConfIdConnectionMode).toInt(),
                ap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(),
                ap->value(WlanQtUtilsAp::ConfIdWpaPskUse).toInt(),
                ap->value(WlanQtUtilsAp::ConfIdWpsSupported).toBool());
        }
        // else: Add WLAN IAP manually
        
        mWizard->show();
    }
    // else: already running Wizard, ignore this call
    
    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_STARTWLANWIZARD_EXIT);
}

/*!
    WLAN Wizard successful completion handler.
    
    @param [in] iapId IAP ID of the new WLAN IAP.
    @param [in] connected TRUE if connected.  
 */

void WlanSnifferMainWindow::handleWlanWizardComplete(
    int iapId,
    bool connected)
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_HANDLEWLANWIZARDCOMPLETE_ENTRY);

    // The wizard must exist
    Q_ASSERT(mWizard);
    
    // Enable scanning again
    mEngine->startWlanScanning();

    if (connected) {
        // The IAP ID must be valid
        Q_ASSERT(iapId != WlanQtUtils::IapIdNone);
        
        // Connect (share) the new IAP in order to keep the connection open when
        // deleting Wizard.
        mEngine->wlanQtUtils()->connectIap(iapId, false);
    }
    // else: created IAP not connected at all, or already dropped
    // (probably due to being out of coverage) so don't try to share it
    
    // Delete the Wizard instance. This is OK since the connect is Qt::QueuedConnection.
    mWizard.clear();
    
    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_HANDLEWLANWIZARDCOMPLETE_EXIT);
}

/*!
    WLAN Wizard cancellation handler.
 */

void WlanSnifferMainWindow::handleWlanWizardCancelled()
{
    OstTraceFunctionEntry0(WLANSNIFFERMAINWINDOW_HANDLEWLANWIZARDCANCELLED_ENTRY);

    // The wizard must exist
    Q_ASSERT(mWizard);
    
    // Enable scanning again
    mEngine->startWlanScanning();
    
    // Delete the Wizard instance. This is OK since the connect is Qt::QueuedConnection.
    mWizard.clear();
    
    OstTraceFunctionExit0(WLANSNIFFERMAINWINDOW_HANDLEWLANWIZARDCANCELLED_EXIT);
}
