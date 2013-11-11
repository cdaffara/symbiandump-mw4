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
* WLAN Sniffer List View. 
*/

// System includes

#include <QGraphicsWidget>
#include <QSharedPointer>
#include <QObjectList>

#include <HbLabel>
#include <HbPushButton>
#include <HbMenu>
#include <HbAction>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbDocumentLoader>
#include <HbInstance>
#include <HbMessageBox>
#include <HbParameterLengthLimiter>

#include <xqserviceutil.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"

#include "wlansnifferengine.h"
#include "wlansniffermainwindow.h"
#include "wlansnifferlistview.h"
#include "wlansnifferlistitem.h"
#include "wlansnifferlistwidget.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansnifferlistviewTraces.h"
#endif

/*!
    \class WlanSnifferListView
    \brief WLAN Sniffer application's list view implementation.
*/

// External function prototypes

// Local constants

//! WLAN Sniffer list view docml file location
static const QString WlanSnifferListViewDocml(":/docml/wlansnifferlistview.docml");

//! WLAN Sniffer list view object name
static const QString WlanSnifferListViewName("occ_list");

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] engine WLAN Sniffer application engine.
    @param [in] mainWindow WLAN Sniffer main window.
*/

WlanSnifferListView::WlanSnifferListView(
    WlanSnifferEngine *engine,
    WlanSnifferMainWindow *mainWindow) :
    HbView(),
    mDocLoader(new HbDocumentLoader(mainWindow)),
    mWlanListWidget(),
    mContextMenu(),
    mContextMenuData(),
    mWlanEnableDialog(),
    mIgnoreWlanScanResults(false),
    mConnectingIapId(WlanQtUtils::IapIdNone),
    mEngine(engine),
    mSwitchWlanAction(0),
    mAddWlanAction(0),
    mWlanList(0),
    mStatusLabel(0),
    mWlanButton(0)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_WLANSNIFFERLISTVIEW_ENTRY);

    // Initialize UI from the docml based on standalone/embedded status
    loadDocml(mEngine->isEmbedded());

    if (mEngine->isEmbedded()) {
        // We need to create a custom navigation action and handle exiting
        // from the application differently in case the application was
        // launched as embedded via QtHighway
        HbAction *completeAction = new HbAction(Hb::BackNaviAction, this);
        setNavigationAction(completeAction);
        bool connectStatus = connect(
            completeAction,
            SIGNAL(triggered(bool)),
            mEngine,
            SLOT(completeService()));
        Q_ASSERT(connectStatus);
    }
    
    // Connect WLAN network open & close signals
    bool connectStatus = connect(
        mEngine->wlanQtUtils(),
        SIGNAL(wlanNetworkOpened(int)),
        this,
        SLOT(updateConnectionOpened(int)));
    Q_ASSERT(connectStatus);
    connectStatus = connect(
        mEngine->wlanQtUtils(),
        SIGNAL(wlanNetworkClosed(int, int)),
        this,
        SLOT(updateConnectionClosed(int))); // "reason" parameter ignored
    Q_ASSERT(connectStatus);
    
    // Connect WLAN ON/OFF setting change signal
    connectStatus = connect(
        mEngine,
        SIGNAL(masterWlanStatus(bool)),
        this,
        SLOT(updateWlanEnabled()));
    Q_ASSERT(connectStatus);
    
    // Connect Force Disable WLAN setting change signal
    connectStatus = connect(
        mEngine,
        SIGNAL(forceDisableWlanStatus(bool)),
        this,
        SLOT(updateWlanEnabled()));
    Q_ASSERT(connectStatus);

    // Connect signals to catch user interaction with the WLAN network list
    connectStatus = connect(
        mWlanList,
        SIGNAL(activated(HbListWidgetItem *)),
        this,
        SLOT(handleListItemActivated(HbListWidgetItem *)));
    Q_ASSERT(connectStatus);
    connectStatus = connect(
        mWlanList,
        SIGNAL(longPressed(HbListWidgetItem *, QPointF)),
        this,
        SLOT(handleListItemLongPressed(HbListWidgetItem *, QPointF)));
    Q_ASSERT(connectStatus);

    // Connect signals to catch user interaction with WLAN ON/OFF switching
    connectStatus = connect(
        mWlanButton,
        SIGNAL(clicked(bool)),
        this,
        SLOT(handleWlanToggled()));
    Q_ASSERT(connectStatus);
    connectStatus = connect(
        mSwitchWlanAction,
        SIGNAL(triggered(bool)),
        this,
        SLOT(handleWlanToggled()));
    Q_ASSERT(connectStatus);

    // Connect adding WLAN manually
    connectStatus = connect(
        mAddWlanAction,
        SIGNAL(triggered(bool)),
        this,
        SLOT(startWlanWizard()));
    Q_ASSERT(connectStatus);
    
    // Connect WLAN scan results signal
    connectStatus = connect(
        mEngine,
        SIGNAL(wlanScanReady()),
        this,
        SLOT(updateListContent()));
    Q_ASSERT(connectStatus);

    // Set the initial value of WLAN state
    updateWlanEnabled();

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_WLANSNIFFERLISTVIEW_EXIT);
}

