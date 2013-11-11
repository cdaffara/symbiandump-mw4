/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of member functions of 
*                AlfExAnalogDialerControl.
*
*/



#include <coemain.h>

// Visuals
#include <alf/alfgridlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alflinevisual.h>

// Layouts
#include <alf/alfdecklayout.h>
#include <alf/alfcurvepath.h>
#include <alf/alfcurvepathlayout.h>

// Brushes
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfimagebrush.h>
#include <alf/alfimage.h>
#include <alf/alfshadowborderbrush.h>
#include <alf/alfgradientbrush.h>

// Unsorted
#include <aknutils.h>
#include <alf/alftransformation.h>
#include <alf/alfevent.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfmappingfunctions.h>
#include <alf/alfroster.h>

#include <e32math.h>

#include "alfexanalogdialercontrol.h"
#include "alfexanalogdialerfeedback.h"

// Image names
_LIT(KImageAnalogDialerBackground, "background.png");
_LIT(KImageAnalogDialerNumbers, "numbers.png");
_LIT(KImageAnalogDialerPlate, "plate.png");
_LIT(KImageAnalogDialerStopper, "stopper.png");

#define RADTODEG(Rad) ((180.0 * Rad) / KPi)

// font color 
const TRgb KFontColor = TRgb(192,192,192);

// dimension of grid
const TInt KDimensionsOfGrid = 2;

// Tags for visuals which have always same appearance. 
// TTagAnalogDialerVisual is used for indexing
const char* KTagArray[] =
    {
    "root-layout",
    "display-deck",
    "display-visual",
    "plate-deck",
    "numbers-visual",
    "plate-visaul",
    "stopper-visual"
    };     

// Dimensions of grid. iLayoutMode is used for indexing
const TInt KRowsAndColumnsOfGrid[][KDimensionsOfGrid] =
    {
    { 1, 2 },   // columns, rows
    { 2, 1 }  
    };
     
// Template for displaying number
_LIT(KNumberDisplayTemplate, "%d");

// Radius of Clear area of plate. Per cents from radius of plate.
const TInt KPlateClearRadius = 25;

// Unit speed of returning plate to idle position. Degrees per second. 
const TInt KPlateReturnSpeed = 65;

// As dragging events occur very often, redrawing plate is optimised that
// it is not drawn after every dragging event. 
// Plate is redrawn when angle of the plate has changed KPlateDraggingDelta clockwise.
const TInt KPlateDraggingDelta = 2;

// position of stopper in degrees. Angle is 43 clockwise down from the X-axis
const TInt KStopperAngle = 360-43;

// angle of arch on which numbers (or holes) are distributed
const TInt KPlateNumbersDistributionAngle = 290;

