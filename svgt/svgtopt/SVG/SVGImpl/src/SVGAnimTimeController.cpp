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
* Description:  SVG Implementation source file
 *
*/


#include "SVGAnimTimeController.h"
#include "SVGAnimationBase.h"

#include "GfxGeneralPath.h"
#include "GfxFlatteningPathIterator.h"

	#ifdef SVG_FLOAT_BUILD
#define MULT255(x) (255*(x))
	#else
#define MULT255(x) (((x)<<8) - (x))
	#endif

_LIT( KEmptyString,"" );

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgAnimTimeController* CSvgAnimTimeController::NewL()
    {
    CSvgAnimTimeController* self = new ( ELeave ) CSvgAnimTimeController();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimTimeController::~CSvgAnimTimeController()
    {
	if ( iKeyTime )
		{
		iKeyTime->Close();
		delete iKeyTime;
		iKeyTime = NULL;
		}

    if ( iAnimTime )
		{
		iAnimTime->Close();
		delete iAnimTime;
		iAnimTime = NULL;
		}

    if ( iBeginTimeList )
        {
        iBeginTimeList->Close();
        delete iBeginTimeList;
		iBeginTimeList = NULL;
        }
    if ( iInitialBeginTimeList )
        {
        iInitialBeginTimeList->Close();
        delete iInitialBeginTimeList;
		iInitialBeginTimeList = NULL;
        }
    if ( iEndTimeList )
        {
        iEndTimeList->Close();
        delete iEndTimeList;
		iEndTimeList = NULL;
        }
    if ( iInitialEndTimeList )
        {
        iInitialEndTimeList->Close();
        delete iInitialEndTimeList;
		iInitialEndTimeList = NULL;
        }
    delete iSplines;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimTimeController::CSvgAnimTimeController() : iCalcMode( KAnimCalcModeLinear ),
                                                   iBeginTime( 0 ),
                                                   iInitialBeginTime(0),
                                                   iDurationTime( KTimeIndefinite ),
                                                   iEndTime( KTimeIndefinite ),
                                                   iInitialEndTime(KTimeIndefinite),
                                                   iRepeatDurationTime( KTimeIndefinite ),
                                                   iNewActiveDurationTime( 0 ),
                                                   iSplineIndex( 0 ),
												   iModifiedEndTime( 0 ),
                                                   iIsSplineCalcMode( EFalse )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::ConstructL()
    {
    iKeyTime = new ( ELeave ) RArray<TKeyTime>( 1 );
    iAnimTime = new ( ELeave ) RArray<TUint32>( 1 );
    for ( TInt32 i = 0; i < KTimeMax + 1; i++ )
        {
        iAnimTime->AppendL(0);
        }
    iBeginTimeList = new ( ELeave ) RArray<TInt32>( 1 );
    iInitialBeginTimeList = new ( ELeave ) RArray<TInt32>( 1 );
    iEndTimeList = new (ELeave) RArray<TInt32>(1);
    iInitialEndTimeList = new (ELeave) RArray<TInt32>(1);
    }


// *******************************************************
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetBeginTime( TInt32 aBeginTime )
    {
    iBeginTime = aBeginTime; // msec
    }
    
    

void CSvgAnimTimeController::SetOrgDurationTime(TInt32 aValue)
{
	iOrgDurationTime = aValue;
}




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::AddBeginTime( TInt32 aBeginTime )
    {
       if ( aBeginTime < 0 )
        {
            aBeginTime = 0;     //Himanshu: to check the negative begin value
        }   
     iBeginTimeList->Append( aBeginTime );
     iBeginTimeList->SortSigned();
    }
void CSvgAnimTimeController::AddEndTime( TInt32 aEndTime )
    {
     iEndTimeList->Append( aEndTime );
     iEndTimeList->SortSigned();
     iEndTime = iEndTimeList->operator[](0);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TUint32 CSvgAnimTimeController::GetBeginTime( TInt32 aIndex )
    {

    if(aIndex >= 0 && aIndex < iBeginTimeList->Count())
	    {

		return (TUint32) (iBeginTimeList->operator[](aIndex));
    	}
	else
		{

		return 0;

	    }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::ResetBeginTime( TInt32 aBeginTime )
    {
    iBeginTime = aBeginTime; // msec
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetDurationTime( TInt32 aDurationTime )
    {
    iDurationTime = aDurationTime; // msec

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32 CSvgAnimTimeController::GetDurationTime()
    {
    return iDurationTime; // msec

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetEndTime( TInt32 aEndTime )
    {
    iEndTime = aEndTime; // msec
	if (aEndTime != (TInt32)KTimeIndefinite)
		iModifiedEndTime= aEndTime;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32 CSvgAnimTimeController::GetEndTime()
    {
    return iEndTime; // msec

    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetRepeatDurationTime( TUint32 aRepeatDurationTime )
    {
    iRepeatDurationTime = aRepeatDurationTime;

   }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::GetAnimTime( TInt32 aTimerTime,
                                          TInt32& aAnimTime,
                                          TInt32& aValueIndex,
                                          TInt32& aSubAnimTime )
    {
    if(iDurationTime == KTimeIndefinite || iDurationTime == 0)
        {
        aValueIndex = 0;
        aSubAnimTime= 0;
        aAnimTime=0;
        return;
        }
    TInt32 x = 0;

    if( iNewActiveDurationTime > 0 )//"repeatDur" <= "Dur"
		{
       	x = MULT255( aTimerTime - iBeginTime ) / iNewActiveDurationTime;
		}
    else
		{

		x = MULT255( aTimerTime - iBeginTime ) / iDurationTime;

		}

    if ( x > KTimeMax )
		{
        x = KTimeMax;
		}
	else
		{
		if ( x < 0 )
			{
			x = 0;
			}
		}

    TInt32 kcount = iKeyTime->Count();

    if ( kcount == 0 )// || iCalcMode == KAnimCalcModePaced)
	    {
        aValueIndex = 0;
        aSubAnimTime = 0;
        aAnimTime = x; //
        // No keytimes or paced animation
        if ( iCalcMode == KAnimCalcModeDiscrete )
            {
         // From SMIL 2.1 Animation Modules
         // Normative: Normative: A from-to animation with a from value vf
         // and a to value vt is equivalent to the same animation with a 
         // values list with 2 values, vf and vt. 
 
            if ( x < KTimeMax / 2 )
				{
                aAnimTime = 0;
				}
            else
				{
                aAnimTime = KTimeMax;
				}
            }
        }
    else if ( iCalcMode == KAnimCalcModePaced )
        {
        aAnimTime = x;
        TInt32 i = 0;
        while ( i < kcount && ( *iKeyTime )[i].iY < aAnimTime )
            i++;
        if ( i > 0 )
            i--;
        aValueIndex = i;
        // to avoid crashing for the array index aValueIndex + 1
		TBool IndexWasDecremented = EFalse;
        if(aValueIndex + 1 == kcount)
           	{
           	 aValueIndex--;
           	 IndexWasDecremented = ETrue;
           	}
        TInt32 dy = ( TInt32 )
                    ( ( *iKeyTime )[aValueIndex + 1].iY -
                      ( *iKeyTime )[aValueIndex].iY );
        if ( dy != 0 )
            aSubAnimTime = MULT255( aAnimTime - ( *iKeyTime )[aValueIndex].iY ) / dy;
        else
            aSubAnimTime = 0;

		if(IndexWasDecremented)
           	{
           	aValueIndex++;
           	}
        }
    else
        {
        // find corresponding keytime index
        TInt32 i = 0;
        while ( i < kcount && ( *iKeyTime )[i].iX < x )
            i++;
        if ( i > 0 )
            i--;


        ///// EXPLICITLY INCREMENT THE aValueIndex TO THE LAST OF THE LIST.

        if(x == 255)
	        {
	        aValueIndex = kcount-1;
	        }
	    else
	    	{
         	aValueIndex = i;
	    	}


        ////////////// END OF ADDITION ///////////////

        if ( iCalcMode == KAnimCalcModeDiscrete )
            {
            if ( x == KTimeMax )
                aValueIndex = iKeyTime->Count() - 1;
            aAnimTime = ( *iKeyTime )[aValueIndex].iY;
            aSubAnimTime = 0;
            }
        else
            {
            if ( iCalcMode == KAnimCalcModeLinear )
                {
                // calcMode:Linear


				if(( *iKeyTime )[i].iX == ( *iKeyTime )[i+1].iX)
					{
					while((i<(kcount-1)) && ( *iKeyTime )[i + 1].iX == ( *iKeyTime )[i].iX )
						{
						// this will increment the index so that
						// the interpolation would be done between the last
						// values.
						i++;
						// count should be less than the total number of elements.
						}


					}

				TInt32 alpha = MULT255( ( TInt32 )
                                        ( x - ( *iKeyTime )[i].iX ) ) /
                               ( TInt32 )
                               ( ( *iKeyTime )[i + 1].iX -
                                 ( *iKeyTime )[i].iX ) ;

                if(x == 255)
                	{
	             	aAnimTime = 255; // THIS SPECIFIES THAT THE DUR IS COMPLETE.
	               	}
				else
					{

					aAnimTime = CSvgAnimationBase::BlendInt( alpha,
	                                                         ( *iKeyTime )[i].iY,
	                                                         ( *iKeyTime )[i + 1].iY );
	                 }
                aSubAnimTime = alpha;

                }
            else
                {
                // calcMode:Spline
                TBool IndexWasDecremented = EFalse;
				if(aValueIndex + 1 == kcount)
					 {
					 aValueIndex--;
					 IndexWasDecremented = ETrue;
					 }
				aAnimTime = ( *iAnimTime )[x];
                TInt32 ydiff = ( TInt32 )
                               ( ( *iKeyTime )[aValueIndex + 1].iY -
                                 ( *iKeyTime )[aValueIndex].iY );
                if ( ydiff != 0 )
					{
                    aSubAnimTime = MULT255( aAnimTime -
                                            ( *iKeyTime )[aValueIndex].iY ) / ydiff;
					}
                else
					 {
                    aSubAnimTime = 0;
                }
				// oops increment again if it was decremented.
				if(IndexWasDecremented)
					 {
					 aValueIndex++;
					 }



                }

            if ( aAnimTime > KTimeMax )
				{
                aAnimTime = KTimeMax;
				}
			else
				{
				if ( aAnimTime < 0 )
					{
					aAnimTime = 0;
					}
				}

            }
        }
    }

// *******************************************************
// Create keytimes
// default is equally deived time
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgAnimTimeController::CreateKeyTime( TInt32 aKeyTimeCount )
    {
    // Create keyTime array if not available
	TInt lDivider = aKeyTimeCount;
	if ( iCalcMode != KAnimCalcModeDiscrete )
		{
		lDivider--;
		}

    if ( iKeyTime->Count() == 0 && aKeyTimeCount > 0 && lDivider != 0 ) // check to avoid division by zero.
        {
        for ( TInt32 i = 0; i < aKeyTimeCount; i++ )
            {
			AddKeyTime( TFloatFixPt( i ) / TFloatFixPt( lDivider ) );
            }
        }
    }

void CSvgAnimTimeController::CreateKeyTimeForEnumeration( TInt32 aKeyTimeCount )
    {
    // Create keyTime array if not available
	// ignore the calc mode specification.

	TInt lDivider = aKeyTimeCount - 1;
    if ( iKeyTime->Count() == 0 && aKeyTimeCount > 0 && lDivider != 0 ) // check to avoid division by zero.
        {
        for ( TInt32 i = 0; i < aKeyTimeCount; i++ )
            {
			AddKeyTime( TFloatFixPt( i ) / TFloatFixPt( lDivider ) );
            }
        }
    }

// Preparing animation time
// this should be called just before animation start
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::PrepareTimeL( const RArray<TFloatFixPt>* aValues )
    {
    TInt32 i;
    // set y for each keytime
    TInt32 count = iKeyTime->Count();

	// THIS IS MODIFIED FOR THE BUG FIX.
    if ( count <= 1)
		{
        return;
		}

    if ( iCalcMode == KAnimCalcModePaced )
        {
        if ( aValues == NULL )
			{
            return;
			}

        TInt32 valCount = aValues->Count();
        if ( valCount != count )
			{
            return; // No animation if values and keytimes has different number of items
			}

        // 'Paced' animation needs unique animation time from
        //  value difference
        RArray<TFloatFixPt>* yary = new ( ELeave ) RArray<TFloatFixPt>( 1 ); // SSB was 10
        CleanupStack::PushL( yary );
	
				#ifdef SVG_FLOAT_BUILD
        yary->AppendL( TFloatFixPt( 0.0f ) );
        TFloatFixPt sum( 0.0f );
        #else
        yary->AppendL( TFloatFixPt( 0, ETrue ) );
        TFloatFixPt sum( 0, ETrue );
        #endif
        
        for ( i = 1; i < valCount; i++ )
            {
            TFloatFixPt value = ( *aValues )[i] - ( *aValues )[i - 1];
            if ( value < TFloatFixPt( 0 ) )
            	{
            	value = TFloatFixPt(0) - value;
            	}
            yary->AppendL( value );
            sum += ( *yary )[i];
            }
        TFloatFixPt tmax( ( TInt32 ) KTimeMax );
        ( *iKeyTime )[0].iY = 0;


		for ( i = 1; i < valCount; i++ )
			{
			if( (TReal32)sum != 0) // check to avoid division by zero.
				{
				( *yary )[i] = ( ( *yary )[i] / sum ) * tmax; // need to make sure value does not exceed 0x7fff
				}
			else
				{
				( *yary )[i] = (TFloatFixPt) (0);
				}

			( *iKeyTime )[i].iY = ( TUint16 )
								  ( ( *iKeyTime )[i - 1].iY +
									( TUint16 ) ( TInt32 ) ( *yary )[i] );
			}
		( *iKeyTime )[count - 1].iY = KTimeMax;

        yary->Close();
        CleanupStack::PopAndDestroy( 1 ); // yary
        }
    else
		{
        for ( i = 0; i < count; i++ )
            {
            ( *iKeyTime )[i].iY = ( TUint16 ) ( i * KTimeMax / ( count -1  ) );
            }
		}
    // interpolate animation time (keySplines)
    for ( i = 1; i < KTimeMax; i++ )
        {
        if ( ( *iAnimTime )[i] == 0 )
            ( *iAnimTime )[i] = ( *iAnimTime )[i - 1];
        }
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::AddKeyTime( TFloatFixPt aTimerTime )
    {
    TKeyTime newKeytime;
    newKeytime.iX = ( TUint16 )
                    ( ( TInt32 ) ( aTimerTime * TFloatFixPt( KTimeMax ) ) );
    newKeytime.iY = 0;
    iKeyTime->Append( newKeytime );
    }

//
// This needs 'L' postfix or TRAP
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::AddKeySplineL( TFloatFixPt aX1,
                                            TFloatFixPt aY1,
                                            TFloatFixPt aX2,
                                            TFloatFixPt aY2 )
    {
    // keySplines must be keyTime-1
    if ( iSplineIndex >= iKeyTime->Count() - 1 )
		{
        return;
		}

    // Keytime must be set before adding keyspline
    CGfxGeneralPath* path = CGfxGeneralPath::NewLC();

    TFloatFixPt x0, y0, x3, y3;
    x0 = ( TInt32 ) ( *iKeyTime )[iSplineIndex].iX;
    y0 = ( TInt32 ) ( *iKeyTime )[iSplineIndex].iY;
    x3 = ( TInt32 )
         ( ( *iKeyTime )[iSplineIndex + 1].iX -
           ( *iKeyTime )[iSplineIndex].iX );
    y3 = ( TInt32 )
         ( ( *iKeyTime )[iSplineIndex + 1].iY -
           ( *iKeyTime )[iSplineIndex].iY );
    aX1 *= x3;
    aY1 *= y3;
    aX2 *= x3;
    aY2 *= y3;

    path->MoveToL( x0, y0, ETrue );
    path->CubicToL( aX1, aY1, aX2, aY2, x3, y3, EFalse );
    TGfxAffineTransform ident;

    CGfxFlatteningPathIterator* itr = CGfxFlatteningPathIterator::NewL( path,
                                                                        &ident,
                                                                        3 );

    CleanupStack::PushL( itr );
    TFloatFixPt tmpcoords[6];
    while ( !itr->IsDone() )
        {
        itr->CurrentSegment( tmpcoords );
        TInt32 x = tmpcoords[0];
        if ( 0 <= x && x <= 255 )
            {
            ( *iAnimTime )[x] = ( TUint8 ) ( TInt32 ) tmpcoords[1];
            }
        itr->NextL();
        }

    iSplineIndex++;
    CleanupStack::PopAndDestroy( 2 ); // path, itr
    }



// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------

TInt CSvgAnimTimeController::BeginTimesCount()
	{
	return iBeginTimeList->Count();
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetBeginTimeList(RArray<TInt32>*& aList)
	{
	if (iBeginTimeList)
		{
		iBeginTimeList->Reset();
		delete iBeginTimeList;
		iBeginTimeList= NULL;
		}

	iBeginTimeList= aList;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetKeyTimeArray(RArray<TKeyTime>*& aArray)
	{
	if (iKeyTime)
		{
		iKeyTime->Close();
		delete iKeyTime;
		iKeyTime= NULL;
		}

	iKeyTime= aArray;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::  SetAnimTimeArray(RArray<TUint32>*& aArray)
	{
	if (iAnimTime)
		{
		iAnimTime->Close();
		delete iAnimTime;
		iAnimTime= NULL;
		}

	iAnimTime= aArray;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimTimeController:: CalculateAnimTimes()
    {
    if(iRepeatDurationTime <= iDurationTime   )
	    {
        iNewActiveDurationTime = iDurationTime;
        iDurationTime = iRepeatDurationTime;

	    }
	if( iBeginTime == KTimeIndefinite || iRepeatDurationTime == KTimeIndefinite )
		{
		iEndTime = KTimeIndefinite;
		}

	}

void CSvgAnimTimeController:: SetOriginalValues_DOMReuse()
    {
    iOrgEndTime = iEndTime;
	iModifiedEndTime= iEndTime;
    iOrgDurationTime = iDurationTime;
	}
TUint32 CSvgAnimTimeController::GetOriginalEndTime()
{
return iOrgEndTime;
}
void CSvgAnimTimeController:: ReInitialize()
    {
     iEndTime = iOrgEndTime;
	 iDurationTime = iOrgDurationTime;
	}
void CSvgAnimTimeController::AddToInitialList(TInt aBeginTime)
    {
        if ( aBeginTime < 0 )
            {
            aBeginTime = 0;     
            }   
     iInitialBeginTimeList->Append( aBeginTime );
     iInitialBeginTimeList->SortSigned();
     
    }
void CSvgAnimTimeController:: ReInitializeForSeek()
    {
/*	if (iOrgEndTime == KTimeIndefinite)
		{
		if( iEndTime == KTimeIndefinite)
			{
            iEndTime= iModifiedEndTime;
			}
		}
	else
		{
		iEndTime = iOrgEndTime;
		}
*/
	iEndTime = iModifiedEndTime;
	 iDurationTime = iOrgDurationTime;
	}

// ---------------------------------------------------------------------------
// CSvgAnimTimeController::SetIsSplineCalcMode
//  Sets the calc mode to spline mode
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetIsSplineCalcMode( 
        const TBool aIsSplineCalcMode ) // Boolean indicating calcMode
    {
    iIsSplineCalcMode = aIsSplineCalcMode;
    }


// ---------------------------------------------------------------------------
// CSvgAnimTimeController::IsSplineCalcMode
//  Returns ETrue if the calc mode is in spline mode
// ---------------------------------------------------------------------------
TBool CSvgAnimTimeController::IsSplineCalcMode() const
    {
    return iIsSplineCalcMode;
    }

// ---------------------------------------------------------------------------
// CSvgAnimTimeController::SetSplineValueL
// Sets the spline parameter string for later processing when calc mode is set
// ---------------------------------------------------------------------------
void CSvgAnimTimeController::SetSplineValueL( const TDesC& aSplineValue )
    {
    iSplines = aSplineValue.AllocL();
    }

// ---------------------------------------------------------------------------
// CSvgAnimTimeController::SplineValue
//  Returns the string corresponding to the Spline parameters.
// ---------------------------------------------------------------------------
TPtrC CSvgAnimTimeController::SplineValue() const
    {
    if ( iSplines )
        {
        return iSplines->Des();
        }
    else
        {
        TPtrC lRetVal;
        lRetVal.Set( KEmptyString );
        return lRetVal;
        }
    }

void CSvgAnimTimeController::GetAnimTimeForEnumeration( TInt32 aTimerTime,
                                          TInt32& aAnimTime,
                                          TInt32& aValueIndex,
                                          TBool aKeyTimesPresent
                                           )
    {

	// This function is specifically for enumerations. i.e. "strings"
	// remember that these attributes  {"strings"} can not be interpolated
	// linearly so the calc mode for these should be
	// discrete irrespective of what is specified in the .svg file.


	// THE following code supports only "discrete" calc mode.

	if(  iDurationTime == KTimeIndefinite || iDurationTime == 0)
        {
        aValueIndex = 0;
        aAnimTime=0;
        return;
        }
    TInt32 x;

    if( iNewActiveDurationTime > 0 )//"repeatDur" <= "Dur"
		{
       	x = MULT255( aTimerTime - iBeginTime ) / iNewActiveDurationTime;
		}
    else
		{

	   	x = MULT255( aTimerTime - iBeginTime ) / iDurationTime;

		}

    if ( x > KTimeMax )
		{
        x = KTimeMax;
		}
	else
		{
		if ( x < 0 )
			{
			x = 0;
			}
		}

    TInt32 kcount = iKeyTime->Count();

    if ( kcount == 0 )
	    {
        aValueIndex = 0;
        aAnimTime = x; //
        // No keytimes or paced animation
           if ( x < KTimeMax / 2 )
				{
                aAnimTime = 0;
				}
            else
				{
                aAnimTime = KTimeMax;
				}

        }
     else
        {
        // find corresponding keytime index
        TInt32 i = 0;
		// earlier the last one was getting missed. this is specifically for
		// "string" data types.
        while ( i < kcount && ( *iKeyTime )[i].iX <= x )
			{
            i++;
			}
        if ( i > 0 )
			{
			i--;
			}

			/// PROPAGATE THE CHANGES TO THIS PLACE ALSO.
        if(x == 255)
	        {
	        aValueIndex = kcount-1;
	        }
	    else
	    	{
         	aValueIndex = i;
	    	}
	    	////// END OF ADDITION.

		if(i == (kcount-1))
			{
			i--;
			}
		// do not go down if "keyTimes" is specified in the file.

        if(aKeyTimesPresent)
	        {
	        return;
	        }

		///come to this place if keyTimes is not present.
		TInt32 alpha  = MULT255( ( TInt32 )
	                                        ( x - ( *iKeyTime )[i].iX ) ) /
	                               ( TInt32 )
	                               ( ( *iKeyTime )[i + 1].iX -
	                                 ( *iKeyTime )[i].iX ) ;



		// gives you AnimTime between 0-255 , this is used when KeyTimes is not
		// explicitly specified in the .svg file.
		aAnimTime = CSvgAnimationBase::BlendInt( alpha,
                                                         ( *iKeyTime )[i].iY,
                                                         ( *iKeyTime )[i + 1].iY );


		if(aAnimTime > KTimeMax)
			aAnimTime = KTimeMax;
        if(aAnimTime < 0)
			aAnimTime = 0;

        }
    }

//////////////////////////////////////
void CSvgAnimTimeController::CopyL(CSvgAnimTimeController* newElement)
{
	if(newElement)
		{
		newElement->iCalcMode = iCalcMode;
		newElement->iBeginTime = iBeginTime;
		newElement->iDurationTime = iDurationTime;
		newElement->iOrgDurationTime = iOrgDurationTime;
		newElement->iEndTime = iEndTime;
		newElement->iOrgEndTime = iOrgEndTime;
		newElement->iRepeatDurationTime = iRepeatDurationTime;
		newElement->iNewActiveDurationTime = iNewActiveDurationTime;
		newElement->iSplineIndex = iSplineIndex;
		newElement->iModifiedEndTime = iModifiedEndTime;
        newElement->iIsSplineCalcMode = EFalse;
        newElement->iSplines = NULL; // There is no parsing later
		TInt Count= iKeyTime->Count();
		newElement->iKeyTime->Reset();
		for(TInt i=0; i<Count; i++)
            {
            newElement->iKeyTime->AppendL(this->iKeyTime->operator[](i));
            }
        TInt Count2= iAnimTime->Count();
        newElement->iAnimTime->Reset();
        for(TInt i2=0; i2<Count2; i2++)
            {
            newElement->iAnimTime->AppendL(this->iAnimTime->operator[](i2));
            }
        TInt Count3= iBeginTimeList->Count();
        newElement->iBeginTimeList->Reset();
        for(TInt i3=0; i3<Count3; i3++)
            {
            newElement->iBeginTimeList->AppendL(this->iBeginTimeList->operator[](i3));
            }
		}

}
void CSvgAnimTimeController:: SetModifiedEndTime(TUint32 aTime)
    {
	if(aTime >= iBeginTime)
	    iModifiedEndTime = aTime;
	else
	    iModifiedEndTime = iBeginTime;

	}

void CSvgAnimTimeController:: Reset()
    {
   // iEndTime = iModifiedEndTime;

   if(iOrgEndTime == KTimeIndefinite)
		{
		iEndTime = iModifiedEndTime;
		}
	else
		{
		iEndTime= iOrgEndTime;
		}
	 iDurationTime = iOrgDurationTime ;


	}
void CSvgAnimTimeController:: SetEndTimesIndefinite()
    {
	iModifiedEndTime = KTimeIndefinite;
	iEndTime = KTimeIndefinite;
	}
void CSvgAnimTimeController::GetNextEndTime(TInt32 aBeginTime)
    {
    // this is similar to adobe.
	TInt lCount = iEndTimeList->Count();
	for(TInt i=0; i<lCount; i++)
		{
		if(iEndTimeList->operator[](i) >= aBeginTime)
			{
		 	iEndTime = iEndTimeList->operator[](i);
		 	return;
			}
		}
	iEndTime = KTimeIndefinite;
	}
void CSvgAnimTimeController::SetNextBeginTime(TInt32 aCurTime)
    {
    // Initially begintime is indefinite
    // checking beginlist
    if( aCurTime == KTimeIndefinite && iBeginTimeList->Count() > 0)
        {
        iBeginTime = iBeginTimeList->operator[](0);
        }
    // pick the begin time that is greater than the time passed
    else
        {
        TInt lCount = iBeginTimeList->Count();
    	for(TInt i=0; i<lCount; i++)
    		{
    		if(iBeginTimeList->operator[](i) >= aCurTime)
    			{
    		 	iBeginTime = iBeginTimeList->operator[](i);
    		 	return;
    			}
    		}
    	
    	iBeginTime = KTimeIndefinite;
	    }
	}
	
void CSvgAnimTimeController::SaveEndTime()	
    {
    iInitialEndTime = iEndTime;
    iInitialEndTimeList->Reset();
    for(TInt i=0; i<iEndTimeList->Count(); i++)
        {
        iInitialEndTimeList->Append(iEndTimeList->operator[](i));
        }
    }
void CSvgAnimTimeController::SaveBeginTime()
    {
    iInitialBeginTime = iBeginTime;
    iInitialBeginTimeList->Reset();
    for(TInt i =0; i<iBeginTimeList->Count(); i++)
        {
        iInitialBeginTimeList->Append(iBeginTimeList->operator[](i));
        }
    }
void CSvgAnimTimeController::ResetEndTime()
    {

    iEndTime = iInitialEndTime;
    iModifiedEndTime = iInitialEndTime;
    
    iEndTimeList->Reset();
    
    for(TInt i=0; i<iInitialEndTimeList->Count();i++)
        {
        iEndTimeList->Append(iInitialEndTimeList->operator[](i));
        }
    }
void CSvgAnimTimeController::ResetBeginTime()
    {
    iBeginTime = iInitialBeginTime;
    iBeginTimeList->Reset();
    for(TInt i=0; i<iInitialBeginTimeList->Count();i++)
        {
        iBeginTimeList->Append(iInitialBeginTimeList->operator[](i));
        }
    }
TInt32 CSvgAnimTimeController::LastEndTime()
    {
    if(iEndTimeList)
        {
        if(iEndTimeList->Count())
            {
            return iEndTimeList->operator[](iEndTimeList->Count() - 1);
            }
        else
            {
            return KTimeIndefinite;
            }    
        }
    else
        {
        return KTimeIndefinite;
        }
    }
TInt32 CSvgAnimTimeController::LastBeginTime()
    {
    if(iBeginTimeList)
        {
        if(iBeginTimeList->Count())
            {
            return iBeginTimeList->operator[](iBeginTimeList->Count() - 1);
            }
        else
            {
            return KTimeIndefinite;
            } 
        }
    else
        {
        return KTimeIndefinite;
        }
    }
    
