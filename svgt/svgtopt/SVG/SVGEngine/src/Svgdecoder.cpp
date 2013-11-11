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
* Description:  SVG Engine source file
 *
*/

#include <e32def.h>
#include <e32base.h>
#include <s32mem.h>
#include "Svgdecoder.h"
#include "SVGClrCssValueImpl.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGIntCssValueImpl.h"
#include "SVGStrCssValueImpl.h"
#include "SVGVectorCssValueImpl.h"
#include "GfxAffineTransform.h"
#include "SVGTextElementImpl.h"
#include "SVGFontFaceElementImpl.h"
#include "SvgStopElementImpl.h"
#include "SVGUseElementImpl.h"
#include "SVGAnimationBase.h"
#include "SVGAnimateMotionElementImpl.h"
#include "SVGMpathElementImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGDiscardElementImpl.h"
#include "SVGMediaElementBase.h"
#include "SVGMediaAnimationElementImpl.h"
#include "SVGTimedEntityInterface.h"
#include "SVGAudioElementImpl.h"

#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGMemoryManager.h"

#include <languages.hrh>

_LIT(OFFSET, "offset");
_LIT(ZEROVALUE, "0");

// --------------------------------------------------------------------------
// CSvgDecoder* CSvgDecoder::NewL(const TDesC& aFileName )
// ---------------------------------------------------------------------------
CSvgDecoder* CSvgDecoder::NewL(const TDesC& aFileName )
{
	CSvgDecoder *self = new (ELeave) CSvgDecoder();

	CleanupStack::PushL(self);

    self->ConstructL(aFileName);

	CleanupStack::Pop();

	return self;
}


// --------------------------------------------------------------------------
// void CSvgDecoder::ConstructL(const TDesC& aFileName)
// ---------------------------------------------------------------------------
void CSvgDecoder::ConstructL(const TDesC& aFileName)
{
	User::LeaveIfError(iFs.Connect() );
	User::LeaveIfError (iFStream.Open(iFs, aFileName, EFileRead));
	iStream= iFStream;

	/*Arrays Added For forward referencing in USE and Animate elements*/
	iUseElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iUseElementArray->Reset();

	iAnimRefElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iAnimRefElementArray->Reset();
}


// --------------------------------------------------------------------------
// CSvgDecoder* CSvgDecoder::NewL(const TDesC8& aByteData )
// ---------------------------------------------------------------------------
CSvgDecoder* CSvgDecoder::NewL(const TDesC8& aByteData )
{
	CSvgDecoder *self = new (ELeave) CSvgDecoder();

	CleanupStack::PushL(self);

    self->ConstructL(aByteData);

	CleanupStack::Pop();

	return self;
}


// --------------------------------------------------------------------------
// void CSvgDecoder::ConstructL(const TDesC8& aByteData)
// ---------------------------------------------------------------------------
void CSvgDecoder::ConstructL(const TDesC8& aByteData)
{
	iDStream.Open(aByteData);
	iStream= iDStream;

	iUseElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iUseElementArray->Reset();

	iAnimRefElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iAnimRefElementArray->Reset();

	// added to hold switch element handles so we can go back and delete false children of the switch
	iSwitchElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
    iSwitchElementArray->Reset();

	// Arrays added to reset event listeners for animation element
	iAnimationElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iAnimationElementArray->Reset();

    //added to hold elements with required features, attributes, or sys language
    //so we can go back and delete the ones that dont pass at the start.
    iReqFetAttSysArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iReqFetAttSysArray->Reset();
}


// --------------------------------------------------------------------------
// CSvgDecoder::CSvgDecoder()
// ---------------------------------------------------------------------------
CSvgDecoder::CSvgDecoder()
		:iRootElement( NULL ),
		 iIsAnimElement(EFalse),
		 iIsFixPt(EFalse),
		 iIsRGB(EFalse),
		 iEmbeddedImagesCount(0)

{
}


// --------------------------------------------------------------------------
// CSvgDecoder::~CSvgDecoder()
// ---------------------------------------------------------------------------
CSvgDecoder::~CSvgDecoder()
{
	iStream.Close();
	iDStream.Close();
	iFStream.Close();

	iFs.Close();

	if(iUseElementArray)
	    {
		iUseElementArray->Reset();
		iUseElementArray->Close();
		delete iUseElementArray;
		iUseElementArray = NULL;
		}

	if(iAnimRefElementArray)
	    {
		iAnimRefElementArray->Reset();
		iAnimRefElementArray->Close();
		delete iAnimRefElementArray;
		iAnimRefElementArray = NULL;
		}

	if(iSwitchElementArray)
        {
        iSwitchElementArray->Close();
        delete iSwitchElementArray;
        }
    if (iReqFetAttSysArray)
        {
        iReqFetAttSysArray->Close();
        delete iReqFetAttSysArray;
        }
    if(iAnimationElementArray)
        {
		iAnimationElementArray->Reset();
        iAnimationElementArray->Close();
        delete iAnimationElementArray;
        }
    
        iImageElements.Close();
    
}


// --------------------------------------------------------------------------
// TDesC& CSvgDecoder::DecodeTDesCLC()
// ---------------------------------------------------------------------------
TDesC& CSvgDecoder::DecodeTDesCLC()
{

	TUint8 lUniCodeLen = 2;

	TUint8 uStrLen;
	iStream >> uStrLen ;
	HBufC *lBuf = NULL;

	if (uStrLen)
		{

		lBuf = HBufC::NewLC(uStrLen/lUniCodeLen);
        TPtr lTmp= lBuf->Des();
    	iStream.ReadL (lTmp, (TInt)  (uStrLen/lUniCodeLen));
		}

	else
		{
		lBuf = HBufC::NewLC(0);
		}

		return (TDesC &) *lBuf;
}

// --------------------------------------------------------------------------
// TDesC& CSvgDecoder::DecodeImageEmbeddedTDesCLC()
// ---------------------------------------------------------------------------
TDesC& CSvgDecoder::DecodeImageEmbeddedTDesCLC()
{

//	TUint8 lUniCodeLen = 2;

	TInt32 lImageSize;
	iStream >> lImageSize ;

	HBufC *lBuf = NULL;
    HBufC8* lBuffer =NULL;

	if (lImageSize)
		{
        lBuffer = HBufC8::NewLC(lImageSize);
        TPtr8 lTemp= lBuffer->Des();
    	iStream.ReadL (lTemp, (TInt)  (lImageSize));

		lBuf = HBufC::NewLC(lImageSize);
        TPtr lTmp= lBuf->Des();
        lTmp.Copy(lTemp);
		}

	else
		{
        lBuffer = HBufC8::NewLC(0);
		lBuf = HBufC::NewLC(0);
		}


	return (TDesC &) *lBuf;
}



