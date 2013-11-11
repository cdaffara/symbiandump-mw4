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


#include "SVGLinearGradientElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SvgStopElementImpl.h"
#include "SVGIntCssValueImpl.h"

#include "GfxAffineTransform.h"

_LIT(X1,"x1");
_LIT(X2,"x2");
_LIT(Y1,"y1");
_LIT(Y2,"y2");
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLinearGradientElementImpl* CSvgLinearGradientElementImpl::NewL(const TUint8 aElemID, CSvgDocumentImpl* aDoc )
	{
    CSvgLinearGradientElementImpl *self = new (ELeave) CSvgLinearGradientElementImpl(aDoc);
    CleanupStack::PushL(self);
    self->ConstructL(aElemID);
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgLinearGradientElementImpl::ConstructL(const TUint8 aElemID)
	{
    CSvgGradientElementImpl::ConstructL(aElemID);

	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLinearGradientElementImpl::CSvgLinearGradientElementImpl( CSvgDocumentImpl* aDoc ):
	CSvgGradientElementImpl(aDoc)
	{
	iGradientPaint = &iGfxLinearPaint;
	}

// ---------------------------------------------------------------------------
//Dt:17-Feb-2009 modified the function to support  the % values for X1,Y1,X2,Y2
//TSW ISSUE ID: ANAE-7NZ8B8
// ---------------------------------------------------------------------------
TInt CSvgLinearGradientElementImpl::SetAttributeL(const TDesC &aName, const TDesC &aValue)
	{
	TFloatFixPt KZero( KFloatFixZero );
	TFloatFixPt KOne( KFloatFixOne );
	TFloatFixPt val;
	if ( aName == X1 || aName == X2 || aName == Y1 || aName == Y2 )
		{

		if ( IsValidValue(aValue) )
			{
			val = TFloatFixPt::ConvertString(aValue); 
			if ( aValue.Locate('%') != KErrNotFound )
				{
				val = val / TFloatFixPt(100);
				}
			}
		else
			{
			if( aName == X2 )
				{
				val = KOne;
				}
			else 
				{
				val = KZero;
				}
			}
		if( aName==X1 )
			{
			iGfxLinearPaint.iX1 = val;
			}
		else if( aName==Y1 )   
			{
			iGfxLinearPaint.iY1 = val;
			}
		else if( aName==X2 )
			{
			iGfxLinearPaint.iX2 = val;
			}
		else
			{
			iGfxLinearPaint.iY2 = val;
			}

		}
	else 
		{
		CSvgGradientElementImpl::SetAttributeL( aName, aValue );
		}
    return 0;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MGfxPaint *CSvgLinearGradientElementImpl::PaintAttribute()
	{
	TInt i=0;
	TFloatFixPt lOffset;
	TUint32 lStopColor;
	TFloatFixPt	lStopOpacity;
	if( iStopElementData )
		{
		iStopElementData->Reset();
		}
	
	TInt svgStopEleArrayCnt = iSvgStopElementArray->Count();
    for(i=0;i<svgStopEleArrayCnt ;i++)
		{
		CSvgStopElementImpl* temp = (*iSvgStopElementArray)[i];
		
		// Check if the element has valid offset value. If the offset had invalid value
		// it will be made zero but this function will return FALSE.
		if( temp->IsValidElement() )
		{
		    temp->GetOffset(lOffset);
            temp->GetStopColor(lStopColor);
	    	temp->GetStopOpacity (lStopOpacity);
	    	TSvgStopData la ;
	    	la.iOffset = lOffset;
	    	la.iStopColor = lStopColor;
	    	la.iStopOpacity = lStopOpacity;
	    	TInt error = iStopElementData->Append(la);
	    	if(error != KErrNone)
		 	{
			// not enough memory
			break;
			}	
		}
        
		}
	iGfxLinearPaint.iStopData = iStopElementData;
    const TGfxAffineTransform& tempTransform = GetCTM();
	iGradientPaint->iGradientTransform.matrix[0][0] = tempTransform.iM00.iValue;
    iGradientPaint->iGradientTransform.matrix[0][1] = tempTransform.iM01.iValue;
    iGradientPaint->iGradientTransform.matrix[0][2] = tempTransform.iM02.iValue;
	iGradientPaint->iGradientTransform.matrix[1][0] = tempTransform.iM10.iValue;
	iGradientPaint->iGradientTransform.matrix[1][1] = tempTransform.iM11.iValue;
	iGradientPaint->iGradientTransform.matrix[1][2] = tempTransform.iM12.iValue;
    return &iGfxLinearPaint;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLinearGradientElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
	{
	 switch ( aNameId )
        {
        case KAtrX1:
			 aValue = iGfxLinearPaint.iX1 ;
			 break;
        case KAtrY1:
			 aValue = iGfxLinearPaint.iY1 ;
			break;
        case KAtrX2 :
			 aValue = iGfxLinearPaint.iX2 ;
			break;
        case KAtrY2 :
			 aValue = iGfxLinearPaint.iY2 ;
			break;
        default:
			return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
	return 0;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLinearGradientElementImpl::SetAttributeFloatL( const TInt aNameId, const TFloatFixPt aValue )
	{
    switch ( aNameId )
        {
        case KAtrX1:
			iGfxLinearPaint.iX1 = aValue;
			break;
        case KAtrY1:
			iGfxLinearPaint.iY1 = aValue;
			break;
        case KAtrX2 :
			iGfxLinearPaint.iX2 = aValue;
			break;
        case KAtrY2 :
			iGfxLinearPaint.iY2 = aValue;
			break;
        default:
			return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
	return 0;
	}
TInt CSvgLinearGradientElementImpl::SetAttributeIntL( const TInt aNameId,
                                        const TInt32 aValue )
{
	CSvgGradientElementImpl::SetAttributeIntL(  aNameId,
												 aValue );
	return 0;

}
TInt CSvgLinearGradientElementImpl::GetAttributeIntL( const TInt aNameId, TInt32& aValue )
{
	CSvgGradientElementImpl::GetAttributeIntL(aNameId,  aValue );
	return 0;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLinearGradientElementImpl::~CSvgLinearGradientElementImpl()
	{

	}

void CSvgLinearGradientElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<linearGradient xlink:href=\"hmm\" spreadMethod=\"hmm\" gradientUnits=\"hmm\" gradientTransform=\"hmm\" x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"/>",
		/*Href(),*/ (int)iGfxLinearPaint.iX1, (int)iGfxLinearPaint.iY1, (int)iGfxLinearPaint.iX2, (int)iGfxLinearPaint.iY2);
		#endif
	}
}
TBool CSvgLinearGradientElementImpl::IsValidValue(const TDesC& aValue)
{
    TBool lValidValue = ETrue;
    // aValue can only contain these characters "-+.%0123456789", If it contains any thing
    // other than this it should be treated as 0.
    
    _LIT(KValidChars,"-+.%0123456789");
    TBufC<14> lValidChars(KValidChars);
        
    TInt llength = aValue.Length();
        
    for( int charpos = 0 ; charpos < llength; charpos++ )
    {
        if( lValidChars.Locate(aValue[charpos]) == KErrNotFound )
        {
            
            lValidValue = EFalse;
            break;
        }
    }
    
    return lValidValue;
}
