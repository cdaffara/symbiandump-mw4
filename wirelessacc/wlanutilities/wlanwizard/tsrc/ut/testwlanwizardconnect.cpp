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

#include <HbApplication>
#include <HbDocumentLoader>
#include <HbStackedWidget>
#include <HbRadioButtonList>
#include <HbAction>
#include <HbLineEdit>
#include <HbLabel>

#include <QGraphicsWidget>
#include <QObjectList>
#include <QtCore>
#include <QTest>
#include <QDebug>
#include <QList>

#include <cmmanagerdefines_shim.h>
#include <wlanerrorcodes.h>

// User includes

#include "testwlanwizardconnect.h"
#include "testwlanwizardconnect_conf.h"
#include "hbautotest.h"
#include "wlanwizard.h"
#include "wlanwizard_p.h"
#include "wlanwizardpagessid.h"
#include "wlanqtutils_context.h"
#include "wlanqtutils.h"
#include "wlanqtutilsap.h"

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// TEST CASES
// ---------------------------------------------------------

/*!
 * Connect to open:
 * - verifies IAP settings
 * - ICT success
 * - Finish button pressed
 */
void TestWlanWizardConnect::tc_connect_to_open_success()
{
#ifdef tc_connect_to_open_success_enabled
    tc_connect_success("tc_connect_to_open_success", CMManagerShim::Infra,
        CMManagerShim::WlanSecModeOpen, false, "");
#endif 
}

/*!
 * Connect to open
 */
void TestWlanWizardConnect::tc_connect_to_open_success_adhoc()
{
#ifdef tc_connect_to_open_success_adhoc_enabled
    tc_connect_success("tc_connect_to_open_success_adhoc", CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, false, "");
#endif 
}

/*!
 * Connect to open
 */
void TestWlanWizardConnect::tc_connect_to_wep_success()
{
#ifdef tc_connect_to_wep_success_enabled
    tc_connect_success("tc_connect_to_wep_success", CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeWep, false, "abcde");
#endif 
}

/*!
 * Connect to open
 */
void TestWlanWizardConnect::tc_connect_to_wpa_psk_success()
{
#ifdef tc_connect_to_wpa_psk_success_enabled
    tc_connect_success("tc_connect_to_wpa_psk_success", CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, false, "password");
#endif 
}
/*!
 * Connect to open
 */
void TestWlanWizardConnect::tc_connect_to_wpa2_psk_success()
{
#ifdef tc_connect_to_wpa2_psk_success_enabled
    tc_connect_success("tc_connect_to_wpa2_psk_success", CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeWpa2, false, "password");
#endif 
}

/*!
 * Connect to open:
 * - Opens
 * - ICT success
 * - Cancel pressed in summary page
 */
void TestWlanWizardConnect::tc_connect_to_open_success_cancel()
{
#ifdef tc_connect_to_open_success_cancel_enabled

    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);

    mView->mWizard->setParameters(
        "tc_connect_to_open_success_cancel",
        "tc_connect_to_open_success_cancel",
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeOpen,
        false,
        false);

    mView->showWizard();
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "tc_connect_to_open_success_cancel"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QTest::qWait(WaitTimeForUi);

    // Ignore previous calls
    mWlanQtUtilsContext->calledMethods();

    QCOMPARE( mouseClickCancel(), true );

    QStringList calledMethods;
    calledMethods << "disconnectIap" << "deleteIap";

    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);
    QCOMPARE(verifyStatus(WizardStatusSignalCancelled), true);
#endif 
}

