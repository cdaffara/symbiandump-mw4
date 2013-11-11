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
* This is the source file for testing Wlan Qt Utilities library.
*/

#include <QApplication>
#include <QtCore>
#include <QTest>
#include <QSignalSpy>
#include <QSharedPointer>
#include <rconnmon.h>
#include <nifvar.h>
#include <cmmanagerdefines_shim.h>

#include "wlanqtutilsap.h"
#include "wlanqtutils.h"
#include "wlanqtutils_p.h"
#include "wlanqtutilsconmonwrapper.h"
#include "wlanqtutilsconmonwrapperinfo_symbian.h"
#include "testwlanqtutils.h"
#include "wlanqtutilstestcontext.h"

WlanQtUtilsTestContext testContext;

const QString TestWlanQtUtils::commsDatDefault_ = "default.cre";

// ---------------------------------------------------------
// FRAMEWORK FUNCTIONS
// ---------------------------------------------------------

/**
 * Test main function. Runs all test cases.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\wlanqtutils_qtest_log.txt";
 
    TestWlanQtUtils tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}

/**
 * This function will be called before the first test function is executed.
 */
void TestWlanQtUtils::initTestCase()
{
    wlanQtUtils_ = NULL;
    mSignalScanReady = NULL;
    mSignalScanApReady = NULL;
    mSignalScanDirectReady = NULL;
    signalWlanNetworkOpened_ = NULL;
    signalWlanNetworkClosed_ = NULL;
    signalIctResult_ = NULL;

    //If Active scheduler exists then don't install a new one as it will cause panic
    if (CActiveScheduler::Current() == NULL) {
        CActiveScheduler *scheduler = new CActiveScheduler();
        CActiveScheduler::Install(scheduler);
    }
}

/**
 * This function will be called after the last test function was executed.
 */
void TestWlanQtUtils::cleanupTestCase()
{
    // CommsDat file is initialized.
    subTestLoadCommsDatFile(commsDatDefault_);
}

/**
 * This function will be called before each test function is executed.
 */
void TestWlanQtUtils::init()
{
    // CommsDat file is initialized.
    subTestLoadCommsDatFile(commsDatDefault_);

    testContext.initialize();

    QVERIFY(wlanQtUtils_ == NULL);
    QVERIFY(mSignalScanReady == NULL);
    QVERIFY(mSignalScanApReady == NULL);
    QVERIFY(mSignalScanDirectReady == NULL);
    QVERIFY(signalWlanNetworkOpened_ == NULL);
    QVERIFY(signalWlanNetworkClosed_ == NULL);
    QVERIFY(signalIctResult_ == NULL);
    subTestNewWlanQtUtils();
}

/**
 * This function will be called after each test function is executed.
 */
void TestWlanQtUtils::cleanup()
{
    delete wlanQtUtils_;
    wlanQtUtils_ = NULL;

    QCOMPARE(mSignalScanReady->count(), 0);
    delete mSignalScanReady;
    mSignalScanReady = NULL;

    QCOMPARE(mSignalScanApReady->count(), 0);
    delete mSignalScanApReady;
    mSignalScanApReady = NULL;

    QCOMPARE(mSignalScanDirectReady->count(), 0);
    delete mSignalScanDirectReady;
    mSignalScanDirectReady = NULL;

    QCOMPARE(signalWlanNetworkOpened_->count(), 0);
    delete signalWlanNetworkOpened_;
    signalWlanNetworkOpened_ = NULL;

    QCOMPARE(signalWlanNetworkClosed_->count(), 0);
    delete signalWlanNetworkClosed_;
    signalWlanNetworkClosed_ = NULL;

    QCOMPARE(signalIctResult_->count(), 0);
    delete signalIctResult_;
    signalIctResult_ = NULL;
}

// ---------------------------------------------------------
// TEST CASES
// ---------------------------------------------------------

/**
 * Test available WLAN APs when scan is triggered by client.
 */
void TestWlanQtUtils::testAvailableWlan()
{
    testContext.mScan.createDefaultWlanScanApResultList(4);
    
    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();
}

/**
 * Test available WLAN APs when scan is triggered by client.
 * Two scan results have the same SSID and the latter one of those will be removed.
 * Two scan results have the same SSID but different security mode
 * Two scan results have the same SSID and security mode but different WpaPsk value
 * and both are included in the results.
 * Two scan results have the same SSID but different connection mode
 */
void TestWlanQtUtils::testAvailableWlanDuplicates()
{
    testContext.mScan.createDefaultWlanScanApResultList(11);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID");
    testContext.mScan.mWlanScanApResultList[2]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID");
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID, different SecMode");
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa);
    testContext.mScan.mWlanScanApResultList[6]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID, different SecMode");
    testContext.mScan.mWlanScanApResultList[6]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeOpen);
    testContext.mScan.mWlanScanApResultList[7]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID & SecMode diff Psk");
    testContext.mScan.mWlanScanApResultList[7]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa2);
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID & SecMode diff Psk");
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa2);
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse,true);
    testContext.mScan.mWlanScanApResultList[10]->setValue(WlanQtUtilsAp::ConfIdSsid, "Same SSID");
    testContext.mScan.mWlanScanApResultList[10]->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Adhoc);

    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    // First remove AP that has duplicate SSID.
    testContext.mScan.mWlanScanApResultList.removeAt(2);
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();
}

