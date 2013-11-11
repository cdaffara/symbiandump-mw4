/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 */

// System includes
#include <HbApplication>
#include <QtCore>
#include <QTest>
#include <QDebug>
#include <QList>

// User includes
#include "testwlanwizard.h"
#include "testwlanwizardeap.h"
#include "testwlanwizardconnect.h"
#include "testwlanwizardmanual.h"
#include "testwlanwizardwps.h"

static bool match(int argc, char *argv[], const QString &tc);

// ======== LOCAL FUNCTIONS ========
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    int res = 0;
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    
    // log folder: 
    // - emulator: \epoc32\winscw\c\data
    // - device:   c:\data
    
    // By default all test cases are executed.
    // Use following to limit the testcases as your needs:
    // t_wlanwizard -t <testcases>
    //     testcases    utils connect manual eap wps
    // e.g. t_wlanwizard -t connect wps 
    // runs only "connect" and "wps" test cases
    
    if (match(argc, argv, "utils")) {
        app.setApplicationName("TestWlanWizardUtilities");
        pass[2] = "c:\\data\\TestWlanWizardUtilities.txt";
        TestWlanWizard *tcUtilities = new TestWlanWizard();
        res |= QTest::qExec(tcUtilities, 3, pass);
        delete tcUtilities;
        tcUtilities = NULL;
    }
    
    if (match(argc, argv, "connect")) {
        app.setApplicationName("TestWlanWizardConnect");
        pass[2] = "c:\\data\\TestWlanWizardConnect.txt";
        TestWlanWizardConnect *tcConnect = new TestWlanWizardConnect();
        res |= QTest::qExec(tcConnect, 3, pass);
        delete tcConnect;
        tcConnect = NULL;
    }
    
    if (match(argc, argv, "manual")) {
        app.setApplicationName("TestWlanWizardManual");
        pass[2] = "c:\\data\\TestWlanWizardManual.txt";
        TestWlanWizardManual *tcManual = new TestWlanWizardManual();
        res |= QTest::qExec(tcManual, 3, pass);
        delete tcManual;
        tcManual = NULL;
    }
    
    if (match(argc, argv, "eap")) {
        app.setApplicationName("TestWlanWizardEap");
        pass[2] = "c:\\data\\TestWlanWizardEap.txt";
        TestWlanWizardEap *tcEap = new TestWlanWizardEap();
        res |= QTest::qExec(tcEap, 3, pass);
        delete tcEap;
        tcEap = NULL;
    }

    if (match(argc, argv, "wps")) {
        app.setApplicationName("TestWlanWizardWps");
        pass[2] = "c:\\data\\TestWlanWizardWps.txt";
        TestWlanWizardWps *tcWps = new TestWlanWizardWps();
        res |= QTest::qExec(tcWps, 3, pass);
        delete tcWps;
        tcWps = NULL;
    }
    
    return res;
}

bool match(int argc, char *argv[], const QString &tc)
{
    bool testArgument = false;
    bool testCase = false;
    bool ret = true;
    
    int i = argc;
    while( --i > 0) {
        if (QString(argv[i]) == QString("-t")) {
            testArgument = true;
            break;
        }
    }
    
    i = argc;
    while( --i > 0) {
        if (QString(argv[i]) == tc) {
            testCase = true;
            break;
        }
    }
    
    if (testArgument && !testCase) {
        ret = false;
    }
    
    return ret;
}
