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
#include "SVGFontElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"

#ifdef SVG_FONTS_INCLUDE


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontElementImpl* CSvgFontElementImpl::NewL(  const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )
    {
    CSvgFontElementImpl*self    = new ( ELeave ) CSvgFontElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(aElemID);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontElementImpl* CSvgFontElementImpl::NewLC(  const TUint8 aElemID,
                                                 CSvgDocumentImpl* aDoc )
    {
    CSvgFontElementImpl*self    = new ( ELeave ) CSvgFontElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(aElemID);

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgFontElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL(  aElemID );


	#ifdef SVG_FLOAT_BUILD	
    iHorzAdvX = TFloatFixPt(0);
    iHorzOrgX = TFloatFixPt(0);
    iHorzOrgY = TFloatFixPt(0);
	#else
    iHorzAdvX = TFloatFixPt( 0 ,ETrue);
    iHorzOrgX = TFloatFixPt( 0 ,ETrue);
    iHorzOrgY = TFloatFixPt( 0 ,ETrue);
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
CSvgFontElementImpl::CSvgFontElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgFontElementImpl::~CSvgFontElementImpl()
    {
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
TFloatFixPt CSvgFontElementImpl::GetHorzOrgX()
    {
    return iHorzOrgX;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontElementImpl::GetHorzOrgY()
    {
    return iHorzOrgY;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgFontElementImpl::GetHorzAdvX()
    {
    return iHorzAdvX;
    }

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {

    CSvgElementImpl::SetAttributeL(aName,aValue);

    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )

{

     switch ( aNameId )
        {
        case KAtrHorizAdvX:
                            iHorzAdvX = aValue;
                            break;
        case KAtrHorizOriginX:
                            iHorzOrgX = aValue;
                            break;
        case KAtrHorizOriginY:
                            iHorzOrgY = aValue;
                            break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgFontElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrHorizOriginX:
        aValue = GetHorzOrgX();
        break;
        case KAtrHorizAdvX:
        aValue = GetHorzAdvX();
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgFontElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgFontElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                  CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }

void CSvgFontElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<font horiz-origin-x=\"%d\" horiz-origin-y=\"%d\" horiz-adv-x=\"%d\">", (int)iHorzOrgX, (int)iHorzOrgY, (int)iHorzAdvX);
		#endif

		/*vert-origin-x
		vert-origin-y
		vert-adv-y*/
	}
}

#endif     //ifdef SVG_FONTS_INCLUDE