/**
 * Test available WLAN APs when scan is triggered by client.
 * Include all supported security modes.
 */
void TestWlanQtUtils::testAvailableWlanSecModes()
{
    testContext.mScan.createDefaultWlanScanApResultList(11);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeOpen);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Adhoc);
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWep);
    testContext.mScan.mWlanScanApResultList[2]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWep);
    testContext.mScan.mWlanScanApResultList[3]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecMode802_1x);
    testContext.mScan.mWlanScanApResultList[4]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
    testContext.mScan.mWlanScanApResultList[6]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa2);
    testContext.mScan.mWlanScanApResultList[7]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa2);
    testContext.mScan.mWlanScanApResultList[7]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
    testContext.mScan.mWlanScanApResultList[8]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWapi);
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWapi);
    // Let's trick the stub to return WapiPsk by putting WpaPskUse on *
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
    testContext.mScan.mWlanScanApResultList[10]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa2);
    testContext.mScan.mWlanScanApResultList[10]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
    testContext.mScan.mWlanScanApResultList[10]->setValue(WlanQtUtilsAp::ConfIdWpsSupported, true);

    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    // Remove the trick *. WapiPsk and Wapi both map to just Wapi
    testContext.mScan.mWlanScanApResultList[9]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse,false);
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();
}

/**
 * Test available WLAN APs when scan is triggered by client.
 * Include some exotic ssids.
 */
void TestWlanQtUtils::testAvailableWlanSsids()
{
    testContext.mScan.createDefaultWlanScanApResultList(6);
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSsid, "           ");
    testContext.mScan.mWlanScanApResultList[2]->setValue(WlanQtUtilsAp::ConfIdSsid, "    whitespace around ssid  ");
    testContext.mScan.mWlanScanApResultList[3]->setValue(WlanQtUtilsAp::ConfIdSsid, QString(QChar()));
    testContext.mScan.mWlanScanApResultList[4]->setValue(WlanQtUtilsAp::ConfIdSsid, "maximum length ssid that is 32 c");
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdSsid, "A");

    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    // Empty ssids are removed from results
    testContext.mScan.mWlanScanApResultList.removeAt(3);
    testContext.mScan.mWlanScanApResultList.removeAt(1);
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
}

/**
 * Test available WLAN APs when there are also WLAN IAPs available.
 */
void TestWlanQtUtils::testAvailableWlanWithIaps()
{
    // Form WLAN IAP scan result list
    testContext.mScan.createDefaultWlanScanIapResultList(3);
    testContext.mScan.mWlanScanIapResultList.removeAt(1);
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdIapId, 5);
    testContext.mScan.mWlanScanIapResultList[1]->setValue(WlanQtUtilsAp::ConfIdIapId, 7);
    
    // Form WLAN AP scan result list
    testContext.mScan.createDefaultWlanScanApResultList(7);
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSsid, "WLAN IAP 3");
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa);    
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse,true);    
    testContext.mScan.mWlanScanApResultList[5]->setValue(WlanQtUtilsAp::ConfIdSsid, "WLAN IAP 1");
    // SSID is found in Internet Snap, but security mode does not match:
    testContext.mScan.mWlanScanApResultList[6]->setValue(WlanQtUtilsAp::ConfIdSsid, "WLAN IAP 2");
    testContext.mScan.mWlanScanApResultList[6]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa);

    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    // Verify WLAN IAP list
    testContext.mScan.verifyWlanScanIapResultList(iaps);

    // Verify WLAN AP list. First, remove scan results that will not appear
    // because they are WLAN IAPs.
    testContext.mScan.mWlanScanApResultList.removeAt(5);
    testContext.mScan.mWlanScanApResultList.removeAt(1);
    testContext.mScan.verifyWlanScanApResultList(aps);
    
    iaps.clear();
    aps.clear();
}

/**
 * Test refereshing of WLAN APs when client requests sequential scans.
 */
void TestWlanQtUtils::testAvailableWlanSequence()
{
    // **************** Before 1st scan ********************
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);
    // Verify we have no results
    QCOMPARE(aps.count(), 0);
    QCOMPARE(iaps.count(), 0);
    
    // **************** 1st scan ********************
    // 6 APs are required for this scan
    testContext.mScan.createDefaultWlanScanApResultList(6);

    wlanQtUtils_->scanWlans();
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get the results for the 1st scan
    wlanQtUtils_->availableWlans(iaps, aps);
    // Verify the results for the scan
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();

    // **************** 2nd scan ********************
    // 2 APs are required for this scan
    testContext.mScan.createDefaultWlanScanApResultList(2);

    wlanQtUtils_->scanWlans();
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get the results for the 2nd scan
    wlanQtUtils_->availableWlans(iaps, aps);
    // Verify the results for the scan
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();

    // **************** 3rd scan ********************
    // 4 APs are required for the 3rd scan
    testContext.mScan.createDefaultWlanScanApResultList(4);

    wlanQtUtils_->scanWlans();
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);
    
    // Get the results for the 3rd scan
    wlanQtUtils_->availableWlans(iaps, aps);
    // Verify the results for the scan
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
    aps.clear();
}

/**
 * Test availability scan that finds nothing.
 */
void TestWlanQtUtils::testAvailableWlanScanEmpty()
{
    // Form result list
    testContext.mScan.createDefaultWlanScanApResultList(0);

    // Request a direct scan to get result signal
    wlanQtUtils_->scanWlans();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);
    testContext.mScan.verifyWlanScanApResultList(aps);
    QCOMPARE(iaps.count(), 0);
}

