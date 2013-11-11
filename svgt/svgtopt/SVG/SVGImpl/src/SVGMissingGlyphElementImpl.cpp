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
#include "SVGMissingGlyphElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGPathDataParser.h"

#include "GfxAffineTransform.h"

#ifdef SVG_FONTS_INCLUDE

_LIT( D, "d" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgMissingGlyphElementImpl* CSvgMissingGlyphElementImpl::NewL(  const TUint8 aElemID,
																CSvgDocumentImpl* aDoc )
	{
	CSvgMissingGlyphElementImpl*self    = new ( ELeave )
										  CSvgMissingGlyphElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL( aElemID);
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgMissingGlyphElementImpl* CSvgMissingGlyphElementImpl::NewLC(  const TUint8 aElemID,
																 CSvgDocumentImpl* aDoc )
	{
	CSvgMissingGlyphElementImpl*self    = new ( ELeave )
										  CSvgMissingGlyphElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID);

	return self;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMissingGlyphElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgElementImpl::InitializeL(  aElemID);

	iShape = CGfxGeneralPath::NewL();
	#ifdef SVG_FLOAT_BUILD	
	iHorzAdvX = TFloatFixPt( 0 );
	#else
	iHorzAdvX = TFloatFixPt( 0, ETrue );
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
CSvgMissingGlyphElementImpl::~CSvgMissingGlyphElementImpl()
    {
    if(iShape)
		{
		delete iShape;
		iShape = NULL;
		}
	if(iSvgStyleProperties)
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
TFloatFixPt CSvgMissingGlyphElementImpl::GetHorzAdvX()
    {
    return iHorzAdvX;
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgMissingGlyphElementImpl::SetPath( const TDesC& aPath )
    {
	TSvgPathDataParser::ParsePathData( aPath, iShape );
    }


// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMissingGlyphElementImpl::SetAttributeL( const TDesC& aName,
                                                 const TDesC& aValue )
    {
	CSvgElementImpl::SetAttributeL(aName,aValue);

	if (aName == D )
		{
         SetPath( aValue );
        }
    return KErrNone;
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMissingGlyphElementImpl::GetAttributeFloat( const TInt aNameId,
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

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMissingGlyphElementImpl::GetAttributePath( const TInt aNameId,
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

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMissingGlyphElementImpl::SetAttributePathL( const TInt aNameId,
                                                     CGfxGeneralPath* /* aValue */ )
    {
	if ( aNameId != KAtrData )
		return KErrNoAttribute;


    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMissingGlyphElementImpl::SetAttributePathRef( const TInt aNameId,
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


// *******************************************************
// From CSvgElementImpl
// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

MXmlElement* CSvgMissingGlyphElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgMissingGlyphElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                          CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgMissingGlyphElementImpl::CSvgMissingGlyphElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }

	TInt CSvgMissingGlyphElementImpl::SetAttributeFloatL( const TInt aNameId,
                                                      const TFloatFixPt aValue )
	{
		if( aNameId == KAtrHorizAdvX )
			{
			iHorzAdvX = aValue;
			}
		else
			{
			return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
			}
		return KErrNone;
	}

CGfxGeneralPath* CSvgMissingGlyphElementImpl::GetPathAttribute(TInt aAttributeId)
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
void CSvgMissingGlyphElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{
	// i think it will be better if it is  deleted here.
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

void CSvgMissingGlyphElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	RDebug::Printf("<missing-glyph horiz-adv-x=\"%d\" >", (int)iHorzAdvX);

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

	RDebug::Printf("</missing-glyph>", this);
	#endif
	}
}
#endif     //ifdef SVG_FONTS_INCLUDE
