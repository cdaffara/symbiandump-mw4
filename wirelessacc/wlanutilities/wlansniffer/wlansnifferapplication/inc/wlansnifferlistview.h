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

#ifndef WLANSNIFFERLISTVIEW_H
#define WLANSNIFFERLISTVIEW_H

// System includes

#include <QSharedPointer>
#include <QScopedPointer>

#include <HbView>
#include <HbMenu>
#include <HbMessageBox>

// User includes

// Forward declarations

class HbDocumentLoader;
class HbListWidget;
class HbListWidgetItem;
class HbLabel;
class HbPushButton;
class WlanSnifferMainWindow;
class WlanSnifferListWidget;
class WlanSnifferEngine;
class WlanQtUtilsAp;

// External data types

// Constants

// Class declaration

class WlanSnifferListView : public HbView
{
    Q_OBJECT

public:

    // Data types

    WlanSnifferListView(
        WlanSnifferEngine *engine,
        WlanSnifferMainWindow *mainWindow);
    
    ~WlanSnifferListView();

signals:

    /*!
        Signal for triggering WLAN Wizard start. 
        
        @param [in] ap WLAN access point.
     */

    void wizardTriggered(const WlanQtUtilsAp *ap);
    
public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferListView)

    void loadDocml(bool isEmbedded);
    
    void updateWlanStatusLabel(bool enabled);
    
    void handleConnect(QVariant data);

private slots:

    void updateListContent();
    
    void updateWlanEnabled();
    
    void updateConnectionOpened(int iapId);
    
    void updateConnectionClosed(int iapId);
    
    void handleContextMenuClosed();

    void handleListItemActivated(HbListWidgetItem *item);
    
    void handleListItemLongPressed(
        HbListWidgetItem *item,
        const QPointF &coords);
    
    void handleListItemConnect();
    
    void handleListItemDisconnect();
    
    void handleWlanToggled();
    
    void startWlanWizard();
    
    void handleWlanEnableDialogClosed(int action);

private: // data

    // Owned data
    
    //! Document loader for list view
    QScopedPointer<HbDocumentLoader> mDocLoader;
    //! WLAN Sniffer list view custom widget
    QSharedPointer<WlanSnifferListWidget> mWlanListWidget;
    //! Context menu reference, if one is open
    QSharedPointer<HbMenu> mContextMenu;
    /*!
       Data identifying the network for which the context menu has been
       opened: WlanQtUtilsAp class, or int IAP ID.
     */
    QVariant mContextMenuData;
    //! WLAN enabling in airplane mode dialog
    QSharedPointer<HbMessageBox> mWlanEnableDialog;
    //! Flag for handling WLAN scan result update ignoring
    bool mIgnoreWlanScanResults;
    //! ID of the IAP we are currently connecting
    int mConnectingIapId;

    // Not owned data
    
    //! WLAN Sniffer engine
    WlanSnifferEngine *mEngine;
    //! WLAN ON/OFF switch action
    HbAction *mSwitchWlanAction;
    //! Add WLAN manually action
    HbAction *mAddWlanAction;
    //! WLAN Sniffer list view widget
    HbListWidget *mWlanList;
    //! WLAN Status label
    HbLabel *mStatusLabel;
    //! WLAN ON/OFF button
    HbPushButton *mWlanButton;

    // Friend classes
};

#endif // WLANSNIFFERLISTVIEW_H
