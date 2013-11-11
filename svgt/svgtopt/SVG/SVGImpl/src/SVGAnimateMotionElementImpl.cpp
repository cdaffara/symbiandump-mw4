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
#include <e32math.h>

#include "SVGAnimateMotionElementImpl.h"

#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxGeneralPath.h"
#include "GfxFlatteningPathIterator.h"
#include "SVGPathDataParser.h"
#include "SVGPointLexer.h"
#include "SVGStringTokenizer.h"

_LIT( AUTO, "auto" );
_LIT( AUTOREVERSE, "auto-reverse" );
_LIT( SEMICOLON, ";" );

// *******************************************************
// Constructor/deconstructor

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateMotionElementImpl* CSvgAnimateMotionElementImpl::NewL(  const TUint8 aElemID,
																  CSvgDocumentImpl* aDoc )
	{
	CSvgAnimateMotionElementImpl*   self    = new ( ELeave )
											  CSvgAnimateMotionElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateMotionElementImpl* CSvgAnimateMotionElementImpl::NewLC(  const TUint8 aElemID,
																   CSvgDocumentImpl* aDoc )
	{
	CSvgAnimateMotionElementImpl* self = new ( ELeave )
											  CSvgAnimateMotionElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgAnimationBase::ConstructL( aElemID );

	iMotionPath = CGfxGeneralPath::NewL();
	iAnimTime->SetCalMode( KAnimCalcModePaced ); // animateMotion has differenr default
	iReqAttrFlag=KSVG_AMINATEMO_ELEMFLAG;
	}



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateMotionElementImpl::~CSvgAnimateMotionElementImpl()
    {
	if ( iMotionPath )
		{
		delete iMotionPath;
		iMotionPath = NULL;
		}
	if ( iPathIterator )
		{
		delete iPathIterator;
		iPathIterator = NULL;
		}
    }

// *******************************************************
// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateMotionElementImpl::CSvgAnimateMotionElementImpl( CSvgDocumentImpl* aDoc ) : CSvgAnimationBase( aDoc ),
                                                                                       iCurrentSeg( 0,
                                                                                                    0,
                                                                                                    0,
                                                                                                    0 ),
                                                                                       //iIsValues( EFalse ),
                                                                                       iPrevRotate(0)
                                                                                       
    {
    iAttrId = KAtrTransform;
    }

// *******************************************************
// From SVG DOM


// *******************************************************
// SVG Implementation


// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::ResetReferenceElementL()
    {
    if(iTargetElement != NULL )
    	{
    	if( ((iTargetElement->iAnimateAttrSet == NULL) ||
    	    (iTargetElement->iAnimateAttrSet->Find(iAttrId) == KErrNotFound) ) )
    		{
    		if ( iInitDone )
    			{
    			iCheckFirstAnim= ETrue;
    			SetToOriginalL();
    			if( !(iTargetElement->iAnimateAttrSet) )
    				{
                    iTargetElement->iAnimateAttrSet = new (ELeave) RArray<TUint32>(1);
                    }
                iTargetElement->iAnimateAttrSet->AppendL((TUint32)iAttrId);
    			}
    		}
        }
    }
// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::SetAttributeL( const TDesC& aName,
                                                  const TDesC& aValue )
    {
    _LIT( KTmpAttrName, "attributeName" );
    _LIT( KTmpPath, "path" );
    _LIT( KTmpRotate, "rotate" );
    _LIT( KTmpFrom, "from" );
    _LIT( KTmpTo, "to" );
    _LIT( KTmpValues, "values" );
    _LIT( KTmpBy, "by" );


    if ( aName == KTmpAttrName )
        {
        // Ignore 'attributeName' attribute
        }
    else if ( aName == KTmpPath )
        {
        iMotionPath->Reset();
		TSvgPathDataParser::ParsePathData( aValue, iMotionPath );

        CGfxPathIterator*   itr;
        TFloatFixPt              tmpcoords[6];
        iMotionPath->GetPathIteratorL( &iIdentTransform, itr );
        CleanupStack::PushL( itr );

        while ( !itr->IsDone() )
            {
            switch ( itr->CurrentSegment( tmpcoords ) )
                {
                case EGfxSegMoveTo:
                iToFloatX = tmpcoords[0];
                iToFloatY = tmpcoords[1];
                break;
                case EGfxSegLineTo:
                iToFloatX = tmpcoords[0];
                iToFloatY = tmpcoords[1];
                break;
                case EGfxSegQuadTo:
                iToFloatX = tmpcoords[2];
                iToFloatY = tmpcoords[3];
                break;
                case EGfxSegCubicTo:
                iToFloatX = tmpcoords[4];
                iToFloatY = tmpcoords[5];
                break;
                case EGfxSegClose:
                iToFloatX = tmpcoords[0]; //.
                iToFloatY = tmpcoords[1];
                }

            itr->NextL();
            }
        iPathSeen = ETrue;
        CleanupStack::PopAndDestroy();
		if(iReqAttrFlag == KAtrSVGAmo && (iMotionPath->PointTypeArray())->Count() != 0 )
			{
			iReqAttrFlag = 0;
			}
        }
	else if ( aName == KTmpRotate )
		{
		if ( aValue == AUTO )
			{
			iAnimMotionRotate = KAnimMotionRotateAuto;
			}
		else if ( aValue == AUTOREVERSE )
			{
			iAnimMotionRotate = KAnimMotionRotateAutoReverse;
			}
		else
			{
			TLex    lString ( aValue );
			if (lString.Val( iAnimMotionRotate )!= KErrNone)
				{
				iAnimMotionRotate= 0;
				}
			if ( iAnimMotionRotate < 0 )
				{
					// Note: A problem may occur if the value is less than -3600
	        iAnimMotionRotate += 3600;
				}
			iAnimMotionRotate %= 360;
			}
		}
    else if ( aName == KTmpFrom )
	    {
		if ( ! iHaveValues )
			{
			iNoFrom = EFalse;
			iMotionPath->Reset();
			TSvgPointLexer  svgplex ( aValue );
			TChar           com;
			TFloatFixPt          x, y;

            svgplex.GetNextPoint( com, x, y );
            svgplex.Cleanup();

			iOrgFromFloatX = iFromFloatX = x;
			iOrgFromFloatY = iFromFloatY = y;
			iMotionPath->MoveToL( x, y, ETrue );  // 'from' -> MoveTo
			}

		if(iReqAttrFlag == KAtrSVGAmo)
			{
			iReqAttrFlag = KAtrToBy;
			}
		else
			{
			iReqAttrFlag = 0;
			}
		}
    else if ( aName == KTmpBy )
	    {
		if ( !iHaveTo )
			{
			iHaveBy = ETrue;
			if(iReqAttrFlag == KAtrSVGAmo || iReqAttrFlag == KAtrToBy)
				{
				iReqAttrFlag = 0;
				}
		    if ( !iHaveValues )
			    {
	               TSvgPointLexer  svgplex ( aValue );
	            TChar           com;
		        TFloatFixPt          x, y;

                svgplex.GetNextPoint( com, x, y );
                svgplex.Cleanup();

                if ( iNoFrom )
	                {
		            iToFloatX = x;
			        iToFloatY = y;
				    iNoFrom = EFalse;
				    // this is taken from animate transforms.
				   iAdditive = KAdditiveSum;
				   iAdditiveOrg = KAdditiveSum;
					}
                else
	                {
		            iToFloatX = iFromFloatX + x;
			        iToFloatY = iFromFloatY + y;
				    }

	            CGfxPathIterator*   itr;
		        iMotionPath->GetPathIteratorL( &iIdentTransform, itr );
			    CleanupStack::PushL( itr );

				if ( itr->IsDone() )
					{
					TFloatFixPt lZero;
					// If the animation path is empty, fill with default value.
					iMotionPath->MoveToL( lZero, lZero, ETrue );
					}
				CleanupStack::PopAndDestroy( 1 ); // itr
				iMotionPath->LineToL( x, y, ETrue );  // 'by' -> LineTo
				}
			}

		if(iReqAttrFlag == KAtrSVGAmo && (iMotionPath->PointTypeArray())->Count() != 0 )
			{
			iReqAttrFlag = 0;
			}
	    }
	else if ( aName == KTmpTo )
		{
		if ( !iHaveValues )
			{

			// this is taken from animate transforms.
			if ( iNoFrom )
               {
	           iAdditive = KAdditiveSum;
			   iAdditiveOrg = KAdditiveSum;
			   }
			TSvgPointLexer  svgplex ( aValue );
			TChar           com;
			TFloatFixPt          x, y;
			iHaveTo = ETrue;
            svgplex.GetNextPoint( com, x, y );
            svgplex.Cleanup();

            iToFloatX = x;
			iToFloatY = y;
			CGfxPathIterator*   itr;
			iMotionPath->GetPathIteratorL( &iIdentTransform, itr );
			CleanupStack::PushL( itr );
			if ( itr->IsDone() )
				{
				TFloatFixPt lZero;
				// If the animation path is empty, fill with default value.
				// should this default be zero or object position
				iMotionPath->MoveToL( lZero, lZero, ETrue );
				}
			CleanupStack::PopAndDestroy( 1 ); // itr
			iMotionPath->LineToL( x, y, ETrue );  // 'to' -> LineTo
			}

		if(iReqAttrFlag == KAtrSVGAmo || iReqAttrFlag == KAtrToBy)
			{
			iReqAttrFlag = 0;
			}
		}
	else if ( aName == KTmpValues )
		{
		// Process only if Path values have not been seen
		if ( !iPathSeen )
		    {
    		TStringTokenizer    tkn     ( aValue, SEMICOLON );
    		TBool               first   = ETrue;
    		iHaveValues = ETrue;
    		iNoFrom = EFalse; //.
    		iMotionPath->Reset();
    		while ( tkn.HasMoreTokens() ) // 'values' is a set of coordinates
    			{
    			TSvgPointLexer  svgplex ( tkn.NextToken() );
    			TChar           com;
    			TFloatFixPt          x, y;
                svgplex.GetNextPoint( com, x, y );
                svgplex.Cleanup();

    			if ( first )
    				{
    				iMotionPath->MoveToL( x, y, ETrue );
    				iOrgFromFloatX = iFromFloatX = x; //.
    				iOrgFromFloatY = iFromFloatY = y;
    				first = EFalse;
    				}
    			else
    				{
    				iMotionPath->LineToL( x, y, ETrue );
    				}
    			iToFloatX = x; //.
    			iToFloatY = y;
    			}

    		if(iReqAttrFlag == KAtrSVGAmo || iReqAttrFlag == KAtrToBy)
    			{
    			iReqAttrFlag = 0;
    			}
		    }
		}
    else
	    {
		return CSvgAnimationBase::SetAttributeL( aName, aValue );
		}

	return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
TBool CSvgAnimateMotionElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    return CSvgAnimationBase::ReceiveEventProcL( aEvent, this );
    }

