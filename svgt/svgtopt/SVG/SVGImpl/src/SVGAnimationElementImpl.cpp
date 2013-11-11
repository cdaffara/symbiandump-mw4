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
#include "SVGAnimationElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGAnimationBase.h"
#include "SVGEvent.h"
#include "SVGEventHandler.h"

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


// ============================= LOCAL FUNCTIONS ===============================

// Setter functions

// -----------------------------------------------------------------------------
// SetRecursionVariable: Set aIsRecursion to indicate that the element has been
// cloned or not.
//
// Returns: TFloatFixPt
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::SetRecursionVariable(TBool aIsRecursion)
	{
	iUseInRecursion = aIsRecursion;
	}



// ============================ MEMBER FUNCTIONS ===============================

// Getter functions

// -----------------------------------------------------------------------------
// X: returns iX
//
// Returns: TFloatFixPt
// -----------------------------------------------------------------------------
//
TFloatFixPt CSvgAnimationElementImpl::X()
    {
    return iX;
    }


// -----------------------------------------------------------------------------
// Y: returns iY
//
// Returns: TFloatFixPt
// -----------------------------------------------------------------------------
//
TFloatFixPt CSvgAnimationElementImpl::Y()
    {
    return iY;
    }


// -----------------------------------------------------------------------------
// Width: returns iWidth
//
// Returns: TFloatFixPt
// -----------------------------------------------------------------------------
//
TFloatFixPt CSvgAnimationElementImpl::Width()
    {
    return iWidth;
    }


// -----------------------------------------------------------------------------
// Height: return iHeight
//
// Returns: TFloatFixPt
// -----------------------------------------------------------------------------
//
TFloatFixPt CSvgAnimationElementImpl::Height()
    {
    return iHeight;
    }


// -----------------------------------------------------------------------------
// PreservRatio: Returns the PreserveAspecRatio's setting. Mainly used by encoder.
//
// Returns: Descriptor of PreserveAspecRatio value
// -----------------------------------------------------------------------------
//
const TDesC& CSvgAnimationElementImpl::PreservRatio()
    {
    return (TDesC&) *iPreservRatio;
    }


