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
* Description:  CAlfPerfApptextTestCase implementation.
 *
*/


#include "alfperfappbasetestcasecontrol.h"
#include "alfperfapptexttestcase.h"
#include "alfperfapp.hrh"
#include "alfperfappconfigconstants.h"
#include <aknutils.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alfcommand.h>
#include <alf/alfevent.h>
#include <alf/alftexturemanager.h>
#include <alf/alftextvisual.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfutil.h>
#include <e32math.h>

//#include <alfperfapp_texttest.mbg>

/**
 * Control group for Text test cases.
 */
const TInt KAlfPerfAppTextControlGroup = 1;

/*
 * Square root of sequence length for the image sequence cases
 */
const TInt SEQUENCE_LENGTH_SQRT = 7;

/**
 * Start next 'cycle' command.
 */
const TInt KAlfPerfAppTextCmdNext = 0x5001;

/**
 * Amount of text visuals.
 */
const TInt KAlfPerfAppTextTestCaseBasicTextCount = 16;

/**
 * Basic Text scale & movement test case.
 */
class CAlfPerfAppTextTestCaseBasic : public CAlfPerfAppBaseTestCaseControl
    {
public:
    CAlfPerfAppTextTestCaseBasic();
    ~CAlfPerfAppTextTestCaseBasic();

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

    /**
     * Returns opacity for index.
     */
    TReal32 OpacityForIndex( TInt aIndex );

    /**
     * Returns size for index.
     */
    TAlfRealSize SizeForIndex( TInt aIndex );

    /**
     * Returns position for index.
     */
    TAlfRealPoint PositionForIndex( TInt aIndex, const TAlfRealSize& aSize );

private:
    /**
     * Test case of which execution this instance was created.
     */ 
    TInt iCaseId;

    /**
     * Text visuals.
     */
    RPointerArray< CAlfTextVisual > iTexts;

    /**
     * Cycle counter.
     */        
    TInt iCycleCounter;


    };

/**
 * Many texts at the same time
 */
class CAlfPerfAppTextTestCaseManyTexts : public CAlfPerfAppBaseTestCaseControl
    {
public:
    CAlfPerfAppTextTestCaseManyTexts();
    ~CAlfPerfAppTextTestCaseManyTexts();

    virtual void ConstructL( 
            CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea
            , TInt aSequenceIndex, TInt aVisualCount );      
    virtual void DoStartExecuteL();
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    virtual void SetVisibleArea( const TRect& aVisibleArea );    
    TTestCaseSpecificResultText getResultL();
private:

    /**
     * Next cycle.
     */
    void NextCycleL();

    /**
     * Returns opacity for index.
     */
    TReal32 OpacityForIndex( TInt aIndex );

    /**
     * Returns size for index.
     */
    TAlfRealSize SizeForIndex( TInt aIndex );

    /**
     * Returns position for index.
     */
    TBool PositionForIndex( TInt aIndex, TAlfRealPoint& aPos, const TAlfRealSize& aSize );

private:

    /**
     * Text visuals.
     */
    RPointerArray< CAlfTextVisual > iTexts;

    /**
     * Cycle counter.
     */        
    TInt iCycleCounter;

    /**
     * Test case parameters
     */
    TInt iVisualCount; 				// Number of visual to be created
    TInt iVisualColums;
    TInt iVisualRows;
    TAlfRealPoint iVisualSpeed;    	// Pixels to move between cycles
    //TFileName iImageFileName;		// File name of the texture text file	
    TInt iTextCount;				// Number of different texts
    //TAlfTextureFlags iTextureFlags;	// Texture flags	
    TAlfRealSize iVisualSize;		// Initial size of the visual
    TAlfRealPoint iVisualSizeVel; 	// Velocity of visual size change per cycle	
    TInt iSimulatedPointerEventCount; // Number of pointer events sent to visula in one cycle
    TUint32 iPointerEventTime;      //Time to handle pointer event 
    TBool iInactiveVisual;
    TInt iLastIndexInAnimatedArea;  // the index of the last visual that is animated, all before this are also animated
    TInt iSequenceIndex;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppTextTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppTextMin && 
    aCaseId < EAlfPerfAppTextMax;
    }