// ---------------------------------------------------------------------------
// From CSvgAnimationBase
// ---------------------------------------------------------------------------
//
TBool CSvgAnimateMotionElementImpl::AnimProcL( MSvgTimerEvent* aEvent )

    {


  if((iAnimStatus != KAnimActive) || iDoFreeze)
	   {
	   if(iFill== KAnimFillFreeze && !iDoFreeze && iAnimStatus == KAnimFinished)
	   		{
	   		// calculate the freeze value.
	   		}
	   else
		   {
		   CSvgAnimationBase::CheckForEndTimesAndFreezeL(this);
		   return EFalse;
		   }
	   }
    if ( ( iPathIterator == NULL ) || ( iPathIterator->IsDone() ) )
		{
        return EFalse;  // Already end of path. Nothing to do any more.
		}

    TFloatFixPt  nextPos, diff;
    TFloatFixPt  zero    ( 0 );

    // Get next position in [0..255] range
    TInt32  animatetime; // Not used...
    TInt32  valix, subanimtime;
    iAnimTime->GetAnimTime( (aEvent->Time()-iNegativeBeginTime), animatetime, valix, subanimtime );

    // Get next position in actual length
    TFloatFixPt  v2;
    if ( valix >= iValuesFloat->Count() - 1 )
		{
		valix= iValuesFloat->Count() - 1;
        v2 = ( *iValuesFloat )[valix];
		}
    else
		{
        v2 = ( *iValuesFloat )[valix + 1];
		}
    nextPos = CSvgAnimationBase::BlendFloat( subanimtime,
                                             ( *iValuesFloat )[valix],
                                             v2 );

    diff = nextPos - iCurrentPos;
    TGfxSegType SegType;

    while ( iSubPos + diff > iSubLength && !iPathIterator->IsDone() )
        {
        if ( iSubLength < zero )
			{
            iSubLength = zero;
			}
        TFloatFixPt  tmpcoords[6];
        diff -= iSubLength - iSubPos;
        iPathIterator->NextL();
        SegType = iPathIterator->CurrentSegment( tmpcoords );
        iCurrentSeg.iX1 = iCurrentSeg.iX2;
        iCurrentSeg.iY1 = iCurrentSeg.iY2;
        iCurrentSeg.iX2 = tmpcoords[0];
        iCurrentSeg.iY2 = tmpcoords[1];

        if ( SegType == EGfxSegMoveTo )
			{
            iSubLength = zero;
			}
        else
			{
            iSubLength = iCurrentSeg.Length();
			}

        iSubPos = zero;
        }
    iSubPos += diff;

    // Calcualate or set rotation
    TReal32 rot = 0.0f;
    if ( iAnimMotionRotate < 0 )
        {
        TFloatFixPt  dx  = iCurrentSeg.iX1 - iCurrentSeg.iX2;
        if ( dx == zero )
        {

			if( iAnimMotionRotate == KAnimMotionRotateAuto )
			{
				if(iPrevRotate == 0 || iPrevRotate >= 3.1415926f)
					rot = 3.1415926f + 1.5707963f;
				else
					rot = 1.5707963f; // pi/2
			}
			else // AutoReverse
			{
				if(iPrevRotate >= 0 )
					rot = 1.5707963f;
				else
					rot = -1.5707963f;

            }
		}
        else
            {
            TReal   atan;
            Math::ATan( atan, ( TReal32 )( iCurrentSeg.iY1 - iCurrentSeg.iY2 ), ( TReal32 )dx );
            rot = ( TReal32 ) atan;
			if ( iAnimMotionRotate == KAnimMotionRotateAuto )
				{
				rot += 3.1415926f;
				}

			}
		iPrevRotate = rot;
        }
    else
        {
        rot = iAnimMotionRotate * 3.1415926f / 180.0f;
        }

    // Create transform
    MSvgTransformList*  trList;
    TInt32              matrixIndex;
    iTargetElement->GetTransform( trList );
    iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex );
    // new position
    TInt32  subalpha;
    if ( iSubLength == zero )
		{
        subalpha = 0;
		}
    else
		{
        subalpha = ( TInt32 ) ( iSubPos / iSubLength * TFloatFixPt( 255 ) );  // need to make sure value does not exceed 0x7fff
		}
    if ( subalpha > 0xff )
		{
        subalpha = 0xff;
		}
    if ( subalpha < 0 )
		{
        subalpha = 0;
		}

    TFloatFixPt  trnsx, trnsy;
    trnsx = CSvgAnimationBase::BlendFloat( subalpha,
                                           iCurrentSeg.iX1,
                                           iCurrentSeg.iX2 );
    trnsy = CSvgAnimationBase::BlendFloat( subalpha,
                                           iCurrentSeg.iY1,
                                           iCurrentSeg.iY2 );

    TGfxAffineTransform deltaTr = TGfxAffineTransform::GetTranslateInstance( trnsx,
                                                                             trnsy );
    if ( rot != 0.0f )
        {
        deltaTr.Rotate( rot );
        }

    // Set transform

    if ( iAccumulate == KAccumSum ) //.
        {
        TGfxAffineTransform accumulateDeltaTr = TGfxAffineTransform::GetTranslateInstance( iFromFloatX - iOrgFromFloatX, iFromFloatY - iOrgFromFloatY);
        deltaTr.Concatenate( accumulateDeltaTr );
        }

    if ( iAdditive == KAdditiveSum ) //.
        {
        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate( deltaTr );
        trList->ReplaceItem( curMatrix, matrixIndex );
        }
    else
        {
        trList->ReplaceItem( deltaTr, matrixIndex );

        }
	if(iFill== KAnimFillFreeze)
		{
	 	iEndMatrix = deltaTr;
		}
    // update current position
    iCurrentPos = nextPos;

	 	CSvgAnimationBase::CheckForEndTimesAndFreezeL(this);
    return ETrue;
    }


