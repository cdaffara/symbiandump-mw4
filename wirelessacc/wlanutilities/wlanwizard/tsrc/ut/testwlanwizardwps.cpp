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
* WLAN Wizard UT.
*/

// System includes

#include <QTest>
#include <QDebug>
#include <QList>

#include <cmmanagerdefines_shim.h>
#include <wlanerrorcodes.h>

// User includes

#include "testwlanwizardwps.h"
#include "testwlanwizardwps_conf.h"
#include "wlanwizard.h"
#include "wlanwizard_p.h"
#include "wlanqtutils_context.h"
#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "wlanwizardpageinternal.h"
#include "wpswizardpage.h"

#include "wlanmgmtclient_context.h"

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// TEST CASES
// ---------------------------------------------------------

void TestWlanWizardWps::tcConfigureManualOpen()
{
#ifdef tcConfigureManualOpen_enabled
    const QString ssid("tcConfigureManualOpen"); 
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    WlanQtUtilsAp ap;
    ap.setValue(WlanQtUtilsAp::ConfIdConnectionMode, CMManagerShim::Infra);
    ap.setValue(WlanQtUtilsAp::ConfIdSecurityMode, CMManagerShim::WlanSecModeOpen);
    ap.setValue(WlanQtUtilsAp::ConfIdName, ssid);
    ap.setValue(WlanQtUtilsAp::ConfIdSsid, ssid.toUtf8());
    ap.setValue(WlanQtUtilsAp::ConfIdHidden, false);
    ap.setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, false);

    // Default values
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true );
    ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1 );
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, QString());
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeOpen,
        false,
        true);
        
    mView->showWizard();
    verifyModeSelection();
    QCOMPARE(selectRadioButton("radioButtonList", 2), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( mWlanQtUtilsContext->verifyWlanIapSettings(ap), true);
#endif
}

void TestWlanWizardWps::tcPushButtonOpen()
{
#ifdef tcPushButtonOpen_enabled
    tcPushButton(
        "tcPushButtonOpen",
        EWlanIapSecurityModeAllowUnsecure,
        CMManagerShim::WlanSecModeOpen,
        EWlanOperatingModeInfrastructure,
        CMManagerShim::Infra,
        EWlanDefaultWepKey1,
        CMManagerShim::WepKeyIndex1);
#endif
}

void TestWlanWizardWps::tcPushButtonWep1()
{
#ifdef tcPushButtonWep1_enabled
    tcPushButton(
        "tcPushButtonWep1",
        EWlanIapSecurityModeWep,
        CMManagerShim::WlanSecModeWep,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey1,
        CMManagerShim::WepKeyIndex1);
#endif
}

void TestWlanWizardWps::tcPushButtonWep2()
{
#ifdef tcPushButtonWep2_enabled
    tcPushButton(
        "tcPushButtonWep2",
        EWlanIapSecurityModeWep,
        CMManagerShim::WlanSecModeWep,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey2,
        CMManagerShim::WepKeyIndex2);
#endif
}

void TestWlanWizardWps::tcPushButtonWep3()
{
#ifdef tcPushButtonWep3_enabled
    tcPushButton(
        "tcPushButtonWep3",
        EWlanIapSecurityModeWep,
        CMManagerShim::WlanSecModeWep,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey3,
        CMManagerShim::WepKeyIndex3);
#endif
}

void TestWlanWizardWps::tcPushButtonWep4()
{
#ifdef tcPushButtonWep4_enabled
    tcPushButton(
        "tcPushButtonWep4",
        EWlanIapSecurityModeWep,
        CMManagerShim::WlanSecModeWep,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey4,
        CMManagerShim::WepKeyIndex4);
#endif
}

void TestWlanWizardWps::tcPushButtonWpa()
{
#ifdef tcPushButtonWpa_enabled
    tcPushButton(
        "tcPushButtonWpa",
        EWlanIapSecurityModeWpa,
        CMManagerShim::WlanSecModeWpa,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey1,
        CMManagerShim::WepKeyIndex1);
#endif
}