// ==========================================================================
// Decodig Attribute
// ==========================================================================
TInt CSvgDecoder::DecodeAttributeL(const TUint16 aName)
	{

	if( DecodePresentationAttributeL (aName) ||
		DecodeSVGTAttributeL (aName) ||
		DecodeSvgElementAttributeL (aName) ||
		DecodeIdAndXmlAttributeL ( aName ) ||
		DecodeUriRefAttributeL ( aName ) ||
		DecodeAnimationAttributeL (aName) ||
		DecodeTransformAttributeL (aName) ||
		DecodeLangSpaceAttributeL (aName) ||
		DecodeTestAttributeL ( aName ) ||
		DecodeDiscardAttributeL ( aName )
	  )
		{
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodePresentationAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodePresentationAttributeL(const TUint16 aName)
	{

	if(aName<=KCSS_MAX_ATTR)
		{

		if (aName== KCSS_ATTR_FILL )
			{

		    CCssValue* tParentValue = NULL;
			CCssValue* tValue = NULL;

			iCurrentElement->FindProperty (KCSS_ATTR_FILL,tValue);

			if(iCurrentElement!= iRootElement)
				{
				iCurrentParentElement->FindProperty (KCSS_ATTR_FILL,tParentValue);
				}

			if (tParentValue == tValue || tValue == NULL)
				{
				if (iDocument)
				{
					tValue = iDocument->iMemoryManager->GetCssPaintObjectL(iCurrentElement);
				}
				SetPaintValueL (tValue);
			    iCurrentElement->SetPresentationAttribute (KCSS_ATTR_FILL,tValue);
				}
			else
				{
				SetPaintValueL (tValue);
				}

			return ETrue;
			}

		if (aName== KCSS_ATTR_STROKE || aName== KCSS_ATTR_COLOR)
			{
		 	return DecodeColorCssValueL(aName);
			}

		if (aName== KCSS_ATTR_GROUP_OPACITY)
			{
			iDocument->iHasGroupOpacity = ETrue;
			return DecodeFloatCssValueL(aName);
			}
		if (aName== KCSS_ATTR_FILL_OPACITY ||
			aName== KCSS_ATTR_STROKE_OPACITY ||
			aName== KCSS_ATTR_STROKEWIDTH ||
			aName== KCSS_ATTR_FONTSIZE ||
			aName== KCSS_ATTR_STROKE_MITERLIMIT ||
			aName== KCSS_ATTR_STROKE_DASHOFFSET
			)

			{
			return DecodeFloatCssValueL(aName);
			}

		if (aName== KCSS_ATTR_VISIBILITY ||
			aName== KCSS_ATTR_FONTSTYLE ||
			aName== KCSS_ATTR_FONTWEIGHT ||
			aName== KCSS_ATTR_DISPLAY ||
            aName== KCSS_ATTR_TEXTANCHOR ||
			aName== KCSS_ATTR_TEXTDECORATION
			)

			{
			return DecodeIntCssValueL(aName);
			}

		if (aName== KCSS_ATTR_FONTFAMILY ||
			aName== KCSS_ATTR_FILLRULE ||
			aName== KCSS_ATTR_STROKE_LINECAP ||
			aName== KCSS_ATTR_STROKE_LINEJOIN
			)

			{
			return DecodeStringCssValueL(aName);
			}


		if (aName== KCSS_ATTR_STROKE_DASHARRAY)
			{
			CArrayFix<TFloatFixPt>* lArrayFix;
			TInt8 lCount;
			iStream >> lCount;
			if (lCount)
				{
				lArrayFix= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( lCount );
				}
			else
				{
				lArrayFix= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
				}

			 CleanupStack::PushL(lArrayFix); //cleanup

			TFloatFixPt lTempFix;
			TReal32 lTempReal;
			
			for(TInt8 i=0; i<lCount; i++)
				{
					if (iIsFixPt)
					{
					//was encoded as fixed point
					lTempFix.iValue = iStream.ReadInt32L();
					lArrayFix->AppendL( lTempFix );	
					}
					else
					{
					//was encoded as floats
					
					iStream >> lTempReal;
					lArrayFix->AppendL( (TFloatFixPt) lTempReal);	
					}
				}

			CCssValue* tParentValue = NULL;
		    CCssValue* tValue = NULL;

			iCurrentElement->FindProperty ((TInt8)aName,tValue);

			if(iCurrentElement!= iRootElement)
				{
				iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
				}

            if ( tParentValue == tValue || tValue == NULL )
                {
                // This attribute is inherited from parent, or not 
                // defined in parent
                if ( iDocument )
                    {
                    // Create a new Vector object and clone lArrayFix
                    tValue = iDocument->iMemoryManager->GetCssVectorObjectL( lArrayFix );
                    // Associate tValue with the current element
                    iCurrentElement->SetPresentationAttribute ( ( TInt8 ) aName,tValue );
                    }
                // tValue contains a copy of lArrayFix, hence lArrayFix is not needed
                // anymore. 
                // tValue is owned by the element.
                CleanupStack::PopAndDestroy( lArrayFix ); //cleanup
                }
            else
                {
                // stroke dash array was already present in the element
                // SetValueL function takes ownership of lArrayFix
                ((CVectorCssValueImpl*)tValue)->SetValueL ( lArrayFix );
                // No need to destroy lArrayFix as it is now owned 
                // by the element.
                CleanupStack::Pop( lArrayFix ); //cleanup
                }


			return ETrue;

			}

		else
			{
			return ETrue;
			}

		}

	else
		{
		return EFalse;
		}

	}


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeSVGTAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeSVGTAttributeL(const TUint16 aName)
	{

	if( (iCurrentElement->ElemID() == KSvgSvgElement) &&
      ( aName== KAtrWidth || aName== KAtrHeight ) )
		{
		TUint8 lFlag;
		
		iStream >> lFlag ;
		
		if (iIsFixPt)
		{
			TFloatFixPt lAtrValue;
			lAtrValue.iValue = iStream.ReadInt32L();
			
				// Flag is used to ckech whether height and width are percentage or not.
			if(lFlag)
			{
			    if(aName == KAtrWidth)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = ETrue;
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInUserCoordinate = lAtrValue;
			    }
			    else if(aName == KAtrHeight)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = ETrue;
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInUserCoordinate = lAtrValue;
			    }
			}
			else
			{
			    if(aName == KAtrWidth)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = EFalse;
			    }
			    else if(aName == KAtrHeight)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = EFalse;
			    }
			iCurrentElement->SetAttributeFloatL( (TInt)aName, lAtrValue );
			}
		}
		else
		{
			TReal32 lAtrValue;
			iStream >> lAtrValue;
			
				// Flag is used to ckech whether height and width are percentage or not.
			if(lFlag)
			{
			    if(aName == KAtrWidth)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = ETrue;
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInUserCoordinate = lAtrValue;  
			    }
			    else if (aName == KAtrHeight)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = ETrue;
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInUserCoordinate = lAtrValue;
			    }

			}
			else
			{
			    if(aName == KAtrWidth)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = EFalse;
			    }
			    else if(aName == KAtrHeight)
			    {
			        ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = EFalse;
			    }   
			iCurrentElement->SetAttributeFloatL( (TInt)aName, (TFloatFixPt)lAtrValue );
			}
		}
		
	    return ETrue;
		}

	if(iCurrentElement->ElemID() == KSvgTextElement)
		{
		if (aName== KAtrX || aName== KAtrY)
			{
			CArrayFixFlat<TFloatFixPt>* lArrayFix;
			TInt8 lCount;
			iStream >> lCount;
			if (lCount)
				{
				lArrayFix= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( lCount );
				}
			else
				{
				lArrayFix= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
				}

			CleanupStack::PushL(lArrayFix); //cleanup

			for(TInt8 i=0; i<lCount; i++)
				{
				if (iIsFixPt)
				{
					TFloatFixPt lTemp;
					lTemp.iValue = iStream.ReadInt32L();
					lArrayFix->AppendL( lTemp );
				}
				else
				{
					TReal32 lTemp;
					iStream >> lTemp ;
					lArrayFix->AppendL( (TFloatFixPt) lTemp);	
				}
				
				}

			((CSvgTextElementImpl*)iCurrentElement)->SetXYArray(aName,lArrayFix);
			CleanupStack::Pop(lArrayFix); //cleanup
			return ETrue;
			}
		}


	if( aName<KSvgFloatAttrEndIndex ||
		aName== KAtrFx ||
		aName== KAtrFy ||
		aName== KAtrOffset ||
		aName== KAtrStopOpacity
	  )
		{

		return DecodeAttributeFloatL(aName);
		}

	else if(aName == KAtrK)
		{
		iCurrentElement->iReqAttrFlag =0;
		return DecodeAttributeFloatL(aName);
		}

	else if( aName<= KSvgDesAttrEndIndex ||
    		 aName== KAtrMedia ||
			 aName== KAtrTitle
		   )
		{

		return DecodeAttributeDesL(aName);
		}
 else if (aName== KAtrVolume)
			{
			if (iIsFixPt)
			{
				TFloatFixPt lTemp;
				lTemp.iValue = iStream.ReadInt32L();
				TBuf<16> lTemp2;
				_LIT(KAudioLevelFormat, "%f");
				lTemp2.Format(KAudioLevelFormat, (float)lTemp );
				//lTemp2.AppendNum((float)lTemp );
				((CSvgAudioElementImpl *)iCurrentElement)->SetAttributeL (_L("volume"),lTemp2);
			}
			else
			{
				TReal32 lTemp;
				iStream >> lTemp;
				TBuf<16> lTemp2;
				_LIT(KAudioLevelFormat, "%f");
				lTemp2.Format(KAudioLevelFormat, (float)lTemp );
				//lTemp2.AppendNum(lTemp);
				((CSvgAudioElementImpl *)iCurrentElement)->SetAttributeL (_L("volume"),lTemp2);
			}
			return ETrue;
			}
	else if (aName== KAtrAudioLevel)
			{
			if (iIsFixPt)
			{
				TFloatFixPt lTemp;
				lTemp.iValue = iStream.ReadInt32L();
				TBuf<16> lTemp2;
				_LIT(KAudioLevelFormat, "%f");
				lTemp2.Format(KAudioLevelFormat, (float)lTemp );
				//lTemp2.AppendNum((float)lTemp );
				((CSvgAudioElementImpl *)iCurrentElement)->SetAttributeL (_L("audio-level"),lTemp2);
			}
			else
			{
				TReal32 lTemp;
				iStream >> lTemp;
				TBuf<16> lTemp2;
				_LIT(KAudioLevelFormat, "%f");
				lTemp2.Format(KAudioLevelFormat, (float)lTemp );
				//lTemp2.AppendNum(lTemp);
				((CSvgAudioElementImpl *)iCurrentElement)->SetAttributeL (_L("audio-level"),lTemp2);
			}
			return ETrue;
			}

	switch( aName )
		{
		case KAtrType:
			{
			if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
				{
				if (iCurrentElement->iReqAttrFlag == KAtrSVGTrf)
					{
					iCurrentElement->iReqAttrFlag=KAtrAttributeName;
					}
				else
					{
					iCurrentElement->iReqAttrFlag= 0;
					}
				TUint8 lType, lSize;
				iStream >> lType;
				((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
				iStream >> lSize;
				((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetMatrixDataSize(lSize);
				return ETrue;
				}
			else if(iCurrentElement->ElemID()== KSvgAudioElement)
				    {
				    TPtrC lPtr(DecodeTDesCLC());
				    ((CSvgAudioElementImpl*)iCurrentElement)->SetAttributeL(_L("type"),lPtr);
		            CleanupStack::PopAndDestroy( 1 );				    
				return ETrue;
				}
			else
				{
				return DecodeAttributeDesL(aName);
				}
			}

        case KAtrCdata:
            {
            if ( iCurrentElement->ElemID() == KSvgTextElement )
                {
                // DecodeTDesCLC() allocates the string on the cleanup stack 
                // and advances the file pointer beyond the CData
                // SetTextL() makes a copy of the CData in the text element. 
                ( ( CSvgTextElementImpl* )iCurrentElement )->SetTextL( 
                        DecodeTDesCLC() ); 
                }
            else
                {
                // Skip the CData
                DecodeTDesCLC();
                }
            // Free the CData allocated inside DecodeTDesCLC()
            CleanupStack::PopAndDestroy( 1 );
            return ETrue;
            }

		case KAtrD:
		case KAtrPoints:
			{
			iCurrentElement->iReqAttrFlag = 0;
			return DecodeAttributePathL(aName);
			}

		case KAtrSpreadMethods:
		case KAtrGradientUnits:
			{
			return DecodeAttributeIntL(aName);
			}

		case KAtrRotate:
			{
			// for AnimateMotion Element
			if (iCurrentElement->ElemID()== KSvgAnimateMotionElement)
				{
				TInt16 lRotate;
				iStream >> lRotate;
				((CSvgAnimateMotionElementImpl*)iCurrentElement)->SetAnimRotate(lRotate);
				}

			// For Text Element
			else
				{
				TInt8 lCount;
				CArrayFixFlat<TReal32>*   lArray;
				iStream >> lCount;
				if(lCount)
					{
					lArray= new ( ELeave ) CArrayFixFlat<TReal32>( lCount );
					CleanupStack::PushL(lArray); //cleanup
					for (TInt i=0; i<lCount; i++)
						{
							TReal32 lTemp;
							iStream >> lTemp;
							lArray->AppendL((TReal32) lTemp);
						}
    			
    			    if ( iCurrentElement->ElemID() == KSvgTextElement )	
	    		        {
		    	        ((CSvgTextElementImpl*)iCurrentElement)->SetRotateArray (lArray);
			            }
					
					CleanupStack::Pop(lArray); //cleanup
					}
				}

			return ETrue;
			}

		case KAtrStopColor:
			{
			TUint32 lColor;
			iStream >> lColor;
			// Shift from XBGR to XRGB
			if(!iIsRGB)
				{
				lColor = (  lColor & 0xFF000000) |
					  	 ( (lColor & 0x00FF0000) >> 16 ) |
				    	 (  lColor & 0x0000FF00       ) |
				     	 ( (lColor & 0x000000FF) << 16 );
				}
			
			if ( iCurrentElement->ElemID() == KSvgStopElement )	
			    {
    		    ((CSvgStopElementImpl*)iCurrentElement)->SetStopColorL(lColor);			    
			    }

			return ETrue;
			}

		default:
			{
			return EFalse;
			}
		}
	}

// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeAnimationAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeAnimationAttributeL(const TUint16 aName)
	{

	if (aName== KAtrAttributeName)
		{
		TUint16 lAtrId;
		TUint8 lType;
		iStream >> lAtrId;

		// For Animate Transform element
		if (iCurrentElement->ElemID()== KSvgAnimateTransformElement)
			{
			// Attribute Id of the animate transform has been changed. For binary compatibility
			if (lAtrId == KAtrTransform )
				{
                 lAtrId= KAtrAnimateTransformAttrId;
				}
			((CSvgAnimationBase*)iCurrentElement)->SetAttributeId(lAtrId);

			if (iCurrentElement->iReqAttrFlag == KAtrSVGTrf)
				{
				iCurrentElement->iReqAttrFlag=KAtrType;
				}
			else
				{
				iCurrentElement->iReqAttrFlag= 0;
				}
			}

		//For other animation elements
		else
			{
			((CSvgAnimationBase*)iCurrentElement)->SetAttributeId(lAtrId);
			iCurrentElement->iReqAttrFlag= 0;
			iStream >> lType;
			((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
			((CSvgAnimationBase*)iCurrentElement)->GetAttrValueL();
			}
		return ETrue;
		}

	if (aName== KAtrBegin)
		{
        // Although discard element belongs to animation, the discard element isn't
        // necessary inherited from giant AnimationBase, so handle attribute separately.
        //
    	if ( iCurrentElement->ElemID() == KSvgDiscardElement /*|| 
             iCurrentElement->ElemID() == KSvgMediaAnimationElement*/ )
            {
    	    return EFalse;
            }

		TInt8 lCheck, lCount;
		iStream >> lCheck;
		// Flag is used to check for Indefinite value.
		if(lCheck)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetAbsoluteBeginTime(KTimeIndefinite);
            ((CSvgAnimationBase*)iCurrentElement)->SetBeginTime(KTimeIndefinite);
			}
		else
			{
			CArrayFixFlat<CSvgAnimationBase::TEventListItem>*  lEventList;
			RArray<TInt32>*    lTimeList;
        // event list
			iStream >> lCount;
			if (lCount)
				{
                lEventList= new (ELeave) CArrayFixFlat<CSvgAnimationBase::TEventListItem>(lCount);
				}
			else
				{
				lEventList= new (ELeave) CArrayFixFlat<CSvgAnimationBase::TEventListItem>(1);
				}

			CleanupStack::PushL(lEventList); //cleanup

            for( TInt i=0; i<lCount; i++)
				{
				CSvgAnimationBase::TEventListItem lItem;
				TInt32 lTime;
				iStream >> lTime;
				lItem.iTime= lTime;

				TInt32 lOffset;
				iStream >> lOffset;
				lItem.iOffset= lOffset;

				if (iIsFixPt)
				{
					TFloatFixPt temp;
					temp.iValue = iStream.ReadInt32L();
					lItem.iRepeatValue = (TReal32)temp;
				}
				else
				{
					TReal32 lValue;
					iStream >> lValue;
					lItem.iRepeatValue= lValue;
				}
				

				TPtrC lPtr(DecodeTDesCLC());
				// check for the use of unique Id of the target element
				if(lPtr == _L("unid"))
					{
					lItem.iTargetElement = (((CSvgAnimationBase*)iCurrentElement)->GetTargetElem());
					lItem.iReferenceElemeId= NULL;
					}
				else
					{
					// get element by id and append its pointer.

                	// there can be an issue if the id is not found. in  case of forward reference.
                	// for this we will store the id if it is not found at this time.
					lItem.iTargetElement =  (CSvgElementImpl*)iDocument->GetElementById( lPtr );
					if( lItem.iTargetElement == NULL)
                     	{
                     	// this indicates that this is a forward reference in case of begin.

                     	lItem.iReferenceElemeId = lPtr.AllocL();
                     	}
					else
                     	{
                     	// otherwise the element is found and we
                     	// do not need to keep iReferenceElemeId so
                     	// make it NULL;
                     	lItem.iReferenceElemeId = NULL;
                     	}
					}

				CleanupStack::PopAndDestroy( 1 );

				TUint16 lCode;
				iStream >> lCode;
				lItem.iAccessKeyCode= lCode;

				TInt8 lEvent;
				iStream >> lEvent;
				lItem.iEvent= (TSvgEvent) lEvent;

				switch(lItem.iEvent)
					{
					case ESvgEventKey:
					case ESvgEventNone:
						{
						lItem.iTargetElement =NULL;
						break;
						}
					}
                lEventList->AppendL((CSvgAnimationBase::TEventListItem) lItem);
				}

			((CSvgAnimationBase*)iCurrentElement)->SetEventList(lEventList);
			CleanupStack::Pop(lEventList); //cleanup

       //begin time list
			iStream >> lCount;
			if (lCount)
				{
                lTimeList= new (ELeave) RArray<TInt32>(lCount);
				}
			else
				{
				lTimeList= new (ELeave) RArray<TInt32>(1);
				}

                // For proper cleanup
            ((CSvgAnimationBase*)iCurrentElement)->SetBeginTimeList(lTimeList);
            lTimeList->AppendL(NULL);
            lTimeList->Remove(0);

             for( TInt i=0; i<lCount; i++)
                {
                TInt32 lTime;
                iStream >> lTime;
                lTimeList->AppendL( (TInt32) lTime);
                }

			 TInt32 lTime;
			 iStream >> lTime;
			 ((CSvgAnimationBase*)iCurrentElement)->SetAbsoluteBeginTime(lTime);
			 iStream >> lTime;
			 ((CSvgAnimationBase*)iCurrentElement)->SetBeginTime(lTime);


			 iStream >> lCheck;
			 // flag to ckeck Adding to the Reciever EventList
			 if(lCheck)
				{
				iDocument->AddToEventReceiverListL( iCurrentElement,
                                                        KSvgEventMaskExternalUI );
				}
			}
		return ETrue;
		}


	if (aName== KAtrEnd)  //End Time
		{
		TInt8 lCheck;
		iStream >> lCheck;

		// Flag is used to check for Indefinite value
		if(lCheck)
			{
            ((CSvgAnimationBase*)iCurrentElement)->SetEndTime(KTimeIndefinite);
			}
		else
			{
		  	CArrayFixFlat<CSvgAnimationBase::TEventListItem>*  lTimeList;
        // event list

			lTimeList= new (ELeave) CArrayFixFlat<CSvgAnimationBase::TEventListItem>(1);
			CleanupStack::PushL(lTimeList); //cleanup

			CSvgAnimationBase::TEventListItem lItem;
			TInt8 lEvent;
			iStream >> lEvent;
			lItem.iEvent = (TSvgEvent) lEvent;

			if (iIsFixPt)
			{
				TFloatFixPt temp;
				temp.iValue = iStream.ReadInt32L();
				lItem.iRepeatValue = (TReal32)temp;
			}
			else
			{
				TReal32 lValue;
				iStream >> lValue;
				lItem.iRepeatValue= lValue;
			}
			

			TPtrC lPtr( DecodeTDesCLC() );

			if (lPtr == _L("unid") )
				{
				lItem.iTargetElement = ((CSvgAnimationBase*)iCurrentElement)->GetTargetElem();
				lItem.iReferenceElemeId = NULL;
				}
			else
				{
				// get element by id and append its pointer.
				lItem.iTargetElement = ( CSvgElementImpl * )
							 iDocument->GetElementById( lPtr );
				if(lItem.iTargetElement == NULL)
					 {
					 lItem.iReferenceElemeId = lPtr.AllocL();
					 }
				 else
					{
					// Set the iEndReferenceElemeId to NULL:
					lItem.iReferenceElemeId = NULL;
					}
				}

			CleanupStack::PopAndDestroy( 1 );

			switch ( lItem.iEvent )
                {
                case ESvgEventKey:
					{
         			((CSvgAnimationBase*)iCurrentElement)->SetEndTime(KTimeIndefinite );  // same as indefinite

					TUint16 lCode;
					iStream >> lCode;
					lItem.iAccessKeyCode= lCode;
					lItem.iTargetElement =NULL;
		           iDocument->AddToEventReceiverListL( iCurrentElement,
                                                        KSvgEventMaskExternalUI );
	               break;
					}
                case ESvgEventWallClock:
                // not supported yet
                break;
                case ESvgEventNone:
					{
					TInt32 lTime;
					iStream >> lTime;
					lItem.iTime= lTime;
					((CSvgAnimationBase*)iCurrentElement)->AddEndTime(lTime);
					lItem.iTargetElement =NULL;
				    break;
					}
                default:
					{
				    ((CSvgAnimationBase*)iCurrentElement)->SetEndTime( KTimeIndefinite );  // same as indifinite
					TInt32 lOffset;
					iStream >> lOffset;
					lItem.iOffset= lOffset;

					lItem.iTime = KTimeIndefinite;
		            iDocument->AddToEventReceiverListL( iCurrentElement,
                                                       KSvgEventMaskInternal );
					}

                }
			lTimeList->AppendL( (CSvgAnimationBase::TEventListItem) lItem);
			((CSvgAnimationBase*)iCurrentElement)->SetEndTimeList( lTimeList);
			CleanupStack::Pop(lTimeList); //cleanup

			}

		return ETrue;
		}

////////// for multiple end times

	if (aName== KMultipleEndTime)
		{
		TInt8  lCount;

		CArrayFixFlat<CSvgAnimationBase::TEventListItem>*  lTimeList;
       // event list
		iStream >> lCount;
		if (lCount)
			{
            lTimeList= new (ELeave) CArrayFixFlat<CSvgAnimationBase::TEventListItem>(lCount);
			}
		else
			{
			lTimeList= new (ELeave) CArrayFixFlat<CSvgAnimationBase::TEventListItem>(1);
			}

		CleanupStack::PushL(lTimeList); //cleanup

		for( TInt i=0; i<lCount; i++)
			{
			CSvgAnimationBase::TEventListItem lItem;
			TInt32 lTime;
			iStream >> lTime;
			lItem.iTime= lTime;

			TInt32 lOffset;
			iStream >> lOffset;
			lItem.iOffset= lOffset;

			if (iIsFixPt)
			{
				TFloatFixPt temp;
				temp.iValue = iStream.ReadInt32L();
				lItem.iRepeatValue = (TReal32)temp;
			}
			else
			{
				TReal32 lValue;
				iStream >> lValue;
				lItem.iRepeatValue= lValue;
			}	
			

			TPtrC lPtr(DecodeTDesCLC());
			// check for the use of unique Id of the target element
			if(lPtr == _L("unid"))
				{
				lItem.iTargetElement = (((CSvgAnimationBase*)iCurrentElement)->GetTargetElem());
				lItem.iReferenceElemeId= NULL;
				}
			else
				{
				// get element by id and append its pointer.
               	// there can be an issue if the id is not found. in  case of forward reference.
               	// for this we will store the id if it is not found at this time.
				lItem.iTargetElement =  (CSvgElementImpl*)iDocument->GetElementById( lPtr );
				if( lItem.iTargetElement == NULL)
                   	{
                   	// this indicates that this is a forward reference in case of begin.

					lItem.iReferenceElemeId = lPtr.AllocL();
                   	}
				else
                   	{
                   	// otherwise the element is found and we
                   	// do not need to keep iReferenceElemeId so
                   	// make it NULL;
                   	lItem.iReferenceElemeId = NULL;
                   	}
				}

			CleanupStack::PopAndDestroy( 1 );

			TUint16 lCode;
			iStream >> lCode;
			lItem.iAccessKeyCode= lCode;

			TInt8 lEvent;
			iStream >> lEvent;
			lItem.iEvent= (TSvgEvent) lEvent;

			switch ( lItem.iEvent )
                {
                case ESvgEventKey:
                lItem.iTargetElement =NULL;
                ((CSvgAnimationBase*)iCurrentElement)->SetEndTime( KTimeIndefinite );  // same as indefinite

                iDocument->AddToEventReceiverListL( iCurrentElement,
                                                        KSvgEventMaskExternalUI );
                break;

                case ESvgEventWallClock:
                // not supported yet
                break;

                case ESvgEventNone:
                lItem.iTargetElement =NULL;
                // Simple Offset-value
                ((CSvgAnimationBase*)iCurrentElement)->AddEndTime( lItem.iTime );

                break;

                default:
                // <id>.<event> +/- <offeset>
                ((CSvgAnimationBase*)iCurrentElement)->SetEndTime( KTimeIndefinite );  // same as indifinite

                iDocument->AddToEventReceiverListL( iCurrentElement,
                                                        KSvgEventMaskInternal );
                }

			lTimeList->AppendL((CSvgAnimationBase::TEventListItem) lItem);
			}

		((CSvgAnimationBase*)iCurrentElement)->SetEndTimeList(lTimeList);
		CleanupStack::Pop(lTimeList); //cleanup




		return ETrue;
		}


		if (aName== KAtrDur)
			{
			TInt32 lTime;
			TInt8 lCheck;
			iStream >> lTime;
			((CSvgAnimationBase*)iCurrentElement)->SetDurationTime(lTime);
			iStream >> lCheck;
			// Flag is used to check for Indefinite value
			if(lCheck)
				{
				((CSvgAnimationBase*)iCurrentElement)->SetBeginTime(KTimeIndefinite);
				}
			return ETrue;
			}

		if (aName== KAtrFrom)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();

			//For Animate Transform element
			if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
				{
				CSvgAnimateTransformElementImpl::TMatrixData  lMatrix;
				
				if (iIsFixPt)
				{
					for (TInt i=0; i<3; i++)
					{
					lMatrix.iData[i].iValue = iStream.ReadInt32L();
					}	
				}
				else
				{
					TReal32 lTemp;
					for (TInt i=0; i<3; i++)
					{
					iStream >> lTemp;
					lMatrix.iData[i]= (TFloatFixPt) lTemp;
					}	
				}
		
				((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetMatrixData(0,lMatrix);
				}

			// for other Animation elements
			else
				{

				TUint8 lType;
				iStream >>lType;
				((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
				switch ( lType )
					{
					case KSvgTypeOpacity:
					case KSvgTypeLength:
						{
						if (iIsFixPt)
						{	
							TFloatFixPt lValue; 
							lValue.iValue = iStream.ReadInt32L();
							((CSvgAnimationBase*)iCurrentElement)->SetFromFloat(lValue);
						}
						else
						{
							TReal32 lValue;
							iStream >> lValue;
							((CSvgAnimationBase*)iCurrentElement)->SetFromFloat((TFloatFixPt)lValue);
						}
						
						break;
						}
					case KSvgTypeList:
					case KSvgTypePath:
						{
						CGfxGeneralPath* lPath;
						DecodeAnimationPathL(lPath);
						((CSvgAnimationBase*)iCurrentElement)->SetFromPathL(lPath);
						break;
						}
					case KSvgTypeColor:
						{	
						TInt32 lColor;
						iStream >> lColor;
						// Shift from XBGR to XRGB
						if(!iIsRGB)
							{
							lColor = (  lColor & 0xFF000000) |
						         ( (lColor & 0x00FF0000) >> 16 ) |
							     (  lColor & 0x0000FF00       ) |
							     ( (lColor & 0x000000FF) << 16 );
							}
						((CSvgAnimationBase*)iCurrentElement)->SetFromInt(lColor);
						break;
						}
					case KSvgTypeTextAnchor:
                    case KSvgTypeGradientUnits:
					case KSvgTypeSpreadMethod:
					case KSvgTypeInteger:
					case KSvgTypeDisplay:
					case KSvgTypeVisibility:
						{
						TInt32 lTemp;
						iStream >> lTemp;
						((CSvgAnimationBase*)iCurrentElement)->SetFromInt(lTemp);
						break;
						}

				    // viewBox attribute
					case KSvgTypeViewBox:
						{
						
						if (iIsFixPt)
						{
							TGfxRectangle2D temp;
							temp.iX.iValue = iStream.ReadInt32L();
							
							temp.iY.iValue = iStream.ReadInt32L();
							temp.iWidth.iValue = iStream.ReadInt32L();
							temp.iHeight.iValue = iStream.ReadInt32L();

							((CSvgAnimationBase*)iCurrentElement)->SetFromViewBox(temp);
						}
						else
						{
							TReal32 lValue;
							TGfxRectangle2D temp;

							this->iStream >> lValue;
							temp.iX = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iY = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iWidth = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iHeight = (TFloatFixPt) lValue;

							((CSvgAnimationBase*)iCurrentElement)->SetFromViewBox(temp);	
						}
						break;
						}
					}
				}
			 return ETrue;
			}

		if (aName== KAtrTo)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetToFlag();

			// For Animate Transform
			if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
				{
				if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
					{
					((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
					}
				CSvgAnimateTransformElementImpl::TMatrixData  lMatrix;
				
				if (iIsFixPt)
				{
					for (TInt i=0; i<3; i++)
					{
					lMatrix.iData[i].iValue = iStream.ReadInt32L();
					}
				}
				else
				{
					TReal32 lTemp;
					for (TInt i=0; i<3; i++)
					{
					iStream >> lTemp;
					lMatrix.iData[i]= (TFloatFixPt) lTemp;
					}
				}
			
				((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetMatrixData(1,lMatrix);
				((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetAccumMatrix();
				}

			// for Other Animation Elements
			else
				{
				TUint8 lType;
				iStream >>lType;
				((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
				switch ( lType )
					{
					case KSvgTypeOpacity:
					case KSvgTypeLength:
						{
						
						if (iIsFixPt)
						{
							TFloatFixPt lValue;
							lValue.iValue = iStream.ReadInt32L();
							((CSvgAnimationBase*)iCurrentElement)->SetToFloat(lValue);
						}
						else
						{
							TReal32 lValue;
							iStream >> lValue;
							((CSvgAnimationBase*)iCurrentElement)->SetToFloat((TFloatFixPt)lValue);
						}
					
						break;
						}
					case KSvgTypeList:
					case KSvgTypePath:
						{
						CGfxGeneralPath* lPath;
						DecodeAnimationPathL(lPath);
						((CSvgAnimationBase*)iCurrentElement)->SetToPath(lPath);
						break;
						}
					case KSvgTypeColor:
					{
						TInt32 lColor;
						iStream >> lColor;
						// Shift from XBGR to XRGB
						if(!iIsRGB)
							{
							lColor = (  lColor & 0xFF000000) |
						         ( (lColor & 0x00FF0000) >> 16 ) |
							     (  lColor & 0x0000FF00       ) |
							     ( (lColor & 0x000000FF) << 16 );
							}
						((CSvgAnimationBase*)iCurrentElement)->SetToInt(lColor);
						break;
					}
					case KSvgTypeTextAnchor:
					case KSvgTypeGradientUnits:
					case KSvgTypeSpreadMethod:
					case KSvgTypeInteger:
					case KSvgTypeDisplay:
					case KSvgTypeVisibility:
						{
						TInt32 lTemp;
						iStream >> lTemp;
						((CSvgAnimationBase*)iCurrentElement)->SetToInt(lTemp);
						break;
						}

					 // viewBox attribute
					case KSvgTypeViewBox:
						{
						if (iIsFixPt)
						{
							TGfxRectangle2D temp;
							temp.iX.iValue = iStream.ReadInt32L();
							temp.iY.iValue = iStream.ReadInt32L();
							temp.iWidth.iValue = iStream.ReadInt32L();
							temp.iHeight.iValue = iStream.ReadInt32L();

							((CSvgAnimationBase*)iCurrentElement)->SetToViewBox(temp);
						}
						else
						{
							TReal32 lValue;
							TGfxRectangle2D temp;

							this->iStream >> lValue;
							temp.iX = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iY = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iWidth = (TFloatFixPt) lValue;
							this->iStream >> lValue;
							temp.iHeight = (TFloatFixPt) lValue;

							((CSvgAnimationBase*)iCurrentElement)->SetToViewBox(temp);	
						}
			
						break;
						}

					}
				}
			 return ETrue;

			}

		if (aName== KAtrBy) //by
			{
			((CSvgAnimationBase*)iCurrentElement)->SetByFlag();

			// For Animate Transform
			if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
				{
				if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
					{
					((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
					}
				CSvgAnimateTransformElementImpl::TMatrixData  lMatrix;
				
				if (iIsFixPt)
				{
					for (TInt i=0; i<3; i++)
					{
					lMatrix.iData[i].iValue = iStream.ReadInt32L();
					}
				}
				else
				{
					TReal32 lTemp;
					for (TInt i=0; i<3; i++)
					{
					iStream >> lTemp;
					lMatrix.iData[i]= (TFloatFixPt) lTemp;
					}
				}
				
				((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetMatrixData(1,lMatrix);
				}

			// for Other Animation Elements
			else
				{
				TUint8 lType;
				iStream >>lType;
				((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
				switch ( lType )
					{
					case KSvgTypeOpacity:
					case KSvgTypeLength:
						{
						if (iIsFixPt)
						{
							TFloatFixPt lValue;
							lValue.iValue = iStream.ReadInt32L();
							
							((CSvgAnimationBase*)iCurrentElement)->SetToFloat( lValue );
							if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
							{
							((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
                            ((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();
							((CSvgAnimationBase*)iCurrentElement)->SetFromFloat((TFloatFixPt)0);
							}
						}
						else
						{
							TReal32 lValue;
							iStream >> lValue;
							
							((CSvgAnimationBase*)iCurrentElement)->SetToFloat((TFloatFixPt)lValue);
							if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
							{
							((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
                            ((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();
							((CSvgAnimationBase*)iCurrentElement)->SetFromFloat((TFloatFixPt)0);
							}
						}
						
						break;
						}
					case KSvgTypeList:
					case KSvgTypePath:
						{
						CGfxGeneralPath* lPath;
						DecodeAnimationPathL(lPath);
						((CSvgAnimationBase*)iCurrentElement)->SetToPath(lPath);
						break;
						}

                    case KSvgTypeColor:
						{
						TInt32 lColor;
						iStream >> lColor;
						// Shift from XBGR to XRGB
						if(!iIsRGB)
							{
							lColor = (  lColor & 0xFF000000) |
						         ( (lColor & 0x00FF0000) >> 16 ) |
							     (  lColor & 0x0000FF00       ) |
							     ( (lColor & 0x000000FF) << 16 );
							}
						((CSvgAnimationBase*)iCurrentElement)->SetToInt(lColor);
						if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
							{
							((CSvgAnimationBase*)iCurrentElement)->SetFromInt((TInt32)0);
							}
						break;
						}
					case KSvgTypeTextAnchor:
                    case KSvgTypeGradientUnits:
					case KSvgTypeSpreadMethod:
					case KSvgTypeInteger:
						{
						TInt32 lTemp;
						iStream >> lTemp;
						((CSvgAnimationBase*)iCurrentElement)->SetToInt(lTemp);
						if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
							{
							((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
                            ((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();
							((CSvgAnimationBase*)iCurrentElement)->SetFromInt((TInt32)0);
							}
						break;
						}

                    case KSvgTypeDisplay:
					case KSvgTypeVisibility:
						{
						TInt32 lTemp;
						iStream >> lTemp;
						((CSvgAnimationBase*)iCurrentElement)->SetToInt(lTemp);
						break;
						}

					 // viewBox attribute
					case KSvgTypeViewBox:
						{
						if (iIsFixPt)
						{
							TGfxRectangle2D temp;
							temp.iX.iValue = iStream.ReadInt32L();
							temp.iY.iValue = iStream.ReadInt32L();
							temp.iWidth.iValue = iStream.ReadInt32L();
							temp.iHeight.iValue = iStream.ReadInt32L();
							
							((CSvgAnimationBase*)iCurrentElement)->SetToViewBox(temp);
						}
						else
						{
							TReal32 lValue;
							TGfxRectangle2D temp;

							iStream >> lValue;
							temp.iX = (TFloatFixPt) lValue;
							iStream >> lValue;
							temp.iY = (TFloatFixPt) lValue;
							iStream >> lValue;
							temp.iWidth = (TFloatFixPt) lValue;
							iStream >> lValue;
							temp.iHeight = (TFloatFixPt) lValue;
							
							((CSvgAnimationBase*)iCurrentElement)->SetToViewBox(temp);
						}
						
						if( ((CSvgAnimationBase*)iCurrentElement)->GetFromFlag() )
							{
							((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
                            ((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();
							((CSvgAnimationBase*)iCurrentElement)->SetFromViewBox
								                    (TGfxRectangle2D( 0, 0, 0, 0 ) );
							}
						break;
						}
					}
				}
			return ETrue;

			}


		if (aName== KAtrValues)  // values
			{
			((CSvgAnimationBase*)iCurrentElement)->SetFromFlag();
			((CSvgAnimationBase*)iCurrentElement)->SetValuesFlag();

			// For Animate Transform
			if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
				{
				TUint16 lCount;
				RArray<CSvgAnimateTransformElementImpl::TMatrixData>* lValues= NULL;

				iStream >>lCount;
				if (lCount)
					{
					lValues= new (ELeave) RArray<CSvgAnimateTransformElementImpl::TMatrixData>(lCount);
					}
				else
					{
					lValues= new (ELeave) RArray<CSvgAnimateTransformElementImpl::TMatrixData>(1);
					}

				// for proper cleanup
                ((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetTransformValues(lValues);
                CSvgAnimateTransformElementImpl::TMatrixData  lMatrix1;
                lValues->AppendL(lMatrix1);
                lValues->Remove(0);

                for (TInt i=0; i<lCount; i++)
                        {
                        CSvgAnimateTransformElementImpl::TMatrixData  lMatrix;
                        
                        if (iIsFixPt)
                        {
                        	for (TInt i=0; i<3; i++)
                            {
                            lMatrix.iData[i].iValue = iStream.ReadInt32L();
                            }
                        }
                        else
                        {
                        	TReal32 lTemp;
                        	for (TInt i=0; i<3; i++)
                            {
                            iStream >> lTemp;
                            lMatrix.iData[i]= (TFloatFixPt) lTemp;
                            }
                        }
                        
                        lValues->AppendL( (CSvgAnimateTransformElementImpl::TMatrixData) lMatrix);
                        }
				if(iCurrentElement->ElemID()== KSvgAnimateTransformElement)
					{
					((CSvgAnimateTransformElementImpl*)iCurrentElement)->SetAccumMatrix();
					}
				}


			// For other Animation elements
			else
				{
				TUint8 lType;
				iStream >>lType;
				((CSvgAnimationBase*)iCurrentElement)->SetAttributeType(lType);
				switch ( lType )
					{
					case KSvgTypeOpacity:
					case KSvgTypeLength:
						{
						RArray<TFloatFixPt>* lArray;
						TUint16 lCount;
						iStream >> lCount;
						if (lCount)
							{
							lArray= new (ELeave) RArray<TFloatFixPt>(lCount);
							}
						else
							{
							lArray= new (ELeave) RArray<TFloatFixPt>(1);
							}

                        // for proper cleanup
                        ((CSvgAnimationBase*)iCurrentElement)->SetFloatValuesArray(lArray);
                        lArray->AppendL(NULL);
                        lArray->Remove(0);
                        
                        TFloatFixPt lValueFix;
                        TReal32 lValueFloat;
                        
                        for(TInt i=0; i<lCount; i++)
                            {
                            
                            if(iIsFixPt)
                            {
                            	lValueFix.iValue = iStream.ReadInt32L();
                            	lArray->AppendL( lValueFix );
                            }
                            else
                            {
                            	iStream >> lValueFloat;
                            	lArray->AppendL((TFloatFixPt) lValueFloat);
                            }
                            
                            }
						break;
						}
					case KSvgTypeList:
					case KSvgTypePath:
						{
						TUint16 lCount;
						RPointerArray<CGfxGeneralPath>* lArray;
						iStream >> lCount;
						if (lCount)
							{
							lArray= new (ELeave) RPointerArray<CGfxGeneralPath>(lCount);
							}
						else
							{
							lArray= new (ELeave) RPointerArray<CGfxGeneralPath>(1);
							}

						// for proper cleanup
                        ((CSvgAnimationBase*)iCurrentElement)->ResetFloatValuesArray();
                        ((CSvgAnimationBase*)iCurrentElement)->SetPathValuesArray(lArray);
                        lArray->AppendL(NULL);
                        lArray->Remove(0);
                        for(TInt i=0; i<lCount; i++)
                            {
                            CGfxGeneralPath* lPath;
                            DecodeAnimationPathL(lPath);
                            lArray->AppendL( (CGfxGeneralPath*)lPath );
                            }
                        break;
                        }

					case KSvgTypeColor:
						{
						RArray<TInt32>* lArray;
						TUint16 lCount;
						iStream >> lCount;
						if (lCount)
							{
							lArray= new (ELeave) RArray<TInt32>(lCount);
							}
						else
							{
							lArray= new (ELeave) RArray<TInt32>(1);
							}

						// for proper cleanup
                        ((CSvgAnimationBase*)iCurrentElement)->ResetFloatValuesArray();
                        ((CSvgAnimationBase*)iCurrentElement)->SetIntValuesArray(lArray);
                        lArray->AppendL(NULL);
                        lArray->Remove(0);

                        for(TInt i=0; i<lCount; i++)
                            {
                            TInt32 lColor;
                            iStream >> lColor;
							if(!iIsRGB)
								{
								lColor = (  lColor & 0xFF000000) |
							         ( (lColor & 0x00FF0000) >> 16 ) |
								     (  lColor & 0x0000FF00       ) |
								     ( (lColor & 0x000000FF) << 16 );
								}
                            lArray->AppendL((TInt32) lColor);
							}
						break;
						}

                    case KSvgTypeGradientUnits:
					case KSvgTypeSpreadMethod:
					case KSvgTypeInteger:
					case KSvgTypeDisplay:
					case KSvgTypeVisibility:
					case KSvgTypeTextAnchor:
						{
						RArray<TInt32>* lArray;
						TUint16 lCount;
						iStream >> lCount;
						if (lCount)
							{
							lArray= new (ELeave) RArray<TInt32>(lCount);
							}
						else
							{
							lArray= new (ELeave) RArray<TInt32>(1);
							}

						// for proper cleanup
                        ((CSvgAnimationBase*)iCurrentElement)->ResetFloatValuesArray();
                        ((CSvgAnimationBase*)iCurrentElement)->SetIntValuesArray(lArray);
                        lArray->AppendL(NULL);
                        lArray->Remove(0);

                        for(TInt i=0; i<lCount; i++)
                            {
                            TInt32 lValue;
                            iStream >> lValue;
                            lArray->AppendL((TInt32) lValue);
							}
						break;
						}

					// viewBox
					case KSvgTypeViewBox:
						{
						RArray<TGfxRectangle2D>* lArray;
						TUint16 lCount;
						iStream >> lCount;
						if (lCount)
							{
							lArray= new (ELeave) RArray<TGfxRectangle2D>(lCount);
							}
						else
							{
							lArray= new (ELeave) RArray<TGfxRectangle2D>(1);
							}
						TGfxRectangle2D temp1;
                        // for proper cleanup
                        ((CSvgAnimationBase*)iCurrentElement)->SetViewBoxValuesArray(lArray);
                        lArray->AppendL(temp1);
                        lArray->Remove(0);
                        
                        
                        TGfxRectangle2D temp;
                        TReal32 lValueFloat;
                        for(TInt i=0; i<lCount; i++)
                            {
                            
                            if (iIsFixPt)
                            {
								temp.iX.iValue = iStream.ReadInt32L();
								temp.iY.iValue = iStream.ReadInt32L();
                            	temp.iWidth.iValue = iStream.ReadInt32L();
                            	temp.iHeight.iValue = iStream.ReadInt32L();

                            	lArray->AppendL((TGfxRectangle2D) temp);
                            }
                            else
                            {
								iStream >> lValueFloat;
								temp.iX = (TFloatFixPt) lValueFloat;
								iStream >> lValueFloat;
                            	temp.iY = (TFloatFixPt) lValueFloat;
                            	iStream >> lValueFloat;
                            	temp.iWidth = (TFloatFixPt) lValueFloat;
                            	iStream >> lValueFloat;
                            	temp.iHeight = (TFloatFixPt) lValueFloat;

                            	lArray->AppendL((TGfxRectangle2D) temp);
                            }
                            }
						break;
						}
					}
				}
			 return ETrue;

			}

			// Used to set the Fill attribute of Animation elements
		if (aName== KAnimFreeze)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetAnimFreeze();
			return ETrue;
			}

		if (aName== KAtrRepeatDur)
			{
			TUint32 lDur;
			iStream >> lDur;
			((CSvgAnimationBase*)iCurrentElement)->SetRepeatDuration(lDur);
			return ETrue;
			}

		if (aName== KAtrRepeatCount)
			{
			
			if (iIsFixPt)
			{
				TFloatFixPt lCount;
				lCount.iValue = iStream.ReadInt32L();
				((CSvgAnimationBase*)iCurrentElement)->SetRepeatCount(lCount.iValue);
			}
			else
			{
				TReal32 lCount;
				iStream >> lCount;
				((CSvgAnimationBase*)iCurrentElement)->SetRepeatCount(lCount);
			}
			
			return ETrue;
			}

	    if (aName== KAtrRestart)
			{
			TUint8 lMode;
			iStream >> lMode;
		    ((CSvgAnimationBase*)iCurrentElement)->SetRestartMode((TRestartMode)lMode);
			return ETrue;
			}

		if (aName== KAtrAccumulate)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetAccumulate(KAccumSum);
			return ETrue;
			}

		if (aName== KAtrAdditive)
			{
			((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
			return ETrue;
			}

		if (aName== KAtrCalcMode)
			{
			TUint8 lMode;
			iStream >> lMode;
			((CSvgAnimationBase*)iCurrentElement)->SetCalcMode((TAnimCalcMode) lMode);
			return ETrue;
			}

		if (aName== KAtrKeyTimes)
			{
			TUint16 lCount;
			RArray<CSvgAnimTimeController::TKeyTime>* lArray;
			iStream >> lCount;
			if (lCount)
				{
				((CSvgAnimationBase*)iCurrentElement)->SetKeyTimeFlag();
				lArray= new (ELeave) RArray<CSvgAnimTimeController::TKeyTime>(lCount);
				}
			else
				{
				lArray= new (ELeave) RArray<CSvgAnimTimeController::TKeyTime>(1);
				}
			CSvgAnimTimeController::TKeyTime  lTime1;
            // for proper cleanup
            ((CSvgAnimationBase*)iCurrentElement)->SetKeyTimeArray(lArray);
            lArray->AppendL(lTime1);
            lArray->Remove(0);

            for (TInt i=0; i<lCount; i++)
                {
                TUint8 lTemp;
                CSvgAnimTimeController::TKeyTime  lTime;
                iStream >> lTemp;
                lTime.iX = (TUint16) (lTemp);
                iStream >> lTemp;
                lTime.iY = (TUint16) (lTemp);
                lArray->AppendL( (CSvgAnimTimeController::TKeyTime) lTime);
                }
			return ETrue;
			}

		if (aName== KAtrKeySplines)
			{
			TUint16 lCount;
			RArray<CSvgAnimTimeController::TKeyTime>* lArray;
			RArray<TUint32>* lAnimArray;

			// keyTime Array
			iStream >> lCount;
			if (lCount)
				{
				lArray= new (ELeave) RArray<CSvgAnimTimeController::TKeyTime>(lCount);
				}
			else
				{
				lArray= new (ELeave) RArray<CSvgAnimTimeController::TKeyTime>(1);
				}
			CSvgAnimTimeController::TKeyTime  lTime1;
            // for proper cleanup
            ((CSvgAnimationBase*)iCurrentElement)->SetKeyTimeArray(lArray);
            lArray->AppendL(lTime1);
            lArray->Remove(0);
            for (TInt i=0; i<lCount; i++)
                {
                TUint8 lTemp;
                CSvgAnimTimeController::TKeyTime  lTime;
                iStream >> lTemp;
                lTime.iX = (TUint16) (lTemp);
                iStream >> lTemp;
                lTime.iY = (TUint16) (lTemp);
                lArray->AppendL( (CSvgAnimTimeController::TKeyTime) lTime);
                }

			//AnimTime Array
			iStream >> lCount;
			if (lCount)
				{
				lAnimArray = new (ELeave) RArray<TUint32>(lCount);
				}
			else
				{
				lAnimArray= new (ELeave) RArray<TUint32>(1);
				}

            ((CSvgAnimationBase*)iCurrentElement)->SetAnimTimeArray(lAnimArray);
            lAnimArray->AppendL(NULL);
            lAnimArray->Remove(0);

            for (TInt i=0; i<lCount; i++)
                {
                TUint8 lTemp;
                iStream >> lTemp;
                lAnimArray->AppendL( (TUint32) lTemp);
                }
			return ETrue;
		}


	// Animate motion Element is handled here
	if (aName== KAtrAnimateMotion)
		{
		DecodeAttributePathL(KAtrAnimateMotion);
		TUint8 lFlag;
		iStream >> lFlag;
		if(lFlag == KAtrAdditiveSet)
			{
            iCurrentElement->iReqAttrFlag= 0;
			((CSvgAnimationBase*)iCurrentElement)->SetAdditive(KAdditiveSum);
			return ETrue;
			}
        iCurrentElement->iReqAttrFlag= lFlag;
		return ETrue;
		}

	if (aName== KAtrXlinkhref)
		{
	    _LIT( KXlink, "xlink:href" );
		TPtrC lPtr(DecodeTDesCLC());
		CSvgMediaAnimationElementImpl* mediaElement = static_cast<CSvgMediaAnimationElementImpl*>(iCurrentElement);
		mediaElement->SetAttributeL(KXlink, lPtr);
		CleanupStack::PopAndDestroy(1);
		return ETrue;
		}
		
	if( aName == KAtrSyncBehavior )
		{
		TUint8 syncBehavior;
		iStream>>syncBehavior;
		((CSvgMediaElementBase*)iCurrentElement)->SetSyncBehavior( (TSvgSyncBehaviour) syncBehavior );
		return ETrue;
		}
	else if( aName == KAtrSyncTolerance)
		{
		TUint8 syncTolerance;
		iStream>>syncTolerance;
		((CSvgMediaElementBase*)iCurrentElement)->SetSyncTolerance( syncTolerance, EFalse );
		return ETrue;
		}
	else if( aName == KAtrSyncMaster )
		{
		TUint8 syncMaster ;
		iStream>>syncMaster ;
		((CSvgMediaElementBase*)iCurrentElement)->SetSyncMaster( syncMaster );
		return ETrue;
		}
					
	return EFalse;
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeSvgElementAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeSvgElementAttributeL(const TUint16 aName)
	{
	if ( ( aName<= KSvgSVGAttrEndIndex) ||
		 ( aName == KAtrSyncBehaviorDefault ) ||	// To support backward compatibility
		 ( aName == KAtrSyncToleranceDefault ))
		{

		if (aName== KAtrViewBox)
			{
			TGfxRectangle2D lRect;
			
			if (iIsFixPt)
			{
				lRect.iX.iValue = iStream.ReadInt32L();
				lRect.iY.iValue = iStream.ReadInt32L();
				lRect.iWidth.iValue = iStream.ReadInt32L();
				lRect.iHeight.iValue = iStream.ReadInt32L();

				((CSvgSvgElementImpl *)iCurrentElement)->SetViewBoxL(lRect);
			}
			else
			{
				TReal32 lTemp;

				iStream >> lTemp;
				lRect.iX= (TFloatFixPt) lTemp;
				iStream >> lTemp;
				lRect.iY= (TFloatFixPt) lTemp;
				iStream >> lTemp;
				lRect.iWidth= (TFloatFixPt) lTemp;
				iStream >> lTemp;
				lRect.iHeight= (TFloatFixPt) lTemp;

				((CSvgSvgElementImpl *)iCurrentElement)->SetViewBoxL(lRect);
			}
			return ETrue;
			}

		if (aName== KAtrBaseProfile)
			{
			((CSvgSvgElementImpl *)iCurrentElement)->SetBaseProfileL(DecodeTDesCLC());
			 CleanupStack::PopAndDestroy( 1 );
			 return ETrue;
			}

		if (aName== KAtrVersion)
			{
			
			if (iIsFixPt)
			{
				TFloatFixPt lTemp;
				lTemp.iValue = iStream.ReadInt32L();
				TBuf<16> lTemp2;
				lTemp2.AppendNum( (float)lTemp );
				((CSvgSvgElementImpl *)iCurrentElement)->SetVersion (lTemp2);
			}
			else
			{
				TReal32 lTemp;
				iStream >> lTemp;
				TBuf<16> lTemp2;
				lTemp2.AppendNum(lTemp);
				((CSvgSvgElementImpl *)iCurrentElement)->SetVersion (lTemp2);
			}
			
			return ETrue;

			/*
			((CSvgSvgElementImpl *)iCurrentElement)->SetVersion (DecodeTDesCLC());
			CleanupStack::PopAndDestroy( 1 );
			return ETrue;
			*/
			}

		if (aName== KAtrZoomAndPan)
			{
			TInt8 lTemp;
			iStream >> lTemp;
			TSvgZoomAndPanType lType= (TSvgZoomAndPanType) lTemp;
			((CSvgSvgElementImpl *)iCurrentElement)->SetZoomAndPan(lType);
			return ETrue;
			}

		if (aName== KAtrPreserveAspectRatio)
			{
			if (iCurrentElement->ElemID()== KSvgSvgElement)
				{
				TInt8 lTemp;
				iStream >> lTemp;
				TSvgPreserveAspectAlignType lAType= (TSvgPreserveAspectAlignType) lTemp;
				iStream >> lTemp;
				TSvgMeetOrSliceType lMType= (TSvgMeetOrSliceType) lTemp;
				((CSvgSvgElementImpl *)iCurrentElement)->SetPreserveAspectRatioL(lAType, lMType);
				return ETrue;
				}
			else if(iCurrentElement->ElemID()== KSvgMediaAnimationElement)
				{
				TInt8 lTemp;
				iStream >> lTemp;
				TSvgPreserveAspectAlignType lAType= (TSvgPreserveAspectAlignType) lTemp;
				iStream >> lTemp;
				TSvgMeetOrSliceType lMType= (TSvgMeetOrSliceType) lTemp;
				((CSvgMediaAnimationElementImpl*)iCurrentElement)->SetPreserveAspectRatioL(lAType, lMType);
				return ETrue; 
				}
			else
				{
				((CSvgImageElementImpl *)iCurrentElement)->SetParL(DecodeTDesCLC());
				CleanupStack::PopAndDestroy( 1 );
				return ETrue;
				}
			}

		if( aName == KAtrSyncBehaviorDefault )
			{
			TUint8 syncBehaviorDefault;
			iStream>>syncBehaviorDefault;
			((CSvgSvgElementImpl*)iCurrentElement)->SetSyncBehaviorDefault( (TSvgSyncBehaviour) syncBehaviorDefault );
			return ETrue;
			}
		else if( aName == KAtrSyncToleranceDefault )
			{
			TUint8 syncToleranceDefault;
			iStream>>syncToleranceDefault;
			((CSvgSvgElementImpl*)iCurrentElement)->SetSyncToleranceDefault( syncToleranceDefault );
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}

	else
		{
		return EFalse;
		}
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeLangSpaceAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeLangSpaceAttributeL(const TUint16 aName)
	{
	if (aName<= KSvgLangAttrEndIndex)
		{

		if (aName== KAtrXmlLang)
			{
			iCurrentElement->SetXMLLangL (DecodeTDesCLC());
			 CleanupStack::PopAndDestroy( 1 );
			return ETrue;
			}

		if (aName== KAtrXmlSpace)
			{
			iCurrentElement->SetXMLSpaceL (DecodeTDesCLC());
			 CleanupStack::PopAndDestroy( 1 );
			return ETrue;
			}

		else
			{
			return EFalse;
			}
		}

	else
		{
		return EFalse;
		}
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeTransformAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeTransformAttributeL(const TUint16 aName)
	{

	if (aName== KAtrTransform)
		{
		
		if (iIsFixPt)
		{
			TUint32 lTemp1;
			TGfxAffineTransform lTransform;
			
			lTransform.iM00.iValue = iStream.ReadInt32L();
			lTransform.iM01.iValue = iStream.ReadInt32L();
			lTransform.iM02.iValue = iStream.ReadInt32L();
			lTransform.iM10.iValue = iStream.ReadInt32L();
			lTransform.iM11.iValue = iStream.ReadInt32L();
			lTransform.iM12.iValue = iStream.ReadInt32L();
			iStream >> lTemp1;
			lTransform.iTransType= (TGfxTransformType) lTemp1;

			iCurrentElement->SetTransformList(lTransform);
		}
		else
		{
			TReal32 lTemp;
			TUint32 lTemp1;
			TGfxAffineTransform lTransform;

			iStream >> lTemp;
			lTransform.iM00= (TFloatFixPt) lTemp;
			iStream >> lTemp;
			lTransform.iM01= (TFloatFixPt) lTemp;
			iStream >> lTemp;
			lTransform.iM02= (TFloatFixPt) lTemp;
			iStream >> lTemp;
			lTransform.iM10= (TFloatFixPt) lTemp;
			iStream >> lTemp;
			lTransform.iM11= (TFloatFixPt) lTemp;
			iStream >> lTemp;
			lTransform.iM12= (TFloatFixPt) lTemp;
			iStream >> lTemp1;
			lTransform.iTransType= (TGfxTransformType) lTemp1;

			iCurrentElement->SetTransformList(lTransform);
		}
		
		return ETrue;
		}

	else if	( aName== KAtrGradientTransform)
		{
		
		if (iIsFixPt)
		{
			SVGReal lMatrix[2][3];
			
			TFloatFixPt lTemp;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[0][0]= lTemp.iValue;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[0][1]= lTemp.iValue;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[0][2]= lTemp.iValue;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[1][0]= lTemp.iValue;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[1][1]= lTemp.iValue;
			lTemp.iValue = iStream.ReadInt32L();
			lMatrix[1][2]= lTemp.iValue;

			((CSvgGradientElementImpl*)iCurrentElement)->SetGradientTransMatrix(lMatrix);
		}
		else
		{
			TReal32 lTemp;
			SVGReal lMatrix[2][3];

			iStream >> lTemp;
			lMatrix[0][0]= (SVGReal) lTemp;
			iStream >> lTemp;
			lMatrix[0][1]= (SVGReal) lTemp;
			iStream >> lTemp;
			lMatrix[0][2]= (SVGReal) lTemp;
			iStream >> lTemp;
			lMatrix[1][0]= (SVGReal) lTemp;
			iStream >> lTemp;
			lMatrix[1][1]= (SVGReal) lTemp;
			iStream >> lTemp;
			lMatrix[1][2]= (SVGReal) lTemp;

			((CSvgGradientElementImpl*)iCurrentElement)->SetGradientTransMatrix(lMatrix);
		}

		return ETrue;
		}



	else
		{
		return EFalse;
		}
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeTestAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeTestAttributeL(const TUint16 aName)
	{
	if(aName<= KSvgTestAttrEndIndex)
		{

		if (
			aName== KAtrRequiredFeatures ||
			aName== KAtrRequiredExtensions ||
			aName== KAtrSystemLanguage
		   )
			{
			CDesCArrayFlat* lFeatures;
			TInt8 lCount;
			iStream >> lCount;
			if(lCount)
				{
				lFeatures= new ( ELeave ) CDesCArrayFlat( lCount );
				}
			else
				{
				lFeatures= new ( ELeave ) CDesCArrayFlat( 1 );
				}

			CleanupStack::PushL(lFeatures); //cleanup

			for(TInt8 i=0; i<lCount; i++)
				{
				lFeatures->AppendL((const TDesC&)DecodeTDesCLC());
				CleanupStack::PopAndDestroy( 1 );
				}

            iCurrentElement->CreateSvgTestL(aName);  //instantiates iSvgTest
			if (aName==	 KAtrRequiredFeatures)
				{
		         iCurrentElement->SetRequiredFeatures (lFeatures);
				}
			else if (aName== KAtrRequiredExtensions)
				{
				iCurrentElement->SetRequiredExtensions (lFeatures);
				}
			else
				{
		         iCurrentElement->SetSystemLanguage (lFeatures);
				}

			CleanupStack::Pop(lFeatures); //cleanup

		 return ETrue;
			}

		else
			{
			return EFalse;
			}
		}

	else
		{
		return EFalse;
		}
	}




// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeIdAndXmlAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeIdAndXmlAttributeL(const TUint16 aName)
	{
    TBool rVal = EFalse;
	if (aName== KAtrId)
		{
		rVal = iCurrentElement->SetIdandXmlbaseL( _L("id"), DecodeTDesCLC());
		CleanupStack::PopAndDestroy( 1 );
		}
	else if (aName== KAtrXmlBase)
		{
		rVal = iCurrentElement->SetIdandXmlbaseL( _L("xml:base"), DecodeTDesCLC());
		CleanupStack::PopAndDestroy( 1 );
		}
	return rVal;
	}

// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeUriRefAttributeL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeUriRefAttributeL(const TUint16 aName)
	{
   	if (aName<= KSvgUriAttrEndIndex)
		{
 		if (aName== KAtrXlinkhref)
			{
            // Although discard element belongs to animation, the discard element isn't
            // necessary inherited from giant AnimationBase, so handle attribute separately.
            //
        	if ( iCurrentElement->ElemID() == KSvgDiscardElement || iCurrentElement->ElemID() == KSvgMediaAnimationElement )
        	    return EFalse;

			TPtrC lPtr(DecodeTDesCLC());
			/*Code modified for Forward refrence support in animation*/
			iCurrentElement->SetUriRefDesL (aName, lPtr );
			// For Animation Elements
			if(iIsAnimElement)
				{
				CSvgElementImpl* lElement;
				lElement= (CSvgElementImpl*) iDocument->GetElementById(lPtr);
				if(lElement)
					{
					((CSvgAnimationBase*)iCurrentElement)->SetTarget(lElement);
					}
				else
					{
					 if (!iAnimRefElementArray)
						 {
                         iAnimRefElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
                         }
                     iAnimRefElementArray->AppendL((CSvgElementImpl*)iCurrentElement);
                    }

				CleanupStack::PopAndDestroy( 1 );
				return ETrue;
				}


			//Only for Image Element
			if (iCurrentElement->ElemID() == KSvgImageElement)
				{
                ((CSvgImageElementImpl*)iCurrentElement)->SetUriL(lPtr);
                iImageElements.Append((CSvgImageElementImpl*)iCurrentElement);
				}
			CleanupStack::PopAndDestroy( 1 );

			// setting reference element of use Element
			if (iCurrentElement->ElemID() == KSvgUseElement)
				{
			    if( ((CSvgUseElementImpl*)iCurrentElement)->SetRefElemById
			             (DecodeTDesCLC()) != KErrNone)
			        {
					// if it is null reallocate the memeory again.
					 if (!iUseElementArray)
						 {
                         iUseElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
                         }
                     iUseElementArray->AppendL((CSvgElementImpl*)iCurrentElement);
                    }
                CleanupStack::PopAndDestroy( 1 );
                return ETrue;
				}
// for future animation element
//		    else if (iCurrentElement->ElemID() == KSvgAnimationElement)
//				{
//			    if( ((CSvgAnimationElementImpl*)iCurrentElement)->SetRefElemById
//			             (DecodeTDesCLC()) == KErrNone)
//			        {
					// if it is null reallocate the memeory again.
//					 if (!iAnimationElementArray)
//						 {
//     					 iAnimationElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
//						 }
//			         User::LeaveIfError(iAnimationElementArray->Append((CSvgElementImpl*)iCurrentElement));
//			        }
//				CleanupStack::PopAndDestroy( 1 );
//				return ETrue;
//				}

			// setting Path of Mpath element
			if (iCurrentElement->ElemID() == KSvgMpathElement)
				{
				iCurrentElement->iReqAttrFlag =0;
				TInt8 lCheck;
				iStream >> lCheck;
				//  Flag is used to check the validity of reference element
				if (lCheck)
					{
					((CSvgMpathElementImpl*)iCurrentElement)->SetAttributePathByIdL(DecodeTDesCLC());
					CleanupStack::PopAndDestroy( 1 );
					}
				}

			return ETrue;
			}

		else if (aName== KAtrXlinkactuate ||
			     aName== KAtrXlinkarcrole ||
				 aName== KAtrXlinkrole ||
				 aName== KAtrXlinktitle ||
				 aName== KAtrXlinktype ||
				 aName== KAtrXlinkshow
			    )
			{
			iCurrentElement->SetUriRefDesL (aName, DecodeTDesCLC() );
			CleanupStack::PopAndDestroy( 1 );
			return ETrue;
			}

		else
			{
			return EFalse;
			}
		}

	else if (aName== KXlinkhrefImageEmbedded)
		{
		TPtrC lPtr(DecodeImageEmbeddedTDesCLC());

		iCurrentElement->SetUriRefDesL (aName, lPtr );

			//Only for Image Element
		if (iCurrentElement->ElemID() == KSvgImageElement)
            {
            // Keep track of embedded images
            iEmbeddedImagesCount++;
            ((CSvgImageElementImpl*)iCurrentElement)->SetUriL(lPtr);
            ((CSvgImageElementImpl*)iCurrentElement)->LoadUriL();
            }
        CleanupStack::PopAndDestroy( 2 );
        return ETrue;
        }
    else
		{
		return EFalse;
		}
	}


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeDiscardAttributeL (const TUint16& aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeDiscardAttributeL (const TUint16& aName)
	{
	if ( iCurrentElement->ElemID() != KSvgDiscardElement )
	    return EFalse;

	TInt8 lCheck;
	TInt16 lTemp;

	if (aName== KAtrBegin)
	    {
		// Flag is used to check for SyncValue flag.
		iStream >> lCheck;
		if ( lCheck )
			{
            ((CSvgDiscardElementImpl*)iCurrentElement)->SetSyncValueDefined(ETrue);
			}
        else
            {
			((CSvgDiscardElementImpl*)iCurrentElement)->SetSyncValueDefined(EFalse);
            }

		// Flag is used to check for EventValue flag.
		iStream >> lCheck;
		if ( lCheck )
			{
            ((CSvgDiscardElementImpl*)iCurrentElement)->SetEventValueDefined(ETrue);
            }
        else
            {
            ((CSvgDiscardElementImpl*)iCurrentElement)->SetEventValueDefined(EFalse);
            }


		// Flag is used to check for SyncElement's id.
		iStream >> lCheck;
		if ( lCheck )
			{
    		TPtrC lPtr(DecodeTDesCLC());
    		((CSvgDiscardElementImpl*)iCurrentElement)->SetBeginSyncElementId(lPtr);
	        CleanupStack::PopAndDestroy( 1 );
            }


		iStream >> lTemp;
		((CSvgDiscardElementImpl*)iCurrentElement)->SetAbsoluteBeginTime(lTemp);

		iStream >> lTemp;
		((CSvgDiscardElementImpl*)iCurrentElement)->SetRefBeginTime(lTemp);

		iStream >> lTemp;
		((CSvgDiscardElementImpl*)iCurrentElement)->SetKeyValue(lTemp);

		iStream >> lTemp;
		((CSvgDiscardElementImpl*)iCurrentElement)->SetBeginReferenceEvent((TSvgEvent)lTemp);

	    }
	else
	if (aName== KAtrXlinkhref)
        {
		// Flag is used to check for HrefValue flag.
		iStream >> lCheck;
		if ( lCheck )
			{
            ((CSvgDiscardElementImpl*)iCurrentElement)->SetHrefValueDefined(ETrue);
    		TPtrC lPtr(DecodeTDesCLC());
    		((CSvgDiscardElementImpl*)iCurrentElement)->SetTargetId(lPtr);
            CleanupStack::PopAndDestroy( 1 );
            }
        else
            ((CSvgDiscardElementImpl*)iCurrentElement)->SetHrefValueDefined(EFalse);

        }

    return ETrue;
    }



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeAttributeFloatL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeAttributeFloatL(const TUint16 aName)
	{
	if (iIsFixPt)
	{
		TFloatFixPt lTemp;
		lTemp.iValue = iStream.ReadInt32L();
		iCurrentElement->SetAttributeFloatL( (TInt)aName, lTemp );
	}
	else
	{
		TReal32 lTemp;
		iStream >> lTemp;
		iCurrentElement->SetAttributeFloatL( (TInt)aName, (TFloatFixPt)lTemp );
	}
	
	return ETrue;
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeAttributeIntL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeAttributeIntL(const TUint16 aName)
	{
	TInt8 lTemp;
	iStream >> lTemp;
	iCurrentElement->SetAttributeIntL( (TInt)aName, (TInt32)lTemp );
	return ETrue;
	}


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeAttributePathL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeAttributePathL(const TUint16 aName)
	{
	CGfxGeneralPath* lPath;
	DecodeAnimationPathL(lPath);

	iCurrentElement->SetAttributePathRef( (TInt)aName, lPath);

	return  ETrue;
	}


// --------------------------------------------------------------------------
// void CSvgDecoder::DecodeAnimationPathL(CGfxGeneralPath*& aPath)
// ---------------------------------------------------------------------------
void CSvgDecoder::DecodeAnimationPathL(CGfxGeneralPath*& aPath)
    {

    aPath = CGfxGeneralPath::NewLC();

    {
        const TInt KLargePathTypeCount = 4*256;

        TUint16 typeCount = 0;
        iStream >> typeCount;

        /******************************************************************/
        // Path Type
        /******************************************************************/
        RArray<TUint32>* pathTypeArray = NULL;
        // Put path-type-array to path to handle cleanup
        if ( typeCount == 0 )
            {
            pathTypeArray = new (ELeave)RArray<TUint32>( 1 );
            aPath->SetPointTypeArrayRef( pathTypeArray );
            }
        else
            {
            pathTypeArray = new (ELeave)RArray<TUint32>( typeCount );
            aPath->SetPointTypeArrayRef( pathTypeArray );
            }

        // Occurs only for very large paths
        if ( typeCount > KLargePathTypeCount )
            {
            TUint8* byteData = new (ELeave) TUint8[typeCount];
            CleanupArrayDeletePushL( byteData );

            iStream.ReadL( byteData, typeCount );
            
            for ( TInt i = 0; i < typeCount; i++ )
                {
            
                if(byteData[i] == EGfxEncodedSegMoveTo)
                {
                	byteData[i] = EGfxSegMoveTo;
                }
                else if(byteData[i] == EGfxEncodedSegLineTo )
                {
                	byteData[i]  = EGfxSegLineTo;
                }
                else if(byteData[i] == EGfxEncodedSegQuadTo)
                {
                	byteData[i]  = EGfxSegQuadTo;
                }
                else if(byteData[i] == EGfxEncodedSegCubicTo)
                {
                	byteData[i]  = EGfxSegCubicTo;
                }
                else if(byteData[i] == EGfxEncodedSegClose)
                {
                	byteData[i]  = EGfxSegClose;
                }
                // Path will close RArray if Leave occurs
                pathTypeArray->AppendL( byteData[i] );
                }
                //Transfering ownership to Path
            aPath->PathSegmentTypeArray(byteData);
            aPath->Count(typeCount);
            CleanupStack::Pop( 1 );
            //delete [] byteData;
            }
        else
            {
            TUint8 *byteData = new (ELeave) TUint8[KLargePathTypeCount];
            CleanupArrayDeletePushL( byteData );
            iStream.ReadL( byteData, typeCount );
            for ( TInt i = 0; i < typeCount; i++ )
                {
            
                if(byteData[i] == EGfxEncodedSegMoveTo)
                {
                	byteData[i] = EGfxSegMoveTo;
                }
                else if(byteData[i] == EGfxEncodedSegLineTo )
                {
                	byteData[i]  = EGfxSegLineTo;
                }
                else if(byteData[i] == EGfxEncodedSegQuadTo)
                {
                	byteData[i]  = EGfxSegQuadTo;
                }
                else if(byteData[i] == EGfxEncodedSegCubicTo)
                {
                	byteData[i]  = EGfxSegCubicTo;
                }
                else if(byteData[i] == EGfxEncodedSegClose)
                {
                	byteData[i]  = EGfxSegClose;
                }
                // Path will close RArray if Leave occurs
                pathTypeArray->AppendL( byteData[i] );
                }
            aPath->PathSegmentTypeArray(byteData);
            aPath->Count(typeCount);
             CleanupStack::Pop( 1 );
            }
    }

    /******************************************************************/
    // Path Points
    /******************************************************************/
    {
        const TInt KLargePathPointsCount = 256;

        TUint16 valueCount = 0;
        iStream >> valueCount;

        RArray<TFloatFixPt>* pathPointsArray = NULL;

        if ( valueCount == 0 )
            {
            pathPointsArray = new (ELeave)RArray<TFloatFixPt>( 1 );
            aPath->SetPointCoordsArrayRef( pathPointsArray );
            }
        else
            {
            pathPointsArray = new (ELeave)RArray<TFloatFixPt>( valueCount );
            aPath->SetPointCoordsArrayRef( pathPointsArray );
            }

        if ( valueCount > KLargePathPointsCount )
            {
            if (iIsFixPt)
	            {
	            TFloatFixPt* fixedData = new (ELeave) TFloatFixPt[valueCount];
                CleanupArrayDeletePushL( fixedData );

	            TInt byteCount = sizeof( TFloatFixPt ) * valueCount;
	            iStream.ReadL( (TUint8*)fixedData, byteCount );

	            for ( TInt i = 0; i < valueCount; i++ )
	                {
	                // Path will close RArray if Leave occurs
	                pathPointsArray->AppendL( fixedData[i] );
	                }

	            CleanupStack::Pop( 1 ); // fixedData 
	            delete [] fixedData;         	
            	}
            else
	            {
	            TReal32* real32Data = new (ELeave) TReal32[valueCount];
                CleanupArrayDeletePushL( real32Data );

	            TInt byteCount = sizeof( TReal32 ) * valueCount;
	            iStream.ReadL( (TUint8*)real32Data, byteCount );

	            for ( TInt i = 0; i < valueCount; i++ )
	                {
	                // Path will close RArray if Leave occurs
	                pathPointsArray->AppendL( (TFloatFixPt)real32Data[i] );
	                }

	            CleanupStack::Pop( 1 ); // real32Data    
	            delete [] real32Data;       	
	            }
            }
         else
            {
            if (iIsFixPt)
            	{
            	TFloatFixPt fixedData[KLargePathPointsCount];
            	TInt byteCount = sizeof( TFloatFixPt ) * valueCount;
            	iStream.ReadL( (TUint8*)fixedData, byteCount );

            	for ( TInt i = 0; i < valueCount; i++ )
                	{
               	 	pathPointsArray->AppendL( fixedData[i] );
                	}            	
            	}
            else
            	{
            	TReal32 real32Data[KLargePathPointsCount];
            	TInt byteCount = sizeof( TReal32 ) * valueCount;
            	iStream.ReadL( (TUint8*)real32Data, byteCount );

            	for ( TInt i = 0; i < valueCount; i++ )
                	{
                	pathPointsArray->AppendL( (TFloatFixPt)real32Data[i] );
                	}            	
            	}
            }

    }

    CleanupStack::Pop( 1 ); // aPath
    }


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeAttributeDesL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeAttributeDesL(const TUint16 aName)
	{
	iCurrentElement->SetAttributeDesL ((TInt)aName, DecodeTDesCLC());
	CleanupStack::PopAndDestroy( 1 );
	return ETrue;
	}

// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeStringCssValueL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeStringCssValueL(const TUint16 aName)
	{

	CCssValue* tParentValue = NULL;
    CCssValue* tValue = NULL;

	iCurrentElement->FindProperty ((TInt8)aName,tValue);

	TPtrC lPtr(DecodeTDesCLC());

	if(iCurrentElement->ElemID() == KSvgFontfaceElement)
		{
		((CSvgFontFaceElementImpl *)iCurrentElement)->SetFontFamilyL(lPtr);
		}

	if(iCurrentElement!= iRootElement)
		{
		iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
		}

	if (tParentValue == tValue || tValue == NULL)
		{

		if (iDocument)
		{
			tValue = iDocument->iMemoryManager->GetCssStrObjectL( lPtr );
		}



		CleanupStack::PopAndDestroy( 1 );// cleanup : for lPtr
	    iCurrentElement->SetPresentationAttribute ((TInt8)aName,tValue);
		}
	else
		{
		((CStrCssValueImpl*)tValue)->SetValueL ( lPtr );
		 CleanupStack::PopAndDestroy( 1 );
		}

	return ETrue;
	}



// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeIntCssValueL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeIntCssValueL(const TUint16 aName)
	{

	TInt32  lInt32;
	iStream >> lInt32;

	CCssValue* tParentValue = NULL;
    CCssValue* tValue = NULL;

	iCurrentElement->FindProperty ((TInt8)aName,tValue);

	if(iCurrentElement!= iRootElement)
		{
		iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
		}

	if (tParentValue == tValue || tValue == NULL)
		{

		if (iDocument)
		{
			tValue = iDocument->iMemoryManager->GetCssIntObjectL( (TInt)lInt32 );
		}

	    iCurrentElement->SetPresentationAttribute ((TInt8)aName,tValue);
		}
	else
		{
		((CIntCssValueImpl*)tValue)->SetValueL ((TInt)lInt32);
		}

	return ETrue;
	}


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeFloatCssValueL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeFloatCssValueL(const TUint16 aName)
	{

	if (iIsFixPt)
	{
		TFloatFixPt lFixed;
		lFixed.iValue = iStream.ReadInt32L();
		
		CCssValue* tParentValue = NULL;
    	CCssValue* tValue = NULL;

		iCurrentElement->FindProperty ((TInt8)aName,tValue);

		if(iCurrentElement!= iRootElement)
		{
		iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
		}

		if (tParentValue == tValue || tValue == NULL)
		{
		if (iDocument)
		{
			tValue = iDocument->iMemoryManager->GetCssFloatObjectL( lFixed );
		}

	    iCurrentElement->SetPresentationAttribute ((TInt8)aName,tValue);
		}
		else
		{
		((CFloatCssValueImpl*)tValue)->SetValueL ( lFixed );
		}		
	}
	else
	{
		TReal32  lFlt32;
		iStream >> lFlt32;
		
		CCssValue* tParentValue = NULL;
    	CCssValue* tValue = NULL;

		iCurrentElement->FindProperty ((TInt8)aName,tValue);

		if(iCurrentElement!= iRootElement)
		{
		iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
		}

		if (tParentValue == tValue || tValue == NULL)
		{
		if (iDocument)
		{
			tValue = iDocument->iMemoryManager->GetCssFloatObjectL((TFloatFixPt)lFlt32);
		}

	    iCurrentElement->SetPresentationAttribute ((TInt8)aName,tValue);
		}
		else
		{
		((CFloatCssValueImpl*)tValue)->SetValueL ((TFloatFixPt)lFlt32);
		}
	}

	return ETrue;
	}


// --------------------------------------------------------------------------
// TBool CSvgDecoder::DecodeColorCssValueL(const TUint16 aName)
// ---------------------------------------------------------------------------
TBool CSvgDecoder::DecodeColorCssValueL(const TUint16 aName)
	{
	TUint32 lColor;
	iStream >> lColor;

	// Shift from XBGR to XRGB
	if(!iIsRGB)
		{
		lColor = (  lColor & 0xFF000000) |
			 ( (lColor & 0x00FF0000) >> 16 ) |
		     (  lColor & 0x0000FF00       ) |
		     ( (lColor & 0x000000FF) << 16 );
		}
	
	CCssValue* tParentValue = NULL;
    CCssValue* tValue = NULL;

	iCurrentElement->FindProperty ((TInt8)aName,tValue);

	if(iCurrentElement!= iRootElement)
		{
		iCurrentParentElement->FindProperty ((TInt8)aName,tParentValue);
		}

	if (tParentValue == tValue || tValue == NULL)
		{
		if ( iDocument )
		{
			tValue = iDocument->iMemoryManager->GetCssClrObjectL();
		
		CleanupStack::PushL(tValue); //cleanup

		((CClrCssValueImpl*)tValue)->CloneRGBValueL (lColor);
	    iCurrentElement->SetPresentationAttribute ((TInt8)aName,tValue);
	    CleanupStack::Pop(tValue); //cleanup
	    }
		}
	else
		{
		((CClrCssValueImpl*)tValue)->CloneRGBValueL (lColor);
		}

	return ETrue;
	}



// --------------------------------------------------------------------------
// void CSvgDecoder::SetPaintValueL (CCssValue*& aValue)
// ---------------------------------------------------------------------------
void CSvgDecoder::SetPaintValueL (CCssValue*& aValue)
	{
	TUint8 lCheckGradient;
	iStream >> lCheckGradient;

	if( lCheckGradient)
		{
		((CPaintCssValueImpl *)aValue)->SetUrlflag();
		((CPaintCssValueImpl *)aValue)->SetUrlIdL( DecodeTDesCLC() );
		 CleanupStack::PopAndDestroy( 1 );
		}
	else
		{
		TUint32 lColor;
 		iStream >> lColor;
		// Shift from XBGR to ARGB
		if(!iIsRGB)
			{
			lColor = (  lColor & 0xFF000000) |
			     ( (lColor & 0x00FF0000) >> 16 ) |
			     (  lColor & 0x0000FF00       ) |
			     ( (lColor & 0x000000FF) << 16 );
			}
		((CPaintCssValueImpl *)aValue)->CloneRGBValueL (lColor);
		}
	}





// ==========================================================================
// Start Decoding
// ==========================================================================
CSvgElementImpl* CSvgDecoder::StartDecodingL(CSvgDocumentImpl *aDocument,
											CSvgErrorImpl& aError )
	{

	iDocument= aDocument;

	iSvgError = &aError;
    aError.SetErrorCode( ESvgNoError );

	TUint32 lValidBinary;
	iStream >> lValidBinary;

	if (lValidBinary != KBinaryFile && 
		lValidBinary != KBinaryFile2 && 
		lValidBinary != KBinaryFile3 && 
		lValidBinary != KBinaryFile4)
		{
		iSvgError->SetErrorCode ( ESvgbFileNotValid );
        iSvgError->SetDescription( _L( "Not a valid binary file." ) );
        iSvgError->SetIsWarning( EFalse );
        return NULL;
	}
	
	if (lValidBinary == KBinaryFile2)
		{
		iIsFixPt = ETrue;
		iIsRGB = EFalse;
		}
		
	if (lValidBinary == KBinaryFile3)
		{
		iIsFixPt = ETrue;
		iIsRGB = ETrue;
		}
		
	if(lValidBinary == KBinaryFile4)
		{
		iIsFixPt = EFalse;
		iIsRGB = ETrue;
		}

	TUint8 lElemId;
	TUint16 lAttrName;

	MXmlElement* newElement;

	iStream >> lElemId;
	while (lElemId != KEndSvgFile)
		{

		 // checks end of element
		while (lElemId==KEndElemIndex)
			{
			if ( iCurrentParentElement != iRootElement )
				{
				// Depth is decreased, so the current parent should be one level up
				iCurrentParentElement = ( CSvgElementImpl * )
			                       iCurrentParentElement->ParentNode();
				}

			iStream >> lElemId;
			}

		// checks for Cdata
  		if (lElemId == KCDataPresent)
			{
			iCurrentElement= iCurrentParentElement;
			DecodeAttributeL((TUint16)KAtrCdata);
			iStream >> lElemId;
			continue;
			}



		 //checks end of file.
		if (lElemId == KEndSvgFile)
			{
			break;
			}


		newElement  = iDocument->CreateElementL( lElemId );

		if( newElement == NULL)
			{
				User::Leave(KErrCorrupt);
			}

	    iCurrentElement = ( CSvgElementImpl * ) newElement;


		if ( lElemId == KSvgSvgElement && !iRootElement )
			{
	        iRootElement = iCurrentElement;

	      	//CleanupStack::PushL(iRootElement);
	        iDocument->AppendChildL( newElement );
	        //CleanupStack::Pop(iRootElement);

		    }
		// for all other elements
		else
			{
    		 iCurrentParentElement->AppendChildL( newElement );
		    }

		// Setting target element for animations
		if( iCurrentElement->IsAnimatedElement() )
			{
            ((CSvgAnimationBase*)iCurrentElement)->SetTarget( iCurrentParentElement);
            // For media elements do not set iIsAnimElement as 
            // the Xlink:Href  is for external files rather
            // than internal elements
            if (!( iCurrentElement->ElemID() >= KSvgMediaElemsStartIndex &&
             iCurrentElement->ElemID() <= KSvgMediaElemsEndIndex ) )
                {
			    iIsAnimElement= ETrue;                
                }
			}


		iStream >> lAttrName;

		// for attribute list.
		while (lAttrName!= KStartNewElem)
			{
			DecodeAttributeL(lAttrName);
			iStream >> lAttrName;
			}
		if( iCurrentElement->ElemID() == KSvgStopElement)
		   {
		   if( ((CSvgElementImpl*)iCurrentElement->ParentNode()) != NULL )
              {
              if((((((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID()) == KSvgRadialGradientElement) ||
	              ((((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID()) == KSvgLinearGradientElement) ) )
		         {
	    	     CSvgGradientElementImpl *parent = ((CSvgGradientElementImpl *)iCurrentElement->ParentNode());
        
                 if(parent)
		            {
	                // Initialize the offset value to 0 if its still -1.
	                TFloatFixPt lOffsetValue;
	                TFloatFixPt lDefaultOffsetValue(-1);
	                TBuf<6>     lAttributeName;
	                TBuf<1>     lValueBuffer;
	                      
	                lAttributeName.Append(OFFSET);
	                lValueBuffer.Append(ZEROVALUE);
	                       
	                ((CSvgStopElementImpl*)iCurrentElement)->GetOffset( lOffsetValue );
	                      
	                // Offset Value of -1 indicates that Offset Attribute is not declared in
	                // stop element.
	                if( lOffsetValue == lDefaultOffsetValue )
	                   {
	                    // Deliberately calling SetAttributeL in place of SetAttributeFloatL as the latter inturn
	                    // calls UpdateOffsetValues which should be called on any Stop element once it is added to
	                    // to the Stop element array Owned by parent Gradient element.
	                    ((CSvgStopElementImpl*)iCurrentElement)->SetAttributeL( lAttributeName, lValueBuffer );
	                   }
	                   
	                // The function not only adds the element in Stop element array but also
                    // adjusts the offset values of all the previously added elements such that
                    // each gradient offset value is greater than the previous gradient stop's
                    // offset value.It calls UpdateOffsetValues to adjust the values.
	                ((CSvgGradientElementImpl *)parent)->AddStopElementInArray((CSvgStopElementImpl*)iCurrentElement);
		            }
	              }
            }
		}


		if (iCurrentElement->ElemID() == KSvgUseElement)
			{
			((CSvgUseElementImpl *)iCurrentElement)->SetReferenceElementL();
			}
/*		else if (iCurrentElement->ElemID() == KSvgAnimationElement)
			{
			((CSvgAnimationElementImpl *)iCurrentElement)->SetReferenceElementL();
			if(((CSvgAnimationElementImpl *)iCurrentElement)->RecursionVariable())
				{
				iSvgError->SetErrorCode( ESvgDocumentNotValid );
				iSvgError->SetIsWarning( EFalse );
				iSvgError->SetDescription( _L( "Invalid Document \n" ) );
				iSvgError->AppendDescription( _L("Animation element in loop") );
                return iRootElement;
                }
            }
*/
        if (iCurrentElement->HasAnyTests())
        {
            iReqFetAttSysArray->AppendL(iCurrentElement);
        }

        if (iCurrentElement->ElemID() == KSvgSwitchElement)
        {
            iSwitchElementArray->AppendL(iCurrentElement);
        }

		// For DOM reuse
		if( iCurrentElement->IsAnimatedElement() )
            {
            ((CSvgAnimationBase*)iCurrentElement)->SetOriginalValues_DOMReuse() ;
            iAnimationElementArray->AppendL(iCurrentElement);
            }


		if( iCurrentElement->ElemID() >= KSvgLinearGradientElement &&
			iCurrentElement->ElemID() <= KSvgStopElement )
			{
			iCurrentElement->SetAttributeIntL(KCSS_ATTR_DISPLAY,0);
			}

		if (	iCurrentElement->ElemID() == KSvgMpathElement &&
				iCurrentParentElement->ElemID() == KSvgAnimateMotionElement &&
				iSvgError->ErrorCode() == ESvgMissingRequiredAttribute &&
				iSvgError->IsWarning()
		   )


			{
			iSvgError->SetErrorCode( ESvgNoError );
			// add to event receiver list will have only one copy
			iDocument->AddToEventReceiverListL( iCurrentParentElement, KSvgEventMaskTimer );
			}

		CheckRequiredAttributesL( lElemId);
		iIsAnimElement= EFalse;

		iCurrentParentElement = iCurrentElement;

		iStream >> lElemId;

		}
    //Load Images
    
    TInt lImageElementCnt = iImageElements.Count();
    TInt lTotalImagesCount = lImageElementCnt + iEmbeddedImagesCount;
    iDocument->SetImageElementsCount(lTotalImagesCount);
    for( TInt index = 0 ; index < lImageElementCnt ; index++)
        {
          iImageElements[index]->LoadUriL();
        }
    
	if(iUseElementArray)
		{
	 // support for forward reference in use element
	TInt lCount = iUseElementArray->Count();
	TInt lIndex= 0;
	while(lIndex < lCount)
	{
		TPtrC tPtr2 = ((CSvgElementImpl*)iUseElementArray->operator[](lIndex))->Href();
		TInt Pos1= tPtr2.Locate('#');
        HBufC*  tBufC   = HBufC::NewLC( tPtr2.Length() );
	    TPtr    tPtr3    ( tBufC->Des() );
	    tPtr3.Copy( tPtr2 );
	    tPtr3.TrimAll();
	    // remove '#'
		if(Pos1 != KErrNotFound)
			{
			tPtr3.Delete(Pos1,1);
			}
		if(((CSvgUseElementImpl *)iUseElementArray->operator[](lIndex))->SetRefElemById(tPtr3) != KErrNotFound)
			{
			((CSvgUseElementImpl *)iUseElementArray->operator[](lIndex))->SetReferenceElementL();

			}
			lIndex++;
		CleanupStack::PopAndDestroy( 1 );
	}
		}
	// support for forward reference in use element
	if(iAnimRefElementArray)
		{
		TInt lCount = iAnimRefElementArray->Count();
		TInt lIndex= 0;
	while(lIndex < lCount)
		{
		TPtrC tPtr2 = ((CSvgElementImpl*)iAnimRefElementArray->operator[](lIndex))->Href();
		((CSvgAnimationBase *)iAnimRefElementArray->operator[](lIndex))->SetRefElemById(tPtr2);
		lIndex++;
			}
		}

	// For Animation elements
	if (iAnimationElementArray)
		{
		TInt lCount = iAnimationElementArray->Count();
		TInt lIndex = 0;
		while(lIndex < lCount)
	    	{
		    ((CSvgAnimationBase *)iAnimationElementArray->operator[](lIndex))->CheckBeginTime();
		    lIndex ++;
		    }
		}
    // Reset iCurrentElement, as it is no longer used
    iCurrentElement = NULL;
	RemoveFalseElements();
	RemoveFalseSwitchCases();

	return iRootElement;
	}



// --------------------------------------------------------------------------
// void CSvgDecoder::CheckRequiredAttributesL(const TUint8 aName )
// ---------------------------------------------------------------------------
void CSvgDecoder::CheckRequiredAttributesL(const TUint8 aName )
	{


	if( iCurrentElement->iReqAttrFlag != 0)

			{

			iSvgError->SetErrorCode( ESvgMissingRequiredAttribute );
			iSvgError->SetIsWarning( ETrue );
			iSvgError->SetDescription( _L( "Missing required attribute \"" ) );
			switch(iCurrentElement->iReqAttrFlag)
			{
			case KSVG_PATH_ELEMFLAG:
				iSvgError->AppendDescription( _L("d") );
				break;
			case KAtrWidth:
				iSvgError->AppendDescription( _L("width") );
				break;
			case KAtrHeight:
				iSvgError->AppendDescription( _L("height") );
				break;
			case KSVG_POLYLINE_ELEMFLAG:
				iSvgError->AppendDescription( _L("points") );
				break;
			case KAtrRy:
				iSvgError->AppendDescription( _L("Ry") );
				break;
			case KAtrRx:
				iSvgError->AppendDescription( _L("Rx") );
				break;
			case KAtrSVGRec:
				iSvgError->AppendDescription( _L("width") );
				iSvgError->AppendDescription( _L( "\" and \"" ) );
				iSvgError->AppendDescription(_L("height"));
				break;
			case KAtrSVGElp:
				iSvgError->AppendDescription( _L("Rx") );
				iSvgError->AppendDescription( _L( "\" and \"" ) );
				iSvgError->AppendDescription(_L("Ry"));
				break;
			case KAtrSVGTrf:
				iSvgError->AppendDescription( _L("attributeName") );
				iSvgError->AppendDescription( _L( "\" and \"" ) );
				iSvgError->AppendDescription(_L("type"));
				break;
			case KSVG_ANIMATE_ELEMFLAG:
				iSvgError->AppendDescription( _L("attributeName") );
				break;
			case KSVG_CIRCLE_ELEMFLAG:
				iSvgError->AppendDescription( _L("r") );
				break;
			case KSVG_HKERN_ELEMFLAG:
				iSvgError->AppendDescription( _L("k") );
				break;
			case KAtrType:
				iSvgError->AppendDescription( _L("type") );
				break;
			case KAtrXlinkhref:
				iSvgError->AppendDescription (_L("Xlink:href"));
				break;
			case KAtrSVGAmo:
				iSvgError->AppendDescription( _L("path") );
				((CSvgDocumentImpl*)iDocument)->RemoveFromEventReceiverList( iCurrentElement );
				break;
			case KAtrToBy:
				iSvgError->AppendDescription( _L("to/by") );
				((CSvgDocumentImpl*)iDocument)->RemoveFromEventReceiverList( iCurrentElement );
				break;
			}
			iSvgError->AppendDescription( _L( "\" for <" ) );
			// access schema data to get the name of the attribute which is missing
			// currently the error msg doesnt not report the name of the attribute

			TBuf<20> lElemName = _L("svg");
			((CSvgDocumentImpl*)iDocument)->SchemaData()->GetSVGElementName(aName,lElemName);
			iSvgError->AppendDescription( lElemName );
			iSvgError->AppendDescription( _L( ">." ) );
			// turn off element

			  ((CSvgElementImpl*)iCurrentElement)->SetTurnOff( ETrue );

			((CSvgElementImpl*)iCurrentElement)->SetPropertyL(KCSS_ATTR_DISPLAY,_L("none"));


		}
	}

//---------------------------------------------------
//Removal of elements that dont pass
//required extensions, required features, and system language
//requirements
//switch statement is special case were only the first one
//that passes these requirements will be used so toss the rest
//---------------------------------------------------
void CSvgDecoder::RemoveFalseElements()
    {
    if (iReqFetAttSysArray == NULL)
        {
        // Error Case
        return;        
        }

    //loop through the list of elements with required features, attributes, or sys language
    TInt reqFetSysArrayCnt = iReqFetAttSysArray->Count();
    while ( reqFetSysArrayCnt > 0 )
        {
        CSvgElementImpl* lCurElement = ( CSvgElementImpl* )
            iReqFetAttSysArray->operator[]( reqFetSysArrayCnt - 1 );

        if ( lCurElement != NULL )
            {
            CSvgElementImpl* lCurParent = ( CSvgElementImpl* )
                lCurElement->ParentNode();
 
            //just a normal element check it and remove it if it doesnt pass
            TBool lResult = EFalse;
            TRAPD( error, lResult  = VerifyReqExReqFtrSysL( lCurElement ) );
            if ( error == KErrNone && !lResult )
                {
                // Remove internal references to the element about to be 
                // removed
                // This function would also remove the lCurElement from
                // iReqFetAttSysArray.
                RemoveInternalReferences( lCurElement );
                //element doesnt have proper required extension, attributes, or system language
                lCurParent->RemoveChild( lCurElement );
                delete (CXmlElementImpl*)lCurElement;
                lCurElement = NULL;
                }
            else
                {
                // Remove the last element from the array as it is processed
                iReqFetAttSysArray->Remove( reqFetSysArrayCnt - 1 );
                } // if ( error == KErrNone && !lResult )
            }
        else
            {
            // Remove the last element from the array as it is processed
            iReqFetAttSysArray->Remove( reqFetSysArrayCnt - 1 );
            } // if ( lCurElement != NULL )
        // Update the count of elements in the array
        reqFetSysArrayCnt = iReqFetAttSysArray->Count();
        } // while ( reqFetSysArrayCnt > 0 )
    // Clear the array as it is no longer required.
    iReqFetAttSysArray->Reset();    
    }

// --------------------------------------------------------------------------
// void CSvgDecoder::RemoveFalseSwitchCases()
// ---------------------------------------------------------------------------
void CSvgDecoder::RemoveFalseSwitchCases()
    {
    if (iSwitchElementArray == NULL)
        {
        // Error Case
        return;
        }
        
    TInt switchEleArrayCnt = iSwitchElementArray->Count();
    while ( switchEleArrayCnt > 0 )
        {
        TBool foundTrue = EFalse;
        CSvgElementImpl* curSwitchElem = ( CSvgElementImpl* )
            iSwitchElementArray->operator[]( switchEleArrayCnt - 1 );

        if ( curSwitchElem != NULL )
            {
            //get the first child...which is where the first
            CSvgElementImpl* curCaseElem = (CSvgElementImpl*)curSwitchElem->FirstChild();

            while ( curCaseElem != NULL )
                {
                CSvgElementImpl* nextCaseElem = (CSvgElementImpl*)curCaseElem->NextSibling();
                // foundTrue is set to TRUE when an element whose test passes is found. The
                // subsequent elements are to be removed.
                if ( foundTrue )
                    {
                    // Remove internal references of the element from 
                    // decoder's lists
                    RemoveInternalReferences( curCaseElem );
                    //already found the true case in the switch delete the rest
                    curSwitchElem->RemoveChild( curCaseElem );
                    delete ( CXmlElementImpl* )curCaseElem;
                    curCaseElem = NULL;
                    }
                else
                    {
                    TBool lResult = EFalse;
                    TRAPD(error, lResult  = VerifyReqExReqFtrSysL(curCaseElem));
                    if ( error == KErrNone && !lResult )
                        {
                        // Remove internal references of the element from 
                        // decoder's lists
                        RemoveInternalReferences( curCaseElem );
                        //this element doesnt meet the switch requirements delete it and its children
                        curSwitchElem->RemoveChild(curCaseElem);
                        delete (CXmlElementImpl*)curCaseElem;
                        curCaseElem = NULL;
                        }
                    else
                        {
                        //one evaluated to true so keep it but go ahead and delete the rest in the switch
                        //should only be one child for switch in end
                        foundTrue = ETrue;
                        } // if ( error == KErrNone && !lResult )
                    } // if ( foundTrue )
                // Proceed checking the next sibling
                curCaseElem = nextCaseElem;
                } // while ( curCaseElem != NULL )
            } // if ( curSwitchElem != NULL )
        // Remove the last switch element which was processed
        iSwitchElementArray->Remove( switchEleArrayCnt - 1 );
        // Update the count of the array
        switchEleArrayCnt = iSwitchElementArray->Count();
        } // while ( switchEleArrayCnt > 0 )
    // Clear the array as it is no longer needed
    iSwitchElementArray->Reset();    
    }

//---------------------------------------------------
//Check to see
//required extensions, required features, and system language
//requirements
//---------------------------------------------------
TBool CSvgDecoder::VerifyReqExReqFtrSysL( CSvgElementImpl* aElement )
{
    const CDesCArrayFlat* tempTestArray = NULL; // 'const' keyword added due to S60 (CW) build team recommendation

	TBool   doDraw  = ETrue;

    // First check for requiredExtensions
    aElement->GetRequiredExtensions( tempTestArray );

    if ( tempTestArray && tempTestArray->MdcaCount() )
    {
    	// Check for all entries in requiredExtensions

    	TInt lCount = tempTestArray->MdcaCount();

      	for ( TInt i = 0; i < lCount; i++ )
      	{
      		TPtrC tmpPtr = tempTestArray->MdcaPoint( i );
       	 	if ( tmpPtr.Length() )
       	 	{
        		doDraw = EFalse;
          		break;
        	}
        	else if ( aElement->HasExtension( tmpPtr ) == EFalse )
        	{
        		doDraw = EFalse;
          		break;
        	}
      	}
    }

    // Second, check for requiredFeatures
    aElement->GetRequiredFeatures( tempTestArray );
    if ( tempTestArray && tempTestArray->MdcaCount() && doDraw)
    {
    	// Check for all entries in requiredFeatures
      TInt lCount = tempTestArray->MdcaCount();
      for ( TInt i = 0; i < lCount; i++ )
      {
      	TPtrC tmpPtr = tempTestArray->MdcaPoint( i );
        if ( aElement->HasFeature( tmpPtr ) == EFalse )
        {
        	doDraw = EFalse;
          	break;
        }
      }
    }

	TBufC<5> iSystemLanguage;
	SystemLanguage( iSystemLanguage.Des() );

	// Third, check for systemLanguage
	// Future enhancement: System language doesnt need to be stored in seperate array indexes
	// could have just stored it as one big string and use findf without the loop
    aElement->GetSystemLanguage( tempTestArray );
    TBool doDrawSystem = EFalse;
    if ( tempTestArray && tempTestArray->MdcaCount() && doDraw)
	{
    	TInt lCount = tempTestArray->MdcaCount();

      	for ( TInt i = 0; i < lCount; i++ )
      	{
      		TPtrC tmpPtr = tempTestArray->MdcaPoint( i );

        	if ( tmpPtr.FindF( iSystemLanguage ) >= 0 )
			{
        		doDrawSystem = ETrue;
          		break;
			}
      	}

      	if (doDrawSystem == EFalse)
     	{
     		doDraw = EFalse;
     	}
     }

	return doDraw;
}

// --------------------------------------------------------------------------
// void CSvgDecoder::SystemLanguage( TPtr aValue )
// ---------------------------------------------------------------------------
void CSvgDecoder::SystemLanguage( TPtr aValue )
    {
    _LIT( KEn, "en" );
    _LIT( KFr, "fr" );
    _LIT( KDe, "de" );
    _LIT( KEs, "es" );

    _LIT( KAf, "af" );
    _LIT( KAm, "am" );
    _LIT( KAr, "ar" );
    _LIT( KBg, "bg" );
    _LIT( KBn, "bn" );
    _LIT( KBo, "bo" );
    _LIT( KCa, "ca" );
    _LIT( KCs, "cs" );
    _LIT( KCy, "cy" );
    _LIT( KDa, "da" );
    _LIT( KEl, "el" );
    _LIT( KEt, "et" );
    _LIT( KFa, "fa" );
    _LIT( KFi, "fi" );
    _LIT( KGa, "ga" );
    _LIT( KGd, "gd" );
    _LIT( KGu, "gu" );
    _LIT( KHe, "he" );
    _LIT( KHi, "hi" );
    _LIT( KHu, "hu" );
    _LIT( KHr, "hr" );
    _LIT( KHy, "hy" );
    _LIT( KId, "id" );
    _LIT( KIs, "is" );
    _LIT( KIt, "it" );
    _LIT( KJa, "ja" );
    _LIT( KKa, "ka" );
    _LIT( KKk, "kk" );
    _LIT( KKm, "km" );
    _LIT( KKn, "kn" );
    _LIT( KKo, "ko" );
    _LIT( KLo, "lo" );
    _LIT( KLt, "lt" );
    _LIT( KLv, "lv" );
    _LIT( KMk, "mk" );
    _LIT( KMl, "ml" );
    _LIT( KMn, "mn" );
    _LIT( KMo, "mo" );
    _LIT( KMr, "mr" );
    _LIT( KMs, "ms" );
    _LIT( KMy, "my" );
    _LIT( KNo, "no" );
    _LIT( KNl, "nl" );
    _LIT( KPa, "pa" );
    _LIT( KPl, "pl" );
    _LIT( KPt, "pt" );
    _LIT( KRo, "ro" );
    _LIT( KRu, "ru" );
    _LIT( KSi, "si" );
    _LIT( KSk, "sk" );
    _LIT( KSl, "sl" );
    _LIT( KSo, "so" );
    _LIT( KSr, "sr" );
    _LIT( KSq, "sq" );
    _LIT( KSv, "sv" );
    _LIT( KSw, "sw" );
    _LIT( KTa, "ta" );
    _LIT( KTe, "te" );
    _LIT( KTh, "th" );
    _LIT( KTi, "ti" );
    _LIT( KTk, "tk" );
    _LIT( KTl, "tl" );
    _LIT( KTr, "tr" );
    _LIT( KUk, "uk" );
    _LIT( KUr, "ur" );
    _LIT( KVi, "vi" );
    //_LIT( KZh, "zh" );
    _LIT( KZu, "zu" );

    _LIT( KEnB, "en-UK");
    _LIT( KEnUS, "en-US");
    _LIT( KZhTW, "zh-TW");
    _LIT( KZhHK, "zh-HK");
    _LIT( KZhCN, "zh-CN");
    _LIT( KFrCA, "fr-CA");
    _LIT( KPtBR, "pt-BR");
    _LIT( KEnTW, "en-TW");
    _LIT( KEnHK, "en-HK");
    _LIT( KEnCN, "en-CN");
    _LIT( KEnJP, "en-JP");
    _LIT( KEnTH, "en-TH");
    _LIT( KEsAR, "es-AR");
    _LIT( KMsAP, "ms-AP");
    _LIT( KEnAP, "en-AP" );    // KLangApacEnglish
    _LIT( KIdAP, "id-AP" );    // KLangApacIndonesian
    _LIT( KEu, "eu" );    // KLangBasque
    _LIT( KGl, "gl" );    // KLangGalician

    _LIT(KDefault, "qqqqq");

    switch ( User::Language() )
        {
        case ELangTest:
        aValue = KEn;
        break;

        case ELangEnglish:
        aValue = KEnB;
        break;
        case ELangAmerican:
        aValue = KEnUS;
        break;
        case ELangAustralian:
        case ELangNewZealand:
        case ELangCanadianEnglish:
        case ELangSouthAfricanEnglish:
        case ELangInternationalEnglish:
        aValue = KEn;
        break;

        case ELangFrench:
        case ELangSwissFrench:
        case ELangBelgianFrench:
        aValue = KFr;
        break;

        case ELangGerman:
        case ELangAustrian:
        case ELangSwissGerman:
        aValue = KDe;
        break;

        case ELangSpanish:
        case ELangInternationalSpanish:
        aValue = KEs;
        break;

        case ELangLatinAmericanSpanish:
        aValue = KEsAR;
        break;

        case ELangItalian:
        case ELangSwissItalian:
        aValue = KIt;
        break;

        case ELangSwedish:
        case ELangFinlandSwedish:
        aValue = KSv;
        break;

        case ELangDanish:
        aValue = KDa;
        break;

        case ELangNorwegian:
        case ELangNorwegianNynorsk:
        aValue = KNo;
        break;

        case ELangFinnish:
        aValue = KFi;
        break;

        case ELangBrazilianPortuguese:
        aValue = KPtBR;
        break;

        case ELangPortuguese:
        aValue = KPt;
        break;

        case ELangTurkish:
        case ELangCyprusTurkish:
        aValue = KTr;
        break;

        case ELangIcelandic:
        aValue = KIs;
        break;

        case ELangRussian:
        case ELangBelarussian:
        aValue = KRu;
        break;

        case ELangHungarian:
        aValue = KHu;
        break;

        case ELangDutch:
        case ELangBelgianFlemish:
        aValue = KNl;
        break;

        case ELangCzech:
        aValue = KCs;
        break;

        case ELangSlovak:
        aValue = KSk;
        break;

        case ELangPolish:
        aValue = KPl;
        break;

        case ELangSlovenian:
        aValue = KSl;
        break;

        case ELangPrcChinese:
        aValue = KZhCN;
        break;
        case ELangTaiwanChinese:
        aValue = KZhTW;
        break;
        case ELangHongKongChinese:
        aValue = KZhHK;
        break;

        case ELangJapanese:
        aValue = KJa;
        break;

        case ELangThai:
        aValue = KTh;
        break;

        case ELangAfrikaans:
        aValue = KAf;
        break;

        case ELangAlbanian:
        aValue = KSq;
        break;

        case ELangAmharic:
        aValue = KAm;
        break;

        case ELangArabic:
        aValue = KAr;
        break;

        case ELangArmenian:
        aValue = KHy;
        break;

        case ELangTagalog:
        aValue = KTl;
        break;

        case ELangBengali:
        aValue = KBn;
        break;

        case ELangBulgarian:
        aValue = KBg;
        break;

        case ELangBurmese:
        aValue = KMy;
        break;

        case ELangCatalan:
        aValue = KCa;
        break;

        case ELangCroatian:
        aValue = KHr;
        break;

        case ELangEstonian:
        aValue = KEt;
        break;

        case ELangFarsi:
        aValue = KFa;
        break;

        case ELangCanadianFrench:
        aValue = KFrCA;
        break;

        case ELangScotsGaelic:
        aValue = KGd;
        break;

        case ELangGeorgian:
        aValue = KKa;
        break;

        case ELangGreek:
        case ELangCyprusGreek:
        aValue = KEl;
        break;

        case ELangGujarati:
        aValue = KGu;
        break;

        case ELangHebrew:
        aValue = KHe;
        break;

        case ELangHindi:
        aValue = KHi;
        break;

        case ELangIndonesian:
        aValue = KId;
        break;

        case ELangIrish:
        aValue = KGa;
        break;

        case ELangKannada :
        aValue = KKn;
        break;


        case ELangKazakh:
        aValue = KKk;
        break;

        case ELangKhmer:
        aValue = KKm;
        break;

        case ELangKorean:
        aValue = KKo;
        break;

        case ELangLao:
        aValue = KLo;
        break;

        case ELangLatvian:
        aValue = KLv;
        break;

        case ELangLithuanian:
        aValue = KLt;
        break;

        case ELangMacedonian:
        aValue = KMk;
        break;

        case ELangMalay:
        aValue = KMs;
        break;

        case ELangMalayalam:
        aValue = KMl;
        break;

        case ELangMarathi:
        aValue = KMr;
        break;

        case ELangMoldavian:
        aValue = KMo;
        break;

        case ELangMongolian:
        aValue = KMn;
        break;

        case ELangPunjabi:
        aValue = KPa;
        break;

        case ELangRomanian:
        aValue = KRo;
        break;

        case ELangSerbian:
        aValue = KSr;
        break;

        case ELangSinhalese:
        aValue = KSi;
        break;

        case ELangSomali:
        aValue = KSo;
        break;

        case ELangSwahili:
        aValue = KSw;
        break;

        case ELangTamil:
        aValue = KTa;
        break;

        case ELangTelugu:
        aValue = KTe;
        break;

        case ELangTibetan:
        aValue = KBo;
        break;

        case ELangTigrinya:
        aValue = KTi;
        break;

        case ELangTurkmen:
        aValue = KTk;
        break;

        case ELangUkrainian:
        aValue = KUk;
        break;

        case ELangUrdu:
        aValue = KUr;
        break;

        case ELangVietnamese:
        aValue = KVi;
        break;

        case ELangWelsh:
        aValue = KCy;
        break;

        case ELangZulu:
        aValue = KZu;
        break;

        // from \\epoc32\\include\\oem\\languages.hrh

        case KLangTaiwanEnglish:
        aValue = KEnTW;
        break;

        case KLangHongKongEnglish:
        aValue = KEnHK;
                break;

        case KLangPrcEnglish:
        aValue = KEnCN;
        break;

        case KLangJapaneseEnglish:
        aValue = KEnJP;
        break;

        case KLangThaiEnglish:
        aValue = KEnTH;
        break;

        case KLangApacMalay:
        aValue = KMsAP;
        break;

        case KLangApacEnglish:
            {
            aValue = KEnAP;
            break;
            }
        case KLangApacIndonesian:
            {
            aValue = KIdAP;
            break;
            }
        case KLangBasque:
            {
            aValue = KEu;
            break;
            }
        case KLangGalician:
            {
            aValue = KGl;
            break;
            }

        // Cingular English
        case 6154:
        aValue = KEnUS;
        break;

        default:
        aValue = KDefault;
        break;
        }
    }

// --------------------------------------------------------------------------
// TInt CSvgDecoder::RemoveInternalReferences( CSvgElementImpl* )
// ---------------------------------------------------------------------------
void CSvgDecoder::RemoveInternalReferences( CSvgElementImpl* aElement )
    {
    // Remove the references of aElement in content handler's lists

    // Use Elements 
    TInt lIndex = 0;
    if ( aElement->ElemID() == KSvgUseElement )
        {
        lIndex = iUseElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iUseElementArray->Remove( lIndex );
            }
        }
    else if ( aElement->ElemID() == KSvgSwitchElement )
        {
        // Switch Elements    
        lIndex = iSwitchElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iSwitchElementArray->Remove( lIndex );
            }
        }
    else if ( aElement->IsAnimatedElement() )
        {
        // Animation elements include animate* elements, set, 
        // "animation",  audio elements etc.
        lIndex = iAnimationElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iAnimationElementArray->Remove( lIndex );
            }        
        lIndex = iAnimRefElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iAnimRefElementArray->Remove( lIndex );
            }
        }
    // Elements with test attributes - requiredFeatures, 
    // requiredExtensions, systemLanguage
    lIndex = iReqFetAttSysArray->Find( aElement );
    if ( lIndex != KErrNotFound )
        {
        iReqFetAttSysArray->Remove( lIndex );
        }

    // Remove Internal references of subtree elements as well
    CSvgElementImpl* lChild = ( CSvgElementImpl* )aElement->FirstChild();
    while ( lChild != NULL )
        {
        RemoveInternalReferences( lChild );
        lChild = ( CSvgElementImpl* )lChild->NextSibling();
        }

    }