/**
 * Test WLAN availability scan failure.
 */
void TestWlanQtUtils::testAvailableWlanScanError()
{
    // Set the scan to fail
    testContext.mScan.mApScanRetValue = KErrGeneral;
    
    // Start the scan
    wlanQtUtils_->scanWlans();

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusError);
}

/**
 * Test available WLAN APs when scan is triggered by client.
 */
void TestWlanQtUtils::testAvailableWlanAps()
{
    testContext.mScan.createDefaultWlanScanApResultList(4);
    
    // Request a scan to get result signal
    wlanQtUtils_->scanWlanAps();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanApReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlanAps(aps);

    testContext.mScan.verifyWlanScanApResultList(aps);
    aps.clear();
}

/**
 * Test a direct scan that finds nothing.
 */
void TestWlanQtUtils::testAvailableWlanApsScanEmpty()
{
    // Form result list
    testContext.mScan.createDefaultWlanScanApResultList(0);

    // Request a direct scan to get result signal
    wlanQtUtils_->scanWlanAps();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanApReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlanAps(aps);
    testContext.mScan.verifyWlanScanApResultList(aps);
}

/**
 * Test cancellation of WLAN AP scan.
 */
void TestWlanQtUtils::testStopWlanApScan()
{
    // Do not complete scan request immediately
    testContext.mScan.mCompleteWlanApScan = false;
    
    // Start the scan
    wlanQtUtils_->scanWlanAps();
    // Cancel the scan
    wlanQtUtils_->stopWlanScan();

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanApReady, WlanQtUtils::ScanStatusCancelled);
}

/**
 * Test WLAN AP scan failure.
 */
void TestWlanQtUtils::testWlanApScanError()
{
    // Set the scan to fail
    testContext.mScan.mApScanRetValue = KErrGeneral;
    
    // Start the scan
    wlanQtUtils_->scanWlanAps();

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanApReady, WlanQtUtils::ScanStatusError);
}

/**
 * Test WLAN AP scan with special characters.
 */
void TestWlanQtUtils::testAvailableWlanApsSpecialChars()
{
    testContext.mScan.createDefaultWlanScanApResultList(3);
    // Set SSID's to include special characters
    testContext.mScan.mWlanScanApResultList[0]->setValue(
        WlanQtUtilsAp::ConfIdSsid,
        "\x57\x41\x50\x49\xBD\xA7\xA4\xE4\xF6");
    testContext.mScan.mWlanScanApResultList[1]->setValue(
        WlanQtUtilsAp::ConfIdSsid,
        "\x01\x02\x03\xAB\x43\x52\x41\x50\xBB\xA7\xA4\xFE\xFF");
    testContext.mScan.mWlanScanApResultList[2]->setValue(
        WlanQtUtilsAp::ConfIdSsid,
        "12345678901234567890123456789012");  // Maximum SSID length of 32 bytes

    // Request a scan to get result signal
    wlanQtUtils_->scanWlanAps();
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanApReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlanAps(aps);

    testContext.mScan.verifyWlanScanApResultList(aps);
    aps.clear();
}

/**
 * Test a basic direct scan.
 */
void TestWlanQtUtils::testDirectScanBasic()
{
    // Form result list
    testContext.mScan.createDefaultWlanScanApResultList(2);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSsid, "testssid");
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeOpen);
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSsid, "testssid");
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdBssid, "654321");
    testContext.mScan.mWlanScanApResultList[1]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);

    // Request a direct scan to get result signal
    wlanQtUtils_->scanWlanDirect(QString("testssid"));
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanDirectReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlanAps(aps);

    testContext.mScan.verifyWlanScanApResultList(aps);
    aps.clear();
}

/**
 * Test a direct scan that finds nothing.
 */
void TestWlanQtUtils::testDirectScanEmpty()
{
    // Form result list
    testContext.mScan.createDefaultWlanScanApResultList(0);

    // Request a direct scan to get result signal
    wlanQtUtils_->scanWlanDirect(QString("notavailable"));
    
    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanDirectReady, WlanQtUtils::ScanStatusOk);

    // Get and verify the list of available WLAN APs.
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlanAps(aps);
    testContext.mScan.verifyWlanScanApResultList(aps);
}

/**
 * Test cancellation of direct scan.
 */
void TestWlanQtUtils::testDirectScanStop()
{
    // Do not complete scan request immediately
    testContext.mScan.mCompleteWlanApScan = false;
    
    // Start the scan
    wlanQtUtils_->scanWlanDirect(QString("testssid"));
    // Cancel the scan
    wlanQtUtils_->stopWlanScan();

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanDirectReady, WlanQtUtils::ScanStatusCancelled);
}

/**
 * Test direct scan failure.
 */
void TestWlanQtUtils::testDirectScanError()
{
    // Set the scan to fail
    testContext.mScan.mApScanRetValue = KErrGeneral;
    
    // Start the scan
    wlanQtUtils_->scanWlanDirect(QString("boundtofail"));

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanDirectReady, WlanQtUtils::ScanStatusError);
}

