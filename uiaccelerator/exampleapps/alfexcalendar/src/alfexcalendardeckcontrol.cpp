/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include <alf/alftextvisual.h>
#include <alf/alfgridlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfcurvepath.h>
#include <alf/alfevent.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alflinevisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alftransformation.h>
#include <alf/alfcommand.h>

#include "alfexcalendardeckcontrol.h"
#include "alfexcalendarmonth.h"
#include "alfexcalendarengine.h"

_LIT( KImagePath, "c:\\data\\Images\\Pictures\\" );
_LIT( KCalendar, "calendar.jpg" );

const TInt KMonthViewTransitTime = 1500;
const TInt KMonthViewRotationTime = 1500;
const TInt KMonthViewScalingTime = 2500;
const TInt KMonthViewOpacityTime = 2500;

const TReal KUnitSize = 1.0f;
const TReal KMonthXSf = 0.2f;
const TReal KMonthYSf = 0.2f;

// Custome events
enum TCustomEvent
{
    EMoveMonthToBack = 1,
    EMonthMovedToFront,

    ECustomEventInvalid
};

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::NewLC
// --------------------------------------------------------------------------
//
CAlfExCalendarDeckControl* CAlfExCalendarDeckControl::NewLC(
    CAlfEnv& aEnv,
    CAlfExCalendarEngine& aCalendarEngine )
    {
    CAlfExCalendarDeckControl* self =
        new (ELeave) CAlfExCalendarDeckControl( aCalendarEngine, aEnv );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    return self;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::CAlfExCalendarDeckControl
// --------------------------------------------------------------------------
//
CAlfExCalendarDeckControl::CAlfExCalendarDeckControl(
        CAlfExCalendarEngine& aCalendarEngine,
        CAlfEnv& aEnv )
    : iFocusedMonth( (KMonthsVisible-1) ), iMonthView( EFalse ),
      iCalendarEngine( aCalendarEngine ), iEnv( aEnv ),
      iMonthAnimationStyle( EAnimationStyle1 ),
      iMonthTransitionState( ETransitionStateStopped )
    {
    }


// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::ConstructL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::ConstructL( CAlfEnv& aEnv )
    {
    CAlfControl::ConstructL( aEnv );
    
    /* // not using currently
    // Background image.
    iBackground = CAlfImageVisual::AddNewL(*this);
    Env().TextureManager().SetImagePathL( KImagePath );
    SetImageTexture( iBackground, KCalendar );
    */

    // Get display rectangle
    iDisplayRect = aEnv.PrimaryDisplay().VisibleArea();

    // create Deck Layout and add to CAlfExCalendarDeckControl
    // CAlfExCalendarDeckControl takes ownership of created layout
    iDeck = CAlfDeckLayout::AddNewL( *this );

    
    // Calendar logic here
    // we create only KMonthsVisible number (atleast 2 :))of CAlfExCalendarMonth instances
    // and only use those many, to show un-limited calendar months.
    // take the responsibility of updating CAlfExCalendarMonth instances 
    // when moving between the months.
    iSelectedTime.HomeTime();
    iCurrentMonthControlIndex = (KMonthsVisible - 1);
    
    TTime tmpTime = iSelectedTime - (TTimeIntervalMonths)( KMonthsVisible - 1 );
    for( TInt i = 0; i < KMonthsVisible; i++ )
        {
        iMonthControls[i] = CAlfExCalendarMonth::NewLC( iDeck, tmpTime.DateTime(), aEnv, *this );
        tmpTime += (TTimeIntervalMonths)1;
        CleanupStack::Pop();
        };

    // move Focus to current month
    iMonthControls[iCurrentMonthControlIndex]->MonthFocusedL();
    iMonthView = ETrue;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::~CAlfExCalendarDeckControl
// --------------------------------------------------------------------------
//
CAlfExCalendarDeckControl::~CAlfExCalendarDeckControl()
    {
    // delete all iMonthControls
    for (TInt i = 0; i < KMonthsVisible ; i++ )
        {
        delete iMonthControls[i];
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::SetImageTexture
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::SetImageTexture(
    CAlfImageVisual* aImageVisual,
    const TDesC& aImageFileName )
    {
    
    TRAP_IGNORE(
            // creates texture from given aImageFileName
            CAlfTexture& texture =
                Env().TextureManager().LoadTextureL(
                        aImageFileName,
                        EAlfTextureFlagDefault,
                        KAlfAutoGeneratedTextureId );

            aImageVisual->SetScaleMode( CAlfImageVisual::EScaleCover );
            
            // sets created texture to given aImageVisual
            aImageVisual->SetImage( TAlfImage( texture ) );
                        );
    
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::SetMonthView
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::SetMonthView( TBool aVal )
    {
    iMonthView = aVal;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::CalendarEngine
// --------------------------------------------------------------------------
//
CAlfExCalendarEngine& CAlfExCalendarDeckControl::CalendarEngine() const
    {
    return iCalendarEngine;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::OfferEventL
// --------------------------------------------------------------------------
//
TBool CAlfExCalendarDeckControl::OfferEventL(const TAlfEvent& aEvent )
	{
    // DO NOT change the order of below aEvent checking
    
    // Pointer events are irrelevant for 3.2,
    // but caught here for 5.0 compatibility.
    if( aEvent.IsPointerEvent() )
        {
        return ETrue;
        }

    if( aEvent.IsCustomEvent() /* && iMonthTransitionState != ETransitionStateStopped */ )
        {
        switch( aEvent.CustomParameter() )
            {
            // month transition (animation)is completed. Handle custome events
            case EMoveMonthToBack:
                iDeck->MoveVisualToBack( iDeck->Visual( iFocusedMonth ) );
                iMonthTransitionState = ETransitionStateStopped;
                iMonthControls[iCurrentMonthControlIndex]->MonthFocusedL();
                iMonthView = ETrue;
                return ETrue;

            case EMonthMovedToFront:
                iMonthTransitionState = ETransitionStateStopped;
                iMonthControls[iCurrentMonthControlIndex]->MonthFocusedL();
                iMonthView = ETrue;
                return ETrue;
            
            default:
                break;                
            }
        }

    // Don't handle events while month transition
    if( iMonthTransitionState != ETransitionStateStopped )
        {
        return ETrue;
        }
    
    // If month view
    if( iMonthView )
        {
        TBool ret = iMonthControls[iCurrentMonthControlIndex]->OfferEventL( aEvent );
        
        // iMonthControls can change the iMonthView value. 
        // month might have been closed
        if( iMonthView )
            {
            return ret;
            }
        else
            {
            // dont return, continue event processing here
            }
        }

    if ( aEvent.Code() == EEventKey )
        {
        iMonthAnimationStyle = EAnimationStyle1;
        TInt code = aEvent.KeyEvent().iCode;
        switch( code )
            {
            case EKeyUpArrow:
                iMonthAnimationStyle = EAnimationStyle2;
                MoveToPrevMonthL();
                return ETrue;

            case EKeyDownArrow:
                iMonthAnimationStyle = EAnimationStyle2;
                MoveToNextMonthL();
                return ETrue;
            
            case EKeyLeftArrow:
                MoveToPrevMonthL();
                return ETrue;

            case EKeyRightArrow:
                MoveToNextMonthL();
                return ETrue;

            default:
                break;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToNextMonthL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToNextMonthL()
    {
    // calls appropriate function according to animation style
    switch( iMonthAnimationStyle )
        {
        case EAnimationStyle1:
            MoveToNextMonthAnimation1L();
            break;
        case EAnimationStyle2:
            MoveToNextMonthAnimation2L();
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToPrevMonthL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToPrevMonthL()
    {
    // calls appropriate function according to animation style
    switch( iMonthAnimationStyle )
        {
        case EAnimationStyle1:
            MoveToPrevMonthAnimation1L();
            break;
        case EAnimationStyle2:
            MoveToPrevMonthAnimation2L();
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::UpdateNextMonthL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::UpdateNextMonthL()
    {
    // change time to next month
    iSelectedTime += (TTimeIntervalMonths)1;
    
    // set iCurrentMonthControlIndex value properly.
    // some logic here based on how many no of month layout we have
    iCurrentMonthControlIndex++;
    if( iCurrentMonthControlIndex == KMonthsVisible )
        {
        iCurrentMonthControlIndex = 0;
        }
    
    // now update the month layout according to iSelectedTime
    iMonthControls[iCurrentMonthControlIndex]->UpdateMonthL( iSelectedTime.DateTime() );    
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::UpdatePrevMonthL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::UpdatePrevMonthL()
    {
    // change time to previous month
    iSelectedTime -= (TTimeIntervalMonths)1;
    
    // set iCurrentMonthControlIndex value properly.
    iCurrentMonthControlIndex--;
    if( iCurrentMonthControlIndex == -1 )
        {
        iCurrentMonthControlIndex = (KMonthsVisible-1);
        }

    // update the month layout according to iSelectedTime
    iMonthControls[iCurrentMonthControlIndex]->UpdateMonthL( iSelectedTime.DateTime() );    
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToNextMonthAnimation1L
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToNextMonthAnimation1L()
    {
    // updates the month layout which is at bottom of deck layout
    UpdateNextMonthL();

    // get the bottom most month to front of DECK layout
    iDeck->MoveVisualToFront( iDeck->Visual( 0 ) );
    
    // move this month to extreme right of display
    iDeck->Visual( iFocusedMonth ).SetPos( TPoint( iDisplayRect.Width(), 0 ), 0 );
    
    // now move this month to 0,0 position in KMonthViewTransitTime time
    iDeck->Visual( iFocusedMonth ).SetPos( TPoint( 0, 0 ), KMonthViewTransitTime );
    
    // with rotation transformation
    RotateVisualL( iDeck->Visual( iFocusedMonth ), 360, 0, KMonthViewRotationTime );

    // send custome event, handle EMonthMovedToFront in OfferEventL() after KMonthViewTransitTime
    Env().Send( TAlfCustomEventCommand( EMonthMovedToFront, this ), KMonthViewTransitTime );
    
    // change iMonthTransitionState state
    iMonthTransitionState = ETransitionStateMovingToNext;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToPrevMonthAnimation1L
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToPrevMonthAnimation1L()
    {
    // updates the month layout which is just below the top of deck layout
    UpdatePrevMonthL();

    // move currently focused month to out of screen horizontally
    iDeck->Visual( iFocusedMonth ).SetPos( TPoint( iDisplayRect.Width(), 0 ), KMonthViewTransitTime );
    
    // with rotation transformation
    RotateVisualL( iDeck->Visual( iFocusedMonth ), 0, 360, KMonthViewRotationTime );

    // move month to back of DECK after KMonthViewTransitTime duration by sending custom event
    // custom event EMoveMonthToBack should be handled in OfferEventL()
    Env().Send( TAlfCustomEventCommand( EMoveMonthToBack, this ), KMonthViewTransitTime );
    
    // change iMonthTransitionState state
    iMonthTransitionState = ETransitionStateMovingToPrev;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToNextMonthAnimation2L
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToNextMonthAnimation2L()
    {
    // updates the month layout which is at bottom of deck layout
    UpdateNextMonthL();

    // get the bottom most month to front of DECK
    iDeck->MoveVisualToFront( iDeck->Visual( 0 ) );

    // position it at bottom of display
    iDeck->Visual( iFocusedMonth ).SetPos( TPoint( 0, iDisplayRect.Height() ), 0 );

    // ans move it to 0,0 position (top left of display)
    iDeck->Visual( iFocusedMonth ).SetPos( TPoint( 0, 0 ), KMonthViewTransitTime );

    // send custom event and handle it in OfferEventL()
    Env().Send( TAlfCustomEventCommand( EMonthMovedToFront, this ), KMonthViewTransitTime );

    // change iMonthTransitionState state
    iMonthTransitionState = ETransitionStateMovingToNext;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::MoveToPrevMonthAnimation2L
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::MoveToPrevMonthAnimation2L()
    {
    // updates the month layout which is just below (2nd one)
    // from the top of deck layout
    UpdatePrevMonthL();

    // move currently visible month (which is on top of layout)
    // to bottom of didplay
    iDeck->Visual( iFocusedMonth ).SetPos(
            TPoint( 0, iDisplayRect.Height() ), KMonthViewTransitTime );

    // then move month to back of DECK after KMonthViewTransitTime duration
    Env().Send(
            TAlfCustomEventCommand( EMoveMonthToBack, this ),
            KMonthViewTransitTime );
    
    // change iMonthTransitionState state
    iMonthTransitionState = ETransitionStateMovingToPrev;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::ScaleVisualL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::ScaleVisualL(
        CAlfVisual& aVisual,
        TReal aXSf,
        TReal aYSf,
        TInt aTime )
    {
    aVisual.EnableTransformationL();
    TAlfTimedValue xSc( KUnitSize );
    xSc.SetTarget( aXSf, aTime );
    TAlfTimedValue ySc( KUnitSize );
    ySc.SetTarget( aYSf, aTime );
    aVisual.Transformation().Scale( xSc, ySc );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::RotateVisualL
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::RotateVisualL(
        CAlfVisual& aVisual,
        TReal aInitAngle,
        TReal aTargetAngle,
        TInt aTime )
    {
    aVisual.EnableTransformationL();
    TAlfTimedValue angle( aInitAngle );
    angle.SetTarget( aTargetAngle, aTime );
    aVisual.Transformation().Rotate( angle , 0.0, 0.0, 1.0 );
    }


// --------------------------------------------------------------------------
// CAlfExCalendarDeckControl::SetOpacity
// --------------------------------------------------------------------------
//
void CAlfExCalendarDeckControl::SetOpacity(
    CAlfVisual& aVisual,
    TReal aInitValue,
    TReal aTargetValue,
    TInt aTime )
    {
    TAlfTimedValue opacity;
    opacity.SetValueNow( aInitValue );
    opacity.SetTarget( aTargetValue, aTime ); // in milliseconds
    aVisual.SetOpacity( opacity );
    }


// end of file

