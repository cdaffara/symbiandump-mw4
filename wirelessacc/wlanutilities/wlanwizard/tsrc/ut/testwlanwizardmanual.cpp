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
* WLAN Wizard manual IAP addition cases.
*/

// System includes

#include <QGraphicsWidget>
#include <QObjectList>
#include <QtCore>
#include <QTest>
#include <QDebug>
#include <QList>

#include <HbApplication>
#include <HbDocumentLoader>
#include <HbStackedWidget>
#include <HbRadioButtonList>
#include <HbAction>
#include <HbLineEdit>
#include <HbLabel>

#include <cmmanagerdefines_shim.h>

// User includes

#include "testwlanwizardmanual.h"
#include "testwlanwizardmanual_conf.h"
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


void TestWlanWizardManual::tcStartWizard()
{
    qDebug("Start TestWlanWizardManual");
}

/*!
 * 
 */
void TestWlanWizardManual::tc_open_scan_results_received_at_scan_page()
{
#ifdef tc_open_scan_results_received_at_scan_page_enabled
    
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    
    mWlanQtUtilsContext->setImmediateApScanResult(false);
    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    mWlanQtUtilsContext->emitScanApReady();
    mWlanQtUtilsContext->setImmediateApScanResult(true);
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    
#endif
}

/*!
 * 
 */ 
void TestWlanWizardManual::tc_attempt_to_input_too_long_ssid()
{
#ifdef tc_attempt_to_input_too_long_ssid_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanDirectResult("12345678901234567890123456789012", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "1234567890123456789012345678901234567890"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "12345678901234567890123456789012"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );  
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
#endif
}

/*!
 *  
 */
void TestWlanWizardManual::tc_scan_succesful_press_cancel_at_summary()
{
#ifdef tc_scan_succesful_press_cancel_at_summary_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );  
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    // Currently against the spec in http://wikis.in.nokia.com/pub/UXD/101ConnectionManager/occ_logical_flows_and_wireframes.pdf
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );

    QCOMPARE( mouseClickCancel(), true );

    QTest::qWait(WaitTimeForUi);

#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_press_previous_at_scanning_page()
{
#ifdef tc_press_previous_at_scanning_page_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );  

    // Click previous button and next again
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    // Currently against the spec in http://wikis.in.nokia.com/pub/UXD/101ConnectionManager/occ_logical_flows_and_wireframes.pdf
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );

#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_scan_succesful_go_to_summary_and_finish()
{
#ifdef tc_scan_succesful_go_to_summary_and_finish_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true ); 
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    // Currently against the spec in http://wikis.in.nokia.com/pub/UXD/101ConnectionManager/occ_logical_flows_and_wireframes.pdf
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    QCOMPARE( mouseClickFinish(), true );
#endif
}

/*!
 * Stop verifying buttons for views that have been already verified.
 */
void TestWlanWizardManual::tc_wep_pwd_too_short_error_label()
{
#ifdef tc_wep_pwd_too_short_error_label_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    
    // Key query short pwd
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "password"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyDialogText("labelErrorNote", hbTrId("txt_occ_dialog_key_is_of_incorrect_length_please")), true );
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);

    // Key query success
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyDialogText("labelErrorNote", ""), true );
    QCOMPARE( insertTextToObject("lineEditKey", "12345"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_press_previous_at_key_query()
{
#ifdef tc_press_previous_at_key_query_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_manual_selection_with_press_previous()
{
#ifdef tc_manual_selection_with_press_previous_enabled

    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkMode, "huuhaa3421"), true );
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkSecurity, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true ); 
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkMode, "huuhaa3421"), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkSecurity, "huuhaa3421"), true );
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );

    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkMode, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true ); 
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkSecurity, "huuhaa3421"), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true ); 
    QCOMPARE( verifySelectedRadioButton("list"), 0 );
    
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
#endif    
}

/*!
 * 
 */