/**
 * This function tests creation of WLAN IAP in a succesful manner.
 * - WLAN scan is made because otherwise we cannot verify that IAP creation is successful.
 * - Check that there are no available WLAN IAPs.
 * - Fetch SNAP list.
 * - Create WLAN IAP.
 * - Check that WLAN IAP has been created and that this IAP is not in WLAN AP scan results.
 */
void TestWlanQtUtils::testCreateIapOk()
{
    // Form WLAN AP scan result list
    testContext.mScan.createDefaultWlanScanApResultList(1);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSsid, "testCreateIapOk");
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdSecurityMode,CMManagerShim::WlanSecModeWpa);
    testContext.mScan.mWlanScanApResultList[0]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse,true);

    // Form WLAN IAP scan result list
    testContext.mScan.createDefaultWlanScanIapResultList(0);

    // Request a scan to get result signal
    wlanQtUtils_->scanWlans();

    // Let active object run and verify signal.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Ensure there are no WLAN IAPs but there is one scan result.
    QList< QSharedPointer<WlanQtUtilsAp> > iaps;
    QList< QSharedPointer<WlanQtUtilsAp> > aps;
    wlanQtUtils_->availableWlans(iaps, aps);

    QCOMPARE(iaps.count(), 0);
    testContext.mScan.verifyWlanScanApResultList(aps);

    // Execute createIap() function
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testCreateIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testCreateIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWpaPsk, "100euronlounas");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);

    // Form WLAN IAP scan result list
    testContext.mScan.createDefaultWlanScanIapResultList(1);
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdIapId, iapId);
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdName, QString("testCreateIapOk"));
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdSsid, QString("testCreateIapOk"));
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdSecurityMode, (int)(CMManagerShim::WlanSecModeWpa));
    testContext.mScan.mWlanScanIapResultList[0]->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);

    // Re-execute the scan so that scan lists get updated due to the IAP creation
    wlanQtUtils_->scanWlans();
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);

    // Verify that created IAP is in the IAP list and AP list is empty.
    wlanQtUtils_->availableWlans(iaps, aps);
    testContext.mScan.verifyWlanScanIapResultList(iaps);
    QCOMPARE(aps.count(), 0);
}

/**
 * This function tests creation of WLAN IAP in unsuccesful manner.
 */
void TestWlanQtUtils::testCreateIapErr()
{
    // Execute createIap() function with invalid parameters
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testCreateIapErr");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testCreateIapErr");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, false);
    // Invalid security mode -> exception
    wlanAp->setValue(
        WlanQtUtilsAp::ConfIdSecurityMode,
        CMManager::EWlanSecModeWAPI + 1111);
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId == WlanQtUtils::IapIdNone);
}

/**
 * This function tests creation of WLAN IAPs with different WEP keys.
 */
void TestWlanQtUtils::testCreateIapWepKeys()
{
    // Create the IAPs with different kind of WEP keys
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testCreateIapWepKeys");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testCreateIapWepKeys");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWep);
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex2);
    // All keys in HEX format
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey1, "1234567891");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey2, "1234567891");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey3, "1234567891");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey4, "1234567891");

    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);
    
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex3);
    // All keys in ASCII format
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey1, "12345");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey2, "12345");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey3, "12345");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey4, "12345");

    iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);

    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex4);
    // Different keys, including a missing one
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey1, "");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey2, "12345678911234567892123456");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey3, "12345");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdWepKey4, "1234567891123");

    iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);
}

/**
 * This function tests updating of WLAN IAP in a succesful manner.
 */
void TestWlanQtUtils::testUpdateIapOk()
{
    // Create an IAP that can be updated
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testUpdateIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testUpdateIapOk");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());

    // Update the name and verify it changed
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "UPDATED_testUpdateIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "UPDATED_testUpdateIapOk");

    bool success = wlanQtUtils_->updateIap(iapId, wlanAp.data());
    QVERIFY(success == true);
    QString name = wlanQtUtils_->iapName(iapId);
    QCOMPARE(name, QString("UPDATED_testUpdateIapOk"));
}

/**
 * This function tests updating of WLAN IAP in unsuccesful manner.
 */
void TestWlanQtUtils::testUpdateIapErr()
{
    // Try to update a non-existing IAP
    WlanQtUtilsAp wlanAp;
    bool success = wlanQtUtils_->updateIap(200, &wlanAp);
    QVERIFY(success == false);
}

/**
 * This function tests deletion of WLAN IAP in a succesful manner.
 */
void TestWlanQtUtils::testDeleteIapOk()
{
    // Create an IAP that can be deleted
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testDeleteIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testDeleteIapOk");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());

    // Check that IAP now exists
    QString name = wlanQtUtils_->iapName(iapId);
    QCOMPARE(name, QString("testDeleteIapOk"));
    
    // Delete it and verify it does not exist anymore
    wlanQtUtils_->deleteIap(iapId);
    name = wlanQtUtils_->iapName(iapId);
    QVERIFY(name.isEmpty());
}

/**
 * This function tests deletion of WLAN IAP in unsuccesful manner.
 */
void TestWlanQtUtils::testDeleteIapErr()
{
    // Try to delete a non-existing IAP
    wlanQtUtils_->deleteIap(200);

    // No signals or return values to verify
}

/**
 * This function tests connecting to IAP in a succesful manner.
 */
