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
 * Description: Implementation of middleware wrapper class
 *
 *
 */
// System includes
#include <wlanmgmtcommon.h>
#include <e32math.h>

// User includes
#include "cwpsactiverunner.h"

// External function prototypes

//Constants
static const TInt KArrayGranularity = 4;
static const TInt KDefaultPinLength = 10;

// Trace includes
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cwpsactiverunnerTraces.h"
#endif

/*!
 \class CWpsActiveRunner
 \brief CWpsActiveRunner is a wrapper class wlan Mgmt engine interface.
 The wrapper class for wlan management engine calls
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
 * Creates the instance of active runner which encapsulates the wlan mgmt
 * interface middleware calls
 *
 *
 * \param MWpsActiveRunnerCallback[in] call back interface to notify the
 *      completion of the middleware calls
 */
CWpsActiveRunner* CWpsActiveRunner::NewL(MWpsActiveRunnerCallback& aObserver)
{
    OstTraceFunctionEntry0( CWPSACTIVERUNNER_NEWL_ENTRY );
    CWpsActiveRunner* self = new (ELeave) CWpsActiveRunner(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CWPSACTIVERUNNER_NEWL_EXIT );
    return self;
}

/*!
 * Creates the instance of active runner which encapsulates the wlan mgmt
 * interface middleware calls
 *
 *
 * \param MWpsActiveRunnerCallback[in] call back interface to notify the
 *      completion of the middleware calls
 */
CWpsActiveRunner::CWpsActiveRunner(MWpsActiveRunnerCallback& aObserver) :
    CActive(EPriorityStandard),
    iObserver(aObserver),
    isCancelTriggered(false),
    iWLANMgmtClient(NULL),
    iIapParametersArray(NULL)
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_CWPSACTIVERUNNER_ENTRY, this );
    OstTraceFunctionExit1( CWPSACTIVERUNNER_CWPSACTIVERUNNER_EXIT, this );
}

/*!
 * ConstructL, Two phased constructor.
 *
 */
void CWpsActiveRunner::ConstructL()
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_CONSTRUCTL_ENTRY, this );

    CActiveScheduler::Add(this);
    
    iWLANMgmtClient = CWlanMgmtClient::NewL();

    iIapParametersArray = new (ELeave) CArrayFixSeg<
        TWlanProtectedSetupCredentialAttribute> (KArrayGranularity);
    
    OstTraceFunctionExit1( CWPSACTIVERUNNER_CONSTRUCTL_EXIT, this );
}

/*!
 * Destructor
 *
 */
CWpsActiveRunner::~CWpsActiveRunner()
{
    OstTraceFunctionEntry0( DUP1_CWPSACTIVERUNNER_CWPSACTIVERUNNER_ENTRY );
    Cancel();
    delete iWLANMgmtClient;
    delete iIapParametersArray;
    OstTraceFunctionExit0( DUP1_CWPSACTIVERUNNER_CWPSACTIVERUNNER_EXIT );
}

/*!
 * Initiates the call to the middleware
 *
 * \param aSsid[in] The network ssid to which we have to connect
 * \param aPin[in] The pin to be used for WPS negotiating
 */
void CWpsActiveRunner::StartSetup(RBuf8& aSsid, int aPin)
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_STARTSETUP_ENTRY, this );
    TWlanSsid ssid;
    TWlanWpsPin pin;

    isCancelTriggered = false;
    ssid.Copy(aSsid);
    TBuf8<KDefaultPinLength> pinCode;

    // When Pin is zero it means that push-button mode is used.
    if (aPin == 0) {
        pinCode.AppendFill('0', 8);
    } else {
        pinCode.AppendNum(aPin);
    }

    pin.Copy(pinCode);

    iWLANMgmtClient->RunProtectedSetup(iStatus, ssid, pin, *iIapParametersArray);

    SetActive();

    OstTraceFunctionExit1( CWPSACTIVERUNNER_STARTSETUP_EXIT, this );
}

/*!
 * RunL implementation
 */
void CWpsActiveRunner::RunL()
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_RUNL_ENTRY, this );

    TInt completionCode = iStatus.Int();
    QList<TWlanProtectedSetupCredentialAttribute> credentials;

    if (!isCancelTriggered) {

        if (completionCode < KErrNone) {
            //Raise Error
            QT_TRYCATCH_LEAVING(iObserver.WpsActiveRunnerStopped(credentials, completionCode));
        } else {
            TInt len = iIapParametersArray->Length();
            TInt count;
            for (count = 0 ; count < iIapParametersArray->Count() ; count++) {
                TWlanProtectedSetupCredentialAttribute attr =(*iIapParametersArray)[count];
                credentials.append(attr);
            }
            QT_TRYCATCH_LEAVING(iObserver.WpsActiveRunnerStopped(credentials,completionCode));
        }
    }

    OstTraceFunctionExit1( CWPSACTIVERUNNER_RUNL_EXIT, this );
}

/*!
 * Cancels the ongoing call
 */
void CWpsActiveRunner::DoCancel()
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_DOCANCEL_ENTRY, this );
    isCancelTriggered = true;

    iWLANMgmtClient->CancelProtectedSetup();

    OstTraceFunctionExit1( CWPSACTIVERUNNER_DOCANCEL_EXIT, this );
}

/*!
 * Handles the error usecase
 */
TInt CWpsActiveRunner::RunError(TInt aError)
{
    OstTraceFunctionEntry1( CWPSACTIVERUNNER_RUNERROR_ENTRY, this );

    OstTrace1( TRACE_ERROR, CWPSACTIVERUNNER_RUNERROR, "CWpsActiveRunner::RunError;aError=%d", aError );

    QList<TWlanProtectedSetupCredentialAttribute> credentials;
    iWLANMgmtClient->CancelProtectedSetup();
    
    QT_TRYCATCH_LEAVING(iObserver.WpsActiveRunnerStopped(credentials, aError));

    return KErrNone;
}