// minimum rotation of plate that a number becomes selected. If plate has been dialed
// less than or equal to KPlateMinimumRotateAngle degrees, number is not selected.
const TInt KPlateMinimumRotateAngle=43;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerControl* CAlfExAnalogDialerControl::NewL(CAlfEnv& aEnv)
    {
    CAlfExAnalogDialerControl* self = CAlfExAnalogDialerControl::NewLC(aEnv);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerControl* CAlfExAnalogDialerControl::NewLC(CAlfEnv& aEnv)
    {
    CAlfExAnalogDialerControl* self = new (ELeave) CAlfExAnalogDialerControl();
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
// ~CAlfExAnalogDialerControl
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerControl::~CAlfExAnalogDialerControl()
    {
    // Remove observer is still there
    if (iFlags & EAnalogDialerControlFlagLongTapRegistered)
        {
        Display()->Roster().RemovePointerEventObserver(EAlfPointerEventReportLongTap, *this);
        }
    Env().CancelCommands(this);
    delete iFeedback;
    }

// ---------------------------------------------------------------------------
// CAlfExAnalogDialerControl
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerControl::CAlfExAnalogDialerControl()
        : CAlfControl()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::ConstructL(CAlfEnv& aEnv)
    {
    CAlfControl::ConstructL(aEnv);
    // Id cannot be be set in constructor. Id is needed in appui to retrieve control from control group
	SetId(KAlfExAnalogDialerControlId);

    // resolve layout: portrait or landscape
    iLayoutMode = ResolveLayout(iSquareSide, iLongerSide);
    
    // setup visuals accroding to layout
    (iLayoutMode == ELayoutAnalogDialerPortrait) ? SetupPortraitModeL() : SetupLandscapeModeL();

    // set up vibra system
    iFeedback = CAlfExAnalogDialerFeedback::NewL();
    }   
    
    
    
// ---------------------------------------------------------------------------
// Creates layouts and visual for portrait layout
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::SetupPortraitModeL()
    {
    // Setup grid (root layout)
    AddGridLayoutL();

    // from top to bottom 1) display pane 2) dialer
    AddDisplayPaneL();
    AddDialerL();

    //AddEffectLayout();
    }

// ---------------------------------------------------------------------------
// Creates layouts and visual for portrait layout
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::SetupLandscapeModeL()
    {
    // Setup grid (root layout)
    AddGridLayoutL();

    // from left to right 1) dialer 2) display pane
    AddDialerL();
    AddDisplayPaneL();

    //AddEffectLayout();
    }


// ---------------------------------------------------------------------------
// Creates grid layout or modify existing one after layout change
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::AddGridLayoutL()
    {
    if (!iRootLayout)
        {
        // Fresh start of the application. Create a grid layout to act as the root layout visual.
        iRootLayout = CAlfGridLayout::AddNewL(*this, 
                                            KRowsAndColumnsOfGrid[iLayoutMode][EAlfGridColumn], 
                                            KRowsAndColumnsOfGrid[iLayoutMode][EAlfGridRow]);
        // Tag is not really needed, because root layout assigned to member variable
        iRootLayout->SetTagL(_L8(KTagArray[ETagAnalogDialerRootLayout]));   

        // Brushed must enabled before they can be used
        iRootLayout->EnableBrushesL();      

        // Background image is implemented using image brush
        CAlfTexture& backgroundTexture = Env().TextureManager().LoadTextureL( 
                                            KImageAnalogDialerBackground,
                                            EAlfTextureFlagDefault, 
                                            KAlfAutoGeneratedTextureId);        
        // Default scale mode CAlfImageVisual::EScaleFit is OK
        CAlfImageBrush* brush = CAlfImageBrush::NewLC(Env(), TAlfImage(backgroundTexture));

        // sets background image behind the visual
        brush->SetLayer(EAlfBrushLayerBackground);
        
        // Append brush to the layout and transfer its ownership to the visual.    
        iRootLayout->Brushes()->AppendL(brush, EAlfHasOwnership);
        CleanupStack::Pop(brush);
        }
    else
        {
        // Layout change has taken place. Modify dimensions and weights
        // Reset current dimension
        for(TInt i=0; i<KDimensionsOfGrid; i++)
            {
            // resetting non existing weights causes Leave
            TRAP_IGNORE(iRootLayout->RemoveWeightL(EAlfGridColumn, i));
            TRAP_IGNORE(iRootLayout->RemoveWeightL(EAlfGridRow, i));
            }
        // now set up new dimension
        iRootLayout->SetColumnsL(KRowsAndColumnsOfGrid[iLayoutMode][EAlfGridColumn]);
        iRootLayout->SetRowsL(KRowsAndColumnsOfGrid[iLayoutMode][EAlfGridRow]);
        }

    // Sets weights of grid
    SetWeightsOfGridL(*iRootLayout,
                      iLayoutMode,
                      iSquareSide,
                      iLongerSide,
                      iPlateTopOffset);
                             
    // Set the padding of the root layout
    
    // TODO: Padding disabled for now. This seems to mess the angle calculations as we the calculations do not consider
    // the effect of padding.

    /*
    TRect displayRect = Env().PrimaryDisplay().VisibleArea();
    TInt pad = displayRect.Height();
    if ( iLayoutMode == ELayoutAnalogDialerLandscape )
        {
        pad = displayRect.Width();
        }   
    layout->SetInnerPadding(TPoint( 
    		pad * KPaddingExtents[iLayoutMode][EAlfGridColumn],
    		pad * KPaddingExtents[iLayoutMode][EAlfGridRow]));
    layout->SetPadding(pad * KPaddingPercentage);*/
    }


// ---------------------------------------------------------------------------
// Adds content into display pane of the application
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::AddDisplayPaneL()
    {
    CAlfDeckLayout* displayDeck = CAlfDeckLayout::AddNewL(*this, iRootLayout);
    displayDeck->SetTagL(_L8( KTagArray[ETagAnalogDialerDisplayDeck]));

    // Add TextVisual number display
    iDisplayPane = CAlfTextVisual::AddNewL(*this, displayDeck );
    // Tag is not really needed
    iDisplayPane->SetTagL(_L8( KTagArray[ETagAnalogDialerDisplayVisual]));
    iDisplayPane->SetAlign(EAlfAlignHRight, EAlfAlignVCenter);      

    /*iDisplayPane->SetFlag(EAlfVisualFlagManualLayout);
    
    // Decoration
    if (iLayoutMode == ELayoutAnalogDialerPortrait)
    	{
    	iDisplayPane->SetCenteredPosAndSize(TPoint(180,100),TSize(300,50));
    	}
    else 
    	{
    	iDisplayPane->SetCenteredPosAndSize(TPoint(150,180),TSize(200,300));
    	}*/
    iDisplayPane->SetStyle(EAlfTextStyleTitle);
    iDisplayPane->SetColor(KFontColor);    


    // Enable brushed for borders etc
    iDisplayPane->EnableBrushesL();      

    // Display rectangular 
    CAlfShadowBorderBrush* brush = CAlfShadowBorderBrush::NewLC(Env(),6);
    brush->SetOpacity(.25f);
    
    // Append brush to the visual and transfer its ownership to the visual.    
    iDisplayPane->Brushes()->AppendL(brush, EAlfHasOwnership);
    CleanupStack::Pop(brush);    
    }

// ---------------------------------------------------------------------------
// Adds content into dialer pane of the application
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::AddDialerL()
    {
    // Dialer consists of three images on top of each other. Deck is needed to manage them
    // Parent layout is grid
    CAlfDeckLayout* dialerDeck = CAlfDeckLayout::AddNewL(*this, iRootLayout);
    dialerDeck->SetTagL(_L8(KTagArray[ETagAnalogDialerPlateDeck]));
        
	// Load images:    
    // 1) Image with the numbers
    CAlfTexture& numbersTexture = Env().TextureManager().LoadTextureL( KImageAnalogDialerNumbers, 
                                                                        EAlfTextureFlagDefault, 
                                                                        KAlfAutoGeneratedTextureId);

    // Parent layout is deck
	CAlfImageVisual* numbers  = CAlfImageVisual::AddNewL(*this,dialerDeck); 
    // Default scale mode CAlfImageVisual::EScaleFit is OK
	// attach texture to image visual
	numbers->SetImage(TAlfImage(numbersTexture));
	
	// 2) plate with holes
	CAlfTexture& plateTexture = Env().TextureManager().LoadTextureL(  KImageAnalogDialerPlate, 
	                                                            EAlfTextureFlagDefault, 
	                                                            KAlfAutoGeneratedTextureId);
    // Parent layout is deck
	iPlate = CAlfImageVisual::AddNewL(*this,dialerDeck); 
    // Default scale mode CAlfImageVisual::EScaleFit is OK
	iPlate->SetImage(TAlfImage(plateTexture));

	// 2) stopper
	CAlfTexture& stopper = Env().TextureManager().LoadTextureL( KImageAnalogDialerStopper, 
	                                                            EAlfTextureFlagDefault, 
	                                                            KAlfAutoGeneratedTextureId);
    // Parent layout is deck
	iStopper  = CAlfImageVisual::AddNewL(*this,dialerDeck); 
    // Default scale mode CAlfImageVisual::EScaleFit is OK
	iStopper->SetImage(TAlfImage(stopper));
    
    }

// ---------------------------------------------------------------------------
// Events from framework
// ---------------------------------------------------------------------------
//
TBool CAlfExAnalogDialerControl::OfferEventL(const TAlfEvent& aEvent)
    {
    // Ignore other events except pointer events
    if (aEvent.IsPointerEvent() && aEvent.PointerDown())
	    {
	    return HandlePointerDownEventL(aEvent);    
	    }
	 
	else if(aEvent.IsPointerEvent() && aEvent.PointerEvent().iType == TPointerEvent::EDrag )
	    {
	    return HandleDraggingEventL(aEvent);
	    }
	
	else if(aEvent.IsPointerEvent() && aEvent.PointerUp())
	    {
	    return HandlePointerUpEventL(aEvent);
	    }

	else if(aEvent.IsPointerEvent() && aEvent.PointerLongTap())
	    {
	    return HandlePointerLongTapEventL(aEvent);
	    }
	    
	return EFalse;
	}

// ---------------------------------------------------------------------------
// Handles pointer down events
// ---------------------------------------------------------------------------
//
TBool CAlfExAnalogDialerControl::HandlePointerDownEventL(const TAlfEvent& aEvent)
	{
	// Topmost image i.e. stopper image visual gets the pointer event
	CAlfVisual* onVisual = aEvent.Visual();
	if (onVisual == iStopper)
	    {      
	    // Is plate rotating starting?
        if (IsPointerEventFollowingNumbersArcPath(aEvent))
            {
    		ResetDraggingData();

            // Plate rotation is starting
    	    // Register to receive dragging events
    	    Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportDrag, *this);   

            // Dragging has started
    	    iFlags |= EAnalogDialerControlDragging;
            }
        // Is clear key pressed?
        else if (IsPointerEventOnClearKey(aEvent))
            {
    		ResetClearKeyData();
    	        
            if (!(iFlags & EAnalogDialerControlFlagLongTapRegistered))
                {
                // Clear key handling needs long tap events
    	        // Register for long tap events
                Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportLongTap, *this);   
                iFlags |= EAnalogDialerControlFlagLongTapRegistered;
                }
            // Clear key event has started
    	    iFlags |= EAnalogDialerControlClearKeyEvent;
            }
	    }
	return ETrue;
	}