void TestWlanQtUtils::testConnectIapOk()
{
    testContext.esock_.startRetValue_ = KErrNone;

    // Esock stub completes connection creation immediately
    wlanQtUtils_->connectIap(5);

    // Connection creation in ConnMon interface
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 5);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
        EConnMonCreateConnection,
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Connection status change in ConnMon interface
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    
    // Receive signal for connection opening (caused by connectIap, which completed immediately)
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), 5); 

    // Connection status change to opened in ConnMon interface. Sub test cases between test
    // cases check that no extra signals are sent
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));
}

/**
 * This function tests connecting to IAP in unsuccesful manner.
 */
void TestWlanQtUtils::testConnectIapErr()
{
    testContext.esock_.startRetValue_ = KErrGeneral;

    wlanQtUtils_->connectIap(7);

    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 7);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonDeleteConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
    
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkClosed_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), 7);
    QCOMPARE(arguments.at(1).toInt(), KErrGeneral);
}

// TODO: testDisconnectFail cases are needed when fail branches are
// implemented into the connmon wrapper
/**
 * This function tests IAP disconnecting functionality.
 */
void TestWlanQtUtils::testDisconnectIapOk()
{
    // Create and connect an IAP we can then disconnect
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testDisconnectIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testDisconnectIapOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);

    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    testContext.esock_.startRetValue_ = KErrNone;
    wlanQtUtils_->connectIap(iapId);
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), iapId); 
    
    // Make sure there is the newly created connection active and also
    // another one to gain coverage
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(2, iapId - 1);
    // The disconnect function does not have any return values or
    // signals related to the disconnection, thus plain
    // function call is done for the test.
    wlanQtUtils_->disconnectIap(iapId);
}

/**
 * This function tests IAP disconnecting functionality when there is no
 * IAP to disconnect.
 */
void TestWlanQtUtils::testDisconnectIapIgnored()
{
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 150);
    wlanQtUtils_->disconnectIap(200);
    wlanQtUtils_->disconnectIap(WlanQtUtils::IapIdNone);
}

/**
 * This function tests moving of an IAP into Internet destination. 
 */
void TestWlanQtUtils::testMoveIapToInternetOk()
{
    // First create the IAP to move
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testMoveIapToInternetOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testMoveIapToInternetOk");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);

    // Verify the name
    QCOMPARE(wlanQtUtils_->iapName(iapId), QString("testMoveIapToInternetOk"));

    // Now move the IAP to internet destination
    wlanQtUtils_->moveIapToInternetSnap(iapId);

    // No interface for checking the destination via WlanQtUtils, so skipping it
    
    // Delete the IAP
    wlanQtUtils_->deleteIap(iapId);
}

/**
 * This function tests connection status getter.
 */
void TestWlanQtUtils::testConnectionStatus()
{
    // Request status when there is no connection
    WlanQtUtils::ConnStatus status = wlanQtUtils_->connectionStatus();
    QCOMPARE(status, WlanQtUtils::ConnStatusDisconnected);
    
    // Make a connection and request its status
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 5);
    // Send event for connection creation.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
    // Send events for connection status change -> connecting
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KStartingConnection));

    // Request status when there is connection in connecting state
    status = wlanQtUtils_->connectionStatus();
    QCOMPARE(status, WlanQtUtils::ConnStatusConnecting);

    // Send events for connection status change -> connected
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);

    // Request status when there is connection in connected state
    status = wlanQtUtils_->connectionStatus();
    QCOMPARE(status, WlanQtUtils::ConnStatusConnected);
}

/**
 * This function tests IAP name getter with existing IAP ID.
 */
void TestWlanQtUtils::testIapNameFound()
{
    // Create the IAP we want to find with the getter
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testIapFound");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testIapFound");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa2);

    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    
    // Execute the function under test and check that we got the correct name
    QString name = wlanQtUtils_->iapName(iapId);
    QCOMPARE(name, QString("testIapFound"));
}

/**
 * This function tests IAP name getter with non-existing IAP ID.
 */
void TestWlanQtUtils::testIapNameNotFound()
{
    // Execute the function under test and check that we get no name as result
    QString name = wlanQtUtils_->iapName(200); // id in valid range, but not found -> KErrNotFound
    QVERIFY(name.isEmpty());

    name = wlanQtUtils_->iapName(1000); // id not in valid range -> KErrArgument
    QVERIFY(name.isEmpty());

    name = wlanQtUtils_->iapName(3); // id of cellular IAP -> discarded
    QVERIFY(name.isEmpty());
}

/**
 * This function tests active WLAN IAP getter with existing connection.
 */
void TestWlanQtUtils::testActiveIapFound()
{
    // Create the IAP we want to find with the getter
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testConnectedWlanIdFound");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testConnectedWlanIdFound");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);

    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, iapId);

    // Send event for connection creation.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
        EConnMonCreateConnection,
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Send events for connection status change -> opened.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KStartingConnection));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId);
        
    // Execute the function under test and check that we get valid ID as result
    int id = wlanQtUtils_->activeIap();
    QCOMPARE(id, iapId);
}

/**
 * This function tests active WLAN IAP getter with connection existing already during dll construction.
 */
void TestWlanQtUtils::testActiveIapFoundConstructor()
{
    // IAP IDs 4 and 5 exist in default commsdat file, 4 is GPRS, 5 is WLAN
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(2, 4);
    testContext.connMon_.activeConnections_.activeConnList_[0]->connMonBearerType_ = EBearerGPRS;

    // Create a new instance in order to test functionality triggered in constructor.
    WlanQtUtils *utils = new WlanQtUtils();

    // Execute the function under test and check that we get valid ID as result
    int id = utils->activeIap();
    QCOMPARE(id, 5);

    delete utils;
}

