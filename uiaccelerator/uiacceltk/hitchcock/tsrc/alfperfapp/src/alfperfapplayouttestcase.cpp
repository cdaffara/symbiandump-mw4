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
* Description:  CAlfPerfAppLayoutTestCase implementation.
*
*/


#include "alfperfappbasetestcasecontrol.h"
#include "alfperfapplayouttestcase.h"
#include "alfperfapp.hrh"

#include <aknutils.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alfcommand.h>
#include <alf/alfevent.h>
#include <alf/alftexturemanager.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfutil.h>
#include <alf/alfgridlayout.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfviewportlayout.h>

#include <alfperfapp_imagetest.mbg>

/**
 * Control group for test cases.
 */
const TInt KAlfPerfAppLayoutControlGroup = 1;

/**
 * Start next 'cycle' command.
 */
const TInt KAlfPerfAppLayoutCmdNext = 0x5001;

/**
 * Testing layouts
 */
class CAlfPerfAppLayoutTestCaseControl : public CAlfPerfAppBaseTestCaseControl
    {
public:
    CAlfPerfAppLayoutTestCaseControl();
    ~CAlfPerfAppLayoutTestCaseControl();
    
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );      
    virtual void DoStartExecuteL();
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    virtual void SetVisibleArea( const TRect& aVisibleArea );    

private:
    
    /**
     * Next cycle.
     */
    void NextCycleL();
         
private:
    /**
     * Test case of which execution this instance was created.
     */ 
    TInt iCaseId;
    
    /**
     * Image visuals.
     */
    RPointerArray< CAlfImageVisual > iImages;
        
    /**
     * Cycle counter.
     */        
    TInt iCycleCounter;
    
    /**
     * Image loader utility.
     * Owned.
     */
    CAlfImageLoaderUtil* iLoader;
    
    
    /**
     * Main layout
     * Owned.
     */
    CAlfLayout* iLayout;
    
    /**
     * Test case parameters
     */
    TInt iVisualCount; 				// Number of visual to be created
    TFileName iImageFileName;		// File name of the texture image file	
	TInt iImageCount;				// Number of different images
    TAlfTextureFlags iTextureFlags;	// Texture flags	
    TAlfRealSize iVisualSize;		// Initial size of the visual
    };
        
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppLayoutTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppLayoutMin && 
           aCaseId < EAlfPerfAppLayoutMax;
    }

