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



#ifndef CALFEXMONTH_H_
#define CALFEXMONTH_H_

#include <alf/alfcontrol.h>

#include "alfexcalendarengineeventtimer.h"

// forward declarations
class CAlfGridLayout;
class CAlfTextVisual;
class CAlfExCalendarDeckControl;
class CAlfBorderBrush;
class CAlfShadowBorderBrush;
class CAlfGradientBrush;
class CAlfImageBrush;
class MAlfBitmapProvider;

// Constants declarations
const TRgb KWeekDayColor = TRgb( 0xff8844 ); // BGR value
const TRgb KWeekendDayColor = TRgb( 0x7f7fff );
const TRgb KFocusedDayColor = TRgb( 0x0000ff );
const TRgb KWeekNumberColor = TRgb( 0x88ffff ); //


// CAlfExCalendarMonth class declaration
class CAlfExCalendarMonth : 
    public CAlfControl,
    public MAlfExCalendarEngineEventTimerObserver
{

public: // methods

    /**
     * 2 phase constructor. A static factory method.
     * @param aParentLayout
     * @param aDateTime
     * @param aEnv
     * @param aCalendarControl
     * @return 
     */
    static CAlfExCalendarMonth* NewLC(
            CAlfLayout* aParentLayout,
            TDateTime aDateTime,
            CAlfEnv& aEnv,
            CAlfExCalendarDeckControl& aCalendarControl );
    
	virtual ~CAlfExCalendarMonth();

    TBool OfferEventL( const TAlfEvent& aEvent );
    
	void UpdateMonthL( TDateTime aDateTime );
    
    void MonthFocusedL();

    
private: // methods

    CAlfExCalendarMonth(
            CAlfExCalendarDeckControl& aCalendarControl,
            CAlfEnv& aEnv,
            TDateTime aDateTime );

	void ConstructL( CAlfLayout* aParentLayout );
	
	void SetWeekDayNameL(
            const TDesC& aDayName,
            CAlfGridLayout* aGrid,
            TInt aDayIndex,
            TRgb aColor = KWeekDayColor );
	
	void AddDateVisualsL( CAlfGridLayout* aGrid );
	
	TInt SetDatesL( CAlfGridLayout* aGrid, TTime aTime );

    void SetDateNumberL(
            CAlfGridLayout* aGrid,
            TInt aVisualIndex,
            TInt aDayNumber,
            TInt aDateTextStyle,
            TRgb aColor = KWeekDayColor );

    void SetWeekNumberL(
            CAlfGridLayout* aGrid,
            TInt aVisualIndex,
            TInt aDayNumber,
            TInt aWeekTextStyle );

    void ClearDayVisualL(
            CAlfGridLayout* aGrid,
            TInt aVisualIndex );
    
    void UpdateDaysWithCalEventsL();
	
    void RemoveAllDayBrushesL();
    
	void MoveDayFocusL( TInt aChange );

    void SetMonthBackgroundL( CAlfVisual* aVisual,  TRgb aColor = KRgbGray );

    void SetMonthTitleTextL();
    
    void SetBgImgL( CAlfVisual* aVisual );

    void SetSVGImageTextureL(
            CAlfVisual* aVisual,
            const TDesC& aSvgFileName );

    void GetSVGImageTextureL(
            const TDesC& aSvgFileName,
            CAlfTexture& aTexture );
    
    void SetDateGridWeightsL(
        CAlfGridLayout* aDatesGrid,
        TInt aRowIndex = -1, 
        TInt aWeight = 1 );
    
    void UnFocusDayL( CAlfTextVisual& aTextVisual );
    
    void FocusDayL( CAlfTextVisual& aTextVisual );
    
    void OpenDayViewL();
    
    void CloseDayViewL();

    void ShowCalendarEventsL();
    
    void ScaleVisualL(
            CAlfVisual& aVisual,
            TReal aXSf,
            TReal aYSf,
            TInt aTime );
    
    void RotateVisualL(
            CAlfVisual& aVisual,
            TReal aInitAngle,
            TReal aTargetAngle,
            TInt aTime );

    void SetTextStylesL(
        CAlfTextVisual& aTextVisual,
        const TDesC &aText,
        TRgb aColor = KRgbYellow,
        CAlfTextVisual::TLineWrap aLineWrap = CAlfTextVisual::ELineWrapBreak,
        TAlfAlignHorizontal aAlignH = EAlfAlignHCenter, 
        TAlfAlignVertical aAlignV = EAlfAlignVCenter,
        TPoint aPoint = TPoint(0,0) );

    TInt DateTextStyleL();

    /**
     * From MAlfExCalendarEngineEventTimerObserver
     */
    void HandleTimerEventL();

    
private: // Data

    // TODO - comment all members, remove/optimize unnecessary , refactor
	
    CAlfImageLoaderUtil* iImageLoaderUtil;
    
	CAlfEnv& iEnv;

    CAlfExCalendarDeckControl& iCalendarControl;

    CAlfExCalendarEngineEventTimer* iEventTimer;


    TDateTime iDateTime;   
    
    TTime iTime; // TODO - cleanup/remove these variables. just iDateTime can be sufficient
    
    TInt iMonthNumber;

    TInt iDaysInMonth;
    
    TInt iDayNoInWeek;

	
    CAlfGridLayout* iBottomGrid;
    
	CAlfGridLayout*	iDatesGrid;

    CAlfTextVisual* iMonthText;

    TInt iFocusedDay;

    TInt iDayVisualIndex;
    
    TBool iDayView;
    
    TBool iIsDayFocused;

    
    CAlfImageBrush* iNormalDayBrush;

    CAlfImageBrush* iEventsDayBrush;
    
    CAlfImageBrush* iFocusedDayBrush;
	
    TAlfTimedPoint iOriginalDayPos;

    TBuf<128> iCalendarEventTextBuffer;
    
};

#endif /*CALFEXMONTH_H_*/
