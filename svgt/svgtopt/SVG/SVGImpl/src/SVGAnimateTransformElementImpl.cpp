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


#include "SVGAnimateTransformElementImpl.h"

#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "SVGPointLexer.h"
#include "SVGStringTokenizer.h"

#include "GfxAffineTransform.h"

_LIT( SEMICOLON, ";" );


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateTransformElementImpl* CSvgAnimateTransformElementImpl::NewL(  const TUint8 aElemID,
																		CSvgDocumentImpl* aDoc )
	{
	CSvgAnimateTransformElementImpl*self    = new ( ELeave )
											  CSvgAnimateTransformElementImpl( aDoc );

	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateTransformElementImpl* CSvgAnimateTransformElementImpl::NewLC(  const TUint8 aElemID,
																		 CSvgDocumentImpl* aDoc )
	{
	CSvgAnimateTransformElementImpl*self    = new ( ELeave )
											  CSvgAnimateTransformElementImpl( aDoc );

	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::ConstructL(  const TUint8 aElemID)
	{
	CSvgAnimationBase::ConstructL( aElemID );

	// Initial length of values is 2 (from and to)
	iTransformValues = new ( ELeave ) RArray<TMatrixData>( 2 );
    TMatrixData mtrx;
    iTransformValues->AppendL( mtrx );
    iTransformValues->AppendL( mtrx );

    iOrgTransformValues = new ( ELeave ) RArray<TMatrixData>( 2 );

    iOrgTransformValues->AppendL( mtrx );
    iOrgTransformValues->AppendL( mtrx );


	iAccumMatrixData.iData[0] = 0;
	iAccumMatrixData.iData[1] = 0;
	iAccumMatrixData.iData[2] = 0;

	iReqAttrFlag=KSVG_ANIMATETRANSFORM_ELEMFLAG;



	}




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateTransformElementImpl::~CSvgAnimateTransformElementImpl()
    {
    if ( iTransformValues )
		{
        iTransformValues->Close();
	    delete iTransformValues;
	    iTransformValues = NULL;
		}
	if ( iOrgTransformValues )
		{
        iOrgTransformValues->Close();
	    delete iOrgTransformValues;
	    iOrgTransformValues = NULL;

		}
    }

// ---------------------------------------------------------------------------
// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAnimateTransformElementImpl::CSvgAnimateTransformElementImpl( CSvgDocumentImpl* aDoc ) : CSvgAnimationBase( aDoc ),

                                                                                             iMatrixDataSize( 2 ),
																							 iMultipleRendering (EFalse)
    {
    iDataType = KSvgTypeTranslate;
    }

// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::ResetReferenceElementL()
    {
    if(iTargetElement != NULL )
    	{
    	if( ((iTargetElement->iAnimateAttrSet == NULL) || (iTargetElement->iAnimateAttrSet->Find(iAttrId) == KErrNotFound) ) )
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

// ---------------------------------------------------------------------------
// From MXmlElement
// ---------------------------------------------------------------------------
TInt CSvgAnimateTransformElementImpl::SetAttributeL( const TDesC& aName,
                                                     const TDesC& aValue )
    {
    _LIT( KTmpAttrName, "attributeName" );
    _LIT( KTmpTransform, "transform" );
    _LIT( KTmpFrom, "from" );
    _LIT( KTmpTo, "to" );
    _LIT( KTmpBy, "by" );
    _LIT( KTmpType, "type" );
    _LIT( KTmpScale, "scale" );
    _LIT( KTmpRotate, "rotate" );
    _LIT( KTmpSkewX, "skewX" );
    _LIT( KTmpSkewY, "skewY" );
    _LIT( KTmpValues, "values" );
    _LIT( KTmpGradientTransform, "gradientTransform");

    	if ( !iTargetSet )
			{
			iTargetElement = ( CSvgElementImpl * ) ParentNode();// default is parent element
			iTargetSet = ETrue;
			}
		   	TUint16 lElementId = 0;
		if (iTargetElement)
		    lElementId = iTargetElement->ElemID();
       if ( aName == KTmpAttrName )
	        {

			if(iReqAttrFlag == KAtrSVGTrf)
				{
				iReqAttrFlag = KAtrType;
				}
			else
				{
				iReqAttrFlag = 0;
				}
	    	if( aValue == KTmpTransform &&
				!(lElementId == KSvgRadialGradientElement || lElementId
				 == KSvgLinearGradientElement)
				 )
				{
				iAttrId = KAtrAnimateTransformAttrId;
	       		return KErrNone;
				}
			else if (aValue == KTmpGradientTransform &&
				(lElementId == KSvgRadialGradientElement || lElementId
				 == KSvgLinearGradientElement))
				{
				 iAttrId = KAtrAnimateTransformAttrId;
	       		 return KErrNone;
				}
			else if(aValue == KTmpGradientTransform) // Specifically for Xlink:href case
				{
				 iAttrId = KAtrGradientTransform;
				 return KErrNone;
				}
			else
	            {
	            // value must be not be 'transform'
	            iAttrId = 0xffff;
	            return KErrNone;
	            }
	        }
    else if ( aName == KTmpType )
        {

		if(iReqAttrFlag == KAtrSVGTrf)
			{
			iReqAttrFlag = KAtrAttributeName;
			}
		else
			{
			iReqAttrFlag = 0;
			}

        // Use iDataType to keep transformation type
        if ( aValue == KTmpScale )
            {
            iDataType = KSvgTypeScale;
            iMatrixDataSize = 2;
            }
        else if ( aValue == KTmpRotate )
            {
            iDataType = KSvgTypeRotate;
            iMatrixDataSize = 3;
            }
        else if ( aValue == KTmpSkewX )
            {
            iDataType = KSvgTypeSkewX;
            iMatrixDataSize = 1;
            }
        else if ( aValue == KTmpSkewY )
            {
            iDataType = KSvgTypeSkewY;
            iMatrixDataSize = 1;
            }
        else
            {
            iDataType = KSvgTypeTranslate;  // default transformation
            iMatrixDataSize = 2;
            }
        return KErrNone;
        }
    else if ( aName == KTmpFrom )
        {
        iNoFrom = EFalse;
        // Processing for semi-colon separated data is needed
        DesToMatrixData( aValue, ( *iTransformValues )[0] );
        return KErrNone;
        }
    else if ( aName == KTmpTo )
        {
        iHaveTo = ETrue;

        if ( iNoFrom )
            {
            iAdditive = KAdditiveSum;
            iAdditiveOrg = KAdditiveSum;
            }

        // Processing for semi-colon separated data is needed
        DesToMatrixData( aValue, ( *iTransformValues )[1] );
		iAccumMatrixData.iData[0] = (*iTransformValues)[1].iData[0];
		iAccumMatrixData.iData[1] = (*iTransformValues)[1].iData[1];
		iAccumMatrixData.iData[2] = (*iTransformValues)[1].iData[2];
        return KErrNone;
        }
    else if ( aName == KTmpBy )
        {
        if ( !iHaveTo )
            {
            TInt32  i;
            DesToMatrixData( aValue, ( *iTransformValues )[1] );
            iHaveBy = ETrue;

            if(iNoFrom)
                {
                iAdditive = KAdditiveSum;
                iAdditiveOrg = KAdditiveSum;
                }
            for ( i = 0; i < 3; i++ )
                {
                ( *iTransformValues )[1].iData[i] = ( *iTransformValues )[1].iData[i] +
                                                    ( *iTransformValues )[0].iData[i];
                }

            }
        return KErrNone;
        }
    else if ( aName == KTmpValues )
        {
        TStringTokenizer    tkn ( aValue, SEMICOLON );
        iTransformValues->Reset();

   		iHaveValues = ETrue;

        iNoFrom = EFalse;

        while ( tkn.HasMoreTokens() )
            {
            TMatrixData mtrx;
            DesToMatrixData( tkn.NextToken(), mtrx );
            iTransformValues->AppendL( mtrx );
            }
        TInt lTransformValuesCount = iTransformValues->Count();
		if(lTransformValuesCount > 0)
			{
			iAccumMatrixData.iData[0] = (*iTransformValues)[lTransformValuesCount - 1].iData[0];
			iAccumMatrixData.iData[1] = (*iTransformValues)[lTransformValuesCount - 1].iData[1];
			iAccumMatrixData.iData[2] = (*iTransformValues)[lTransformValuesCount - 1].iData[2];
			}

        return KErrNone;
        }

    return CSvgAnimationBase::SetAttributeL( aName, aValue );
    }

// ---------------------------------------------------------------------------
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
TBool CSvgAnimateTransformElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    return CSvgAnimationBase::ReceiveEventProcL( aEvent, this );
    }

// ---------------------------------------------------------------------------
// From CSvgAnimationBase
// ---------------------------------------------------------------------------

TBool CSvgAnimateTransformElementImpl::AnimProcL( MSvgTimerEvent* aEvent )

    {


  if((iAnimStatus != KAnimActive) || iDoFreeze)
	   {
	   if(iFill== KAnimFillFreeze && !iDoFreeze && iAnimStatus == KAnimFinished)
	   		{

	   		}
	   else
		   {
		   CSvgAnimationBase::CheckForEndTimesAndFreezeL(this );
		   return EFalse;
		   }
	   }
    // No animation if 'from' or 'to' is not available
    if ( iTransformValues->Count() < 2 )
		{
        return EFalse;
		}

    // Calc alpha from time [0-255]
    TInt32  alpha;
    TInt32  valix1, valix2, subanimtime;
    TMatrixData mdata;

    iAnimTime->GetAnimTime((aEvent->Time()-iNegativeBeginTime), alpha, valix1, subanimtime );

 // check for the range of valix
	if ( valix1 >= iTransformValues->Count() )
		{
        valix1 = iTransformValues->Count() - 1;
		}


    if ( iNoFrom && !iHaveBy ) //. This is to emulate Adobe.
        {
        if ( alpha > ( KTimeMax >> 1 ) )
            {
            subanimtime = KTimeMax;
            }
        else
            {
            subanimtime = 0;
            return ETrue;
            }
        }

    if ( valix1 >= iTransformValues->Count() - 1 )
		{
        valix2 = valix1;
		}

    else
		{
        valix2 = valix1 + 1;
		}

	mdata.iData[0] = BlendFloat( subanimtime,
							   ( *iTransformValues )[valix1].iData[0],
							   ( *iTransformValues )[valix2].iData[0] );
	mdata.iData[1] = BlendFloat( subanimtime,
							   ( *iTransformValues )[valix1].iData[1],
							   ( *iTransformValues )[valix2].iData[1] );
	mdata.iData[2] = BlendFloat( subanimtime,
							   ( *iTransformValues )[valix1].iData[2],
							   ( *iTransformValues )[valix2].iData[2] );
    // Additive
    /*
     * !!!! Additive="replace" is not supported
     * !!!! Additional implementation needed in MSvgTransformList
     */

    // Set value
    TGfxAffineTransform deltaTr;
    switch ( iDataType )
        {
        case KSvgTypeTranslate:
        deltaTr = TGfxAffineTransform::GetTranslateInstance( mdata.iData[0],
                                                             mdata.iData[1] );
        break;
        case KSvgTypeScale:
        deltaTr = TGfxAffineTransform::GetScaleInstance( mdata.iData[0],
                                                         mdata.iData[1] );
        break;
        case KSvgTypeRotate:
        deltaTr = TGfxAffineTransform::GetRotateInstance( ( TReal32 )
                                                          mdata.iData[0] * 3.1415926f /
                                                          180.0f,
                                                          mdata.iData[1],
                                                          mdata.iData[2] );
        break;
        case KSvgTypeSkewX:
        deltaTr = TGfxAffineTransform::GetShearInstance( ( TReal32 )
                                                         mdata.iData[0]* 3.1415926f /
                                                         180.0f,
                                                         0.0f );
        break;
        case KSvgTypeSkewY:
        deltaTr = TGfxAffineTransform::GetShearInstance( 0.0f,
                                                         ( TReal32 )
                                                         mdata.iData[0]* 3.1415926f /
                                                         180.0f );
        break;
        }
    MSvgTransformList*  trList;
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
    TInt32 matrixIndex;
    ( ( CSvgElementImpl * ) iTargetElement )->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex );
   if ( iAdditive == KAdditiveSum ) //.
        {
        if(iNoFrom && subanimtime == KTimeMax && !iHaveBy)
	        {
	        iTargetElement->SetOverwriteTransforms( ETrue );
	        }
		/*else
	        {
	        //if additive = sum and there is a from or a by then dont overwrite the transform
	        iTargetElement->iOverwriteTransforms = EFalse;
	        }*/
	    if(iHaveTo && iNoFrom)
	    {
	    	iTargetElement->SetOverwriteTransforms(ETrue);
	    	trList->ReplaceItem(deltaTr, matrixIndex);
	    }
	    else 
	    {
        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate( deltaTr );
        trList->ReplaceItem( curMatrix, matrixIndex );
	    }

        }
    else
        {
        iTargetElement->SetOverwriteTransforms( ETrue );
        trList->ReplaceItem( deltaTr, matrixIndex );

        }
    if(iFill == KAnimFillFreeze)
	    {
	    iEndMatrix = deltaTr;
	    }
	CSvgAnimationBase::CheckForEndTimesAndFreezeL(this );
    return ETrue;
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetFillValueL()
    {
    MSvgTransformList*  trList;
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
     TInt32              matrixIndex;
     iTargetElement->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex );
    iFillMatrix = trList->GetItem(matrixIndex);
    }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetToOriginalL()
    {
    MSvgTransformList*  trList;
	TInt32 matrixIndex;
	iTargetElement->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex );
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );

    trList->ReplaceItem( iFillMatrix /*TGfxAffineTransform()*/,
                             matrixIndex );
    iTargetElement->SetOverwriteTransforms( EFalse );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetToInitialValueL()
    {
    MSvgTransformList*  trList;
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
    TInt32 matrixIndex;
    iTargetElement->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex );

    if( iAdditive == KAdditiveSum)
    	{

        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate(iFillMatrix);
       	trList->ReplaceItem( curMatrix, matrixIndex );
    	}
    else
    	{
    //	iTargetElement->iOverwriteTransforms = ETrue;
         trList->ReplaceItem( iFillMatrix, matrixIndex );
    	}


    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetToEndValueL()
    {
    MSvgTransformList*  trList;
    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
    TInt32 matrixIndex;
 	iTargetElement->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex );

	 if( iAdditive == KAdditiveSum)
    	{
		if(iNoFrom &&  !iHaveBy)
	        {
	        iTargetElement->SetOverwriteTransforms( ETrue );
	        }
	        if(iHaveTo && iNoFrom)
	        {
	        	iTargetElement->SetOverwriteTransforms(ETrue);
	        	trList->ReplaceItem(iEndMatrix, matrixIndex);
	        }
            else 
            {
        TGfxAffineTransform curMatrix = trList->GetItem( matrixIndex );
        curMatrix.Concatenate(iEndMatrix);
       	trList->ReplaceItem( curMatrix, matrixIndex );
            }
    	}
    else
    	{
    	iTargetElement->SetOverwriteTransforms( ETrue );
        trList->ReplaceItem( iEndMatrix, matrixIndex );
    	}

  }