void TestWlanWizardConnect::tc_network_failure_OpenAuthFailed()
{
#ifdef tc_network_failure_OpenAuthFailed_enabled
    tc_network_error_codes(
        "tc_network_failure_OpenAuthFailed", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanOpenAuthFailed );
#endif
}
void TestWlanWizardConnect::tc_network_failure_SharedKeyAuthRequired()
{
#ifdef tc_network_failure_SharedKeyAuthRequired_enabled
    tc_network_error_codes(
        "tc_network_failure_SharedKeyAuthRequired", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanSharedKeyAuthRequired );
#endif
}
void TestWlanWizardConnect::tc_network_failure_WpaAuthRequired()
{
#ifdef tc_network_failure_WpaAuthRequired_enabled
    tc_network_error_codes(
        "tc_network_failure_WpaAuthRequired", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanWpaAuthRequired );
#endif
}
void TestWlanWizardConnect::tc_network_failure_WpaAuthFailed()
{
#ifdef tc_network_failure_WpaAuthFailed_enabled
    tc_network_error_codes(
        "tc_network_failure_WpaAuthFailed", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeWpa, 
        false, 
        "password",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanWpaAuthFailed );
#endif
}
void TestWlanWizardConnect::tc_network_failure_802dot1xAuthFailed()
{
#ifdef tc_network_failure_802dot1xAuthFailed_enabled
    tc_network_error_codes(
        "tc_network_failure_802dot1xAuthFailed", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlan802dot1xAuthFailed );
#endif
}
void TestWlanWizardConnect::tc_network_failure_IllegalEncryptionKeys()
{
#ifdef tc_network_failure_IllegalEncryptionKeys_enabled
    tc_network_error_codes(
        "tc_network_failure_IllegalEncryptionKeys", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanIllegalEncryptionKeys );
#endif
}
void TestWlanWizardConnect::tc_network_failure_PskModeRequired()
{
#ifdef tc_network_failure_PskModeRequired_enabled
    tc_network_error_codes(
        "tc_network_failure_PskModeRequired", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanPskModeRequired );
#endif
}
void TestWlanWizardConnect::tc_network_failure_EapModeRequired()
{
#ifdef tc_network_failure_EapModeRequired_enabled
    tc_network_error_codes(
        "tc_network_failure_EapModeRequired", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_authentication_unsuccessful"), 
        KErrWlanEapModeRequired );
#endif
}
void TestWlanWizardConnect::tc_network_failure_IllefalWpaPskKey()
{
#ifdef tc_network_failure_IllefalWpaPskKey_enabled
    tc_network_error_codes(
        "tc_network_failure_IllefalWpaPskKey", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_incorrect_wpa_preshared_key_pleas"), 
        KErrWlanIllegalWpaPskKey );
#endif
}
void TestWlanWizardConnect::tc_network_failure_SharedKeyAuthFailed()
{
#ifdef tc_network_failure_SharedKeyAuthFailed_enabled
    tc_network_error_codes(
        "tc_network_failure_SharedKeyAuthFailed", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_incorrect_wep_key_please_check_the"), 
        KErrWlanSharedKeyAuthFailed );
#endif
}
void TestWlanWizardConnect::tc_network_failure_ErrNone()
{
#ifdef tc_network_failure_ErrNone_enabled
    tc_network_error_codes(
        "tc_network_failure_ErrNone", 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeOpen, 
        false, 
        "",
        hbTrId("txt_occ_dialog_incorrect_wep_key_please_check_the"), 
        KErrWlanSharedKeyAuthFailed );
#endif
}
void TestWlanWizardConnect::tc_network_failure_30180()
{
#ifdef tc_network_failure_30180_enabled
    const QString ssid("tc_network_failure_30180");
    tc_network_error_codes(
        ssid, 
        CMManagerShim::Adhoc,
        CMManagerShim::WlanSecModeWep, 
        false, 
        "abcde",
        hbTrId("txt_occ_dialog_connection_failed"), 
        30180 );
    
    QCOMPARE( mouseClickPrevious(), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( mouseClickNext(), true );
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, hbTrId("txt_occ_dialog_connection_failed")), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);

    QStringList calledMethods;
    calledMethods 
        << "updateIap" 
        << "activeIap" 
        << "disconnectIap"
        << "connectIap";
    
    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);
    
#endif
}

void TestWlanWizardConnect::tc_iap_creation_fails()
{
#ifdef tc_iap_creation_fails_enabled

    const QString ssid("tc_iap_creation_fails");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(-1);

    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeOpen, 
        false,
        false);

    mView->showWizard();
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyCurrentPageWithInfo(
            WlanWizardPageInternal::PageGenericError,
            hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret") ), true );
    
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    
    QStringList calledMethods;
    calledMethods << "WlanQtUtils" << "createIap";
    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);
    
    QCOMPARE( mouseClickCancel(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyStatus(WizardStatusSignalCancelled, 2), true);
    QTest::qWait(WaitTimeForUi);
#endif 
}

void TestWlanWizardConnect::tc_cancel_key_query()
{
#ifdef tc_cancel_key_query_enabled
      
    const QString ssid("tc_cancel_key_query");
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Adhoc, 
        CMManagerShim::WlanSecModeWep,
        true,
        false);
    mView->showWizard();

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( mouseClickCancel(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyStatus(WizardStatusSignalCancelled), true);    
    
    mWlanQtUtilsContext->emitWlanNetworkClosed(-1, KErrCancel);
    callWlanWizard_startPageOperation();
    
    QStringList calledMethods;
    calledMethods << "WlanQtUtils";
    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);
#endif
}

/*!
 * Helpper test case for testing success case with compinations of provided
 * parameters.
 */
