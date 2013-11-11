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


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGAnimateElementImpl.h"

#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"




// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgAnimateElementImpl* CSvgAnimateElementImpl::NewL(  const TUint8 aElemID,
                                                      CSvgDocumentImpl* aDoc )
    {
    CSvgAnimateElementImpl* self    = new ( ELeave )
                                      CSvgAnimateElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateElementImpl* CSvgAnimateElementImpl::NewLC(  const TUint8 aElemID,
                                                       CSvgDocumentImpl* aDoc )
    {
    CSvgAnimateElementImpl* self    = new ( ELeave )
                                      CSvgAnimateElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }

// *******************************************************
// Protected

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgAnimationBase::ConstructL( aElemID );

    iReqAttrFlag=KSVG_ANIMATE_ELEMFLAG;
	iIsInheritedAttribute = EFalse;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateElementImpl::~CSvgAnimateElementImpl()
    {
    }

// *******************************************************
// Protected


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateElementImpl::CSvgAnimateElementImpl( CSvgDocumentImpl* aDoc ) : CSvgAnimationBase( aDoc )
    {
    }

// *******************************************************
// From SVG DOM


// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::ResetReferenceElementL()
    {
    if(iTargetElement != NULL )
        {
        if( ((iTargetElement->iAnimateAttrSet == NULL) || (iTargetElement->iAnimateAttrSet->Find(iAttrId) == KErrNotFound) ) )
            {
            if  ( iInitDone && iAnimStatus != KAnimNotActive)
                {
				iCheckFirstAnim= ETrue;
                SetToOriginalL();
                if( !(iTargetElement->iAnimateAttrSet) )
                    {
                    iTargetElement->iAnimateAttrSet = new (ELeave) RArray<TUint32>(1);
                    }
                iTargetElement->iAnimateAttrSet->AppendL((TUint32)(iAttrId));
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
TBool CSvgAnimateElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    return CSvgAnimationBase::ReceiveEventProcL( aEvent, this );
    }


// ---------------------------------------------------------------------------
// From CSvgAnimationBase
// ---------------------------------------------------------------------------
//
TBool CSvgAnimateElementImpl::AnimProcL( MSvgTimerEvent* aEvent )

    {

    if(iAnimStatus == KAnimActive && !iHadBegun)
		{
		SetFromValuesL();
		iHadBegun = ETrue;

		}


	if((iAnimStatus != KAnimActive) || iDoFreeze)
	   {
	   if(iFill== KAnimFillFreeze && !iDoFreeze && iAnimStatus == KAnimFinished)
	   		{

	   		}
	   else
		   {
		   CSvgAnimationBase::CheckForEndTimesAndFreezeL(this);
		   return EFalse;
		   }
	   }

    // Calc alpha from time [0-255]
    TInt32  alpha;
    TInt32  valix, subanimtime;

    switch ( iDataType )
        {
        // x, y, width, height,
        // rx, ry, cx, cy, radius
        // x1, y1, x2, y2
        case KSvgTypeOpacity:
        case KSvgTypeLength:
            {
			iAnimTime->GetAnimTime( (aEvent->Time()-iNegativeBeginTime) , alpha, valix, subanimtime );
            TFloatFixPt  v1  = iFromFloat;
            TFloatFixPt  v2  = iToFloat;
            TFloatFixPt  lCurValue;
            if(iHaveBy &&  iCurrentRepeatCount > 0 && iAccumulate == KAccumSum )
                {
                v1= KFloatFixZero;
                v2 = iToFloat - iFromFloat;
                }
            if ( iValuesFloat->Count() > 0 )
                {

                // check for the range of valix
				if(valix >= iValuesFloat->Count() )
					{
					valix = iValuesFloat->Count() - 1;
					}

                v1 = ( *iValuesFloat )[valix];
                if ( valix + 1 >= iValuesFloat->Count() )
                    {
                    v2 = ( *iValuesFloat )[valix];
                    }
                else
                    {
                    v2 = ( *iValuesFloat )[valix + 1];
                    }
                alpha = subanimtime;
                }
            lCurValue = BlendFloat( alpha, v1, v2 );
            if( iAccumulate == KAccumSum && !iNoFrom )
                {
                TFloatFixPt lRepeatCount((TInt) iCurrentRepeatCount) ;
                if(iValuesFloat->Count() > 0)
                    {
                    lCurValue += ((*iValuesFloat) [iValuesFloat->Count() -1]) * lRepeatCount ;
                    }
                else
                    {
                    TFloatFixPt lRepeatCount((TInt) iCurrentRepeatCount) ;
                    lCurValue += lRepeatCount * (v2);
                    if(iHaveBy &&  iCurrentRepeatCount > 0)
                        {
                        lCurValue += iFromFloat;
                        }
                    }
                }
            if ( ( iFill == KAnimFillFreeze ) )
                {
                iEndFloat = lCurValue;
                }
            if ( ( iAdditive == KAdditiveSum ) && !iNoFrom ) //.
                {
                TFloatFixPt  lOrig;
                iTargetElement->GetAttributeFloat( iAttrId, lOrig );
                lCurValue += lOrig;
                }
            iTargetElement->SetAttributeFloatL( iAttrId, lCurValue );

            }
            break;
        case KSvgTypeList:
        case KSvgTypePath:
            {
			iAnimTime->GetAnimTime( (aEvent->Time()-iNegativeBeginTime), alpha, valix, subanimtime );
            CGfxGeneralPath*v1  = iFromPath;
            CGfxGeneralPath*v2  = iToPath;

            if ( iValuesPath->Count() > 0 )
                {

				// check for the range of valix
				if(valix >= iValuesPath->Count() )
					{
					valix = iValuesPath->Count() - 1;
					}

                v1 = iValuesPath->operator[]( valix );
                if ( valix + 1 >= iValuesPath->Count() )
                    {
                    v2 = iValuesPath->operator[]( valix );
                    }
                else
                    {
                    v2 = iValuesPath->operator[]( valix + 1 );
                    }
                alpha = subanimtime;
                }

            CGfxGeneralPath*lCurPath    = CGfxGeneralPath::NewLC();

            BlendPathL( alpha, v1, v2, lCurPath );

            if ( iAdditive == KAdditiveSum )
                {
                AddPathsL( lCurPath, iOrgPath );
                }
            else
                {
                // User current blended value
                }
            if ( ( iFill == KAnimFillFreeze ) )
                {
                CopyPathsL( iEndPath, lCurPath );
                }

            iTargetElement->SetAttributePathL( iAttrId, lCurPath );

            CleanupStack::PopAndDestroy( 1 ); // lCurPath

            }
            break;
        case KSvgTypeTransform:
        break;

        case KSvgTypeColor:
            {
			iAnimTime->GetAnimTime((aEvent->Time()-iNegativeBeginTime), alpha, valix, subanimtime );
            TInt32  v1  = iFromInt;
            TInt32  v2  = iToInt;
            TInt32  lCurValue;
            if(iHaveBy &&  iCurrentRepeatCount > 0 && iAccumulate == KAccumSum )
                {
                v1= 0;
                v2 = SubtractColor(iToInt,iFromInt);
                }

            if ( iValuesInt->Count() > 0 )
                {

                // check for the range of valix
				if(valix >= iValuesInt->Count() )
					{
					valix = iValuesInt->Count() - 1;
					}

                v1 = ( *iValuesInt )[valix];
                if ( valix + 1 >= iValuesInt->Count() )
                    {
                    v2 = ( *iValuesInt )[valix];
                    }
                else
                    {
                    v2 = ( *iValuesInt )[valix + 1];
                    }
                alpha = subanimtime;
                }

                if ( iNoFrom ) //. This is to emulate Adobe.
                    {
                    if ( alpha > ( KTimeMax >> 1 ) )
                        {
                        alpha = KTimeMax;
                        }
                    else
                        {
                        alpha = 0;
                        }
                    }

            lCurValue = BlendColor( alpha, v1, v2 );
            if( iAccumulate == KAccumSum && !iNoFrom )
                {


                    if(iValuesInt->Count() > 0)
                        {
                            TInt32  ColorToAdd = 0;
                            ColorToAdd = ((*iValuesInt) [iValuesInt->Count() -1]);
                            for( TInt i=0 ; i< iCurrentRepeatCount ; i++)
                                {
                                lCurValue = AddColor( lCurValue,  ColorToAdd );
                                }

                        }
                    else
                        {
                            for (TInt i=0; i<iCurrentRepeatCount;i++)
                                {
                                lCurValue = AddColor( lCurValue, v2 )   ;
                                }

                            if(iHaveBy &&  iCurrentRepeatCount > 0)
                                {

                                lCurValue = AddColor( lCurValue,  iFromInt );
                                }

                        }
                }

            		// Fix for accum_addit_7.svg
            if ( ( iFill == KAnimFillFreeze ) )
                {
                iEndInt = lCurValue;
                iEndInt = lCurValue;
                }
            if ( ( iAdditive == KAdditiveSum ) && !iNoFrom ) //.
                {
                TInt32  lOrig;
                iTargetElement->GetAttributeIntL( iAttrId, lOrig );



                lCurValue = AddColor( lCurValue, lOrig );
                }

            iTargetElement->SetAttributeIntL( iAttrId, lCurValue );

            }
            break;
        case KSvgTypeTextAnchor:
        case KSvgTypeVisibility:
        case KSvgTypeGradientUnits:
        case KSvgTypeSpreadMethod:
        case KSvgTypeDisplay:
            {
			iAnimTime->GetAnimTimeForEnumeration( (aEvent->Time()-iNegativeBeginTime), alpha, valix, iKeyTimesPresent);
            TInt32  v1  = iFromInt;
            TInt32  v2  = iToInt;
            TInt32  lCurValue;
            if ( iValuesInt->Count() > 0 )
                {

				  // check for the range of valix
				if(valix >= iValuesInt->Count() )
					{
					valix = iValuesInt->Count() - 1;
					}

                if(iKeyTimesPresent)
					{
					if ( valix < iValuesInt->Count() )
						{
           				lCurValue = (*iValuesInt)[valix];
						}
					else
						{
						lCurValue = (*iValuesInt)[valix-1];
						}
					}
				else
					{
                TInt index = ( alpha * iValuesInt->Count() / 255 ) ;
					if(index == iValuesInt->Count()) index--;
                if ( index >= 0 )
                    {
                    lCurValue = (*iValuesInt)[index];
                    }
                else
                    {
                    lCurValue = (*iValuesInt)[0];
                    }
                }

                }

            else
                {
                if(alpha>=128) // average of 256 to determine which value should predominate
                    {
                    lCurValue = v2;
                    }
                else
                    {
                    lCurValue = v1;
                    }
                }
            if ( ( iFill == KAnimFillFreeze ) )
                {
                iEndInt = lCurValue;
                }

            iTargetElement->SetAttributeIntL( iAttrId, lCurValue );

            }
	       break;

		case KSvgTypeViewBox:
			 {
             DoViewBoxAnimProcL(aEvent);
             break;
			 }
        }
    CSvgAnimationBase::CheckForEndTimesAndFreezeL(this);
    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::BlendPathL( TInt32 aAlpha,
                                         CGfxGeneralPath* aFromPath,
                                         CGfxGeneralPath* aToPath,
                                         CGfxGeneralPath*& aBlendPath )
    {
    if ( aFromPath && aToPath )
        {
        aBlendPath->SetPointTypeArrayL( aFromPath->PointTypeArray() );

        RArray<TFloatFixPt>* lCurPoints  = new ( ELeave ) RArray<TFloatFixPt>( 20 );
        CleanupStack::PushL( lCurPoints );
        lCurPoints->AppendL(NULL);
        lCurPoints->Remove(0);

	    RArray<TFloatFixPt>* fromPoints  = aFromPath->PointCoordsArrayAll();
	    RArray<TFloatFixPt>* toPoints    = aToPath->PointCoordsArrayAll();

	    TInt            lFromcount       = fromPoints->Count();
		TInt            lTocount       = toPoints->Count();
	    TInt            i           = 0;
	    TFloatFixPt          lCurPoint;

	    while (i < lFromcount && i < lTocount )
	        {
	        lCurPoint = BlendFloat( aAlpha,
	                                fromPoints->operator[]( i ),
                                    toPoints->operator[]( i ) );
            lCurPoints->AppendL( lCurPoint );
            i++;
            }

        aBlendPath->SetPointCoordsArrayL( lCurPoints );

	    lCurPoints->Close();
	    CleanupStack::PopAndDestroy( 1 ); // lCurPoints
	    }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::AddPathsL( CGfxGeneralPath*& aPathResult,
                                        CGfxGeneralPath* aPath1 )
    {
    RArray<TFloatFixPt>* lCurPoints  = new ( ELeave ) RArray<TFloatFixPt>( 20 );
    CleanupStack::PushL( lCurPoints );
    lCurPoints->AppendL(NULL);
    lCurPoints->Remove(0);

    RArray<TFloatFixPt>* points1 = aPath1->PointCoordsArrayAll();
    RArray<TFloatFixPt>* result  = aPathResult->PointCoordsArrayAll();
    TInt            pcount   = points1->Count();
    TInt            rcount   = result->Count();
    TInt            i       = 0;
    TFloatFixPt          lCurPoint;

    while ( i < pcount && i < rcount )
        {
        lCurPoint = points1->operator[]( i ) + result->operator[]( i );
        lCurPoints->AppendL( lCurPoint );
        i++;
        }

    aPathResult->SetPointCoordsArrayL( lCurPoints );

    lCurPoints->Close();
    CleanupStack::PopAndDestroy( 1 ); // lCurPoints
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::CopyPathsL( CGfxGeneralPath*& aPathResult,
                                         CGfxGeneralPath* aPath1 )
    {
    RArray<TFloatFixPt>* lCurPoints  = new ( ELeave ) RArray<TFloatFixPt>( 20 );
    CleanupStack::PushL( lCurPoints );

    lCurPoints->AppendL(NULL);
    lCurPoints->Remove(0);
    RArray<TFloatFixPt>* points1 = aPath1->PointCoordsArrayAll();

    TInt            count   = points1->Count();
    TInt            i       = 0;
    TFloatFixPt          lCurPoint;

    while ( i < count )
        {
        lCurPoint = points1->operator[]( i );
        lCurPoints->AppendL( lCurPoint );
        i++;
        }

    aPathResult->SetPointCoordsArrayL( lCurPoints );

    lCurPoints->Close();
    CleanupStack::PopAndDestroy( 1 ); // lCurPoints
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::ResetAnimationL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateElementImpl::SetAttributeIntL( const TInt aNameId, const TInt32 aValue)
{
	return CSvgAnimationBase::SetAttributeIntL(aNameId,aValue);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	return CSvgAnimationBase::GetAttributeIntL(aNameId,aValue);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateElementImpl::SetAttributeFloatL(const TInt aNameId, TFloatFixPt aValue )
{
	return CSvgAnimationBase::SetAttributeFloatL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateElementImpl::GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue )
{
	return CSvgAnimationBase::GetAttributeFloat(aNameId,aValue);
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgAnimateElementImpl::ReInitializeAnimation()
    {

	if ( iAttrId == 0xffff )
        return;

	CSvgAnimationBase::ReInitializeAnimation();

	if(iCheckFirstAnim)
		{
		if(iIsInheritedAttribute)
			{
			iTargetElement->PointToParent(iAttrId);
			}
		else
			{
			TRAPD(error, SetToOriginalL());

				if (error != KErrNone)
				{
					// error processing not processed
					return;
				}
			}
		}


    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void  CSvgAnimateElementImpl::SetOriginalValues_DOMReuse()
	{
	 // calling base class function.
	CSvgAnimationBase::SetOriginalValues_DOMReuse();

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CGfxGeneralPath* CSvgAnimateElementImpl::GetPathAttribute(TInt aAttributeId)
	{
	return 	CSvgAnimationBase::GetPathAttribute(aAttributeId);

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
	{

		CSvgAnimationBase::SetPathAttribute(aAttributeId, aPathHandle);

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::Reset(MSvgEvent* aEvent)
	{

	iIsUserSeek = ETrue;

	// first call the animation base function.
	TSvgTimerEvent* timerEvent  = ( TSvgTimerEvent* ) aEvent;


	if((TInt32)timerEvent->Time() < iAbsoluteBeginTime)
		{
		// let it come to initial position.
		((CSvgDocumentImpl*)iOwnerDocument)->iInitialDrawFlag = ETrue;
		ReInitializeAnimation();
		return;
		}
	TRAPD(error,CSvgAnimationBase::ResetL( aEvent, this));

		if (error != KErrNone)
		{
			// error processing not processed
			return;
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
/////////////////////////////////////////////
MXmlElement* CSvgAnimateElementImpl::CloneL(MXmlElement*
aParentElement)
	{
		CSvgAnimateElementImpl* newElement = CSvgAnimateElementImpl::NewL(  this->ElemID(),
											((CSvgDocumentImpl*)iOwnerDocument) );
		CleanupStack::PushL( newElement );
		newElement->iParentNode = aParentElement;
		// copy the data
		// end copying data from this class.
		// this will get cloned setting the parent element to aParentElement;

		newElement->iOwnerDocument = this->iOwnerDocument;

		// set the target element this needs to be modified.
		newElement->iTargetElement = (CSvgElementImpl*)aParentElement;
		this->CopyL(newElement);
		CleanupStack::Pop();
		return newElement;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::CopyL(CSvgAnimateElementImpl* aDestElement )
    {
    if(aDestElement)
	    {
	    // copy from superclass
	    this->CSvgAnimationBase::CopyL(aDestElement);
	    // copy reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
	    }
   }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::DeactivateAnimation()
	{
	CSvgAnimationBase::DeactivateAnimation(this);
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// for calculation of interpolated values for viewBox animation.
void CSvgAnimateElementImpl::DoViewBoxAnimProcL(MSvgTimerEvent*  aEvent)
	{

    TInt32  alpha;
    TInt32  valix;
    TInt32  subanimtime;
	TReal32 vBoxX=0;
	TReal32 vBoxY=0;
	TReal32 vBoxW=0;
	TReal32 vBoxH=0;

	iAnimTime->GetAnimTime( (aEvent->Time()-iNegativeBeginTime), alpha, valix, subanimtime );
    TGfxRectangle2D  v1  = iFromViewBox;
    TGfxRectangle2D  v2  = iToViewBox;
    TGfxRectangle2D  lCurValue;
    if(iHaveBy &&  iCurrentRepeatCount > 0 && iAccumulate == KAccumSum )
        {
        v1= TGfxRectangle2D( vBoxX, vBoxY, vBoxW, vBoxH );
        vBoxX = iToViewBox.iX -  iFromViewBox.iX;
		vBoxY = iToViewBox.iY -  iFromViewBox.iY;
		vBoxW = iToViewBox.iWidth -  iFromViewBox.iWidth;
		vBoxH = iToViewBox.iHeight -  iFromViewBox.iHeight;
        v2 = TGfxRectangle2D( vBoxX, vBoxY, vBoxW, vBoxH );
        }

    if ( iValuesViewBox->Count() > 0 )
       {

		// check for the range of valix
		if(valix >= iValuesViewBox->Count() )
			{
			valix = iValuesViewBox->Count() - 1;
			}

		v1 = ( *iValuesViewBox )[valix];
        if ( valix + 1 >= iValuesViewBox->Count() )
           {
           v2 = ( *iValuesViewBox )[valix];
           }
        else
           {
           v2 = ( *iValuesViewBox )[valix + 1];
           }
           alpha = subanimtime;
       }

			// to get the interpolation of each values of the viewBox
	vBoxX = BlendFloat( alpha, v1.iX, v2.iX );
	vBoxY = BlendFloat( alpha, v1.iY, v2.iY );
	vBoxW = BlendFloat( alpha, v1.iWidth, v2.iWidth );
	vBoxH = BlendFloat( alpha, v1.iHeight, v2.iHeight );

	lCurValue = TGfxRectangle2D( vBoxX, vBoxY, vBoxW, vBoxH );

    if( iAccumulate == KAccumSum && !iNoFrom)
		{
        TFloatFixPt lRepeatCount((TInt) iCurrentRepeatCount) ;
        if(iValuesViewBox->Count() > 0)
            {
			TGfxRectangle2D  lVal= ((*iValuesViewBox) [iValuesViewBox->Count() -1]);

            lCurValue.iX += lVal.iX * lRepeatCount;
			lCurValue.iY += lVal.iY * lRepeatCount;
			lCurValue.iWidth += lVal.iWidth * lRepeatCount;
			lCurValue.iHeight += lVal.iHeight * lRepeatCount;
            }
        else
           {
           lCurValue.iX += v2.iX * lRepeatCount;
		   lCurValue.iY += v2.iY * lRepeatCount;
			lCurValue.iWidth += v2.iWidth * lRepeatCount;
	    	lCurValue.iHeight += v2.iHeight * lRepeatCount;
           if(iHaveBy &&  iCurrentRepeatCount > 0)
                {
                lCurValue.iX += iFromViewBox.iX ;
				lCurValue.iY += iFromViewBox.iY ;
				lCurValue.iWidth += iFromViewBox.iWidth ;
				lCurValue.iHeight += iFromViewBox.iHeight ;
                }
           }
       }

   if ( ( iFill == KAnimFillFreeze ) )
      {
      iEndViewBox = lCurValue;
      }
   if ( ( iAdditive == KAdditiveSum ) && !iNoFrom ) //.
      {
      TGfxRectangle2D  lOrig;
      if(iTargetElement->ElemID() == KSvgSvgElement)
		{
		((CSvgSvgElementImpl*)iTargetElement)->GetViewBox( lOrig );
		}

      lCurValue.iX += lOrig.iX ;
      lCurValue.iY += lOrig.iY ;
	  lCurValue.iWidth += lOrig.iWidth ;
	  lCurValue.iHeight += lOrig.iHeight ;
      }

    // set the value to the target element.
	 if(iTargetElement->ElemID() == KSvgSvgElement)
		{
		((CSvgSvgElementImpl*)iTargetElement)->SetViewBoxL( lCurValue );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

       TBool CSvgAnimateElementImpl::DoAnimProcL(MSvgEvent* aEvent)
	{
	return this->AnimProcL((MSvgTimerEvent*)aEvent);
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::InitAnimationL()
	{

	if(!iTargetElement)
		{
		// if target element is not found then remove it.
		((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this);
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
		return;
		}

	if (!iHaveValues)
		{
		if (!iHaveTo && !iHaveBy)
			{
			((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this );
            ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
			}
		}

	iIsInheritedAttribute = iTargetElement->IsInherited(iAttrId);
	CSvgAnimationBase::InitAnimationL();
	SetFillValueL();

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateElementImpl::SetToOriginalL()
	{

	// this is to make sure the inherited style attributes are made point back to the parent.
	if(iIsInheritedAttribute && iAnimStatus == KAnimFinished)
		{
		// only if the animation has finished
		iTargetElement->PointToParent(iAttrId);
		}
	else
		{
		CSvgAnimationBase::SetToOriginalL();
		}
	}

void CSvgAnimateElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<animate attributeType=\"hmmm\" attributeName=\"hmm\" from=\"%d\" to=\"%d\" dur=\"hmm\" repeatCount=\"hmm\"/>",
		/*iDataType,*/ (int)iFromFloat, (int)iToFloat/*, iRepeatCount*/);
		#endif
	}
}