/*!
    Destructor.
*/

WlanSnifferListView::~WlanSnifferListView()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_WLANSNIFFERLISTVIEWDESTR_ENTRY);
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_WLANSNIFFERLISTVIEWDESTR_EXIT);
}

/*!
    Loading of WLAN Sniffer List View docml.
    
    @param [in] isEmbedded TRUE if WLAN Sniffer is launched as embedded. 
*/

void WlanSnifferListView::loadDocml(bool isEmbedded)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_LOADDOCML_ENTRY);

    // Pass the view to the document loader. Document loader uses this view
    // when docml is parsed, instead of creating a new view.
    setObjectName(WlanSnifferListViewName);
    QObjectList objectList;
    objectList.append(this);
    mDocLoader->setObjectTree(objectList);

    // First load the common section
    bool ok = false;
    mDocLoader->load(WlanSnifferListViewDocml, &ok);
    Q_ASSERT(ok);
    
    // Then load the mode specific section
    if (isEmbedded) {
        mDocLoader->load(WlanSnifferListViewDocml, "embedded", &ok);
    } else {
        mDocLoader->load(WlanSnifferListViewDocml, "standalone", &ok);
    }
    Q_ASSERT(ok);
    
    // Set view menu
    HbMenu *viewMenu = qobject_cast<HbMenu *>(mDocLoader->findWidget("viewMenu"));
    Q_ASSERT(viewMenu);
    setMenu(viewMenu);

    // WLAN Sniffer list widget takes responsibility of the list widget behaviour
    mWlanList = qobject_cast<HbListWidget *>(mDocLoader->findWidget("listWidget"));
    Q_ASSERT(mWlanList);
    mWlanListWidget = QSharedPointer<WlanSnifferListWidget>(new WlanSnifferListWidget(mWlanList));

    // Retrieve pointers to widgets we need to access from the code
    mSwitchWlanAction = qobject_cast<HbAction *>(mDocLoader->findObject("switchWlanAction"));
    Q_ASSERT(mSwitchWlanAction);

    mAddWlanAction = qobject_cast<HbAction *>(mDocLoader->findObject("addWlanAction"));
    Q_ASSERT(mAddWlanAction);

    mWlanButton = qobject_cast<HbPushButton *>(mDocLoader->findWidget("wlanButton"));
    Q_ASSERT(mWlanButton);
    
    mStatusLabel = qobject_cast<HbLabel *>(mDocLoader->findWidget("statusLabel"));
    Q_ASSERT(mStatusLabel);
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_LOADDOCML_EXIT);
}

/*!
    Updates WLAN status label based on current WLAN status.
    
    @param [in] enabled True if WLAN is enabled.
*/

