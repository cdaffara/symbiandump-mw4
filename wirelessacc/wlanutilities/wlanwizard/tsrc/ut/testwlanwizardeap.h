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

#ifndef TESTWLANWIZARDRNDEAP_H
#define TESTWLANWIZARDRNDEAP_H

#include "testwlanwizardcontext.h"

class TestWlanWizardEap : public TestWlanWizardContext
{
Q_OBJECT

private slots:
    void tc_eap_outer_page_orientation_change();
    void tc_eap_aka();
    void tc_eap_sim();
    void tc_eap_leap();
    void tc_eap_peap_gtc();
    void tc_eap_peap_mschapv2();
    void tc_eap_ttls_mschapv2();
    void tc_eap_ttls_pap();
    void tc_eap_ttls_gtc();
    void tc_eap_aka_connect_KErrWlanUserRejected();
    void tc_eap_aka_connect_KErrWlanUserCertificateExpired();
    void tc_eap_aka_connect_KErrWlanServerCertificateExpired();
    void tc_eap_aka_connect_KErrWlanCerficateVerifyFailed();
    void tc_eap_aka_connect_KErrWlanNoCipherSuite();
    void tc_eap_aka_connect_KErrWlanSimNotInstalled();
    void tc_eap_aka_connect_KErrWlanEapFastPacStoreCorrupted();
    void tc_eap_aka_connect_KErrWlanEapSimFailed();
    void tc_eap_aka_connect_KErrWlanNotSubscribed();
    void tc_eap_aka_connect_KErrCancel();
    void tc_eap_aka_connect_KErrWlanEapTlsFailed();
    void tc_eap_aka_connect_KErrWlanEapPeapFailed();
    void tc_eap_aka_connect_KErrWlanEapAkaFailed();
    void tc_eap_aka_connect_KErrWlanEapTtlsFailed();
    void tc_eap_aka_connect_KErrWlanLeapFailed();
    void tc_eap_aka_connect_KErrWlanNoUserCertificate();
    void tc_eap_aka_connect_KErrWlanEapFastTunnelCompromiseError();
    void tc_eap_aka_connect_KErrWlanEapFastUnexpextedTlvExhanged();
    void tc_eap_aka_connect_KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled();
    void tc_eap_aka_connect_KErrWlanEapFastNoMatchingPacForAid();
    void tc_eap_aka_connect_KErrWlanEapFastAuthFailed();
    void tc_eap_aka_connect_KErrWlanAccessBarred();
    void tc_eap_aka_connect_KErrWlanPasswordExpired();
    void tc_eap_aka_connect_KErrWlanNoDialinPermissions();
    void tc_eap_aka_connect_KErrWlanAccountDisabled();
    void tc_eap_aka_connect_KErrWlanRestrictedLogonHours();
    void tc_eap_ttls_mschapv2_connect_KErrWlanEapGtcFailed();
    void tc_eap_ttls_mschapv2_connect_KErrWlanEapMsChapv2();
    void tc_eap_ttls_mschapv2_connect_KErrWlanNotSubscribed();
    void tc_eap_ttls_gtc_back_and_forward();
    void tc_eap_aka_set_outer_type_fails();
    void tc_eap_aka_save_outer_configuration_fails();
    void tc_eap_aka_set_configuration_reference_fails();
    void tc_eap_peap_gtc_save_inner_configuration_fails();
    void tc_eap_peap_identity_validators();
    void tc_eap_leap_user_password_validators();
    void tc_eap_tls_no_user_certificate();
    void tc_eap_tls_one_user_certificate_with_ca_selection();
    void tc_eap_tls_two_user_certificates();
    void tc_eap_tls_back_and_forward_certs_with_cert_update();
    void tc_eap_sim_finish();
    void tc_eap_sim_cancel();
    void tc_eap_fast_confirmed();
    void tc_eap_fast_new_store();
    void tc_eap_fast_new_store_save_fails();
    void tc_eap_fast_new_store_and_validators();
    void tc_eap_fast_new_store_back_and_forwards();
    void tc_eap_fast_new_store_read_config_fail();
    void tc_eap_fast_confirm_store();
    void tc_eap_fast_confirm_store_and_validators();
    void tc_eap_fast_confirm_store_back_and_forwards();
         
private: // Helpper methods
    void verify_outertype_select_first(
        TestWlanWizardContext::ButtonStatus nextButtonStatus);
    void verify_outertype(
        TestWlanWizardContext::ButtonStatus nextButtonStatus);
    
    void verify_innertype_select_first(
        int outerType,
        TestWlanWizardContext::ButtonStatus nextButtonStatus);
    
    void verify_innertype(
        int outerType,
        TestWlanWizardContext::ButtonStatus nextButtonStatus);
    
    void verify_username_password_click_next(
        int type, 
        const QString &username, 
        const QString &password);
    
    void verify_username_password(int type);
    
    void verify_ca_cert_page_select_and_click_next(bool automatic);
    void verify_ca_cert_page();
    void verify_ca_cert_page( const QString &text, int index);
    
    void verify_user_cert_page_select_and_click_next(
        ButtonStatus nextButtonStatus,
        int index);
    
    void verify_user_cert_page(ButtonStatus nextButtonStatus);
    void verify_user_cert_page(
        ButtonStatus nextButtonStatus, 
        const QString &text,
        int index);
    
    void verify_identity_modify_and_click_next(
        const QString &outerType,        
        bool usernameAutomatic,
        const QString &username,
        bool realmAutomatic,
        const QString &realm);
    void verify_identity_and_click_next(const QString &outerType);
    void verify_identity_page(const QString &outerType);
    
    void verify_new_pac_store_password_page(ButtonStatus nextButtonStatus);
    void verify_confirm_pac_store_password_page(ButtonStatus nextButtonStatus);
    
private: // Helpper test cases
    void tc_eap_aka_sim(int type);
    
    void tc_eap_peap_ttls(
        int outerType, 
        int innerType,
        int errorCode = 0,
        const QString &errorString = QString(),
        bool saveInnerConfReturn = true);
    
    void tc_eap_aka_connect_error(
        int errorCode, 
        const QString &errorString);

    void tc_eap_fast(
        int pacStoreState, 
        bool savePacStoreReturn,
        bool readPacStoreReturn = true);
    
    
};

#endif /* TESTWLANWIZARDRNDEAP_H */