// -----------------------------------------------------------------------------
// SetAttributeL: Called by Contenthandler or decoder to set attributes
//
// Returns: TInt error code
// -----------------------------------------------------------------------------
//
TInt CSvgAnimationElementImpl::SetAttributeL( const TDesC& aName,
                                        const TDesC& aValue )
    {
    _LIT( KXlink, "xlink:href" );
    _LIT( KPreserveAspectRatio, "preserveAspectRatio" );

    if ( aName == KXlink )
        {
        // STEP 1 - Get the reference element
        // If the first char is '#' then remove it
        // This is possible if coming from cXML parser and not CVG Decoder
        TInt pos = aValue.Locate( '#' );
        if ( pos != KErrNotFound && pos == 0 )
            {
            HBufC*  tBufC   = HBufC::NewLC( aValue.Length() );
            TPtr    tPtr    = tBufC->Des();
            tPtr.Copy( aValue );
            tPtr.Delete( pos, 1 );

			if ( this->SetXlinkAttributeL( aName, tPtr ) )
				{
				iReferencedElement = ( CSvgElementImpl * )
					((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( tPtr );

				if(iReferencedElement == NULL)
					{
					// failure in finding the referenced element
					CleanupStack::PopAndDestroy( 1 ); // tBufC
					return KErrReferencedElementNotFound;
					}
				}

			CleanupStack::PopAndDestroy( 1 ); // tBufC
			}
		}
	else if ( aName == KPreserveAspectRatio )
    	{
        iPreservRatio = aValue.AllocL();
    	}
	else
	    {
    	CSvgAnimationBase::SetAttributeL(aName, aValue);
	    }

	return KErrNone;
    }


// -----------------------------------------------------------------------------
// SetAttributeFloatL: Called by Contenthandler or decoder to set attributes
//
// Returns: TInt error code
// -----------------------------------------------------------------------------
//
TInt CSvgAnimationElementImpl::SetAttributeFloatL( const TInt aNameId,
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
            break;

        case KAtrHeight:
            iHeight = aValue;
            break;

        default:
            return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// GetAttributeFloat: returns value of the floating attribute
//
// Returns: TInt error code
// -----------------------------------------------------------------------------
//
TInt CSvgAnimationElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
				 aValue = iX ;
			     break;

        case KAtrY:
				aValue = iY ;
			    break;

        case KAtrWidth:
				aValue = iWidth ;
		        break;

        case KAtrHeight:
				aValue = iHeight ;
		        break;

        default:
            return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// GetUriRefId: Get the id of reference target id
//
// Returns: TDesC& id
// -----------------------------------------------------------------------------
//
//const TDesC& CSvgAnimationElementImpl::GetUriRefId()
//	{
//	return (TDesC&) (iReferencedElement->Id());
//	}


// -----------------------------------------------------------------------------
// IsUriRefSet: Return true or not if uri is set
//
// Returns: TBool
// -----------------------------------------------------------------------------
//
TBool CSvgAnimationElementImpl::IsUriRefSet()
	{
    if(iReferencedElement)
        {
		return ETrue;
	    }
	  else
	    {
		return EFalse;
	    }
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
TBool CSvgAnimationElementImpl::ReceiveEventL( MSvgEvent* aEvent )
    {
    // Processing events after begin time
	if( CSvgElementImpl::IsSVGEnginePaused())
    {
    	return EFalse;
    }
	CSvgEngineImpl* engine = ((CSvgDocumentImpl*)iOwnerDocument)->Engine();
	if (engine == NULL)
		{
		return EFalse;
		}

    TInt32 lEngineCurrentTime = engine->CurrentTIme();

    if ( lEngineCurrentTime < iAbsoluteBeginTime)
        {
        // not begin yet.
        return ETrue;
        }
    else
        {
        // rewrite engine time to simulate different timer
    	if ( aEvent->EventType() == ESvgEngineEventTimer )
    		{
        	((MSvgTimerEvent*)aEvent)->SetTime( (TInt32)(lEngineCurrentTime
        		- iAbsoluteBeginTime ));
    		}
        }

    // Basically forward received events to all its children as if they receive
    // event from DocumentImpl.
    if ( iAnimationEventHandler->ProcessEventL( aEvent ))
        {
        if (iInitSortList)
            {
            iAnimationEventHandler->SortEventList();
            iInitSortList = EFalse;
            }
        iAnimationEventHandler->DoAnimProcL(aEvent);
        engine->RedrawL();
        }
    else
        {
        // this is to keep the dom in its final state.
        if(aEvent->EventType() == ESvgEngineEventTimer)
            {
            iAnimationEventHandler->DoAnimProcL(aEvent);
            }
        }

    // Process key event separately.
	if (aEvent->EventType() == ESvgEngineEventKeyPress)
		{
		}

    // return true to be redrawn
    return ETrue;
    }


// -----------------------------------------------------------------------------
// RecursionVariable: Getter function that could be called by Contenthandler or
// decoder to determine if the element has been cloned.
// elements
//
// Return: TBool ETrue if the element is cloned already
// -----------------------------------------------------------------------------
//
TBool CSvgAnimationElementImpl::RecursionVariable()
	{
	return iUseInRecursion;
	}


// -----------------------------------------------------------------------------
// SetRefElemById: Called by Contenthandler or decoder to set referenced
// elements
//
// Return: none
// -----------------------------------------------------------------------------
//
TInt  CSvgAnimationElementImpl::SetRefElemById(const TDesC& aName)
	{
	 iReferencedElement = ( CSvgElementImpl * )
                     ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( aName );
     if( iReferencedElement == NULL )
     	{
     	return KErrNotFound;
     	}
     else
     	{
         return KErrNone;
    	}

	}


// -----------------------------------------------------------------------------
// SetReferenceElementL: Called by Contenthandler or decoder to clone referenced
// element which has been determined in SetRefElemById(...).
//
// Return: none
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::SetReferenceElementL()
    {
    _LIT( KPreserveAspectRatio, "preserveAspectRatio" );

	if(!RecursionVariable())
		{
		if( iReferencedElement != NULL )
			{
			// Setting iUseInRecursion to True so that the Call to CloneL
			// is not called to this instance of use in this recursion
			SetRecursionVariable(ETrue);

     		CSvgElementImpl* theElement = (CSvgElementImpl *)iReferencedElement->CloneL((MXmlElement*)this);
     		CleanupStack::PushL(theElement);
			this->CXmlElementImpl::AppendChildL(theElement);
			CleanupStack::Pop();
			if(!RecursionVariable())
				{
				// The Recursion variable is set to false only when the Cloning
				// goes into loop.
				// Using this information and then Setting the Recursion variable
				// back to True to use the information to set error code in
				// StartElementL in Content Handler
				//
				SetRecursionVariable(ETrue);
				}
			else // Normal CloneL loop exit
				{
				// Setting iUseInRecursion back to False
				SetRecursionVariable(EFalse);
				}

                CSvgDocumentImpl* doc = ( CSvgDocumentImpl* ) OwnerDocument();
                CSvgSvgElementImpl* lSvgRoot = (CSvgSvgElementImpl*) doc->RootElement();
                if (iPreservRatio != NULL)
                	{
                	lSvgRoot->SetAttributeL( KPreserveAspectRatio, iPreservRatio->Des() );
                	}
                lSvgRoot->SetWidth( iX );
                lSvgRoot->SetHeight( iY );

			}
    	}
    }



// -----------------------------------------------------------------------------
// ResetEventListener: Reset event listeners for it's children
//
// Returns: none
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::ResetEventListener()
    {
    FindAllAnimationElements( (CSvgElementImpl* )this, iMyAnimationElementList );

    // Remove event listening for every animation elements that are child of
    // this Animation element. This is to implement a separated time container
    //
    // Instead, register event linstening to the event handler of this element.
    //
    TInt myAnimationEleListCnt = iMyAnimationElementList->Count();
	for (int i=0; i<myAnimationEleListCnt; i++)
    	{
		CSvgElementImpl* lCurElem =
		    (CSvgElementImpl*)iMyAnimationElementList->operator[](i);
		// Removing...
        ((CSvgDocumentImpl*)iOwnerDocument)->RemoveFromEventReceiverList(lCurElem);

        // Adding...
        TUint8 eventMask = iAnimationEventHandler->EventMask(lCurElem);
        TRAPD(err, iAnimationEventHandler->AddToEventReceiverListL( lCurElem, eventMask ));
        if (err != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(msg, "CSvgAnimationElementImpl::ResetEventListener():Can't add myself to event receiverList");
            RDebug::Print(msg);
            #endif //_DEBUG
            }
        }
    }



// -----------------------------------------------------------------------------
// RemoveEventListener: Remove event listeners for it's children
//
// Returns: none
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::RemoveEventListener()
    {
    if ( !iMyAnimationElementList )
         return;
    
    FindAllAnimationElements( (CSvgElementImpl* )this, iMyAnimationElementList );

    // Remove event listening for every animation elements that are child of
    // this Animation element. This is to implement a separated time container
    //
    // Instead, register event linstening to the event handler of this element.
    //
	for (int i=0; i<iMyAnimationElementList->Count(); i++)
    	{
		CSvgElementImpl* lCurElem =
		    (CSvgElementImpl*)iMyAnimationElementList->operator[](i);
		// Removing...
        iAnimationEventHandler->RemoveFromEventReceiverList( lCurElem );
        }
    }




// -----------------------------------------------------------------------------
// CSvgAnimationElementImpl: Find all animation elements and save it to aList
//
// Returns: none
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::FindAllAnimationElements(
                               CSvgElementImpl* aElement,
                               RPointerArray<CSvgElementImpl>* aList )
    {
    CSvgElementImpl* child = (CSvgElementImpl*)aElement->FirstChild();
    while ( child != NULL )
    	{
        if ( IsAnimationElement(child) )
        	{
            aList->Append( child );
        	}
        FindAllAnimationElements( child, aList );
        child = (CSvgElementImpl*)child->NextSibling();
    	}
    }



// -----------------------------------------------------------------------------
// IsAnimationElement: A utility function that check if the element
// is an animation element.
//
// Returns: TBool ETrue if the element is a animation element.
//                EFalse if the element is not an animation element.
// -----------------------------------------------------------------------------
//
TBool CSvgAnimationElementImpl::IsAnimationElement(CSvgElementImpl* aElement)
    {

	TInt id = aElement->ElemID();

    return (( id == KSvgAnimateElement ) ||
	        ( id == KSvgAnimateMotionElement ) ||
	        ( id == KSvgAnimateTransformElement ) ||
	        ( id == KSvgSetElement ) ||
	        ( id == KSvgAnimateColorElement ));
    }


// -----------------------------------------------------------------------------
// ResetAnimationL: Actions to reset the animation
//
// Returns: none
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::ResetAnimationL()
    {
    }


// -----------------------------------------------------------------------------
// AnimProcL: Actions to reset the animation
//
// Returns: none
// -----------------------------------------------------------------------------
//
TBool CSvgAnimationElementImpl::AnimProcL( MSvgTimerEvent* aEvent )
    {
	if (aEvent->EventType() == ESvgEngineEventKeyPress)
		{
		}

    return EFalse;
    }



// -----------------------------------------------------------------------------
// CloneL: Perform a deep clone of this object
//
// Returns: MXmlElement pointer to the newly created element.
// -----------------------------------------------------------------------------
//
MXmlElement* CSvgAnimationElementImpl::CloneL(MXmlElement* aParentElement)
    {

	if(RecursionVariable())
		{
		SetRecursionVariable(EFalse);
		return NULL;
		}

	CSvgAnimationElementImpl* newElement = CSvgAnimationElementImpl::NewL( this->ElemID(),
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
void CSvgAnimationElementImpl::CopyL( CSvgAnimationElementImpl* aDestElement )
    {
    if(aDestElement)
	   	{
	    aDestElement->iX = this->iX;
	    aDestElement->iY = this->iY;
	    aDestElement->iHeight = this->iHeight;
	    aDestElement->iWidth = this->iWidth;
		aDestElement->iReferencedElement = this->iReferencedElement;

		aDestElement->SetRecursionVariable(this->RecursionVariable());
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		// copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
	    }
    }


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSvgAnimationElementImpl* CSvgAnimationElementImpl::NewL( const TUint8 aElemID,
                                                      CSvgDocumentImpl* aDoc)
    {
    CSvgAnimationElementImpl* self = new (ELeave)CSvgAnimationElementImpl(aDoc);
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSvgAnimationElementImpl* CSvgAnimationElementImpl::NewLC( const TUint8 aElemID,
                                                       CSvgDocumentImpl* aDoc)
    {
    CSvgAnimationElementImpl* self = new (ELeave) CSvgAnimationElementImpl(aDoc);
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }


// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSvgAnimationElementImpl::CSvgAnimationElementImpl( CSvgDocumentImpl* aDoc )
	 : CSvgAnimationBase( aDoc )
	{
	iInitSortList = ETrue; //True indicates that the soring has not been done.
    }


// -----------------------------------------------------------------------------
// Symbian default constructor that can leave.
// -----------------------------------------------------------------------------
//
void CSvgAnimationElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgAnimationBase::ConstructL( aElemID );

	iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	iSvgStyleProperties->Remove( 0 );

    iMyAnimationElementList = new (ELeave)RPointerArray<CSvgElementImpl> (1);
	iMyAnimationElementList->Reset();

    iSvgTransformable = CSvgTransformableImpl::NewL();

	#ifdef SVG_FLOAT_BUILD
    iX = ( 0 );
    iY = ( 0 );
	#else
    iX.operator = ( 0 );
    iY.operator = ( 0 );
	#endif

	iUseInRecursion = EFalse;

    iAnimationEventHandler = CSvgEventHandler::NewL();

//    ((CSvgDocumentImpl*)
//    	iOwnerDocument)->AddToEventReceiverListL(this, KSvgEventMaskTimer |
//    	    KSvgEventMaskInternal | KSvgEventMaskExternalUI);
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSvgAnimationElementImpl::~CSvgAnimationElementImpl()
    {

    // unregister event listening for all childs.
    RemoveEventListener();

    if( iAnimationEventHandler )
        {
        delete iAnimationEventHandler;
        iAnimationEventHandler = NULL ;
        }


	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}

    if ( iMyAnimationElementList )
        {
		iMyAnimationElementList->Reset();
        iMyAnimationElementList->Close();
        delete iMyAnimationElementList;
        }

    if ( iPreservRatio )
        {
        delete iPreservRatio;
        }

    }

void CSvgAnimationElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<animation x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"hmm\" preserveAspectRatio=\"hmm\" />",
	 	(int)iX, (int)iY, (int)iWidth, (int)iHeight/*, Href(), PreservRatio()*/);
		#endif
	}
}
//  End of File
