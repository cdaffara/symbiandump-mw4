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
#include "SVGForeignObjectElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgForeignObjectElementImpl* CSvgForeignObjectElementImpl::NewL( const TUint8 aElemID,
                                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgForeignObjectElementImpl*   self    = new ( ELeave )
                                              CSvgForeignObjectElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgForeignObjectElementImpl* CSvgForeignObjectElementImpl::NewLC( const TUint8 aElemID,
                                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgForeignObjectElementImpl*   self    = new ( ELeave )
                                              CSvgForeignObjectElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgForeignObjectElementImpl::CSvgForeignObjectElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);
    iReqAttrFlag = KAtrSVGRec;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgForeignObjectElementImpl::~CSvgForeignObjectElementImpl()
    {
    iReqAttrFlag = KAtrSVGRec;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgForeignObjectElementImpl::X()
{
	return iX;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgForeignObjectElementImpl::Y()
{
	return iY;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgForeignObjectElementImpl::Width()
{
	return iWidth;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgForeignObjectElementImpl::Height()
{
	return iHeight;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgForeignObjectElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
{
    switch ( aNameId )
        {
        case KAtrX:
					iX = aValue;
		break;
        case KAtrY:
					iY = aValue;
		break;

        case KAtrWidth:
            iWidth = aValue;
            if(iReqAttrFlag == KAtrSVGRec)
                {
                iReqAttrFlag = KAtrHeight;
                }
            else
                {
                iReqAttrFlag = 0;
                }
        break;
        case KAtrHeight:
            iHeight = aValue;
            if(iReqAttrFlag == KAtrSVGRec)
                {
                iReqAttrFlag = KAtrWidth;
                }
            else
                {
                iReqAttrFlag = 0;
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
TInt CSvgForeignObjectElementImpl::SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue )
{
    switch ( aNameId )
        {
        case KAtrCdata:
        SetCdataL( aValue );
        break;

        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }
    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgForeignObjectElementImpl::SetCdataL( const TDesC& aCdata )
{
    if ( iCdata )
        {
        delete iCdata;
        iCdata = NULL;
        }

    iCdata = aCdata.AllocL();

}

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgForeignObjectElementImpl::SetAttributeL( const TDesC& aName,
                                                  const TDesC& aValue )
    {

    if ( this->SetLangSpaceAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }
    if ( this->SetTransform( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( this->SetTestAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( SetIdandXmlbaseL( aName, aValue ) )
        {
        return KErrNone;
        }

    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgForeignObjectElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                           CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgForeignObjectElementImpl::ConstructL( const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );
    }

/*** FROM MSvgMouseListener ***/
// ---------------------------------------------------------------------------
// mouse entered
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::MouseEntered( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	/*CSvgEngineImpl* aEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	for (TInt i = 0; i < aElements.Count(); i++ )
	{
		if ( aElements[i] == this )
		{
			aEngine->NotifyHyperlinkEntered(this);
			return ETrue;
		}
    }*/

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer exits a visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::MouseExited( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                   TInt /*aX*/, TInt /*aY*/ )
{
	/*CSvgEngineImpl* aEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	for (TInt i = 0; i < aElements.Count(); i++ )
	{
		if ( aElements[i] == this )
		{
			aEngine->NotifyHyperlinkExited(this);
			return ETrue;
		}
    }*/

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::MouseMoved( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::MousePressed( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is released on on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgForeignObjectElementImpl::MouseReleased( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	/*CSvgEngineImpl* aEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();

	for (TInt i = 0; i < aElements.Count(); i++ )
	{
		if ( aElements[i] == this )
		{
			aEngine->NotifyHyperlinkActivated(this);
			return ETrue;
		}
    }*/

    return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgForeignObjectElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<foreignObject x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">", (int)X(), (int)Y(), (int)Width(), (int)Height());
		RDebug::Printf("</foreignObject>");
		#endif
	}
}
