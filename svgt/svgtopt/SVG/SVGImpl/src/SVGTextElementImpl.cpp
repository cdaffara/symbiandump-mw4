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
#include "SVGTextElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGSchemaData.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGStrCssValueImpl.h"
#include "SVGIntCssValueImpl.h"
#include "SVGPaintCssValueImpl.h"

#include "GfxLine2D.h"
#include "GfxStroke.h"
#include "GfxColor.h"

#include "GfxAffineTransform.h"


#include "SVGStringTokenizer.h"
#include "GfxGeneralPath.h"

#include "SVGGlyphElementImpl.h"
//#include "SvgDefaultFont.h"
#include "SVGErrorImpl.h"
#include "SVGFourPointRect.h"

#include "hal.h"
#include "hal_data.h"

#define KDefaultFontSize 10
#define KOne 1
#define KDefaultUnitsPerEm 1000

_LIT( KGlyphNameNone, "GlyphNameNone");
_LIT( KDefaultFont, "serif" ); // Similar changes should be applied to Gfx2dGcOpenVg.cpp file

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------

CSvgTextElementImpl* CSvgTextElementImpl::NewL(  const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )

    {
    CSvgTextElementImpl*self    = new ( ELeave ) CSvgTextElementImpl( aDoc );
    CleanupStack::PushL( self );


    self->ConstructL(  aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------


CSvgTextElementImpl* CSvgTextElementImpl::NewLC(  const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )
    {
    CSvgTextElementImpl*self    = new ( ELeave ) CSvgTextElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTextElementImpl::CSvgTextElementImpl( CSvgDocumentImpl* aDoc ): iOffset(0)
    {
    SetOwnerDocument(aDoc);

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgTextElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL(  aElemID );


    iArrayX = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
    iArrayY = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );

	#ifdef SVG_FLOAT_BUILD
    iArrayX->AppendL( TFloatFixPt(0) );
    iArrayY->AppendL( TFloatFixPt(0) );

    iPoint.iX = TFloatFixPt( 0 );
    iPoint.iY = TFloatFixPt( 0 );
	#else
    iArrayX->AppendL( TFloatFixPt( 0 , ETrue) );
    iArrayY->AppendL( TFloatFixPt( 0 , ETrue) );


    iPoint.iX = TFloatFixPt( 0 ,ETrue);
    iPoint.iY = TFloatFixPt( 0 ,ETrue);
	#endif

    iText = HBufC::NewL( 2 );
   // *iText = _L( " " );

    iG1 = HBufC::NewL( 0 );
    iG2 = HBufC::NewL( 0 );

    iU1 = HBufC::NewL( 0 );
    iU2 = HBufC::NewL( 0 );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
     User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	iSvgStyleProperties->Remove( 0 );

	iTriedLoadingSVGFonts = EFalse;
	iNeedToCacheGlyphs = EFalse;

    iSvgTransformable = CSvgTransformableImpl::NewL();

	//set in the default font...just in case
	SetFontFamilyL(KDefaultFont ); //_L("DefaultFont"));

	iEditable = EFalse;

	if (OwnerDocument())
	{
	((CSvgDocumentImpl*)OwnerDocument())->AddInternalMouseListener( this );
	}

	//gets the default font that the graphics context holds at the start
	//should be whatever the client side specified as the initial fontspec
	//GetCurrentFontScaled(iBitmapFont, iBitmapFontSpec);

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgTextElementImpl::~CSvgTextElementImpl()
    {

    iGlyphElements.Close();

    if ( iFamilies )
        {
        iFamilies->Reset();
        delete iFamilies;
        iFamilies = NULL;
        }
    if ( iArrayX )
        {
        iArrayX->Reset();
        delete iArrayX;
        iArrayX = NULL;
        }
    if ( iArrayY )
        {
        iArrayY->Reset();
        delete iArrayY;
        iArrayY = NULL;
        }
    if ( iArrayRotate )
        {
        iArrayRotate->Reset();
        delete iArrayRotate;
        iArrayRotate = NULL;
        }

    if ( iText )
        {
        delete iText;
        iText = NULL;
        }

    if ( iG1 )
        {
        delete iG1;
        iG1 = NULL;
        }

    if ( iG2 )
        {
        delete iG2;
        iG2 = NULL;
        }

    if ( iU1)
        {
        delete iU1;
        iU1 = NULL;
        }

    if ( iU2 )
        {
        delete iU2;
        iU2 = NULL;
        }

        ((CSvgDocumentImpl*)OwnerDocument())->RemoveInternalMouseListener( this );
    // The Font is owned by TextElement
    // now as each text element may have its own specific font
    // and keeping one copy at Gc level will not work.
    // Gc is just refering to the Font from Textelement, so
    // need to free it here. Note the iWsSceenDevice initialisation
    // was done in draw as it needs to happens in main thread in case
    // of progressive rendering.
    //
    if (iBitmapFont)
    {
    	 CSvgBitmapFontProvider *tempBitmapFontProvider = ((CSvgDocumentImpl*)iOwnerDocument)->GetBitmapFontProvider();
    	 tempBitmapFontProvider->ReleaseFont(iBitmapFont);
    	 iBitmapFont = NULL;
    	
	}

    if ( iSvgStyleProperties )
        {
        iSvgStyleProperties->Close();
        delete iSvgStyleProperties;
        iSvgStyleProperties = NULL;
        }

    }


// *******************************************************
// From SVG DOM
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt32 CSvgTextElementImpl::NumberOfChars()
    {
    return iText->Length();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::GetX( CArrayFix<TFloatFixPt>* aX )
    {
    	if (aX->Count() > 0)
    	{
    aX->At( 0 ) = iPoint.iX;
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::GetY( CArrayFix<TFloatFixPt>* aY )
    {
    	if (aY->Count() > 0 )
    	{
    aY->At( 0 ) = iPoint.iY;
    	}
    }

// *******************************************************
// SVG Implementation

TBool CSvgTextElementImpl::IsNumberAttributeValid( const TDesC& aValue )

    {
        TBool lValidAttr = EFalse;

        if ( aValue.Length() > 0 )
        {

        TLex    lInput ( aValue );

        TChar tmpchar;
        while ( lInput.Peek() != 0 )
        {
        tmpchar = lInput.Get();
        if( !(tmpchar.IsDigit()) && !(tmpchar == '.') && !(tmpchar == '-') && !(tmpchar == ' ') )
            {
            lValidAttr = EFalse;
            break;
            }
            lValidAttr = ETrue;
        }

        }

        return lValidAttr;
}

// ---------------------------------------------------------------------------
// xml:space is an inheritable attribute which can have one of two values:
//
// default (the initial/default value for xml:space) - When xml:space="default":
//      - Remove all newline characters.
//      - Convert all tab characters into space characters.
//      - Strip off all leading and trailing space characters.
//      - All contiguous space characters will be consolidated.
//
// preserve - When xml:space="preserve" :
//      - Convert all newline and tab characters into space characters.
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetTextL( const TDesC& aText )
    {
    _LIT( KPreserve, "preserve" );
    _LIT( KDefault, "default");
    if ( iText )
        {
        delete iText;
        iText = NULL;
        }

    iText = aText.AllocL();
    TPtr textDes = iText->Des();
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
        	if (textDes[i] == '\n' ||
        	    textDes[i] == '\r') 
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

        iUseDefaultSVGFont = IsMostBasicLatin();
        iNeedToCacheGlyphs = ETrue;
    }

void CSvgTextElementImpl::CacheGlyphsForText()
{
	if (!iNeedToCacheGlyphs || iText == NULL || iText->Length() == 0 || iSVGFont == NULL)
	{
		return;
	}
    // Reset the cache first
    iGlyphElements.Reset();
	//initialize the glyph elements cache array...this seems expensive any other way to do this
	TInt textLength = iText->Length();
	for (TInt j = 0; j < textLength; j++)
	{
		//it might actually be better to put a ptr to missing glyph in here instead of null
		iGlyphElements.Append(NULL);
	}

	//we have an svg font...
    if ( iSVGFont->HasChildNodes() )
    {
    	/*******************************/
    	/* Text Caching functionality, */
    	/*******************************/

        CSvgElementImpl* lFirstChild = ( CSvgElementImpl * ) iSVGFont->FirstChild();

        while ( lFirstChild != NULL )
        {
        	/***********************/
    		/*CACHING HKERN ELEMENT*/
    		/***********************/
            if( lFirstChild->ElemID() == KSvgHkernElement)
            {
            	//1 per font
            	iHkernElement = lFirstChild;
            }

    		/******************************/
    		/*CACHING MISSINGGLYPH ELEMENT*/
    		/******************************/

            if ( lFirstChild->ElemID() == KSvgMissingglyphElement)
            {
            	//1 per font
              	iMissingGlyphElement = lFirstChild;
            }

    		/***********************/
    		/* FONTFACE ELEMENT    */
    		/***********************/

            if ( lFirstChild->ElemID() == KSvgFontfaceElement )
            {
            	//1 per font
				iFontFaceElement = lFirstChild;
            }

            /***********************/
            /* CACHE GLYPH ELEMENT */
            /***********************/

        	if ( lFirstChild->ElemID() == KSvgGlyphElement)
                {
                TPtrC16 lUnicode;
                lFirstChild->GetAttributeDes( KAtrUnicode, lUnicode );

                TPtr textDes = iText->Des();

                TPtrC  lLangCode;
                TInt    langCodeFound = 0;

                TInt checkErrorLangCode = lFirstChild->GetAttributeDes( KAtrLang, lLangCode);
                
                if (checkErrorLangCode !=  KErrNoAttribute)
                    {
                    TPtrC lXmlLangAttr(this->XMLLang());
                    langCodeFound = lLangCode.CompareF( lXmlLangAttr );
                    }
                    
                TInt textLength = iText->Length();
                for (TInt i=0; i < textLength; i++)
                    {
                    	//this probably isnt right i think one of these is a string that needs to be an int
                    if ( textDes[i] == lUnicode[0] && langCodeFound == 0)
                    	{
                    		//then this text string uses this glyph and we need to cache it in each spot it is used
                    		iGlyphElements[i] = lFirstChild;
                    	}
                    }
                }

      		lFirstChild = ( CSvgElementImpl* ) lFirstChild->NextSibling();
    	}
    }

    iNeedToCacheGlyphs = EFalse;
}

TBool CSvgTextElementImpl::IsMostBasicLatin()
{
	//check to see if all basic latin characters
	if (iText == NULL || iText->Length() == 0)
	{
		return EFalse;
	}

	TPtr textDes = iText->Des();

    TInt lOutOfLatinCount = 0;

	TInt textDesLength = textDes.Length();
    for (TInt i=0; i< textDesLength; i++)
    {
      	if (!((TInt)textDes[i] <= 0x007F))
       	{
       		lOutOfLatinCount++;
       		break;
      	}
    }

    //make sure that at least half arent outside of basic latin characters
    if (lOutOfLatinCount > (textDes.Length() / 2) )
    {
      	return EFalse;
    }

    return ETrue;
}

TDesC& CSvgTextElementImpl::GetText()
{
    return *iText;
}

void CSvgTextElementImpl::SetFont(CFont* aBitmapFont, CSvgElementImpl* aSVGFont)
{
	if (aBitmapFont != NULL)
	{
		iBitmapFont = aBitmapFont;
	}
	if (aSVGFont != NULL)
	{
		iSVGFont = aSVGFont;
	}
}

TBool CSvgTextElementImpl::HasFont()
{
	if (iBitmapFont || iSVGFont)
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}


void CSvgTextElementImpl::FreeFontData()
{
	
	if (iBitmapFont)
    {
    	 CSvgBitmapFontProvider *tempBitmapFontProvider = ((CSvgDocumentImpl*)iOwnerDocument)->GetBitmapFontProvider();
    	 tempBitmapFontProvider->ReleaseFont(iBitmapFont);
    	 iBitmapFont = NULL;
    	
	}
	
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetFontFamilyL( const TDesC& aValue )
    {
		//May not need to be called every frame, the CSS value is stored on the element level

    TInt    pos;
    if ( iFamilies )
        {
        iFamilies->Reset();
        delete iFamilies;
        iFamilies = NULL;
        }
    iFamilies = new ( ELeave ) CDesCArrayFlat( 1 );
    TStringTokenizer    tkn ( aValue, _L( "," ) );

    _LIT( KQuote, "'" );
    while ( tkn.HasMoreTokens() )
        {
        TPtrC lToken = tkn.NextToken();
        pos = lToken.Find( KQuote );
        TLex lStr( lToken );

        const TChar KQuoteChar( '\'' );
        if ( pos == KErrNotFound )
            {
            lStr.SkipSpaceAndMark(); //Skip whitespaces at beginning of font-family value
            while (lStr.Get()) //get the remaining string from Mark
            {
            }
            lStr.UnGet(); //"back" up one character

            const TChar KWhiteSpaceChar( ' ' );

            while (lStr.Peek() == KWhiteSpaceChar) //for multi white spaces at the end
            {
            	lStr.UnGet();
            }
                        //Token length = iNext pos - iMark pos
            lStr.Inc(); //Increment iNext position by 1 to "include" iNext

            }
        else
            {
            pos++;
            lStr.SkipAndMark( pos );
            lStr.Inc( pos );
            while ( lStr.Get() != KQuoteChar )
                {
                }
            lStr.UnGet();
            }
        TPtrC   lFontFamily = lStr.MarkedToken();
        iFamilies->AppendL( lFontFamily );
        }

        //default font added to end here...
        iFamilies->AppendL( KDefaultFont ); // _L("DefaultFont"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetRotateL( const TDesC& aValue )
    {
    iArrayRotate = new ( ELeave ) CArrayFixFlat<TReal32>( 1 );
    TStringTokenizer    tkn ( aValue, _L( ", " ) );

    TInt lCount = 0;

    while ( tkn.HasMoreTokens() )
        {
        TPtrC   lToken  = tkn.NextToken();
        TLex    lString ( lToken );
        TReal32 val;
        if (lString.Val( val, '.' ) == KErrNone)
            {
            iArrayRotate->AppendL( val );
            lCount++;
            }
        }

    if( lCount == 0)
        {
        if ( iArrayRotate )
            {
            iArrayRotate->Reset();
            delete iArrayRotate;
            iArrayRotate = NULL;
            }
        }

    }

// ---------------------------------------------------------------------------
//AA: Generic function to parse the "x" or "y" coordinate values
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetArrayL( const TDesC& aValue , const TBool aIsX)
    {

    CArrayFixFlat<TFloatFixPt>* lArray;
    TFloatFixPt lPt;
    // the flag is used to determine which coodinate needs to be processed
    if(aIsX)
        {
        lArray=iArrayX;
        lPt=iPoint.iX;

        }
    else
        {
        lArray=iArrayY;
        lPt=iPoint.iY;
        }

    if ( lArray )
        {
        lArray->Reset();
        delete lArray;
        lArray = NULL;
        }

    lArray = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
CleanupStack::PushL(lArray);

    if( (aValue.Length() > 0 ) && ( IsNumberAttributeValid(aValue) ) )
    {
        TStringTokenizer    tkn     ( aValue, _L( ", " ) );

        TInt                lFirstXY = 0;

        TInt lCount = 0;

        while ( tkn.HasMoreTokens() )
        {
        TPtrC   lToken  = tkn.NextToken();
        TLex    lString ( lToken );
        TReal32 val;
        if (lString.Val( val, '.' ) == KErrNone)
            {
        if( val < -32765 )
            val = -32765;
        if( val > 32765 )
            val = 32765;

        if ( lFirstXY == 0 )
            {
            lPt = val;
            lFirstXY = 1;
            }

        lArray->AppendL( ( TFloatFixPt ) val );
        lCount++;
            }
        }

        if( lCount == 0)
        {
        lPt = TFloatFixPt( 0 );
        lArray->AppendL( ( TFloatFixPt ) 0 );
        }
    }
    else
    {
        lPt = TFloatFixPt( 0 );
        lArray->AppendL( ( TFloatFixPt ) 0 );

    }
CleanupStack::Pop(lArray);

    if(aIsX)
        {
        iPoint.iX = lPt;
        iArrayX = lArray;
        }
    else
        {
        iPoint.iY = lPt;
        iArrayY = lArray;
        }
	// In case of fewer x and more y and 
	// fewer y and more x, we need to append the last value of the 
	// coordinate to the corresponding array to be used for all the 
	// remaining characters.
	if ( iArrayY && iArrayX )   
		{
		
		TInt xCount=iArrayX->Count();
		TInt yCount=iArrayY->Count();
		TInt charCount=iText->Length();
		
		// Either x or y should be having valid values.
		if(xCount>0 || yCount>0)
   			{
   			// Text has more characters than the x or y values.
   			if(charCount>xCount||charCount>yCount)
   			
   				{
   				// More y than x and More characters than x append last
   				// value of the x coordinate.
   				if((yCount>=xCount) && (charCount>xCount))
	   				{
	   				iArrayX->AppendL(iArrayX->At(xCount-1), (charCount-xCount));
	   				}
   				// More x than y and More characters than y append last
   				// value of the y coordinate.
   				if((yCount<=xCount) && (charCount>yCount))
	   				{
	   				iArrayY->AppendL(iArrayY->At(yCount-1), (charCount-yCount));
	   				}
   				}
   			
   			}
		}
        
       }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetX( CArrayFix<TFloatFixPt>* aX )
    {
    	if (aX->Count() > 0)
    	{
    iPoint.iX = aX->At( 0 );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetY( CArrayFix<TFloatFixPt>* aY )
    {
    	if (aY->Count() > 0)
    	{
    iPoint.iY = aY->At( 0 );
    	}
    }

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {
    CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }


// *******************************************************
// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
        case KAtrRefX:
        aValue = iPoint.iX;
        break;
        case KAtrY:
        case KAtrRefY:
        aValue = iPoint.iY;
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
			{
			iPoint.iX = aValue;
			if(!iArrayX)
				{
				iArrayX= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
				}

            iArrayX->Reset();
			iArrayX->AppendL((TFloatFixPt)aValue);
			}
        break;
        case KAtrY:
			{
			iPoint.iY = aValue;
			if(!iArrayY)
				{
				iArrayY= new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
				}

            iArrayY->Reset();
			iArrayY->AppendL((TFloatFixPt)aValue);
			}
        break;


        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCdata:
        aValue.Set(*iText);
        break;
        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgTextElementImpl::SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCdata:
        SetTextL( aValue );
        break;

        case KAtrX:
        SetArrayL(aValue, ETrue);
        break;

        case KAtrY:
        SetArrayL(aValue, EFalse);
        break;

        case KAtrRotate:
        SetRotateL( aValue );
        break;

        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }

    // check first character is number or '.'
    if ( aNameId == KAtrX || aNameId == KAtrY )
        {
        if ( aValue.Length() == 0 ||
             ( !TChar( aValue[0] ).IsDigit() && aValue[0] != '.' && aValue[0] != '-') )
            {
            if ( iOwnerDocument )
                {
               _LIT( KMsg, "Invalid value-string for number attribute: " );
                ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                }
            }
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// This is a Local Function.
// Checks to see if the given character is within the SVG Font's Unicode Range.*
// Only the following formats are allowed for "unicode-range" attribute
// Single numbers: U+20A7 | U+215? | U+00?? | U+E??
// Pair of numbers: U+AC00-D7FF | U+F9000-FAFF
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::IsWithinUnicodeRange( const TDesC& aUnicodeRange,
                                                 const TDesC& aCharacter )
    {
    _LIT( KQQ, "??" );
    _LIT( KQ, "?" );
    _LIT( KF, "F" );
    _LIT( KFF, "FF" );
    _LIT( KLitZero, "0" );
    _LIT( KZZero, "00" );
    _LIT( KHyph, "-" );
    _LIT( KUni, "U+" );

    TUint32     lStartRange = 0;
    TUint32     lEndRange = 65536;

    TUint32     lCharacter      = ( TUint32 ) aCharacter.operator[]( 0 );

    TBuf<11>    lUnicodeRange;

    if(aUnicodeRange.Length() <= 11)
        lUnicodeRange   = aUnicodeRange; //Max.Length "U+00AB-00AF"
    else
        return EFalse; //Length is greater than 11.

    TInt        pos;


    //Removing "U+"
    pos = lUnicodeRange.Find( KUni );
    if ( pos != KErrNotFound )
        lUnicodeRange.Delete( pos, 2 );
    else
        return EFalse; // Not a valid unicode range, i.e. not "U+" element

    //Finding "-", if yes, then pair found.
    pos = lUnicodeRange.Find( KHyph );
    if ( pos != KErrNotFound )//found
    {
        TPtrC   lStartString    = lUnicodeRange.Left( pos );
        TLex    lStart          ( lStartString );
        if (lStart.Val( lStartRange, EHex ) != KErrNone)
            lStartRange=0;

        TPtrC   lEndString  = lUnicodeRange.Right( lUnicodeRange.Length() -
                                                   pos -
                                                   1 );
        TLex    lEnd        ( lEndString );
        if (lEnd.Val( lEndRange, EHex ) != KErrNone)
             lEndRange= 65536;
        }
    else //single number
    {
        pos = lUnicodeRange.Find( KQQ );
        if ( pos != KErrNotFound )
            {
            lUnicodeRange.Replace( pos, 2, KZZero );
            TPtrC   lStartString    = lUnicodeRange.Left( pos + 2 );
            TLex    lStart          ( lStartString );
            if (lStart.Val( lStartRange, EHex ) != KErrNone)
                lStartRange=0;

            lUnicodeRange.Replace( pos, 2, KFF );
            TPtrC   lEndString  = lUnicodeRange.Left( pos + 2 );
            TLex    lEnd        ( lEndString );
            if (lEnd.Val( lEndRange, EHex ) != KErrNone)
                lEndRange= 65536;
            }

        else if ( (pos = lUnicodeRange.Find( KQ ) ) != KErrNotFound )
            {
            lUnicodeRange.Replace( pos, 1, KLitZero );
            TPtrC   lStartString    = lUnicodeRange.Left( pos + 1 );
            TLex    lStart          ( lStartString );
            if (lStart.Val( lStartRange, EHex ) != KErrNone)
                lStartRange=0;

            lUnicodeRange.Replace( pos, 1, KF );
            TPtrC   lEndString  = lUnicodeRange.Left( pos + 1 );
            TLex    lEnd        ( lEndString );
            if (lEnd.Val( lEndRange, EHex ) != KErrNone)
                lEndRange= 65536;
            }
        else
            {
            TLex    lEnd    ( lUnicodeRange );
            if (lEnd.Val( lEndRange, EHex ) != KErrNone)
                lEndRange= 65536;
            lStartRange = lEndRange;
            }
        }

    if ( ( lCharacter >= lStartRange ) && ( lCharacter <= lEndRange ) )
        return ETrue;
    else
        return EFalse;
    }

//Method to set kerning pairs u1 and u2.
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetUKernPairsL( const TDesC& aU1, const TDesC& aU2 )
   {
    if(iU1)
    	{
	    delete iU1;
	    iU1 = NULL;
    	}
    iU1 = aU1.AllocL();
	if(iU2)
		{
		delete iU2;
	    iU2 = NULL;
		}
     iU2 = aU2.AllocL();

   }


//Method to set kerning pairs g1 and g2.
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetGKernPairsL( const TDesC& aG1, const TDesC& aG2 )
   {

   if(iG1)
		{
		delete iG1;
		iG1 = NULL;
		}
    iG1 = aG1.AllocL();

	if(iG2)
		{
		delete iG2;
		iG2 = NULL;
		}
    iG2 = aG2.AllocL();

   }


// ---------------------------------------------------------------------------
// This method checks to see whether kerning is required.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::IsKerningRequired( const TDesC& aPrevGlyphName,
                                              const TDesC& aCurrGlyphName,
                                              const TDesC& aPrevUnicode,
                                              const TDesC& aCurrUnicode )
   {
    _LIT( KGlyphNameNone, "GlyphNameNone");
    TBool lFoundG1(EFalse);
    TBool lFoundG2(EFalse);
    TBool lFoundU1(EFalse);
    TBool lFoundU2(EFalse);

    //Finding G1
    if( aPrevGlyphName != KGlyphNameNone)
    {
    TStringTokenizer    tkn1 ( iG1->Des(), _L( "," ) );
    while ( tkn1.HasMoreTokens() )
    {
        TPtrC   lToken  = tkn1.NextToken();
        if( (lToken.Compare(aPrevGlyphName)) == 0 )
        {
            lFoundG1 = ETrue;
                break;
        }
    }
    }
    //Finding G2
    if( aCurrGlyphName != KGlyphNameNone)
    {
    TStringTokenizer    tkn2 ( iG2->Des(), _L( "," ) );
    while ( tkn2.HasMoreTokens() )
    {
        TPtrC   lToken  = tkn2.NextToken();
        if( (lToken.Compare(aCurrGlyphName)) == 0 )
        {
            lFoundG2 = ETrue;
                break;
        }
    }
    }
    //Finding U1
    TStringTokenizer    tkn3 ( iU1->Des(), _L( "," ) );
    while ( tkn3.HasMoreTokens() )
    {

        TPtrC   lToken  = tkn3.NextToken();

        if( lToken.Length() == 1) //Unicode character
        {
            if( (lToken.Compare(aPrevUnicode.Right(1))) == 0 )
            {
                lFoundU1 = ETrue;
                break;
            }

        }
        else // Unicode Range
        {
            if(IsWithinUnicodeRange( lToken, aPrevUnicode ))
                lFoundU1 = ETrue;
                break;
        }

    }

    //Finding U2
    TStringTokenizer    tkn4 ( iU2->Des(), _L( "," ) );
    while ( tkn4.HasMoreTokens() )
    {

        TPtrC   lToken  = tkn4.NextToken();

        if( lToken.Length() == 1) //Unicode character
        {
            if( (lToken.Compare(aCurrUnicode.Left(1))) == 0 )
            {
                lFoundU2 = ETrue;
                break;
            }

        }
        else // Unicode Range
        {
            if(IsWithinUnicodeRange( lToken, aCurrUnicode ))
            {
                lFoundU2 = ETrue;
                break;
            }
        }

    }

    if ( (lFoundG1 || lFoundU1) && (lFoundG2 || lFoundU2) )
        return ETrue;
    else
        return EFalse;
   }


// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgTextElementImpl::CloneL(MXmlElement* aParentElement)
    {

	CSvgTextElementImpl* newElement = CSvgTextElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

		CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::CopyL( CSvgTextElementImpl* aDestElement )
    {
    if(aDestElement)
    {

    // copy stuff from superclass
    this->CSvgElementImpl::CopyL(aDestElement);

	if(this->iText)
		{
         delete aDestElement->iText;
		 aDestElement->iText= NULL;
		 aDestElement->iText= (this->iText)->AllocL();
		}
    aDestElement->iPoint.iX= this->iPoint.iX;
	aDestElement->iPoint.iY= this->iPoint.iY;

	aDestElement->iLetterSpacing= this->iLetterSpacing;
	aDestElement->iWordSpacing= this->iWordSpacing;
	aDestElement->iRotate= this->iRotate;

	//iFamilies
	if(this->iFamilies)
		{
		TInt lCount= (this->iFamilies)->Count();
		if(lCount)
			{
            if(aDestElement->iFamilies)
				{
				aDestElement->iFamilies->Reset();
			    delete aDestElement->iFamilies;
			    aDestElement->iFamilies = NULL;
			    }
			aDestElement->iFamilies = new ( ELeave ) CDesCArrayFlat( 1 );
			for(TInt i=0; i<lCount; i++)
				{
				TPtrC lPtr= (this->iFamilies)->operator[](i);
				aDestElement->iFamilies->AppendL(lPtr);
				}
			}
		}

    //iArraX
	if(this->iArrayX)
		{
		TInt lCount= (this->iArrayX)->Count();
		if(lCount)
			{
            if(aDestElement->iArrayX)
				{
				aDestElement->iArrayX->Reset();
			    delete aDestElement->iArrayX;
			    aDestElement->iArrayX = NULL;
			    }
			aDestElement->iArrayX = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
			for(TInt i=0; i<lCount; i++)
				{
				TFloatFixPt lFix= (this->iArrayX)->operator[](i);
				aDestElement->iArrayX->AppendL(lFix);
				}
			}
		}

    //iArrayY
	if(this->iArrayY)
		{
		TInt lCount= (this->iArrayY)->Count();
		if(lCount)
			{
            if(aDestElement->iArrayY)
				{
				aDestElement->iArrayY->Reset();
			    delete aDestElement->iArrayY;
			    aDestElement->iArrayY = NULL;
			    }
			aDestElement->iArrayY = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
			for(TInt i=0; i<lCount; i++)
				{
				TFloatFixPt lFix= (this->iArrayY)->operator[](i);
				aDestElement->iArrayY->AppendL(lFix);
				}
			}
		}

  // iArrayRotate
	if(this->iArrayRotate)
		{
		TInt lCount= (this->iArrayRotate)->Count();
		if(lCount)
			{
            if(aDestElement->iArrayRotate)
				{
				aDestElement->iArrayRotate->Reset();
			    delete aDestElement->iArrayRotate;
			    aDestElement->iArrayRotate = NULL;
			    }
			aDestElement->iArrayRotate = new ( ELeave ) CArrayFixFlat<TReal32>( 1 );
			for(TInt i=0; i<lCount; i++)
				{
				TReal32 lFix= (this->iArrayRotate)->operator[](i);
				aDestElement->iArrayRotate->AppendL(lFix);
				}
			}
		}

///////////////////Cloning the glyph elements///////////////////////
		TInt lCount= (this->iGlyphElements).Count();
		if(lCount)
			{
            for(TInt i=0; i<lCount; i++)
				{
				CSvgElementImpl* element = (this->iGlyphElements).operator[](i);
				aDestElement->iGlyphElements.AppendL(element);
				}
			}


        aDestElement->iNeedToCacheGlyphs = this->iNeedToCacheGlyphs;

//////////////////////////////////////////////////////////////////////
	aDestElement->iTextAnchor= this->iTextAnchor;
	aDestElement->iTextDecoration= this->iTextDecoration;

	if(this->iG1)
		{
         delete aDestElement->iG1;
		 aDestElement->iG1= NULL;
		 aDestElement->iG1= (this->iG1)->AllocL();
		}

	if(this->iG2)
		{
         delete aDestElement->iG2;
		 aDestElement->iG2= NULL;
		 aDestElement->iG2= (this->iG2)->AllocL();
		}

	if(this->iU1)
		{
         delete aDestElement->iU1;
		 aDestElement->iU1= NULL;
		 aDestElement->iU1= (this->iU1)->AllocL();
		}

	if(this->iU2)
		{
         delete aDestElement->iU2;
		 aDestElement->iU2= NULL;
		 aDestElement->iU2= (this->iU2)->AllocL();
		}

	//Note that copying iBitmapFont from one text element to
	//another, there is danger of double free. Let new element
	//get its own Font pointer. Anyway it just updates the
	//access count on the server side if two fonts are same.
	//aDestElement->iBitmapFont= this->iBitmapFont;
	aDestElement->iBitmapFont= NULL;


    // copy the reference to idoc (CSvgDocumentImpl)
    aDestElement->iOwnerDocument = this->iOwnerDocument;
    }

    }

// ---------------------------------------------------------------------------
// This Method is used to draw the TEXT described within the <text> element.
// There are 2 types of Fonts used to render the text. First, the family-name
// is checked to see if there exists a SVG FONT defined within the SVG File.
// If there is one, then text is rendered using this font, else the text is
// rendered using the Bitmap system fonts using the Drawstring() in Gfx2D.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
{
    if (iText->Length() == 0)
       {
       	return EFalse;
       }

    // Reset and set GC
    this->DrawShapeL( aGc, aElement );
    //if ( !aGc->RenderingHints()->Visibility() )
      //  return EFalse;  // Do nothing if visibility is False.


    TInt8   lTextDecoration = (TInt8)
    (((CIntCssValueImpl *)(iSvgStyleProperties->operator[](KCSS_ATTR_TEXTDECORATION)))->Value());

    TInt8   lTextAnchor =(TInt8)(
    ((CIntCssValueImpl *)(iSvgStyleProperties->operator[](KCSS_ATTR_TEXTANCHOR)))->Value());

/**********************************************************/
/* ROM Configuration Flag whether to include SVG FONTS    */
/**********************************************************/

	//get the scaling factor currently in the graphics library.

    TFloatFixPt KZero;

    //CSvgElementImpl* lFirstChild = NULL;
    CGfxGeneralPath* lShape = NULL;
    CGfxGeneralPath* lShapeMG = NULL;
    TFloatFixPt origX ( iPoint.iX );
    TFloatFixPt currentPosOrigX( iPoint.iX );
    /*iSVGBbox.iX = iPoint.iX;
    iSVGBbox.iY = iPoint.iY;
    iSVGBbox.iWidth = 0;*/
    TFloatFixPt currentPosOrigY( iPoint.iY );
    TFloatFixPt lUnitsPerEm( 1000 );
    TFloatFixPt lAlphabetic( 0 );
    TFloatFixPt lFontHorzOrgX( 0 );
    TFloatFixPt lFontHorzAdvX( 0 );
    TFloatFixPt lMissingGlyphHorzAdvX( 0 );
    TFloatFixPt lFontSize( 10 );
    TPtrC lFontFamily;
    TPtrC16 lUnicode;
    TPtrC16 lPrevUnicode;
    TPtrC16 lUnicodeRange; //Max.Length "U+00AB-00AF"
    TPtrC16 lLangCode; //ex: en-US or en-GB or ja-JP or zh-CN or en....
    TPtrC16 lG1;
    TPtrC16 lG2;
    TPtrC16 lU1;
    TPtrC16 lU2;
    TPtrC16 lPrevGlyphName;
    TPtrC16 lCurrGlyphName;
    TFloatFixPt lK; //kerning purpose....

    TFloatFixPt lAscent( KZero );
    TFloatFixPt lDescent( KZero );
    TFloatFixPt lUnderlinePosition( KZero );
    TFloatFixPt lUnderlineThickness( KZero );
    TFloatFixPt lOverlinePosition( KZero );
    TFloatFixPt lOverlineThickness( KZero );
    TFloatFixPt lStrikethroughPosition( KZero );
    TFloatFixPt lStrikethroughThickness( KZero );
    TFloatFixPt lTotalTextAdvance ( KZero );

    //Getting font-size property

    CCssValue* lCssValue = NULL;
    this->FindProperty( KCSS_ATTR_FONTSIZE, lCssValue, aElement );
    if ( lCssValue )
        {
        lFontSize = (( CFloatCssValueImpl * ) lCssValue )->Value();
        if(lFontSize <= KZero )
            lFontSize = TFloatFixPt ( 10 );
        }

    //Getting font-family property
    this->FindProperty( KCSS_ATTR_FONTFAMILY, lCssValue, aElement );
    if ( lCssValue && ((( CStrCssValueImpl * ) lCssValue )->Value()).Length() != 0)
        {
        lFontFamily.Set( ( ( CStrCssValueImpl * ) lCssValue )->Value() );
        SetFontFamilyL( lFontFamily );
        }

	
		//attempt to load svg font files of same font family name
		//this boolean will be a problem if we want to be able to swap
		//font-family names on the fly and use svg fonts.
    	if (!iTriedLoadingSVGFonts)
    	{
    		LoadExternalSVGFontL(lFontFamily);
    		iTriedLoadingSVGFonts = ETrue;
    	}

    //Retrieving Font Element Pointer from the Font Table
    //if font in svg document
    iSVGFont = NULL;
    if ( iFamilies != NULL )
    {
    	TInt familiesCnt = iFamilies->Count();
    	for (int i=0; i< familiesCnt; i++)
    	{
        	iSVGFont = ( CSvgElementImpl * )
                ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->iFontHashMap->GetFontPtr(iFamilies->operator[]( i ));

			if (iSVGFont == NULL)
			{
				iSVGFont = ( CSvgElementImpl * )
                ((CSvgDocumentImpl*)iOwnerDocument)->iFontHashMap->GetFontPtr( iFamilies->operator[]( i ) );
			}

        	if (iSVGFont != NULL)
        	{
        		//found a font with that family name...continue
        		break;
        	}
    	}
    }

/**********************************************************/
/* Checking for pre-defined SVG Fonts through the         */
/* retrieved pointer from the Font Table.                 */
/* IF the Font is found (ptr!=NULL), SVG Fonts are used   */
/* ELSE Bitmap Font is used through DrawString() method   */
/**********************************************************/

    if ( iSVGFont != NULL && iSVGFont->HasChildNodes())
    {
       	TGfxAffineTransform currentTM;
       	//we found an svg font and are using it...

       	iSVGFont->GetAttributeFloat( KAtrHorizOriginX, lFontHorzOrgX );
       	iSVGFont->GetAttributeFloat( KAtrHorizAdvX, lFontHorzAdvX );
       	lMissingGlyphHorzAdvX = lFontHorzAdvX; //Default Value
       	TInt checkErrShapeMG = -1;//To keep a value other than KErrNone. Should this be replaced

       	if (iNeedToCacheGlyphs)
       	{
       		CacheGlyphsForText();
       	}

        TFloatFixPt lUnitsPerEmInverse;
        TFloatFixPt minusOne( -1 );

		/***********************/
    	/* HKERN ELEMENT       */
    	/***********************/
        if (iHkernElement != NULL)
        {
       		iHkernElement->GetAttributeFloat( KAtrK, lK );

          	TInt checkErrorG1 = iHkernElement->GetAttributeDes( KAtrG1, lG1);
          	TInt checkErrorG2 = iHkernElement->GetAttributeDes( KAtrG2, lG2);
           	TInt checkErrorU1 = iHkernElement->GetAttributeDes( KAtrU1, lU1);
           	TInt checkErrorU2 = iHkernElement->GetAttributeDes( KAtrU2, lU2);

       		if( (checkErrorG1 != KErrNoAttribute) && (checkErrorG2 != KErrNoAttribute) )
      		{
        		  SetGKernPairsL( lG1, lG2);
       		}

       		if( (checkErrorU1 != KErrNoAttribute) && (checkErrorU2 != KErrNoAttribute) )
          	{
          		SetUKernPairsL( lU1, lU2);
          	}
        }

    	/********************************/
    	/* Text Rendering functionality */
    	/*******************************/

    	/***********************/
    	/* MISSINGGLYPH ELEMENT*/
    	/***********************/
        if (iMissingGlyphElement != NULL)
        {
           	iMissingGlyphElement->GetAttributeFloat( KAtrHorizAdvX,
                                                    lMissingGlyphHorzAdvX );

           	if ( lMissingGlyphHorzAdvX == TFloatFixPt( 0 ) )
           	{
           		lMissingGlyphHorzAdvX = lFontHorzAdvX;
           	}

           	checkErrShapeMG = iMissingGlyphElement->GetAttributePath( KAtrData,
                                                                    lShapeMG );
         }

   		/***********************/
   		/* FONTFACE ELEMENT    */
   		/***********************/

		if (iFontFaceElement != NULL)
		{
           	iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm,
                                                    lUnitsPerEm );

	#ifdef SVG_FLOAT_BUILD
	       	if ( lUnitsPerEm <= TFloatFixPt( 0 ) )
        	{
              	lUnitsPerEm = TFloatFixPt( 1000 );
            }
	#else
	       	if ( lUnitsPerEm <= TFloatFixPt( 0,ETrue ) )
        	{
              	lUnitsPerEm = TFloatFixPt( 1000 );
            }
	#endif
            lUnitsPerEmInverse = TFloatFixPt( 1 ) / lUnitsPerEm;

            iFontFaceElement->GetAttributeFloat( KAtrAscent,
                                                    lAscent );

            iFontFaceElement->GetAttributeFloat( KAtrDescent,
                                                    lDescent );

            iFontFaceElement->GetAttributeFloat( KAtrUnderlinePosition,
                                                    lUnderlinePosition );

            iFontFaceElement->GetAttributeFloat( KAtrUnderlineThickness,
                                                    lUnderlineThickness );

            iFontFaceElement->GetAttributeFloat( KAtrOverlinePosition,
                                                    lOverlinePosition );

            iFontFaceElement->GetAttributeFloat( KAtrOverlineThickness,
                                                    lOverlineThickness );

         	iFontFaceElement->GetAttributeFloat( KAtrStrikethroughPosition,
                                                    lStrikethroughPosition );

            iFontFaceElement->GetAttributeFloat( KAtrStrikethroughThickness,
                                                    lStrikethroughThickness );

            iFontFaceElement->GetAttributeFloat( KAtrAlphabetic,
                                                    lAlphabetic );
        }

        //Checking valid UnicodeRange of FontFace Element this is trying to limit the number that we have to search through
        //but why do we need this in here

        /*TInt checkErrorUnicodeRange = iFontFaceElement->GetAttributeDes( KAtrUnicodeRange,
                                                                                lUnicodeRange );
        if ( checkErrorUnicodeRange != KErrNoAttribute )
        {
          	//Function call to unicode-range bounds.
            if(!IsWithinUnicodeRange( lUnicodeRange,
                                                  outputChar ))
            {
               	currentPosOrigX += ( lFontHorzAdvX * lUnitsPerEmInverse ) * lFontSize;
                lIsWithinUnicodeRange = EFalse;
                break;
            }
        }*/

        //Text Anchor related.
        //lTotalTextAdvance = lFontHorzAdvX * (TFloatFixPt)iText->Length() * lUnitsPerEmInverse * lFontSize;
        // Calculate the exact total text advance instead of taking default per glyph
        // advance which will approximate the value.
        TFloatFixPt lGlyphHorzAdvX;
        TInt langCodeFound( 0 );
        // Calculate the total text advance in the following cases:
        //     a. Text Anchor is middle
        //     b. Text Anchor is end
        //     c. Text Decoration is needed( Underline/Overline/Strikethrough )
        if( lTextAnchor == 1 || lTextAnchor == 2 || lTextDecoration != -1 )
            {  
               lTotalTextAdvance= GetTotalTextAdvance(lK,lUnitsPerEmInverse,lFontSize,lMissingGlyphHorzAdvX,lFontHorzAdvX);
            }

        if( lTextAnchor == 1 )//Middle
        {
          	currentPosOrigX -= (lTotalTextAdvance * TFloatFixPt(.5f));
            origX -= (lTotalTextAdvance * TFloatFixPt(.5f));
        }

        if( lTextAnchor == 2 )//End
        {
            currentPosOrigX -= lTotalTextAdvance;
            origX -= lTotalTextAdvance;
        }
        //else default:start

        if( lAlphabetic != KZero )
        {
            //Should be rationally negative, but Adobe traverses positive.
            currentPosOrigY += ( lAlphabetic * lUnitsPerEmInverse ) * lFontSize;
        }

        if( lFontHorzOrgX != KZero )
        {
            //Should be rationally positive, but Adobe traverses negative.
            currentPosOrigX -= ( lFontHorzOrgX * lUnitsPerEmInverse ) * lFontSize;
            origX -= ( lFontHorzOrgX * lUnitsPerEmInverse ) * lFontSize;
        }

    	/***********************/
    	/* GLYPH ELEMENT       */
    	/***********************/
    	 //Assume that xml:lang matches lang code of glyph element.
        //TInt lUnicodeLength( 0 );
		
		//The x coordinate is needed to draw the text decorations. 
		if(iArrayX)
    		{
    		origX=iArrayX->At(0);
    		}
		TInt glyphEleCnt = iGlyphElements.Count();
        for (TInt i=0; i < glyphEleCnt; i++)
        {
        	CSvgElementImpl* lGlyphElement = (CSvgElementImpl*)iGlyphElements[i];

        	if (lGlyphElement != NULL)
        	{
        		//have a valid glyph
        		TInt checkErrorUnicode = lGlyphElement->GetAttributeDes( KAtrUnicode,
                                                                           lUnicode );
                //check the glyph name
                TInt checkErrorGlyphName = lGlyphElement->GetAttributeDes( KAtrGlyphName,
                                                                           lCurrGlyphName );
              	if( checkErrorGlyphName == KErrNoAttribute )
              	{
              		lCurrGlyphName.Set( KGlyphNameNone );
              	}

                //check its language
                TInt checkErrorLangCode = lGlyphElement->GetAttributeDes( KAtrLang,
                                                                            lLangCode );
                TPtrC lXmlLangAttr(this->XMLLang());

                if( checkErrorLangCode != KErrNoAttribute ) //if Lang code present
                {
                  	langCodeFound = lLangCode.CompareF( lXmlLangAttr );//compare xml:lang with Lang code.
                }

                //if Lang code is matching && unicode attr. present, we are good.
                if ( (checkErrorUnicode != KErrNoAttribute) && (langCodeFound == 0) )
                {
                  	//Checking and Introducing kerning(adjusting spacing).
                    if( lPrevGlyphName.Length() > 0 )
                    {
                         if( IsKerningRequired( lPrevGlyphName, lCurrGlyphName, lPrevUnicode, lUnicode ))
                         {
                              currentPosOrigX -= ( lK * lUnitsPerEmInverse ) * lFontSize;
                         }
                    }

                    lGlyphElement->GetAttributeFloat( KAtrHorizAdvX,
                                                       lGlyphHorzAdvX );

	#ifdef SVG_FLOAT_BUILD
                    if ( lGlyphHorzAdvX == TFloatFixPt( 0 ) )
	#else
                    if ( lGlyphHorzAdvX == TFloatFixPt( 0,ETrue ) )
	#endif
                    {
                      	lGlyphHorzAdvX = /*(TFloatFixPt)lUnicodeLength **/ lFontHorzAdvX;
                    }

                    TInt checkErrorShape = lGlyphElement->GetAttributePath( KAtrData,
                                                                                  lShape );
                  	/***********************/
    				/* Drawing the Glyph   */
    				/***********************/
                    if ( checkErrorShape == KErrNone )
                    {
                        this->DrawShapeL( aGc, aElement );
						
            			// Fix for UMAA-753ARS
            			// Scale the strokewidth before applying 
            			// the scaling for glyph
            			ScaleStrokeWidth(aGc);
    					
    					// For multiple x, y get the current position 
    					// from the array.
    					if((iArrayX != NULL ) && ( i< iArrayX->Count() ))
    						{
    						currentPosOrigX = iArrayX->At(i);
    						}
    					if((iArrayY!=NULL) && (i<iArrayY->Count() ) )
    						{
    						currentPosOrigY = iArrayY->At(i);
    						}
    																	
						currentTM = this->GetCTM();

                        currentTM.Translate( currentPosOrigX,
                                                     currentPosOrigY );
                        currentTM.Scale( lUnitsPerEmInverse,
                                                minusOne * lUnitsPerEmInverse );
                        currentTM.Scale( lFontSize, lFontSize );
                        aGc->SetTransform( currentTM );

                        aGc->DrawL( lShape );

                        lPrevUnicode.Set(lUnicode);
                        lPrevGlyphName.Set(lCurrGlyphName);

                        currentPosOrigX += ( lGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;

                    }
                }

        	}
        	else
        	{
        		//need to use missing glyph for this one...
        		//iGlyphElements.Insert(iMissingGlyphElement, i);

        		/***********************/
                /* Drawing MissingGlyph*/
                /***********************/

               	if ( checkErrShapeMG == KErrNone )
                {
                   	this->DrawShapeL( aGc, aElement );
					
					// Fix for UMAA-753ARS
            		// Scale the strokewidth before applying 
            		// the scaling for glyph
            		ScaleStrokeWidth(aGc);
                   	
                   	// For multiple x, y get the current position 
    				// from the array.
                   	if((iArrayX != NULL ) && ( i< iArrayX->Count() ))
    						{
    						currentPosOrigX = iArrayX->At(i);
    						}
    					if((iArrayY!=NULL) && (i<iArrayY->Count() ) )
    						{
    						currentPosOrigY = iArrayY->At(i);
    						}
                   	currentTM = this->GetCTM();

                   	currentTM.Translate( currentPosOrigX, currentPosOrigY );
                   	currentTM.Scale( lUnitsPerEmInverse,
                                    minusOne * lUnitsPerEmInverse );
                   	currentTM.Scale( lFontSize, lFontSize );
                   	aGc->SetTransform( currentTM );
					
                   	aGc->DrawL( lShapeMG );
                 }

                 currentPosOrigX += ( lMissingGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;

             }
        }//end-for
		TGfxRectangle2D lBbox;
		if(iGlyphElements[glyphEleCnt-1] && lShape)
			{
			// Get the bbox for the last glyph element. 
			lShape->GetBounds(currentTM,lBbox);
			}
		else if ( lShapeMG )
			{
			// Get the bbox for the last missing-glyph element.
			lShapeMG->GetBounds(currentTM,lBbox);
			}
		currentPosOrigX=lBbox.iX+lBbox.iWidth;
/**********************************************************/
/* Text Decoration Implementation for SVG Fonts           */
/* The following attributes are supported in this section */
/* "underline-position"                                   */
/* "underline-thickness"                                  */
/* "overline-position"                                    */
/* "overline-thickness"                                   */
/* "strikethrough-position"                               */
/* "strikethrough-thickness"                              */
/**********************************************************/

       if( lTextDecoration != -1 )
       {
           TFloatFixPt lLineStartX( origX );//Text Anchored position(see under fontfaceFound section).
           TFloatFixPt lLineStartY( currentPosOrigY );
           TFloatFixPt lLineEndX( origX + lTotalTextAdvance );
           TFloatFixPt lLineEndY( currentPosOrigY );
           TFloatFixPt lStrokeThickness ( 1 );//default thickness > 0; pensize is set to 1 in Gfx2D.

           if( lTextDecoration == 1 )
            {
               if( lUnderlinePosition != KZero )
               {
                //negative(minus sign) due to inverted font coordinate system
                   lLineStartY = lLineEndY = currentPosOrigY - (lUnderlinePosition * lUnitsPerEmInverse) * lFontSize;
               }
               else if( lUnderlinePosition == KZero  && lDescent != KZero )
               {
                   lLineStartY = lLineEndY = currentPosOrigY - ((lDescent/TFloatFixPt(2)) * lUnitsPerEmInverse) * lFontSize;
               }
               else
               {
                   //default. No information provided( descent or underline position)
               }
               if( lUnderlineThickness != KZero )
               {
                lStrokeThickness = lUnderlineThickness * lUnitsPerEmInverse * lFontSize ;
               }
           }

           if( lTextDecoration == 2 )
           {
            if( lOverlinePosition != KZero )
               {
                //negative(minus sign) due to inverted font coordinate system
                   lLineStartY = lLineEndY = currentPosOrigY - (lOverlinePosition * lUnitsPerEmInverse) * lFontSize;
               }
               else if( lOverlinePosition == KZero && lAscent != KZero )
               {
                   lLineStartY = lLineEndY = currentPosOrigY - (lAscent * lUnitsPerEmInverse) * lFontSize;
               }
               else
               {
                   //default. No information provided( ascent or overline position)
               }
               if( lOverlineThickness != KZero )
               {
                lStrokeThickness = lOverlineThickness * lUnitsPerEmInverse * lFontSize ;
               }
           }


           if( lTextDecoration == 3 )
           {
               if( lStrikethroughPosition != KZero )
               {
                //negative(minus sign) due to inverted font coordinate system
                   lLineStartY = lLineEndY = currentPosOrigY - (lStrikethroughPosition * lUnitsPerEmInverse) * lFontSize;
               }
               else if( lStrikethroughPosition == KZero && lAscent != KZero )
               {
                   lLineStartY = lLineEndY = currentPosOrigY - ((lAscent/TFloatFixPt(3)) * lUnitsPerEmInverse) * lFontSize;
               }
               else
               {
                   //default. No information provided( ascent or strikethrough position)
               }
               if( lStrikethroughThickness != KZero )
               {
                lStrokeThickness = lStrikethroughThickness * lUnitsPerEmInverse * lFontSize ;
               }
           }

           TGfxLine2D lLine( lLineStartX ,lLineStartY, lLineEndX, lLineEndY );

           this->DrawShapeL( aGc, aElement );

           // Setting Stokewidth for SVG Fonts. Check if there exists a value or default value (1) is set.
           if( ( lOverlineThickness != KZero ) || ( lUnderlineThickness != KZero ) || ( lStrikethroughThickness != KZero ) )
           {
               TGfxStroke lStroke = aGc->Stroke();
               lStroke.SetStrokeWidth( lStrokeThickness); // relation to font co-ordinate system.
               aGc->SetStroke( lStroke );
           }


           TGfxColor lForegroundColor(aGc->ForegroundColor());
           if ( lForegroundColor.GetColor() == KGfxColorNull )
           {
           	if ( (aGc->Paint()) != NULL )
               	aGc->SetForegroundColor(TGfxColor( (aGc->Paint())->GetColor() ));
           }

       	aGc->DrawL( &lLine );
       }//end of text decoration functionality
    }
    else
    {

/**********************************************************/
/* If SVG Font is not available then Bitmap Fonts are used*/
/* to render the text by calling the DrawString() function*/
/* of Gfx2D                                               */
/**********************************************************/
            UpdateCurrentScaledFont();

        aGc->DrawStringL( iText->Des(),
                         iPoint.iX,
                         iPoint.iY,
                         lTextAnchor,
                         lTextDecoration,
                         iFamilies,
                         iWordSpacing,
                         iLetterSpacing,
                         iArrayRotate,
                         iArrayX,
                         iArrayY,
                         iBoundingBox,
			 iBitmapFont,
			 iBitmapFontSpec );

        // Storing of the font is done in above routine
        // only, hence removing below lines.
        // store font info for bitmap font
        //if ( iBitmapFont == NULL )
      //iBitmapFont = GetCurrentFontScaled();

    }
            // Fix for ScreenSaver Bug
         CSvgDocumentImpl* doc = (CSvgDocumentImpl*)OwnerDocument();
            if ( doc && doc->Engine() )
            {
             iGfx2dGc = doc->Engine()->GraphicsContext();
            }
    return EFalse;
}

// -----------------------------------------------------------------------------
// void CSvgTextElementImpl::ScaleStrokeWidth(CGfx2dGc* aGc)
// Scale the stroke width in order to nullify the effect of scaling appiled on
// the glyph
// -----------------------------------------------------------------------------
void CSvgTextElementImpl::ScaleStrokeWidth(CGfx2dGc* aGc)
	{
	
    TFloatFixPt lStrokeWidth;
    CCssValue* lCssValue = NULL;
    this->FindProperty( KCSS_ATTR_STROKEWIDTH, lCssValue);
	if ( lCssValue )
    	{
        lStrokeWidth = ( ( CFloatCssValueImpl * ) lCssValue )->Value();
        if (lStrokeWidth <= TFloatFixPt(0))
        	{
            aGc->SetStrokeWidth( TFloatFixPt(0) );
            }
        else
        	{
    		// Get the font-size
    		TFloatFixPt lFontSize( 10 );
    		this->FindProperty( KCSS_ATTR_FONTSIZE, lCssValue, NULL );
    		if ( lCssValue )
        		{
        		lFontSize = (( CFloatCssValueImpl * ) lCssValue )->Value();
        		if(lFontSize <= TFloatFixPt(0) )
        			{
        			lFontSize = TFloatFixPt ( 10 );
        			}
        		}
			TFloatFixPt lUnitsPerEm( 1000 );
			
			// Get units-per-em
			if (iFontFaceElement != NULL)
				{
				iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm,
                                                    lUnitsPerEm );
		
				#ifdef SVG_FLOAT_BUILD
	       				if ( lUnitsPerEm <= TFloatFixPt( 0 ) )
        					{
            				lUnitsPerEm = TFloatFixPt( 1000 );
            				}
				#else
	       				if ( lUnitsPerEm <= TFloatFixPt( 0,ETrue ) )
        					{
              				lUnitsPerEm = TFloatFixPt( 1000 );
            				}
				#endif
			
				}
        
            	lStrokeWidth=(lStrokeWidth*lUnitsPerEm)/lFontSize;
        		aGc->SetStrokeWidth(lStrokeWidth);      
           	}
     	}
													
	}

// *******************************************************
// From MSvgLocatable/MSvgTransformable
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    	if (iSVGFont)
    	{
    		//get the bounding boxes of
    		//all of the glyphs in the child and add them together.
    		GetBBoxForSVGText(aBbox);
    	}
    	else if (iBitmapFont)
    	{
    		GetBBoxForSystemText(aBbox);
        }
        else
        {
            //this is an error case if it gets to here
            // When GetBBox is called just after PrepareDom, the font
            // spec is not initialied yet. It happens only in DrawL.
            // To take care of this issue, and give a correct bounding box,
            // Bitmap font is assumed. Note: This would not work if
            // SVG font is selected. This needs to be fixed more generically.

            UpdateCurrentScaledFont();
        	if (iBitmapFont)
        	    {
        	    GetBBoxForSystemText(aBbox);
        	    }
        	else
        	    {
        	    aBbox.iY = iPoint.iY;
        	    aBbox.iX = iPoint.iX;
        	    aBbox.iWidth = 0;
        	    aBbox.iHeight = 0;
        	    }

        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::GetFourPointBBox( TSvgFourPointRect& aFourPointBBox )
    {
    	if (iSVGFont)
    	{
    		//get the bounding boxes of
    		//all of the glyphs in the child and add them together.
    		GetBBoxForSVGText(aFourPointBBox,this->GetCTM());
    	}
    	else if (iBitmapFont)
    	{
    		GetBBoxForSystemText(aFourPointBBox);
        }
        else
        {
        	//this is an error case if it gets to here
        }
    }

void CSvgTextElementImpl::GetBBoxForSVGText( TGfxRectangle2D& aBbox )
{
	//bbox for svg fonts
	TSvgFourPointRect lFourPtRect;
    GetBBoxForSVGText(lFourPtRect,this->GetCTM());

    lFourPtRect.GetTRect(aBbox);
}

void CSvgTextElementImpl::GetBBoxForSVGText( TSvgFourPointRect& aFourPointBbox, const TGfxAffineTransform& aTransform)
{
	//bbox for svg fonts
    TGfxRectangle2D lBbox;

	TFloatFixPt lascent = Ascent();
	TFloatFixPt ldescent = Descent();
	
	// Fix for ANAE-739CPX
	// When ascent and descent are not specified, get the 
	// the translated and scaled bounding-box for each glyph
	// and then find the total bounding box.
	TFloatFixPt KZero;
	if(lascent<=KZero||ldescent<=KZero)
		{
	    TGfxRectangle2D lBbox1; /* Final bounding box for the text */
		TFloatFixPt lFontSize;
        CCssValue* lCssValue = NULL;
        
        // Get the font-size
    	this->FindProperty( KCSS_ATTR_FONTSIZE, lCssValue, NULL );
    	if ( lCssValue )
        	{
        	lFontSize = (( CFloatCssValueImpl * ) lCssValue )->Value();
        	if(lFontSize <= KZero )
        		{
        		lFontSize = TFloatFixPt ( KDefaultFontSize );
        		}
        	}
	
        TFloatFixPt lUnitsPerEm( KDefaultUnitsPerEm );
		TFloatFixPt lUnitsPerEmInverse(KZero);
		
		// Get units-per-em
		if (iFontFaceElement != NULL)
			{
           	iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm,
                                                    lUnitsPerEm );
		

	       	if ( lUnitsPerEm <= KZero  )
          		{
              	lUnitsPerEm = TFloatFixPt( KDefaultUnitsPerEm );
            	}
	    
            lUnitsPerEmInverse = TFloatFixPt( KOne ) / lUnitsPerEm;
			}
		
			TFloatFixPt lGlyphHorzAdvX(KZero);
			TFloatFixPt lFontHorzAdvX(KZero);
			TFloatFixPt lMissingGlyphHorzAdvX(KZero);
			TFloatFixPt lK(KZero);
			
			if(iSVGFont)
				{
				// Get horiz-adv-x
				iSVGFont->GetAttributeFloat( KAtrHorizAdvX, lFontHorzAdvX );
				}
			
			
			if (iMissingGlyphElement != NULL)
        		{
           		iMissingGlyphElement->GetAttributeFloat( KAtrHorizAdvX,
                                                    lMissingGlyphHorzAdvX );
			

				if ( lMissingGlyphHorzAdvX == KZero )
	  	   			{
           			lMissingGlyphHorzAdvX = lFontHorzAdvX;
           			}
        		}
        
			// Get "k" for hkern
			if(iHkernElement)
				{
				iHkernElement->GetAttributeFloat( KAtrK, lK );	
				}
			TFloatFixPt currentPosOrigX(iPoint.iX);
			
			// Find the total width of the BBox
			TFloatFixPt lTotalTextAdvance =GetTotalTextAdvance(lK,lUnitsPerEmInverse,lFontSize,lMissingGlyphHorzAdvX,lFontHorzAdvX);
			
			//Get the text-anchor
			TInt8   lTextAnchor =(TInt8)(
			((CIntCssValueImpl *)(iSvgStyleProperties->operator[](
									KCSS_ATTR_TEXTANCHOR)))->Value());

			if( lTextAnchor == 1 )//Middle
        		{
          		currentPosOrigX -= (lTotalTextAdvance * TFloatFixPt(.5f));
            	}

        	if( lTextAnchor == 2 )//End
        		{
            	currentPosOrigX -= lTotalTextAdvance;
            	}
        		
			TFloatFixPt minusOne( -1 );
        	
           	// (xMin1, yMin1) is the Top-left-corner and 
        	// (xMax1, yMax1) is the bottom-right-corner 
        	// of the final bounding box for the text    
	   		TFloatFixPt xMin1(KMAXFLOATFIX), yMin1(KMAXFLOATFIX), xMax1(KMINFLOATFIX), yMax1(KMINFLOATFIX);
	   		TGfxRectangle2D lBbox;
			const TDesC& ltext=*iText;
	    	TInt ltextLength = ltext.Length();
			TInt glyphEleCnt = iGlyphElements.Count();
	   	
	   		for (TInt i= 0; i < ltextLength && i < glyphEleCnt; i++)
    			{
       			CSvgElementImpl* lGlyphElement = (CSvgElementImpl*)iGlyphElements[i];
        		
        		CGfxGeneralPath* lShape = NULL;
                if(lGlyphElement||iMissingGlyphElement)
                	{
                		// Tranformation Matrix for positioning and scaling the BBox
        			TGfxAffineTransform myCurrentTM =  aTransform;
			
					myCurrentTM.Translate( currentPosOrigX, iPoint.iY);
        		
            		myCurrentTM.Scale( lUnitsPerEmInverse,
      	                           minusOne * lUnitsPerEmInverse );
            		myCurrentTM.Scale( lFontSize, lFontSize );
                	
                	if(lGlyphElement)
                		{
                		TInt checkErrorShape = lGlyphElement->GetAttributePath( 
                											KAtrData,lShape );	
						
                        lGlyphElement->GetAttributeFloat( KAtrHorizAdvX,
                                                           lGlyphHorzAdvX );

    
                        if ( lGlyphHorzAdvX == KZero )
    	                    {
                          	lGlyphHorzAdvX = /*(TFloatFixPt)lUnicodeLength **/ lFontHorzAdvX;
                        	}
                		currentPosOrigX += ( lGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;
                		}
					else 
						{
						TInt checkErrorShape = iMissingGlyphElement->
								GetAttributePath(KAtrData,lShape );	
					
						currentPosOrigX += ( lMissingGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;
						}
      				// Get the tranformed BBox for each glyph
        			lShape->GetBounds(myCurrentTM,	lBbox);
        		
        			// Find the (xMin1,yMin1) and (xMax1, yMax1) for the total 
        			// bounding box of text
        			if(lBbox.iX<xMin1) xMin1=lBbox.iX;
        			if(lBbox.iY<yMin1) yMin1=lBbox.iY;
        			if((lBbox.iY+lBbox.iHeight)>yMax1) yMax1=lBbox.iY+lBbox.iHeight;
        			if((lBbox.iX+lBbox.iWidth)>xMax1) xMax1=lBbox.iX+lBbox.iWidth;
        			}
            	     
           		}
        	lBbox1.iX=xMin1;
        	lBbox1.iY=yMin1;
        	lBbox1.iWidth=xMax1-xMin1;
        	lBbox1.iHeight=yMax1-yMin1;
         
           	aFourPointBbox.SetRectPoints(lBbox1);
			return;		
		}
	//  End of fix for ANAE-739CPX

	lBbox.iHeight = lascent + ldescent;
	lBbox.iWidth = TextAdvance(*iText);

	TReal32 lascentReal = (TReal32)lascent;
	TReal32 ldescentReal = (TReal32)ldescent;
	TReal32 liHeightReal = (TReal32)lBbox.iHeight;
	TReal32 liWidthReal = (TReal32)lBbox.iWidth;

		//////////////////// rotate without scaling again //////////////////////
    lBbox.iX = iPoint.iX;
    lBbox.iY = iPoint.iY - lascent;

    //at this point the bbox not scaled,translated but not rotated

    //should we create 1/scale factor or create with scale factor then get inverse
    /*TGfxAffineTransform aScaleMat;
    TReal32 inverseScale = 1.0/(TReal)iScale;
    aScaleMat = TGfxAffineTransform::GetScaleInstance(inverseScale, inverseScale);
    */

    const TGfxAffineTransform& ctm = aTransform;

  	TGfxPoint2D transformedPoint(lBbox.iX, lBbox.iY);
    ctm.Transform(&transformedPoint, &transformedPoint, 1);

    TGfxPoint2D lTopLeftCorner(lBbox.iX, lBbox.iY);
    TGfxPoint2D lTopRightCorner(lBbox.iX + lBbox.iWidth, lBbox.iY);
    TGfxPoint2D lBottomLeftCorner(lBbox.iX, lBbox.iY + lBbox.iHeight);
    TGfxPoint2D lBottomRightCorner(lBbox.iX + lBbox.iWidth, lBbox.iY + lBbox.iHeight);

    TGfxPoint2D lTransTopLeftCorner;
    TGfxPoint2D lTransBottomLeftCorner;
    TGfxPoint2D lTransTopRightCorner;
    TGfxPoint2D lTransBottomRightCorner;

    ctm.Transform(&lTopLeftCorner, &lTransTopLeftCorner,1);
    ctm.Transform(&lTopRightCorner, &lTransTopRightCorner,1);
    ctm.Transform(&lBottomLeftCorner, &lTransBottomLeftCorner,1);
    ctm.Transform(&lBottomRightCorner, &lTransBottomRightCorner,1);

    TFloatFixPt lXTranslation =  transformedPoint.iX - lTransTopLeftCorner.iX;
    TFloatFixPt lYTranslation = (transformedPoint.iY - lTransTopLeftCorner.iY ); //- ascent;

    // X position is affected by anchor
    switch( TextAnchor() )
    {
       	case EStartAnchor:
           	//do nothing
        break;
       	case EMiddleAnchor:
       		lXTranslation = lXTranslation - ( lBbox.iWidth / TFloatFixPt( 2 ) );
        break;
       	case EEndAnchor:
           	lXTranslation = lXTranslation - lBbox.iWidth;
        break;
       	default:
       	 	//do nothing
       	break;
    }

    lTransTopLeftCorner.iX = lTransTopLeftCorner.iX + lXTranslation;
    lTransTopLeftCorner.iY = lTransTopLeftCorner.iY + lYTranslation;

    lTransTopRightCorner.iX = lTransTopRightCorner.iX + lXTranslation;
    lTransTopRightCorner.iY = lTransTopRightCorner.iY + lYTranslation;

    lTransBottomLeftCorner.iX = lTransBottomLeftCorner.iX + lXTranslation;
    lTransBottomLeftCorner.iY = lTransBottomLeftCorner.iY + lYTranslation;

    lTransBottomRightCorner.iX = lTransBottomRightCorner.iX + lXTranslation;
    lTransBottomRightCorner.iY = lTransBottomRightCorner.iY + lYTranslation;

    //at this point I have the exact path coordinates of the bbox
    aFourPointBbox.SetRectPoints(lTransTopLeftCorner, lTransTopRightCorner, lTransBottomLeftCorner, lTransBottomRightCorner);

	//aFourPointBbox.SetRectPoints(aTopLeftCorner, aTopRightCorner, aBottomLeftCorner, aBottomRightCorner);

}

void CSvgTextElementImpl::GetBBoxForSystemText( TGfxRectangle2D& aBbox )
{
	//bbox for bitmap fonts
	TSvgFourPointRect lFourPtRect;
    GetBBoxForSystemText(lFourPtRect);

    lFourPtRect.GetTRect(aBbox);

}

void CSvgTextElementImpl::GetBBoxForSystemText( TSvgFourPointRect& aFourPointBbox )
{
    //bbox for bitmap fonts
    TGfxRectangle2D lBbox;

    iScale = GetCurrentScale();

    if (iBoundingBox.iWidth == 0 && iBoundingBox.iHeight == 0)
        {
        iBoundingBox.iWidth  = iBitmapFont->TextWidthInPixels(iText->Des());
        iBoundingBox.iHeight = iBitmapFont->FontMaxHeight();
        }

    //scaled already for bitmap text...
    lBbox.iWidth  = iBoundingBox.iWidth;
    lBbox.iHeight = iBoundingBox.iHeight;
    TFloatFixPt lascent = Ascent();
    TFloatFixPt ldescent = TFloatFixPt(iBoundingBox.iHeight) - Ascent();
    TReal32 linverseScale = 1.0/(TReal)iScale;
    
    TGfxAffineTransform lInitTranslationMat = TGfxAffineTransform::GetTranslateInstance(iPoint.iX,iPoint.iY);
    TGfxAffineTransform lTotalMat = GetCTM();
    lTotalMat.Concatenate(lInitTranslationMat);
    TGfxAffineTransform lInverseScaleMat = TGfxAffineTransform::GetScaleInstance(linverseScale, linverseScale);
    lTotalMat.Concatenate(lInverseScaleMat);
    TGfxAffineTransform lDescentCorrectionMat =TGfxAffineTransform::GetTranslateInstance(0,ldescent);
    lTotalMat.Concatenate(lDescentCorrectionMat);

    TGfxPoint2D lTopLeftCorner(0, TFloatFixPt(0) - lBbox.iHeight );
    TGfxPoint2D lTopRightCorner(lBbox.iWidth , TFloatFixPt(0) - lBbox.iHeight );
    TGfxPoint2D lBottomLeftCorner(0, 0);
    TGfxPoint2D lBottomRightCorner(lBbox.iWidth, 0 );

    
    TGfxPoint2D lTransTopLeftCorner;
    TGfxPoint2D lTransBottomLeftCorner;
    TGfxPoint2D lTransTopRightCorner;
    TGfxPoint2D lTransBottomRightCorner;

		//applying current rotation transform without the scale.
    lTotalMat.Transform(&lTopLeftCorner, &lTransTopLeftCorner,1);
    lTotalMat.Transform(&lTopRightCorner, &lTransTopRightCorner,1);
    lTotalMat.Transform(&lBottomLeftCorner, &lTransBottomLeftCorner,1);
    lTotalMat.Transform(&lBottomRightCorner, &lTransBottomRightCorner,1);

    TFloatFixPt lXTranslation = 0;
    //TFixPt lXTranslation = transformedPoint.iX - iPoint.iX;

    TFloatFixPt lYTranslation = 0;
		//TFixPt lYTranslation = transformedPoint.iY - iPoint.iY;

    // X position is affected by anchor
    switch( TextAnchor() )
    {
       	case EStartAnchor:
           	//do nothing
        break;
       	case EMiddleAnchor:
       		lXTranslation = lXTranslation - ( lBbox.iWidth / TFloatFixPt( 2 ) );
        break;
       	case EEndAnchor:
           	lXTranslation = lXTranslation - lBbox.iWidth;
        break;
       	default:
       	 	//do nothing
       	break;
    }

    lTransTopLeftCorner.iX += lXTranslation;
    lTransTopLeftCorner.iY += lYTranslation;

    lTransTopRightCorner.iX += lXTranslation;
    lTransTopRightCorner.iY += lYTranslation;

    lTransBottomLeftCorner.iX += lXTranslation;
    lTransBottomLeftCorner.iY += lYTranslation;

    lTransBottomRightCorner.iX += lXTranslation;
    lTransBottomRightCorner.iY += lYTranslation;

    //at this point I have the exact path coordinates of the bbox
    aFourPointBbox.SetRectPoints(lTransTopLeftCorner, lTransTopRightCorner, lTransBottomLeftCorner, lTransBottomRightCorner);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    if((iText->Des()).Length()==0)
    	{
    		aBbox.iHeight=0;
    		aBbox.iWidth=0;
    		aBbox.iX=0;
    		aBbox.iY=0;
    		return;
    		
    	}
    //  If both bitmap font and SVG font are not present then 
    //  attempt to fetch SVG font. If that fails then 
    //  try obtaining the bitmap font
   if ( iSVGFont  )
    	{
        TSvgFourPointRect lFourPtRect;
        TGfxAffineTransform TM ;
        GetBBoxForSVGText(lFourPtRect,TM);
        
        lFourPtRect.GetTRect(aBbox);
        return;
        }
    
    else
    
      	{
      	
      	TRAPD( ErrCode , CreateSVGFontL());
      	if( ErrCode != KErrNotFound)
      	   {
            //first en cache the glymph and then 
            //calculate total text advance
            TSvgFourPointRect lFourPtRect;
            TGfxAffineTransform TM ;
        	GetBBoxForSVGText(lFourPtRect,TM);
            lFourPtRect.GetTRect(aBbox);
            return;
            }
       
       
       else
            {
            UpdateCurrentScaledFont();
            iBoundingBox.iWidth = iBitmapFont->TextWidthInPixels(iText->Des());
        	iBoundingBox.iHeight = iBitmapFont->FontMaxHeight();
      
        	      //unscaled for bitmap text...
            aBbox.iWidth = iBoundingBox.iWidth;
            TFloatFixPt ascent = Ascent();
                //aBbox.iHeight = ascent + Descent();

            aBbox.iHeight = iBoundingBox.iHeight;

            aBbox.iY = iPoint.iY - ascent;
            
            // X position is affected by anchor
		    switch( TextAnchor() )
		        {
		        case EStartAnchor:
		            aBbox.iX = iPoint.iX;
		            break;
		        case EMiddleAnchor:
		            aBbox.iX = iPoint.iX - ( aBbox.iWidth / TFloatFixPt( 2 ) );
		            break;
		        case EEndAnchor:
		            aBbox.iX = iPoint.iX - aBbox.iWidth;
		            break;
		        default:
		            aBbox.iX = iPoint.iX;
		            break;
		        }
		            
            } 	
      	 
      	}
      
      	
    }




void  CSvgTextElementImpl::SetRotateArray(CArrayFixFlat<TReal32>*& aRotate)
    {
     if ( iArrayRotate )
        {
        iArrayRotate->Reset();
        delete iArrayRotate;
        iArrayRotate= NULL;
        }

     iArrayRotate= aRotate;
/*   TInt lCount= aRotate->Count();
     iArrayRotate = new ( ELeave ) CArrayFixFlat<TReal32>( lCount );

     for (TInt i=0; i<lCount; i++)
        {
         iArrayRotate->AppendL((TReal32)aRotate->operator[](i));
        }
        */
    }

void CSvgTextElementImpl::SetXYArray(TUint16 aAtrId, CArrayFixFlat<TFloatFixPt>*& aX )
    {
    if (aAtrId== KAtrX)
        {
        if ( iArrayX )
            {
            iArrayX->Reset();
            delete iArrayX;
            iArrayX = NULL;
            }
        iArrayX= aX;
        SetX(aX);
        }
    else
        {
        if ( iArrayY )
            {
            iArrayY->Reset();
            delete iArrayY;
            iArrayY = NULL;
            }
        iArrayY= aX;
        SetY(aX);
        }
    }

TFloatFixPt CSvgTextElementImpl::FontSize()
{
    CCssValue* cssValue = NULL;
    FindProperty( KCSS_ATTR_FONTSIZE, cssValue, this );
    if ( cssValue )
    {
        TFloatFixPt fontsize = ((CFloatCssValueImpl*)cssValue)->Value();
        if ( fontsize  <= TFloatFixPt( 0 ) )
            fontsize = TFloatFixPt( 10 );
        return fontsize;
    }
    else
    {
    	return 10;
    }
}

TFloatFixPt CSvgTextElementImpl::TextAdvance( const TDesC& aText, TInt aIndex )
{
    if ( iBitmapFont )
    {
    	TInt advance = 0;
        UpdateCurrentScaledFont();
    	if ( iBitmapFont)
    	{
    		advance = ((CFbsFont*)iBitmapFont)->TextWidthInPixels(aText);

    		return TFloatFixPt(advance);
    	}

    	return 0;
    }
    else if ( iSVGFont )
    {
    	//if an svg font...
    	TReal32 fontHorzAdvX = 0.0;
    	TFloatFixPt advanceX = TFloatFixPt( 0 );

		TInt textLength = aText.Length();
		TInt glyphEleCnt = iGlyphElements.Count();
	   	for (TInt i= aIndex; i < (aIndex+textLength) && (i< glyphEleCnt); i++)
    	{
    		//assuming that glyph element cache is a parallel vector here
    		CSvgElementImpl* lGlyphElement = (CSvgElementImpl*)iGlyphElements[i];

        	if (lGlyphElement != NULL)
        	{
    			lGlyphElement->GetAttributeFloat( KAtrHorizAdvX, advanceX );
    			fontHorzAdvX += (TReal32)advanceX;
    		}
    	}

    	TFloatFixPt lUnitsPerEm = TFloatFixPt(1000);

		if (iFontFaceElement)
    	{
        	iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm, lUnitsPerEm);

 			if (lUnitsPerEm == TFloatFixPt(0))
 			{
        		lUnitsPerEm = TFloatFixPt(1000);
        	}
 		}

 		// Must use TReal to prevent overflow
        TReal32 retValue = fontHorzAdvX * (TReal32)FontSize() / (TReal32)lUnitsPerEm;
    	return (TFloatFixPt)retValue;
    }
    else
    {
    	//case where we dont have a bitmap font or an svg font
    	return 0;
    }
}



TFloatFixPt CSvgTextElementImpl::Ascent()
{
    
        //check for the bbox issue
        if ( iSVGFont )
        {
        //if an svg font the ascent is this...

        TFloatFixPt lAscent, lUnitsPerEm;
        TReal32 totalAscent = 0.0;

        if (iFontFaceElement)
            {
            iFontFaceElement->GetAttributeFloat( KAtrAscent,
                                                    lAscent );

            iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm, lUnitsPerEm);

            if(lUnitsPerEm==TFloatFixPt(0))
            	{
            	lUnitsPerEm=TFloatFixPt(1000);
            	}
            totalAscent = (TReal32)lAscent * (TReal32)FontSize() / (TReal32)lUnitsPerEm;

            return (TFloatFixPt)totalAscent;
            }

        //default value for ascent
        return 0;
        }
        else if ( iBitmapFont )
            {
          //  if ( iBitmapFont )
        
        UpdateCurrentScaledFont();
        //if a bitmap font the ascent is this....
        TInt ascent = 10;

        if (iBitmapFont)
            {
            //Tweak factor is needed as font not behaving correctly for
            // all languages.
            ascent = iBitmapFont->FontMaxAscent() + 1;            
            }
        else
            {
            }

        return ( ascent >= 0 ) ? ascent : -ascent;
        }
           
    else
        {
        //case where we dont have a bitmap font or svg font
        return 0;
        }
    }

const CFont* CSvgTextElementImpl::Font() const
    {
    return iBitmapFont;
    }
    
TFloatFixPt CSvgTextElementImpl::Descent()
{
	
    //check for the bbox issue
    if ( iSVGFont )
    {
    	//for svg fonts...
    	//if an svg font the descent is this...

    	TFloatFixPt lDescent, lUnitsPerEm;
    	TReal32 totalDescent = 0.0;

    	if (iFontFaceElement)
    	{
    		iFontFaceElement->GetAttributeFloat( KAtrDescent,
                                                    lDescent );

            iFontFaceElement->GetAttributeFloat( KAtrUnitsPerEm, lUnitsPerEm);

            if(lUnitsPerEm==TFloatFixPt(0))
            	{
            	lUnitsPerEm=TFloatFixPt(1000);
            	}
            
            totalDescent = (TReal32)lDescent * (TReal32)FontSize() / (TReal32)lUnitsPerEm;

            return ( totalDescent >= 0 ) ? (TFloatFixPt)totalDescent : (TFloatFixPt)-totalDescent;
    	}

    	return 0;
    }
    else if ( iBitmapFont )
        {
        UpdateCurrentScaledFont();
		//for bitmap fonts...
		TInt descent = 10;

		if (iBitmapFont)
		{
			descent = iBitmapFont->DescentInPixels();
		}
		else
		{
		}

		return ( descent >= 0 ) ? descent : -descent;

        }
    else
    {
    	//case where we dont have a bitmap font or svg font
    	return 0;
    }
}

TTextAnchor CSvgTextElementImpl::TextAnchor()
{

    TInt anchor = ((CIntCssValueImpl*)((*iSvgStyleProperties)[KCSS_ATTR_TEXTANCHOR]))->Value();
    return ( anchor == -1 ) ? EStartAnchor : (TTextAnchor)anchor;
}

TBool CSvgTextElementImpl::LoadExternalSVGFontL(const TDesC& aFontFamily)
{
    CSvgEngineImpl* lEngine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();
    CSvgDocumentImpl* lNewFontDoc = NULL;

    if (lEngine == NULL)
    {
    	return EFalse;
    }

    if ( lEngine->iFontHashMap->HasFontFamilyName(aFontFamily) )
    {
    	//already have that font
    	return EFalse;
    }
    else
    {
    	CSvgBitmapFontProvider *tempBitmapFontProvider = ((CSvgDocumentImpl*)iOwnerDocument)->GetBitmapFontProvider();
    	lNewFontDoc = CSvgDocumentImpl::NewL( tempBitmapFontProvider );
    	lNewFontDoc->SetEngine(lEngine);
    }

    if (aFontFamily.Length() > 0)
    {
    	TBuf<256> myFontUrl;
    	myFontUrl.Copy(aFontFamily.Left(24));
    	myFontUrl.Append(_L(".svg"));

    	if (FetchSvgFontL(myFontUrl, lNewFontDoc))
    	{
    		if ( lEngine->iFontHashMap->AddFontDocument(lNewFontDoc, aFontFamily) )
    		{
    			return ETrue;
    		}
    	}
    }

    //If basic latin use SVG DEFAULT FONT
    if ( iUseDefaultSVGFont && !lEngine->iFontHashMap->HasFontFamilyName(aFontFamily))
    {
    	if (FetchSvgFontL(_L("defaultsvgfont.svg"), lNewFontDoc))
    	{
          TBool aReturn =	lEngine->iFontHashMap->AddFontDocument(lNewFontDoc, KDefaultFont );

    		//this will overwrite whatever font family the element had set...
    		//remove this to let fonts fall through to bitmap their specified bitmap fonts
    		//SetFontFamilyL(_L("NokiaSansWide"));
    		if(!aReturn)
    		    {
	            delete lNewFontDoc;
    		    }
    		return ETrue;
    	}
    	/*else if (LoadDefaultSvgFont(iNewFontDoc))
    	{
    		if (iEngine->AddFontDocument(iNewFontDoc, aFontFamily))
    		{
    			return ETrue;
    		}
    	}*/
    }

    delete lNewFontDoc;
    return EFalse;
}

TBool CSvgTextElementImpl::IsEditable()
{
 	return iEditable;
}

TBool CSvgTextElementImpl::FetchSvgFontL(const TDesC& aUri, CSvgDocumentImpl* newFontDocument)
{
		CSvgErrorImpl* lmyError = CSvgErrorImpl::NewL();

    	//do a fetchimage style fetch of the SVG font file here...
    	// Connect session
        RFs lSession;
        RFile fileHandle;
        if ( lSession.Connect() == KErrNone )
        {
        	CSvgEngineImpl* lEngine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();

        	if (lEngine->FetchFont( aUri, lSession, fileHandle))
        	{
        		TBuf<512> aName;

				fileHandle.Name(aName);

        		if ( aName.CompareF(aUri) == 0)
        		{
        			//need to attach this to the real document and just once for all text elements
	        		newFontDocument->Load( fileHandle, *lmyError );
				}
				else
				{
					lSession.Close();
        			delete lmyError;
        			return EFalse;
				}

        		if (lmyError->ErrorCode() == ESvgNoError)
        		{
        			lSession.Close();
        			delete lmyError;
        			return ETrue;
        		}
        	}
        	else
        	{
        		lSession.Close();
        	}
        }

    	delete lmyError;
        return EFalse;
}


/*TBool CSvgTextElementImpl::LoadDefaultSvgFont(CSvgDocumentImpl* newFontDocument)
{
		CSvgErrorImpl* myError = CSvgErrorImpl::NewL();
		CSvgEngineImpl* iEngine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();

   		TPtrC8 svgfontdata( (TUint8*)strSVGFont, User::StringLength((TUint8*)strSVGFont) );
   		iEngine->newFontDocument->Load( svgfontdata, *myError );

   		if (myError->ErrorCode() != ESvgNoError)
   		{
   			delete myError;
   			return EFalse;
   		}

   		delete myError;
   		return ETrue;
}*/

/*** FROM MSvgMouseListener ***/
// ---------------------------------------------------------------------------
// mouse entered
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyTextEntered(this);
			return ETrue;
		}
    }

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer exits a visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyTextExited(this);
			return ETrue;
		}
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is moved inside a visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::MouseMoved( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                       TInt /*aX*/, TInt /*aY*/ )
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::MousePressed( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is released on on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgTextElementImpl::MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	TInt eleCnt = aElements.Count();
	for (TInt i = 0; i < eleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyTextActivated(this);
			return ETrue;
		}
  }

  return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgTextElementImpl::SetEditable( const TDesC& aValue )
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

void CSvgTextElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	RDebug::Printf("<text x=\"%d\" y=\"%d\" editable=\"%d\" >hmm</text>", (int)iPoint.iX, (int)iPoint.iY, (int)IsEditable()/*, iText*/);
	#endif
	}
}

void CSvgTextElementImpl::UpdateCurrentScaledFont()
    {

    // Get latest scaling factor
    TFloatFixPt scale = GetCurrentScale();

    TFloatFixPt fontsize = (TFloatFixPt)FontSize();

    TFloatFixPt fontHeight = scale * fontsize;
    // RDebug::Printf("Requesting fontHeight: %d\n", (TInt)fontHeight);

    GetScaledFont( fontHeight, (*iFamilies)[0], iBitmapFont, iBitmapFontSpec );
    // RDebug::Printf("Ascent : %d\n", (TInt)iBitmapFont->AscentInPixels());
    // RDebug::Printf("Descent: %d\n", (TInt)iBitmapFont->DescentInPixels());
    }
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::GetSystemFontScaled(TFloatFixPt aHeight,
//                                     const TDesC& aTypefaceName,//                                     CFont*& aFont,
//                                     TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
 void CSvgTextElementImpl::GetScaledFont( TFloatFixPt aHeight,
                                          const TDesC& aTypefaceName,
                                          CFont*& aFont,
                                          TFontSpec& aFontSpec )
    {
    if (aHeight >= TFloatFixPt(300))
        {
        aHeight = TFloatFixPt(300);
        }

    // Calculate twip size
    TInt twipsSize;
    if( aHeight > TFloatFixPt(0) )
        {
        TInt xpixels,ypixels,xtwips,ytwips;
        HAL::Get(HALData::EDisplayXPixels, xpixels);
        HAL::Get(HALData::EDisplayYPixels, ypixels);
        HAL::Get(HALData::EDisplayXTwips, xtwips);
        HAL::Get(HALData::EDisplayYTwips, ytwips);

        if ( xpixels == 0 )
            {
            return;
            }

        TFloatFixPt ratio = TFloatFixPt(xtwips)/(TFloatFixPt)xpixels;
        twipsSize = (TInt) (ratio * aHeight);//((float)ytwips/ypixel) is same
        }
    else
        {
        return;
        }

    TFontStrokeWeight strokeWeight = EStrokeWeightNormal;
    TFontPosture fontPosture = EPostureUpright ;

    // Get font-weight
    TInt32 fontweight = 1;
    CCssValue* weightValue = NULL;
    FindProperty( KCSS_ATTR_FONTWEIGHT, weightValue, this );

    if ( weightValue )
        {
        fontweight = ((CIntCssValueImpl*)weightValue)->Value();
        }

    if ( ( fontweight == 1 ) ||
         ( fontweight == 2 ) ||
         ( fontweight == 9 ) ||
         ( fontweight == 10 ) ||
         ( fontweight == 11 ) ||
         ( fontweight == 12 ) ) // Bold
        {
        strokeWeight = EStrokeWeightBold;
        }
    else
        {
        strokeWeight = EStrokeWeightNormal;
        }

    // Get font-style
    TInt32 style = 1;
    CCssValue* styleValue = NULL;
    FindProperty( KCSS_ATTR_FONTSTYLE, styleValue, this );

    if ( styleValue )
        {
        style = ((CIntCssValueImpl*)styleValue)->Value();
        }

    if ( style == 1 )
        {
        //Italic
        fontPosture = EPostureItalic;
        }
    else
        {
        fontPosture = EPostureUpright;
        }


    TFontStyle fontStyle( fontPosture, strokeWeight, EPrintPosNormal );

    // Get Latest required Font Spec.
    TFontSpec tempFontSpec(aFontSpec);
    tempFontSpec.iTypeface.iName = aTypefaceName.Left(24);
    tempFontSpec.iHeight = twipsSize;
    tempFontSpec.iFontStyle = fontStyle;

    CSvgBitmapFontProvider *tempBitmapFontProvider = ((CSvgDocumentImpl*)iOwnerDocument)->GetBitmapFontProvider();
    if( aFont )
        {
        if(!(tempFontSpec == aFontSpec))
            {
            aFontSpec = tempFontSpec;
            tempBitmapFontProvider->ReleaseFont(aFont);
            tempBitmapFontProvider->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
            }
        }
        else
        {
        aFontSpec = tempFontSpec;
        tempBitmapFontProvider->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
        }

    }
// ---------------------------------------------------------------------------
    // Calculate the exact total text advance instead of taking default per glyph
    // advance which will approximate the value.
// ---------------------------------------------------------------------------
    TFloatFixPt CSvgTextElementImpl::GetTotalTextAdvance(const TFloatFixPt& alK,const TFloatFixPt& alUnitsPerEmInverse,const TFloatFixPt& alFontSize,const TFloatFixPt& alMissingGlyphHorzAdvX,const TFloatFixPt& alFontHorzAdvX)
    {
		//Text Anchor related.
        //lFontHorzAdvX = 400;
        //lTotalTextAdvance = lFontHorzAdvX * (TFloatFixPt)iText->Length() * lUnitsPerEmInverse * lFontSize;
        
        
        TFloatFixPt lGlyphHorzAdvX;
        TInt langCodeFound( 0 );
        TInt8   lTextAnchor =(TInt8)(((CIntCssValueImpl *)(iSvgStyleProperties->operator[](KCSS_ATTR_TEXTANCHOR)))->Value());
        TPtrC16 lPrevGlyphName;
        TPtrC16 lCurrGlyphName;
        TFloatFixPt lK = alK; //kerning purpose....
        TFloatFixPt KZero;
        TFloatFixPt lTotalTextAdvance ( KZero );
        TPtrC16 lUnicode;
        TPtrC16 lLangCode; //ex: en-US or en-GB or ja-JP or zh-CN or en....
        TPtrC16 lPrevUnicode;
        TPtrC16 lUnicodeRange; //Max.Length "U+00AB-00AF"
        TFloatFixPt lUnitsPerEmInverse = alUnitsPerEmInverse;
        TFloatFixPt lFontSize =  alFontSize;
        TFloatFixPt lFontHorzOrgX( 0 );
        TFloatFixPt lFontHorzAdvX = alFontHorzAdvX;
        TFloatFixPt lMissingGlyphHorzAdvX = alMissingGlyphHorzAdvX;
    		
            
            TInt glyphEleCnt = iGlyphElements.Count();
            for (TInt i=0; i < glyphEleCnt; i++)
            {
            	CSvgElementImpl* lGlyphElement = (CSvgElementImpl*)iGlyphElements[i];

            	if (lGlyphElement != NULL)
            	{
            		//have a valid glyph
            		TInt checkErrorUnicode = lGlyphElement->GetAttributeDes( KAtrUnicode,
                                                                               lUnicode );
                    //check the glyph name
                    TInt checkErrorGlyphName = lGlyphElement->GetAttributeDes( KAtrGlyphName,
                                                                               lCurrGlyphName );
                  	if( checkErrorGlyphName == KErrNoAttribute )
                  	{
                  		lCurrGlyphName.Set( KGlyphNameNone );
                  	}

                    //check its language
                    TInt checkErrorLangCode = lGlyphElement->GetAttributeDes( KAtrLang,
                                                                                lLangCode );
                    TPtrC lXmlLangAttr(this->XMLLang());

                    if( checkErrorLangCode != KErrNoAttribute ) //if Lang code present
                    {
                      	langCodeFound = lLangCode.CompareF( lXmlLangAttr );//compare xml:lang with Lang code.
                    }

                    //if Lang code is matching && unicode attr. present, we are good.
                    if ( (checkErrorUnicode != KErrNoAttribute) && (langCodeFound == 0) )
                    {
                      	//Checking and Introducing kerning(adjusting spacing).
                        if( lPrevGlyphName.Length() > 0 )
                        {
                             if( IsKerningRequired( lPrevGlyphName, lCurrGlyphName, lPrevUnicode, lUnicode ))
                             {
                                  lTotalTextAdvance -= ( lK * lUnitsPerEmInverse ) * lFontSize;
                             }
                        }

                        lGlyphElement->GetAttributeFloat( KAtrHorizAdvX,
                                                           lGlyphHorzAdvX );

    	#ifdef SVG_FLOAT_BUILD
                        if ( lGlyphHorzAdvX == TFloatFixPt( 0 ) )
    	#else
                        if ( lGlyphHorzAdvX == TFloatFixPt( 0,ETrue ) )
    	#endif
                        {
                          	lGlyphHorzAdvX = /*(TFloatFixPt)lUnicodeLength **/ lFontHorzAdvX;
                        }

                       
                            lTotalTextAdvance += ( lGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;
                          

                     }
                    

            	}
            	else
            	{
            		//need to use missing glyph for this one...
            		
                     lTotalTextAdvance += ( lMissingGlyphHorzAdvX * lUnitsPerEmInverse ) * lFontSize;
                    
                }
            }//end-for

            
            return lTotalTextAdvance ;

    }// End Function    
void CSvgTextElementImpl::CreateSVGFontL()
{
    CCssValue* lCssValue = NULL;
    
    TPtrC lFontFamily;
    //Getting font-family property
    this->FindProperty( KCSS_ATTR_FONTFAMILY, lCssValue,this );
    if ( lCssValue && ((( CStrCssValueImpl * ) lCssValue )->Value()).Length() != 0)
        {
        lFontFamily.Set( ( ( CStrCssValueImpl * ) lCssValue )->Value() );
        SetFontFamilyL( lFontFamily );
        }

		//attempt to load svg font files of same font family name
		//this boolean will be a problem if we want to be able to swap
		//font-family names on the fly and use svg fonts.
  	if (!iTriedLoadingSVGFonts)
   	    {
    		LoadExternalSVGFontL(lFontFamily);
    		iTriedLoadingSVGFonts = ETrue;
   	    }

    //Retrieving Font Element Pointer from the Font Table
    //if font in svg document
    iSVGFont = NULL;
    if ( iFamilies != NULL )
        {
    		TInt familiesCnt = iFamilies->Count();
    		for (int i=0; i< familiesCnt; i++)
            {
            // In JSR-226 case, the engine may not have been created yet.            
            if ( ((CSvgDocumentImpl*)iOwnerDocument)->Engine() )
            	{
            	iSVGFont = ( CSvgElementImpl * )((CSvgDocumentImpl*)iOwnerDocument)->Engine()->iFontHashMap->GetFontPtr(iFamilies->operator[]( i ));
            	}

			if (iSVGFont == NULL)
				{
				iSVGFont = ( CSvgElementImpl * )((CSvgDocumentImpl*)iOwnerDocument)->iFontHashMap->GetFontPtr(iFamilies->operator[]( i ) );
				}
				
	        if (iSVGFont != NULL)
  	      		{
    	    	//found a font with that family name...continue
      	  		break;
        		}
    		}
    	}

/**********************************************************/
/* Checking for pre-defined SVG Fonts through the         */
/* retrieved pointer from the Font Table.                 */
/* IF the Font is found (ptr!=NULL), SVG Fonts are used   */
/* ELSE Bitmap Font is used through DrawString() method   */
/**********************************************************/

	if(!iSVGFont)
		{
		User::Leave(KErrNotFound);
		}

    if ( iSVGFont != NULL && iSVGFont->HasChildNodes())
        {
        //we found an svg font and are using it...
        //	iSVGFont->GetAttributeFloat( KAtrHorizOriginX, lFontHorzOrgX );
        //	iSVGFont->GetAttributeFloat( KAtrHorizAdvX, lFontHorzAdvX );
        //	lMissingGlyphHorzAdvX = lFontHorzAdvX; //Default Value
        //	TInt checkErrShapeMG = -1;//To keep a value other than KErrNone. Should this be replaced
       	CacheGlyphsForText();
        }
        
   
    
    
    

}