void WlanSnifferListView::updateWlanStatusLabel(bool enabled)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_UPDATEWLANSTATUSLABEL_ENTRY);
    
    QString status;
    if (!enabled) {
        // WLAN is OFF
        status = hbTrId("txt_occ_grid_wlan_is_switched_off");
    } else {
        int iapId = mEngine->wlanQtUtils()->activeIap();
        
        switch (mEngine->wlanQtUtils()->connectionStatus()) {
        case WlanQtUtils::ConnStatusConnecting:
            // WLAN is connecting
            Q_ASSERT(iapId != WlanQtUtils::IapIdNone);
            status = HbParameterLengthLimiter(
                "txt_occ_grid_connecting_to_1").arg(
                    mEngine->wlanQtUtils()->iapName(iapId));
            break;
            
        case WlanQtUtils::ConnStatusConnected:
            // WLAN is connected
            Q_ASSERT(iapId != WlanQtUtils::IapIdNone);
            status = HbParameterLengthLimiter(
                "txt_occ_grid_connected_to_1").arg(
                    mEngine->wlanQtUtils()->iapName(iapId));
            break;
            
        default:
            // WLAN is disconnected
            status = hbTrId("txt_occ_grid_not_connected");        
            break;
        }
    }
    mStatusLabel->setPlainText(status);

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_UPDATEWLANSTATUSLABEL_EXIT);
}

/*!
    Handles a "Connect" action for the selected IAP or AP item.
    
    @param data IAP ID (int), or AP class (WlanQtUtilsAp) to connect.
*/

void WlanSnifferListView::handleConnect(QVariant data)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLECONNECT_ENTRY);

    // Check whether we need to disconnect a previous connection first
    int activeWlanId = mEngine->wlanQtUtils()->activeIap();
    
    // Get IAP ID if IAP is available
    int iapId = WlanQtUtils::IapIdNone;
    if (data.canConvert<int>()) {
        iapId = data.toInt();
    }
    OstTraceExt2(
        TRACE_NORMAL,
        WLANSNIFFERLISTVIEW_HANDLECONNECT,
        "WlanSnifferListView::handleConnect;activeWlanId=%d;iapId=%d",
        activeWlanId,
        iapId);
    
    // Skip connecting if the IAP is already connected
    if (activeWlanId != WlanQtUtils::IapIdNone && iapId == activeWlanId) {
        return;
    }
    
    if (activeWlanId != WlanQtUtils::IapIdNone) {
        mEngine->wlanQtUtils()->disconnectIap(activeWlanId);
        
        // Update list widget so that the IAP is no longer connected
        updateListContent();
    }
    
    if (iapId != WlanQtUtils::IapIdNone) {
        // Item was an IAP, connect it.
        mConnectingIapId = iapId;
        mStatusLabel->setPlainText(
            HbParameterLengthLimiter(
                "txt_occ_grid_connecting_to_1").arg(
                    mEngine->wlanQtUtils()->iapName(iapId)));
        mEngine->wlanQtUtils()->connectIap(iapId);
    } else {
        // Item is a WLAN AP. Summon the Wlan Wizard to handle creation
        // of the IAP and connecting it.
        Q_ASSERT(data.canConvert<WlanQtUtilsAp>());
        WlanQtUtilsAp ap = data.value<WlanQtUtilsAp>();
        emit wizardTriggered(&ap);
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLECONNECT_EXIT);
}

/*!
    Updates WLAN list widget with new WLAN scan results. 
*/

void WlanSnifferListView::updateListContent()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_UPDATELISTCONTENT_ENTRY);

    if (!mIgnoreWlanScanResults) {
        // Get the latest scan results
        QList< QSharedPointer<WlanQtUtilsAp> > iaps;
        QList< QSharedPointer<WlanQtUtilsAp> > aps;
        mEngine->wlanQtUtils()->availableWlans(iaps, aps);
    
        // Check for connected IAP
        int iapId = WlanQtUtils::IapIdNone;
        if (mEngine->wlanQtUtils()->connectionStatus() ==
            WlanQtUtils::ConnStatusConnected) {
            iapId = mEngine->wlanQtUtils()->activeIap();
        }
    
        // Let the list widget class update the list content
        mWlanListWidget->updateContent(iaps, aps, iapId);
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_UPDATELISTCONTENT_EXIT);
}

/*!
    WLAN state settings change handler. This slot handles both the
    WLAN ON/OFF setting and WLAN forced OFF setting.
    Updates all WLAN ON/OFF related (UI) elements.
*/

