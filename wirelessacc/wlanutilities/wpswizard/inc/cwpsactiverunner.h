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
* Description: WPS Wizard: Wrapper for middleware calls 
*
*/

#ifndef CWPSACTIVERUNNER_H_
#define CWPSACTIVERUNNER_H_

// System includes
#include <e32base.h>
#include <wlanmgmtclient.h>

// User includes
#include "mwpsactiverunnercallback.h"

// Forward declarations
// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */
// Class declaration
class CWpsActiveRunner : public CActive
{
public:
    static CWpsActiveRunner* NewL(MWpsActiveRunnerCallback& aObserver);
    virtual ~CWpsActiveRunner();

public:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    void StartSetup(RBuf8& aSsid, int aPin);
	
protected:

private:
    explicit CWpsActiveRunner(MWpsActiveRunnerCallback& aObserver);
    void ConstructL();

private:    

    //! observer for notification of WPS setup completion
    MWpsActiveRunnerCallback& iObserver;
    //! To know if cancel was called before the completion
    TBool isCancelTriggered;
    //! WLAN management client object
    CWlanMgmtClient* iWLANMgmtClient;
    //! Array to hold the results of the WPS setup
    CArrayFixSeg<TWlanProtectedSetupCredentialAttribute>* iIapParametersArray;
    
    friend class TestWlanWizardContext;
};

#endif /* CWPSACTIVERUNNER_H_ */
