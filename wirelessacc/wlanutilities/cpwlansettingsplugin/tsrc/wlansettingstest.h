/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

#ifndef WLANSETTINGSTEST_H_
#define WLANSETTINGSTEST_H_


#include <QtTest/QtTest>
#include <QScopedPointer>
#include "wlansettings.h"

class WlanSettingsTest: public QObject
{
    Q_OBJECT
    
public:
    WlanSettingsTest();    
    ~WlanSettingsTest();
    
private slots:  
    void test_setWlanPowerMode();
    
    void test_CheckJoinWlanModeManual();
    
    void test_CheckWlanScanIntervalManual();
    
    void test_CheckJoinWlanModeAuto();
    
    void test_CheckWlanScanIntervalAuto();

    void test_CheckManualIntervalValidity();
    
    void test_CheckWlanPowerSavingMode();
    
private:    //data
    
    QScopedPointer<WlanSettings> mSecuritySettings;
    uint mScanIntervalAuto;
    uint mScanIntervalManual;
    int mJoinWlanMode;
    
};


#endif /* WLANSETTINGSTEST_H_ */
