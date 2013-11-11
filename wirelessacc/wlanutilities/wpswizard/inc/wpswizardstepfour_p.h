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

#ifndef WPSWIZARDSTEPFOUR__P_H_
#define WPSWIZARDSTEPFOUR__P_H_

// System includes

// User includes
#include "wpswizardstepfour.h"

// Forward declarations
class MWpsActiveRunnerCallback;
class CWpsActiveRunner;


// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsPageStepFourPrivate : public MWpsActiveRunnerCallback
    {
public:
    
    explicit WpsPageStepFourPrivate(WpsPageStepFour* pageStepFour);
       ~WpsPageStepFourPrivate();
 
    /*!
     * From mwpsactiverunnercallback.h
     * This method is called from the active runner class to notify the end
     * of operation
     */
    void WpsActiveRunnerStopped(
            QList<TWlanProtectedSetupCredentialAttribute>& aCredentials,
            TInt aError);
    
    void StartSetup(QString ssid,int pin);
    
    void Cancel();
       

protected:

private: // data

    //! C-Class implementation of the WPS setup functionality.
    QScopedPointer<CWpsActiveRunner> mWpsActiveRunner;
    //! Pointer to the WpsPageStepFour object
    WpsPageStepFour* q_ptr;
    
    friend class TestWlanWizardContext;
    };

#endif /* WPSWIZARDSTEPFOUR__P_H_ */