CAlfPerfAppTextTestCase* CAlfPerfAppTextTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppTextTestCase* self = 
    new (ELeave) CAlfPerfAppTextTestCase( aCaseId, aSequenceIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppTextTestCase::~CAlfPerfAppTextTestCase()
    {
    if ( iEnv && iControl )
        {
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppTextControlGroup );
        }
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppTextTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& aVisibleArea, TRequestStatus& aStatus )
    {
    iEnv = &aEnv;

    // Init display, fullscreen if in sequence case mode
    CAlfDisplay* display = 0;
    CAlfControlGroup& group = iEnv->NewControlGroupL( KAlfPerfAppTextControlGroup );
    CAlfPerfAppBaseTestCaseControl* control = 0;
    iTotalVisualCount = -1;
    iAnimationDivider = 0;

    if(SequenceIndex() != 0)
          {
          // Calculate 
          TInt caseNum = SEQUENCE_LENGTH_SQRT - (SequenceIndex()-1) / SEQUENCE_LENGTH_SQRT - 1;
          TReal trg = 0.0;
          Math::Pow(trg, 2.0, caseNum);
          iAnimationDivider = trg+0.5;
          
          TRect rect;
          AknLayoutUtils::LayoutMetricsRect( 
                  AknLayoutUtils::EApplicationWindow, 
                  rect );
          iEnv->SetFullScreenDrawing( ETrue );
          
          
          TInt counter = 10 - ((SequenceIndex()-1)%SEQUENCE_LENGTH_SQRT) - 1; // 2^3...2^9 always choose the 
          trg = 0.0;
          Math::Pow(trg, 2.0, counter);
          iTotalVisualCount = trg+0.5;
          
          display = &iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayFullScreen );

          display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

          control = new (ELeave) CAlfPerfAppTextTestCaseManyTexts();
          CleanupStack::PushL( control );
          ((CAlfPerfAppTextTestCaseManyTexts*)control)->ConstructL( *iEnv, iCaseId, aVisibleArea,SequenceIndex(),iTotalVisualCount );

          }
    else
        {
        display = &iEnv->NewDisplayL( aVisibleArea, CAlfEnv::ENewDisplayAsCoeControl );
        display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

        switch ( iCaseId )
            {
            case EAlfPerfAppTextBasicMoveScale:
                control = new (ELeave) CAlfPerfAppTextTestCaseBasic;
                CleanupStack::PushL( control );
                control->ConstructL( *iEnv, iCaseId, aVisibleArea );
                break;

            case EAlfPerfAppTextManyTexts:
            case EAlfPerfAppTextManyTextsWithPointerEvent:
            case EAlfPerfAppTextManyTextsWithAutoSize:
            case EAlfPerfAppTextInactiveVisuals:
                control = new (ELeave) CAlfPerfAppTextTestCaseManyTexts;
                CleanupStack::PushL( control );
                ((CAlfPerfAppTextTestCaseManyTexts*)control)->ConstructL( *iEnv, iCaseId, aVisibleArea,SequenceIndex(),iTotalVisualCount );
                break;

            default:
                User::Leave( KErrNotSupported );
                break;
            }
          }

    group.AppendL( control ); // ownership passed to control group.
    iControl = control;
    CleanupStack::Pop( control );    

    display->Roster().ShowL( group );

    // This must be last statement.
    CAlfPerfAppTestCase::CompleteNow( aStatus, KErrNone );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppTextTestCase::ExecuteL( TRequestStatus& aStatus )
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
void CAlfPerfAppTextTestCase::TearDown()
    {
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppTextTestCase::HandleVisibleAreaChange( const TRect& aRect )
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

TInt CAlfPerfAppTextTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppTextTestCase::CAlfPerfAppTextTestCase( TInt aCaseId, TInt aSequenceIndex )
: CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
        {
        }

void CAlfPerfAppTextTestCase::ConstructL()
    {
    }

// Implementation of CAlfPerfAppTextTestCaseBasic:

CAlfPerfAppTextTestCaseBasic::CAlfPerfAppTextTestCaseBasic()
    {
    }

CAlfPerfAppTextTestCaseBasic::~CAlfPerfAppTextTestCaseBasic()
    {
    iTexts.Close();
    }

void CAlfPerfAppTextTestCaseBasic::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );

    for ( TInt ii = 0; ii < KAlfPerfAppTextTestCaseBasicTextCount; ii++ )
        {
        CAlfTextVisual* visual = CAlfTextVisual::AddNewL( *this );
        visual->SetFlag( EAlfVisualFlagManualLayout );
        visual->SetOpacity( 1.0f );

        visual->SetOpacity( OpacityForIndex( ii ) );
        TAlfRealSize sz = SizeForIndex( ii );
        visual->SetSize( sz );
        visual->SetPos( PositionForIndex( ii, sz ) );

        visual->SetTextL(_L("Text"));

        iTexts.AppendL( visual );

        }

    }