CAlfPerfAppLayoutTestCase* CAlfPerfAppLayoutTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppLayoutTestCase* self = 
        new (ELeave) CAlfPerfAppLayoutTestCase( aCaseId, aSequenceIndex);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppLayoutTestCase::~CAlfPerfAppLayoutTestCase()
    {
    if ( iEnv && iControl )
        {
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppLayoutControlGroup );
        }
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppLayoutTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& aVisibleArea, TRequestStatus& aStatus )
    {
    iEnv = &aEnv;
    
    CAlfDisplay& display = 
        iEnv->NewDisplayL( aVisibleArea, CAlfEnv::ENewDisplayAsCoeControl );
    display.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    CAlfControlGroup& group = 
        iEnv->NewControlGroupL( KAlfPerfAppLayoutControlGroup );

    CAlfPerfAppBaseTestCaseControl* control = NULL;
    
    switch ( iCaseId )
        {
        case EAlfPerfAppLayoutGrid:
        case EAlfPerfAppLayoutDeck:
        case EAlfPerfAppLayoutAnchor:
        case EAlfPerfAppLayoutCurvePath:
        case EAlfPerfAppLayoutFlow:
        case EAlfPerfAppLayoutViewport:
            control = new (ELeave) CAlfPerfAppLayoutTestCaseControl;
            break;
                        
        default:
            User::Leave( KErrNotSupported );
            break;
        }
        
    CleanupStack::PushL( control );
    control->ConstructL( *iEnv, iCaseId, aVisibleArea );
    group.AppendL( control ); // ownership passed to control group.
    iControl = control;
    CleanupStack::Pop( control );    
    
    display.Roster().ShowL( group );
    
    // This must be last statement.
    CAlfPerfAppTestCase::CompleteNow( aStatus, KErrNone );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppLayoutTestCase::ExecuteL( TRequestStatus& aStatus )
    {
    // SetupL must have been called first.
    __ASSERT_ALWAYS( iEnv, User::Invariant() );
    __ASSERT_ALWAYS( iControl, User::Invariant() );
    
    iControl->StartExecuteL( aStatus );
    }

// -----------------------------------------------------------------------------
// Tears down.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppLayoutTestCase::TearDown()
    {
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppLayoutTestCase::HandleVisibleAreaChange( const TRect& aRect )
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
 
TInt CAlfPerfAppLayoutTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppLayoutTestCase::CAlfPerfAppLayoutTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppLayoutTestCase::ConstructL()
    {
    }


// Implementation of CAlfPerfAppLayoutTestCaseControl:

CAlfPerfAppLayoutTestCaseControl::CAlfPerfAppLayoutTestCaseControl()
    {
    }

CAlfPerfAppLayoutTestCaseControl::~CAlfPerfAppLayoutTestCaseControl()
    {
    iImages.Close();
    
    delete iLoader;
    }
    
void CAlfPerfAppLayoutTestCaseControl::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );
    
    // Parameters for test cases
    TSize gridSize 			= TSize(10,10); // Total count should be equal to iVisualCount
    iImageFileName 			= _L("c:\\data\\others\\alfperfapp_imagetest.png");	
	iImageCount 			= 4;
    iVisualCount 			= 100;	
    iTextureFlags 			= EAlfTextureFlagDefault;	
	iVisualSize				= TAlfRealSize( 25.f, 25.f );

    switch(CaseId())
    	{
        case EAlfPerfAppLayoutGrid:
    		iLayout = CAlfGridLayout::AddNewL(*this, gridSize.iWidth, gridSize.iHeight);
        	break;
       
        case EAlfPerfAppLayoutDeck:
    		iLayout = CAlfDeckLayout::AddNewL(*this);
        	break;
       
        case EAlfPerfAppLayoutAnchor:
            iLayout = CAlfAnchorLayout::AddNewL(*this);  
        	break;
       
        case EAlfPerfAppLayoutCurvePath:
        	iLayout = CAlfCurvePathLayout::AddNewL(*this);
        	break;
       
        case EAlfPerfAppLayoutFlow:
        	iLayout = CAlfFlowLayout::AddNewL(*this);
        	break;
       
        case EAlfPerfAppLayoutViewport:
        	iLayout = CAlfViewportLayout::AddNewL(*this);
        	break;
       	
    	default:
            User::Leave( KErrNotSupported );
            break;
 		};
    


	// Create grid layout
	
    CAlfTextureManager& textureManager = aEnv.TextureManager();
    
	   
    TParsePtr parse(iImageFileName);
    	
    for ( TInt count = 0;  count < iImageCount; count++ )
        {        
        CAlfTexture& texture = textureManager.LoadTextureL(iImageFileName, iTextureFlags, count+1);
        }   
       	
 	for ( TInt ii = 0; ii < iVisualCount; ii++ )
        {
        CAlfImageVisual* visual = CAlfImageVisual::AddNewL( *this,  iLayout);
        iImages.AppendL( visual );        
        visual->SetSize( iVisualSize );
        }
    
   	iLayout->UpdateChildrenLayout();
    }
    
void CAlfPerfAppLayoutTestCaseControl::DoStartExecuteL()
    {
    NextCycleL();
    // Complete after one cycle
    CompleteNow( KErrNone );
    }

void CAlfPerfAppLayoutTestCaseControl::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppLayoutCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppLayoutTestCaseControl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppLayoutCmdNext )
        {
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        return ETrue;
        }
    
    return CAlfPerfAppBaseTestCaseControl::OfferEventL( aEvent );
    }

void CAlfPerfAppLayoutTestCaseControl::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppLayoutCmdNext, this );
    User::LeaveIfError( Env().Send( command, 500 ) );
    
    iCycleCounter++;
    
    for(TInt i=0; i<10; i++)
    	{
   		iLayout->UpdateChildrenLayout();
   		}
    }