void TestWlanWizardManual::tc_illegal_characters_in_wep_key()
{
#ifdef tc_illegal_characters_in_wep_key_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
    
    QCOMPARE( insertTextToObject("lineEditKey", "passworddd"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyDialogText("labelErrorNote", hbTrId("txt_occ_dialog_illegal_characters_in_key_please_c")), true );
    QTest::qWait(WaitTimeForUi);

#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_too_short_wpa_key()
{
#ifdef tc_too_short_wpa_key_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QTest::qWait(2000);
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
    
    QCOMPARE( insertTextToObject("lineEditKey", "1234"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyDialogText("labelErrorNote", hbTrId("txt_occ_dialog_preshared_key_too_short_at_least")), true );
    QTest::qWait(WaitTimeForUi);
#endif
}

/*!
 * wpa psk
 */
void TestWlanWizardManual::tc_wpa_psk_success()
{
#ifdef tc_wpa_psk_success_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QTest::qWait(2000);
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );

    QCOMPARE( insertTextToObject("lineEditKey", "1234567890"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * no wpa psk
 */
void TestWlanWizardManual::tc_wpa_no_psk_go_to_eap_start()
{
#ifdef tc_wpa_no_psk_go_to_eap_start_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageEapStart), true );
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageEapStart), true );    
#endif
}

/*!
 * wpa2 psk
 */
void TestWlanWizardManual::tc_wpa2_psk_success()
{
#ifdef tc_wpa2_psk_success_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa2, true, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QTest::qWait(2000);
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );

    QCOMPARE( insertTextToObject("lineEditKey", "1234567890"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * wpa2 no psk
 */
void TestWlanWizardManual::tc_wpa2_no_psk_go_to_eap_start()
{
#ifdef tc_wpa2_no_psk_go_to_eap_start_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa2, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageEapStart), true );
#endif
}

/*!
 * Wlan network closed -> generic error
 */
void TestWlanWizardManual::tc_network_closed_generic_error()
{
#ifdef tc_network_closed_generic_error_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(100, 1);
    mWlanQtUtilsContext->setConnectionSuccessed(false);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, hbTrId("txt_occ_dialog_connection_failed")), true );
    
#endif
}

/*!
 * adhoc no psk
 */
void TestWlanWizardManual::tc_adhoc_network_no_psk()
{
#ifdef tc_adhoc_network_no_psk_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Adhoc, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    
#endif
}

/*!
 * 802_1x
 */
void TestWlanWizardManual::tc_secmode_802_1x_go_to_eap_start()
{
#ifdef tc_secmode_802_1x_go_to_eap_start_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageEapStart), true );
#endif
}

/*!
 * Wapi
 */
void TestWlanWizardManual::tc_sec_mode_wapi()
{
#ifdef tc_sec_mode_wapi_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWapi, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * Multiple AP scan match results
 */
void TestWlanWizardManual::tc_multiple_open_scan_results_single_match()
{
#ifdef tc_multiple_open_scan_results_single_match_enabled
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 400);
    mApOpenList->Add("foobar", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 300);
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 200);

    mApList->Add("foobar", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 300);
    
    mWlanQtUtilsContext->setScanWlanApsResult(mApOpenList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("foobar", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "foobar"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "foobar"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "foobar"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_iap_creation_fail()
{
#ifdef tc_iap_creation_fail_enabled
    
    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(-1);
    
    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    
    QStringList buttons;
    buttons << hbTrId("txt_occ_dblist_val_infrastructure_public") 
            << hbTrId("txt_occ_list_infrastructure_hidden") 
            << hbTrId("txt_occ_list_adhoc_1");
    QCOMPARE( verifyRadioButtons("list"), buttons );

    QCOMPARE( selectRadioButton( "list", 2 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QCOMPARE( verifySelectedRadioButton("list"), 2 );
    QCOMPARE( selectRadioButton( "list", 2 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageNetworkSecurity, "huuhaa3421"), true );
    
    buttons.clear();
    buttons << hbTrId("txt_occ_list_open") 
            << hbTrId("txt_occ_list_wep_1");
    QCOMPARE( verifyRadioButtons("list"), buttons );
    
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret")), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_scan_ap_status_not_ok()
{
#ifdef tc_scan_ap_status_not_ok_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mWlanQtUtilsContext->setScanApStatus(WlanQtUtils::ScanStatusError);
    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
#endif
}
    
/*!
 * 
 */
void TestWlanWizardManual::tc_scan_direct_status_not_ok()
{
#ifdef tc_scan_direct_status_not_ok_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false);

    mWlanQtUtilsContext->setScanWlanApsResult(mApList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    //mWlanQtUtilsContext->setImmediateApScanResult(false);
    mWlanQtUtilsContext->setScanDirectStatus(WlanQtUtils::ScanStatusError);
    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
#endif
}

/*!
 * Ict Result enumerator used. Hotspot.
 */
void TestWlanWizardManual::tc_ictresult_enumerated_hotspot()
{
#ifdef tc_ictresult_enumerated_hotspot_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 400);
    
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctHotspotPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(1000);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(1000);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(1000);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QTest::qWait(5000);
#endif
}

/*!
 * Ict Result enumerator used. Ict canceled.
 */
void TestWlanWizardManual::tc_ictresult_enumerated_cancel()
{
#ifdef tc_ictresult_enumerated_cancel_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 400);
    
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctCancelled);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(1000);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(1000);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(1000);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
#endif
}

/*!
 * Ict fails in this case
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_2_netmode_options()
{
#ifdef tc_multiple_ssids_with_same_name_2_netmode_options_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    // Add duplicates
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    
    // Add all five kinds of security modes.
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false, 500);
    mApList->Add("huuhaa3421", CMManagerShim::Adhoc, CMManagerShim::WlanSecModeOpen, false, false, 400);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, false, false, 300);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false, 200);
    
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctFailed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    
    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QStringList buttons;
    buttons << hbTrId("txt_occ_list_infrastructure_hidden") 
            << hbTrId("txt_occ_list_adhoc_1");
    QCOMPARE( verifyRadioButtons("list"), buttons );
    
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( selectRadioButton( "list", 1 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_press_previous()
{
#ifdef tc_multiple_ssids_with_same_name_press_previous_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    // Add duplicates
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);

    // Add all five kinds of security modes.
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false, 500);
    mApList->Add("huuhaa3421", CMManagerShim::Adhoc, CMManagerShim::WlanSecModeOpen, false, false, 400);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, true, false, 300);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false, 200);

    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkSecurity), true );
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkSecurity), true );
    QCOMPARE( selectRadioButton( "list", 1 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_all_sec_modes()
{
#ifdef tc_multiple_ssids_with_same_name_all_sec_modes_enabled
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    // Add duplicates
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);

    // Add all five kinds of security modes.
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false, 500);
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa2, true, false, 500);
    mApOpenList->Add("huuhaa3421", CMManagerShim::Adhoc, CMManagerShim::WlanSecModeOpen, false, false, 400);
    mApOpenList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false, 200);
    mApOpenList->Add("dippadappa", CMManagerShim::Infra, CMManagerShim::WlanSecModeOpen, false, false, 200);
    
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    // Add duplicates
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);

    // Add all five kinds of security modes.
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false, 500);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa2, true, false, 500);
    mApList->Add("huuhaa3421", CMManagerShim::Adhoc, CMManagerShim::WlanSecModeOpen, false, false, 400);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, true, false, 300);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false, 200);

    mWlanQtUtilsContext->setScanWlanApsResult(mApOpenList->List());
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QStringList buttons;
    buttons << hbTrId("txt_occ_dblist_val_infrastructure_public") 
            << hbTrId("txt_occ_list_infrastructure_hidden") 
            << hbTrId("txt_occ_list_adhoc_1");
    QCOMPARE( verifyRadioButtons("list"), buttons );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkSecurity), true );
    QCOMPARE( mouseClickPrevious(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QCOMPARE( mouseClickNext(), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkSecurity), true );
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
#endif
}
   
/*!
 * 
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_orientation_switch()
{
#ifdef tc_multiple_ssids_with_same_name_orientation_switch_enabled
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    // Add duplicates
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);

    // Add all five kinds of security modes.
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, true, false, 500);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, true, false, 300);
    mApList->Add("huuhaa3421", CMManagerShim::Infra, CMManagerShim::WlanSecMode802_1x, false, false, 200);

    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa3421", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );
    
    mMainWindow->setOrientation(Qt::Horizontal, false);
    QTest::qWait(WaitTimeForUi);
    mMainWindow->setOrientation(Qt::Vertical, false);
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa3421"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa3421"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkSecurity), true );
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageKeyQuery, "huuhaa3421"), true );
#endif
}

/*!
 * 
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_wps_and_nonwps()
{
#ifdef tc_multiple_ssids_with_same_name_wps_and_nonwps_enabled
    mApList->Add("huuhaa", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, false, 100);
    
    // Add all five kinds of security modes.
    mApList->Add("huuhaa", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, true, true, 300);
    
    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );

    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageNetworkMode), true );
    QStringList buttons;
    buttons << hbTrId("txt_occ_list_infrastructure_hidden") 
            << hbTrId("txt_occ_list_wifi_protected_setup");
    QCOMPARE( verifyRadioButtons("list"), buttons );
    QCOMPARE( selectRadioButton( "list", 1 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageWpsStart), true );
#endif 
}

/*!
 * 
 */
void TestWlanWizardManual::tc_multiple_ssids_with_same_name_two_wps_aps()
{
#ifdef tc_multiple_ssids_with_same_name_two_wps_aps_enabled
    mApList->Add("huuhaa", CMManagerShim::Infra, CMManagerShim::WlanSecModeWpa, false, true, 100);

    // Add all five kinds of security modes.
    mApList->Add("huuhaa", CMManagerShim::Infra, CMManagerShim::WlanSecModeWep, true, true, 300);

    mWlanQtUtilsContext->setScanWlanDirectResult("huuhaa", mApList->List());
    mWlanQtUtilsContext->setCreateWlanIapResult(100);
    mWlanQtUtilsContext->setSignalIctResult(100, WlanQtUtils::IctPassed);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(100);
    mWlanQtUtilsContext->setConnectionSuccessed(true);

    mView->showWizard();

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSsid), true );

    QCOMPARE( mouseClickObject("lineEditKey"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditKey", "huuhaa"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickObject("dialog"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( mouseClickNext(), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageScanning, "huuhaa"), true );
    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageWpsStart), true );
#endif
}