// ---------------------------------------------------------------------------
// Handles dragging events
// ---------------------------------------------------------------------------
//
TBool CAlfExAnalogDialerControl::HandleDraggingEventL(const TAlfEvent& aEvent)
	{
    if (iFlags & EAnalogDialerControlDragging)
        {
    	TPoint plateCentricPoint;
    	ConvertEventToPlateCentricPosition(aEvent,
    	                                   iPlateTopOffset,
    	                                   iSquareSide,
    	                                   plateCentricPoint);
    	TInt plateRadius = iSquareSide / 2;
    	if (!IsPointerEventOnArcPath(plateCentricPoint,
    	                             plateRadius,
    	                             plateRadius * KPlateClearRadius / 100))
    		{
    		FinishDragging();
    		}
        else
            {
            // Pointer event is following path
            // Get angle of pointer event and do further calculation based on it
            TInt degrees = Angle(plateCentricPoint.iX, plateCentricPoint.iY);

            // Save the angle of first dragging pointer event.
            // It is used to resolve selected number.
        	if (iDraggingData.iInitialAngle == 0)
        	    {
        	    iDraggingData.iInitialAngle = degrees; 
        	    iDraggingData.iPreviousAngle = degrees;
        	    }
        	        
            // As dragging events occur very often, redrawing plate is optimised that
            // it is not drawn after every dragging event. Change of KPlateDraggingDelta degrees
            // is needed until plate is redrawn.
            if (    degrees >= iDraggingData.iPreviousAngle + KPlateDraggingDelta 
                ||  degrees <= iDraggingData.iPreviousAngle - KPlateDraggingDelta)
                {
                ResolveXAxisCrossing(degrees);                     
                iDraggingData.iPreviousAngle = degrees;
                            
                if (    iDraggingData.iIsClockWiseOverZeroReached
                    &&  degrees < KStopperAngle)
                    {
                    if (iDraggingData.iStopperReached)
                        {
                        // do not redraw as plate cannot rotate more
                        return ETrue;
                        }
                    // stopper has been reached
                    iDraggingData.iStopperReached = ETrue;
                    }
                else if (   !iDraggingData.iIsClockWiseOverZeroReached
                        &&  degrees > iDraggingData.iInitialAngle)
                    {
                    // Rotation counter clockwise to the left of idle position is asked
                    // Plate does not rotate counterclockwise to the left of idle position
                    return ETrue;
                    }
                else if (iDraggingData.iStopperReached)
                    {                        
                    // user has dragged 360 degrees and is dragging now between idle and stopper position
                    return ETrue;
                    }
                 else
                    {
                    // rotating when angle is between idle and stopper position
                    iDraggingData.iStopperReached = EFalse;
                    }
                //DebugDraggingData(degrees);
                RotatePlate(    *iPlate,
                                iDraggingData.iInitialAngle,
                                degrees,
                                iDraggingData.iIsClockWiseOverZeroReached);
                }
            // else - delta of rotation has not reached KPlateDraggingDelta
            }
        }
    // else - it is permitted that clear key event is dragged outside clear key area. What matters is where
    // the pointer event up takes place
    return ETrue;
	}

