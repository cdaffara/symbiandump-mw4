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


#include "wlansettingstest.h"

//constructor
WlanSettingsTest::WlanSettingsTest()
{
    mSecuritySettings.reset(new WlanSettings);
    
    //Initialise the wlansettings
    mSecuritySettings->init();
}


//Test Scenario : 1
void WlanSettingsTest::test_setWlanPowerMode()
{
    int powerSavingOption(ETrue);
    
    int error  = mSecuritySettings->setWlanPowerSaving(powerSavingOption);
    
    QVERIFY(error == KErrNone);
}

/////////////////////////////////////////Manual WLan joining Mode//////////////////////////////////////////////////

//Test Scenario : 2
void WlanSettingsTest::test_CheckJoinWlanModeManual()
{
    mJoinWlanMode = WlanSettings::EScanNetworkUserDefined;
    int error  = mSecuritySettings->setJoinWlanMode(mJoinWlanMode);
    
    QVERIFY(error == KErrNone);
    
    //Load the Settings
    int loadError = mSecuritySettings->loadSettings();       
    QVERIFY(loadError == KErrNone);
    
    //Verify Mode
    int mode  = mSecuritySettings->joinWlanMode();        
    QVERIFY(mode == mJoinWlanMode );
}

/////////////////////////////////////////User-defined Scan Interval/////////////////////////////////////
//Test Scenario : 3
void WlanSettingsTest::test_CheckWlanScanIntervalManual()
{
    
    mScanIntervalManual = 16;
    
    //Set some user-defined Interval
    int setError  = mSecuritySettings->setWlanScanInterval(mScanIntervalManual);
    QVERIFY(setError == KErrNone);
    
    //Load the Settings
    int loadError = mSecuritySettings->loadSettings();       
    QVERIFY(loadError == KErrNone);
    
    //Check if the interval set is right
    uint scanIntervalset  = mSecuritySettings->scanInterval();        
    QVERIFY(scanIntervalset == mScanIntervalManual);
    
    //Verify the Network type, based on the scan interval set
    int networkType = mSecuritySettings->scanNetworkType();
    QVERIFY(networkType == WlanSettings::EScanNetworkUserDefined);
    
}


//////////////////////////////////////////Automatic WLAN joining Mode///////////////////////////////////////////

//Test Scenario : 4
void WlanSettingsTest::test_CheckJoinWlanModeAuto()
{
    mJoinWlanMode = WlanSettings::EScanNetworkAuto;
    int error  = mSecuritySettings->setJoinWlanMode(mJoinWlanMode);
    
    QVERIFY(error == KErrNone);
    
    
    //Load the Settings
    int loadError = mSecuritySettings->loadSettings();       
    QVERIFY(loadError == KErrNone);
    
    
    //Verify Mode
    int mode  = mSecuritySettings->joinWlanMode();        
    QVERIFY(mode == mJoinWlanMode );

}

///////////////////////////////////////Automatic Scan Interval/////////////////////////////////////////
//Test Scenario : 5
void WlanSettingsTest::test_CheckWlanScanIntervalAuto()
{
    uint KDefaultScanInterval(5);
    
    mScanIntervalAuto = WlanSettings::ScanNetworkAuto;
    
    //Set the Auto Interval
    int setError  = mSecuritySettings->setWlanScanInterval(mScanIntervalAuto);    
    QVERIFY(setError == KErrNone);
    
    //Load the Settings
    int loadError = mSecuritySettings->loadSettings();       
    QVERIFY(loadError == KErrNone);
    
    //Check the Auto Interval
    uint scanIntervalset  = mSecuritySettings->scanInterval();        
    QVERIFY(scanIntervalset == KDefaultScanInterval );
    
    //Verify the Network type, based on the scan interval set
    int networkType = mSecuritySettings->scanNetworkType();
    QVERIFY(networkType == WlanSettings::EScanNetworkAuto);
}

//Test Scenario : 6
void WlanSettingsTest::test_CheckManualIntervalValidity()
{
    uint KDefaultScanInterval(5);
    //Invalid User defined Interval
    mScanIntervalManual = 45;
       
    int setError  = mSecuritySettings->setWlanScanInterval(mScanIntervalManual);
    QVERIFY(setError == KErrNone);
    
    //Load the Settings
    int loadError = mSecuritySettings->loadSettings();       
    QVERIFY(loadError == KErrNone);
    
    //Validate ;When User-defined Interval is invalid,Auto Interval should have been set
    uint scanIntervalset  = mSecuritySettings->scanInterval();        
    QVERIFY(scanIntervalset == KDefaultScanInterval);

}

//Test Scenario : 7
void WlanSettingsTest::test_CheckWlanPowerSavingMode()
{
    int wlanPowerSavingMode(ETrue);
    
    int setError  = mSecuritySettings->setWlanPowerSaving(wlanPowerSavingMode);
    QVERIFY(setError == KErrNone);
    
    //Verify Power saving mode parameter
    int powerSavingMode  = mSecuritySettings->isWlanPowerSavingEnabled();
    QVERIFY(powerSavingMode == wlanPowerSavingMode);
}

//Delete data members
WlanSettingsTest::~WlanSettingsTest()
{

}
