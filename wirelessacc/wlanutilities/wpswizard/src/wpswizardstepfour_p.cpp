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
* WPS Wizard Page: Step 4 progress bar display
*/

//System Includes

//User Includes
#include "wpswizard.h"
#include "wpswizardstepfour_p.h"
#include "cwpsactiverunner.h"

// Trace includes

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizardstepfour_pTraces.h"
#endif

// External function prototypes

// Local constants


/*!
   \class WpsPageStepFourPrivate
   \brief Private implementation of wps wizard page for step four. 
 */
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for WPS page four
   
   @param [in] WpsPageStepFour* Pointer to the public/Ui class 
 */

WpsPageStepFourPrivate::WpsPageStepFourPrivate(WpsPageStepFour* pageStepFour) :
    mWpsActiveRunner(NULL), 
    q_ptr(pageStepFour)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOURPRIVATE_WPSPAGESTEPFOURPRIVATE_ENTRY, this);
    
    // start the framework.
    CWpsActiveRunner* runner = NULL;
    QT_TRAP_THROWING(runner = CWpsActiveRunner::NewL(*this));
    mWpsActiveRunner.reset(runner);
    
    OstTraceFunctionExit1(WPSPAGESTEPFOURPRIVATE_WPSPAGESTEPFOURPRIVATE_EXIT, this);
}

/*!
   Destructor
 */
WpsPageStepFourPrivate::~WpsPageStepFourPrivate()
{
    OstTraceFunctionEntry1(DUP1_WPSPAGESTEPFOURPRIVATE_WPSPAGESTEPFOURPRIVATE_ENTRY, this);
    OstTraceFunctionExit1(DUP1_WPSPAGESTEPFOURPRIVATE_WPSPAGESTEPFOURPRIVATE_EXIT, this);
}

/*!
   This method will initiate the WPS setup functionality.
   
   @param [in] ssid name of the network which supports wps
   @param [in] pin pin number to be used for WPS negotiating.
 */
void WpsPageStepFourPrivate::StartSetup(QString ssid, int pin)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOURPRIVATE_STARTSETUP_ENTRY, this);
    
    TPtrC ptrName(reinterpret_cast<const TText*> (ssid.constData()));

    RBuf8 ssidName;
    QT_TRAP_THROWING(ssidName.CreateL(ptrName.Length()));
    ssidName.Copy(ptrName);

    mWpsActiveRunner->StartSetup(ssidName, pin);
    ssidName.Close();
    
    OstTraceFunctionExit1(WPSPAGESTEPFOURPRIVATE_STARTSETUP_EXIT, this);
}

/*!
   CallBack informing the completion of the WPS setup.
   
   @param [in] aCredentials An array of credentials from the remote AP
   @param [in] aError integer variable indicating result of the operation
 */
void WpsPageStepFourPrivate::WpsActiveRunnerStopped(
    QList<TWlanProtectedSetupCredentialAttribute>& aCredentials, 
    TInt aError)
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOURPRIVATE_WPSACTIVERUNNERSTOPPED_ENTRY, this);

    // mWpsErrorCode = aError;
    if (aError == KErrNone) {
        q_ptr->setCredentials(aCredentials, aCredentials.count());
    } else if (aError == KErrCancel) {
        // operation was cancelled 
    } else {
        OstTrace1( 
            TRACE_ERROR, 
            WPSPAGESTEPFOURPRIVATE_WPSACTIVERUNNERSTOPPED_ERROR, 
            "WpsPageStepFourPrivate::WpsActiveRunnerStopped;Error=%d", 
            aError);
        
        q_ptr->handleError(aError);
    }
    
    OstTraceFunctionExit1(WPSPAGESTEPFOURPRIVATE_WPSACTIVERUNNERSTOPPED_EXIT, this);
}

/*!
   To cancel the ongoing WPS setup implementation.
 */
void WpsPageStepFourPrivate::Cancel()
{
    OstTraceFunctionEntry1(WPSPAGESTEPFOURPRIVATE_CANCEL_ENTRY, this);
    
    mWpsActiveRunner->DoCancel();
    
    OstTraceFunctionExit1(WPSPAGESTEPFOURPRIVATE_CANCEL_EXIT, this);
}

