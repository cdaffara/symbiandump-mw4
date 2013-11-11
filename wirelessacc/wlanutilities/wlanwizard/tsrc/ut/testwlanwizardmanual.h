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

#ifndef TESTWLANWIZARDRNMANUAL_H_
#define TESTWLANWIZARDRNMANUAL_H_

#include "testwlanwizardcontext.h"

class TestWlanWizardManual : public TestWlanWizardContext
{
Q_OBJECT

private slots:
    // Test cases
    void tcStartWizard();
    void tc_open_scan_results_received_at_scan_page();
    void tc_attempt_to_input_too_long_ssid();
    void tc_scan_succesful_press_cancel_at_summary();
    void tc_press_previous_at_scanning_page();
    void tc_scan_succesful_go_to_summary_and_finish();
    void tc_wep_pwd_too_short_error_label();
    void tc_press_previous_at_key_query();
    void tc_manual_selection_with_press_previous();
    void tc_illegal_characters_in_wep_key();
    void tc_too_short_wpa_key();
    void tc_wpa_psk_success();
    void tc_wpa_no_psk_go_to_eap_start();
    void tc_wpa2_psk_success();
    void tc_wpa2_no_psk_go_to_eap_start();
    void tc_network_closed_generic_error();
    void tc_adhoc_network_no_psk();
    void tc_secmode_802_1x_go_to_eap_start();
    void tc_sec_mode_wapi();
    void tc_multiple_open_scan_results_single_match();
    void tc_iap_creation_fail();
    void tc_scan_ap_status_not_ok();
    void tc_scan_direct_status_not_ok();
    void tc_ictresult_enumerated_hotspot();
    void tc_ictresult_enumerated_cancel();
    void tc_multiple_ssids_with_same_name_2_netmode_options();
    void tc_multiple_ssids_with_same_name_press_previous();
    void tc_multiple_ssids_with_same_name_all_sec_modes();
    void tc_multiple_ssids_with_same_name_orientation_switch();
    void tc_multiple_ssids_with_same_name_wps_and_nonwps();
    void tc_multiple_ssids_with_same_name_two_wps_aps();
    
};

#endif /* TESTWLANWIZARDRNMANUAL_H_ */
