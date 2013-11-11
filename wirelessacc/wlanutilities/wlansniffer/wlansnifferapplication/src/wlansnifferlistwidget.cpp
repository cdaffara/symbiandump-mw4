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
* WLAN Sniffer list widget class.
*/

// System includes

#include <QSharedPointer>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbAbstractViewItem>
#include <HbStringUtil>
#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsap.h"

#include "wlansniffer.h"
#include "wlansnifferlistitem.h"
#include "wlansnifferlistwidget.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansnifferlistwidgetTraces.h"
#endif

/*!
    \class WlanSnifferListWidget
    \brief WLAN Sniffer Custom List Widget implementation. 
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

/*!
    String comparator for list view. Used for locale-aware SSID comparison
    in list view.
    
    @param [in] string1 String #1 to compare.
    @param [in] string2 String #2 to compare.

    @return Zero (0), if strings are considered to be same,
            Positive (>0) if string1 is considered to be "greater than" string2. 
            Negative (<0) if string1 is considered to be "less than" string2. 
*/

static int WlanSsidStringComparator(
    const QString &string1,
    const QString &string2)
{
    return HbStringUtil::compareC(string1, string2);    
}

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] listWidget List Widget created to customize.
*/

WlanSnifferListWidget::WlanSnifferListWidget(HbListWidget *listWidget) :
    mListWidget(listWidget)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_WLANSNIFFERLISTWIDGET_ENTRY);
    
    // Set custom WLAN list item layout
    mListWidget->setLayoutName("wlanlistitem");

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_WLANSNIFFERLISTWIDGET_EXIT);
}

/*!
    Destructor.
*/

WlanSnifferListWidget::~WlanSnifferListWidget()
{
    OstTraceFunctionEntry0(DUP1_WLANSNIFFERLISTWIDGET_WLANSNIFFERLISTWIDGET_ENTRY);
    OstTraceFunctionExit0(DUP1_WLANSNIFFERLISTWIDGET_WLANSNIFFERLISTWIDGET_EXIT);
}

/*!
    Updates the list widget content with the given IAPs and APs.
    
    @param [in] iaps New WLAN IAP list.
    @param [in] aps New WLAN AP list.
    @param [in] connectedIapId IAP ID of the connected WLAN or
                IapIdNone if not connected.
 */

void WlanSnifferListWidget::updateContent(
    QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
    QList< QSharedPointer<WlanQtUtilsAp> > &aps,
    int connectedIapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_UPDATESCANSTATUS_ENTRY);
    
    traceListContent();
    
    // Sort the scan result lists
    sortIaps(iaps, connectedIapId);
    sortAps(aps);
    
    // Remove items, which are not present in new IAP or AP lists
    removeLostItems(iaps, aps);

    // Add items, which are only found in the new IAP and AP lists.
    // Also update the icons of existing items (connection mode and
    // signal strength may have changed)
    addDiscoveredItemsAndUpdateRest(iaps, aps, connectedIapId);

    traceListContent();
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_UPDATESCANSTATUS_EXIT);
}

/*!
    Scroll the list widget to given row.
    
    @param [in] row Row to scroll to. Indexing starts from zero. 
 */

void WlanSnifferListWidget::scrollTo(int row)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_SCROLLTO_ENTRY);

    OstTrace1(
        TRACE_NORMAL,
        WLANSNIFFERLISTWIDGET_SCROLLTO,
        "WlanSnifferListWidget::scrollTo;row=%d",
        row);

    // Check that the row exists
    if (row < mListWidget->count()) {
        // Do the scrolling
        QModelIndex index = mListWidget->model()->index(row, 0);
        mListWidget->scrollTo(index);
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_SCROLLTO_EXIT);
}

/*!
    IAP list sorting. Sorts IAPs into alphabetical order taking localization
    into account.
    
    @param [in,out] iaps WLAN IAP list.
    @param [in] connectedIapId IAP ID of the connected WLAN or
                IapIdNone if not connected.
*/

