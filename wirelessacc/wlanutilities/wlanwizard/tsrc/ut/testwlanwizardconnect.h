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

#ifndef TESTWLANWIZARDRNDCONNECT_H_
#define TESTWLANWIZARDRNDCONNECT_H_

#include "testwlanwizardcontext.h"

class TestWlanWizardConnect : public TestWlanWizardContext
{
Q_OBJECT

private slots:
    void tc_connect_to_open_success();
    void tc_connect_to_open_success_adhoc();
    void tc_connect_to_wep_success();
    void tc_connect_to_wpa_psk_success();
    void tc_connect_to_wpa2_psk_success();
    void tc_connect_to_open_success_cancel();
    void tc_network_failure_OpenAuthFailed();
    void tc_network_failure_SharedKeyAuthRequired();
    void tc_network_failure_WpaAuthRequired();
    void tc_network_failure_WpaAuthFailed();
    void tc_network_failure_802dot1xAuthFailed();
    void tc_network_failure_IllegalEncryptionKeys();
    void tc_network_failure_PskModeRequired();
    void tc_network_failure_EapModeRequired();
    void tc_network_failure_IllefalWpaPskKey();
    void tc_network_failure_SharedKeyAuthFailed();
    void tc_network_failure_ErrNone();
    void tc_network_failure_30180();
    void tc_iap_creation_fails();
    void tc_cancel_key_query();
    
private:
    void tc_connect_success(
        const QString &ssid,
        int networkMode,
        int securityMode,
        bool hidden,
        QString key);
    
    void tc_network_error_codes(
        const QString &ssid,
        int networkMode,
        int securityMode,
        bool hidden,
        QString key,
        const QString &errorString,
        int errorCode);
};

#endif /* TESTWLANWIZARDRNDCONNECT_H_ */