/**
 * This function tests active WLAN IAP getter without existing connection.
 */
void TestWlanQtUtils::testActiveIapNotFound()
{
    // Execute the function under test and check that we get invalid ID as result
    int id = wlanQtUtils_->activeIap();
    QCOMPARE(id, WlanQtUtils::IapIdNone);
}

/**
 * Test WLAN scan triggering interface.
 */
void TestWlanQtUtils::testScanWlans()
{
    // Execute function under test
    wlanQtUtils_->scanWlans();

    // No need to verify scan results here, testAvailableWlan* test cases
    // are for that. Just make sure the result signal is received.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);
}

/**
 * Test WLAN scan result ignoring & deletion during an ongoing request.
 */
void TestWlanQtUtils::testScanWlansIgnored()
{
    // Do not complete scan request immediately
    testContext.mScan.mCompleteWlanApScan = false;
    
    // Execute function under test
    wlanQtUtils_->scanWlans();

    // Ongoing scan is cancelled in destructors
}

/**
 * Test WLAN scan stopping interface when scan is active.
 */
void TestWlanQtUtils::testStopWlanScanOk()
{
    // Do not complete scan request immediately
    testContext.mScan.mCompleteWlanApScan = false;
    // Start a scan
    wlanQtUtils_->scanWlans();
    // Execute function under test
    wlanQtUtils_->stopWlanScan();

    // Catch & check the scan result signal
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusCancelled);

    // Restart scan to make sure previous is really finished
    testContext.mScan.mCompleteWlanApScan = true;
    wlanQtUtils_->scanWlans();

    // No need to verify scan results here, testAvailableWlan* test cases
    // are for that. Just make sure the result signal is received.
    subTestScanResultSignal(mSignalScanReady, WlanQtUtils::ScanStatusOk);
}

/**
 * Test WLAN scan stopping interface when no scan is ongoing.
 */
void TestWlanQtUtils::testStopWlanScanIgnored()
{
    // Stop without having an active scan
    wlanQtUtils_->stopWlanScan();
    
    // No return values or signals to verify
}

/**
 * This function tests Wlan network opening signal when network is not opened by the dll.
 */
void TestWlanQtUtils::testWlanNetworkOpened()
{
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 5);

    // Send event for connection creation.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Send events for connection status change -> opened.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConfigDaemonFinishedRegistration));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);

    // Send uninteresting event to gain coverage
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
        EConnMonNetworkStatusChange,
        10));
}

/**
 * This function tests Wlan network closing signal when network is not closed by the dll.
 */
void TestWlanQtUtils::testWlanNetworkClosed()
{
    // First create a connection
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 5);

    // Send event for connection creation.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Send events for connection status change -> opened.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);

    // Send event for connection status change -> closed.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerClosed));

    // Send event for connection deletion.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonDeleteConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    subTestSignalWaitAndTake(signalWlanNetworkClosed_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);
    QCOMPARE(arguments.at(1).toInt(), KErrNone);
}

/**
 * This function tests WlanQtUtilsAp copy constructor.
 */
void TestWlanQtUtils::testApCopyConstructor()
{
    WlanQtUtilsAp firstAp;
    firstAp.setValue(WlanQtUtilsAp::ConfIdSsid, "testApCopyConstructor");
    firstAp.setValue(WlanQtUtilsAp::ConfIdSignalStrength, 90);
    firstAp.setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    firstAp.setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    firstAp.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);    
    firstAp.setValue(WlanQtUtilsAp::ConfIdWpaPsk, "100euronlounas");
    
    // Create the copy AP
    WlanQtUtilsAp secondAp(firstAp);
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdSsid).toString(), QString("testApCopyConstructor"));
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdSecurityMode).toInt(), (int)(CMManagerShim::WlanSecModeWpa));
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdWpaPskUse).toBool(), true);
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdSignalStrength).toInt(), 90);
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdConnectionMode).toInt(), (int)(CMManagerShim::Infra));
    QCOMPARE(secondAp.value(WlanQtUtilsAp::ConfIdWpaPsk).toString(), QString("100euronlounas"));
}

/**
 * This function tests ICT when connection test passes.
 */
void TestWlanQtUtils::testConnectionTestOk()
{
    // Create new IAP to test
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testConnectionTestOk");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testConnectionTestOk");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);

    testContext.esock_.startRetValue_ = KErrNone;

    // Esock stub completes connection creation immediately
    wlanQtUtils_->connectIap(iapId, true);

    // Connection creation in ConnMon interface
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, iapId);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Connection status change in ConnMon interface
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    
    // Receive signal for connection opening (caused by connectIap, which completed immediately)
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), iapId); 

    // Connection status change to opened in ConnMon interface. Sub test cases between test
    // cases check that no extra signals are sent
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    // Connection test automatically started at this point. Call result function explicitly
    // No interface exists that we could check that the IAP is moved to Internet SNAP correctly
    wlanQtUtils_->d_ptr->updateIctResult(IctsWlanLoginInterface::IctsPassed);

    subTestSignalWaitAndTake(signalIctResult_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId); 
    QCOMPARE(arguments.at(1).toInt(), (int)WlanQtUtils::IctPassed);
}