void WlanSnifferListWidget::sortIaps(
    QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
    int connectedIapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_SORTIAPS_ENTRY);
    
    // Order the list by IAP name
    qSort(
        iaps.begin(),
        iaps.end(),
        WlanSnifferListWidget::iapLessThan);

    // Find the possible connected IAP
    int connectedIndex = 0;
    for (int i = 0; i < iaps.size(); i++) {
        if (iaps[i]->value(WlanQtUtilsAp::ConfIdIapId).toInt() == connectedIapId) {
            connectedIndex = i;
            break;
        }
    }

    // Move the possible connected IAP to the beginning
    if (connectedIndex) {
        OstTraceExt2(
            TRACE_NORMAL,
            WLANSNIFFERLISTWIDGET_SORTIAPS_MOVE_CONNECTED,
            "WlanSnifferListWidget::sortIaps Moving connected IAP to top;connectedIapId=%d;connectedIndex=%d",
            connectedIapId,
            connectedIndex);
        
        iaps.move(connectedIndex, 0);
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_SORTIAPS_EXIT);
}

/*!
    AP list sorting. Sorts APs into alphabetical order taking localization
    into account.
    
    @param [in,out] aps WLAN AP list.
*/

void WlanSnifferListWidget::sortAps(
    QList< QSharedPointer<WlanQtUtilsAp> > &aps)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_SORTAPS_ENTRY);
    
    // Order the list by AP SSID
    qSort(
        aps.begin(),
        aps.end(),
        WlanSnifferListWidget::apLessThan);

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_SORTAPS_EXIT);
}

/*!
    Compares two WLAN IAPs. Comparison is done with the IAP name.
    
    @param[in] iap1 IAP to compare with.
    @param[in] iap2 IAP to compare.
    
    @return Returns true if the first iap is "less than" the second one.
 */

bool WlanSnifferListWidget::iapLessThan(
    const QSharedPointer<WlanQtUtilsAp> iap1,
    const QSharedPointer<WlanQtUtilsAp> iap2)
{
    // Primary comparison is based on the name
    int result = HbStringUtil::compareC(
        iap1->value(WlanQtUtilsAp::ConfIdName).toString(),
        iap2->value(WlanQtUtilsAp::ConfIdName).toString());

    return (result < 0) ? true : false;
}

/*!
    Compares two WLAN APs. Comparison is done with the AP SSID.
    
    @param[in] ap1 AP to compare with.
    @param[in] ap2 AP to compare.
    
    @return Returns true if the first ap is "less than" the second one.
 */

bool WlanSnifferListWidget::apLessThan(
    const QSharedPointer<WlanQtUtilsAp> ap1,
    const QSharedPointer<WlanQtUtilsAp> ap2)
{
    // Use AP comparison function with localized SSID comparison
    int result = WlanQtUtilsAp::compare(
        ap1.data(),
        ap2.data(),
        WlanSsidStringComparator);
        
    return (result < 0) ? true : false;
}

/*!
    Function for finding the given IAP from the old Wlan list.
    
    @param [in] iap IAP to find.
    
    @return Found WLAN List Widget item, 0 if not found.
*/

WlanSnifferListItem *WlanSnifferListWidget::findFromOldIapList(
    QSharedPointer<WlanQtUtilsAp> iap)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_FINDFROMOLDIAPLIST_IAP_ENTRY);

    WlanSnifferListItem *result = NULL;
    QVariant iapData = iap->value(WlanQtUtilsAp::ConfIdIapId);
    for (int row = 0; row < mListWidget->count(); row++) {
        HbListWidgetItem *item = mListWidget->item(row);
        if (item->data() == iapData) {
            result = static_cast<WlanSnifferListItem *>(item);
            break;
        }
    }

    OstTraceExt2(
        TRACE_DUMP,
        WLANSNIFFERLISTWIDGET_FINDFROMOLDIAPLIST_IAP,
        "WlanSnifferListWidget::findFromOldIapList;iapId=%d;found=%hhu",
        iapData.toInt(),
        (result != NULL) ? true : false);
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_FINDFROMOLDIAPLIST_IAP_EXIT);
    return result;
}

