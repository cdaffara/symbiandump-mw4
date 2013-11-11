/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "SVGDiscardElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGAnimTimingParser.h"
#include "SVGEngineImpl.h"
#include "SVGAnimationBase.h"
#include "SVGRectElementImpl.h"
#include "SVGEvent.h"

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


// ============================= LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RemoveTargetElement: A utility function that renmoves target element.
// Remove target element and all its child from its parent level.
// Allocated Style properties of the target elements have to be deleted here.
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::RemoveTargetElement()
	{
	
    if (iTargetElement!= NULL)
        {
		CSvgElementImpl* lParent = (CSvgElementImpl*)iTargetElement->ParentNode();
		if ( lParent != NULL )
		    {
		    lParent->RemoveChild(iTargetElement);
		    }
		else if ( iTargetElement->ElemID() == KSvgSvgElement )
			{
			// remove all children
		    CSvgElementImpl* child = (CSvgElementImpl*)iTargetElement->FirstChild();
		    CSvgElementImpl* sib = (CSvgElementImpl*) NULL;
		    while ( (child != NULL ) || sib != (CSvgElementImpl*) NULL )
		    	{
		        sib = (CSvgElementImpl*)child->NextSibling();
		        // remove myself later
				if (child != this)
					{					
					iTargetElement->RemoveChild(child);
					delete child;
					}
				child = sib;
		    	}
			}
        
        // make sure that the target element is not root
        if ( iTargetElement->ElemID() != KSvgSvgElement )
        	{
			delete iTargetElement;        	
			iTargetElement = NULL;
        	}
        iRemoveMyself = ETrue;
        }
	}

// -----------------------------------------------------------------------------
// FindTargetElementL: A utility function that finds target element
// and adjust the begin time.
//
// Returns: TBool ETrue if target is set.
//                EFalse if target is not found.
// -----------------------------------------------------------------------------
//
TBool CSvgDiscardElementImpl::FindTargetElementL()
	{
	if (iTargetElement)
		return ETrue;

	// Target could be set from one of the following ways:
	// 1) Defined as SyncBased value.  ==> get element
	// 2) xlink-href defined.          ==> get element
	// 3) Invalid element id           ==> Ignore
	// 4) No specified xlink-href nor other element id. ==> Use parent

	if ( iHrefValueDefined )
    	{
    	// Case 2 & 3
    	TPtrC lPtr = iTargetId->Des();
		iTargetElement = ( CSvgElementImpl * )
	    	((CSvgDocumentImpl*)iOwnerDocument)->GetElementById(lPtr);
    	if (iTargetElement == NULL)
        	{
        	// Case 3
        	// Ignore and remove myself later.
        	iRemoveMyself = ETrue;
        	return EFalse;
        	}
    	}
    else // Case 4
        {
        // Set parent to target only when the parent is an animation element
		iTargetElement = ( CSvgElementImpl * ) ParentNode();
        }

	// Target element should be determined by now.


	// Check to see if the target element is animatable.
	// If not, remove myself.
	// Calculate begin time.

	if ( iSyncValueDefined )
		{
		// Get reference target element
		iRefTargetElement = ( CSvgElementImpl * )
	    ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById(iBeginSyncElementId);

	    // Only Syncbased begin time
	    if (!iEventValueDefined)
	    	{
	    	// If specified element doesn't exist or the element is not animated,
	    	// remove myself.
	    	if ((iRefTargetElement == NULL) /*|| !IsAnimationElement(iRefTargetElement)*/)
	        	{
	        	// No referenced element or not animatable element (no begin/end attr)
	        	// ==> Ignore and remove myself later.
	        	iRemoveMyself = ETrue;
	        	return EFalse;
	        	}

	        // re-calculate begin time. e.g. id1.start+5s
	        if (iBeginReferenceEvent == ESvgEventBeginEvent)
	        	{
				iAbsoluteBeginTime +=
					((CSvgAnimationBase*)iRefTargetElement)->GetAbsoluteBeginTime();
	        	}
	        else if (iBeginReferenceEvent == ESvgEventEndEvent)
	        	{
				iAbsoluteBeginTime +=
					((CSvgAnimationBase*)iRefTargetElement)->GetEndTime();
	        	}
	    	}
	    else // iEventValueDefined is TRUE
	    	{
	    	// Listen external events
	        ((CSvgDocumentImpl*)
	        	iOwnerDocument)->AddToEventReceiverListL(iRefTargetElement,
                                                KSvgEventMaskExternalUI);
	    	}
		}
	else if (iEventValueDefined)
    	{
       	// Listen external events
        ((CSvgDocumentImpl*)
        	iOwnerDocument)->AddToEventReceiverListL(iTargetElement,
                                            KSvgEventMaskExternalUI);
    	}

    return ETrue; // Set Target element
	}