void WlanSnifferListView::updateWlanEnabled()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_UPDATEWLANENABLED_ENTRY);
    
    // Check the updated setting status
    bool forcedOff = mEngine->forceDisableWlan();
    bool enabled = mEngine->masterWlan() && !forcedOff;
    
    // Update the WLAN status label
    updateWlanStatusLabel(enabled);

    // Set the WLAN ON/OFF button state
    mWlanButton->setChecked(enabled);

    // Select the right WLAN button icon and menu action text
    if (enabled) {
        mWlanButton->setIcon(HbIcon("qtg_mono_wlan"));
        mSwitchWlanAction->setText(hbTrId("txt_occ_opt_switch_wlan_off"));
    } else {
        mWlanButton->setIcon(HbIcon("qtg_mono_wlan_offline"));
        mSwitchWlanAction->setText(hbTrId("txt_occ_opt_switch_wlan_on"));
    }

    // Set triggable WLAN UI elements disabled if WLAN is forced OFF
    mWlanButton->setEnabled(!forcedOff);
    mSwitchWlanAction->setEnabled(!forcedOff);

    // Disable manual WLAN IAP creation when WLAN is switched OFF
    mAddWlanAction->setEnabled(enabled);

    // Switch WLAN scanning ON/OFF
    if (enabled) {
        mEngine->startWlanScanning();
    } else {
        mEngine->stopWlanScanning();
        // Clear the network list when going to offline
        mWlanList->clear();
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_UPDATEWLANENABLED_EXIT);
}

/*!
    Connection opened slot. Updates connection status.
    
    @param [in] iapId IAP ID of the connected WLAN IAP. 
*/

void WlanSnifferListView::updateConnectionOpened(int iapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_UPDATECONNECTIONOPENED_ENTRY);
    
    mConnectingIapId = WlanQtUtils::IapIdNone;
    mStatusLabel->setPlainText(
        HbParameterLengthLimiter(
            "txt_occ_grid_connected_to_1").arg(
                mEngine->wlanQtUtils()->iapName(iapId)));
    
    // Update the list widget content
    updateListContent();

    // Scroll to the top of the list
    mWlanListWidget->scrollTo(0);
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_UPDATECONNECTIONOPENED_EXIT);
}

/*!
    Connection closing handler. Updates connection status.
    
    @param [in] iapId Disconnected IAP ID. 
*/

void WlanSnifferListView::updateConnectionClosed(int iapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_UPDATECONNECTIONCLOSED_ENTRY);
    
    if (mEngine->masterWlan() && !mEngine->forceDisableWlan()) {
        // Check whether we can update the status text to "Not connected"
        if (mConnectingIapId == WlanQtUtils::IapIdNone ||
            mConnectingIapId == iapId) {
            mStatusLabel->setPlainText(hbTrId("txt_occ_grid_not_connected"));
        }
        // else: we are already connecting to another network so don't touch
        // the status label
        
        // Update the list widget content
        updateListContent();
    }

    if (mConnectingIapId == iapId) {
        // Not connecting to this network anymore
        mConnectingIapId = WlanQtUtils::IapIdNone;
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_UPDATECONNECTIONCLOSED_EXIT);
}

/*!
    Context menu closing handler.
*/

void WlanSnifferListView::handleContextMenuClosed()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLECONTEXTMENUCLOSED_ENTRY);

    // Let list updating start again
    mIgnoreWlanScanResults = false;
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLECONTEXTMENUCLOSED_EXIT);
}

/*!
    List item activation handler. Connects the WLAN network, and if there
    is no IAP yet for it, starts WLAN Wizard.
    
    @param [in] item Selected WLAN network list item. 
*/

void WlanSnifferListView::handleListItemActivated(HbListWidgetItem *item)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLELISTITEMACTIVATED_ENTRY);
    
    handleConnect(item->data());

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLELISTITEMACTIVATED_EXIT);
}

/*!
    WLAN List item long press handler.
    The long press of a list item (i.e. WLAN IAP) opens a context menu that
    is populated depending on the state of the WLAN network in case.
    
    @param [in] item Selected list item.
    @param [in] coords Coordinates of the long press.
*/

