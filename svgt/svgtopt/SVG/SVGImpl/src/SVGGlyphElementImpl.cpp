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
#include "SVGGlyphElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGPathDataParser.h"
#include "SVGPathElementImpl.h"

#include "GfxAffineTransform.h"

#ifdef SVG_FONTS_INCLUDE

_LIT( SPACE, " " );
_LIT( D, "d" );

// ==========================================================================
// Need method description
// ==========================================================================
CSvgGlyphElementImpl* CSvgGlyphElementImpl::NewL(  const TUint8 aElemID,
												  CSvgDocumentImpl* aDoc )
	{
	CSvgGlyphElementImpl*   self    = new ( ELeave )
									  CSvgGlyphElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(   aElemID);
	CleanupStack::Pop();

	return self;
	}

// ==========================================================================
// Need method description
// ==========================================================================
CSvgGlyphElementImpl* CSvgGlyphElementImpl::NewLC(  const TUint8 aElemID,
												   CSvgDocumentImpl* aDoc )
	{
	CSvgGlyphElementImpl*   self    = new ( ELeave )
									  CSvgGlyphElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(   aElemID);

	return self;
	}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgGlyphElementImpl::ConstructL(  const TUint8 aElemID )
	{

	CSvgElementImpl::InitializeL( aElemID);

	iOwnedPath = EFalse;
	iUnicode = HBufC::NewL( 2 );
	*iUnicode = SPACE;
	iGlyphName = HBufC::NewL( 0 );
	iLangCode = HBufC::NewL( 0 );
	iShape = CGfxGeneralPath::NewL();
	#ifdef SVG_FLOAT_BUILD
  iHorzAdvX = TFloatFixPt( 0 );
	#else
    iHorzAdvX = TFloatFixPt( 0 , ETrue);
	#endif

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
iSvgStyleProperties->Remove( 0 );


	}






// ==========================================================================
// Need method description
// ==========================================================================

CSvgGlyphElementImpl::CSvgGlyphElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }


// ==========================================================================
// Need method description
// ==========================================================================
CSvgGlyphElementImpl::~CSvgGlyphElementImpl()
    {
	if ( iUnicode )
		{
		delete iUnicode;
		iUnicode = NULL;
		}

	if ( iGlyphName )
		{
		delete iGlyphName;
		iGlyphName =  NULL;
		}

	if ( iLangCode )
		{
		delete iLangCode;
		iLangCode = NULL;
		}

	if ( iShape )
		{
		CSvgElementImpl* aChild = (CSvgElementImpl*)FirstChild();
		if (!(aChild && aChild->ElemID() == KSvgPathElement))
			{
			delete iShape;
			iShape = NULL;
			}
		else if ( iOwnedPath )
			{
			delete iShape;
			}
		iShape = NULL;
		}

	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}

    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgGlyphElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iShape->GetBounds( GetCTM(), aBbox );
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgGlyphElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iShape->GetBounds( identityTx, aBbox );
    }