// ---------------------------------------------------------------------------
// Handle pointer up event
// ---------------------------------------------------------------------------
//
TBool CAlfExAnalogDialerControl::HandlePointerUpEventL(const TAlfEvent& aEvent)
	{
	if (iFlags & EAnalogDialerControlDragging)
		{
		// Plate rotating has ended. Process result.
		FinishDragging();
		}
    else if (iFlags & EAnalogDialerControlClearKeyEvent)
        {
		// clear key event has ended
        if (IsPointerEventOnClearKey(aEvent))
            {
            ClearNumberDisplay(iFlags&EAnalogDialerControlFlagLongTapPressed);
            }
        // else - lifting pointer outside Clear key area cancels clearing number
        ResetClearKeyData();
        }
    return ETrue;
	}

// ---------------------------------------------------------------------------
// Handle long tap event
// ---------------------------------------------------------------------------
//
TBool CAlfExAnalogDialerControl::HandlePointerLongTapEventL(const TAlfEvent& /*aEvent*/)
    {
    if (iFlags & EAnalogDialerControlClearKeyEvent)
        {
        iFlags |= EAnalogDialerControlFlagLongTapPressed;
        }
    return ETrue;  
    }

// ---------------------------------------------------------------------------
// AddNumberToDisplayPaneL
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::AddNumberToDisplayPaneL(TInt aNumber)
	{	
	if ( aNumber > 9 )
	    {
	    return;
	    }
    TBuf<8> number;        
    
    // aNumber is 0...9
    // Second parameter: number 10 must be changed to 0
    number.Format(KNumberDisplayTemplate, (aNumber+1)%10);
    if (iDisplayNumber.Length() + number.Length() <= KNumberDisplayBufferLength)
    	{
    	iDisplayNumber.Append(number);    
    	iDisplayPane->SetTextL(iDisplayNumber);
    	}
	}
	

