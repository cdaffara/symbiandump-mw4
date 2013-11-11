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
#include <AknUtils.h>

#include <alf/AlfEnv.h>
#include <alf/AlfDisplay.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>

#include "AlfPerfApp.hrh"
#include "AlfPerfAppAppUi.h"
#include "AlfPerfAppSuiteTestCase.h"
#include "AlfPerfAppSuiteTestCaseScroll.h"

// ============================= MEMBER FUNCTIONS ==============================

CAlfPerfAppSuiteTestCase* CAlfPerfAppSuiteTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
	{
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }
    
	CAlfPerfAppSuiteTestCase* self = new (ELeave) CAlfPerfAppSuiteTestCase( aCaseId, aSequenceIndex );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	    
CAlfPerfAppSuiteTestCase::~CAlfPerfAppSuiteTestCase()
	{
    if ( iEnv && iControl )
        {
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppSuiteControlGroup );
        }	
	}
	    
CAlfPerfAppSuiteTestCase::CAlfPerfAppSuiteTestCase( TInt aCaseId, TInt aSequenceIndex )
	:CAlfPerfAppTestCase(aSequenceIndex), iCaseId( aCaseId )
	{
	
	}
		
void CAlfPerfAppSuiteTestCase::ConstructL()
	{
	
	}
		
void CAlfPerfAppSuiteTestCase::SetupL(
		CAlfEnv& aEnv, 
	    const TRect& /*aVisibleArea*/, 
	    TRequestStatus& aStatus )
	{
    iEnv = &aEnv;
    iStatus = &aStatus;
    
    // Set screen orientation
    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();
    switch ( iCaseId )
        {    
        case EAlfPerfAppSuiteCaseScrollContinuousPortrait:
        case EAlfPerfAppSuiteCaseScrollIncrementalPortrait:
        	appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationPortrait );
            break;
            
        case EAlfPerfAppSuiteCaseScrollContinuousLandscape:
        case EAlfPerfAppSuiteCaseScrollIncrementalLandscape:
        	appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );
            break;
        
        default:
            User::Leave( KErrNotSupported );
            break;
        }       
        
    
    // Create new fullscreen display
    TRect rect;
	AknLayoutUtils::LayoutMetricsRect( 
	    AknLayoutUtils::EApplicationWindow, 
	    rect );    
    
	iEnv->SetFullScreenDrawing( ETrue );
	CAlfDisplay& display = 
	    iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayFullScreen );
    
    display.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    // Create control group
    CAlfControlGroup& group = 
        iEnv->NewControlGroupL( KAlfPerfAppSuiteControlGroup );

    // Create testcase control
    CAlfPerfAppSuiteTestCaseControl* control = NULL;
    switch ( iCaseId )
        {
        case EAlfPerfAppSuiteCaseScrollContinuousPortrait:
        case EAlfPerfAppSuiteCaseScrollIncrementalPortrait:
        case EAlfPerfAppSuiteCaseScrollContinuousLandscape:
        case EAlfPerfAppSuiteCaseScrollIncrementalLandscape:
            control = new (ELeave) CAlfPerfAppSuiteTestCaseScroll(this);
            break;
        
        default:
            User::Leave( KErrNotSupported );
            break;
        }
        
    CleanupStack::PushL( control );
    control->ConstructL( *iEnv, iCaseId, rect );
    group.AppendL( control ); // ownership passed to control group.
    iControl = control;
    CleanupStack::Pop( control );    

    // Ready for showing
    display.Roster().ShowL( group );
    
    // ImagesLoaded() send the status completion.
	}

void CAlfPerfAppSuiteTestCase::ExecuteL( TRequestStatus& aStatus )
	{
    // SetupL must have been called first.
    __ASSERT_ALWAYS( iEnv, User::Invariant() );
    __ASSERT_ALWAYS( iControl, User::Invariant() );
    
    iControl->StartExecuteL( aStatus );
	}

void CAlfPerfAppSuiteTestCase::TearDown()
	{
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;	
    
    // Return orientation back to normal
    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();
    TRAP_IGNORE(appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationUnspecified ));    
	}

void CAlfPerfAppSuiteTestCase::ImagesLoaded( TInt aErrorCode )
    {
    if( iStatus )
        {
        CAlfPerfAppTestCase::CompleteNow( *iStatus, aErrorCode );
        iStatus = 0;
        }
    }

void CAlfPerfAppSuiteTestCase::HandleVisibleAreaChange( const TRect& aRect )
	{
    if ( iEnv && iControl )
        {
        if ( iEnv->DisplayCount() > 0 )
            {
            iEnv->PrimaryDisplay().SetVisibleArea( aRect );
            }
        iControl->SetVisibleArea( aRect );
        }	
	}

TInt CAlfPerfAppSuiteTestCase::CaseID()
    {
        return iCaseId;
    }

TBool CAlfPerfAppSuiteTestCase::IsSupported( TInt aCaseId )
	{
    return aCaseId > EAlfPerfAppSuiteMin && 
           aCaseId < EAlfPerfAppSuiteMax;	
	}

// end of file