void WlanSnifferListView::handleListItemLongPressed(
    HbListWidgetItem *item,
    const QPointF &coords)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLELISTITEMLONGPRESSED_ENTRY);

    mContextMenu = QSharedPointer<HbMenu>(new HbMenu());

    int activeIap = mEngine->wlanQtUtils()->activeIap();
    
    // Remember the item that was long pressed
    mContextMenuData = item->data();
    if (mContextMenuData.canConvert<int>()
        && mContextMenuData.toInt() == activeIap) {
        // Connected IAP, add "Disconnect" action
        mContextMenu->addAction(
            hbTrId("txt_common_menu_disconnect"),
            this,
            SLOT(handleListItemDisconnect()));
    } else {
        // Not connected IAP or AP, add "Connect" action
        mContextMenu->addAction(
            hbTrId("txt_common_menu_connect"),
            this,
            SLOT(handleListItemConnect()));
    }
    
    // Show the menu and connect closure signal (to re-enable list refreshing)
    bool connectStatus = connect(
        mContextMenu.data(),
        SIGNAL(aboutToClose()),
        this,
        SLOT(handleContextMenuClosed()));
    Q_ASSERT(connectStatus);
    mContextMenu->setTimeout(HbPopup::ContextMenuTimeout);
    mContextMenu->setPreferredPos(coords);
    mContextMenu->show();
    
    // Skip WLAN scan result updates during context menu handling
    mIgnoreWlanScanResults = true;
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLELISTITEMLONGPRESSED_EXIT);
}

/*!
    Handles the "Connect" action selected from the context menu for a list item.
*/

void WlanSnifferListView::handleListItemConnect()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLELISTITEMCONNECT_ENTRY);

    handleConnect(mContextMenuData);
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLELISTITEMCONNECT_EXIT);
}

/*!
    Handles the "Disconnect" action selected from the context menu for a list item.
*/

void WlanSnifferListView::handleListItemDisconnect()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLELISTITEMDISCONNECT_ENTRY);

    // "Disconnect" was only added, if the item was an IAP and data was
    // the IAP ID.
    Q_ASSERT(mContextMenuData.canConvert<int>());
    mEngine->wlanQtUtils()->disconnectIap(mContextMenuData.toInt());
    
    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLELISTITEMDISCONNECT_EXIT);
}

/*!
    Function for handling WLAN ON/OFF switch initiation.
*/

void WlanSnifferListView::handleWlanToggled()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLEWLANTOGGLED_ENTRY);
    
    // Toggle the new WLAN ON/OFF value
    bool wlanOn = mEngine->masterWlan(); 
    OstTraceExt1(
        TRACE_NORMAL,
        WLANSNIFFERLISTVIEW_HANDLEWLANTOGGLED,
        "WlanSnifferListView::handleWlanToggled;wlan=%hhu",
        wlanOn);
    
    // We have to check whether the offline mode is ON. If it is, then the
    // user needs to be queried whether WLAN will be used in offline mode.
    if (!wlanOn && mEngine->offlineMode()) {
        // Show the dialog and wait for user input.
        mWlanEnableDialog = QSharedPointer<HbMessageBox>(
            new HbMessageBox(HbMessageBox::MessageTypeQuestion)); 
        mWlanEnableDialog->setText(hbTrId("txt_occ_info_activate_wlan_in_airplane_mode"));
        mWlanEnableDialog->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
        // Open the dialog and connect the result to the handleWlanEnableDialogClosed slot
        mWlanEnableDialog->open(this, SLOT(handleWlanEnableDialogClosed(int)));
    } else {
        // Stop WLAN scanning immediately when switching WLAN OFF.
        // WLAN might have sent scan results just before disabling WLAN.
        if (wlanOn) {
            mEngine->stopWlanScanning();
        }
        mEngine->setMasterWlan(!wlanOn);
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLEWLANTOGGLED_EXIT);
}

/*!
    Function for handling WLAN Wizard starting when adding WLAN manually.
*/

void WlanSnifferListView::startWlanWizard()
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_STARTWLANWIZARD_ENTRY);

    emit wizardTriggered(NULL);

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_STARTWLANWIZARD_EXIT);
}

/*!
    Function to handle the input received when the wlan enabling 
    query is closed.
    
    @param [in] action The user action received from the dialog.
*/

void WlanSnifferListView::handleWlanEnableDialogClosed(int action)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTVIEW_HANDLEWLANENABLEDIALOGCLOSED_ENTRY);

    // The dialog must exist
    Q_ASSERT(mWlanEnableDialog);

    // If the user selected yes, then the wlan value is toggled,
    // otherwise nothing needs to be done.
    if (action == HbMessageBox::Yes) {
        mEngine->setMasterWlan(true); 
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTVIEW_HANDLEWLANENABLEDIALOGCLOSED_EXIT);
}