// ---------------------------------------------------------------------------
// SwitchLayoutL
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::SwitchLayoutL()
    {
    // Get access to first leafs (visuals) of grid layout
    CAlfVisual* displayDeck = FindTag(_L8( KTagArray[ETagAnalogDialerDisplayDeck]));
    CAlfVisual* dialerDeck = FindTag(_L8( KTagArray[ETagAnalogDialerPlateDeck]));
    
    if (displayDeck && dialerDeck)
        {
        // remove visuals from grid layout
        iRootLayout->Remove(displayDeck);
        iRootLayout->Remove(dialerDeck);
 
        // resolve new layout
        iLayoutMode = ResolveLayout(iSquareSide,
                                    iLongerSide);
        
        // modify grid layout
        AddGridLayoutL();
        
        // put visuals back to layout
        if (iLayoutMode == ELayoutAnalogDialerPortrait)
            {
            User::LeaveIfError(iRootLayout->Append(displayDeck));
            User::LeaveIfError(iRootLayout->Append(dialerDeck));
            }
        else
            {
            User::LeaveIfError(iRootLayout->Append(dialerDeck));
            User::LeaveIfError(iRootLayout->Append(displayDeck));
            }
            
        // Notify Alf about layout change
        Env().NotifyLayoutChangedL();
        }
    // else - better not to do anything

    iFeedback->Start();

    // expose new root layout
    /*TAlfTimedValue opacity(0.0f);
    opacity.SetTarget(255, 500);
    iRootLayout->SetOpacity(opacity);   */

    }

// ---------------------------------------------------------------------------
// Hides the visual
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::PrepareForLayoutSwitchL()
    {
    /*CAlfVisual* layout = FindTag(_L8(KTagArray[ETagAnalogDialerRootLayout]));
    if ( layout )
        {
        TAlfTimedValue opacity(1.0f);
        opacity.SetTarget( 0.0f, 500);
        layout->SetOpacity(opacity);   
        }
    iFeedback->Start();
    User::After(500000);*/
    }


// ---------------------------------------------------------------------------
// Resolves current layout.
// ---------------------------------------------------------------------------
// 
CAlfExAnalogDialerControl::TLayoutMode CAlfExAnalogDialerControl::ResolveLayout(
                                            TInt& aShorterSide,
                                            TInt& aLongerSide) const
    {
    // Resolve whether application is now running in portrait or landscape mode
    // Function must also return the shorter extent of the application 
    // to be used as side length of the square of dialer plate.
    // Longer side is returned also to be used in grid weights.
    TLayoutMode layoutMode = ELayoutAnalogDialerPortrait;
    TRect displayRect = Env().PrimaryDisplay().VisibleArea();
    TInt height = displayRect.Height();
    TInt width = displayRect.Width();
    if (width > height)
        {
        layoutMode = ELayoutAnalogDialerLandscape;
        aShorterSide = height;
        aLongerSide = width;
        }
    else
        {
        // default mode is OK
        aShorterSide = width;
        aLongerSide = height;
        }
    return layoutMode;
    }
 