/**
 * This function tests ICT when connection test fails.
 */
void TestWlanQtUtils::testConnectionTestErr()
{
    // Create and connect an IAP and request ICT to be run
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testConnectionTestErr1");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testConnectionTestErr1");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeWpa);
    
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    wlanQtUtils_->connectIap(iapId, true);

    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, iapId);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), iapId); 

    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    // Connection test automatically started at this point. Call result function explicitly
    // No interface exists that we could check that IAP is not moved to another SNAP
    wlanQtUtils_->d_ptr->updateIctResult(IctsWlanLoginInterface::IctsFailed);

    subTestSignalWaitAndTake(signalIctResult_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId); 
    QCOMPARE(arguments.at(1).toInt(), (int)WlanQtUtils::IctFailed);

    // Repeat with cancel status
    // Send event for connection status change -> closed.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerClosed));

    // Send event for connection deletion.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonDeleteConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    subTestSignalWaitAndTake(signalWlanNetworkClosed_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId);
    QCOMPARE(arguments.at(1).toInt(), KErrNone);

    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testConnectionTestErr2");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testConnectionTestErr2");
    
    iapId = wlanQtUtils_->createIap(wlanAp.data());
    wlanQtUtils_->connectIap(iapId, true);

    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, iapId);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), iapId); 

    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    // Connection test automatically started at this point. Call result function explicitly
    // No interface exists that we could check that IAP is not moved to another SNAP
    wlanQtUtils_->d_ptr->updateIctResult(IctsWlanLoginInterface::IctsCanceled);

    subTestSignalWaitAndTake(signalIctResult_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId); 
    QCOMPARE(arguments.at(1).toInt(), (int)WlanQtUtils::IctCancelled);
}

/**
 * This function tests ICT when connection test passes in hotspot case.
 */
void TestWlanQtUtils::testConnectionTestHotspot()
{
    // Create new IAP to test
    QScopedPointer<WlanQtUtilsAp> wlanAp(subTestNewAp());
    wlanAp->setValue(WlanQtUtilsAp::ConfIdName, "testConnectionTestHotspot");
    wlanAp->setValue(WlanQtUtilsAp::ConfIdSsid, "testConnectionTestHotspot");
    int iapId = wlanQtUtils_->createIap(wlanAp.data());
    QVERIFY(iapId != WlanQtUtils::IapIdNone);

    testContext.esock_.startRetValue_ = KErrNone;

    // Esock stub completes connection creation immediately
    wlanQtUtils_->connectIap(iapId, true);

    // Connection creation in ConnMon interface
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, iapId);
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Connection status change in ConnMon interface
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    
    // Receive signal for connection opening (caused by connectIap, which completed immediately)
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened_, &arguments); 
    QCOMPARE(arguments.at(0).toInt(), iapId); 

    // Connection status change to opened in ConnMon interface. Sub test cases between test
    // cases check that no extra signals are sent
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    // Connection test automatically started at this point.
    // Call hotspot case slot explicitly
    wlanQtUtils_->d_ptr->updateIctHotspotCase();
    
    // Call result function explicitly
    // No interface exists that we could check that the IAP is not moved to another SNAP
    wlanQtUtils_->d_ptr->updateIctResult(IctsWlanLoginInterface::IctsHotspotPassed);

    subTestSignalWaitAndTake(signalIctResult_, &arguments);
    QCOMPARE(arguments.at(0).toInt(), iapId); 
    QCOMPARE(arguments.at(1).toInt(), (int)WlanQtUtils::IctHotspotPassed);
}

/**
 * This function tests that Gprs connection events are ignored.
 */
void TestWlanQtUtils::testConnMonEventGprs()
{
    // First create a connection
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 4);
    testContext.connMon_.activeConnections_.activeConnList_[0]->connMonBearerType_ = EBearerGPRS;
    
    // Send event for connection creation.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonCreateConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    // Send events for connection status change -> opened.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    // Send event for connection status change -> closed.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerClosed));

    // Send event for connection deletion.
    wlanQtUtils_->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonDeleteConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));
}

/**
 * This function tests ConnMon event handling with connection existing already during dll construction.
 */
void TestWlanQtUtils::testConnMonEventCreatedBeforeConstructor()
{
    // IAP ID 5 exists in default commsdat file
    testContext.connMon_.activeConnections_.createDefaultActiveConnList(1, 5);

    // Create a new instance in order to test functionality triggered in constructor.
    WlanQtUtils *utils = new WlanQtUtils();

    QSignalSpy *signalWlanNetworkOpened = new QSignalSpy(utils, SIGNAL(wlanNetworkOpened(int)));
    QVERIFY(signalWlanNetworkOpened->isValid() == true);
    QSignalSpy *signalWlanNetworkClosed = new QSignalSpy(utils, SIGNAL(wlanNetworkClosed(int, int)));
    QVERIFY(signalWlanNetworkClosed->isValid() == true);
    
    // Send events for connection status change -> opened.
    utils->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KConnectionOpen));
    utils->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerOpen));

    QList<QVariant> arguments;
    subTestSignalWaitAndTake(signalWlanNetworkOpened, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);

    // Send event for connection status change -> closed.
    utils->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonConnectionStatusChange(
        testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId(),
        0,
        KLinkLayerClosed));

    // Send event for connection deletion.
    utils->d_ptr->mConMonWrapper->d_ptrInfo->EventL(CConnMonEventBase(
            EConnMonDeleteConnection,
            testContext.connMon_.activeConnections_.activeConnList_[0]->connectionId()));

    subTestSignalWaitAndTake(signalWlanNetworkClosed, &arguments);
    QCOMPARE(arguments.at(0).toInt(), 5);
    QCOMPARE(arguments.at(1).toInt(), KErrNone);
    
    delete signalWlanNetworkOpened;
    delete signalWlanNetworkClosed;
    delete utils;
}