// -----------------------------------------------------------------------------
// HasAnimationElements: A utility function that check if the element
// contains animation element(s).
//
// Returns: TBool ETrue if the element contains animation element(s)
//                EFalse if the element doesn't animated.
// -----------------------------------------------------------------------------
//
/*
TBool CSvgDiscardElementImpl::HasAnimationElements(CSvgElementImpl* aElement)
    {
	TBool lFound = EFalse;

	// Return immediately if the target element is already an animation element.
	if (IsAnimationElement(aElement))
		return ETrue;

	// Look through childs
    CSvgElementImpl* child = (CSvgElementImpl*)aElement->FirstChild();
    while ( child != NULL )
    	{
        if ( IsAnimationElement(child) )
        	{
        	lFound = ETrue;
        	break;
        	}
        child = (CSvgElementImpl*)child->NextSibling();
    	}
    if (lFound)
    	return ETrue;

    return EFalse;
    }
*/


// -----------------------------------------------------------------------------
// IsAnimationElement: A utility function that check if the element
// is an animation element.
//
// Returns: TBool ETrue if the element is a animation element.
//                EFalse if the element is not an animation element.
// -----------------------------------------------------------------------------
//

/*
TBool CSvgDiscardElementImpl::IsAnimationElement(CSvgElementImpl* aElement)
    {

	TInt id = aElement->ElemID();

    return (( id == KSvgAnimateElement ) ||
	        ( id == KSvgAnimateMotionElement ) ||
	        ( id == KSvgAnimateTransformElement ) ||
	        ( id == KSvgSetElement ) ||
	        ( id == KSvgAnimateColorElement ));
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SetAttributeL: The implemented function will be called from CSvgContentHandler.
//
// From CSvgElementImpl
//
// There will be two attributes been set: begin and xlink:href.
// If xlink:href is not specified, the target element will be parent element
// If begin time is not specifid, the target element will be remove immdeiately.
//
// Returns: ETrue always in this case.
// -----------------------------------------------------------------------------
//
TInt CSvgDiscardElementImpl::SetAttributeL( const TDesC& aName,
                                            const TDesC& aValue )
    {
    _LIT( KTmpXlinkHref, "xlink:href" );
    _LIT( KTmpBegin, "begin" );

    // If not xlink:href, it must be attribute "begin"
     if ( aName == KTmpXlinkHref )
        {
		// STEP 1 - Get the reference element
		// If the first char is '#' then remove it
		// This is possible if coming from cXML parser and not the Decoder
		iHrefValueDefined = ETrue;
		TInt pos = aValue.Locate( '#' );
		if ( pos == 0 )
			{
			if (iTargetId)
				{
				delete iTargetId;
				iTargetId = NULL;
				}
			iTargetId = aValue.AllocL();
	    	TPtr tPtr = iTargetId->Des();
			tPtr.Delete(pos, 1);
			}
        }
    else if ( aName == KTmpBegin )
        {
        CSvgAnimTimingParser* atParser = CSvgAnimTimingParser::NewLC( aValue, this );
        TInt32     clockValue;
        TReal32    lRepeatBeginValue;
        atParser->Parse( iBeginSyncElementId,
                         iBeginReferenceEvent,
                         clockValue,
                         lRepeatBeginValue,
                         ETrue);

   		iAbsoluteBeginTime = clockValue;

        if ( iBeginSyncElementId.Size() != 0 )
        	{
        	iSyncValueDefined = ETrue;
        	}

        if ( iBeginReferenceEvent != ESvgEventNone )
        	{
        	iEventValueDefined = ETrue;
        	iRefBeginTime = clockValue;
	   		iAbsoluteBeginTime = KTimeIndefinite;
        	if (iBeginReferenceEvent == ESvgEventKey)
        		{
        		iKeyValue = atParser->AccekeyValue();
        		}
        	}


        CleanupStack::PopAndDestroy( 1 );     // atParser
        }
	
	CSvgElementImpl::SetAttributeL(aName,aValue);
	
    return KErrNone;
    }




// -----------------------------------------------------------------------------
// ReceiveEventL: The implemented function will be called whenever subscribed
// events are received.
//
// From MSvgEventReceiver
//
// Return: TBool ETrue if redraw is needed
//               EFalse if redraw is not needed
// -----------------------------------------------------------------------------
//
TBool CSvgDiscardElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {

	if (iRemoveMyself)
		{
		// Two reasons to remove myself.
		// 1) when discard element is not sepcified inside of the target
	    //    element so the object is not removed automatically along with
	    //    target element.
	    // 2) Invalid begin time
		iTargetElement = ( CSvgElementImpl * ) ParentNode();
		iTargetElement->RemoveChild(this);
		delete this;
		return EFalse;
    	}

	// Looking for target as soon as first event received. Also adject time
	// according to timeing attribute.
	if (!FindTargetElementL())
		{
		// Target not found, exit and remove myself next time
		return EFalse;
		}

	// Target element should be identified by now.

	CSvgEngineImpl* engine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();
	if (engine == NULL)
		{
		return EFalse;
		}

    TInt32 lEngineCurrentTime = engine->CurrentTIme();

	// User input event
	if (aEvent->EventType() == ESvgEngineEventKeyPress)
		{
        MSvgUiKeyEvent* evt   = ( MSvgUiKeyEvent* ) aEvent;
    	if (evt->KeyCode() == iKeyValue)
    	    {
    		// Remove target immediately
        	RemoveTargetElement();
        	engine->RedrawL();
        	return ETrue;
    	    }
        return EFalse;
		}
	// Timer event
    if (aEvent->EventMask() == KSvgEventMaskTimer)
        {

        if (lEngineCurrentTime >= iAbsoluteBeginTime)
            {
            // Time to remove target
        	RemoveTargetElement();
            }
        }
    // Internal event
    else if ((aEvent->EventMask() == KSvgEventMaskInternal )
    		 && (!iRefBeginTimeSet))
    	{
        MSvgInternalEvent*  evt = ( MSvgInternalEvent* ) aEvent;
        TSvgEvent s_evt = evt->SvgEvent();
        if (s_evt == iBeginReferenceEvent)
        	{

	    	// Event other than AccessKey
	    	// Check to see if there is a clock associated with the event.
	    	 if (s_evt == iBeginReferenceEvent)
	    		{
	    		// This is some form of event+(-)clock so delay removing
	    		// Note that once the reference time is set, the absolutetime
	    		// can't be changed with other events.
	    		if (iRefTargetElement != NULL)
	    			{
					// This is a syncbased event
		    		if (evt->ObjectAddress() == iRefTargetElement)
		    			{
			    		iAbsoluteBeginTime = lEngineCurrentTime + iRefBeginTime;
			    		iRefBeginTimeSet = ETrue;
			    		return EFalse;
		    			}
	    			}
	    		else
	    			{
					// Not syncbased event
		    		if (evt->ObjectAddress() == iTargetElement)
		    			{
			    		iAbsoluteBeginTime = lEngineCurrentTime + iRefBeginTime;
			    		iRefBeginTimeSet = ETrue;
			    		return EFalse;
		    			}
	    			}
	    		} //  if (s_evt == iBeginReferenceEvent)...
	    	else
	    		{
	    		// No clock assoicated with the event.
	    		if (iRefTargetElement != NULL)
	    			{
	    			// This is a syncbased event
		    		if (evt->ObjectAddress() == iRefTargetElement)
		    			{
			    		// Remove target immediately
			        	RemoveTargetElement();
		    			}
	    			}
				// Not syncbased event
	    		else if (evt->ObjectAddress() == iTargetElement)
	    			{
		    		// Remove target immediately
		        	RemoveTargetElement();
	    			}
	    		}
        	} // if (s_evt == iBeginReferenceEvent)...
    	} //if ((aEvent->EventMask() == KSvgEventMaskInte...
    else
   		{
   		// Not interested event
    	return EFalse;
   		}

    // return true to be redrawn
    return ETrue;
    }


// Setter functions

// -----------------------------------------------------------------------------
// SetTargetId: Set iTargetId to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetTargetId(const TDesC& aTargetId)
    {
    delete iTargetId;
    iTargetId = NULL;
    TRAPD(err, iTargetId = aTargetId.AllocL());
    if (err != KErrNone)
        {
        // Only display error message. If this failed, iTargetId can safely to be null.
        #ifdef _DEBUG
            RDebug::Printf("CSvgDiscardElementImpl::SetTargetId Error: memory allocation failed.");
        #endif
        }
    }


// -----------------------------------------------------------------------------
// SetSyncValueDefined: Set iSyncValueDefined to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetSyncValueDefined(TBool aSyncValueDefined)
    {
    iSyncValueDefined = aSyncValueDefined;
    }



// -----------------------------------------------------------------------------
// SetEventValueDefined: Set iEventValueDefined to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetEventValueDefined(TBool aEventValueDefined)
    {
    iEventValueDefined = aEventValueDefined;
    }


// -----------------------------------------------------------------------------
// SetHrefValueDefined: Set iHrefValueDefined to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetHrefValueDefined(TBool aHrefValueDefined)
    {
    iHrefValueDefined = aHrefValueDefined;
    }


// -----------------------------------------------------------------------------
// SetBeginSyncElementId: Set aBeginSyncElementId to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetBeginSyncElementId(const TDesC& aBeginSyncElementId)
    {
    iBeginSyncElementId.SetLength(0);
    iBeginSyncElementId.Append(aBeginSyncElementId);
    }


// -----------------------------------------------------------------------------
// SetAbsoluteBeginTime: Set iAbsoluteBeginTime to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetAbsoluteBeginTime(TInt aAbsoluteBeginTime)
    {
    if (aAbsoluteBeginTime == -1)
    	{
	    iAbsoluteBeginTime = KTimeIndefinite;
    	}
    else
    	{
	    iAbsoluteBeginTime = aAbsoluteBeginTime;
    	}
    }



// -----------------------------------------------------------------------------
// SetRefBeginTime: Set aRefBeginTime to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetRefBeginTime(TInt aRefBeginTime)
    {
    iRefBeginTime = aRefBeginTime;
    }



// -----------------------------------------------------------------------------
// SetKeyValue: Set aKeyValue to the object. Mainly used by decoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetKeyValue(TInt aKeyValue)
    {
    iKeyValue = aKeyValue;
    }



// -----------------------------------------------------------------------------
// TargetId: Returns iBeginReferenceEvent that is specified in begin attribute.
// Mainly used by encoder.
//
// Returns: void
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::SetBeginReferenceEvent(TSvgEvent aBeginReferenceEvent)
    {
    iBeginReferenceEvent = aBeginReferenceEvent;
    }


// Getter functions

// -----------------------------------------------------------------------------
// TargetId: Returns the target element's id. Mainly used by encoder.
//
// Returns: Descriptor of target element's id
// -----------------------------------------------------------------------------
//
const TDesC& CSvgDiscardElementImpl::TargetId()
    {
    return (TDesC&) *iTargetId;
    }



// -----------------------------------------------------------------------------
// BeginSyncElementId: Returns element's id that is specified in begin attribute.
// Mainly used by encoder.
//
// Returns: Descriptor of the element's id
// -----------------------------------------------------------------------------
//
const TDesC& CSvgDiscardElementImpl::BeginSyncElementId()
    {
    return iBeginSyncElementId;
    }



// -----------------------------------------------------------------------------
// AbsoluteBeginTime: Returns the absolute begin time that is identified in
// SetAttribute. The begin time may be changed when using with SyncBase or
// EventBased attribute.
//
// Mainly used by encoder.
//
// Returns: TInt AbsoluteBeginTime
// -----------------------------------------------------------------------------
//
TInt CSvgDiscardElementImpl::AbsoluteBeginTime()
    {
    if (iAbsoluteBeginTime == KTimeIndefinite)
    	{
    	// write -1 in case both encorder and decoder understand.
    	return -1;
    	}
    else
		{
	    return iAbsoluteBeginTime;
		}
    }


// -----------------------------------------------------------------------------
// IsSyncValueDefined: Returns the flag iSyncValueDefined that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TBool iSyncValueDefined
// -----------------------------------------------------------------------------
//
TBool CSvgDiscardElementImpl::IsSyncValueDefined()
    {
    return iSyncValueDefined;
    }


// -----------------------------------------------------------------------------
// IsEventValueDefined: Returns the flag iEventValueDefined that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TBool iEventValueDefined
// -----------------------------------------------------------------------------
//
TBool CSvgDiscardElementImpl::IsEventValueDefined()
    {
    return iEventValueDefined;
    }


// -----------------------------------------------------------------------------
// IsHrefValueDefined: Returns the flag iHrefValueDefined that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TBool iHrefValueDefined
// -----------------------------------------------------------------------------
//
TBool CSvgDiscardElementImpl::IsHrefValueDefined()
    {
    return iHrefValueDefined;
    }


// -----------------------------------------------------------------------------
// RefBeginTime: Returns the reference begin time that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TInt iRefBeginTime
// -----------------------------------------------------------------------------
//
TInt CSvgDiscardElementImpl::RefBeginTime()
    {
    return iRefBeginTime;
    }


// -----------------------------------------------------------------------------
// KeyValue: Returns the key value (scan code) that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TInt iKeyValue
// -----------------------------------------------------------------------------
//
TInt CSvgDiscardElementImpl::KeyValue()
    {
    return iKeyValue;
    }


// -----------------------------------------------------------------------------
// KeyValue: Returns the iBeginReferenceEvent that was set in
// SetAttribute.
//
// Mainly used by encoder.
//
// Returns: TSvgEvent iBeginReferenceEvent
// -----------------------------------------------------------------------------
//
TSvgEvent CSvgDiscardElementImpl::BeginReferenceEvent()
    {
    return iBeginReferenceEvent;
    }



// -----------------------------------------------------------------------------
// CloneL: Perform a deep clone of this object
//
// Returns: MXmlElement pointer to the newly created element.
// -----------------------------------------------------------------------------
//
MXmlElement* CSvgDiscardElementImpl::CloneL(MXmlElement* aParentElement)
    {
	CSvgDiscardElementImpl* newElement = CSvgDiscardElementImpl::NewL( this->ElemID(),
	        ((CSvgDocumentImpl*)iOwnerDocument) );
	CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;

    // copy everything over
    this->CopyL(newElement);
    CleanupStack::Pop();

    return newElement;
    }


// -----------------------------------------------------------------------------
// CopyL: Perform a deep copy of this object
//
// Returns: none
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::CopyL( CSvgDiscardElementImpl* aDestElement )
    {
    if(aDestElement)
	   	{
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		// copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
	    }
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSvgDiscardElementImpl* CSvgDiscardElementImpl::NewL( const TUint8 aElemID,
                                                      CSvgDocumentImpl* aDoc)
    {
    CSvgDiscardElementImpl* self = new (ELeave)CSvgDiscardElementImpl(aDoc);
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSvgDiscardElementImpl* CSvgDiscardElementImpl::NewLC( const TUint8 aElemID,
                                                       CSvgDocumentImpl* aDoc)
    {
    CSvgDiscardElementImpl* self = new (ELeave) CSvgDiscardElementImpl(aDoc);
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSvgDiscardElementImpl::CSvgDiscardElementImpl( CSvgDocumentImpl* aDoc )
	{
    SetOwnerDocument(aDoc);
    }


// -----------------------------------------------------------------------------
// Symbian default constructor that can leave.
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CXmlElementImpl::InitializeL( aElemID );

 	iTargetElement = NULL;
 	iTargetId = HBufC::NewL(0);
 	iAbsoluteBeginTime = 0;
 	iRemoveMyself = EFalse;
 	iHrefValueDefined = EFalse;
 	iBeginReferenceEvent = (TSvgEvent)NULL;
 	iSyncValueDefined = EFalse;
 	iEventValueDefined = EFalse;
 	iRefBeginTime = 0;
	iRefTargetElement = NULL;
	iRefBeginTimeSet = EFalse;
	iKeyValue = 0;
	iBeginSyncElementId.SetLength(0);

    ((CSvgDocumentImpl*)
    	iOwnerDocument)->AddToEventReceiverListL(this, KSvgEventMaskTimer |
    	    KSvgEventMaskInternal | KSvgEventMaskExternalUI);
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSvgDiscardElementImpl::~CSvgDiscardElementImpl()
    {
    delete iTargetId;
    }

// -----------------------------------------------------------------------------
// Print
// -----------------------------------------------------------------------------
//
void CSvgDiscardElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<discard xlink:href =\"hmm\" begin=\"%d\" />", /*Href(),*/ (int)iAbsoluteBeginTime);
		#endif
	}
}

//  End of File