// ---------------------------------------------------------------------------
// ResolveXAxisCrossing
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerControl::ResolveXAxisCrossing(TInt aAngle)
    {   
    // Delta of angle change, which indicates X-axis crossing
    const TInt KXAxisCrossingAngleDelta = 300;

    TInt degreesDelta = aAngle - iDraggingData.iPreviousAngle;
    // switch from 0 to 359?
    if (degreesDelta > KXAxisCrossingAngleDelta)
        {
        iDraggingData.iIsClockWiseOverZeroReached = ETrue;
        }
    // switch from 359 to 0?
    else if (   iDraggingData.iIsClockWiseOverZeroReached
            &&  degreesDelta < (-KXAxisCrossingAngleDelta))
        {
        iDraggingData.iIsClockWiseOverZeroReached = EFalse;
        }    
    }


// ---------------------------------------------------------------------------
// Calculate angle of triangle, when adjacent and opposite sides are known
// ---------------------------------------------------------------------------
// 
TInt CAlfExAnalogDialerControl::Angle(TReal32 aAdjacent, TReal32 aOpposite)
	{
	if (aAdjacent == 0)
	    {
	    aAdjacent = 0.01;
	    }
	
	if (aOpposite == 0)
		{
		aOpposite = 0.01;
		}

	TReal angle;
	Math::ATan(angle, Abs(aOpposite / aAdjacent));
	
	if (aAdjacent < 0 && aOpposite > 0)
		{
		angle =  KPi - angle;
		}
	else if (aAdjacent < 0 && aOpposite < 0)
		{
		angle = KPi + angle; 
		}
    else if (aAdjacent > 0 && aOpposite < 0)
		{
		angle = 2* KPi -  angle;
		}
    return RADTODEG(angle);			
	}

// ---------------------------------------------------------------------------
// Calculate hypotenuse of triangle when length of adjacent and opposite sides are known
// ---------------------------------------------------------------------------
// 
TInt CAlfExAnalogDialerControl::Hypotenuse(const TPoint& aPosition)
	{
	TInt sideLength = aPosition.iX * aPosition.iX;
	TInt oppositeLength = aPosition.iY * aPosition.iY;
	
	TReal hypotenuse;
	Math::Sqrt(hypotenuse, sideLength + oppositeLength);
	return hypotenuse;
	}
	

// ---------------------------------------------------------------------------
// Order of function calls fix the layout of grid in portrait and landscape modes.
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::SetWeightsOfGridL(  CAlfGridLayout& aGridLayout,
                                                    TLayoutMode     aLayout,
                                                    TInt            aSquareSide,
                                                    TInt            aLongerSide,
                                                    TInt&           aPlateTop)
    {
    // Set up a variable for default value.
    RArray<TInt> weightsDefault;
    User::LeaveIfError(weightsDefault.Append(1));   

    // sets weights of grid. Use pizels of application window size to get exact adjustment of pane sizes
    RArray<TInt> weights;
    if (aLayout == ELayoutAnalogDialerPortrait)
        {
        // top: display; take rest what dialer plate leaves for number display
        User::LeaveIfError(weights.Append(aLongerSide-aSquareSide));

        // bottom: dialer, which must be square
        User::LeaveIfError(weights.Append(aSquareSide));
        aGridLayout.SetRowsL(weights);
        
        // default weight for dimension, which is 1. Needed, because weights are reset
        aGridLayout.SetColumnsL(weightsDefault);
        
        // Offset to the plate in Y-dimension is height of number display
        aPlateTop = aLongerSide-aSquareSide;
        }
    else
        {
        // left: dialer, which must be square
        User::LeaveIfError(weights.Append(aSquareSide));
        // right: display; take rest what dialer plate leaves for number display
        User::LeaveIfError(weights.Append(aLongerSide-aSquareSide));

        // default weight for dimension, which is 1. Needed, because weights are reset
        aGridLayout.SetRowsL(weightsDefault);
        aGridLayout.SetColumnsL(weights);
        
        aPlateTop = 0;
        }         
    weights.Close(); 
    weightsDefault.Close(); 
    }
    
    
// ---------------------------------------------------------------------------
// ConvertEventToPlateCentricPosition
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::ConvertEventToPlateCentricPosition(
                                        const TAlfEvent&    aEvent,
	                                    TInt                aPlateTopOffset,
	                                    TInt                aSquareSide,
	                                    TPoint&             aPlateCentricPoint)
    {
    // event position is converted in a such way that centre position of the plate is (0,0)
    // iY: value is negative, if position is below the centre position
    // iY: value is position, if position is above the centre position
    // iX: value is negative, if position is to the left the centre position
    // iX: value is position, if position is to the right of the centre position

    // aSquare side is length of whole square side
	const TInt plateCentreY = aPlateTopOffset + aSquareSide / 2;
	aPlateCentricPoint.iX = aEvent.PointerEvent().iParentPosition.iX - aSquareSide / 2;
	aPlateCentricPoint.iY = plateCentreY - aEvent.PointerEvent().iParentPosition.iY ;
    }
    
    
