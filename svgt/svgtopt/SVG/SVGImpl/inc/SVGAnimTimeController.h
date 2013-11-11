/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_CSVGANIMTIMECONTROLLER_
#define _INC_CSVGANIMTIMECONTROLLER_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#include "GfxFloatFixPt.h"

#include "SVGEngineImpl.h"
#include "SVGEvent.h"



const TInt KTimeMax = 255;
const TUint32 KTimeIndefinite = 0x7fffffff;
typedef TUint8 TAnimCalcMode;
const TAnimCalcMode KAnimCalcModeDiscrete = 0;
const TAnimCalcMode KAnimCalcModeLinear = 1;
const TAnimCalcMode KAnimCalcModePaced = 2;
const TAnimCalcMode KAnimCalcModeSpline = 3;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimTimeController : public CBase
    {
    public:
        void  AddToInitialList(TInt aBeginTime);

		void                SetBeginTimeList(RArray<TInt32>*& aList);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgAnimTimeController*  CSvgAnimTimeController::NewL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        ~CSvgAnimTimeController();
    private:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgAnimTimeController();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL();

    public:
		

		//For Dur as media for audio and vedio elements
		void SetOrgDurationTime(TInt32 aValue);
		

		
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetBeginTime( TInt32 aBeginTime );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ResetBeginTime( TInt32 aBeginTime );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetDurationTime( TInt32 aDurationTime );

       /**
         * Need method description
         *
         * Himanshu: added to adjust the duration time in case there is a negative begin time
         * 
         * 
         */
       TInt32                           GetDurationTime( );
       
       /**
         * Need method description
         *
         * Himanshu: added to adjust the end time in case there is a negative begin time
         * 
         * 
         */
       TInt32                           GetEndTime( );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetEndTime( TInt32 aEndTime );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetRepeatDurationTime( TUint32 aRepeatDurationTime );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void SetCalMode( TAnimCalcMode aCalcMode )
            {
            iCalcMode = aCalcMode;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void ResetKeyTime()
            {
            iKeyTime->Reset();
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    CreateKeyTime( TInt32 aKeyTimeCount );
		void CreateKeyTimeForEnumeration( TInt32 aKeyTimeCount );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    PrepareTimeL( const RArray<TFloatFixPt>* aValues );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32 BeginTime()
            {
            return iBeginTime;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32 DurationTime()
            {
            return iDurationTime;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
       

		TUint32 EndTime()
            {
            return iEndTime;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32 RepeatDurationTime()
            {
            return iRepeatDurationTime;
            }

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TAnimCalcMode CalMode()
            {
            return iCalcMode;
            }
		
		
		 void  CalculateAnimTimes();	


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void     AddBeginTime( TInt32 aBeginTime );
		void 	 AddEndTime( TInt32 aEndTime );
		void 	GetNextEndTime(TInt32 aBeginTime);
		void SetNextBeginTime(TInt32 aCurTime);
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32   GetBeginTime( TInt32 aIndex );
        
         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt 	BeginTimesCount();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
       // void     AddBeginEvent( TSvgEvent aBeginEvent );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
       // TSvgEvent   GetBeginEvent( TInt32 aIndex );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    AddKeyTime( TFloatFixPt aTimerTime );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    AddKeySplineL( TFloatFixPt aX1, TFloatFixPt aY1, TFloatFixPt aX2, TFloatFixPt aY2 );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void    GetAnimTime( TInt32 aTimerTime,
                             TInt32& aAnimTime,
                             TInt32& aValueIndex,
                             TInt32& aSubAnimTime );
        void GetAnimTimeForEnumeration( TInt32 aTimerTime,
                                          TInt32& aAnimTime,
                                          TInt32& aValueIndex,
                                          TBool aKeyTimesPresent
                                           );
                             
        void CopyL(CSvgAnimTimeController* newElement);
        void SetEndTimesIndefinite();
        TUint32 GetOriginalEndTime();
        void ReInitializeForSeek();

        /**
         * Sets the calc mode to spline mode
         *
         * @since S60 v3.1
         * @param aIsSplineCalcMode Indicates whether the 
         *        calcMode is of type "spline"
         * @return none.
         */
        void SetIsSplineCalcMode( TBool aIsSplineCalcMode );

        /**
         * Returns ETrue if the calc mode is in spline mode
         *
         * @since S60 v3.1
         * @return TBool ETrue if spline mode, EFalse otherwise.
         */
        TBool IsSplineCalcMode() const;

        /**
         * Sets the spline parameter string for later processing
         * (when calc mode is set) 
         *
         * @since S60 v3.1
         * @param aIsSplineCalcMode Indicates whether the 
         *        calcMode is of type "spline"
         * @return none.
         */
        void SetSplineValueL( const TDesC& aSplineValue );

        /**
         * Returns the string corresponding to the Spline parameters.
         *
         * @since S60 v3.1
         * @return TPtrC Pointer to Spline param string.
         */
        TPtrC SplineValue() const;
#if defined(__WINS__)
    public:
        CSvgEngineImpl*             iEngine;
#endif

    
        class TKeyTime
            {
            public:
                TUint16                  iY;
                TUint16                  iX;
            };

	private:

        TAnimCalcMode           iCalcMode;

		TUint32                  iBeginTime;
        TUint32                  iInitialBeginTime;
        TUint32                  iDurationTime;
		TUint32                  iOrgDurationTime;
        TUint32                  iEndTime;
        TUint32                  iInitialEndTime;
		TUint32                  iOrgEndTime;
        TUint32                  iRepeatDurationTime;
        TUint32                  iNewActiveDurationTime;

        // Key time
        RArray<TKeyTime>*       iKeyTime;
        RArray<TUint32>*         iAnimTime;
        TUint16                 iSplineIndex;

        RArray<TInt32>*         iBeginTimeList;
        RArray<TInt32>*         iInitialBeginTimeList;
        RArray<TInt32>*         iEndTimeList;
        RArray<TInt32>*         iInitialEndTimeList;

		TUint32                  iModifiedEndTime;
        // Indicates whether the calc mode is spline or otherwise
        TBool                   iIsSplineCalcMode;
        // Stores the spline parameters as a string
        HBufC*                  iSplines;

	public:
		// for decoder.
		void    SetKeyTimeArray(RArray<TKeyTime>*& aArray);
		void   SetAnimTimeArray(RArray<TUint32>*& aArray);
		// for DOM tree Reusage
		void   SetOriginalValues_DOMReuse();
		void   ReInitialize();

		void  SetModifiedEndTime(TUint32 aTime);
		    	
		void		 Reset();
		void SaveBeginTime();
		void SaveEndTime();
		void ResetBeginTime();
		void ResetEndTime();
        TInt32 LastEndTime();
        TInt32 LastBeginTime();
    };


#endif  //_INC_CSvgAnimTimeController_