//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::ResetAnimationL()
    {
    TFloatFixPt  tmpcoords[6];

    // Path iterator
    if ( iPathIterator )
        {
        delete iPathIterator;
        iPathIterator = NULL;
        }

    iPathIterator = CGfxFlatteningPathIterator::NewL( iMotionPath,
                                                      &iIdentTransform,
                                                      3 );


    // path length
    iPathIterator->CurrentSegment( tmpcoords );
    iCurrentSeg.iX1 = tmpcoords[0];
    iCurrentSeg.iY1 = tmpcoords[1];
    iCurrentSeg.iX2 = tmpcoords[0];
    iCurrentSeg.iY2 = tmpcoords[1];

    iSubLength = -1;
    iCurrentPos = 0;
    iSubPos = 0;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::InitAnimationL()
    {
	if(!iTargetElement)
		{
		// if target element is not found then remove it.
		((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(this);
		((CSvgDocumentImpl*)iOwnerDocument)->Engine()->UpdatePresentation(1);
		return;
		}

    TFloatFixPt len;
    TFloatFixPt lx, ly;
    TFloatFixPt dx, dy, int0x7f( KMAXFLOATFIX );
    // Create values of distance from start point on motion path
    TFloatFixPt  tmpcoords[6];
        {
        CGfxGeneralPath*    path    = CGfxGeneralPath::NewLC();
        CGfxPathIterator*   itr;
        iMotionPath->GetPathIteratorL( &iIdentTransform, itr );

        CleanupStack::PushL( itr );
        TFloatFixPt  length;
		iValuesFloat->Reset();
        while ( !itr->IsDone() )
            {
            switch ( itr->CurrentSegment( tmpcoords ) )
                {
                case EGfxSegMoveTo:
                path->MoveToL( tmpcoords[0], tmpcoords[1], ETrue );
                lx = tmpcoords[0];
                ly = tmpcoords[1];
                len = TSvgPathDataParser::PathLengthL( path );
                break;
                case EGfxSegLineTo:
                path->LineToL( tmpcoords[0], tmpcoords[1], ETrue );
                dx = lx - tmpcoords[0];
                dy = ly - tmpcoords[1];
                if ( dx > int0x7f || dy > int0x7f )
                    {
                    // calculate as integer
                    TInt32 tmplen, tx, ty;
                    tx = ( TInt32 ) dx;
                    ty = ( TInt32 ) dy;
	#ifdef SVG_FLOAT_BUILD
                    tmplen = TFloatFixPt::Sqrt(tx * tx + ty * ty);
	#else
                    tmplen = TFloatFixPt::FixedSqrtGeneral( tx * tx + ty * ty, 0 );
	#endif
                    len += TFloatFixPt( tmplen );
                    }
                else
                    {
                    // calculate as TFixPt
	
                    len += TFloatFixPt::Sqrt( dx * dx + dy * dy );
                    }
                lx = tmpcoords[0];
                ly = tmpcoords[1];

                break;
                case EGfxSegQuadTo:
                path->QuadToL( tmpcoords[0],
                               tmpcoords[1],
                               tmpcoords[2],
                               tmpcoords[3],
                               ETrue );
                len = TSvgPathDataParser::PathLengthL( path );
                break;
                case EGfxSegCubicTo:
                path->CubicToL( tmpcoords[0],
                                tmpcoords[1],
                                tmpcoords[2],
                                tmpcoords[3],
                                tmpcoords[4],
                                tmpcoords[5],
                                ETrue );
                len = TSvgPathDataParser::PathLengthL( path );
                break;
                case EGfxSegClose:
                path->LineToL( tmpcoords[0], tmpcoords[1], ETrue );
                len = TSvgPathDataParser::PathLengthL( path );
				break;
                }

            length = len;
            //length = TSvgPathDataParser::PathLengthL( path );
            iValuesFloat->AppendL( length );
            itr->NextL();
            }
        CleanupStack::PopAndDestroy( 2 ); // path, itr
        }

    CSvgAnimationBase::InitAnimationL();
    if(!iKeyTimesPresent)
    iAnimTime->CreateKeyTime( iMotionPath->PointTypeArray()->Count() );
    iAnimTime->PrepareTimeL( iValuesFloat );
	SetFillValueL();
    this->ResetAnimationL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetFillValueL()
    {
    iPrevRotate = 0;
    MSvgTransformList* trList = NULL;
    if ( iTargetElement )
        {
        ((CSvgElementImpl*)iTargetElement)->GetTransform( trList );
        TInt32 matrixIndex = -1;
        iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex );
        ((CSvgElementImpl*)iTargetElement)->GetTransform( trList );
        if ( trList && matrixIndex != -1 )
            {
            iFillMatrix = trList->GetItem(matrixIndex);
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetToOriginalL()
    {
    MSvgTransformList*  trList;
    TInt32              matrixIndex;
	if (iTargetElement == NULL)
		{
		return;
		}
    iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex );
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );


        trList->ReplaceItem( iFillMatrix, matrixIndex );


    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetToInitialValueL()
    {
    MSvgTransformList*  trList;
    TInt32              matrixIndex;
    iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex );
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );

    if( iAdditive == KAdditiveSum)
    	{
        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate(iFillMatrix);
       	trList->ReplaceItem( curMatrix, matrixIndex );
    	}
    else
    	{
         trList->ReplaceItem( iFillMatrix, matrixIndex );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetToEndValueL()
    {
    MSvgTransformList*  trList;
    TInt32              matrixIndex;
    iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex );
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );

	 if( iAdditive == KAdditiveSum)
    	{
        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate(iEndMatrix);
       	trList->ReplaceItem( curMatrix, matrixIndex );
    	}
    else
    	{
         trList->ReplaceItem( iEndMatrix, matrixIndex );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetAccumulateValues()
    {
    TFloatFixPt              lDelta;
	if(iHaveBy)
		{

		lDelta = iToFloatX - iFromFloatX;
	    iToFloatX = iToFloatX + lDelta;
	    iFromFloatX = iFromFloatX + lDelta;

	    lDelta = iToFloatY - iFromFloatY;
	    iToFloatY = iToFloatY + lDelta;
	    iFromFloatY = iFromFloatY + lDelta;

		}
	else
		{
	    lDelta = iToFloatX - iFromFloatX;
	    iFromFloatX = iToFloatX + iOrgFromFloatX;
	    iToFloatX = iFromFloatX + lDelta;

	    lDelta = iToFloatY - iFromFloatY;
	    iFromFloatY = iToFloatY + iOrgFromFloatY;
	    iToFloatY = iFromFloatY + lDelta;
	    }
    }

// ---------------------------------------------------------------------------
// From MXmlElementOpt
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::GetAttributePath( const TInt /* aNameId */,
                                                     CGfxGeneralPath*& /* aValue */ )
    {
    return KErrNoAttribute;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::SetAttributePathL( const TInt aNameId,
                                                      CGfxGeneralPath* aValue )
    {
    if ( aNameId == KAtrData )
        {
        iMotionPath->Reset();
        CGfxPathIterator*   lIter   = NULL;
        aValue->GetPathIteratorL( &iIdentTransform, lIter );
        CleanupStack::PushL( lIter );
        iMotionPath->AppendL( lIter );
        CleanupStack::PopAndDestroy( 1 ); //lIter
        }

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::SetAttributePathRef( const TInt aNameId,
                                              CGfxGeneralPath*&  aValue  )
    {
	iIsMotionPath= EFalse;
    if ( aNameId == KAtrAnimateMotion )
        {
		if(iMotionPath)
			{
			delete iMotionPath;
			iMotionPath= NULL;
			}
		iMotionPath= aValue;

		if((iMotionPath->PointTypeArray())->Count() != 0 )
			{
			CGfxPathIterator*   itr = NULL;
			TFloatFixPt              tmpcoords[6];
			TRAPD( error,iMotionPath->GetPathIteratorL( &iIdentTransform, itr ) );
			if (error!= KErrNone)
				{
				return error;
				}



			while ( !itr->IsDone() )
				{
				switch ( itr->CurrentSegment( tmpcoords ) )
					{
					case EGfxSegMoveTo:
					iToFloatX = tmpcoords[0];
					iToFloatY = tmpcoords[1];
					break;
					case EGfxSegLineTo:
					iToFloatX = tmpcoords[0];
					iToFloatY = tmpcoords[1];
					break;
					case EGfxSegQuadTo:
					iToFloatX = tmpcoords[2];
					iToFloatY = tmpcoords[3];
					break;
					case EGfxSegCubicTo:
					iToFloatX = tmpcoords[4];
					iToFloatY = tmpcoords[5];
					break;
					case EGfxSegClose:
					iToFloatX = tmpcoords[0];
					iToFloatY = tmpcoords[1];
                    }

                TRAPD( error, itr->NextL() );
                if ( error != KErrNone )
                    {
                    delete itr;
                    return KErrNone;
                    }
                }

            delete itr;
            itr= NULL;


     		RArray<TFloatFixPt>* lArrayFix;
			lArrayFix= iMotionPath->PointCoordsArrayAll();
			if (lArrayFix)
				{
				iOrgFromFloatX = iFromFloatX = lArrayFix->operator[](0);
				iOrgFromFloatY = iFromFloatY = lArrayFix->operator[](1);
				}

			}
		}

    else
        {
        return KErrNoAttribute;
        }

    return KErrNone;
    }
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetAnimRotate(TInt16 aRotate)
	{
	iAnimMotionRotate= aRotate;
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::SetAttributeIntL( const TInt aNameId, const TInt32 aValue)
{
	if(aNameId == KAtrRotate)
		{
		iAnimMotionRotate = (TInt16)aValue;
		return KErrNone;
		}
	return CSvgAnimationBase::SetAttributeIntL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	if(aNameId == KAtrRotate)
		{
		aValue = (TInt)(iAnimMotionRotate);
		return KErrNone;
		}
	return CSvgAnimationBase::GetAttributeIntL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::SetAttributeFloatL(const TInt aNameId, TFloatFixPt aValue )
{
	return CSvgAnimationBase::SetAttributeFloatL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateMotionElementImpl::GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue )
{
	return CSvgAnimationBase::GetAttributeFloat(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::ReInitializeAnimation()
    {

	if ( iAttrId == 0xffff )
        return;

	if( iCurrentRepeatCount > 0 && iAccumulate == KAccumSum )
		{
		iFromFloatX= iOrgFromFloatX;
		iFromFloatY= iOrgFromFloatY;
		iToFloatX= iOrgToFloatX;
		iToFloatY= iOrgToFloatY;
		}

    CSvgAnimationBase::ReInitializeAnimation();

	iCurrentSeg.iX1 = (TFloatFixPt) 0;
	iCurrentSeg.iX2 = (TFloatFixPt) 0;
	iCurrentSeg.iY1 = (TFloatFixPt) 0;
	iCurrentSeg.iY2 = (TFloatFixPt) 0;
	MSvgTransformList*  trList;
	TInt32              matrixIndex = 0;

	if (iTargetElement)
		{
		TRAPD(error ,iTargetElement->GetAttributeIntL( KAtrAnimMotionMatrixIndex, matrixIndex ));
		if ( error != KErrNone )
		   {
		   // ignore trap error
	      }
		( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
		if( trList && (trList->NumberOfItems() >  matrixIndex) )
			{
			trList->ReplaceItem( TGfxAffineTransform(), matrixIndex );

			trList->SetAdditive( iAdditive, matrixIndex );
			}
		}
	iFillMatrix= TGfxAffineTransform();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void  CSvgAnimateMotionElementImpl::SetOriginalValues_DOMReuse()
	{
	iOrgToFloatX= iToFloatX;
	iOrgToFloatY= iToFloatY;
    // calling base class function.
	CSvgAnimationBase::SetOriginalValues_DOMReuse();
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
CGfxGeneralPath* CSvgAnimateMotionElementImpl::GetPathAttribute(TInt aAttributeId)
	{
	if(aAttributeId == KAtrPath)
		{
		return iMotionPath;
		}
	else
		{
		return CSvgAnimationBase::GetPathAttribute(aAttributeId);
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{

	if(aAttributeId == KAtrPath)
		{
		if(iMotionPath == aPathHandle)
			{
			return;
			}
		delete iMotionPath;
		iMotionPath = aPathHandle;

		CGfxPathIterator*   itr = NULL;
        TFloatFixPt              tmpcoords[6];
        TRAPD(error ,iMotionPath->GetPathIteratorL( &iIdentTransform, itr ));
        if ( error != KErrNone )
		   {
		   // ignore trap error
	       }

		if(itr)
			{
        while ( !itr->IsDone() )
            {
            switch ( itr->CurrentSegment( tmpcoords ) )
                {
                case EGfxSegMoveTo:
                iToFloatX = tmpcoords[0];
                iToFloatY = tmpcoords[1];
                break;
                case EGfxSegLineTo:
                iToFloatX = tmpcoords[0];
                iToFloatY = tmpcoords[1];
                break;
                case EGfxSegQuadTo:
                iToFloatX = tmpcoords[2];
                iToFloatY = tmpcoords[3];
                break;
                case EGfxSegCubicTo:
                iToFloatX = tmpcoords[4];
                iToFloatY = tmpcoords[5];
                break;
                case EGfxSegClose:
                iToFloatX = tmpcoords[0]; //.
                iToFloatY = tmpcoords[1];
                    }
                TRAPD( error, itr->NextL() );
                if ( error != KErrNone )
                    {
                    delete itr;
                    return;
                    }
                }

			delete itr;
			}

		RArray<TFloatFixPt>* lArrayFix;
		lArrayFix= iMotionPath->PointCoordsArrayAll();
		if (lArrayFix)
			{
			iOrgFromFloatX = iFromFloatX = lArrayFix->operator[](0);
			iOrgFromFloatY = iFromFloatY = lArrayFix->operator[](1);
			}

		}
	else
		{
		CSvgAnimationBase::SetPathAttribute(aAttributeId, aPathHandle);
		}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::Reset(MSvgEvent* aEvent)
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


/////////////////////////////////////////////
MXmlElement* CSvgAnimateMotionElementImpl::CloneL(MXmlElement*
aParentElement)
	{
	CSvgAnimateMotionElementImpl* newElement = CSvgAnimateMotionElementImpl::NewL(  this->ElemID(),
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
void CSvgAnimateMotionElementImpl::CopyL(CSvgAnimateMotionElementImpl* aDestElement )
    {
	if(aDestElement)
	{


    // copy stuff from superclass
    this->CSvgAnimationBase::CopyL(aDestElement);

    // copy the reference to idoc (CSvgDocumentImpl)
    aDestElement->iOwnerDocument = this->iOwnerDocument;

    /*Need to check Test whether Copying is proper*/

	aDestElement->iFillMatrix = this->iFillMatrix;
	if(this->iMotionPath)
		{
		/*Motion Path and iPathIterator assignment needs to be checked.*/
        aDestElement->iMotionPath->Reset();
        aDestElement->iMotionPath->SetPointCoordsArrayL(this->iMotionPath->PointCoordsArrayAll());
        aDestElement->iMotionPath->SetPointTypeArrayL(this->iMotionPath->PointTypeArray());
        }
	aDestElement->iIdentTransform = this->iIdentTransform;
    aDestElement->iSubLength = this->iSubLength;
    aDestElement->iCurrentPos = this->iCurrentPos;
    aDestElement->iSubPos = this->iSubPos;
    aDestElement->iCurrentSeg = this->iCurrentSeg;
    aDestElement->iAnimMotionRotate = this->iAnimMotionRotate;
    aDestElement->iEndMatrix = this->iEndMatrix;
    //aDestElement->iIsValues = this->iIsValues;
    aDestElement->iToFloatX = this->iToFloatX;
    aDestElement->iToFloatY = this->iToFloatY;
    aDestElement->iFromFloatX = this->iFromFloatX;
    aDestElement->iFromFloatY = this->iFromFloatY;
    aDestElement->iOrgFromFloatX = this->iOrgFromFloatX;
    aDestElement->iOrgFromFloatY = this->iOrgFromFloatY;
		// for DOM Reuse
	aDestElement->iOrgToFloatX = this->iOrgToFloatX;
    aDestElement->iOrgToFloatY = this->iOrgToFloatY;
	aDestElement->iIsMotionPath = this->iIsMotionPath;

	if(this->iPathIterator)
		{
		aDestElement->ResetAnimationL();
		}
	}
   }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::DeactivateAnimation()
{
CSvgAnimationBase::DeactivateAnimation(this);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimateMotionElementImpl::DoAnimProcL(MSvgEvent* aEvent)
{
return this->AnimProcL((MSvgTimerEvent*)aEvent);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::SetAccumulateValuesForSetMediaTime()
{
if(iAccumulate == KAccumSum )
		{
		iFromFloatX= iOrgFromFloatX;
		iFromFloatY= iOrgFromFloatY;
		iToFloatX= iOrgToFloatX;
		iToFloatY= iOrgToFloatY;

		TInt i=0;
		while(i < iCurrentRepeatCount )
			{
			SetAccumulateValues();
			i++;
			}
		}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateMotionElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<animateMotion calcMode=\"hmm\" xlink:href=\"hmm\" path=\"hmm\" keyPoints=\"hmm\" rotate=\"%d\"/>",
		/*Href(),*/ (int)iAnimMotionRotate);
		#endif
	}
}


