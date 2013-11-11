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

#include <AknBidiTextUtils.h>

#include "SVGTextAreaElementImpl.h"
#include "SVGRectElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGEngineImpl.h"
#include "GfxAffineTransform.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTextAreaElementImpl* CSvgTextAreaElementImpl::NewL(  const TUint8 aElemID,
												CSvgDocumentImpl* aDoc )
{
	CSvgTextAreaElementImpl*self    = new ( ELeave ) CSvgTextAreaElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID);
	CleanupStack::Pop();

	return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTextAreaElementImpl* CSvgTextAreaElementImpl::NewLC(  const TUint8 aElemID,
												 CSvgDocumentImpl* aDoc )
{
	CSvgTextAreaElementImpl*self    = new ( ELeave ) CSvgTextAreaElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTextAreaElementImpl::~CSvgTextAreaElementImpl()
{
    if ( iSvgStyleProperties )
		{
			iSvgStyleProperties->Close();
			delete iSvgStyleProperties;
			iSvgStyleProperties = NULL;
		}

		if ( iInnerRectangle )
		{
			this->RemoveChild(iInnerRectangle);
			delete iInnerRectangle;
			iInnerRectangle = NULL;
		}

		if ( iBorderRectangle )
		{
			this->RemoveChild(iBorderRectangle);
			delete iBorderRectangle;
			iBorderRectangle = NULL;
		}

		if (iTextElementsArray)
		{
			iTextElementsArray->Reset();
			iTextElementsArray->Close();
			delete iTextElementsArray;
			iTextElementsArray = NULL;
		}
	if (iAllText)
	{
		delete iAllText;
		iAllText = NULL;
	}

	((CSvgDocumentImpl*)OwnerDocument())->RemoveInternalMouseListener( this );

}

// *******************************************************
// From SVG DOM
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::X()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->X();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::Y()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->Y();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::Width()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->Width();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::Height()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->Height();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::Rx()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->Rx();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgTextAreaElementImpl::Ry()
{
	if (!iInitialized)
	{
		return 0;
	}

    return iBorderRectangle->Ry();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::IsEditable()
{
    return iEditable;
}


// ---------------------------------------------------------------------------
// For JSR support
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::SetTextAreaDimensions(TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight)
{
	if (!iInitialized)
	{
		TRAPD(err, InitializeL());
    	if (err)
    	    {
		    #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgTextAreaElementImpl::SetTextAreaDimensions Error in InitializeL()" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
    	    }
	}

	TRAPD(err, iBorderRectangle->SetAttributeFloatL(KAtrX, aX));
	if (err)
	    {
	    #ifdef _DEBUG
        _LIT( KErrMsg, "CSvgTextAreaElementImpl::SetTextAreaDimensions Error in SetAttributeFloatL" );
        RDebug::Print(KErrMsg);
        #endif //_DEBUG
	    }

	TRAP(err, iBorderRectangle->SetAttributeFloatL(KAtrY, aY));
	if (err)
	    {
	    #ifdef _DEBUG
        _LIT( KErrMsg, "CSvgTextAreaElementImpl::SetTextAreaDimensions Error in SetAttributeFloatL" );
        RDebug::Print(KErrMsg);
        #endif //_DEBUG
	    }

	TRAP(err, iBorderRectangle->SetAttributeFloatL(KAtrWidth, aWidth));
	if (err)
	    {
	    #ifdef _DEBUG
        _LIT( KErrMsg, "CSvgTextAreaElementImpl::SetTextAreaDimensions Error in SetAttributeFloatL" );
        RDebug::Print(KErrMsg);
        #endif //_DEBUG
	    }

	TRAP(err, iBorderRectangle->SetAttributeFloatL(KAtrHeight, aHeight));
	if (err)
	    {
	    #ifdef _DEBUG
        _LIT( KErrMsg, "CSvgTextAreaElementImpl::SetTextAreaDimensions Error in SetAttributeFloatL" );
        RDebug::Print(KErrMsg);
        #endif //_DEBUG
	    }
}

// From MXmlElementOpt
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
{
	if (!iInitialized)
	{
		TRAPD(err, InitializeL());
    	if (err)
    	    {
		    #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgTextAreaElementImpl::GetAttributeFloat Error in InitializeL()" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
    	    }
	}

    switch ( aNameId )
        {
        case KAtrX:
        case KAtrRefX:
        		aValue = X();
        break;
        case KAtrY:
        case KAtrRefY:
        		aValue = Y();
        break;
        case KAtrWidth:
        		aValue = Width();
        break;
        case KAtrHeight:
        		aValue = Height();
        break;
		// added as a part of get and Set api updation
		case KAtrRx:
				aValue = Rx();
		break;
		case KAtrRy:
				aValue = Ry();
		break;
        default:
        		return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
{
	if (!iInitialized)
	{
		InitializeL();
	}

    switch ( aNameId )
        {
        case KAtrX:
			iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			iInnerRectangle->SetAttributeFloatL(aNameId, aValue+TFloatFixPt(2));
		break;
        case KAtrY:
			iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			iInnerRectangle->SetAttributeFloatL(aNameId, aValue+TFloatFixPt(2));
		break;
		case KAtrRx:
			iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			//iInnerRectangle->SetAttributeFloatL(aNameId, aValue+TFloatFixPt(2));
		break;
		case KAtrRy:
			iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			//iInnerRectangle->SetAttributeFloatL(aNameId, aValue+TFloatFixPt(2));
		break;
        case KAtrWidth:
			if(iReqAttrFlag == KAtrSVGRec)
			{
				iReqAttrFlag = KAtrHeight;
			}
			else
			{
				iReqAttrFlag = 0;
			}

			iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			iInnerRectangle->SetAttributeFloatL(aNameId, aValue-TFloatFixPt(4));
		break;

        case KAtrHeight:
					if(iReqAttrFlag == KAtrSVGRec)
						{
						iReqAttrFlag = KAtrWidth;
						}
					else
						{
						iReqAttrFlag = 0;
						}
					iBorderRectangle->SetAttributeFloatL(aNameId, aValue);
			iInnerRectangle->SetAttributeFloatL(aNameId, aValue-TFloatFixPt(4));
			        break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
}


// *******************************************************
// From MXmlElement
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
{
	if (!iInitialized)
	{
		InitializeL();
	}

		CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    if (!iInitialized)
	{
		return KErrNone;
	}

    switch ( aNameId )
        {
        case KAtrCdata:
        aValue.Set(*iAllText);
        break;
        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCdata:
        SetTextL( aValue );
        break;

        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::SetEditable( const TDesC& aValue )
{
	if (aValue.FindF(_L("simple")) == KErrNotFound && aValue.FindF(_L("true")) == KErrNotFound)
	{
		iEditable = EFalse;
	}
	else
	{
		iEditable = ETrue;
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::LocalizedArrangeTextL( CGfx2dGc* /* aGc */ )
    {
	if (!iInitialized)
	    {
		return;
	    }

	//every time we call arrange text we will clear the previous text elements
	//very expensive but easiest way to do this
	if (iTextElementsArray)
	    {
		TInt textEleArrayCnt = iTextElementsArray->Count();
		for (TInt i=0; i < textEleArrayCnt; i++)
		    {
			this->RemoveChild( iTextElementsArray->operator[](i) );
			delete iTextElementsArray->operator[](i);
		    }

		iTextElementsArray->Reset();
	    }
    // If there is some text in the text area, we need to do the following operations
    //      a. calculate number of lines that can fit in the inner rectangle of textArea 
    //          i. For this we need to get the dimension of Inner Rectangle
    //          ii. Then get the font corresponding to the current scale. This is obtained
    //              by creating a dummy text element and updating its CTM.
    //
	if (iAllText && iAllText->Length() > 0)
	    {
		TFloatFixPt lInnerRectWidthF, lInnerRectHeightF;
		iInnerRectangle->GetAttributeFloat( KAtrWidth, lInnerRectWidthF );
		iInnerRectangle->GetAttributeFloat( KAtrHeight, lInnerRectHeightF );
        
        // Create a dummy text element to obtain the font
      	CSvgTextElementImpl* lTextElement  = 
      	    (CSvgTextElementImpl*)iOwnerDocument->CreateElementL(KSvgTextElement);
      	CleanupStack::PushL(lTextElement);
        lTextElement->SetAttributeDesL(KAtrCdata,iAllText->Des());
        
        this->AppendChildL(lTextElement);
        CleanupStack::Pop(lTextElement);
        lTextElement->UpdateCTM();
        // Update the text element's font corresponding to the CTM.
        lTextElement->UpdateCurrentScaledFont();

        TFloatFixPt lScale = GetCurrentScale();
        
        //Taken the widest charactor to check against the dimensions of the text area.
        //There is no point in going futher, If the area of text area elelment 
        //is not big enough to fit the single char,
        TChar ch('W');
        if ( lScale == TFloatFixPt( 0 ) ||
             ((TReal32)( lInnerRectWidthF * lScale)) < lTextElement->Font()->CharWidthInPixels( ch ) ||
             ((TReal32)( lInnerRectHeightF * lScale)) < lTextElement->Font()->FontMaxHeight())
            {
            // Scale = 0 , means no drawing will take place.
            // Cleanup the text element
    		this->RemoveChild(lTextElement);
			delete lTextElement;
			lTextElement = NULL;
            return;
            }
        
        // Store the current font's ascent and descent
        TInt lTextAscent = lTextElement->Font()->FontMaxAscent();
        TInt lTextDescent = lTextElement->Font()->FontMaxDescent();

	TReal32 lTextHeight = lTextAscent + lTextDescent;

        if (lTextHeight == 0)
	        {
	    	//no text height so we need to not draw the text
    		this->RemoveChild(lTextElement);
			delete lTextElement;
			lTextElement = NULL;
	    	return;
            }
            
        // Calculate gap between two consecutive lines of text
        // CFont provides FontLineGap() which is Font Height + Line Gap
        
        TInt lLineGap = lTextElement->Font()->FontLineGap();
        // Number of lines of text
	    TInt lNumOflines = (TReal32)( lInnerRectHeightF * lScale)/( lLineGap ) ;

        if( lNumOflines < 1)
            {
        	//text is taller than the rectangle
    		this->RemoveChild(lTextElement);
			delete lTextElement;
			lTextElement = NULL;
            return;
            }
        // Create a list of line widths and line texts to use with ConvertToVisualAndWrapToArrayL
        CArrayFixFlat<TInt>* lLineWidthArray = new ( ELeave ) CArrayFixFlat<TInt>( lNumOflines );
        CleanupStack::PushL(lLineWidthArray);
        
        //one px offset is taken on Left & right ends w.r. to inner rect width.
        lLineWidthArray->AppendL((TInt)( lInnerRectWidthF * lScale ) - 2,lNumOflines);

        CArrayFixFlat< TPtrC >*   lWrappedArray = new ( ELeave ) CArrayFixFlat<TPtrC>( lLineWidthArray->Count() );

        CleanupStack::PushL(lWrappedArray);
        
        //ConvertToVisualAndWrapToArrayL requires KAknBidiExtraSpacePerLine. 
        //Refer the help of AknBidiTextUtils::ConvertToVisualAndWrapToArrayL
        HBufC *lBuf = HBufC::NewLC(iAllText->Length() + (lNumOflines) * KAknBidiExtraSpacePerLine);

        TPtr buf = lBuf->Des();
        buf.Copy(iAllText->Des());
        // Below API fills the lWrappedArray with the lines of text. If no space 
        // is available in textArea, then trailing ellipsis(...) will be displayed.
        AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(buf, *lLineWidthArray, 
            *lTextElement->iBitmapFont, *lWrappedArray, ETrue);
        
        // Dummy text element is no longer required.
		this->RemoveChild(lTextElement);
		delete lTextElement;
		lTextElement = NULL;
		
		// Calculating the y offset for first line of text. 
		// Text is positioned wrt font baseline, hence the position =
		//      Inner Rectangle 's Y Coordinate + Unscaled Font Ascent
		// The scaling will be taken care of by UpdateCTM on the text elements.
        TInt lTextCurrentY = TReal32(iInnerRectangle->Y()) + (lTextAscent / (TReal32)lScale );
		TInt lWrappedArrayCnt = lWrappedArray->Count();
		
		// lWrappedArrayCnt is guaranteed to be less than lNumOfLines.
		// This loop creates Text  elements as many as elements in lWrapppedArray.
        // These elements are appended as child to the text area.
		for( TInt i=0; i < lWrappedArrayCnt ; i++)
		    {
		    lTextElement = (CSvgTextElementImpl*)iOwnerDocument->CreateElementL(KSvgTextElement);
            CleanupStack::PushL( lTextElement );
            iTextElementsArray->Append(lTextElement);
		    
		    TPtrC lineText = lWrappedArray->At(i);
		    lTextElement->SetAttributeDesL(KAtrCdata,lineText);
		    
		    //Text positioned from the inner rectangle's X- coordinate + 1 pixel gap 
	    	lTextElement->SetAttributeFloatL(KAtrX, TFloatFixPt(iInnerRectangle->X() + TFloatFixPt(1)));

            //need to set the next text element up to go down the height here too...
            lTextElement->SetAttributeFloatL(KAtrY, TFloatFixPt(lTextCurrentY));

			lTextElement->SetAttributeFloatL(KAtrWidth, TFloatFixPt(iInnerRectangle->Width() - TFloatFixPt(2)));
			lTextElement->SetAttributeFloatL(KAtrHeight, lTextHeight);

            this->AppendChildL(lTextElement);

            lTextElement->UpdateCTM();
            lTextElement->UpdateCurrentScaledFont();

            //The subsequent position calculated as lTextHeight( Ascent + descent ) / unscaled line gap
            lTextCurrentY +=  (TInt)(( lLineGap )/(TReal32)lScale);

            CleanupStack::Pop( lTextElement );
		    }

		 //delete lBuf;
		 CleanupStack::PopAndDestroy(lBuf);
		 CleanupStack::Pop(lWrappedArray);
		 delete lWrappedArray;
		 CleanupStack::Pop(lLineWidthArray);
		 delete lLineWidthArray;

	}
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextAreaElementImpl::GetLength()
    {
        return iAllText->Length();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::GetText( TDes& allText )
    {
    if ( iAllText )
        {
    	if ( allText.MaxLength() < iAllText->Length() )
    	    {
    	    allText = iAllText->Left( allText.MaxLength() );
    	    }
    	    else
    	        {
    	        allText = *iAllText;
    	        }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::SetTextL( const TDesC& aText )
    {

   	_LIT( KPreserve, "preserve" );
    _LIT( KDefault, "default");
    if ( iAllText )
        {
        delete iAllText;
        iAllText = NULL;
        }

    iAllText = aText.AllocL();
    TPtr textDes = iAllText->Des();
    TBool def = ETrue;
    
    // Get the xml:space value.
    TBuf<20> lXmlspace;    
   
    if(XMLSpace() == KPreserve)
    	{
    	lXmlspace=KPreserve;
    	def=EFalse;
    	}
    else if(XMLSpace() == KDefault)
    	{
    	lXmlspace=KDefault;
    	}
    // xml:space is inherited if not specified
    if ( lXmlspace!= KPreserve && lXmlspace != KDefault) 
        {
    	// Traverse to the nearest parent element which has the xml:space 
		// If no xml:space then render using xml:space="default"
	    CSvgElementImpl *lRoot=(CSvgElementImpl *)this->ParentNode();
	 	while(lRoot)
	 		{
	 		if ((lRoot->XMLSpace() == KPreserve))
	    		{
	    		def = EFalse;
	    		break;
	        	}
	        else if(lRoot->XMLSpace()==KDefault)
	        	{
	        	break;	
	        	}
	        lRoot=(CSvgElementImpl *)lRoot->ParentNode();
	 		}
        }
    
    _LIT(KSpace, " ");
    // default    
    if ( def && lXmlspace != KPreserve )
        {
        
        // Remove leading, trailing and contiguous spaces
        textDes.TrimAll();
        
        for (TInt i = textDes.Length() - 1; i >= 0; i--)
        	{
                	
        	// Remove new line character
        	if (textDes[i] == '\n') 
       			{
          		textDes.Delete(i,1);
       			}
          	
          	// Tab to be replaced with space.
          	if(textDes[i] == '\t')
          		{
          		textDes.Replace(i,1,KSpace);
          		}           	
        	}
        }
    // preserve
    else
        {
       
        for ( TInt i = 0; i < textDes.Length(); i++ )
            {
            // ms-dos carriage return contains two characters: 13, 10
           	const TInt KCarriageReturn1 = 13;
           	const TInt KCarriageReturn2 = 10;
           	
           	if ( i + 1 < textDes.Length() &&
           	 	(TInt)textDes[i] == KCarriageReturn1 && 
           	 	(TInt)textDes[i+1] == KCarriageReturn2 )
              	{
                textDes[i] = ' ';
                textDes.Delete( i+1, 1 );
       		    }
			
			// New line and tab should be replaced by space character.         
           	if (textDes[i] == '\n' ||textDes[i] == '\t')
              	{
                textDes.Replace(i,1, KSpace);
               	}
            }
        }
         
        iNeedTextRearrange = ETrue;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::InitializeL()
{
	if (!iBorderRectangle && !iInnerRectangle)
	{
		iBorderRectangle = CSvgRectElementImpl::NewL(KSvgRectElement, (CSvgDocumentImpl*)iOwnerDocument);
		this->AppendChildL(iBorderRectangle);
		iInnerRectangle = CSvgRectElementImpl::NewL(KSvgRectElement, (CSvgDocumentImpl*)iOwnerDocument);
		this->AppendChildL(iInnerRectangle);

		iInnerRectangle->SetAttributeFloatL(KAtrRx, 2);
		iInnerRectangle->SetAttributeFloatL(KAtrRy, 2);
		iInnerRectangle->SetPropertyL(KCSS_ATTR_FILL, _L("none"));
		iInnerRectangle->SetPropertyL(KCSS_ATTR_STROKEWIDTH, _L(".02"));
		iInnerRectangle->SetPropertyL(KCSS_ATTR_STROKE, _L("black"));

		iBorderRectangle->SetAttributeFloatL(KAtrRx, 2);
		iBorderRectangle->SetAttributeFloatL(KAtrRy, 2);
		iBorderRectangle->SetPropertyL(KCSS_ATTR_FILL, _L("none"));
		iBorderRectangle->SetPropertyL(KCSS_ATTR_STROKEWIDTH, _L(".1"));
		iBorderRectangle->SetPropertyL(KCSS_ATTR_STROKE, _L("black"));

		iInitialized = ETrue;
	}
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
{
	if (iBorderRectangle->Width() <= TFloatFixPt(0) || iBorderRectangle->Height() <= TFloatFixPt(0))
		{
			return ETrue;
		}

    TFloatFixPt scale = GetCurrentScale();
    TFloatFixPt width = iInnerRectangle->Width() * scale;
    TFloatFixPt height = iInnerRectangle->Height() * scale;

    if ( width != iLastBBox.iWidth || height != iLastBBox.iHeight )
    {
        iNeedTextRearrange = ETrue;
        iLastBBox.iWidth = width;
        iLastBBox.iHeight = height;
    }

    DrawTextElements( aGc, aElement );

    return ETrue;
 }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::DrawTextElements( CGfx2dGc* aGc, CSvgElementImpl* /*aElement*/ )
{
	//draw text elements...readjust their positions according the width and height of the inner rectangle...
    if (iNeedTextRearrange)
    {
    	//TRAPD(err, ArrangeTextL(aGc));
    	TRAPD(err, LocalizedArrangeTextL(aGc));
    	if (err)
    	    {
		    #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgTextAreaElementImpl::DrawTextElements Error in ArrangeTextL()" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
    	    }

    	iNeedTextRearrange = EFalse;
    }

    //draw text until it reaches the inner border then start a new text element on the next line down
    /*if (iTextElementsArray)
    {
    	 for (int i=0; i < iTextElementsArray->Count(); i++)
    	{
			//draw text elements here...
			TRAPD(err, iTextElementsArray->operator[](i)->DrawL(aGc, aElement));
        	if (err)
        	    {
    		    #ifdef _DEBUG
                _LIT( KErrMsg, "CSvgTextAreaElementImpl::DrawTextElements Error in DrawL()" );
                RDebug::Print(KErrMsg);
                #endif //_DEBUG
        	    }

  		}
    }*/

  	return ETrue;
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::ConstructL(  const TUint8 aElemID )
{
		CSvgElementImpl::InitializeL( aElemID );

		iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR );

    	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    	iSvgStyleProperties->Remove( 0 );

		iTextElementsArray = new (ELeave)RPointerArray<CSvgTextElementImpl> (1);
		iTextElementsArray->Reset();

  		iSvgTransformable = CSvgTransformableImpl::NewL();

		iReqAttrFlag=KSVG_RECT_ELEMFLAG;

		iBorderRectangle = NULL;
		iInnerRectangle = NULL;

		iNeedTextRearrange = ETrue;
		iInitialized = EFalse;

		iAllText = HBufC::NewL( 2 );

  		iEditable = EFalse;

  	// Add this as internal event receiver
  	//((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this, KSvgEventMaskExternalUI);

	((CSvgDocumentImpl*)OwnerDocument())->AddInternalMouseListener( this );

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgTextAreaElementImpl::CSvgTextAreaElementImpl( CSvgDocumentImpl* aDoc )
{
		SetOwnerDocument(aDoc);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::GetBBox( TGfxRectangle2D& aBbox )
{
	if (!iInitialized)
	{
		TGfxRectangle2D myRect(0,0,0,0);
		aBbox = myRect;
		return;
	}

    iBorderRectangle->GetBBox( aBbox );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
{
	if (!iInitialized)
	{
		TGfxRectangle2D myRect(0,0,0,0);
		aBbox = myRect;
		return;
	}

    iBorderRectangle->GetUnscaledBBox(aBbox);
}

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgTextAreaElementImpl::CloneL(MXmlElement* aParentElement)
{
    // create new text area
	CSvgTextAreaElementImpl* newElement = CSvgTextAreaElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

	newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);

    return newElement;
}

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::CopyL( CSvgTextAreaElementImpl* aDestElement )
{
    // copy stuff from superclass
    this->CSvgElementImpl::CopyL(aDestElement);

    // copy iRectangle items special to circle
    aDestElement->iBorderRectangle = (CSvgRectElementImpl*)iBorderRectangle->CloneL((MXmlElement*)iBorderRectangle);
    aDestElement->iInnerRectangle = (CSvgRectElementImpl*)iInnerRectangle->CloneL((MXmlElement*)iInnerRectangle);

    if (iTextElementsArray)
    {
    	TInt textEleArrayCnt = iTextElementsArray->Count();
   	 	for (int i=0; i < textEleArrayCnt; i++)
    	{
    		aDestElement->iTextElementsArray->Append((CSvgTextElementImpl*)iTextElementsArray->operator[](i)->CloneL((MXmlElement*)iTextElementsArray->operator[](i)));
    	}
    }

    // copy the reference to idoc (CSvgDocumentImpl)
    aDestElement->iOwnerDocument = this->iOwnerDocument;
}

/*** FROM MSvgMouseListener ***/
// ---------------------------------------------------------------------------
// mouse entered
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* engine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			engine->NotifyTextAreaEntered(this);
			return ETrue;
		}
    }

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer exits a visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyTextAreaExited(this);
			return ETrue;
		}
    }

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::MouseMoved( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
    return EFalse;
}
// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::MousePressed( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is released on on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextAreaElementImpl::MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyTextAreaActivated(this);
			return ETrue;
		}
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// void CSvgTextAreaElementImpl::Print()
// ---------------------------------------------------------------------------
void CSvgTextAreaElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<textarea x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" editable=\"%d\" >hmm</textarea>", (int)X(), (int)Y(), (int)Width(), (int)Height(), (int)iEditable/*, iAllText*/);
		#endif
	}
}