// *******************************************************
// From SVG DOM

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgGlyphElementImpl::GetUnicode( TDes& aUnicode )
    {
    aUnicode = iUnicode->Des();
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgGlyphElementImpl::GetHorzAdvX()
    {
    return iHorzAdvX;
    }


// *******************************************************
// SVG Implementation




// ==========================================================================
// Need method description
// ==========================================================================
void CSvgGlyphElementImpl::SetPath( const TDesC& aPath )
    {
	TSvgPathDataParser::ParsePathData( aPath, iShape );
    }


// *******************************************************
// From MXmlElementOpt

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::GetAttributeFloat( const TInt aNameId,
                                               TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrRefX:
        aValue = iShape->PointCoordsArray()[0];
        break;
        case KAtrRefY:
        aValue = iShape->PointCoordsArray()[1];
        break;
        case KAtrHorizAdvX:
        aValue = GetHorzAdvX();
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::GetAttributePath( const TInt aNameId,
                                             CGfxGeneralPath*& aValue )
    {
    if ( aNameId == KAtrData )
        {
        aValue = iShape;
        }
    else
        {
        aValue = NULL;
        return KErrNoAttribute;
        }

    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::SetAttributePathL( const TInt aNameId,
                                              CGfxGeneralPath* /* aValue */ )
    {
    if ( aNameId == KAtrData )
        {
        }
    else
        {
        return KErrNoAttribute;
        }

    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::SetAttributePathRef( const TInt aNameId,
                                              CGfxGeneralPath*&  aValue  )
    {
    if ( aNameId == KAtrData )
        {
		if(iShape)
			{
			delete iShape;
			iShape= NULL;
			}
		iShape= aValue;

        }
    else
        {
        return KErrNoAttribute;
        }

    return KErrNone;
    }
// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    switch ( aNameId )
        {
        case KAtrUnicode:
        if ( iUnicode->Length() > 0 )
            {
            aValue.Set( *iUnicode );
            break;
            }
        else
            return KErrNoAttribute;

        case KAtrGlyphName:
        if ( iGlyphName->Length() > 0 )
            {
            aValue.Set( *iGlyphName );
            break;
            }
        else
            return KErrNoAttribute;

        case KAtrLang:
        if ( iLangCode->Length() > 0 )
            {
            aValue.Set( *iLangCode );
            break;
            }
        else
            return KErrNoAttribute;

        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }

// *******************************************************
// From MXmlElement


// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
	CSvgElementImpl::SetAttributeL(aName,aValue);
	if( aName == D )
		{
		SetPath (aValue);
		iOwnedPath = ETrue;
		}
    return KErrNone;
    }


// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgGlyphElementImpl::SetAttributeFloatL( const TInt aNameId,
                                               const TFloatFixPt aValue )
	{
		if( aNameId == KAtrHorizAdvX )
			{
			iHorzAdvX = aValue;
			}
		else return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
    return KErrNone;
	}

// ==========================================================================
// Need method description
// ==========================================================================
TInt  CSvgGlyphElementImpl::SetAttributeDesL( const TInt aNameId, const TDesC& aValue )
{
	switch (aNameId)
		{
		case KAtrUnicode:
						delete iUnicode;
						iUnicode = NULL;
						iUnicode = aValue.AllocL();
						//iUnicode->Des().ZeroTerminate();
						break;
		case KAtrGlyphName:
						delete iGlyphName;
						iGlyphName = NULL;
						iGlyphName = aValue.AllocL();
						break;
		case KAtrLang:
						delete iLangCode;
						iLangCode = NULL;
						iLangCode = aValue.AllocL();
						break;

		default:		return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );

		}
	return KErrNone;
}


// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgGlyphElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TBool CSvgGlyphElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                   CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private

CGfxGeneralPath* CSvgGlyphElementImpl::GetPathAttribute(TInt aAttributeId)
	{
	if(aAttributeId == KAtrD)
		{
		return iShape;
		}
	else
		{
		return NULL;
		}
	}
void CSvgGlyphElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{
	if(aAttributeId == KAtrD)
		{
		if(iShape == aPathHandle)
			{
			return;
			}
		delete iShape;
		iShape = aPathHandle;
		}
}

void CSvgGlyphElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	if (iGlyphName)
	{
	RDebug::Printf("<glyph name=\"%s\" unicode=\"%d\" lang=\"%d\" horiz-adv-x=\"%d\" >",
	iGlyphName, (int)iUnicode, (int)iLangCode, (int)iHorzAdvX);

	if (iShape)
	{
		RArray<TUint32>* lTypes = iShape->PointTypeArray();
		RArray<TFloatFixPt>* lCoords = iShape->PointCoordsArrayAll();

		if (lTypes->Count() != lCoords->Count())
		{
			RDebug::Printf("Invalid path points and types didnt match", this);
			return;
		}

		RDebug::Printf("<path d=\"", this);

		TInt lTypesCnt = lTypes->Count();
		for (TInt i=0; i < lTypesCnt; i++)
		{
			RDebug::Printf("%c ", lTypes->operator[]( i ));
			RDebug::Printf("%d ", (int)(lCoords->operator[]( i )));
		}

		RDebug::Printf(" z\">", this);
	}

	RDebug::Printf("</glyph>", this);
	}
	#endif
	}
}
#endif    //ifdef SVG_FONTS_INCLUDE