void TestWlanWizardConnect::tc_connect_success(const QString &ssid, int networkMode,
    int securityMode, bool hidden, QString key)
{

    mWlanQtUtilsContext->setCreateWlanIapResult(2);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(2);
    mWlanQtUtilsContext->setSignalIctResult(2, WlanQtUtils::IctPassed);

    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        networkMode,
        securityMode,
        true,
        false);

    WlanQtUtilsAp ap;
    ap.setValue(WlanQtUtilsAp::ConfIdConnectionMode, networkMode);
    ap.setValue(WlanQtUtilsAp::ConfIdSecurityMode, securityMode);
    ap.setValue(WlanQtUtilsAp::ConfIdName, ssid);
    ap.setValue(WlanQtUtilsAp::ConfIdSsid, ssid.toUtf8());
    ap.setValue(WlanQtUtilsAp::ConfIdHidden, false);
    ap.setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, hidden);
    
    // Default values
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true );
    ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1 );
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, QString());
    
    QStringList calledMethods;
    calledMethods << "WlanQtUtils";
    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);

    mView->showWizard();
    if (securityMode == CMManagerShim::WlanSecModeWep || securityMode
        == CMManagerShim::WlanSecModeWpa || securityMode == CMManagerShim::WlanSecModeWpa2) {
        // Key query short pwd
        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);

        QCOMPARE( mouseClickObject("lineEditKey"), true );
        QTest::qWait(WaitTimeForUi);
            
        if (securityMode == CMManagerShim::WlanSecModeWep) {
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1);
            QCOMPARE( insertTextToObject("lineEditKey", "abcde"), true );
        }
        else {
            QCOMPARE( insertTextToObject("lineEditKey", "password"), true );
            ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
        }
        QCOMPARE( mouseClickObject("dialog"), true );

        QTest::qWait(WaitTimeForUi);
        QCOMPARE( mouseClickNext(), true );

        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);

        QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
        QTest::qWait(WaitTimeForUi);
    }
    else {
        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

        QTest::qWait(WaitTimeForUi);
        QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
        QTest::qWait(WaitTimeForUi);
    }
    
    QCOMPARE(verifySummaryPage(
        ssid,
        networkMode,
        hidden, 
        securityMode, 
        true,
        hbTrId("txt_occ_dblist_destination_val_internet")),true);

    calledMethods.clear();
    calledMethods << "createIap" << "activeIap" << "connectIap";
    QCOMPARE( mWlanQtUtilsContext->calledMethods(), calledMethods);
    QCOMPARE( mWlanQtUtilsContext->verifyWlanIapSettings(ap), true);
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( mouseClickFinish(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyStatus(WizardStatusSignalFinished, 2), true);
}

/*!
 * Helpper test case for testing success case with combinations of provided
 * parameters.
 */
void TestWlanWizardConnect::tc_network_error_codes(
    const QString &ssid, 
    int networkMode,
    int securityMode, 
    bool hidden, 
    QString key, 
    const QString &errorString, 
    int errorCode)
{
    mWlanQtUtilsContext->setCreateWlanIapResult(2);
    mWlanQtUtilsContext->setConnectionSuccessed(false);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(2, errorCode);
    mWlanQtUtilsContext->setActiveWlanIapResult(1);
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        networkMode,
        securityMode,
        true,
        false);

    WlanQtUtilsAp ap;
    ap.setValue(WlanQtUtilsAp::ConfIdConnectionMode, networkMode);
    ap.setValue(WlanQtUtilsAp::ConfIdSecurityMode, securityMode);
    ap.setValue(WlanQtUtilsAp::ConfIdName, ssid);
    ap.setValue(WlanQtUtilsAp::ConfIdSsid, ssid.toUtf8());
    ap.setValue(WlanQtUtilsAp::ConfIdHidden, false);
    ap.setValue(WlanQtUtilsAp::ConfIdWlanScanSSID, hidden);

    // Default values
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true );
    ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1 );
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, QString());
    ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, QString());
    
    QStringList calledMethods;
    calledMethods << "WlanQtUtils";
    QCOMPARE(mWlanQtUtilsContext->calledMethods(), calledMethods);

    mView->showWizard();
    if (securityMode == CMManagerShim::WlanSecModeWep || securityMode
        == CMManagerShim::WlanSecModeWpa || securityMode == CMManagerShim::WlanSecModeWpa2) {
        // Key query short pwd
        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);

        QCOMPARE( mouseClickObject("lineEditKey"), true );
        QTest::qWait(WaitTimeForUi);

        if (securityMode == CMManagerShim::WlanSecModeWep) {
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey1, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey2, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey3, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepKey4, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWepDefaultIndex, CMManagerShim::WepKeyIndex1);
            QCOMPARE( insertTextToObject("lineEditKey", "abcde"), true );
        }
        else {
            QCOMPARE( insertTextToObject("lineEditKey", "password"), true );
            ap.setValue(WlanQtUtilsAp::ConfIdWpaPsk, key);
            ap.setValue(WlanQtUtilsAp::ConfIdWpaPskUse, true);
        }
        QCOMPARE( mouseClickObject("dialog"), true );

        QTest::qWait(WaitTimeForUi);
        QCOMPARE( mouseClickNext(), true );

        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);

        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, errorString), true );
        QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);
    }
    else {
        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);

        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, errorString), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
        QTest::qWait(WaitTimeForUi);
    }

    calledMethods.clear();
    calledMethods 
        << "createIap" 
        << "activeIap" 
        << "disconnectIap" 
        << "connectIap";
    QCOMPARE( mWlanQtUtilsContext->calledMethods(), calledMethods);
    QCOMPARE( mWlanQtUtilsContext->verifyWlanIapSettings(ap), true);
    QTest::qWait(WaitTimeForUi);
}