//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::InitAnimationL()
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


    CSvgAnimationBase::InitAnimationL();

	if(iMultipleRendering)
		{
		return;
		}

    // Set keytime
	iValuesFloat->Reset();
    TInt count = iTransformValues->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iValuesFloat->AppendL( TFloatFixPt( i ) ); // create dummy scalar array
        }
    if(!iKeyTimesPresent)
    iAnimTime->CreateKeyTime( iValuesFloat->Count() );
    iAnimTime->PrepareTimeL( iValuesFloat );
	SetFillValueL();

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetAccumulateValues()
    {

	if ( !iNoFrom)
        {

        if (  iHaveBy )
	        {
	        TMatrixData delta;

			//0
	        delta.iData[0] = ( *iTransformValues )[1].iData[0] -
	                             ( *iTransformValues )[0].iData[0];

	        ( *iTransformValues )[0].iData[0] = ( *iTransformValues )[1].iData[0];

	        ( *iTransformValues )[1].iData[0] += delta.iData[0];

			//1
	        delta.iData[1] = ( *iTransformValues )[1].iData[1] -
	                             ( *iTransformValues )[0].iData[1];

	        ( *iTransformValues )[0].iData[1] = ( *iTransformValues )[1].iData[1];

	        ( *iTransformValues )[1].iData[1] += delta.iData[1];

			//2
	        delta.iData[2] = ( *iTransformValues )[1].iData[2] -
	                             ( *iTransformValues )[0].iData[2];

	        ( *iTransformValues )[0].iData[2] = ( *iTransformValues )[1].iData[2];

	        ( *iTransformValues )[1].iData[2] += delta.iData[2];

	        }
	        else
		        {
				TInt lTransformValuesCount = (*iTransformValues).Count();
				for(int i=0;i<lTransformValuesCount;i++)
					{
					(*iTransformValues)[i].iData[0] += iAccumMatrixData.iData[0];
					(*iTransformValues)[i].iData[1] += iAccumMatrixData.iData[1];
					(*iTransformValues)[i].iData[2] += iAccumMatrixData.iData[2];
					}
				}
        }
	else
		{
		if (  iHaveBy )
	        {
	        TMatrixData delta;

			//0
	        delta.iData[0] = ( *iTransformValues )[1].iData[0] -
	                             ( *iTransformValues )[0].iData[0];

	        ( *iTransformValues )[0].iData[0] = ( *iTransformValues )[1].iData[0];

	        ( *iTransformValues )[1].iData[0] += delta.iData[0];

			//1
	        delta.iData[1] = ( *iTransformValues )[1].iData[1] -
	                             ( *iTransformValues )[0].iData[1];

	        ( *iTransformValues )[0].iData[1] = ( *iTransformValues )[1].iData[1];

	        ( *iTransformValues )[1].iData[1] += delta.iData[1];

			//2
	        delta.iData[2] = ( *iTransformValues )[1].iData[2] -
	                             ( *iTransformValues )[0].iData[2];

	        ( *iTransformValues )[0].iData[2] = ( *iTransformValues )[1].iData[2];

	        ( *iTransformValues )[1].iData[2] += delta.iData[2];

	        }

		}

    }