/*!
    Function for finding the given AP from the old Wlan list.
    
    @param [in] ap AP to find. 
    
    @return Found WLAN List Widget item, 0 if not found.
*/

WlanSnifferListItem *WlanSnifferListWidget::findFromOldApList(
    QSharedPointer<WlanQtUtilsAp> ap)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_FINDFROMOLDAPLIST_ENTRY);
    
    WlanSnifferListItem *result = NULL;
    for (int row = 0; row < mListWidget->count(); row++) {
        HbListWidgetItem *item = mListWidget->item(row);
        if (item->data().canConvert<WlanQtUtilsAp>()) {
            WlanQtUtilsAp oldAp = item->data().value<WlanQtUtilsAp>();
            if (WlanQtUtilsAp::compare(ap.data(), &oldAp) == 0) {
                result = static_cast<WlanSnifferListItem *>(item);
                break;
            }
        }
    }
    
#ifdef OST_TRACE_COMPILER_IN_USE
    QString tmp(ap.data()->value(WlanQtUtilsAp::ConfIdSsid).toString());
    TPtrC16 ssid(tmp.utf16(), tmp.length());
    OstTraceExt2(
        TRACE_NORMAL,
        WLANSNIFFERLISTWIDGET_FINDFROMOLDAPLIST,
        "WlanSnifferListWidget::findFromOldApList;found=%hhu;ssid=%S",
        (result != NULL) ? true : false,
        ssid);
#endif
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_FINDFROMOLDAPLIST_EXIT);
    return result;
}

/*!
    Function for creating a new list item from the given IAP.
    
    @param [in] iap WLAN IAP to create.
    @param [in] connectedIapId IAP ID of the connected WLAN or
                IapIdNone if not connected.
    
    @return New WLAN List Widget item.
*/

HbListWidgetItem *WlanSnifferListWidget::listItemCreate(
    QSharedPointer<WlanQtUtilsAp> iap,
    int connectedIapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_LISTITEMCREATE_IAP_ENTRY);
    
    WlanSnifferListItem *item = new WlanSnifferListItem();
    item->setNetworkName(iap->value(WlanQtUtilsAp::ConfIdName).toString());
    // Note: WPS icon is needed only in IAP setup, no longer here
    if (iap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt() !=
        CMManagerShim::WlanSecModeOpen) {
        item->setSecureIcon("qtg_small_lock");
    }

    item->setLeftIcon(
        leftIconChoose(
            iap->value(WlanQtUtilsAp::ConfIdIapId).toInt(),
            connectedIapId));
    item->setSignalIcon(
        signalStrengthIconChoose(
            iap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt()));
    
    item->setData(iap->value(WlanQtUtilsAp::ConfIdIapId));
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_LISTITEMCREATE_IAP_EXIT);
    return item;
}

/*!
    Function for creating a new list item from the given AP.
    
    @param [in] ap WLAN AP to create.
    
    @return New WLAN List Widget item.
*/

HbListWidgetItem *WlanSnifferListWidget::listItemCreate(
    QSharedPointer<WlanQtUtilsAp> ap)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_LISTITEMCREATE_AP_ENTRY);
    
    WlanSnifferListItem *item = new WlanSnifferListItem();
    item->setNetworkName(ap->value(WlanQtUtilsAp::ConfIdName).toString());
    if (ap->value(WlanQtUtilsAp::ConfIdWpsSupported).toBool()) {
        // WPS is supported, use a dedicated security icon
        item->setSecureIcon("qtg_small_wifi");
    } else if (ap->value(WlanQtUtilsAp::ConfIdSecurityMode).toInt() 
        != CMManagerShim::WlanSecModeOpen) {
        // Other secure networks
        item->setSecureIcon("qtg_small_lock");
    }
    item->setSignalIcon(
        signalStrengthIconChoose(
            ap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt()));

    QVariant data;
    data.setValue(*ap);
    item->setData(data);

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_LISTITEMCREATE_AP_EXIT);
    return item;
}

/*!
    Function for selecting the correct signal strength icon.
    
    @param [in] signalStrength Signal strength.
    
    @return Signal strength icon logical name.
*/

