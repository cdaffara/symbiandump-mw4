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
#include "SVGFontFaceElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGEngineImpl.h"

#include "GfxAffineTransform.h"
#include "SVGFontHashMap.h"

#ifdef SVG_FONTS_INCLUDE

_LIT( SPACE, " " );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontFaceElementImpl* CSvgFontFaceElementImpl::NewL(  const TUint8 aElemID,
                                                        CSvgDocumentImpl* aDoc )
    {
    CSvgFontFaceElementImpl*self    = new ( ELeave )
                                      CSvgFontFaceElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(   aElemID);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontFaceElementImpl* CSvgFontFaceElementImpl::NewLC(  const TUint8 aElemID,
                                                     CSvgDocumentImpl* aDoc )
    {
    CSvgFontFaceElementImpl*self    = new ( ELeave )
                                      CSvgFontFaceElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(   aElemID);

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontFaceElementImpl::ConstructL(  const TUint8 aElemID)
    {
    CSvgElementImpl::InitializeL( aElemID);
    iFontFamily = HBufC::NewL( 2 );
    *iFontFamily = SPACE;

    iUnicodeRange = HBufC::NewL( 2 );
    *iUnicodeRange = SPACE;

	#ifdef SVG_FLOAT_BUILD	
    iUnitsPerEm = TFloatFixPt( 0 );
    iAlphabetic = TFloatFixPt( 0 );

    iAscent = TFloatFixPt( 0 );
    iDescent = TFloatFixPt( 0 );

    iUnderlinePosition = TFloatFixPt( 0 );
    iUnderlineThickness = TFloatFixPt( 0 );
    iOverlinePosition = TFloatFixPt( 0 );
    iOverlineThickness = TFloatFixPt( 0 );
    iStrikethroughPosition = TFloatFixPt( 0 );
    iStrikethroughThickness = TFloatFixPt( 0 );
	#else
    iUnitsPerEm = TFloatFixPt( 0 ,ETrue );
    iAlphabetic = TFloatFixPt( 0 ,ETrue);

    iAscent = TFloatFixPt( 0 ,ETrue );
    iDescent = TFloatFixPt( 0 ,ETrue );

    iUnderlinePosition = TFloatFixPt( 0 ,ETrue );
    iUnderlineThickness = TFloatFixPt( 0 ,ETrue );
    iOverlinePosition = TFloatFixPt( 0 ,ETrue );
    iOverlineThickness = TFloatFixPt( 0 ,ETrue );
    iStrikethroughPosition = TFloatFixPt( 0 ,ETrue );
    iStrikethroughThickness = TFloatFixPt( 0 ,ETrue );
	#endif

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);

	// This is required to allocate buffer up to granularity so that the
    // following Insert calls cannot leave.

    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontFaceElementImpl::~CSvgFontFaceElementImpl()
    {
    //AJD this whole thing needs to be replaced with some sort of external resource registry area....
    //for images, fonts, other svg files
    CSvgDocumentImpl * ownerDoc = (CSvgDocumentImpl*)iOwnerDocument;
    if ( ownerDoc )
        {
        if ( ownerDoc->Engine() && ownerDoc->Engine()->iFontHashMap)
            {
            TRAPD(err, ownerDoc->Engine()->iFontHashMap->RemoveReferenceFromFontL(iFontFamily->Des()));
            if (err)
                {
                #ifdef _DEBUG
                RDebug::Printf("CSvgFontFaceElementImpl::~CSvgFontFaceElementImpl: Error trapped=%d", err);
                #endif
                }
            }
        
        if ( ownerDoc->iFontHashMap)
            {
            TRAPD(err, ownerDoc->iFontHashMap->RemoveReferenceFromFontL(iFontFamily->Des()));
            if (err)
                {
                #ifdef _DEBUG
                RDebug::Printf("CSvgFontFaceElementImpl::~CSvgFontFaceElementImpl: Error trapped=%d", err);
                #endif
                }
            }
        }
    if ( iFontFamily )
        {
        delete iFontFamily;
        iFontFamily = NULL;
        }

    if ( iUnicodeRange )
        {
        delete iUnicodeRange;
        iUnicodeRange = NULL;
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
const TDesC* CSvgFontFaceElementImpl::GetFontFamily( )
    {
     return iFontFamily;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontFaceElementImpl::GetAscent()
    {
    return iAscent;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontFaceElementImpl::GetDescent()
    {
    return iDescent;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontFaceElementImpl::GetUnitsPerEm()
    {
    return iUnitsPerEm;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontFaceElementImpl::GetAlphabetic()
    {
    return iAlphabetic;
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontFaceElementImpl::SetFontFamilyL( const TDesC& aFontFamily )
    {
    if ( iFontFamily )
        {
        delete iFontFamily;
        iFontFamily = NULL;
        }
    iFontFamily = aFontFamily.AllocL();
    
    if(iOwnerDocument)
   		{
   		//FOR JSR226 we have to store at document level
		//since it doesnt have an engine at parsing time
   		((CSvgDocumentImpl*)iOwnerDocument)->iFontHashMap->AddFontL((CSvgFontElementImpl*) ParentNode(), *iFontFamily);	
   		
   		//Get the parent element(FontElement) and store it in the FontMap Table.
   		CSvgEngineImpl* lEngine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();
   		if (lEngine)
   			{
   			lEngine->iFontHashMap->AddFontL((CSvgFontElementImpl*)ParentNode(), *iFontFamily);
			}
		
   		}
    }
// --------------------------------------------------------------------------
// CSvgFontFaceElementImpl::RemoveFontFamily
// Remove the font-family from the hash map 
// --------------------------------------------------------------------------  
void CSvgFontFaceElementImpl::RemoveFontFamily(const TDesC& aFontFamily)
   	{
   	
   	if(iOwnerDocument)
   		{
   		
   		((CSvgDocumentImpl*)iOwnerDocument)->iFontHashMap->RemoveReferenceFromFontL(aFontFamily);	
   		
   		CSvgEngineImpl* lEngine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();
   		if (lEngine)
   			{
   			lEngine->iFontHashMap->RemoveReferenceFromFontL(aFontFamily);
			}
		
   		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontFaceElementImpl::SetUnicodeRangeL( const TDesC& aUnicodeRange )
    {
    if ( iUnicodeRange )
        {
        delete iUnicodeRange;
        iUnicodeRange = NULL;
        }
    iUnicodeRange = aUnicodeRange.AllocL();
    }



// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontFaceElementImpl::SetAttributeL( const TDesC& aName,
                                             const TDesC& aValue )
    {
    CSvgElementImpl::SetAttributeL(aName,aValue);
    _LIT( KUnicodeRange, "unicode-range" );
    if ( aName == KUnicodeRange )
        {
        SetUnicodeRangeL( aValue );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontFaceElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrAscent:                iAscent = aValue;
                                        break;
        case KAtrDescent:               iDescent = aValue;
                                        break;
        case KAtrAlphabetic:            iAlphabetic = aValue;
                                        break;
        case KAtrUnderlinePosition:     iUnderlinePosition = aValue;
                                        break;
        case KAtrUnderlineThickness:    iUnderlineThickness = aValue;
                                        break;
        case KAtrOverlinePosition:      iOverlinePosition = aValue;
                                        break;
        case KAtrOverlineThickness:     iOverlineThickness = aValue;
                                        break;
        case KAtrStrikethroughPosition: iStrikethroughPosition = aValue;
                                        break;
        case KAtrStrikethroughThickness: iStrikethroughThickness = aValue;
                                        break;
        case KAtrUnitsPerEm:            iUnitsPerEm = aValue;
                                        break;
        default:
            return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontFaceElementImpl::GetAttributeFloat( const TInt aNameId,
                                                 TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrUnitsPerEm:
        aValue = GetUnitsPerEm();
        break;
        case KAtrAscent:
        aValue = GetAscent();
        break;
        case KAtrDescent:
        aValue = GetDescent();
        break;
        case KAtrAlphabetic:
        aValue = GetAlphabetic();
        break;
        case KAtrUnderlinePosition:
        aValue = iUnderlinePosition;
        break;
        case KAtrUnderlineThickness:
        aValue = iUnderlineThickness;
        break;
        case KAtrOverlinePosition:
        aValue = iOverlinePosition;
        break;
        case KAtrOverlineThickness:
        aValue = iOverlineThickness;
        break;
        case KAtrStrikethroughPosition:
        aValue = iStrikethroughPosition;
        break;
        case KAtrStrikethroughThickness:
        aValue = iStrikethroughThickness;
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontFaceElementImpl::GetAttributeDes( const TInt aNameId,
                                               TPtrC16& aValue )
    {
    switch ( aNameId )
        {
        case KAtrUnicodeRange:
            {
            if ( ( iUnicodeRange->Length() > 2 ) )
                {
                aValue.Set( *iUnicodeRange );
                break;
                }
            else
                return KErrNoAttribute;
            }
        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgFontFaceElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgFontFaceElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                      CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgFontFaceElementImpl::CSvgFontFaceElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);
    }

void CSvgFontFaceElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<font-face font-family=\"hmm\" alphabetic=\"%d\" underline-position=\"%d\" underline-thickness=\"%d\" overline-position=\"%d\" overline-thickness=\"%d\" strikethrough-position=\"%d\" strikethrough-thickness=\"%d\" unicode-range=\"hmm\" units-per-em=\"%d\" ascent=\"%d\" descent=\"%d\"\\>",
		 /*iFontFamily,*/ (int)iAlphabetic, (int)iUnderlinePosition, (int)iUnderlineThickness, (int)iOverlinePosition, (int)iOverlineThickness, (int)iStrikethroughPosition, (int)iStrikethroughThickness, /*iUnicodeRange->Des(),*/ (int)iUnitsPerEm, (int)iAscent, (int)iDescent);
		#endif
	}
}

#endif      //ifdef SVG_FONTS_INCLUDE
