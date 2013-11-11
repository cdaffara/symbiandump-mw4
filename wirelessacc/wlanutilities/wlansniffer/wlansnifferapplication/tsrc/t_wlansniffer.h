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
* This is the header file for testing Wlan Sniffer application.
*/

#ifndef T_WLANSNIFFER_H
#define T_WLANSNIFFER_H

// System includes

#include <QObject>
#include <QSharedPointer>

// User includes

// Forward declarations

class HbListWidget;
class WlanQtUtilsAp;
class WlanSnifferListWidget;

// External data types

// Constants

// Class declaration

class TestWlanSniffer: public QObject
{
    Q_OBJECT
    
private slots:

    // Functions from QTest framework.
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test functions for WlanSnifferListWidget class
    void testUpdateContentIapAdded();
    void testUpdateContentApAdded();
    void testUpdateContentIapRemoved();
    void testUpdateContentApRemoved();
    void testUpdateContentIapConnected();
    void testUpdateContentIapDisconnected();
    void testUpdateContentRobustness1();
    void testUpdateContentRobustness2();
    void testUpdateContentSort();
    void testScrollTo();
    
private:
    
    // Helper functions, that is, sub test cases.
    void verifyIap(int row, int iapId);
    void verifyAp(int row, QString ssid, int securityMode);
    void appendWlanAp(
        QString ssId, 
        int signalStrength, 
        int networkMode, 
        int securityMode);
    void appendWlanIap(
        int iapId,
        int netId,
        QString name,
        QString ssId, 
        int signalStrength, 
        int securityMode);

private: // data
    
    HbListWidget *mListWidget;
    WlanSnifferListWidget *mWidget;
    QList< QSharedPointer<WlanQtUtilsAp> > mIaps;
    QList< QSharedPointer<WlanQtUtilsAp> > mAps;
};

#endif // T_WLANSNIFFER_H
