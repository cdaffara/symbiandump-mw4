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


#include "SVGRadialGradientElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SvgStopElementImpl.h"
#include "SVGIntCssValueImpl.h"

#include "GfxAffineTransform.h"

_LIT(FX, "fx");
_LIT(FY, "fy");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgRadialGradientElementImpl* CSvgRadialGradientElementImpl::NewL(const TUint8 aElemID,
																   CSvgDocumentImpl* aDoc )
	{
    CSvgRadialGradientElementImpl *self = new (ELeave) CSvgRadialGradientElementImpl(aDoc);
    CleanupStack::PushL(self);
    self->ConstructL(aElemID);
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgRadialGradientElementImpl::ConstructL(const TUint8 aElemID)
	{
    CSvgGradientElementImpl::ConstructL( aElemID );

	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgRadialGradientElementImpl::CSvgRadialGradientElementImpl( CSvgDocumentImpl* aDoc ):
	CSvgGradientElementImpl(aDoc)
	{
	iGradientPaint = &iGfxRadialPaint;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgRadialGradientElementImpl::SetAttributeL(const TDesC &aName, const TDesC &aValue)
	{
	CSvgGradientElementImpl::SetAttributeL( aName, aValue );

    if(aName == FX)
		{
        iGfxRadialPaint.iFx=TFloatFixPt::ConvertString(aValue);
        
        return KErrNone;
		}
    else if(aName == FY)
		{
        iGfxRadialPaint.iFy=TFloatFixPt::ConvertString(aValue);

        return KErrNone;
		}

    return KErrNone;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MGfxPaint *CSvgRadialGradientElementImpl::PaintAttribute()
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
    for(i=0;i<svgStopEleArrayCnt;i++)
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
		    	break;
			}	
		}
		}
	iGfxRadialPaint.iStopData = 	iStopElementData;

    const TGfxAffineTransform& tempTransform = GetCTM();
	iGradientPaint->iGradientTransform.matrix[0][0] = tempTransform.iM00.iValue;
    iGradientPaint->iGradientTransform.matrix[0][1] = tempTransform.iM01.iValue;
    iGradientPaint->iGradientTransform.matrix[0][2] = tempTransform.iM02.iValue;
	iGradientPaint->iGradientTransform.matrix[1][0] = tempTransform.iM10.iValue;
	iGradientPaint->iGradientTransform.matrix[1][1] = tempTransform.iM11.iValue;
	iGradientPaint->iGradientTransform.matrix[1][2] = tempTransform.iM12.iValue;


    return &iGfxRadialPaint;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgRadialGradientElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
	{
		switch ( aNameId )
        {
        case KAtrCx:
			 aValue = iGfxRadialPaint.iCx ;
			break;
        case KAtrCy:
			aValue = iGfxRadialPaint.iCy ;
			break;
        case KAtrR :
			aValue = iGfxRadialPaint.iR ;
			break;
		case KAtrFx:
			aValue = iGfxRadialPaint.iFx;
			break;
		case KAtrFy:
			aValue = iGfxRadialPaint.iFy;
			break;
        default:
			return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
		}

	return 0;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgRadialGradientElementImpl::SetAttributeFloatL( const TInt aNameId, const TFloatFixPt aValue )
	{
    switch ( aNameId )
        {
        case KAtrCx:
			iGfxRadialPaint.iCx = aValue;
			break;
        case KAtrCy:
			iGfxRadialPaint.iCy = aValue;
			break;
        case KAtrR :
			iGfxRadialPaint.iR = aValue;
			break;
		case KAtrFx:
			iGfxRadialPaint.iFx= aValue;
			break;
		case KAtrFy:
			iGfxRadialPaint.iFy = aValue;
			break;

        default:
			return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
	return 0;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgRadialGradientElementImpl::~CSvgRadialGradientElementImpl()
	{

	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgRadialGradientElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<radialGradient gradientUnits=\"hmm\" gradientTransform=\"hmm\" cx=\"%d\" cy=\"%d\" r=\"%d\" fx=\"%d\" fy=\"%d\" spreadMethod=\"hmm\" xlink:href=\"hmm\"/>",
		(int)iGfxRadialPaint.iCx, (int)iGfxRadialPaint.iCy, (int)iGfxRadialPaint.iR, (int)iGfxRadialPaint.iFx, (int)iGfxRadialPaint.iFy/*, Href()*/);
		#endif
	}
}