QString WlanSnifferListWidget::signalStrengthIconChoose(int signalStrength)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_SIGNALSTRENGTHICONCHOOSE_ENTRY);
    
    QString iconName;
    if (signalStrength <= SignalStrengthHigh) {
        iconName = "qtg_small_signal_good";
    } else if (signalStrength <= SignalStrengthMedium) {
        iconName = "qtg_small_signal_medium";
    } else {
        iconName = "qtg_small_signal_low";
    }
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_SIGNALSTRENGTHICONCHOOSE_EXIT);
    return iconName;
}

/*!
    Function for selecting the correct "left" icon, which is different for
    connected and not connected IAPs.
    
    @param [in] iapId IAP ID of the WLAN IAP.
    @param [in] connectedIapId IAP ID of the connected WLAN or
                IapIdNone if not connected.
                
    @return Icon logical name of the "left" icon.
*/

QString WlanSnifferListWidget::leftIconChoose(
    int iapId,
    int connectedIapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_LEFTICONCHOOSE_ENTRY);
    
    QString iconName;
    if (iapId == connectedIapId) {
        iconName = "qtg_small_connection";
    } else {
        iconName = "qtg_small_wlan";
    }
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_LEFTICONCHOOSE_EXIT);
    return iconName;
}

/*!
    Function for removing from the old Wlan list those items, which are
    not present in the given new lists.
    
    @param [in] iaps New WLAN IAP list.
    @param [in] aps New WLAN AP list.
*/

void WlanSnifferListWidget::removeLostItems(
    const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
    const QList< QSharedPointer<WlanQtUtilsAp> > &aps)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_REMOVELOSTITEMS_ENTRY);
    
    int row = 0;
    // Loop through all rows in the current list and try to find the items from
    // the new lists
    while (row < mListWidget->count()) {
        bool found = false;
        const HbListWidgetItem *item = mListWidget->item(row);
        if (item->data().canConvert<int>()) {
            // Item was an IAP, try to find it from the new IAP list
            foreach (QSharedPointer<WlanQtUtilsAp> iap, iaps) {
                if (iap->value(WlanQtUtilsAp::ConfIdIapId) == 
                    item->data()) {
                    found = true;
                    break;
                }
            }
        } else {
            // Item has to be an AP, try to find it from the new AP list
            Q_ASSERT(item->data().canConvert<WlanQtUtilsAp>());
            WlanQtUtilsAp oldAp = item->data().value<WlanQtUtilsAp>();
            foreach (QSharedPointer<WlanQtUtilsAp> newAp, aps) {
                if (WlanQtUtilsAp::compare(newAp.data(), &oldAp) == 0) {
                    found = true;
                    break;
                }
            }
        }
        if (found == false) {
            // Item was lost. Remove this row and delete it. Don't increment
            // row index, because the next item now has the same row index
            // as this one had
            delete mListWidget->takeItem(row);

            OstTrace1(
                TRACE_NORMAL,
                WLANSNIFFERLISTWIDGET_REMOVELOSTITEMS,
                "WlanSnifferListWidget::removeLostItems Removed;row=%d",
                row);
        } else {
            // Item still exists, move on to next row
            row++;
        }
    }

    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_REMOVELOSTITEMS_EXIT);
}

/*!
    Function for adding to the Wlan list those items, which are new
    in the given new lists. Also, icons and order of the existing list
    items are updated.
    
    @param [in] iaps New WLAN IAP list.
    @param [in] aps New WLAN AP list.
    @param [in] connectedIapId IAP ID of the connected WLAN or
                IapIdNone if not connected.
*/