void CAlfPerfAppTextTestCaseBasic::DoStartExecuteL()
    {
    NextCycleL();

    CompleteAfterL( 5000 );
    }

void CAlfPerfAppTextTestCaseBasic::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppTextCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppTextTestCaseBasic::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
            aEvent.CustomParameter() == KAlfPerfAppTextCmdNext )
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

void CAlfPerfAppTextTestCaseBasic::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppTextCmdNext, this );
    User::LeaveIfError( Env().Send( command, 500 ) );

    iCycleCounter++;

    for ( TInt ii = 0; ii < iTexts.Count(); ii++ )
        {
        CAlfTextVisual* visual = iTexts[ ii ];

        TInt speed = KVisualAnimSpeed;
        if ( ii & 1 )
            {
            speed /= 2;
            }

        visual->SetOpacity( TAlfTimedValue( OpacityForIndex( ii ), speed ) );
        TAlfRealSize sz = SizeForIndex( ii );
        visual->SetSize( sz, speed );
        visual->SetPos( PositionForIndex( ii, sz ), speed );                                    
        }
    }

TReal32 CAlfPerfAppTextTestCaseBasic::OpacityForIndex( TInt aIndex )
    {
    switch ( ( iCycleCounter + aIndex ) % 3 ) 
        {
        case 0:
            return 1.0f;
        case 1:
            return 0.5f;
        case 2:
        default:
            return 0.25f;
        }
    }

TAlfRealSize CAlfPerfAppTextTestCaseBasic::SizeForIndex( TInt aIndex )
    {
    switch ( ( iCycleCounter + aIndex ) % 3 ) 
        {
        case 0:
            return TAlfRealSize( 25.f, 25.f );
        case 1:
            return TAlfRealSize( 50.f, 50.f );
        case 2:
        default:
            return TAlfRealSize( 100.f, 100.f );
        }
    }

TAlfRealPoint CAlfPerfAppTextTestCaseBasic::PositionForIndex( 
        TInt aIndex, const TAlfRealSize& aSize )
    {
    const TSize KSize( Env().PrimaryDisplay().VisibleArea().Size() );

    switch ( ( iCycleCounter + aIndex ) & 3 )
        {
        case 0:
            return TAlfRealPoint( 0.0f, 0.0f ); 

        case 1:
            return TAlfRealPoint( KSize.iWidth - aSize.iWidth, KSize.iHeight - aSize.iHeight ); 

        case 2:
            return TAlfRealPoint( KSize.iWidth - aSize.iWidth, 0.0f ); 

        case 3:
        default:
            return TAlfRealPoint( 0.0f, KSize.iHeight - aSize.iHeight ); 
        }
    }


// Implementation of CAlfPerfAppTextTestCaseManyTexts:
CAlfPerfAppTextTestCaseManyTexts::CAlfPerfAppTextTestCaseManyTexts()
    {
    }

CAlfPerfAppTextTestCaseManyTexts::~CAlfPerfAppTextTestCaseManyTexts()
    {
    iTexts.Close();

    //delete iLoader;
    }