// ---------------------------------------------------------------------------
// Checks whether pointer event occurred between outer and inner radius
// ---------------------------------------------------------------------------
// 
TBool CAlfExAnalogDialerControl::IsPointerEventOnArcPath(   TPoint& aPlateCentricPoint,
	                                                        TInt    aOuterRadius,
	                                                        TInt    aInnerRadius)
    {  
    // Checks whether position is along a path, which is between radii.
    // Event may be exactly on distance of radii
    TBool ret = ETrue;   
	TInt distanceFromPlateCentre = Hypotenuse(aPlateCentricPoint); 	
	if (distanceFromPlateCentre > aOuterRadius || distanceFromPlateCentre < aInnerRadius)
	    {
	    ret = EFalse;
	    }
	return ret;
    }
    
// ---------------------------------------------------------------------------
// Dragging has finished. Resets dragging data
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::ResetDraggingData()
    {
	iFlags &= (~EAnalogDialerControlDragging);
	iDraggingData.iInitialAngle = 0;
	iDraggingData.iPreviousAngle = 0;
	iDraggingData.iIsClockWiseOverZeroReached = EFalse;
	iDraggingData.iStopperReached=EFalse;
    }

// ---------------------------------------------------------------------------
// Clear key event has finished. Resets dragging data
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::ResetClearKeyData()
    {
	iFlags &= (~EAnalogDialerControlClearKeyEvent);
    iFlags &= (~EAnalogDialerControlFlagLongTapPressed);
    }


// ---------------------------------------------------------------------------
// Turns plate back to idle position i.e. counter clokcwise
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::RotatePlateToIdlePosition(CAlfImageVisual&    aImage)
    {
	// Rotate the image. This has immediate effect.
	TAlfTimedValue tValue;
	tValue.SetTargetWithSpeed(0,  // position zero 
	                          KPlateReturnSpeed);
	aImage.SetTurnAngle(tValue);
    }
    
    
// ---------------------------------------------------------------------------
// Rotates image clockwise
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::RotatePlate(
                                    CAlfImageVisual&    aImage,
                                    TInt                aInitialAngle,
                                    TInt                aTargetAngle,
                                    TBool               aIsClockWiseOverZeroReached)
    {
    // When rotates clockwise, rotation angle is negative
    TInt rotateDelta = aTargetAngle-aInitialAngle;
    if (aIsClockWiseOverZeroReached)
        {
        // Angle is big value (near to 360), which cannot be used as such
        // Convert it to negative value
        rotateDelta = (aTargetAngle-360)-aInitialAngle;
        }
    //RDebug::Printf("***AnalogDialer rotateDelta : %d",rotateDelta);
    TAlfTimedValue tValue(rotateDelta);
    aImage.SetTurnAngle(tValue);
    }
    
// ---------------------------------------------------------------------------
// Returns selected number based on rotated angle of plate 
// ---------------------------------------------------------------------------
// 
TInt CAlfExAnalogDialerControl::DialedNumber(   TInt aInitialAngle,
                                                TInt aCurrentAngle) const
    {
    // Default value when dragging event has not crossed zero-angle X-axis
    TInt deltaAngle = aCurrentAngle-aInitialAngle;
    
    // aCurrentAngle must be an angle derived directly from ConvertEventToPlateCentricPosition
    // aCurrentAngle is really current pointer position. KStopper value must be used instead
    if (iDraggingData.iIsClockWiseOverZeroReached)
        {
        deltaAngle = aInitialAngle+(360-KStopperAngle);
        }
    
    // Subtract required minimun angle. 
    deltaAngle = Abs(deltaAngle)-KPlateMinimumRotateAngle;
    if (deltaAngle<0)
        {
        // plate has not been rotated enough to select any number
        return KErrNotFound;
        }
        
    // TODO. These values could be made constant to use plate with other number systems as decimal
    // e.g. binary, or hexadecimal
    // Finally calculate dialed number based on rotated angle
    TInt angleOfANumber(KPlateNumbersDistributionAngle/10);
    TInt dialedNumber = deltaAngle/angleOfANumber;
    if (dialedNumber > 9)
        {
        dialedNumber = 9;
        }
    return dialedNumber;
    }


