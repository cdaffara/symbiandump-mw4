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
 */

#ifndef TESTWLANWIZARDRNDWPS_H
#define TESTWLANWIZARDRNDWPS_H

#include "testwlanwizardcontext.h"

class TestWlanWizardWps : public TestWlanWizardContext
{
Q_OBJECT

private slots:
    void tcConfigureManualOpen();
    void tcPushButtonOpen();
    void tcPushButtonWep1();
    void tcPushButtonWep2();
    void tcPushButtonWep3();
    void tcPushButtonWep4();
    void tcPushButtonWpa();
    void tcPushButtonWpa2();
    void tcPinCode_KErrWlanProtectedSetupOOBInterfaceReadError();
    void tcPinCode_KErrWlanProtectedSetupDecryptionCRCFailure();
    void tcPinCode_KErrWlanProtectedSetup2_4ChannelNotSupported();
    void tcPinCode_KErrWlanProtectedSetup5_0ChannelNotSupported();
    void tcPinCode_KErrWlanProtectedSetupNetworkAuthFailure();
    void tcPinCode_KErrWlanProtectedSetupNoDHCPResponse();
    void tcPinCode_KErrWlanProtectedSetupFailedDHCPConfig();
    void tcPinCode_KErrWlanProtectedSetupIPAddressConflict();
    void tcPinCode_KErrWlanProtectedSetupCouldNotConnectToRegistrar();
    void tcPinCode_KErrWlanProtectedSetupMultiplePBCSessionsDetected();
    void tcPinCode_KErrWlanProtectedSetupRogueActivitySuspected();
    void tcPinCode_KErrWlanProtectedSetupDeviceBusy();
    void tcPinCode_KErrWlanProtectedSetupSetupLocked();
    void tcPinCode_KErrWlanProtectedSetupMessageTimeout();
    void tcPinCode_KErrWlanProtectedSetupRegistrationSessionTimeout();
    void tcPinCode_KErrWlanProtectedSetupDevicePasswordAuthFailure();
    void tcPinCode_KErrWlanProtectedSetupPINMethodNotSupported();
    void tcPinCode_KErrWlanProtectedSetupPBMethodNotSupported();
    void tcPinCode_BackToPushButtonMode();
    /*
    void tcPushButton_KErrNone_back_forward_KErrNone();
    void tcPinCodeMultipleResults();
    void tcPinCodeMultipleResults_back_and_forward();
    */

private: // Helper methods
    void verifyModeSelection(int index = -1);
    
private: // Helper test cases 
    void tcPushButton(
        const QString &ssid,
        int secModeWlan,
        int secModeCmm,
        int operModeWlan,
        int operModeCmm,
        int defaultWepKeyIndexWlan,
        int defaultWepKeyIndexCmm,
        int index = -1);
    
    void tcPinCode_failure(
        int errorCode,
        const QString &errorText);
    

    
};

#endif /* TESTWLANWIZARDRNDWPS_H */
