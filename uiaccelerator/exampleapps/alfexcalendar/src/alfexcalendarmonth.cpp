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



#include <avkon.hrh>

#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfgridlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfcurvepath.h>
#include <alf/alfevent.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfshadowborderBrush.h>
#include <alf/alftransformation.h>
#include <alf/alftextstylemanager.h>
#include <alf/alftextstyle.h>
#include <alf/alfimageloaderutil.h>

#include "alfexcalendarmonth.h"
#include "alfexcalendardeckcontrol.h"
#include "alfexcalendarengine.h"

_LIT( KWkNo, "Wk" );
_LIT( KMonday, "Mo" );
_LIT( KTuesday, "Tu" );
_LIT( KWednesday, "We" );
_LIT( KThursday, "Th" );
_LIT( KFriday, "Fr" );
_LIT( KSaturday, "Sa" );
_LIT( KSunday, "Su" );

_LIT( KMonth1, "January" );
_LIT( KMonth2, "February" );
_LIT( KMonth3, "March" );
_LIT( KMonth4, "April" );
_LIT( KMonth5, "May" );
_LIT( KMonth6, "June" );
_LIT( KMonth7, "July" );
_LIT( KMonth8, "August" );
_LIT( KMonth9, "September" );
_LIT( KMonth10, "October" );
_LIT( KMonth11, "November" );
_LIT( KMonth12, "December" );

// svg files need full path in 3.2
_LIT( KMonthSVGBg, "c:\\data\\Images\\background.svg" );
_LIT( KNormalDayBg, "c:\\data\\Images\\normaldaybutton.svg" );
_LIT( KEventsDayBg, "c:\\data\\Images\\eventsdaybutton.svg" );
_LIT( KTitleBg, "c:\\data\\Images\\title.svg" );
_LIT( KSelectedDayBg, "c:\\data\\Images\\selectedday.svg" );

_LIT( KImagePath, "c:\\data\\Images\\" );

// Custom events
enum
{
    EShowCaledarEvents = 1,
    
};

// Constant declarations
// Month names
const TDesC* KMonths[12] = { &KMonth1, &KMonth2, &KMonth3, &KMonth4,
                             &KMonth5, &KMonth6, &KMonth7, &KMonth8,
                             &KMonth9, &KMonth10, &KMonth11, &KMonth12 };

const TInt KXPaddingLen = 3; // padding between visuals in layout
const TInt KYPaddingLen = 3;
const TInt KNoOfRows = 7;
const TInt KNoOfCols = 8; // including week numbers

const TReal KDayViewSizeXChange = 60.0;
const TReal KDayViewSizeYChange = 60.0;
const TInt KDayViewTransitionTime = 800;
const TInt KDayFocusTime = 1000;

const TReal KUnitSize = 1.0f;
const TReal KXSf = 1.32f;
const TReal KYSf = 1.50f;
const TReal KDayViewXSf = 15.0f;
const TReal KDayViewYSf = 15.0f;