// *******************************************************
// AnimateTransformElementImpl specific methods
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::DesToMatrixData( const TDesC& aDes,
                                                       TMatrixData& aMatrixData )
    {
    TSvgPointLexer  svgplex ( aDes );
    TInt            i       = 0;

    while ( !svgplex.IsDone() && i < 3 )
        {
        TChar   com;
        TFloatFixPt  data;
        TInt32  rslt    = svgplex.GetNext( com, data );
        if ( rslt != KErrNone )
            break;
        aMatrixData.iData[i] = data;
        i++;
        }
    svgplex.Cleanup();

    // Support shortened data expression
    if ( i < iMatrixDataSize )
        {
        TFloatFixPt  zero    ( 0 );
        switch ( iDataType )
            {
            case KSvgTypeTranslate:
            aMatrixData.iData[1] = zero;
            break;
            case KSvgTypeScale:
            aMatrixData.iData[1] = aMatrixData.iData[0];
            break;
            case KSvgTypeRotate:

            if ( (i + 1) < iMatrixDataSize )
				{
                aMatrixData.iData[1] = zero;
				}

            aMatrixData.iData[2] = zero;
            break;
            }
        }
     if( i > iMatrixDataSize)
     	{
     	// reset back everything. this is invalid value.
     	TFloatFixPt  zero    ( 0 );
     	aMatrixData.iData[0] = zero;
     	aMatrixData.iData[1] = zero;
     	aMatrixData.iData[2] = zero;
     	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::ResetAnimationL()
    {
    }

// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------

void  CSvgAnimateTransformElementImpl::SetMatrixDataSize(TUint8 aValue)
	{
	iMatrixDataSize= aValue;
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TBool  CSvgAnimateTransformElementImpl::SetMatrixData( TInt aIndex, TMatrixData& aMatrix)
	{
	if ( iTransformValues && aIndex < iTransformValues->Count() )
		{
		for (TInt i=0; i<3; i++)
			{
			(iTransformValues->operator[](aIndex)).iData[i]= aMatrix.iData[i];
			}
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
void  CSvgAnimateTransformElementImpl::SetTransformValues(RArray<TMatrixData>*&	aValues)
	{
	if (iTransformValues)
		{
		iTransformValues->Close();
		delete iTransformValues;
		iTransformValues= NULL;
		}
	iTransformValues= aValues;
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateTransformElementImpl::SetAttributeIntL( const TInt aNameId,const TInt32 aValue )
	{
	if(aNameId == KAtrType)
		{
		if ( aValue == KSvgTypeScale )
            {
            iDataType = KSvgTypeScale;
            iMatrixDataSize = 2;
            }
        else if ( aValue == KSvgTypeRotate )
            {
            iDataType = KSvgTypeRotate;
            iMatrixDataSize = 3;
            }
        else if ( aValue == KSvgTypeSkewX )
            {
            iDataType = KSvgTypeSkewX;
            iMatrixDataSize = 1;
            }
        else if ( aValue == KSvgTypeSkewY )
            {
            iDataType = KSvgTypeSkewY;
            iMatrixDataSize = 1;
            }
        else
            {
            iDataType = KSvgTypeTranslate;  // default transformation
            iMatrixDataSize = 2;
            }
		return KErrNone;
		}
	return CSvgAnimationBase::SetAttributeIntL(aNameId,aValue);
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateTransformElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
	{
	if(aNameId == KAtrType)
		{
		aValue = iDataType;
		return KErrNone;
		}
	return CSvgAnimationBase::GetAttributeIntL(aNameId,aValue);
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateTransformElementImpl::GetAttributeFloat(const TInt aNameId,TFloatFixPt& aValue )
{
	return CSvgAnimationBase::GetAttributeFloat(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
TInt CSvgAnimateTransformElementImpl::SetAttributeFloatL(const TInt aNameId,TFloatFixPt aValue )
{
	return CSvgAnimationBase::SetAttributeFloatL(aNameId,aValue);
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
// this function is for binary discrepancies
void CSvgAnimateTransformElementImpl::SetAccumMatrix()
{
	if(iHaveTo)
		{
		iAccumMatrixData.iData[0] = (*iTransformValues)[1].iData[0];
		iAccumMatrixData.iData[1] = (*iTransformValues)[1].iData[1];
		iAccumMatrixData.iData[2] = (*iTransformValues)[1].iData[2];
		}
	else
		{
		if(!iNoFrom)
			{
			TInt lTransformValuesCount = iTransformValues->Count();
			if(lTransformValuesCount > 0)
				{
				iAccumMatrixData.iData[0] = (*iTransformValues)[lTransformValuesCount - 1].iData[0];
				iAccumMatrixData.iData[1] = (*iTransformValues)[lTransformValuesCount - 1].iData[1];
				iAccumMatrixData.iData[2] = (*iTransformValues)[lTransformValuesCount - 1].iData[2];
				}
			}
		}
}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::ReInitializeAnimation()
    {

	if ( iAttrId == 0xffff )
        return;

    // Do not set Multiple Rendering flag if animation is not
    // initialised. This allows the Key Times array in the anim
    // time controller to be set correctly so that the 
    // interpolation happens correctly.
    if ( iInitDone )
        {
	    iMultipleRendering=  ETrue;        
        }

	if( iCurrentRepeatCount > 0 && iAccumulate == KAccumSum )
		{
		SetTransValues_DOMReuse();
		}


	CSvgAnimationBase::ReInitializeAnimation();

    if(iTargetElement)
	{
		TInt32 matrixIndex = -1;
		MSvgTransformList*  trList;
		( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );

		TGfxAffineTransform affineTransform;
 		TRAPD(error,iTargetElement->GetAttributeIntL( KAtrAnimTransformMatrixIndex, matrixIndex ));
	    if(error == KErrNone)
		    {
		    ( ( CSvgElementImpl * ) iTargetElement )->GetTransform( trList );
			if( trList && (trList->NumberOfItems() >  matrixIndex) )
				{
				trList->ReplaceItem(affineTransform,matrixIndex);
				}
			}

		}

		iFillMatrix= TGfxAffineTransform();

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void  CSvgAnimateTransformElementImpl::SetTransValues_DOMReuse()
	{
	if(iOrgTransformValues && iTransformValues)
		{
        iTransformValues->Reset();
        
        TInt orgTransformValCnt = iOrgTransformValues->Count();
		for(TInt i=0; i<orgTransformValCnt; i++)
			{
			 TMatrixData lMatrix;
			lMatrix.iData[0] = (*iOrgTransformValues)[i].iData[0];
     		lMatrix.iData[1] = (*iOrgTransformValues)[i].iData[1];
			lMatrix.iData[2] = (*iOrgTransformValues)[i].iData[2];

			iTransformValues->Append((TMatrixData) lMatrix);
			}
		}
	}
// ---------------------------------------------------------------------------
// Functions used by Decoder
// ---------------------------------------------------------------------------
void  CSvgAnimateTransformElementImpl::SetOriginalValues_DOMReuse()
	{

	if(  iAccumulate == KAccumSum )
		{
		if(iTransformValues && iOrgTransformValues)
			{
			iOrgTransformValues->Reset();
			
			TInt transformValCnt = iTransformValues->Count();
			for(TInt i=0; i<transformValCnt; i++)
				{
				 TMatrixData lMatrix;
				lMatrix.iData[0] = (*iTransformValues)[i].iData[0];
     			lMatrix.iData[1] = (*iTransformValues)[i].iData[1];
				lMatrix.iData[2] = (*iTransformValues)[i].iData[2];

				iOrgTransformValues->Append((TMatrixData) lMatrix);
				}
			}
		}

    // calling base class function.
	CSvgAnimationBase::SetOriginalValues_DOMReuse();
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CGfxGeneralPath* CSvgAnimateTransformElementImpl::GetPathAttribute(TInt aAttributeId)
	{
	return CSvgAnimationBase::GetPathAttribute(aAttributeId);

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{

		CSvgAnimationBase::SetPathAttribute(aAttributeId, aPathHandle);

}
// ---------------------------------------------------------------------------
// Set the values in the dom .
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::Reset(MSvgEvent* aEvent)
{

		iIsUserSeek  = ETrue;
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
MXmlElement* CSvgAnimateTransformElementImpl::CloneL(MXmlElement*
aParentElement)
	{

		CSvgAnimateTransformElementImpl* newElement = CSvgAnimateTransformElementImpl::NewL(  this->ElemID(),((CSvgDocumentImpl*)iOwnerDocument) );
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
void CSvgAnimateTransformElementImpl::CopyL(CSvgAnimateTransformElementImpl* aDestElement )
    {
    // copy stuff from superclass
    if(aDestElement)
    {


    this->CSvgAnimationBase::CopyL(aDestElement);
    // copy the reference to idoc (CSvgDocumentImpl)
    aDestElement->iOwnerDocument = this->iOwnerDocument;

	aDestElement->iMultipleRendering = this->iMultipleRendering;
	aDestElement->iMatrixDataSize = this->iMatrixDataSize;
	aDestElement->iFillMatrix = this->iFillMatrix;
	aDestElement->iEndMatrix = this->iEndMatrix;
	TMatrixData lTemp;

	TInt count = this->iTransformValues->Count() ;
	aDestElement->iTransformValues->Reset();
	for(TInt i=0; i<count; i++)
		{
        aDestElement->iTransformValues->AppendL(lTemp);
		aDestElement->iTransformValues->operator[](i).iData[0] =
this->iTransformValues->operator[](i).iData[0];
		aDestElement->iTransformValues->operator[](i).iData[1] =
this->iTransformValues->operator[](i).iData[1];
		aDestElement->iTransformValues->operator[](i).iData[2] =
this->iTransformValues->operator[](i).iData[2];
		}
	TInt count2 = this->iOrgTransformValues->Count();
	aDestElement->iOrgTransformValues->Reset();
	for(TInt i=0; i<count2; i++)
        {
        aDestElement->iOrgTransformValues->AppendL(lTemp);
        aDestElement->iOrgTransformValues->operator[](i).iData[0] =
this->iOrgTransformValues->operator[](i).iData[0];
		aDestElement->iOrgTransformValues->operator[](i).iData[1] =
this->iOrgTransformValues->operator[](i).iData[1];
		aDestElement->iOrgTransformValues->operator[](i).iData[2] =
this->iOrgTransformValues->operator[](i).iData[2];
		}
	aDestElement->iAccumMatrixData.iData[0] = this->iAccumMatrixData.iData[0];
	aDestElement->iAccumMatrixData.iData[1] = this->iAccumMatrixData.iData[1];
	aDestElement->iAccumMatrixData.iData[2] = this->iAccumMatrixData.iData[2];
    }
   }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::DeactivateAnimation()
{
CSvgAnimationBase::DeactivateAnimation(this);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAnimateTransformElementImpl::DoAnimProcL(MSvgEvent* aEvent)
{
return this->AnimProcL((MSvgTimerEvent*)aEvent);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAnimateTransformElementImpl::SetAccumulateValuesForSetMediaTime()
{
if(iAccumulate == KAccumSum )
		{

		SetTransValues_DOMReuse();

		SetAccumMatrix();
		TInt i=0;
		while(i < iCurrentRepeatCount )
			{
			// take care of the accum = sum.
			SetAccumulateValues();
			i++;
			}
		}
}

void CSvgAnimateTransformElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<animateTransform attributeName=\"hmm\" attributeType=\"hmm\" type=\"%d\" from=\"%d\" to=\"%d\" dur=\"hmm\" additive=\"hmm\" />",
		/*iDataType,*/ (int)iFromFloat, (int)iToFloat /*,iAccumulate*/);
		#endif
	}
}