// ---------------------------------------------------------------------------
// Checks whether pointer is on path allowed for plate rotation
// The path is between edges of plate and clear key area
// ---------------------------------------------------------------------------
// 
TBool CAlfExAnalogDialerControl::IsPointerEventFollowingNumbersArcPath( const TAlfEvent& aEvent ) const
    {
	TPoint plateCentricPoint;
	ConvertEventToPlateCentricPosition(aEvent,
	                                    iPlateTopOffset,
	                                    iSquareSide,
	                                    plateCentricPoint);
	// Third parameter: Radius of clear area belongs to clear area. Eliminate it.
	TInt plateRadius = iSquareSide / 2;
	if ( IsPointerEventOnArcPath( plateCentricPoint,
	                              plateRadius,
	                              (plateRadius*KPlateClearRadius/100)+1))
	    {
	    return ETrue;
	    }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// Checks whether pointer is on area of clear key
// ---------------------------------------------------------------------------
// 
TBool CAlfExAnalogDialerControl::IsPointerEventOnClearKey( const TAlfEvent& aEvent ) const
    {
	TPoint plateCentricPoint;
	ConvertEventToPlateCentricPosition(aEvent,
	                                    iPlateTopOffset,
	                                    iSquareSide,
	                                    plateCentricPoint);
	TInt plateRadius = iSquareSide / 2;
	// Third parameter: The 'bull eye' of the plate
	if ( IsPointerEventOnArcPath( plateCentricPoint,
	                              plateRadius*KPlateClearRadius/100,
	                              0))
	    {
	    return ETrue;
	    }
    return EFalse;
    }
    

// ---------------------------------------------------------------------------
// Performs actions needed when dragging is finished
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::FinishDragging()
    {
	// Dragging has finished
	// Remove pointer dragging observer, which will gain us performance
    Display()->Roster().RemovePointerEventObserver( EAlfPointerEventReportDrag,*this );
    DialedNumberToDisplay(  iDraggingData.iInitialAngle,
                            iDraggingData.iPreviousAngle);
    RotatePlateToIdlePosition(*iPlate); 
    ResetDraggingData();
    }
    
// ---------------------------------------------------------------------------
// Puts dialed number into number display
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::DialedNumberToDisplay(  TInt aInitialAngle,
                                                        TInt aCurrentAngle )
    {
    TInt dialedNumber = DialedNumber(aInitialAngle, aCurrentAngle);
    // Filter KErrNotFound out
    if (dialedNumber>=0)
        {
        TRAP_IGNORE(AddNumberToDisplayPaneL( dialedNumber ));
        }
    }

// ---------------------------------------------------------------------------
// Clear number display either wholly or last number
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::ClearNumberDisplay(TBool aClearWholeDisplay)
    {
    TInt lengthNumber = iDisplayNumber.Length();
    if (lengthNumber == 0)
        {
        // nothing to clear
        return;
        }

    if (aClearWholeDisplay)
        {
        // clear whole buf
        iDisplayNumber.Zero();
        }
    else
        {
        // delete one number
        iDisplayNumber.Delete(lengthNumber-1,1);
        }
    // changed number to the screen
    TRAP_IGNORE(iDisplayPane->SetTextL(iDisplayNumber));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
/*void CAlfExAnalogDialerControl::AddEffectLayout()
	{
	
	}*/


// ---------------------------------------------------------------------------
// Debugs dragging data + given angle 
// ---------------------------------------------------------------------------
// 
void CAlfExAnalogDialerControl::DebugDraggingData(TInt aAngle)
    {
    RDebug::Printf("\n***AnalogDialer:");
    RDebug::Printf("***AnalogDialer iFlags & EAnalogDialerControlDragging: %d",iFlags & EAnalogDialerControlDragging);
    RDebug::Printf("***AnalogDialer iDraggingData.iInitialAngle: %d",iDraggingData.iInitialAngle);
    RDebug::Printf("***AnalogDialer iDraggingData.iPreviousAngle: %d",iDraggingData.iPreviousAngle);
    RDebug::Printf("***AnalogDialer iDraggingData.iIsClockWiseOverZeroReached: %d",iDraggingData.iIsClockWiseOverZeroReached);
    RDebug::Printf("***AnalogDialer iDraggingData.iStopperReached: %d",iDraggingData.iStopperReached);
    RDebug::Printf("***AnalogDialer aAngle: %d",aAngle);
    RDebug::Printf("\n");
    }