// --------------------------------------------------------------------------
// CAlfExCalendarMonth::CAlfExCalendarMonth
// --------------------------------------------------------------------------
//
CAlfExCalendarMonth::CAlfExCalendarMonth(
    CAlfExCalendarDeckControl& aCalendarControl,
    CAlfEnv& aEnv,
    TDateTime aDateTime )
    :iMonthNumber(0), iDayNoInWeek(0), iFocusedDay(0), iDayView( EFalse ),
    iCalendarControl( aCalendarControl ), iEnv( aEnv ),
    iIsDayFocused( EFalse ), iDateTime( aDateTime )
    {
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::~CAlfExCalendarMonth
// --------------------------------------------------------------------------
//
CAlfExCalendarMonth::~CAlfExCalendarMonth()
    {
    delete iEventTimer;
    delete iFocusedDayBrush;
    delete iNormalDayBrush;
    delete iEventsDayBrush;
    delete iImageLoaderUtil;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::NewLC
// --------------------------------------------------------------------------
//
CAlfExCalendarMonth* CAlfExCalendarMonth::NewLC( CAlfLayout* aParentLayout, 
		TDateTime aDateTime,
		CAlfEnv& aEnv,
        CAlfExCalendarDeckControl& aCalendarControl )
    {
    CAlfExCalendarMonth* self =
        new (ELeave) CAlfExCalendarMonth( aCalendarControl, aEnv, aDateTime );
    CleanupStack::PushL( self );
    
    self->ConstructL( aParentLayout );
    return self;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::ConstructL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::ConstructL( CAlfLayout* aParentLayout )
	{
    CAlfControl::ConstructL( iEnv );

    iMonthNumber = iDateTime.Month();
    iDateTime.SetDay(0);
    iTime = TTime( iDateTime );
    iDayNoInWeek = iTime.DayNoInWeek();
    iDaysInMonth = iTime.DaysInMonth();
    
    iBottomGrid = CAlfGridLayout::AddNewL( *this, 1, 2, aParentLayout );
    iBottomGrid->SetOpacity( TAlfTimedValue(1.0f) );
    
	RArray<TInt> weights;
    weights.Append( 3 );
    weights.Append( 12 );
    iBottomGrid->SetRowsL( weights );
    weights.Reset();
    
    Env().TextureManager().SetImagePathL( KImagePath );
    
    TRect displayRect = iEnv.PrimaryDisplay().VisibleArea();
    
    iImageLoaderUtil = new(ELeave)CAlfImageLoaderUtil();
    //iImageLoaderUtil->SetSize( TSize( displayRect.Width(), displayRect.Height() ) );
    iImageLoaderUtil->SetSize( TSize(240, 320) ); // This is very important
    
    // set background to month view
    SetMonthBackgroundL( iBottomGrid );
    
    // Add text visual for "month name and year"
	iMonthText = CAlfTextVisual::AddNewL( *this, iBottomGrid );

    // sets title text for month ("Date Month name Year")
    SetMonthTitleTextL();
    
    iMonthText->SetColor( KRgbYellow );
    iMonthText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
    
    // create font style and set to iMonthText
    TInt fontStyleId = KErrNotFound;
    fontStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimarySmallFont,
            EAlfTextStyleSmall );
    CAlfTextStyle* style = iEnv.TextStyleManager().TextStyle( fontStyleId );
    style->SetTextSizeInPixels( 25, ETrue );
    iMonthText->SetTextStyle( style->Id() );

    // set svg graphics as Image brush to iMonthText visual
    SetSVGImageTextureL( iMonthText, KTitleBg );
    
    // create dates grid and add dates
    iDatesGrid = CAlfGridLayout::AddNewL(
            *this,
            KNoOfCols,
            KNoOfRows,
            iBottomGrid );
    AddDateVisualsL( iDatesGrid );

    // set week names in iDatesGrid
    SetWeekDayNameL( KWkNo(), iDatesGrid, 0, KWeekNumberColor );
    SetWeekDayNameL( KMonday(), iDatesGrid, 1 );
    SetWeekDayNameL( KTuesday(), iDatesGrid, 2 );
    SetWeekDayNameL( KWednesday(), iDatesGrid, 3 );
    SetWeekDayNameL( KThursday(), iDatesGrid, 4 );
    SetWeekDayNameL( KFriday(), iDatesGrid, 5 );
    SetWeekDayNameL( KSaturday(), iDatesGrid, 6, KWeekendDayColor );
    SetWeekDayNameL( KSunday(), iDatesGrid, 7, KWeekendDayColor );

    // create CalendarEngine EventTimer
    iEventTimer = CAlfExCalendarEngineEventTimer::NewL( *this );
	
    // Create all brushes here
    MAlfBitmapProvider* provider = iImageLoaderUtil->CreateSVGImageLoaderL( KNormalDayBg );
    CAlfTexture& texture1 = Env().TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, 
            provider,
            EAlfTextureFlagRetainResolution );
    iNormalDayBrush = CAlfImageBrush::NewL( Env(), TAlfImage( texture1 ) );
    iNormalDayBrush->SetLayer( EAlfBrushLayerBackground );

    provider = iImageLoaderUtil->CreateSVGImageLoaderL( KEventsDayBg );
    CAlfTexture& texture2 = Env().TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, 
            provider,
            EAlfTextureFlagRetainResolution );
    iEventsDayBrush = CAlfImageBrush::NewL( Env(), TAlfImage( texture2 ) );
    iEventsDayBrush->SetLayer( EAlfBrushLayerBackground );

    provider = iImageLoaderUtil->CreateSVGImageLoaderL( KSelectedDayBg );
    CAlfTexture& texture3 = Env().TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, 
            provider,
            EAlfTextureFlagRetainResolution );
    iFocusedDayBrush = CAlfImageBrush::NewL( Env(), TAlfImage( texture3 ) );
    iFocusedDayBrush->SetLayer( EAlfBrushLayerForeground );
    
    // end of brushes creation
    
    // set the dates in iDatesGrid according to the iTime
    SetDatesL( iDatesGrid, iTime ); // TODO - refactor - API can be changed

    iDatesGrid->SetInnerPadding( TPoint( KXPaddingLen, KYPaddingLen ) );
	}

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetMonthTitleTextL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetMonthTitleTextL()
    {
    // this fuction sets month title as "date month year"
    TBuf<4> yearBuf;
    yearBuf.Num( iDateTime.Year() );
    TBuf<2> dayBuf;
    dayBuf.Num( iFocusedDay+1 ); // iFocusedDay starts from 0

    TBuf<16> buf;
    buf.Append( dayBuf );
    buf.Append( _L(" "));    
    buf.Append( *(KMonths[iDateTime.Month()]) );
    buf.Append( _L(" "));
    buf.Append( yearBuf );
    iMonthText->SetTextL( buf );
    }


// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetMonthBackgroundL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetMonthBackgroundL( CAlfVisual* aVisual, TRgb /* aColor */ )
    {
    // IMPORTANT: first enable the brushes for the visual
    aVisual->EnableBrushesL();
    
    // now set brush
    SetSVGImageTextureL( aVisual, KMonthSVGBg );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetBgImgL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetBgImgL( CAlfVisual* /* aVisual */ )
    {
    // This method is not used currently
    /*
    // Set background image
    TRAP_IGNORE(
    CAlfTexture& texture =
        Env().TextureManager().LoadTextureL(
                *(KMonthBackgrounds[iMonthNumber]),
                EAlfTextureFlagDefault, //EAlfTextureFlagDoNotRetainResolution,
                KAlfAutoGeneratedTextureId );

    CAlfImageBrush* imgBrush = CAlfImageBrush::NewLC( Env(), TAlfImage( texture ) );
    imgBrush->SetLayer( EAlfBrushLayerBackground );
    aVisual->Brushes()->AppendL( imgBrush, EAlfHasOwnership );
    CleanupStack::Pop( imgBrush );
    );
    */
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetSVGImageTextureL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetSVGImageTextureL(
    CAlfVisual* aVisual,
    const TDesC& aSvgFileName )
    {
    // IMPORTANT: first enable the brushes for the visual
    aVisual->EnableBrushesL(); // incase, it has not been enabled
    
    // SVG graphics loading
    MAlfBitmapProvider* provider = iImageLoaderUtil->CreateSVGImageLoaderL( aSvgFileName );
    CAlfTexture& texture = Env().TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, 
            provider,
            EAlfTextureFlagRetainResolution );

    // create Image brush
    CAlfImageBrush* imgBrush = CAlfImageBrush::NewLC( Env(), TAlfImage( texture ) );
    imgBrush->SetLayer( EAlfBrushLayerBackground );
    aVisual->Brushes()->AppendL( imgBrush, EAlfHasOwnership );
    CleanupStack::Pop( imgBrush );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetWeekDayNameL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetWeekDayNameL(
    const TDesC& aDayName,
    CAlfGridLayout* aGrid,
    TInt aDayIndex,
    TRgb aColor )
	{
    CAlfVisual& cell = aGrid->Visual( aDayIndex );
    CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>( cell );
    
    textVisual.SetTextL( aDayName );
    textVisual.SetColor( aColor );

    TInt fontStyleId = KErrNotFound;
    fontStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimarySmallFont,
            EAlfTextStyleSmall );
    
    CAlfTextStyle* style = iEnv.TextStyleManager().TextStyle( fontStyleId );
    style->SetTextSizeInPixels( 20, ETrue );
    style->SetBold( ETrue );
    textVisual.SetTextStyle( style->Id() );
	}

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::AddDateVisualsL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::AddDateVisualsL( CAlfGridLayout* aGrid )
	{
    // adds text visuals i Grid
	for (TInt i = 0 ; i < KNoOfRows * KNoOfCols ; i ++ )
		{
		CAlfTextVisual* text = CAlfTextVisual::AddNewL( *this, aGrid );
        text->EnableBrushesL( ETrue );
		}
	}

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetDatesL
// --------------------------------------------------------------------------
//
TInt CAlfExCalendarMonth::SetDatesL(
    CAlfGridLayout* aGrid,
    TTime aTime )
    {
    // logic to arrange dates and week numbers in correct positions
    // TODO - there is problem of showing weeke numbers in some months. 
    // look into TTime API/implementation and fix.
    
    iDaysInMonth = aTime.DaysInMonth();
    TInt emptyDays = aTime.DayNoInWeek();
    TInt dateTextStyle = DateTextStyleL();
    TInt weekTextStyle = dateTextStyle;
    
    // 1st row of dates
    // set week number at 1st column
    TInt dayIdx = 1;
    TInt visualIdx = 8;
    SetWeekNumberL( aGrid, visualIdx, dayIdx, weekTextStyle );

    // move to 1st day in month
    visualIdx++;
    // clear text visuals in the grid for first empty days
    for( TInt idx = 1; idx <= emptyDays; idx++, visualIdx++ )
        {
        ClearDayVisualL( aGrid, visualIdx );
        }
    
    // set date numbers from correct week (after the empty days)
    for( TInt colIdx = emptyDays+1; colIdx < KNoOfCols; colIdx++, visualIdx++, dayIdx++ )
        {
        if( colIdx > KNoOfCols-3 ) // weekend days
            {
            SetDateNumberL( aGrid, visualIdx, dayIdx, dateTextStyle, KWeekendDayColor );
            }
        else
            {
            SetDateNumberL( aGrid, visualIdx, dayIdx, dateTextStyle );            
            }
        }
    
    // fill week and date numbers from 2nd row
    for( TInt rowIdx = 2; rowIdx < KNoOfRows && dayIdx <= iDaysInMonth; rowIdx++ )
        {
        // set week number at 1st column
        SetWeekNumberL( aGrid, visualIdx, dayIdx, weekTextStyle );
        visualIdx++;

        // set dates in next columns
        for( TInt colIdx = 1;
             colIdx < KNoOfCols && dayIdx <= iDaysInMonth; /* only week days */
             colIdx++, visualIdx++, dayIdx++ )
            {
            if( colIdx > KNoOfCols-3 ) // weekend days
                {
                SetDateNumberL( aGrid, visualIdx, dayIdx, dateTextStyle, KWeekendDayColor );
                }
            else // week days
                {
                SetDateNumberL( aGrid, visualIdx, dayIdx, dateTextStyle );            
                }            
           }
        }

    // clear remaining text visuals in the grid
    for( ; visualIdx < KNoOfRows * KNoOfCols; visualIdx++ )
        {
        ClearDayVisualL( aGrid, visualIdx );
        }
    
    return 1;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetDateNumberL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetDateNumberL(
        CAlfGridLayout* aGrid,
        TInt aVisualIndex,
        TInt aDayNumber,
        TInt aDateTextStyle,
        TRgb aColor )
    {
    // get text visual from grid
    CAlfVisual& cell = aGrid->Visual( aVisualIndex );
    CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>( cell );
    textVisual.SetTextStyle( aDateTextStyle );
    textVisual.SetColor( aColor );
    textVisual.SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );

    // set the text
    TBuf<3> buf;
    buf.Num( aDayNumber );
    textVisual.SetTextL( buf );

    // append brush
    textVisual.Brushes()->AppendL( iNormalDayBrush, EAlfDoesNotHaveOwnership );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetWeekNumberL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetWeekNumberL(
        CAlfGridLayout* aGrid,
        TInt aVisualIndex,
        TInt aDayNumber,
        TInt aWeekTextStyle )
    {
    // get text visual from grid
    CAlfVisual& cell = aGrid->Visual( aVisualIndex );
    CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>( cell );
    textVisual.SetColor( KWeekNumberColor );
    textVisual.SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
    textVisual.SetTextStyle( aWeekTextStyle );

    // use aDayNumber to get the correct week number
    TDateTime tempDateTime = iDateTime;
    tempDateTime.SetDay( aDayNumber );
    TTime tempTime = TTime( tempDateTime );

    TBuf<3> buf;
    buf.Num( tempTime.WeekNoInYear() );
    textVisual.SetTextL( buf ); 
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::ClearDayVisualL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::ClearDayVisualL(
        CAlfGridLayout* aGrid,
        TInt aVisualIndex )
    {
    CAlfVisual& cell = aGrid->Visual( aVisualIndex );
    CAlfTextVisual& textVisual = static_cast<CAlfTextVisual&>( cell );
    
    TBuf<3> buf;
    textVisual.SetTextL( buf ); // set empty string
    
    // remove all brushes
    if( textVisual.Brushes() )
        {
        TInt count = textVisual.Brushes()->Count();
        for( TInt i = 0;  i < count; ++i )
            {
            textVisual.Brushes()->Remove( 0 );
            }
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::DateTextStyleL
// --------------------------------------------------------------------------
//
TInt CAlfExCalendarMonth::DateTextStyleL()
    {
    // creates font style id and returns
    TInt fontStyleId = KErrNotFound;
    fontStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimarySmallFont,
            EAlfTextStyleSmall );
    CAlfTextStyle* style = iEnv.TextStyleManager().TextStyle( fontStyleId );
    style->SetTextSizeInPixels( 20, ETrue );
    return style->Id();
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::UpdateMonthL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::UpdateMonthL( TDateTime aDateTime )
	{
    // updates month acording to received aDateTime
    
    iDateTime = aDateTime;
    iDateTime.SetDay(0);
    
	iTime = TTime( iDateTime );
    iDaysInMonth = iTime.DaysInMonth();
	iDayNoInWeek = iTime.DayNoInWeek();
	iMonthNumber = iDateTime.Month();
    
	SetDatesL( iDatesGrid, iTime );

    TBuf<4> yearBuf;
    yearBuf.Num( iDateTime.Year() );
    TBuf<16> buf;
    buf.Append( *(KMonths[iMonthNumber]) );
    buf.Append( _L(" "));
    buf.Append( yearBuf );

    iMonthText->SetTextL( buf );
	}
	
// --------------------------------------------------------------------------
// CAlfExCalendarMonth::MonthFocusedL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::MonthFocusedL()
    {
    // update(some kind of selection) all the days which have calendar events
    UpdateDaysWithCalEventsL();
    
    // focus the 1st day
    iFocusedDay = 0;
    iDayVisualIndex = (9 + iDayNoInWeek + iFocusedDay);
    CAlfVisual& startDay = iDatesGrid->Visual( iDayVisualIndex );
    FocusDayL( static_cast<CAlfTextVisual&>( startDay ) );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::UpdateDaysWithCalEventsL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::UpdateDaysWithCalEventsL()
    {
    iDaysInMonth = iTime.DaysInMonth();
    iDayNoInWeek = iTime.DayNoInWeek();
    
    CAlfExCalendarEngine& calendarEngine = iCalendarControl.CalendarEngine();
    calendarEngine.LoadCalendarEventsL( iTime );

    TInt dayVisualIndex = (9 + iDayNoInWeek); // 1st day visual in the month
    for( TInt i = 0; i < iDaysInMonth; i++, dayVisualIndex++ )
        {
        if( dayVisualIndex%8 == 0 ) // Visual index is at week number column (i.e., at column 0)
            {
            dayVisualIndex++;
            }
        if( calendarEngine.EventsAvailable( iTime + (TTimeIntervalDays)i ) )
            {
            CAlfVisual& cell = iDatesGrid->Visual( dayVisualIndex );

            // add shadow border brush to the day visual
            cell.Brushes()->AppendL( iEventsDayBrush, EAlfDoesNotHaveOwnership );
            }
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::RemoveAllDayBrushesL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::RemoveAllDayBrushesL()
    {
    // remove all brushes of day visuals in grid
    for( TInt i = 0 ; i < KNoOfRows * KNoOfCols ; i++ )
        {
        CAlfVisual& day = iDatesGrid->Visual( i );
        
        if( day.Brushes() )
            {
            TInt count = day.Brushes()->Count();
            for( TInt i = 0;  i < count; ++i )
                {
                day.Brushes()->Remove( 0 );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::MoveDayFocusL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::MoveDayFocusL( TInt aChange )
	{
	CAlfVisual& day1 = iDatesGrid->Visual( iDayVisualIndex );
    UnFocusDayL( static_cast<CAlfTextVisual&>( day1 ) );
    
    iFocusedDay += aChange;
	if ( iFocusedDay < 0 )
		{
        RemoveAllDayBrushesL();
        iCalendarControl.SetMonthView( EFalse );
        return;
		}
	if (iFocusedDay > iDaysInMonth -1 )
		{
        RemoveAllDayBrushesL();
        iCalendarControl.SetMonthView( EFalse );
        return;
		}

    if( aChange == 7 )
        {
        iDayVisualIndex += 8;
        }
    else if( aChange == -7 )
        {
        iDayVisualIndex -= 8;
        }
    else
        {
        iDayVisualIndex += aChange;
        if( iDayVisualIndex%8 == 0 ) // Visual index is at week number column (column 0)
            {
            if( aChange > 0 )  // change is +1. move to next visual position
                {
                iDayVisualIndex++;
                }
            else // change is -1. Move to previous visual position
                {
                iDayVisualIndex--;
                }
            }
        }
    
	CAlfVisual& day2 = iDatesGrid->Visual( iDayVisualIndex );
    FocusDayL( static_cast<CAlfTextVisual&>( day2 ) );
	}

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::UnFocusDayL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::UnFocusDayL( CAlfTextVisual& aTextVisual )
    {
    if( !iIsDayFocused ) // if day is not focued, dont do unfocus
        {
        return;
        }
    iIsDayFocused = EFalse;
    
    iEventTimer->Cancel();
    
    // IMPORTANT: Remove brushes applied ONLY in FocusDayL()
    if( aTextVisual.Brushes() )
        {
        aTextVisual.Brushes()->Remove( aTextVisual.Brushes()->Count()-1 ); // 1st Brush
        }
    
    // must scale back to original size
    ScaleVisualL( aTextVisual, (KUnitSize/KXSf), (KUnitSize/KYSf), KDayFocusTime );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::FocusDayL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::FocusDayL( CAlfTextVisual& aTextVisual )
    {
    if( iIsDayFocused ) // if day is already focused
        {
        return;
        }
    SetMonthTitleTextL();

    iIsDayFocused = ETrue;
    
    // Append brush 1
    aTextVisual.Brushes()->AppendL( iFocusedDayBrush, EAlfDoesNotHaveOwnership );

    ScaleVisualL( aTextVisual, KXSf, KYSf, KDayFocusTime );
    
    TTimeIntervalMicroSeconds32 dayOpenTime( 6000000 ); // EVENT TIMER
    iEventTimer->StartTimer( dayOpenTime );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::OpenDayViewL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::OpenDayViewL()
    {
    // check if calendar events available for the day
    CAlfExCalendarEngine& calendarEngine = iCalendarControl.CalendarEngine();
    if( !calendarEngine.EventsAvailable( iTime + (TTimeIntervalDays)iFocusedDay ) )
        {
        return; // return if no calendar events for the day
        }
    // get calendar events
    calendarEngine.GetEventInformation( 
                iTime + (TTimeIntervalDays)iFocusedDay, 
                0, 
                iCalendarEventTextBuffer );

    TAlfTimedValue opacity;
    opacity.SetValueNow( 1 );
    opacity.SetTarget( 0, KDayViewTransitionTime ); // in milliseconds
    // Set opacity of all visuals to 0
    for (TInt i = 0 ; i < KNoOfRows * KNoOfCols ; i ++ )
        {
        CAlfTextVisual& day = static_cast<CAlfTextVisual&>( iDatesGrid->Visual( i ) );
        day.SetOpacity( opacity );
        }

    // store the original day visual position to be used in UnFocusL()
    // to move it back to original position
    iOriginalDayPos = iDatesGrid->Visual(iDayVisualIndex).Pos();

    // now move the day visual to center on display
    TRect displayRect = iEnv.PrimaryDisplay().VisibleArea();
    TAlfRealPoint pos = TAlfRealPoint(displayRect.Width()/2, displayRect.Height()/3 );

    CAlfTextVisual& day = static_cast<CAlfTextVisual&>( iDatesGrid->Visual( iDayVisualIndex ) );
    day.SetPos( pos, KDayViewTransitionTime );

    // Scale the day to fit entire display 
    ScaleVisualL( day, KDayViewXSf, KDayViewYSf, KDayViewTransitionTime );

    // send custom event to show calendar events after KDayViewTransitionTime
    iEnv.Send( TAlfCustomEventCommand( EShowCaledarEvents, this ), KDayViewTransitionTime );
    
    iDayView = ETrue;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::CloseDayViewL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::CloseDayViewL()
    {
    // cancel event timer
    iEventTimer->Cancel();
    
    CAlfTextVisual& dayEventsVisual =
        static_cast<CAlfTextVisual&>( iDatesGrid->Visual( (KNoOfRows*KNoOfCols)-1 ) );
    // no need to put dayEventsVisual visual back to original size/position.
    // this can be used dedicatedly to show calendar events

    // just empty the text and hide it using opacity setting
    TBuf<1> empty;
    dayEventsVisual.SetTextL( empty );
    dayEventsVisual.SetOpacity(0);
    
    // scale the opened day in OpenDayViewL() back to oroginal size
    CAlfTextVisual& day = static_cast<CAlfTextVisual&>( iDatesGrid->Visual( iDayVisualIndex ) );
    ScaleVisualL( day, (KUnitSize/KDayViewXSf), (KUnitSize/KDayViewYSf), KDayViewTransitionTime );
    
    // set position of opened day in OpenDayViewL() back to oroginal position
    day.SetPos( iOriginalDayPos.ValueNow(), KDayViewTransitionTime );

    //set all visuals opacity to 1
    TAlfTimedValue opacity;
    opacity.SetValueNow( 0.0 );
    opacity.SetTarget( 1.0, KDayViewTransitionTime ); // in milliseconds
    for (TInt i = 0 ; i < KNoOfRows * KNoOfCols ; i ++ )
        {
        CAlfTextVisual& day = static_cast<CAlfTextVisual&>( iDatesGrid->Visual( i ) );
        day.SetOpacity( opacity );
        }
    dayEventsVisual.SetOpacity(0); // hide the dedicated dayEventsVisual
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::ScaleVisualL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::ScaleVisualL(
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
// CAlfExCalendarMonth::RotateVisualL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::RotateVisualL(
        CAlfVisual& aVisual,
        TReal aInitAngle,
        TReal aTargetAngle,
        TInt aTime )
    {
    aVisual.EnableTransformationL();
    
    TAlfTimedValue angle( aInitAngle );
    angle.SetTarget( aTargetAngle, aTime );
    aVisual.Transformation().Rotate( angle , 1.0, 0.0, 0.0 );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetTextStylesL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetTextStylesL(
        CAlfTextVisual& aTextVisual,
        const TDesC &aText,
        TRgb aColor,
        CAlfTextVisual::TLineWrap aLineWrap,
        TAlfAlignHorizontal /*aAlignH*/,
        TAlfAlignVertical /*aAlignV*/,
        TPoint /*aPoint*/ )
    {
    aTextVisual.SetTextL( aText );
    aTextVisual.SetColor( aColor );
    aTextVisual.SetWrapping( aLineWrap );
    aTextVisual.SetAlign( EAlfAlignHLeft, EAlfAlignVTop );
    aTextVisual.SetPadding( TPoint(5,5) );

    // S60 Font IDs:
    // EAknLogicalFontPrimaryFont, EAknLogicalFontSecondaryFont, EAknLogicalFontTitleFont, 
    // EAknLogicalFontDigitalFont, EAknHighestLogicalFont, EAknLogicalFontPrimarySmallFont
    TInt fontStyleId = KErrNotFound;
    fontStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimarySmallFont,
            EAlfTextStyleSmall );
    
    CAlfTextStyle* style = iEnv.TextStyleManager().TextStyle( fontStyleId );
    TInt sizeInPixels = style->TextSizeInPixels();
    style->SetTextSizeInPixels( 20, ETrue );
    style->SetBold( EFalse );
    style->SetItalic( EFalse );
    style->SetUnderline( ETrue );
    style->SetStrikeThrough( EFalse );
    aTextVisual.SetTextStyle( style->Id() );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::ShowCalendarEventsL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::ShowCalendarEventsL()
    {
    // use last text visual in the grid to show the calendar events
    CAlfTextVisual& dayEventsVisual =
        static_cast<CAlfTextVisual&>( iDatesGrid->Visual( (KNoOfRows*KNoOfCols)-1 ) );

    // set dayEventsVisual position at 1st text visual of grid.
    // to align the day events from below the month name
    dayEventsVisual.SetPos( iDatesGrid->Visual(0).Pos() );
    
    // set the size to fit to bottom right corner of display.
    TAlfRealRect visualArea = dayEventsVisual.DisplayRectTarget();
    TAlfRealPoint sizeChange( 200, 200 ); // width, height
    TAlfRealPoint newSize = TAlfRealPoint( visualArea.Size().iWidth, visualArea.Size().iHeight)  + sizeChange;
    dayEventsVisual.SetSize( newSize, 0 );
    
    // now set the calendar events text
    SetTextStylesL( dayEventsVisual, iCalendarEventTextBuffer );

    // set opacity to 1 to make it visible
    TAlfTimedValue opacity;
    opacity.SetValueNow( 0 );
    opacity.SetTarget( 1, 500 ); // in milliseconds
    dayEventsVisual.SetOpacity( opacity );    
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::OfferEventL
// --------------------------------------------------------------------------
//
TBool CAlfExCalendarMonth::OfferEventL(const TAlfEvent& aEvent )
	{
    // handle custome vents if any
    if( aEvent.IsCustomEvent() )
        {
        if( iDayView )
            {
                switch( aEvent.CustomParameter() )
                {
                case EShowCaledarEvents:
                    ShowCalendarEventsL();
                    return ETrue;
    
                default:
                    break;
                }
            }
        else
            return ETrue;
        }
    
    // handle key events
	if ( aEvent.Code() == EEventKey )
		{
		TInt code = aEvent.KeyEvent().iCode;
		switch( code )
			{
            case EKeyBackspace: // TODO - use "Back" softkey
                if( iDayView )
                    {
                    // close day view and return to month view
                    iDayView = EFalse;
                    CloseDayViewL();
                    }
                return ETrue;

			case 63557: // TODO - replace with key code
                if( !iDayView )
                    {
                    OpenDayViewL();
                    }
                return ETrue;
                
			case EKeyLeftArrow:
                if( iDayView )
                    {
                    iDayView = EFalse;
                    CloseDayViewL();
                    }
				MoveDayFocusL( -1 );
				return ETrue;
				
			case EKeyRightArrow:
                if( iDayView )
                    {
                    iDayView = EFalse;
                    CloseDayViewL();
                    }
				MoveDayFocusL( 1 );
				return ETrue;

			case EKeyUpArrow:
                if( iDayView )
                    {
                    iDayView = EFalse;
                    CloseDayViewL();
                    }
				MoveDayFocusL( -7 );
				return ETrue;
				
			case EKeyDownArrow:
                if( iDayView )
                    {
                    iDayView = EFalse;
                    CloseDayViewL();
                    }
                MoveDayFocusL( +7 );
                return ETrue;

			default:;break;
			}		
	
		}
	return EFalse;
	}

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::SetDateGridWeightsL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::SetDateGridWeightsL(
    CAlfGridLayout* aDatesGrid,
    TInt aRowIndex, 
    TInt aWeight )
    {
    RArray<TInt> weights;
    CleanupClosePushL( weights );

    for( TInt i = 1; i <= KNoOfRows; ++i )
        {
        User::LeaveIfError( weights.Append( 1 ) );
        }
    if( aRowIndex != -1 )
        {
        weights.Insert( aRowIndex, aWeight );
        }
    
    aDatesGrid->SetRowsL( weights );
    CleanupStack::PopAndDestroy( &weights );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarMonth::HandleTimerEventL
// --------------------------------------------------------------------------
//
void CAlfExCalendarMonth::HandleTimerEventL()
    {
    CAlfExCalendarEngine& calendarEngine = iCalendarControl.CalendarEngine();
    if( calendarEngine.EventsAvailable( iTime + (TTimeIntervalDays)iFocusedDay ) &&
            !iDayView ) // AND if day has not been opend already
        {
        OpenDayViewL();
        }
    }

// end of file

