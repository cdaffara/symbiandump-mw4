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
#include "SVGAElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"

#include "GfxAffineTransform.h"
#include "SVGGElementImpl.h"

_LIT(TARGET, "target");
_LIT(NEW, "new");

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgAElementImpl* CSvgAElementImpl::NewL(  const TUint8 aElemID,
										  CSvgDocumentImpl* aDoc )
	{
	CSvgAElementImpl*   self    = new ( ELeave ) CSvgAElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(aElemID);
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAElementImpl* CSvgAElementImpl::NewLC(  const TUint8 aElemID,
										   CSvgDocumentImpl* aDoc )
	{
	CSvgAElementImpl*   self    = new ( ELeave ) CSvgAElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(aElemID);

	return self;
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAElementImpl::ConstructL( const TUint8 aElemID)
	{
	CSvgElementImpl::InitializeL( aElemID);

	iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	iSvgStyleProperties->Remove( 0 );

    iSvgTransformable = CSvgTransformableImpl::NewL();

    iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();
		iTarget = HBufC::NewL( 0 );

	if (OwnerDocument())
	{
		((CSvgDocumentImpl*)OwnerDocument())->AddInternalMouseListener( this );
	}


	}



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgAElementImpl::~CSvgAElementImpl()
{
    if(iSvgUriReferenceImpl)
    	{
    	delete iSvgUriReferenceImpl;
    	iSvgUriReferenceImpl = NULL;
    	}

    if(iSvgStyleProperties)
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
		if (iTarget)
		{
			delete iTarget;
			iTarget = NULL;
		}

	if (OwnerDocument())
		{
			((CSvgDocumentImpl*)OwnerDocument())->RemoveInternalMouseListener( this );
		}
}

// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgAElementImpl::CSvgAElementImpl( CSvgDocumentImpl* aDoc ) : iInitDone( EFalse )
    {
	SetOwnerDocument(aDoc);
    }


// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgAElementImpl::SetAttributeL( const TDesC& aName, const TDesC& aValue )
    {
    if ( this->SetXlinkAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }
   if ( SetTargetL( aName, aValue ))
   		{
   		return KErrNone;
   		}

	CSvgElementImpl::SetAttributeL(aName,aValue);

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TBool CSvgAElementImpl::SetTargetL( const TDesC& aName, const TDesC& aValue )
    {
	if ( aName == TARGET )
        {
        if(iTarget)
	        {
	        delete iTarget;
	        iTarget = NULL;
	        }
	       iTarget = aValue.AllocL();
        }
    else
        {
  		return EFalse;
        }

    return ETrue;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgAElementImpl::Target( )
{
	 if (iTarget)
	 {
	 		return *iTarget;
	 }
	 else
	 {
    	return KNullDesC;
   }
}

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgAElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::DrawL( CGfx2dGc* /* aGc */,
                               CSvgElementImpl* /* aElement */ )
    {
    if ( !iInitDone )
        {
        // Add child elemenets as mouse event receiver to engine
        // so that child elements can fire ui internal event
        AddEventReceiverSubtreeL( this );

        // Add this as internal event receiver
        ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( this, KSvgEventMaskInternal);


        iInitDone = ETrue;
        }

    return ETrue;
    }


// *******************************************************
// From MSvgEventReceiver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {

	if( CSvgElementImpl::IsSVGEnginePaused())
    {
    	return EFalse;
    }
    // Receive internal UI click event on child elements only
    if ( aEvent->EventType() != ESvgEngineInternalEvent )
		{
        return EFalse;
		}

	// Link activation process
    MSvgInternalEvent*  evt = ( MSvgInternalEvent* ) aEvent;

     if ( !IsChild( this, evt->ObjectAddress() ) )
		{
        return EFalse;
		}

//////

	CSvgEngineImpl* engine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();
 // Process the UI event on its child elements and propagate that event to its dependents
    switch ( evt->SvgEvent() )
        {

        case ESvgEventMousedown:
        case ESvgEventMouseup:
        case ESvgEventClick:
		case ESvgEventActivate:
		case ESvgEventMouseover:
		case ESvgEventMousemove:
		case ESvgEventFocusin:
		case ESvgEventMouseout:
		case ESvgEventFocusout:
            {
            TSvgInternalEvent   lEvt ( evt->SvgEvent(),
                                        ( CSvgElementImpl* )
                                             this );
            engine->ProcessEventL( 
                ( CSvgDocumentImpl* ) OwnerDocument(), &lEvt );
			}
        break;

        default:
        break;
        }

///////


	// To activate only on mouse click event
    if ( evt->SvgEvent() != ESvgEventClick )
		{
        return EFalse;
		}

    // Linking behavior

	TPtrC n(XlinkShow());

	if (n == NEW)
		{
		engine->LinkRequestWithShow( Href(),XlinkShow() ); // ssb iHrefUri->Des());
		}
	else
		{
		engine->LinkRequest( Href() ); // ssb iHrefUri->Des());
		}



    return EFalse;
    }


// *******************************************************
// Private methods
//
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgAElementImpl::AddEventReceiverSubtreeL( CSvgElementImpl* aElement )
    {
    CSvgElementImpl*element = ( CSvgElementImpl* ) aElement->FirstChild();
    while ( element != NULL )
        {

        ((CSvgDocumentImpl*)iOwnerDocument)->AddToEventReceiverListL( element,
                                                KSvgEventMaskExternalUI);

        AddEventReceiverSubtreeL( element );
        element = ( CSvgElementImpl * ) element->NextSibling();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::IsChild( CSvgElementImpl* aSubtree,
                                 CSvgElementImpl* aElement )
    {
    CSvgElementImpl*element = ( CSvgElementImpl* ) aSubtree->FirstChild();
    if ( element != NULL )
        {
        do
            {
            if ( element == aElement || IsChild( element, aElement ) )
				{
                return ETrue;
				}
            element = ( CSvgElementImpl * ) element->NextSibling();
            }
        while ( element != NULL );
        }
    return EFalse;
    }
void CSvgAElementImpl::Reset(MSvgEvent* /*aEvent*/)
{
}

TInt CSvgAElementImpl::SetAttributeDesL( const TInt aNameId,
                                             const TDesC& aValue )
    {
	_LIT( KXlinkHref, "xlink:href" );
	_LIT( KTarget, "target");

    switch ( aNameId )
        {
        case KAtrXlinkhref:
        SetAttributeL(KXlinkHref, aValue );
        break;
        case KAtrTarget:
        SetAttributeL(KTarget, aValue );
        break;
        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }
    return KErrNone;
    }
TInt CSvgAElementImpl::GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue )
{
 if(aNameId == KAtrXlinkhref)
	{
	 aValue.Set( Href() );
	 return KErrNone;
	}
	if (aNameId == KAtrTarget)
	{
		aValue.Set( Target() );
		return KErrNone;
	}
 return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
}

/*** FROM MSvgMouseListener ***/
// ---------------------------------------------------------------------------
// mouse entered
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::MouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();
	
	TInt lEleCnt = aElements.Count();
	for (TInt i = 0; i < lEleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyHyperlinkEntered(this);
			return ETrue;
		}
    }

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer exits a visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::MouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                   TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();
	
	TInt lEleCnt = aElements.Count();
	for (TInt i = 0; i < lEleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyHyperlinkExited(this);
			return ETrue;
		}
    }

	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::MouseMoved( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is pressed down on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::MousePressed( RPointerArray<CSvgElementImpl>& /*aElements*/,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	return EFalse;
}

// ---------------------------------------------------------------------------
// Notified when the mouse pointer is released on on visible svg element.
// ---------------------------------------------------------------------------
TBool CSvgAElementImpl::MouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                    TInt /*aX*/, TInt /*aY*/ )
{
	CSvgEngineImpl* lEngine  = ( ( CSvgDocumentImpl* ) OwnerDocument() )->Engine();
	
	TInt lEleCnt = aElements.Count();
	for (TInt i = 0; i < lEleCnt; i++ )
	{
		if ( aElements[i] == this )
		{
			lEngine->NotifyHyperlinkActivated(this);
			return ETrue;
		}
    }

    return EFalse;
}

void CSvgAElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
{
	CSvgGElementImpl::GetGroupBoundingUnscaled( aBbox, this );
}

void CSvgAElementImpl::GetBBox( TGfxRectangle2D& aBbox )
{
	CSvgGElementImpl::GetGroupBounding( aBbox, this );
}

void CSvgAElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<a xlink:href=\"hmm\">"/*, Href()*/);
		#endif
	}
}
