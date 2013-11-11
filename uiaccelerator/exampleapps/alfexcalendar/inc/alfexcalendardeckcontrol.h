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



#ifndef ALFEXCALENDARDECKCONTROL_H
#define ALFEXCALENDARDECKCONTROL_H

#include <alf/alfcontrol.h>

const TInt KMonthsVisible = 2; // DONT CHANGE

// Animation styles
enum TMonthAnimationStyle
{
    EAnimationStyle1 = 1,
    EAnimationStyle2,
    EAnimationStyle3,
    EAnimationStyleInvalid
};

// 
enum TMonthTransitionState
{
   ETransitionStateMovingToNext = 1,
   ETransitionStateMovingToPrev,
   ETransitionStateStopped
};

// Forward declarations
class CAlfGridLayout;
class CAlfDeckLayout;
class CAlfFlowLayout;
class CAlfCurvePath;
class CAlfCurvePathLayout;
class CAlfImageVisual;
class CAlfExCalendarMonth;
class CAlfExCalendarEngine;

// CAlfExCalendarDeckControl declaration

class CAlfExCalendarDeckControl : public CAlfControl
    {
public: // methods

    /**
     * 2 phase construction
     */
    static CAlfExCalendarDeckControl* NewLC( CAlfEnv& aEnv, CAlfExCalendarEngine& aCalendarEngine );

    /**
     * destructor
     */    
    ~CAlfExCalendarDeckControl();
    
    /**
     * Handles application events
     *
     * @param aEvent contains event data
     * @return true if event is handled successfully
     */    
    TBool OfferEventL( const TAlfEvent& aEvent );

    /**
     * Sets month view value
     *
     * @param aVal value to be set to iMonthView
     */
    void SetMonthView( TBool aVal );

    /**
     * returns CAlfExCalendarEngine object
     * @return CAlfExCalendarEngine object
     */    
    CAlfExCalendarEngine& CalendarEngine() const;
    
private: // Methods
    
    /**
     * constructor
     */
	CAlfExCalendarDeckControl( CAlfExCalendarEngine& aCalendarEngine, CAlfEnv& aEnv  );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL( CAlfEnv& aEnv );

    /**
     * Creates texture from aImageFileName and set it to aImageVisual
     *
     * @param aImageVisual created texture is added to this visual
     * @param aImageFileName file from which texture is created
     */
    void SetImageTexture(
            CAlfImageVisual* aImageVisual,
            const TDesC& aImageFileName );


    /**
     * updates the does transition to next month according to animation style
     */
    void UpdateNextMonthL();

    /**
     * does transition to next month according to animation style
     */
    void UpdatePrevMonthL();

    /**
     * does transition to next month according to animation style
     */
    void MoveToNextMonthL();

    /**
     * does transition to previous month according to animation style
     */
    void MoveToPrevMonthL();

    /**
     * Implements transition to next month with animation style 1
     * Gets the month from right of display with rotation
     */
    void MoveToNextMonthAnimation1L();

    /**
     * Implements transition to next month with animation style 2
     * Gets the month from bottom of the display
     */
    void MoveToNextMonthAnimation2L();
    
    /**
     * Implements transition to previous month with animation style 1
     * Moves curently visible month to right of display with rotation
     * so that below month is visible (which is previous month)
     */
    void MoveToPrevMonthAnimation1L();

    /**
     * Implements transition to previous month with animation style 2
     * Moves curently visible month to bottom of display, so that
     * below month is visible (which is previous month)
     */
    void MoveToPrevMonthAnimation2L();

    
    /**
     * performs scaling transformation according to given scaling factors
     *
     * @param aVisual
     * @param aXSf
     * @param aYSf
     * @param aTime
     */
    void ScaleVisualL(
            CAlfVisual& aVisual,
            TReal aXSf,
            TReal aYSf,
            TInt aTime );

    /**
     * performs rotation transformation according to given timed values.
     *
     * @param aVisual
     * @param aInitAngle
     * @param aTargetAngle
     * @param aTime
     */
    void RotateVisualL(
            CAlfVisual& aVisual,
            TReal aInitAngle,
            TReal aTargetAngle,
            TInt aTime );

    /**
     * Sets opacity of given visual according to timed values.
     *
     * @param aVisual
     * @param aInitValue
     * @param aTargetValue
     * @param aTime
     */
    void SetOpacity(
            CAlfVisual& aVisual,
            TReal aInitValue,
            TReal aTargetValue,
            TInt aTime );
    
private: // Data
	
    // toolkit environment
    CAlfEnv& iEnv;
    
    // deck layout
	CAlfDeckLayout* iDeck;
    
    // index into iMonthControls
    TInt iCurrentMonthControlIndex;
    
    // currently focused month
	TInt iFocusedMonth;
    
    // display rectangle
	TRect iDisplayRect;
	
    // current time/month
	TTime iSelectedTime;

    // month control array
	CAlfExCalendarMonth* iMonthControls[KMonthsVisible];

    // month view status
    TBool iMonthView;

    // dummy calendar engine
    CAlfExCalendarEngine& iCalendarEngine;
    
    // animation style
    TMonthAnimationStyle iMonthAnimationStyle;
    
    // month transition state
    TMonthTransitionState iMonthTransitionState;

    // CAlfImageVisual* iBackground; // Currently not used

    };

#endif // ALFEXCALENDARDECKCONTROL_H

