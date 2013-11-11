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
* This is the header file for testing Wlan Qt Utilities library.
*/

#ifndef TESTWLANQTUTILS_H
#define TESTWLANQTUTILS_H

#include <QObject>

class QSignalSpy;
class WlanQtUtils;
class WlanQtUtilsAp;

class TestWlanQtUtils: public QObject
{
    Q_OBJECT
    
private slots:

    // Functions from QTest framework.
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test functions for public interface.
    
    // scanWlans()
    void testAvailableWlan();
    void testAvailableWlanDuplicates();
    void testAvailableWlanSecModes();
    void testAvailableWlanSsids();
    void testAvailableWlanWithIaps();
    void testAvailableWlanSequence();
    void testAvailableWlanScanEmpty();
    void testAvailableWlanScanError();
    
    // scanWlanAps()
    void testAvailableWlanAps();
    void testAvailableWlanApsScanEmpty();
    void testStopWlanApScan();
    void testWlanApScanError();
    void testAvailableWlanApsSpecialChars();
    
    // scanWlanDirect()
    void testDirectScanBasic();
    void testDirectScanEmpty();
    void testDirectScanStop();
    void testDirectScanError();

    void testCreateIapOk();
    void testCreateIapErr();
    void testCreateIapWepKeys();
    
    void testUpdateIapOk();
    void testUpdateIapErr();
    
    void testDeleteIapOk();
    void testDeleteIapErr();
    
    void testConnectIapOk();
    void testConnectIapErr();
    
    void testDisconnectIapOk();
    void testDisconnectIapIgnored();

    void testMoveIapToInternetOk();

    void testConnectionStatus();
    
    void testIapNameFound();
    void testIapNameNotFound();
    
    void testActiveIapFound();
    void testActiveIapFoundConstructor();
    void testActiveIapNotFound();

    void testScanWlans();
    void testScanWlansIgnored();
    
    void testStopWlanScanOk();
    void testStopWlanScanIgnored();

    void testWlanNetworkOpened();
    void testWlanNetworkClosed();

    void testApCopyConstructor();
    
    // Test functions for private implementation (tested via public interface)
    void testConnectionTestOk();
    void testConnectionTestErr();
    void testConnectionTestHotspot();

    void testConnMonEventGprs();
    void testConnMonEventCreatedBeforeConstructor();

private:
    
    // Helper functions, that is, sub test cases.
    void subTestSignalWaitAndTake(QSignalSpy *spy, QList<QVariant> *arguments);
    void subTestScanResultSignal(QSignalSpy* spy, int status);
    void subTestLoadCommsDatFile(QString newCommsdatFilename);
    void subTestNewWlanQtUtils();
    WlanQtUtilsAp *subTestNewAp();

    // Member variables.
    WlanQtUtils *wlanQtUtils_;
    QSignalSpy *mSignalScanReady;
    QSignalSpy *mSignalScanApReady;
    QSignalSpy *mSignalScanDirectReady;
    QSignalSpy *signalWlanNetworkOpened_;
    QSignalSpy *signalWlanNetworkClosed_;
    QSignalSpy *signalIctResult_;
    
    /* Default CommsDat file name
    SNAP ID=3: Internet:
        IAP ID = 1: packet data 1
        IAP ID = 3: packet data 2
        IAP ID = 4: packet data 3
        IAP ID = 5: WLAN IAP 1, EWlanSecModeOpen
        IAP ID = 6: WLAN IAP 2, EWlanSecModeWep, w2key
        IAP ID = 7: WLAN IAP 3, EWlanSecModeWpa, wlan3key
    SNAP ID = 4: Multimedia msg.
    SNAP ID = 5: WAP services
    SNAP ID = 6: My Snap:
        IAP ID = 8: Home WLAN, EWlanSecModeOpen
        IAP ID = 9: Streaming
     */
    static const QString commsDatDefault_;
};

#endif // TESTWLANQTUTILS_H