void WlanSnifferListWidget::addDiscoveredItemsAndUpdateRest(
    const QList< QSharedPointer<WlanQtUtilsAp> > &iaps,
    const QList< QSharedPointer<WlanQtUtilsAp> > &aps,
    int connectedIapId)
{
    OstTraceFunctionEntry0(WLANSNIFFERLISTWIDGET_ADDDISCOVEREDITEMSANDUPDATEREST_ENTRY);
    
    int row = 0;
    // Loop through new IAPs and try to find them from the old list
    foreach (QSharedPointer<WlanQtUtilsAp> newIap, iaps) {
        WlanSnifferListItem *oldItem = findFromOldIapList(newIap);
        if (oldItem != NULL) {
            // Match found, update icons
            oldItem->setLeftIcon(
                leftIconChoose(
                    newIap->value(WlanQtUtilsAp::ConfIdIapId).toInt(),
                    connectedIapId));
            oldItem->setSignalIcon(
                signalStrengthIconChoose(
                    newIap->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt()));
            // Also move the IAP to new row, if necessary (IAP order changes
            // when IAPs are connected/disconnected)
            int oldRow = mListWidget->row(oldItem);
            if (row != oldRow) {
                OstTraceExt2(
                    TRACE_NORMAL,
                    WLANSNIFFERLISTWIDGET_ADDDISCOVEREDITEMSANDUPDATEREST_MOVE,
                    "WlanSnifferListWidget::addDiscoveredItemsAndUpdateRest Moved;oldRow=%d;row=%d",
                    oldRow,
                    row);
                mListWidget->takeItem(oldRow);
                mListWidget->insertItem(row, oldItem);
            }
        } else {
            // New item discovered, add it to the list. Row is the same as the
            // index of the item in the new scan list
            OstTrace1(
                TRACE_NORMAL,
                WLANSNIFFERLISTWIDGET_ADDDISCOVEREDITEMSANDUPDATEREST_ADD_IAP,
                "WlanSnifferListWidget::addDiscoveredItemsAndUpdateRest Added IAP;row=%d",
                row);
            mListWidget->insertItem(row, listItemCreate(newIap, connectedIapId));
        }
        // Move to next row
        row++;
    }
    // Repeat the same for new APs. Notice that row index continues from
    // previous loop
    foreach (QSharedPointer<WlanQtUtilsAp> newAp, aps) {
        WlanSnifferListItem *oldItem = findFromOldApList(newAp);
        if (oldItem != NULL) {
            oldItem->setSignalIcon(
                signalStrengthIconChoose(
                    newAp->value(WlanQtUtilsAp::ConfIdSignalStrength).toInt()));
            // Notice that AP order may not change, they are always in
            // alphabetical order
        } else {
            OstTrace1(
                TRACE_NORMAL,
                WLANSNIFFERLISTWIDGET_ADDDISCOVEREDITEMSANDUPDATEREST_ADD_AP,
                "WlanSnifferListWidget::addDiscoveredItemsAndUpdateRest Added AP;row=%d",
                row);
            mListWidget->insertItem(row, listItemCreate(newAp));
        }
        row++;
    }
    
    OstTraceFunctionExit0(WLANSNIFFERLISTWIDGET_ADDDISCOVEREDITEMSANDUPDATEREST_EXIT);
}

/*!
    This function traces the contents of the list.
*/

void WlanSnifferListWidget::traceListContent() const
{
#ifdef OST_TRACE_COMPILER_IN_USE
    for (int row = 0; row < mListWidget->count(); row++) {
        HbListWidgetItem *item = mListWidget->item(row);
        if (item->data().canConvert<int>()) {
            OstTraceExt2(
                TRACE_DUMP,
                WLANSNIFFERLISTWIDGET_TRACELISTCONTENT_IAP,
                "WlanSnifferListWidget::traceListContent IAP;row=%d;id=%d",
                row,
                item->data().toInt());
        } else {
            Q_ASSERT(item->data().canConvert<WlanQtUtilsAp>());
            WlanQtUtilsAp ap = item->data().value<WlanQtUtilsAp>();
            QString tmp(ap.value(WlanQtUtilsAp::ConfIdSsid).toString());
            TPtrC16 ssid(tmp.utf16(), tmp.length());
            OstTraceExt2(
                TRACE_DUMP,
                WLANSNIFFERLISTWIDGET_TRACELISTCONTENT_AP,
                "WlanSnifferListWidget::traceListContent AP;row=%d;ssid=%S",
                row,
                ssid);
        }
    }
#endif
}
