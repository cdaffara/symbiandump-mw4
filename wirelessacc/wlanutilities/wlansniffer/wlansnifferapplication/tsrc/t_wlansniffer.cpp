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
* This is the source file for testing Wlan Sniffer application.
*/

// System includes

#include <QTest>
#include <QSharedPointer>
#include <HbApplication>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanqtutilsap.h"

#include "wlansnifferlistwidget.h"
#include "t_wlansniffer.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

/*!
    Test main function. Runs all test cases.
 */

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestWlanSniffer.txt";
 
    TestWlanSniffer tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}

// ======== MEMBER FUNCTIONS ========

/*!
    This function will be called before the first test function is executed.
*/

void TestWlanSniffer::initTestCase()
{
    mWidget = NULL;
}

/*!
    This function will be called after the last test function was executed.
 */

void TestWlanSniffer::cleanupTestCase()
{
}

/*!
    This function will be called before each test function is executed.
 */

void TestWlanSniffer::init()
{
    mListWidget = new HbListWidget();
    mWidget = new WlanSnifferListWidget(mListWidget);
    
    mIaps.clear();
    mAps.clear();
}

/*!
    This function will be called after each test function is executed.
 */

void TestWlanSniffer::cleanup()
{
    delete mWidget;
    mWidget = NULL;

    mListWidget->clear();
    delete mListWidget;
    mListWidget = NULL;
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentIapAdded()
{
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 2", 90, 0, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 2);
    verifyAp(0, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);
    verifyAp(1, QString("SSID 2"), CMManagerShim::WlanSecModeWpa);

    appendWlanIap(1, 0, "SSID 3", "SSID 3", 20, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 1);
    verifyAp(1, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);
    verifyAp(2, QString("SSID 2"), CMManagerShim::WlanSecModeWpa);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentApAdded()
{
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 1);
    verifyAp(0, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);

    appendWlanAp("SSID 2", 90, 0, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 2);
    verifyAp(0, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);
    verifyAp(1, QString("SSID 2"), CMManagerShim::WlanSecModeWpa);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentIapRemoved()
{
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 90, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 3", 90, 0, CMManagerShim::WlanSecModeWep);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 1);
    verifyIap(1, 2);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeWep);

    mIaps.takeAt(0);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 2);
    verifyIap(0, 2);
    verifyAp(1, QString("SSID 3"), CMManagerShim::WlanSecModeWep);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentApRemoved()
{
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 1);
    verifyAp(0, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);

    mAps.takeAt(0);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 0);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentIapConnected()
{
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 95, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 15, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 1);
    verifyIap(1, 2);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeOpen);

    // IAP order changed because one is connected
    mIaps.clear();
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 20, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 2);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 2);
    verifyIap(1, 1);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeOpen);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentIapDisconnected()
{
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 2);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 2);
    verifyIap(1, 1);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeOpen);

    // IAP order changed because not connected anymore
    mIaps.clear();
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 60, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 3);
    verifyIap(0, 1);
    verifyIap(1, 2);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeOpen);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentRobustness1()
{
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 15, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 95, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 4", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 5", 40, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 5);
    verifyIap(0, 1);
    verifyIap(1, 2);
    verifyAp(2, QString("SSID 3"), CMManagerShim::WlanSecModeOpen);
    verifyAp(3, QString("SSID 4"), CMManagerShim::WlanSecModeWep);
    verifyAp(4, QString("SSID 5"), CMManagerShim::WlanSecModeOpen);

    mIaps.clear();
    appendWlanIap(3, 0, "SSID 3", "SSID 3", 10, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 30, CMManagerShim::WlanSecModeWpa);
    mAps.clear();
    appendWlanAp("SSID 4", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 5", 40, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 6", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 7", 40, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 3);
    
    QCOMPARE(mListWidget->count(), 7);
    verifyIap(0, 3);
    verifyIap(1, 1);
    verifyIap(2, 2);
    verifyAp(3, QString("SSID 4"), CMManagerShim::WlanSecModeWep);
    verifyAp(4, QString("SSID 5"), CMManagerShim::WlanSecModeOpen);
    verifyAp(5, QString("SSID 6"), CMManagerShim::WlanSecModeWep);
    verifyAp(6, QString("SSID 7"), CMManagerShim::WlanSecModeOpen);

    mIaps.clear();
    appendWlanIap(4, 0, "SSID X", "SSID X", 10, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 20, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(3, 0, "SSID 3", "SSID 3", 10, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(5, 0, "SSID 3", "SSID 3", 50, CMManagerShim::WlanSecModeWapi);
    mAps.clear();
    appendWlanAp("SSID 4", 30, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 4a", 40, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 7", 76, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 7", 90, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 8", 30, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 9", 30, 0, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 4);

    QCOMPARE(mListWidget->count(), 10);
    verifyIap(0, 4);
    verifyIap(1, 2);
    verifyIap(2, 3);
    verifyIap(3, 5);
    verifyAp(4, QString("SSID 4"), CMManagerShim::WlanSecModeOpen);
    verifyAp(5, QString("SSID 4a"), CMManagerShim::WlanSecModeOpen);
    verifyAp(6, QString("SSID 7"), CMManagerShim::WlanSecModeOpen);
    verifyAp(7, QString("SSID 7"), CMManagerShim::WlanSecModeWep);
    verifyAp(8, QString("SSID 8"), CMManagerShim::WlanSecModeWpa);
    verifyAp(9, QString("SSID 9"), CMManagerShim::WlanSecModeWpa);

    mIaps.clear();
    appendWlanIap(3, 0, "SSID 3", "SSID 3", 50, CMManagerShim::WlanSecModeOpen);
    mAps.clear();

    mWidget->updateContent(mIaps, mAps, 0);

    QCOMPARE(mListWidget->count(), 1);
    verifyIap(0, 3);

    mIaps.clear();
    mAps.clear();

    mWidget->updateContent(mIaps, mAps, 0);

    QCOMPARE(mListWidget->count(), 0);
}

/*!
    This function tests Wlan list widget content updating.
 */

void TestWlanSniffer::testUpdateContentRobustness2()
{
    QSharedPointer<WlanQtUtilsAp> iap2(new WlanQtUtilsAp());
    iap2->setValue(WlanQtUtilsAp::ConfIdIapId, 2);
    iap2->setValue(WlanQtUtilsAp::ConfIdNetworkId, 0);
    iap2->setValue(WlanQtUtilsAp::ConfIdName, "SSID 2");
    iap2->setValue(WlanQtUtilsAp::ConfIdSsid, "SSID 2");
    iap2->setValue(WlanQtUtilsAp::ConfIdSignalStrength, 10);
    iap2->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    iap2->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    QSharedPointer<WlanQtUtilsAp> iap4(new WlanQtUtilsAp());
    iap4->setValue(WlanQtUtilsAp::ConfIdIapId, 4);
    iap4->setValue(WlanQtUtilsAp::ConfIdNetworkId, 0);
    iap4->setValue(WlanQtUtilsAp::ConfIdName, "SSID 4");
    iap4->setValue(WlanQtUtilsAp::ConfIdSsid, "SSID 4");
    iap4->setValue(WlanQtUtilsAp::ConfIdSignalStrength, 20);
    iap4->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    iap4->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 5", 20, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 6", 20, 0, CMManagerShim::WlanSecModeWpa);

    mIaps.append(iap2);
    mIaps.append(iap4);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 6);
    verifyIap(0, 2);
    verifyIap(1, 4);
    verifyAp(2, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);
    verifyAp(3, QString("SSID 3"), CMManagerShim::WlanSecModeWpa);
    verifyAp(4, QString("SSID 5"), CMManagerShim::WlanSecModeWpa);
    verifyAp(5, QString("SSID 6"), CMManagerShim::WlanSecModeWpa);

    mIaps.clear();
    mAps.clear();
    mIaps.append(iap2);
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 2);
    verifyIap(0, 2);
    verifyAp(1, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);

    mIaps.clear();
    mAps.clear();
    mIaps.append(iap2);
    mIaps.append(iap4);
    appendWlanAp("SSID 1", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 5", 20, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 6", 20, 0, CMManagerShim::WlanSecModeWpa);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 6);
    verifyIap(0, 2);
    verifyIap(1, 4);
    verifyAp(2, QString("SSID 1"), CMManagerShim::WlanSecModeOpen);
    verifyAp(3, QString("SSID 3"), CMManagerShim::WlanSecModeWpa);
    verifyAp(4, QString("SSID 5"), CMManagerShim::WlanSecModeWpa);
    verifyAp(5, QString("SSID 6"), CMManagerShim::WlanSecModeWpa);
}