void TestWlanWizardWps::tcPushButtonWpa2()
{
#ifdef tcPushButtonWpa2_enabled
    tcPushButton(
        "tcPushButtonWpa2",
        EWlanIapSecurityModeWpa2Only,
        CMManagerShim::WlanSecModeWpa2,
        EWlanOperatingModeAdhoc,
        CMManagerShim::Adhoc,
        EWlanDefaultWepKey1,
        CMManagerShim::WepKeyIndex1);
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupOOBInterfaceReadError()
{
#ifdef tcPinCode_KErrWlanProtectedSetupOOBInterfaceReadError_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupOOBInterfaceReadError,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupDecryptionCRCFailure()
{
#ifdef tcPinCode_KErrWlanProtectedSetupDecryptionCRCFailure_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupDecryptionCRCFailure,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetup2_4ChannelNotSupported()
{
#ifdef tcPinCode_KErrWlanProtectedSetup2_4ChannelNotSupported_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetup2_4ChannelNotSupported,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetup5_0ChannelNotSupported()
{
#ifdef tcPinCode_KErrWlanProtectedSetup5_0ChannelNotSupported_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetup5_0ChannelNotSupported,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupNetworkAuthFailure()
{
#ifdef tcPinCode_KErrWlanProtectedSetupNetworkAuthFailure_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupNetworkAuthFailure,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupNoDHCPResponse()
{
#ifdef tcPinCode_KErrWlanProtectedSetupNoDHCPResponse_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupNoDHCPResponse,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupFailedDHCPConfig()
{
#ifdef tcPinCode_KErrWlanProtectedSetupFailedDHCPConfig_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupFailedDHCPConfig,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupIPAddressConflict()
{
#ifdef tcPinCode_KErrWlanProtectedSetupIPAddressConflict_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupIPAddressConflict,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupCouldNotConnectToRegistrar()
{
#ifdef tcPinCode_KErrWlanProtectedSetupCouldNotConnectToRegistrar_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupCouldNotConnectToRegistrar,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupMultiplePBCSessionsDetected()
{
#ifdef tcPinCode_KErrWlanProtectedSetupMultiplePBCSessionsDetected_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupMultiplePBCSessionsDetected,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupRogueActivitySuspected()
{
#ifdef tcPinCode_KErrWlanProtectedSetupRogueActivitySuspected_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupRogueActivitySuspected,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupDeviceBusy()
{
#ifdef tcPinCode_KErrWlanProtectedSetupDeviceBusy_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupDeviceBusy,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupSetupLocked()
{
#ifdef tcPinCode_KErrWlanProtectedSetupSetupLocked_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupSetupLocked,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupMessageTimeout()
{
#ifdef tcPinCode_KErrWlanProtectedSetupMessageTimeout_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupMessageTimeout,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupRegistrationSessionTimeout()
{
#ifdef tcPinCode_KErrWlanProtectedSetupRegistrationSessionTimeout_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupRegistrationSessionTimeout,
        hbTrId("txt_occ_dialog_configuration_failed_please_try_ag"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupDevicePasswordAuthFailure()
{
#ifdef tcPinCode_KErrWlanProtectedSetupDevicePasswordAuthFailure_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupDevicePasswordAuthFailure,
        hbTrId("txt_occ_dialog_configuration_failed_authenticatio"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupPINMethodNotSupported()
{
#ifdef tcPinCode_KErrWlanProtectedSetupPINMethodNotSupported_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupPINMethodNotSupported,
        hbTrId("txt_occ_dialog_configuration_failed_authenticatio"));
#endif
}

void TestWlanWizardWps::tcPinCode_KErrWlanProtectedSetupPBMethodNotSupported()
{
#ifdef tcPinCode_KErrWlanProtectedSetupPBMethodNotSupported_enabled
    tcPinCode_failure(
        KErrWlanProtectedSetupPBMethodNotSupported,
        hbTrId("txt_occ_dialog_configuration_failed_authenticatio"));
#endif
}

void TestWlanWizardWps::tcPinCode_BackToPushButtonMode()
{
#ifdef tcPinCode_tcPinCode_BackToPushButtonMode
    tcPinCode_failure(
        KErrWlanProtectedSetupPBMethodNotSupported,
        hbTrId("txt_occ_dialog_configuration_failed_authenticatio"));
        
    QCOMPARE( mouseClickPrevious(), true );
    QTest::qWait(WaitTimeForUi);
        
    tcPushButton(
        "tcPinCode_failure",
        EWlanIapSecurityModeAllowUnsecure,
        CMManagerShim::WlanSecModeOpen,
        EWlanOperatingModeInfrastructure,
        CMManagerShim::Infra,
        EWlanDefaultWepKey1,
        CMManagerShim::WepKeyIndex1,
        1);
#endif
}

void TestWlanWizardWps::verifyModeSelection( int index )
{
    QCOMPARE( verifyCurrentPage(WpsWizardPage::PageWpsWizardStep2 ), true );
    if (index == -1) {
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    } else {
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    }
    
    QStringList list;
    list << hbTrId("txt_occ_list_use_pushbutton")
         << hbTrId("txt_occ_list_use_pin_code")
         << hbTrId("txt_occ_list_configure_manually");
    
    QCOMPARE(verifyRadioButtons("radioButtonList"), list);
    
    if (index != -1) {
        QCOMPARE(verifyRadioButtonState( "radioButtonList", index, list.at(index) ), true);
    }
}


void TestWlanWizardWps::tcPushButton(
    const QString &ssid,
    int secModeWlan,
    int secModeCmm,
    int operModeWlan,
    int operModeCmm,
    int defaultWepKeyIndexWlan,
    int defaultWepKeyIndexCmm,
    int index)
{
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    WlanQtUtilsAp ap;
    ap.setValue(WlanQtUtilsAp::ConfIdConnectionMode, operModeCmm);
    ap.setValue(WlanQtUtilsAp::ConfIdSecurityMode, secModeCmm);
    ap.setValue(WlanQtUtilsAp::ConfIdName, ssid);
    ap.setValue(WlanQtUtilsAp::ConfIdSsid, ssid.toUtf8());
    ap.setValue(WlanQtUtilsAp::ConfIdHidden, false);
    ap.setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, false);

    // Default values
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true );
    ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1 );
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, QString());
    
    if (secModeCmm == CMManagerShim::WlanSecModeWep){
        ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, "wepkey1");
        ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, "wepkey2");
        ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, "wepkey3");
        ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, "wepkey4");
        ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, defaultWepKeyIndexCmm );
    } else if (secModeCmm == CMManagerShim::WlanSecModeWpa ||
        secModeCmm == CMManagerShim::WlanSecModeWpa2) {
        ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, "wpapsk");
    }
    
    mWlanMgmtClientContext->setRunProtectedSetup(ssid, KErrNone, true);
    mWlanMgmtClientContext->appendResult(
        ssid,
        secModeWlan,
        operModeWlan,
        defaultWepKeyIndexWlan,
        "wepkey1",
        "wepkey2",
        "wepkey3",
        "wepkey4",
        "wpapsk");

    // Wizard is already in Mode Selection Page
    if (index == -1) {
        mView->mWizard->setParameters(
            ssid,
            ssid.toUtf8(),
            CMManagerShim::Adhoc,
            CMManagerShim::WlanSecModeWep,
            false,
            true);
        
        mView->showWizard();
    }
    
    verifyModeSelection(index);

    QCOMPARE(selectRadioButton("radioButtonList", 0), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( verifyCurrentPage(WpsWizardPage::PageWpsWizardStep3_Button), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE( mouseClickNext(), true);
    QCOMPARE( verifyCurrentPage(WpsWizardPage::PageWpsWizardStep4), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( mWlanQtUtilsContext->verifyWlanIapSettings(ap), true);
}


void TestWlanWizardWps::tcPinCode_failure(
    int errorCode,
    const QString &errorText)
{
    const QString ssid("tcPinCode_failure");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);

    mWlanMgmtClientContext->setRunProtectedSetup(ssid, errorCode, false);
        
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeWep,
        false,
        true);
        
    mView->showWizard();
    verifyModeSelection();

    QCOMPARE(selectRadioButton("radioButtonList", 1), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( verifyCurrentPage(WpsWizardPage::PageWpsWizardStep3_Number), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( verifyCurrentPage(WpsWizardPage::PageWpsWizardStep4), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, errorText), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
}