void CAlfPerfAppTextTestCaseManyTexts::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea, TInt aSequenceIndex, TInt aVisualCount )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );
    CAlfTextureManager& textureManager = aEnv.TextureManager();

    // Parameters for test cases

    iVisualSpeed 			= TAlfRealPoint(1,0); 	
    iTextCount 			= 4;
    iVisualCount            = (aVisualCount != -1) ? aVisualCount : KVisualCout;    
    iVisualSize				= TAlfRealSize( 25.f, 25.f );
    iVisualSizeVel			= TAlfRealPoint();	
    iSimulatedPointerEventCount = 0;
    iLastIndexInAnimatedArea    = 0;
    iSequenceIndex = aSequenceIndex;
    iPointerEventTime           = 0;
    iInactiveVisual         = EFalse;   
    iVisualColums           = 0;
    iVisualRows             = 0;

    if(aSequenceIndex != 0)
        {
        TInt caseNum = SEQUENCE_LENGTH_SQRT - (iSequenceIndex-1) / SEQUENCE_LENGTH_SQRT - 1;
        TReal trg = 0.0;
        Math::Pow(trg, 2.0, caseNum);
        TInt pow = trg+0.5;
        TReal animatedAreaRelativeSize = 1.f/pow;

        iLastIndexInAnimatedArea = iVisualCount*animatedAreaRelativeSize+0.5;
        iLastIndexInAnimatedArea = (iLastIndexInAnimatedArea < 1) ? 1 : iLastIndexInAnimatedArea;

        trg = 0.0;
        Math::Sqrt(trg,TReal(iVisualCount));
        iVisualColums = (trg + 0.5);
        iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

        TSize displaySize(Env().PrimaryDisplay().VisibleArea().Size());
        iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
        }
    else
        {
        switch(CaseId())
            {
            case EAlfPerfAppTextManyTexts:
                break;

            case EAlfPerfAppTextManyTextsWithPointerEvent:
                iSimulatedPointerEventCount = 10;
                break;

            case EAlfPerfAppTextManyTextsWithAutoSize:
                iVisualSizeVel = TAlfRealPoint(10,10);
                break;
                //To set all but 2 visuals Inactive, 
                //i.e. those will not participate in any of roster operations
            case EAlfPerfAppTextInactiveVisuals:
                iInactiveVisual = ETrue;
                iSimulatedPointerEventCount = 100;
                break;
            default:
                User::Leave( KErrNotSupported );
                break;
            };
        }

    for ( TInt ii = 0; ii < iVisualCount; ii++ )
        {
        CAlfTextVisual* visual = CAlfTextVisual::AddNewL( *this );
        iTexts.AppendL( visual );        

        visual->SetFlag( EAlfVisualFlagManualLayout );
        visual->SetOpacity( 1.0f );

        visual->SetOpacity( OpacityForIndex( ii ) );
        visual->SetSize( iVisualSize );

        TAlfRealPoint pos(0,0);
        if (PositionForIndex( ii, pos, iVisualSize ))
            {
            visual->SetPos( pos );                                    
            }

        visual->SetTextL( _L("TEXT"));
#ifdef ALFPERFAPP_ENABLE_INACTIVE_FLAG_CASES
        if ( iInactiveVisual && ii < iVisualCount-2 )
            {
            visual->SetFlag( EAlfVisualFlagInactive );
            }
#endif
        }
    }

void CAlfPerfAppTextTestCaseManyTexts::DoStartExecuteL()
    {
    NextCycleL();

    CompleteAfterL( 5000 );
    }

void CAlfPerfAppTextTestCaseManyTexts::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppTextCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppTextTestCaseManyTexts::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
            aEvent.CustomParameter() == KAlfPerfAppTextCmdNext )
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

void CAlfPerfAppTextTestCaseManyTexts::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppTextCmdNext, this );
    User::LeaveIfError( Env().Send( command, 500 ) );

    iCycleCounter++;

    // Update cycle state
    for ( TInt ii = 0; ii < iTexts.Count(); ii++ )
        {
        CAlfTextVisual* visual = iTexts[ ii ];

        TInt speed = KVisualAnimSpeed;

        visual->SetOpacity( TAlfTimedValue( OpacityForIndex( ii ), speed ) );
        TAlfRealSize sz = SizeForIndex( ii );
        visual->SetSize( sz, speed );
        
        TAlfRealPoint pos(0,0);
        if (PositionForIndex( ii, pos, iVisualSize ))
            {
            visual->SetPos( pos,speed );                                    
            }
        }
    if ( iSimulatedPointerEventCount )
        {
        TInt tickPeriod1;    
        tickPeriod1 = User::TickCount();
        while( (User::TickCount() - tickPeriod1) ==0 ) ; // wait for a tick rollover

        // Send pointer events    
        for (TInt i=0; i <iSimulatedPointerEventCount; i++)
            {
            const TSize KSize( Env().PrimaryDisplay().VisibleArea().Size() );

            TPoint pos(AlfUtil::RandomInt(0, KSize.iWidth-1), AlfUtil::RandomInt(0, KSize.iWidth-1));

            // down
            TPointerEvent eventDown(TPointerEvent::EButton1Down, 0, pos, TPoint(0,0));
            Env().PrimaryDisplay().HandlePointerEventL( eventDown );

            //up
            TPointerEvent eventUp(TPointerEvent::EButton1Up, 0, pos, TPoint(0,0));
            Env().PrimaryDisplay().HandlePointerEventL( eventUp );
            }          
        TInt tickPeriod2 = User::TickCount();
        iPointerEventTime = iPointerEventTime + ( tickPeriod2 - tickPeriod1 );
        }        
    }