// ---------------------------------------------------------
// SUB TEST CASES
// ---------------------------------------------------------

/**
 * This function waits for active objects to get time to run, takes the arguments of the
 * first signal and returns them from the given signal spy.
 * This function also verifies that the number of signals is 1.
 * 
 * @param[in] spy Signal spy.
 * @param[out] arguments Arguments of the first signal in the given signal spy. NULL if arguments not needed.
 */
void TestWlanQtUtils::subTestSignalWaitAndTake(QSignalSpy* spy, QList<QVariant>* arguments)
{
    QTest::qWait(1);
    QCOMPARE(spy->count(), 1);
    QList<QVariant> arguments_tmp = spy->takeFirst();
    if (arguments != NULL) {
        *arguments = arguments_tmp;
    }
}

/**
 * Test case for catching and verifying the scan result signal.
 * 
 * @param [in] status Scan result status.
 */
void TestWlanQtUtils::subTestScanResultSignal(QSignalSpy* spy, int status)
{
    QList<QVariant> arguments;
    subTestSignalWaitAndTake(spy, &arguments);
    QCOMPARE(arguments.length(), 1);
    QCOMPARE(arguments.at(0).toInt(), status);
}

/**
 * This function loads given CommsDat file, replacing current one.
 * If CommsDat file doesn't exist, it can be re-created by commanding WST script.
 * Using help switch is a quick way:
 *     run_wst HELP
 * 
 * @param newCommsdatFilename Filename of the new CommsDat to be loaded. 
 */
void TestWlanQtUtils::subTestLoadCommsDatFile(QString newCommsdatFilename)
{
    // EPOC's CommsDat filename   
    const QString epocCommsdatFilename("cccccc00.cre");
    // EPOC's directory for CommsDat file
    const QString commsdatDir("c:\\private\\10202be9\\persists\\");
    // Created backup directory under EPOC for CommsDat files
    const QString storeDir("c:\\private\\10202be9\\persists\\backup\\");
    QString nameOld = commsdatDir + epocCommsdatFilename;
    QString nameDefault = storeDir + newCommsdatFilename;

    // First remove the old CommsDat file.
    Q_ASSERT(QFile::remove(nameOld) == TRUE);
    
    // Copy the stored default CommsDat file.
    Q_ASSERT(QFile::copy(nameDefault, nameOld) == TRUE);
}

/**
 * 
 */
void TestWlanQtUtils::subTestNewWlanQtUtils()
{
    delete wlanQtUtils_;
    wlanQtUtils_ = new WlanQtUtils();

    delete mSignalScanReady;
    mSignalScanReady = new QSignalSpy(wlanQtUtils_, SIGNAL(wlanScanReady(int)));
    QVERIFY(mSignalScanReady->isValid() == true);

    delete mSignalScanApReady;
    mSignalScanApReady = new QSignalSpy(wlanQtUtils_, SIGNAL(wlanScanApReady(int)));
    QVERIFY(mSignalScanApReady->isValid() == true);

    delete mSignalScanDirectReady;
    mSignalScanDirectReady = new QSignalSpy(wlanQtUtils_, SIGNAL(wlanScanDirectReady(int)));
    QVERIFY(mSignalScanDirectReady->isValid() == true);

    delete signalWlanNetworkOpened_;
    signalWlanNetworkOpened_ = new QSignalSpy(wlanQtUtils_, SIGNAL(wlanNetworkOpened(int)));
    QVERIFY(signalWlanNetworkOpened_->isValid() == true);
    
    delete signalWlanNetworkClosed_;
    signalWlanNetworkClosed_ = new QSignalSpy(wlanQtUtils_, SIGNAL(wlanNetworkClosed(int, int)));
    QVERIFY(signalWlanNetworkClosed_->isValid() == true);
    
    delete signalIctResult_;
    signalIctResult_ = new QSignalSpy(wlanQtUtils_, SIGNAL(ictResult(int, int)));
    QVERIFY(signalIctResult_->isValid() == true);
}

/**
 * Create a new AP and fill it with default values. 
 */
WlanQtUtilsAp *TestWlanQtUtils::subTestNewAp()
{
    WlanQtUtilsAp *ap = new WlanQtUtilsAp;

    // Set default values
    ap->setValue(WlanQtUtilsAp::ConfIdName, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdSsid, QByteArray());
    ap->setValue(WlanQtUtilsAp::ConfIdSignalStrength, 90);
    ap->setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    ap->setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeOpen);
    ap->setValue(WlanQtUtilsAp::ConfIdWpaPskUse, false);
    ap->setValue(WlanQtUtilsAp::ConfIdWpaPsk, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdWepKey1, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdWepKey2, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdWepKey3, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdWepKey4, QString());
    ap->setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1);
    ap->setValue(WlanQtUtilsAp::ConfIdHidden, false);
    ap->setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, false);

    return ap;
}
