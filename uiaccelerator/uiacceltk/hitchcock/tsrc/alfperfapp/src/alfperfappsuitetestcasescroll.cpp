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
#include <alf/alfimageloaderutil.h>
#include <alf/alftexturemanager.h>
#include <alf/alfevent.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfborderbrush.h>
#include <alf/AlfBrushArray.h>
#include <alf/AlfTextStyle.h>

#include <alfperfapp_imagetest.mbg>

#include "AlfPerfApp.hrh"
#include "AlfPerfAppSuiteTestCaseScroll.h"
#include "AlfPerfAppSuitePeopleControl.h"
#include "AlfPerfAppSuiteEPGControl.h"
#include "AlfPerfAppSuiteTestCase.h"
#include "alfperfappconfigconstants.h"




// ============================= MEMBER FUNCTIONS ==============================

CAlfPerfAppSuiteTestCaseScroll::CAlfPerfAppSuiteTestCaseScroll(CAlfPerfAppSuiteTestCase* aAppSuiteTestCase)
    :iAppSuiteTestCase(aAppSuiteTestCase)
    {
    }

CAlfPerfAppSuiteTestCaseScroll::~CAlfPerfAppSuiteTestCaseScroll()
    {
    }

void CAlfPerfAppSuiteTestCaseScroll::ConstructL(
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppSuiteTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );

    // Setup scrolling cycle lenght
    switch( aCaseId )
    	{
    	case EAlfPerfAppSuiteCaseScrollContinuousPortrait:
    	case EAlfPerfAppSuiteCaseScrollContinuousLandscape:
    		iCycleLenght = KCycleLenghtContinuous;
    		break;

    	case EAlfPerfAppSuiteCaseScrollIncrementalPortrait:
    	case EAlfPerfAppSuiteCaseScrollIncrementalLandscape:
    		iCycleLenght = KCycleLenghtIncremental;
    		break;

    	default:
    		User::Leave( KErrNotFound );
    		break;
    	}
    
    // Use a viewport layout as a base
    iViewPortLayout = CAlfViewportLayout::AddNewL( *this );
    
   	// Visible viewport is full screen
   	iViewPortLayout->SetViewportSize( aVisibleArea.Size(), 0 );
   	
    // Then use a gird to hold suites
    CAlfGridLayout* baseGridLayout = CAlfGridLayout::AddNewL( *this, 1, 1, iViewPortLayout );

    // Construct suites
    
    iLoadedImages = 0;
    iSuiteCount = 3;
    // EPG suite
   	CAlfPerfAppSuiteEPGControl* epg = new (ELeave) CAlfPerfAppSuiteEPGControl(this);
   	epg->ConstructL( aEnv, aVisibleArea.Size(), baseGridLayout );
   	aEnv.ControlGroup( KAlfPerfAppSuiteControlGroup ).AppendL( epg );
	
	// PEOPLE suite
   	CAlfPerfAppSuitePeopleControl* people = new (ELeave) CAlfPerfAppSuitePeopleControl(this);
   	people->ConstructL( aEnv, aVisibleArea.Size(), baseGridLayout );
   	aEnv.ControlGroup( KAlfPerfAppSuiteControlGroup ).AppendL( people );
	
    // Another EPG suite
   	epg = new (ELeave) CAlfPerfAppSuiteEPGControl(this);
   	epg->ConstructL( aEnv, aVisibleArea.Size(), baseGridLayout );
   	aEnv.ControlGroup( KAlfPerfAppSuiteControlGroup ).AppendL( epg );
	
   	// Set correct number of columns to grid
   	baseGridLayout->SetColumns( iSuiteCount );

   	// Virtual viewport depends on the number of suites
   	TSize size = iVisibleArea.Size();
   	size.iWidth = iVisibleArea.Width()*iSuiteCount;
   	iViewPortLayout->SetVirtualSize( size, 0 );

   	// TextureLoadingCompleted() call back is handled now in epg, people control classes.
   	// so no need of explicit wait in this thread.
   	// ImagesLoaded() does the request completion functionlaity. 
    }

void CAlfPerfAppSuiteTestCaseScroll::ImagesLoaded( TInt aErrorCode )
    {
    // inform if there is any error
    if( aErrorCode != KErrNone )
        {
        iAppSuiteTestCase->ImagesLoaded( aErrorCode );
        }
    
    iLoadedImages++;
    if (iLoadedImages == iSuiteCount)
        {
        iAppSuiteTestCase->ImagesLoaded(KErrNone);
        }
    }

void CAlfPerfAppSuiteTestCaseScroll::DoStartExecuteL()
    {
    NextCycleL();
    }

void CAlfPerfAppSuiteTestCaseScroll::SetVisibleArea(
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppSuiteCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppSuiteTestCaseScroll::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() &&
         aEvent.CustomParameter() == KAlfPerfAppSuiteCmdNext )
        {
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        return ETrue;
        }

    return CAlfPerfAppSuiteTestCaseControl::OfferEventL( aEvent );
    }

void CAlfPerfAppSuiteTestCaseScroll::NextCycleL()
    {
    switch( iCaseId )
    	{
    	case EAlfPerfAppSuiteCaseScrollContinuousPortrait:
    	case EAlfPerfAppSuiteCaseScrollContinuousLandscape:
    		{
    	    if( iCycleCounter == KNumberOfRepeats )
    	    	{
    	    	CompleteNow( KErrNone );
    	    	return;
    	    	}

    	    // Bounce between first and last suites
    	    if( iCycleCounter % 2 )
    	    	{
    	    	iViewPortPos = TAlfRealPoint( 0, 0 );
    	    	}
    	    else
    	    	{
    	    	iViewPortPos = TAlfRealPoint( iVisibleArea.Width()*(iSuiteCount-1), 0 );
    	    	}
    		}
    		break;

    	case EAlfPerfAppSuiteCaseScrollIncrementalPortrait:
    	case EAlfPerfAppSuiteCaseScrollIncrementalLandscape:
    		{
    	    if( iCycleCounter == (iSuiteCount*2*KNumberOfRepeats)-1 )
    	    	{
    	    	CompleteNow( KErrNone );
    	    	return;
    	    	}

    	    // Bounce between suites
    	    if( (iCycleCounter / (iSuiteCount-1)) % 2 )
    	    	{
    	    	iViewPortPos.iX -= iVisibleArea.Width();
    	    	}
    	    else
    	    	{
    	    	iViewPortPos.iX += iVisibleArea.Width();
    	    	}
    		}
    		break;

    	default:
    		CompleteNow( KErrNotFound );
    		return;
    	}

    iViewPortLayout->SetViewportPos( iViewPortPos, iCycleLenght );

    iCycleCounter++;

    TAlfCustomEventCommand command( KAlfPerfAppSuiteCmdNext, this );
    User::LeaveIfError( Env().Send( command, iCycleLenght ) );
    }

// end of file
			
