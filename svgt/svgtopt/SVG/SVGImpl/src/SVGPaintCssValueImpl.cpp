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


#include "SVGPaintCssValueImpl.h"
#include "SVGFloatCssValueImpl.h"

#include "SVGElementImpl.h"
#include "SVGColor.h"
#include "GfxPaint.h"

#include "SVGDocumentImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGSolidColorElementImpl.h"



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CPaintCssValueImpl::~CPaintCssValueImpl()
    {

    if( iUrlId )
        {
        delete iUrlId;
        iUrlId = NULL ;
        }

     if ( iValue && iIsValueOwned )

        {
        delete ( TSvgColor * ) iValue;
        }
     iValue = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MGfxPaint* CPaintCssValueImpl::Value()
    {
    CSvgElementImpl* lReferencedElement = NULL;
    if ( iUrlFlag && iSvgElementImpl )
        {
        	// iUrlFlag is true means this paint is gradient.
        	CSvgDocumentImpl *lDoc = (CSvgDocumentImpl *)iSvgElementImpl->OwnerDocument();

        	if (lDoc)
        	{
        		if(iUrlId)
        		{
		        	lReferencedElement = ( CSvgElementImpl*)lDoc->GetElementById( *iUrlId );
		        	if(lReferencedElement)
		        		TInt elementId = lReferencedElement->ElemID();
		        	if ( lReferencedElement && ((lReferencedElement->ElemID() == KSvgLinearGradientElement) ||
		              (lReferencedElement->ElemID() == KSvgRadialGradientElement)) )
		        	{
		         		//Get Attributes from Gradient Element
		        		iValue =  ((CSvgGradientElementImpl *)lReferencedElement)->PaintAttribute();
                        iIsValueOwned = EFalse;
		        	}
		        	else if (lReferencedElement && lReferencedElement->ElemID() == KSvgSolidColorElement )
		        	{
		        		// Get Attributes from solidColor element
		        		GetColorFromSolidColorElement(lReferencedElement);
		        		// delete the reference because we now have a clone.
	   					TRAPD(err, SetValueL(iValue->GetColor()));
		        		delete iUrlId;
		        		iUrlId = NULL;
	   					if (err)
    	   					{
                            #ifdef _DEBUG
                            RDebug::Printf("CPaintCssValueImpl::Value() error trapped=%d", err);
                            #endif
    	   					}
		        		iUrlFlag = EFalse;
  		        	}
  		        	else  // id = color value... for some reason like <rect id="red" fill="red"
  		        		{
						TRAPD(err, SetValueL(iUrlId->Des()));
	   					if (err)
    	   					{
                            #ifdef _DEBUG
                            RDebug::Printf("CPaintCssValueImpl::Value() error trapped=%d", err);
                            #endif
    	   					}
  		        		}
        		}
        	}
        }

    return iValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CPaintCssValueImpl::SetValueL( const TDesC& aValue )
    {
    if ( iValue )
        {
        delete ( TSvgColor * ) iValue;
        iValue = NULL;
        iIsValueOwned = EFalse;
        }

    _LIT( KNone, "none" );

    _LIT( KCurrentColor, "currentColor" );
    _LIT(KUrl,"url");


    TUint32 tempVal = 0x000000;//default is 'black'
    if ( aValue == KNone )
        {
        tempVal = KGfxColorNull;
        }
    else if ( aValue == KCurrentColor )
        {
        tempVal = KSvgCurrentColor;
        }
    else if (aValue.Left(3) == KUrl)
        {
        // URL = gradient at the moment...
        iUrlFlag=ETrue;

        HBufC* lReferenceId = (aValue.Right(aValue.Length()-5)).AllocL();


        if( iUrlId )
            {
            delete iUrlId;
            iUrlId = NULL;
            }

        iUrlId = (lReferenceId->Left(lReferenceId->Length()-1)).AllocL();

        delete lReferenceId;
        lReferenceId = NULL;

        // Return it from here otherwise a memory Leak will occur
        return KErrNone;

        }
    else
        {


        TSvgColor tColor(KGfxColorNull);
        if( tColor.GetStringL( aValue,tempVal) != EFalse )
            {
            }
        else
            {
            tempVal = 0;
            iDefaultSet = ETrue;
            }
        }
    iValue = new ( ELeave ) TSvgColor( tempVal );
    iIsValueOwned = ETrue;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CPaintCssValueImpl::SetValueL( const TInt& aValue )
    {
     if ( iValue && !iUrlFlag)
        {
        delete ( TSvgColor * ) iValue;
        iValue = NULL;
        }
		iUrlFlag = EFalse;
    iValue = new ( ELeave ) TSvgColor( aValue );
    iIsValueOwned = ETrue;
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// This method gets the color value from the referenced solidColor element
// ---------------------------------------------------------------------------
void CPaintCssValueImpl::GetColorFromSolidColorElement(CSvgElementImpl* aReferencedElement)
    {
    CCssValue*  lCssValue = NULL;
    aReferencedElement->FindProperty( KCSS_ATTR_FILL, lCssValue, aReferencedElement );
    if (lCssValue != NULL)
       {
       iValue = ((CPaintCssValueImpl*)lCssValue)->Value();
       ((CSvgSolidColorElementImpl*)aReferencedElement)->ApplyOpacitytoElement( iSvgElementImpl );
       }
    }

/*// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CPaintCssValueImpl::SetL( CPaintCssValueImpl* aValue )
{
	iSvgElementImpl = aValue->iSvgElementImpl;
	iValueType = aValue->iValueType;

	if (aValue->iValue && !aValue->iUrlFlag)
	{
		CloneRGBValueL( ((TSvgColor*)(aValue->iValue))->Value());
	}
	else if (aValue->iValue)
	{
		iValue = aValue->iValue;
	}

	iDefaultSet = aValue->iDefaultSet;
	iUrlFlag = aValue->iUrlFlag;

    if( aValue->iUrlFlag )
        {
        SetUrlIdL(aValue->iUrlId->Des());
        }
}
*/

// *******************************************************
// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CPaintCssValueImpl::CPaintCssValueImpl() :iUrlId(NULL), iValue( NULL ),
                                          iUrlFlag(EFalse), 
                                          iDefaultSet (EFalse), 
                                          iIsValueOwned( EFalse )
    {
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void  CPaintCssValueImpl::SetUrlflag()
    {
    iUrlFlag= ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CPaintCssValueImpl::GetUrlflag()
{
    return iUrlFlag;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void  CPaintCssValueImpl::SetUrlIdL(const TDesC& aValue)
    {
    if( iUrlId )
        {
        delete iUrlId;
        iUrlId = NULL;
        }
    iUrlId  = aValue.AllocL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CPaintCssValueImpl::CloneRGBValueL(const TInt& aValue)
    {
    if (!iUrlFlag)
        {
        if(iValue)
        	{
        	delete ( TSvgColor * ) iValue;
        	}
   		iValue = new ( ELeave ) TSvgColor( (TUint32)aValue );
   		iIsValueOwned = ETrue;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
HBufC*  CPaintCssValueImpl::GetUrlId()
    {
    return iUrlId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CPaintCssValueImpl::IsEqual( CCssValue* aValue )
{
    if (((CPaintCssValueImpl*)aValue)->iValue == iValue)
    {
        return ETrue;
    }

    return EFalse;
}


// ---------------------------------------------------------------------------
// IsDefaultSet check if default color is set
// ---------------------------------------------------------------------------
TBool CPaintCssValueImpl::IsDefaultSet()
	{
	return iDefaultSet;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CPaintCssValueImpl::SetElement(CSvgElementImpl* aElement)
{
	iSvgElementImpl = aElement;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgElementImpl* CPaintCssValueImpl::GetElement()
{
	return iSvgElementImpl;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CPaintCssValueImpl::Print()
{
#ifdef _DEBUG
	if (iValue)
	{
		RDebug::Printf("%d", (( TSvgColor* )iValue)->GetColor() );
	}
#endif
}