/*!
    This function tests list sorting. The list content comes in "random" order,
    and the list widget should sort the list alphabetically. 
 */

void TestWlanSniffer::testUpdateContentSort()
{
    appendWlanIap(1, 0, "Dada", "dada", 20, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(2, 0, "salakka", "salakka", 30, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(3, 0, "lahna", "lahna", 40, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(4, 0, "Labiili", "labiili", 50, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(5, 0, "ABCD", "ABCD", 60, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(6, 0, "connected", "connected", 60, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(7, 0, "IAP 1", "IAP 1", 10, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(8, 0, "IAP 10", "IAP 10", 10, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(9, 0, "IAP 2", "IAP 2", 10, CMManagerShim::WlanSecModeOpen);
    appendWlanIap(10, 0, "IAP 11", "IAP 11", 10, CMManagerShim::WlanSecModeOpen);

    appendWlanAp("jokuap", 20, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("Jippii", 30, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("jalava", 40, 0, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("abba", 50, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("hippokampus", 60, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 6);
    
    QCOMPARE(mListWidget->count(), 15);
    verifyIap(0, 6);
    verifyIap(1, 5);
    verifyIap(2, 1);
    verifyIap(3, 7);
    verifyIap(4, 8);
    verifyIap(5, 10);
    verifyIap(6, 9);
    verifyIap(7, 4);
    verifyIap(8, 3);
    verifyIap(9, 2);
    verifyAp(10, QString("abba"), CMManagerShim::WlanSecModeOpen);
    verifyAp(11, QString("hippokampus"), CMManagerShim::WlanSecModeOpen);
    verifyAp(12, QString("jalava"), CMManagerShim::WlanSecModeWpa);
    verifyAp(13, QString("Jippii"), CMManagerShim::WlanSecModeOpen);
    verifyAp(14, QString("jokuap"), CMManagerShim::WlanSecModeWpa);
}

/*!
    This function tests list scrolling. There's no way to verify in this test
    that the scrolling actually works, but let's execute the code at least. 
 */

void TestWlanSniffer::testScrollTo()
{
    appendWlanIap(1, 0, "SSID 1", "SSID 1", 15, CMManagerShim::WlanSecModeWpa);
    appendWlanIap(2, 0, "SSID 2", "SSID 2", 95, CMManagerShim::WlanSecModeWpa);
    appendWlanAp("SSID 3", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 4", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 5", 40, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 6", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 7", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 8", 40, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 9", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 10", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 11", 40, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 12", 20, 0, CMManagerShim::WlanSecModeOpen);
    appendWlanAp("SSID 13", 30, 0, CMManagerShim::WlanSecModeWep);
    appendWlanAp("SSID 14", 40, 0, CMManagerShim::WlanSecModeOpen);

    mWidget->updateContent(mIaps, mAps, 0);
    
    QCOMPARE(mListWidget->count(), 14);

    // Scroll to last row
    mWidget->scrollTo(13);
    // Scroll to non-existing row -> ignored
    mWidget->scrollTo(20);
    // Scroll to first row
    mWidget->scrollTo(0);

    QCOMPARE(mListWidget->count(), 14);
}

/*!
    This function verifies that the item on "row" is an IAP matching the given parameters.
 */

void TestWlanSniffer::verifyIap(int row, int iapId)
{
    QVERIFY(mListWidget->item(row)->data().canConvert<int>());
    int id = mListWidget->item(row)->data().value<int>();
    QCOMPARE(id, iapId);
}

/*!
    This function verifies that the item on "row" is an AP matching the given parameters.
 */

void TestWlanSniffer::verifyAp(int row, QString ssid, int securityMode)
{
    QVERIFY(mListWidget->item(row)->data().canConvert<WlanQtUtilsAp>());
    WlanQtUtilsAp ap = mListWidget->item(row)->data().value<WlanQtUtilsAp>();
    QCOMPARE(ap.value(WlanQtUtilsAp::ConfIdSsid).toString(), ssid);
    QCOMPARE(ap.value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(), securityMode);
}

/*!
    Sub test case for appending an AP to AP list.
*/

void TestWlanSniffer::appendWlanAp(
    QString ssId, 
    int signalStrength, 
    int networkMode, 
    int securityMode)
{
    QSharedPointer<WlanQtUtilsAp> ap(new WlanQtUtilsAp());
    ap->setValue(WlanQtUtilsAp::ConfIdName, ssId);
    ap->setValue(WlanQtUtilsAp::ConfIdSsid, ssId);
    ap->setValue(WlanQtUtilsAp::ConfIdSignalStrength, signalStrength);
    ap->setValue(WlanQtUtilsAp::ConfIdConnectionMode, networkMode);
    ap->setValue(WlanQtUtilsAp::ConfIdSecurityMode, securityMode);
    ap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, false);
    ap->setValue(WlanQtUtilsAp::ConfIdWpsSupported, false);
    mAps.append(ap);
}

/*!
    Sub test case for appending an IAP to IAP list.
*/

void TestWlanSniffer::appendWlanIap(
    int iapId,
    int netId,
    QString name,
    QString ssId, 
    int signalStrength,
    int securityMode)
{
    WlanQtUtilsAp *iap = new WlanQtUtilsAp();
    iap->setValue(WlanQtUtilsAp::ConfIdIapId, iapId);
    iap->setValue(WlanQtUtilsAp::ConfIdNetworkId, netId);
    iap->setValue(WlanQtUtilsAp::ConfIdName, name);
    iap->setValue(WlanQtUtilsAp::ConfIdSsid, ssId);
    iap->setValue(WlanQtUtilsAp::ConfIdSignalStrength, signalStrength);
    iap->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    iap->setValue(WlanQtUtilsAp::ConfIdSecurityMode, securityMode);
    iap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, false);
    iap->setValue(WlanQtUtilsAp::ConfIdWpsSupported, false);
    mIaps.append(QSharedPointer<WlanQtUtilsAp>(iap));
}