TReal32 CAlfPerfAppTextTestCaseManyTexts::OpacityForIndex( TInt aIndex )
    {
    
    if (iSequenceIndex != 0 && aIndex <= iLastIndexInAnimatedArea)
        {
        if (iCycleCounter&0x01)
            {
            return 0.7f;
            }
        else
            {
            return 0.9f;
            }
        }
    return 0.5f;
    }

TAlfRealSize CAlfPerfAppTextTestCaseManyTexts::SizeForIndex( TInt aIndex )
    {
    TAlfRealSize size = iTexts[ aIndex ]->Size().Target();
    return TAlfRealSize( size.iWidth + iVisualSizeVel.iX, size.iHeight + iVisualSizeVel.iY );
    }

TBool CAlfPerfAppTextTestCaseManyTexts::PositionForIndex( 
        TInt aIndex, TAlfRealPoint& aPos, const TAlfRealSize& /*aSize*/ )
    {
    
    if (iSequenceIndex != 0)       
        {
        if(iCycleCounter == 0 || aIndex <= iLastIndexInAnimatedArea)
            {
            TInt gridIndex = aIndex + iCycleCounter;
            TInt columnPos = gridIndex % iVisualColums;
            TInt rowPos = gridIndex / iVisualColums;
            rowPos = rowPos % iVisualRows; 
            //TAlfRealSize visualSize = iImages[ aIndex ]->Size().Target();
            aPos = TAlfRealPoint( columnPos*iVisualSize.iWidth, rowPos*iVisualSize.iHeight );           
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    


    TInt KItemStep = 20;
    const TSize KSize( Env().PrimaryDisplay().VisibleArea().Size() );
    const TSize KSize2( KSize.iWidth/KItemStep, KSize.iHeight/KItemStep);

    TInt pos = aIndex + iCycleCounter * iVisualSpeed.iX;
    TInt ypos = pos / KSize2.iWidth; 
    ypos = ypos % KSize2.iHeight; 
    TInt xpos = pos % KSize2.iWidth; 
    aPos =  TAlfRealPoint( xpos*KItemStep, ypos*KItemStep );
    return ETrue;
    }
//Write pointer event lag result 
TTestCaseSpecificResultText CAlfPerfAppTextTestCase::getCaseSpecificResultL()
    {
    if(SequenceIndex() != 0 && CaseID() == EAlfPerfAppTextManyTexts)
        {
        TTestCaseSpecificResultText result = HBufC::NewL(KAlfPerfAppMaxCharsInSpecificResultText);
        result->Des().Append(_L("Total Visual Count: "));
        result->Des().AppendNum(iTotalVisualCount);
        result->Des().Append(_L(" Animated: 1/"));
        result->Des().AppendNum(iAnimationDivider);
        return result;
        }
    
    if(iCaseId == EAlfPerfAppTextManyTextsWithPointerEvent ||
            iCaseId == EAlfPerfAppTextInactiveVisuals)
        {
        if ( iControl)
            {
            return static_cast<CAlfPerfAppTextTestCaseManyTexts*>(iControl)->getResultL();
            }
        }
    // If nothing was done before this, return empty result
    TTestCaseSpecificResultText emptyResult = 0;
    return emptyResult;
    }

TTestCaseSpecificResultText CAlfPerfAppTextTestCaseManyTexts::getResultL()
    {
    TTestCaseSpecificResultText result = 0; 
    if ( iSimulatedPointerEventCount )
        { 
        //Assuming tic period to be 1 ms 
        if(iPointerEventTime && iCycleCounter)
            {
            iPointerEventTime = ((iPointerEventTime/iCycleCounter)*1000)/(iSimulatedPointerEventCount*2);
            }
        if(iPointerEventTime)
            {
            result = HBufC::NewL(KAlfPerfAppMaxCharsInSpecificResultText);
            if ( iInactiveVisual )
                {
                result->Des().Append(_L("Inactive Visual "));      		
                }
            result->Des().Append(_L("Avg Pointer event lag: "));
            result->Des().AppendNum( iPointerEventTime );
            result->Des().Append(_L("ms"));
            } 
        }
    return result;
    }


