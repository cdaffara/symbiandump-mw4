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


#include "SVGAttributeVerifier.h"
#include <limits.h>
#include <hal.h>
#include <hal_data.h>
// --------------------------------------------------------------------------
// TBool CSvgAttributeVerifier::ParseForDecimalValue( const TDesC& aNum2Str, TReal32& aValue )
// ---------------------------------------------------------------------------
TBool CSvgAttributeVerifier::ParseForDecimalValue( const TDesC& aNum2Str, TReal32& aValue, const TDesC& aAttrValue, TBool isSvgElement )
	{
	TLex parser( aNum2Str );
	parser.SkipSpace();
	TInt result = parser.Val( aValue, '.' );
	TChar chr;
    TBool numUnits = EFalse ;
    TPtrC16 remChars = parser.Remainder();
    
    TInt dimInTwips ;
    TInt dimInPixels;
    const TReal32 TwipsInCentimeters = 566.929133858268 ;
    const TReal32 TwipsInInches = 1440 ;
    const TReal32 TwipsInPt = 20 ;
    const TReal32 TwipsInPc = 240 ;
    
    _LIT(Kcm,"cm");
    _LIT(Kmm,"mm");
    _LIT(Kin,"in");
    _LIT(KPx,"px");
    _LIT(KPt,"pt");
    _LIT(KPc,"pc");
    _LIT(KPer,"%");
    
    _LIT(KWidth, "width");
    _LIT(KHeight, "height");
    
    if(isSvgElement)
    {
	    if(aAttrValue == KWidth)
	    {
	  		if(remChars == Kcm)
	    	{
		    	numUnits = ETrue;
		    	HAL::Get( HALData::EDisplayXTwips, dimInTwips );
				HAL::Get( HALData::EDisplayXPixels, dimInPixels );
				
				aValue *=  (dimInPixels*TwipsInCentimeters)/dimInTwips ;
				
	    	}
		    else if(remChars == Kmm)
		    {
		    	numUnits = ETrue;
		    	HAL::Get( HALData::EDisplayXTwips, dimInTwips );
				HAL::Get( HALData::EDisplayXPixels, dimInPixels );
				
				aValue *=  (dimInPixels*TwipsInCentimeters/10)/dimInTwips ;
		    }
		    else if(remChars == Kin)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayXTwips, dimInTwips );
				HAL::Get( HALData::EDisplayXPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInInches)/dimInTwips ;
		    }
		    else if(remChars == KPt)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayXTwips, dimInTwips );
				HAL::Get( HALData::EDisplayXPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInPt)/dimInTwips ;
		    }
		    else if(remChars == KPc)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayXTwips, dimInTwips );
				HAL::Get( HALData::EDisplayXPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInPc)/dimInTwips ;
		    }
		    else if(remChars == KPx || remChars == KPer)
		    {
		    	numUnits = ETrue;
		    }
		}
	    else if(aAttrValue == KHeight)
	    {
	    	if(remChars == Kcm)
	    	{
		    	numUnits = ETrue;
		    
		    	HAL::Get( HALData::EDisplayYTwips, dimInTwips );
				HAL::Get( HALData::EDisplayYPixels, dimInPixels );
				
				aValue *=  (dimInPixels*TwipsInCentimeters)/dimInTwips ;
	    	}
		    else if(remChars == Kmm)
		    {
		    	numUnits = ETrue;
		    	
		    	HAL::Get( HALData::EDisplayYTwips, dimInTwips );
				HAL::Get( HALData::EDisplayYPixels, dimInPixels );
				
				aValue *=  (dimInPixels*TwipsInCentimeters/10)/dimInTwips ;
		    }
		    else if(remChars == Kin)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayYTwips, dimInTwips );
				HAL::Get( HALData::EDisplayYPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInInches)/dimInTwips ;
		    }
		    else if(remChars == KPt)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayYTwips, dimInTwips );
				HAL::Get( HALData::EDisplayYPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInPt)/dimInTwips ;
		    }
		    else if(remChars == KPc)
		    {
		    	numUnits = ETrue;
		    	
				HAL::Get( HALData::EDisplayYTwips, dimInTwips );
				HAL::Get( HALData::EDisplayYPixels, dimInPixels );
				aValue *=  (dimInPixels*TwipsInPc)/dimInTwips ;
		    }
		    else if(remChars == KPx || remChars == KPer)
		    {
		    	numUnits = ETrue;
		    }
		}
    }
    
    
    if(!numUnits)
	while(!(parser.Eos()))
	{
		chr=parser.Get();
		if(!chr.IsDigit())
		{
			aValue=0;
			return result==KErrNotFound;	
		}
		parser.Inc();
	}

	if (result != KErrNone)
		{
		aValue= 0;
		}
	parser.SkipSpace();
	return result == KErrNone;
	}




// --------------------------------------------------------------------------
// TBool CSvgAttributeVerifier::ValidAttrValue( const TDesC& aValue,
// ---------------------------------------------------------------------------
TBool CSvgAttributeVerifier::ValidAttrValue( const TDesC& aValue,
											 TReal32& aConvVal,
											 TUint8 aDatatype,
											 const TDesC& aAttrValue,
											 TBool isSvgElement)
	{
	TReal32 lMinValue = (TReal32)(aDatatype==KSVG_LENGTH_TYPE ? 0 : -KMax16BitValue);
	// invalid number string
	if ( !ParseForDecimalValue( aValue, aConvVal, aAttrValue, isSvgElement) )
		{
		aConvVal=0;
		return EFalse;
		}
	// less than minimum value
	else if ( aConvVal > KMax16BitValue )
		{
		aConvVal = KMax16BitValue;
		return EFalse;
		}
	// greater than maximum value
	else if ( aConvVal < lMinValue )
		{
		aConvVal = lMinValue;
		return EFalse;
		}
	// number string is valid and within range
	return ETrue;
	}

