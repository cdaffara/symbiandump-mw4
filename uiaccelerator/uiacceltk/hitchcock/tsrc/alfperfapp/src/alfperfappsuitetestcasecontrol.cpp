/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


// INCLUDES
#include <aknutils.h>

#include <alf/alfenv.h>
#include <alf/alfevent.h>

#include "AlfPerfAppSuiteTestCaseControl.h"


// ============================= MEMBER FUNCTIONS ==============================

CAlfPerfAppSuiteTestCaseControl::CAlfPerfAppSuiteTestCaseControl()
    {
    }
        
CAlfPerfAppSuiteTestCaseControl::~CAlfPerfAppSuiteTestCaseControl()
    {
    CompleteNow( KErrCancel );
    }
        
void CAlfPerfAppSuiteTestCaseControl::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    iCaseId = aCaseId;
    iVisibleArea = aVisibleArea;
    CAlfControl::ConstructL( aEnv );
    TFileName privatePath;
    CEikonEnv::Static()->FsSession().PrivatePath( privatePath );
    ::CompleteWithAppPath( privatePath );
    aEnv.TextureManager().SetImagePathL( privatePath );
    }
        
void CAlfPerfAppSuiteTestCaseControl::StartExecuteL( TRequestStatus& aStatus )
    {
    iStatus = &aStatus;
    *iStatus = KRequestPending;
    
    TRAPD( err, DoStartExecuteL() );
    if ( err != KErrNone )
        {
        iStatus = NULL;
        User::Leave( err );
        }
    }

void CAlfPerfAppSuiteTestCaseControl::CancelExecution()
    {
    CompleteNow( KErrCancel );
    }

void CAlfPerfAppSuiteTestCaseControl::DoStartExecuteL()
    {
    CompleteNow( KErrNone );
    }

void CAlfPerfAppSuiteTestCaseControl::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    }

TBool CAlfPerfAppSuiteTestCaseControl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppSuiteCmdCompleteNow )
        {
        CompleteNow( KErrNone );
        return ETrue;
        }
        
    return CAlfControl::OfferEventL( aEvent );
    }
        
void CAlfPerfAppSuiteTestCaseControl::CompleteAfterL( TInt aDuration )
    {
    // In order to use this service, base class StartExecuteL must
    // have been called.
    __ASSERT_ALWAYS( iStatus, User::Invariant() );
    
    TAlfCustomEventCommand command( KAlfPerfAppSuiteCmdCompleteNow, this );
    User::LeaveIfError( Env().Send( command, aDuration ) );
    }
        
void CAlfPerfAppSuiteTestCaseControl::CompleteNow( TInt aErrorCode )
    {
    if ( iStatus )
        {
        User::RequestComplete( iStatus, aErrorCode );
        iStatus = NULL;
        }
    }

inline TInt CAlfPerfAppSuiteTestCaseControl::CaseId() const
    {
    return iCaseId;
    }

TBool CAlfPerfAppSuiteTestCaseControl::IsExecutionOngoing() const
    {
    return ( iStatus != NULL );
    }

// end of file
