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


#include <utf.h>
#include <s32mem.h>

#include "SVGContentHandler.h"
#include "Svgdecoder.h"

#include  "SVGDocumentImpl.h"
#include  "SVGEngineImpl.h"
#include  "SVGSvgElementImpl.h"
#include  "SVGLineElementImpl.h"
#include  "SVGRectElementImpl.h"
#include  "SVGCircleElementImpl.h"
#include  "SVGEllipseElementImpl.h"
#include  "SVGPolylineElementImpl.h"
#include  "SVGGElementImpl.h"
#include  "SVGPathElementImpl.h"
#include  "SVGMpathElementImpl.h"
#include  "SVGSchemaData.h"
#include  "SVGTextElementImpl.h"
#include  "SVGImageElementImpl.h"
#include  "SVGUseElementImpl.h"
#include  "SVGAElementImpl.h"
#include  "SVGStyleElementImpl.h"
#include  "SVGForeignObjectElementImpl.h"
#include  "SVGSetElementImpl.h"
#include  "SVGAnimateTransformElementImpl.h"
#include  "SVGAnimateElementImpl.h"
#include  "SVGAnimateMotionElementImpl.h"
#include  "SVGAnimationElementImpl.h"
#include  "SVGMetadataElementImpl.h"
#include  "SVGDescElementImpl.h"
#include  "SVGDefsElementImpl.h"
#include  "SVGTitleElementImpl.h"
#include  "SVGFontElementImpl.h"
#include  "SVGFontFaceElementImpl.h"
#include  "SVGGlyphElementImpl.h"
#include  "SVGMissingGlyphElementImpl.h"
#include  "SvgHkernelementimpl.h"

#include "SVGLinearGradientElementImpl.h"
#include "SVGRadialGradientElementImpl.h"
#include "SvgStopElementImpl.h"
#include "SVGDiscardElementImpl.h"
#include "SVGScriptElementImpl.h"

#include "SVGAudioElementImpl.h"

//#ifdef RD_SVGT_MEDIAANIMATION_SUPPORT
#include "SVGMediaAnimationElementImpl.h"
//#endif
#include "SVGTextAreaElementImpl.h"
#include "SVGSolidColorElementImpl.h"

#include "SVGFloatCssValueImpl.h"

#include "SVGEventHandler.h"
#include "SVGErrorImpl.h"
#include "SVGFontHashMap.h"
#include "SVGTimeContainer.h"
#include <ezgzip.h>

//#include <caf/caf.h>
#include <bautils.h>

// ==========================================================================
// Need method description
// ==========================================================================
CSvgDocumentImpl* CSvgDocumentImpl::NewL( CSvgBitmapFontProvider* aSvgBitmapFontProvider, const TBool aHasParent,
    const TSvgSyncBehaviour aSyncBehavDefault,
    const TInt32 aSyncTolDefault )
    {
    CSvgDocumentImpl*   self    = new ( ELeave ) CSvgDocumentImpl(aSvgBitmapFontProvider, 
        aHasParent, aSyncBehavDefault, aSyncTolDefault );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgDocumentImpl* CSvgDocumentImpl::NewLC( CSvgBitmapFontProvider* aSvgBitmapFontProvider, const TBool aHasParent,
    const TSvgSyncBehaviour aSyncBehavDefault,
    const TInt32 aSyncTolDefault )
    {
    CSvgDocumentImpl*   self    = new ( ELeave ) CSvgDocumentImpl(aSvgBitmapFontProvider,
        aHasParent, aSyncBehavDefault, aSyncTolDefault );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::ConstructL()
    {
    iSchemaData = CSvgSchemaData::NewL();


    iEventHandler = CSvgEventHandler::NewL();

    // create CSvgErrorImpl object
//   iSvgError = CSvgErrorImpl::NewL();
    iIsInteractive = EFalse;
    iHasGroupOpacity = EFalse;


//	iImageHashMap = CSvgImageHashMap::NewL();
	iFontHashMap = CSvgFontHashMap::NewL();

	iMemoryManager = CSvgMemoryManager::NewL();

		iTimeForJSR226 = 0;
    // Create the time container used for Runtime Sync
    iTimeContainer = CSvgTimeContainer::NewL( this, iHasParent );
    
    // Add to the time container
    iTimeContainer->AddTimedEntityL( this );
    
    //set media state to ready as default for document
    iTimeContainer->TimedEntityReady( this );
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgDocumentImpl::CSvgDocumentImpl( CSvgBitmapFontProvider* aSvgBitmapFontProvider, const TBool aHasParent,
    const TSvgSyncBehaviour aSyncBehavDefault,
    const TInt32 aSyncTolDefault ) : 
                                       iInitSortList( ETrue ),
                                       iReqExReqFtrSysLTested( EFalse ),
                                       iFinishedParsing( EFalse ),
                                       iFontHashMap( NULL ),
                                       iEngine(NULL),
                                       iMultipleRendering( EFalse ),
                                       iHasGradientElement( EFalse ),
                                       iIsThumbNailMode( EFalse ),
                                       iIsDRMProtected( EFalse ),
                                       iHasParent( aHasParent ),
                                       iSyncBehaviorDefault( 
                                        aSyncBehavDefault ),
                                       iSyncToleranceDefault( 
                                        aSyncTolDefault),
                                        iSvgBitmapFontProvider(aSvgBitmapFontProvider)
                                                                                        
                                        

    {
    SetDRMMode( ETrue );
    SetDRMRights( ETrue );
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgDocumentImpl::~CSvgDocumentImpl()
    {
    if ( iTimeContainer )
       {
       // Stop timer and reset time
       iTimeContainer->UserStop();
       iTimeContainer->UserResetTime();
       }

    if (iPerfText)
    {
        delete iPerfText;
        iPerfText = NULL;
    }

    if( iSchemaData )
        {
        delete iSchemaData;
        iSchemaData = NULL;
        }

    if( iEventHandler )
        {
        delete iEventHandler;
        iEventHandler = NULL ;
        }

//    if( iSvgError )
//        {
//        delete iSvgError;
//        iSvgError = NULL;
//        }

    if( iRootElement )
        {
        delete iRootElement;
        iRootElement = NULL;
        }

/*	if ( iImageHashMap )
	{
		//probably should check to verify that the image ptrs
		//in this have had their images deleted somehow to be safe
		delete iImageHashMap;
		iImageHashMap = NULL;
	} */

	if ( iFontHashMap )
	{
		delete iFontHashMap;
		iFontHashMap = NULL;
	}
    iSvgMouseListeners.Close();

	iSvgAnimations.Close();

    if ( iXmlHandler )
        {
        delete iXmlHandler;
        }

    if ( iError )
        {
        delete iError;
        }

 	if ( iMemoryManager )
 		{
 		delete iMemoryManager;
    	}

    delete iTimeContainer;
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::SetEngine( CSvgEngineImpl* aEngine )
    {
    iEngine = aEngine;
    // Propogate this to all child documentsas well
    // Only animation elements currently possess a new document
    // Locate all the active animation elements
    RPointerArray<CSvgElementImpl> lAnimationEleList;
    
    FindAllElements( 
            (CSvgElementImpl* )RootElement(),
            KSvgMediaAnimationElement, lAnimationEleList );
    // Set the engine on the child documents associated with the animation elements as well
    TInt lAnimationEleCnt = lAnimationEleList.Count();
    for ( TInt lCurAnimationEle = 0; lCurAnimationEle < lAnimationEleCnt; lCurAnimationEle++ )
        {
        CSvgMediaAnimationElementImpl* lAnimationElement = 
            (CSvgMediaAnimationElementImpl* )lAnimationEleList[ lCurAnimationEle ];
        CSvgDocumentImpl* lChildDoc = lAnimationElement->GetChildDocument();
        if ( lChildDoc )
            {
            lChildDoc->SetEngine( aEngine );
            }
        }
    lAnimationEleList.Close();
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgEngineImpl* CSvgDocumentImpl::Engine()
    {
    return iEngine;
    }

//***********************************************************************
// From MXmlDocument
//

// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgDocumentImpl::CreateElementL( const TDesC& aTagName )
    {

    TInt position = iSchemaData->GetSVGElementId(aTagName);

    if ( position == KErrNotFound )
        {
        return NULL;
        }

    	return CreateElementL( position );

    }

    // ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgDocumentImpl::CreateElementL(const TUint8 aTagName )
    {

    TInt position = (TInt) aTagName;

    //##4 this uses the rearrangement of elements, all these elements are same as g element
    // same constructor
    //
    // =====> creating fake G elements to take the place of others...
    //
    //need to remove G elements as place holders that is a bad idea

    if( position >= KSvgAltglyphElement && position <= KSvgViewElement )
        {
        return ( MXmlElement * ) CSvgGElementImpl::NewL( (TUint8) position, this );
        }

    switch ( position )
        {

        case KSvgPathElement:
            // path
            return ( MXmlElement * ) CSvgPathElementImpl::NewL(  (TUint8) position, this );

        case KSvgStopElement:
        	// stop
            return ( MXmlElement * ) CSvgStopElementImpl::NewL((TUint8) position,this);

        case KSvgLinearGradientElement:
        	// linearGradient
            return ( MXmlElement * ) CSvgLinearGradientElementImpl::NewL((TUint8) position,this);

        case KSvgRectElement:
            // rect
            return ( MXmlElement * ) CSvgRectElementImpl::NewL(  (TUint8) position, this );

        case KSvgPolygonElement:
        	// polygon
            return ( MXmlElement * ) CSvgPolylineElementImpl::NewL( (TUint8) position, this );

        case KSvgSvgElement:
            // svg
            return ( MXmlElement * ) CSvgSvgElementImpl::NewL( (TUint8) position, this );

        case KSvgRadialGradientElement:
        	// radialGradient
            return ( MXmlElement * ) CSvgRadialGradientElementImpl::NewL((TUint8) position,this);

        case KSvgCircleElement:
            // circle
            return ( MXmlElement * ) CSvgCircleElementImpl::NewL( (TUint8) position, this );

        case KSvgLineElement:
            // line
            return ( MXmlElement * ) CSvgLineElementImpl::NewL( (TUint8) position,this );

        case KSvgPolylineElement:
            // polyline
            return ( MXmlElement * ) CSvgPolylineElementImpl::NewL( (TUint8) position, this );

        case KSvgEllipseElement:
            // ellipse
            return ( MXmlElement * ) CSvgEllipseElementImpl::NewL( (TUint8) position, this );

       case KSvgForeignObjectElement:
			// foreignObject
    		return ( MXmlElement * ) CSvgForeignObjectElementImpl::NewL((TUint8) position, this);

        case KSvgStyleElement:
            // style
            return ( MXmlElement * ) CSvgStyleElementImpl::NewL( (TUint8) position, this );

        case KSvgUseElement:
            // use
            return ( MXmlElement * ) CSvgUseElementImpl::NewL( (TUint8) position, this );

        case KSvgImageElement:
            // image
            return ( MXmlElement * ) CSvgImageElementImpl::NewL( (TUint8) position, this );

        case KSvgAnimateColorElement:
            // animateColor
        case KSvgAnimateElement:
            {
            // animate
            CSvgAnimateElementImpl* lAnimateElement = CSvgAnimateElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append(lAnimateElement);
            return ( MXmlElement * ) lAnimateElement;
            }

        case KSvgSetElement:
            // set
            return ( MXmlElement * ) CSvgSetElementImpl::NewL( (TUint8) position, this );

        case KSvgMpathElement:
        	// mPath
            return ( MXmlElement * ) CSvgMpathElementImpl::NewL( (TUint8) position, this );

        case KSvgDiscardElement:
  			// discard
            return ( MXmlElement * ) CSvgDiscardElementImpl::NewL( (TUint8) position, this );

 /*       case KSvgAnimationElement:
            {
            // animation
            CSvgAnimationElementImpl* lAnimationElementImpl = CSvgAnimationElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append( lAnimationElementImpl );
            return ( MXmlElement * ) lAnimationElementImpl;
            }
*/
        case KSvgScriptElement:
            {
            // script
            return ( MXmlElement * ) CSvgScriptElementImpl::NewL((TUint8) position,this);
            }

        case KSvgSolidColorElement:
            // solidColor
            return ( MXmlElement * ) CSvgSolidColorElementImpl::NewL((TUint8) position,this);

        case KSvgMetadataElement:
        	// metaData
        	return ( MXmlElement * ) CSvgMetadataElementImpl::NewL((TUint8) position, this);

        case KSvgDescElement:
        	// desc
        	return ( MXmlElement * ) CSvgDescElementImpl::NewL((TUint8) position, this);

        case KSvgTitleElement:
        	// title
        	return ( MXmlElement * ) CSvgTitleElementImpl::NewL((TUint8) position, this);

        case KSvgTextElement:
            // text
            //add in the boolean thing here....
            return ( MXmlElement * ) CSvgTextElementImpl::NewL(  (TUint8) position, this );

        case KSvgTextAreaElement:
            {
            // textArea
            return ( MXmlElement * ) CSvgTextAreaElementImpl::NewL((TUint8) position,this);
            }

        case KSvgAnimateMotionElement:
            {
            // animateMotion
            CSvgAnimateMotionElementImpl* lAnimateMotionElement = CSvgAnimateMotionElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append(lAnimateMotionElement);
            return ( MXmlElement * ) lAnimateMotionElement;
            }

        case KSvgAnimateTransformElement:
            {
            // animateTransform
            CSvgAnimateTransformElementImpl* lAnimateTransformElement = CSvgAnimateTransformElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append(lAnimateTransformElement);
            return ( MXmlElement * ) lAnimateTransformElement;
            }

        case KSvgGlyphElement:
            // glyph
#ifdef SVG_FONTS_INCLUDE
            return ( MXmlElement * ) CSvgGlyphElementImpl::NewL( (TUint8) position, this );
#else
            return ( MXmlElement * ) CSvgGElementImpl::NewL( _L( "g" ),(TUint8) position, this );
#endif
        case KSvgFontElement:
            // font
#ifdef SVG_FONTS_INCLUDE
            return ( MXmlElement * ) CSvgFontElementImpl::NewL( (TUint8) position, this );
#else
            return ( MXmlElement * ) CSvgGElementImpl::NewL( _L( "g" ),(TUint8) position, this );
#endif

        case KSvgAElement:
            // a
            return ( MXmlElement * ) CSvgAElementImpl::NewL( (TUint8) position, this );

        case KSvgFontfaceElement:
            // font-face
#ifdef SVG_FONTS_INCLUDE
            return ( MXmlElement * ) CSvgFontFaceElementImpl::NewL( (TUint8) position, this );
#else
            return ( MXmlElement * ) CSvgGElementImpl::NewL( _L( "g" )(TUint8) position, this );
#endif

        case KSvgMissingglyphElement:
            // missing-glyph
#ifdef SVG_FONTS_INCLUDE
            return ( MXmlElement * ) CSvgMissingGlyphElementImpl::NewL( (TUint8) position, this );
#else
            return ( MXmlElement * ) CSvgGElementImpl::NewL( _L( "g" )(TUint8) position, this );
#endif

        case KSvgHkernElement:
            // hkern
#ifdef SVG_FONTS_INCLUDE
            return ( MXmlElement * ) CSvgHkernElementImpl::NewL( (TUint8) position, this );
#else
            return ( MXmlElement * ) CSvgGElementImpl::NewL( _L( "g" )(TUint8) position, this );
#endif

        case KSvgAudioElement:
            {
            CSvgAudioElementImpl* lAudioElement = CSvgAudioElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append(lAudioElement);
            return ( MXmlElement * ) lAudioElement;
            }
//#ifdef RD_SVGT_MEDIAANIMATION_SUPPORT
        case KSvgMediaAnimationElement:
            {
            CSvgMediaAnimationElementImpl* lMediaAnimationElement = CSvgMediaAnimationElementImpl::NewL( (TUint8) position, this );
            iSvgAnimations.Append(lMediaAnimationElement);
            return ( MXmlElement * ) lMediaAnimationElement;
            }
//#endif

        } // for switch

        return NULL;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgDocumentImpl::CreateAttribute( const TDesC& /* aName */ )
    {
    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgDocumentImpl::GetElementById( const TDesC& aElementId )
    {
    if(iRootElement)
        {
            const TDesC* myId = iRootElement->Id();

                if (myId)
                {
                    if (myId->Length() > 0)
                    {
                        if ( *myId == aElementId )
                        {
                            return iRootElement;
                        }
                    }
                }

    MXmlElement* element = SearchElementById( iRootElement, aElementId );

    return element;
        }
    else
        {
        return NULL;
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgDocumentImpl::GetNumberOfIds(MXmlElement* aElement)
{
    TInt count = 0;

    CSvgElementImpl* child = ( CSvgElementImpl* ) aElement->FirstChild();
    while ( child != NULL )
        {
            const TDesC* lPtr = child->Id();

            if (lPtr)
            {
                count++;
            }
                // search children
                TInt inside_count = GetNumberOfIds( child );
                if ( inside_count > 0 )
                {
                    count = count + inside_count;
                }
                // search siblings

                child = ( CSvgElementImpl * ) child->NextSibling();
        }

   return count;
}

// ==========================================================================
// Need method description
// ==========================================================================
TDesC* CSvgDocumentImpl::GetId(TInt index)
{
				TDesC* id = NULL;
        RPointerArray<TDesC> ids;

        FindAllIds( (CSvgElementImpl*)RootElement(), ids );

        if (index < ids.Count())
        {
            id = ids[index];
        }

        ids.Close();

    return id;
}

// ==========================================================================
// // Return all elements of the given type
// ==========================================================================
void CSvgDocumentImpl::FindAllIds( CSvgElementImpl* aStartElement, RPointerArray<TDesC>& aList )
{
    if ( aStartElement == NULL )
        return;

    CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
    while ( child != NULL )
    {
        // add to list if child is found
        const TDesC* myId = child->Id();

        if ( myId )
            aList.Append( myId );

        // find in grandchildren
        FindAllIds( child, aList );
        child = (CSvgElementImpl*)child->NextSibling();
    }
}


//***********************************************************************
// From MSvgDocument
//



//
// ==========================================================================
// Need method description
// ==========================================================================
TDesC& CSvgDocumentImpl::GetUrl()
    {
    return iUri;
    }

// ==========================================================================
// // Returns the value of the current focus index. 
// ==========================================================================

TInt32 CSvgDocumentImpl::GetCurFocusIndex()
{
	return iCurObjIdx;
}


// ==========================================================================
// // Increment the focus index by one value
// ==========================================================================

TInt32 CSvgDocumentImpl::IncCurFocusIndex()
{
	return ++iCurObjIdx;
}

// ==========================================================================
// // Decrement the focus index by one value
// ==========================================================================
TInt32 CSvgDocumentImpl::DecCurFocusIndex()
{
	return --iCurObjIdx;
}

// ==========================================================================
// // Sets the focus index to the given value
// ==========================================================================
void CSvgDocumentImpl::SetCurFocusIndex(TInt32 aVal)
{
	iCurObjIdx = aVal;
}

// ==========================================================================
// // Returns the current focus object
// ==========================================================================
CSvgElementImpl* CSvgDocumentImpl::GetCurFocusObject()
{
	return iCurrentFocusObject;
}

// ==========================================================================
// // Sets the current focus element to the element specified
// ==========================================================================
void CSvgDocumentImpl::SetCurFocusObject(CSvgElementImpl* aElement)
{
	iCurrentFocusObject = aElement;
}

//
// ==========================================================================
// Need method description
// ==========================================================================
EXPORT_C MXmlElement* CSvgDocumentImpl::RootElement()
    {
    return iRootElement;
    }

//***********************************************************************
//
//

// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgDocumentImpl::SearchElementById( MXmlElement* aElement,
                                                  const TDesC& aElementId )
    {
    CSvgElementImpl* child = ( CSvgElementImpl* ) aElement->FirstChild();
    while ( child != NULL )
        {
            const TDesC* lPtr = child->Id();

            if (lPtr)
            {
                if ( *lPtr == aElementId )
                {
                    return child;
                }
            }
                // search children
                MXmlElement* childrenMatch = SearchElementById( child, aElementId );
                if ( childrenMatch != NULL )
                {
                    return childrenMatch;
                }
                // search siblings

                child = ( CSvgElementImpl * ) child->NextSibling();

        }
    return NULL;
    }




// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgDocumentImpl::AppendChildL( MXmlElement* aNewChild )
    {

    if ( aNewChild && ((CXmlElementImpl*)aNewChild)->ElemID() == KSvgSvgElement )

        {
        // Set new node as the root element, if it is
        if ( iRootElement )
            {
            return NULL;
            }
        iRootElement = (CSvgElementImpl *) aNewChild;

        // Set the new node's next sibling
        aNewChild->SetNextSibling( NULL );
        }
    else
        {
        return NULL;
        }

    return aNewChild;
    }


// ==========================================================================
// Need method description
// ==========================================================================
CSvgSchemaData* CSvgDocumentImpl::SchemaData()
    {
    return iSchemaData;
    }


// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::SetUri( const TDesC& aUri )
    {
    iUri.Zero();
    iUri.Copy(aUri);
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::GetUri( TDes& aUri )
    {
    aUri.Zero();
    aUri.Copy(iUri);
    }

// ==========================================================================
// Register an element for receiving events using the event mask
// ==========================================================================
void CSvgDocumentImpl::AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                             TUint8 aEventMask)
    {
    if (iEventHandler != NULL)
    iEventHandler->AddToEventReceiverListL( aElement, aEventMask );
    }

// ==========================================================================
// Register an element for receiving events and events using the event mask
// ==========================================================================
void CSvgDocumentImpl::AddToEventReceiverListL( MSvgEventReceiver* aElement,
                                                TSvgEvent aEvent,
                                             TUint8 aEventMask)
    {
    if (iEventHandler != NULL)
    iEventHandler->AddToEventReceiverListL( aElement,aEvent,aEventMask );
    }

// ==========================================================================
// Unregister an element for receiving events using the event mask
// ==========================================================================
void CSvgDocumentImpl::RemoveFromEventReceiverList( MSvgEventReceiver* aElement )
    {
    if (iEventHandler != NULL)
    iEventHandler->RemoveFromEventReceiverList( aElement );
    }

// ==========================================================================
// Provide event handler the event timing information for the
// completion of event.
// ==========================================================================
void CSvgDocumentImpl::AddEventBeginTime(MSvgEventReceiver* aElement, TUint32 aTime, MSvgEventReceiver* aTargetElement )
    {
    if (iEventHandler != NULL)
    iEventHandler->AddEventBeginTime( aElement, aTime, aTargetElement );
    }

// ==========================================================================
// Sort the events in a time scale according to their absolute start
// and finish times
// ==========================================================================
void CSvgDocumentImpl::SortEventList()
    {
    if (iEventHandler != NULL)
    iEventHandler->SortEventList();
    }

//
// ==========================================================================
// Returns an element that is registered for the given event mask and
// is the first one starting from the given index (either up or down the
// list based on the aNext parameter)
// ==========================================================================
CSvgElementImpl* CSvgDocumentImpl::GetEventReceiverElement(TInt32 aIndex, TBool aNext, TUint8 aEventMask, TInt32& aNewIndex)
    {
    if (iEventHandler != NULL)
    {
            return (CSvgElementImpl*)iEventHandler->GetEventReceiver(aIndex, aNext, aEventMask, aNewIndex);
    }
    else
    return NULL;

    }

// ==========================================================================
// Is Animation file
// ==========================================================================
TBool CSvgDocumentImpl::IsAnimationFile()
    {
    if( (iEventHandler != NULL) && iEventHandler->Count() )
        return ETrue;
    else
        return EFalse;
    }
    
// ==========================================================================
// IsValidSubEventMask
// ==========================================================================
TBool CSvgDocumentImpl::IsValidSubEventMask(TUint16 aSubEventMask)
    {
    if(iEventHandler)
        {
        return(iEventHandler->IsValidSubEventMask(aSubEventMask));
        }
    else
        {
        return EFalse;    
        }
    }
// ==========================================================================
// Set DRM Mode
// ==========================================================================

void CSvgDocumentImpl::SetDRMMode(TBool aEnable)
    {
    iDrmEnable = aEnable;
    }
// ==========================================================================
// Set DRM Mode
// ==========================================================================
void CSvgDocumentImpl::Reset(MSvgEvent *aEvent)
    {
    if ( iAnimationResetNeeded && iEventHandler != NULL )
        {
        iEventHandler->Reset( aEvent );
        iAnimationResetNeeded = EFalse;
        }
    }
// ==========================================================================
// Set DRM Mode
// ==========================================================================
TBool CSvgDocumentImpl::SvgElementPresent(CSvgElementImpl* aElement)
{
    if(iRootElement == NULL) return EFalse;
    if ( ( ( CSvgElementImpl * ) iRootElement ) == aElement )
        {
        return ETrue;
        }

    return SearchByPointer(iRootElement, aElement);

}
// ==========================================================================
// Set DRM Mode
// ==========================================================================
TBool CSvgDocumentImpl::SearchByPointer(CSvgElementImpl* aParent, CSvgElementImpl* aElement)
{
    CSvgElementImpl* child = ( CSvgElementImpl* ) aParent->FirstChild();
    while ( child != NULL )
        {

        if ( child  == aElement )
            {
            return ETrue;
            }
        // search children
        TBool result = SearchByPointer( child, aElement );
        if (result)
            {
            return ETrue;
            }
        // search siblings
        child = ( CSvgElementImpl * ) child->NextSibling();
        }
    return EFalse;
}
// ==========================================================================
// Set DRM Mode
// ==========================================================================
void CSvgDocumentImpl::SetFocusElement(CXmlElementImpl* aElement )
{
    iCurrentFocusObject = (CSvgElementImpl*)aElement;
}
// ==========================================================================
// Set DRM Mode
// ==========================================================================
CXmlElementImpl* CSvgDocumentImpl::GetFocusElement()
{
    return (CXmlElementImpl*) iCurrentFocusObject;
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::ReInitialize()
    {
    if(iMultipleRendering)
        {
        if (iEventHandler != NULL)
        iEventHandler->ReInitialize();
        }
    else
        {
        iMultipleRendering= ETrue;
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::Load( const TDesC& aFileName, CSvgErrorImpl& aError )
{
    aError.SetErrorCode( ESvgNoError );

    RFs session;
    OpenSession( session, aError );
    if ( aError.HasError() )
        return;

    RFile fileHandle;
    TInt openError = fileHandle.Open( session, aFileName, EFileRead );
    if ( openError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, openError,
                      _L( "Fail to open file for reading: " ), aFileName );
        session.Close();
        return;
    }
    else
    {
        Load( fileHandle, aError );
        session.Close();
    }
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::Load( const TDesC8& aByteData, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements )
{
    aError.SetErrorCode( ESvgNoError );

    //-------------------------------------------------------------------
    // Byte array is gzipped and/or drm:
    // Write buffer to file:
    // a) GZip only has filename function to unzip
    // b) Drm only has file-handle to decrypt
    //-------------------------------------------------------------------
    if ( IsGzipContent( aByteData ) ||
         ( iDrmEnable && IsDRMContent( aByteData ) ) )
    {
        RFs session;
        OpenSession( session, aError );
        if ( aError.HasError() )
            return;

        // Write byte-array to temp file
        TFileName zippedTempFilename;
        if ( WriteToTempFile( session, aByteData, zippedTempFilename, aError ) != KErrNone )
        {
            session.Close();
            return;
        }
        Load( zippedTempFilename, aError );
        session.Delete( zippedTempFilename );
        session.Close();
    }
    //-------------------------------------------------------------------
    // byte-array is neither gzipped nor DRM encrypted
    //-------------------------------------------------------------------
    else
    {
        iIsDRMProtected = EFalse;
        iEventHandler->Reset();
        TRAPD(error,ProcessSvgContentL( aByteData, aError, aRemoveFalseSwitchElements ));
        if ( error != KErrNone )
        {
            PrepareError( aError, ESvgNoMemory, error,
                          _L( "Out of Memory: " ),
                          _L( "Instantiating Parser" ) );
        }

        iInitialDrawFlag = ETrue;
        iFinishedParsing = ETrue;

    }
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::Load( RFile& aFileHandle, CSvgErrorImpl& aError )
{
    aError.SetErrorCode( ESvgNoError );

    RFs session;
    OpenSession( session, aError );
    if ( !aError.HasError() )
    {
        Load( session, aFileHandle, aError );
        session.Close();
    }
}
// -----------------------------------------------------------------------------
// CSvgDocumentImpl::ParentTimeContainerTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//
void CSvgDocumentImpl::ParentTimeContainerTick( 
    TSvgTick aTick ) // Current tick information 
    {
    TBool lDoRedraw = ETrue;
    if ( iHasParent )
        {
        // Do not redraw in child document to avoid
        // double redraws.
        lDoRedraw = EFalse;
        }
    // Create timer event and propogate to engine
    TSvgTimerEvent lTimerEvent( aTick.iParentTcTick );
    // Send to engine to process the timer event
    TRAPD( lProcEvtErr, Engine()->ProcessEventL( 
        this, &lTimerEvent, lDoRedraw ) );
    if ( lProcEvtErr != KErrNone )
        {
        // Error Processing
        }
    }
    
// -----------------------------------------------------------------------------
// CSvgDocumentImpl::GetEntitySyncBehavior
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
TSvgSyncBehaviour CSvgDocumentImpl::GetEntitySyncBehavior()
    {
    // Document is locked with the parent tc timeline
    return ESvgSyncLocked; 
    }
        

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::GetEntityCurrentTime
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgDocumentImpl::GetEntityCurrentTime( TUint32& 
            /* aEntityCurTime */) // Current Entity Time in msecs. 
    {
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::GetCnfSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
//        
void CSvgDocumentImpl::GetCnfSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is configured as 
                                   // Sync Master. 
    {
    // Document can never be a sync master
    aIsSyncMaster = EFalse;
    }                                   

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::GetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::GetCurSyncMasterStatus( 
            TBool& aIsSyncMaster ) // Indicates whether the element is currrently 
                                  // Sync Master. 
    {
    // Document can never be a sync master
    aIsSyncMaster = EFalse;
    }        


// -----------------------------------------------------------------------------
// CSvgDocumentImpl::SetCurSyncMasterStatus
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::SetCurSyncMasterStatus( 
    TBool /*aSyncMasterStatus */) // Indicates whether the element is 
                                  // currrently Sync Master.
    {
    // Document sync master status is always false as it can never be 
    // a sync master
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::CanGenerateTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgDocumentImpl::CanGenerateTick()
    {
    // Document cannot generate tick, as it is not a inherently timed element
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::CanUseParentTick
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
TBool CSvgDocumentImpl::CanUseParentTick()
    {
    // Return True as document can always use parent tick to 
    // advance itself
    return ( ETrue );
    }
    
// -----------------------------------------------------------------------------
// CSvgDocumentImpl::ResyncTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::ResyncTimedEntity( 
            TUint32 /*aSynctime*/ ) // Time for resync in msecs.
    {
    }
    


// -----------------------------------------------------------------------------
// CSvgDocumentImpl::PauseTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::PauseTimedEntity()
    {
    
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::ResumeTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::ResumeTimedEntity()
    {
    
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::StopTimedEntity
// From MSvgTimedEntityInterface
// -----------------------------------------------------------------------------
void CSvgDocumentImpl::StopTimedEntity()
    {
    iEventHandler->ResetTimes();
    }


//From MSvgTimedEntityInterface
TSvgObjectType CSvgDocumentImpl::ObjectType()
{
	return ESvgDocumentElement;	
}


//returns the child time container of the element 
//used in timecontainer
CSvgTimeContainer* CSvgDocumentImpl::GetChildTimeContainer()
{
    return NULL;
}
    

//if node has a child
//		visit the child call postorder on that
//add current node
//if there are siblings 
//		visit all siblings
void CSvgDocumentImpl::ParsePostOrderMediaElements( CSvgElementImpl* aRoot, 
    RPointerArray<CSvgElementImpl>& aPostOrderList)
    {
    if ( !aRoot )
        {
        return;
        }
        
	CSvgElementImpl* lNodePtr = ( CSvgElementImpl* )aRoot->FirstChild();
	while ( lNodePtr != NULL )
	    {
	    ParsePostOrderMediaElements( lNodePtr, aPostOrderList );
        lNodePtr = (CSvgElementImpl* )(lNodePtr->NextSibling() );
	    }
	
	// Add only media elements, currently animation and audio
	if ( aRoot->ElemID() == KSvgMediaAnimationElement || 
	    aRoot->ElemID() == KSvgAudioElement )	
	    {
	    aPostOrderList.Append( aRoot );	    
	    }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::Load( RFs& aSession, RFile& aFileHandle, CSvgErrorImpl& aError )
{
    iEventHandler->Reset();

    RFile ungzippedFile;
    RFile* fileHandle = &aFileHandle;
    TFileName tempFilename;
    TBool needToDeleteTempFile = EFalse;
    iIsDRMProtected = EFalse;

    //************************************************
    // Check for gzip format: write to temp file
    //************************************************
    // Ignore Gzip content due to platform security: uncompressed file must be created
    if ( IsGzipContent( aFileHandle ) )
    {
        //************************************************
        // Ungzipped content may be DRM or txt-svg or binary-svg
        //************************************************
        needToDeleteTempFile = ETrue;
        if ( !ProcessGZipContent( aSession, aFileHandle, tempFilename, aError ) )
            return;

        // Open ungzipped file
        TInt openError = ungzippedFile.Open( aSession, tempFilename, EFileRead );
        if ( openError != KErrNone )
        {
            // couldn't open temp file for writing
            PrepareError( aError, ESvgUnknown, openError,
                          _L( "Loading GZipped SVG File failed: " ),
                          _L( "Error Opening Temp File for Reading." ) );

            // Delete uncompress temp file if needed
            if ( needToDeleteTempFile )
                aSession.Delete( tempFilename );
            return;
        }
        fileHandle = &ungzippedFile;
    }

    HBufC8* svgByteArray = NULL;
    //************************************************
    // Decrypt DRM
    //************************************************
    if ( iDrmEnable && IsDRMContent( *fileHandle ) )
    {
        TRAPD( drmError, svgByteArray = ProcessDRMContentL( *fileHandle, aError ) );
        if ( drmError != KErrNone )
        {
            PrepareError( aError, ESvgNoMemory, drmError,
                          _L( "Out of Memory: " ),
                          _L( "Instantiating DRM Decoder" ) );
            return;
        }
        // drm failed, error should already be set
        else if ( svgByteArray == NULL )
        {
            return;
        }

        if ( needToDeleteTempFile && ( drmError || svgByteArray == NULL ) )
        {
            aSession.Delete( tempFilename );
        }
        iIsDRMProtected = ETrue;
    }
    // Read Non encrypted data
    else
    {
        //************************************************
        // Read plain file: txt-svg or binary-svg
        //************************************************
        TInt fileLength = 0;
        fileHandle->Size( fileLength );
        TRAPD( error, svgByteArray = HBufC8::NewL( fileLength ) );
        if ( error != NULL )
        {
            PrepareError( aError, ESvgNoMemory, error,
                          _L( "Out of Memory" ),
                          _L( "Allocating byte-array for data" ) );
            if ( needToDeleteTempFile )
            {
                aSession.Delete( tempFilename );
            }
            return;
        }
        TInt pos = 0;
        fileHandle->Seek( ESeekStart, pos );
        TPtr8 des = svgByteArray->Des();
        TInt readError = fileHandle->Read( des );
        if ( readError != KErrNone )
        {
            PrepareError( aError, ESvgUnknown, readError,
                          _L( "Reading SVG File Failed." ), _L( "" ) );
            delete svgByteArray;
            if ( needToDeleteTempFile )
            {
                aSession.Delete( tempFilename );
            }
            return;
        }
    }

    //************************************************
    // Parser binary or plain svg content
    //************************************************
    TRAPD( svgError, ProcessSvgContentL( *svgByteArray, aError ) );
    if ( svgError != KErrNone || ( aError.HasError() && !aError.IsWarning() ) )
    {
        delete svgByteArray;
        if ( !aError.HasError() )
        {
            PrepareError( aError, ESvgNoMemory, svgError,
                           _L( "Out of Memory: " ),
                           _L( "Instantiating SVG ContentHandler/Decoder" ) );
        }
        if ( needToDeleteTempFile )
            aSession.Delete( tempFilename );
        return;
    }

    iInitialDrawFlag = ETrue;
    iFinishedParsing = ETrue;

    delete svgByteArray;


    // Delete uncompress temp file if needed
    if ( needToDeleteTempFile )
    {
        // Close only when file handle is for temp file
        fileHandle->Close();
        aSession.Delete( tempFilename );
    }

    // Prepare engine to draw document
    LoadingCompleted();
}

// ==========================================================================
// Need method description
// Leaves only when allocating memory fails
// Reports other errors
// ==========================================================================
void CSvgDocumentImpl::ProcessSvgContentL( const TDesC8& aByteArray, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements )
{
    //************************************************
    // Determine binary or xml svg file
    //************************************************
    if ( IsBinarySvg( aByteArray ) )
    {
        TInt error = KErrNone;
        CSvgDecoder* decoder = CSvgDecoder::NewL( aByteArray );

        TRAP( error, iRootElement = decoder->StartDecodingL( this, aError ) );
        if ( error != KErrNone || iRootElement == NULL && !aError.HasError() )
        {
            if ( error != KErrNone )
            {
                PrepareError( aError, ESvgbFileNotValid, error,
                              _L( "Invalid binary file." ), _L( "" ) );
            }
        }
        delete decoder;
    }
    else
    {
        iXmlHandler = CSvgContentHandler::NewL( aRemoveFalseSwitchElements );
        iRootElement = iXmlHandler->ParseByteData( this, aByteArray, aError );
        delete iXmlHandler;
        iXmlHandler = NULL;
    }
}

// ==========================================================================
// JSR226 only!!
// Allows filling of document from JSR side with 16 bit string instead of 8
//
// ==========================================================================
void CSvgDocumentImpl::Load16BitString( const TDesC16& aSvgString, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements )
{
    aError.SetErrorCode( ESvgNoError );

    iIsDRMProtected = EFalse;

    if (iEventHandler)
    iEventHandler->Reset();
    TRAPD(error,Process16BitSvgContentL( aSvgString, aError, aRemoveFalseSwitchElements ));
    if ( error != KErrNone )
    {
    	PrepareError( aError, ESvgNoMemory, error,
                          _L( "Out of Memory: " ),
                          _L( "Instantiating Parser" ) );
    }

    iInitialDrawFlag = ETrue;
    iFinishedParsing = ETrue;
}

void CSvgDocumentImpl::Process16BitSvgContentL( const TDesC16& aSvgString, CSvgErrorImpl& aError, TBool aRemoveFalseSwitchElements )
{
    iXmlHandler = CSvgContentHandler::NewL( aRemoveFalseSwitchElements );
    iRootElement = iXmlHandler->Parse16BitData( this, aSvgString, aError );
    delete iXmlHandler;
    iXmlHandler = NULL;
}
// END OF JSR226 ONLY
// ==========================================================================

// ==========================================================================
void CSvgDocumentImpl::CancelParsing()
    {
    // Check if there is animation element in the parent svg
    RPointerArray<CSvgElementImpl> lAnimationEleList;
    FindAllElements((CSvgElementImpl* )RootElement(),
               KSvgMediaAnimationElement, lAnimationEleList );
    TInt lAnimationEleCnt = lAnimationEleList.Count();    
    for ( TInt lCurAnimationEle = 0; 
            lCurAnimationEle < lAnimationEleCnt; lCurAnimationEle++ )
        {
        CSvgMediaAnimationElementImpl* lAnimationElement = 
            (CSvgMediaAnimationElementImpl* )lAnimationEleList[ lCurAnimationEle ];
        CSvgDocumentImpl* lChildDoc = lAnimationElement->GetChildDocument();
        if ( lChildDoc )
            {
            lChildDoc->CancelParsing(); // Cancel parsing on child svg
            }
        }
    lAnimationEleList.Close();    
    if ( iXmlHandler )
        {
        iXmlHandler->CancelParsing();
        }
    }

// ==========================================================================
TBool CSvgDocumentImpl::IsParsing()
    {
    // xml handler is set to NULL after parsing
    return iXmlHandler != NULL;
    }

//***************************************************************************
// The function isGroupOpacity checks to see if there is a valid opacity
// attribute.
//***************************************************************************

TBool CSvgDocumentImpl::isGroupOpacity( CSvgElementImpl* aElement )
{
	TBool lReturnValue = EFalse;
    CCssValue*  lCssValue = NULL;

    aElement->FindProperty( KCSS_ATTR_GROUP_OPACITY, lCssValue, aElement );
    if ( lCssValue )
       {
        if ( ((CFloatCssValueImpl*)lCssValue)->Value() != KFloatFixOne )
           {
           // valid opacity value
            lReturnValue = ETrue;
            }
        }

    return lReturnValue;
}


// ==========================================================================
// Pre-condition: content is DRM
// The content of a DRM may be svg-text or svg-bin
// Leaves only for memory allocation failure
// ==========================================================================
HBufC8* CSvgDocumentImpl::ProcessDRMContentL( RFile& aFileHandle, CSvgErrorImpl& aError )
{
    // Open File Content and Data
    ContentAccess::CContent* content = ContentAccess::CContent::NewLC( 
            aFileHandle );
    // Note: very important to use EView and NOT EPlay
    ContentAccess::CData* data = content->OpenContentLC( ContentAccess::EView );

    // Read Length of Content
    TInt length = 0;
    TRAPD( sizeError, data->DataSizeL( length ) );
    // Cannot get length of data
    if ( sizeError != KErrNone )
    {
        ProcessDRMError( sizeError, aError );
        CleanupStack::PopAndDestroy( 2 );
        return NULL;
    }

    // Allocate Memory for Content: no leavable command after this
    // so, pushing onto cleanup stack not needed
    HBufC8* buffer = HBufC8::NewL( length );

    TPtr8 des = buffer->Des();
    TInt readError = data->Read( des );

    // Check for read error
    if ( readError != KErrNone )
    {
        ProcessDRMError( readError, aError );
        CleanupStack::PopAndDestroy( 2 );
        delete buffer;
        return NULL;
    }
    // Only consume rights when not in thumb nail mode
    else if ( !iIsThumbNailMode )
    {
        // Do not consume drm rights if iDrmRightsConsumptionEnabled is EFalse
        if(iDrmRightsConsumptionEnabled)
        {
        TInt intentError = data->ExecuteIntent( ContentAccess::EView );
        if ( intentError != KErrNone )
        {
            ProcessDRMError( readError, aError );
            aError.SetIsWarning( ETrue );
            }
        }
    }
    CleanupStack::PopAndDestroy( 2 );
    return buffer;
}

// ==========================================================================
// Un-gzip to a temp file and return the name of temp file
// ==========================================================================
TBool CSvgDocumentImpl::ProcessGZipContent( RFs& aSession,
                                            RFile& aGZipFileHandle,
                                            TFileName& aUnzippedFileName ,
                                            CSvgErrorImpl& aError )
{
    // Write data to temp file: gzip does not have function to read RFile
    TFileName gzipFilename;
    if ( WriteToTempFile( aSession, aGZipFileHandle, gzipFilename, aError ) != KErrNone )
        return EFalse;

    TBool result = ProcessGZipContent( aSession, gzipFilename, aUnzippedFileName, aError );

    // Delete temp gzipped file
    aSession.Delete( gzipFilename );

    return result;
}

// ==========================================================================
// Uncompress to a temporary file
// ==========================================================================
TBool CSvgDocumentImpl::ProcessGZipContent( RFs& aSession,
                                            TFileName& aZippedFileName,
                                            TFileName& aUnzippedFileName ,
                                            CSvgErrorImpl& aError )
{
    // Create ungzipped temp file
    TFileName path;
    GetProcessPrivatePath( aSession, path );
    RFile gzipOutputFile;
    TInt replaceError = gzipOutputFile.Temp( aSession, path, aUnzippedFileName, EFileWrite );
    if ( replaceError != KErrNone )
    {
        // couldn't open temp file for writing
        PrepareError( aError, ESvgUnknown, replaceError,
                      _L( "Failed to create file or unGZip: " ), aUnzippedFileName );
        return EFalse;
    }

    // okay so far, uncompressing
    CEZGZipToFile* uncompressor = NULL;
    TRAPD( uncompressorError,
           uncompressor = CEZGZipToFile::NewL( aSession, aZippedFileName, gzipOutputFile ) )
    if ( uncompressorError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, uncompressorError,
                      _L( "Out of memory: " ),
                      _L( "Instantiating GZip decompressor" ) );
        gzipOutputFile.Close();
        aSession.Delete( aUnzippedFileName );

        return EFalse;
    }

    // Decompress file
    TBool done = EFalse;
    while ( !done )
    {
        TRAPD( inflateError, done = !uncompressor->InflateL() );
        if ( inflateError != KErrNone )
        {
            PrepareError( aError, ESvgUnknown, inflateError,
                          _L( "Uncompressing GZip file failed." ), _L( "" ) );
            delete uncompressor;
            gzipOutputFile.Close();
            aSession.Delete( aUnzippedFileName );

            return EFalse;
        }
    }

    gzipOutputFile.Close();
    delete uncompressor;

    return ETrue;
}

// ==========================================================================
// Error codes taken from caf/content.h
// ==========================================================================
void CSvgDocumentImpl::ProcessDRMError( TInt errorCode, CSvgErrorImpl& aError )
{
    if ( errorCode == KErrNone )
        return;

    if ( errorCode == KErrNotFound )
    {
        aError.SetDescription( _L( "Content Not Found" ) );
    }
    else if ( errorCode == KErrCAPendingRights )
    {
        aError.SetDescription( _L( "Rights Have Not Arrived" ) );
    }
    else if ( errorCode == KErrCANoPermission )
    {
        aError.SetDescription( _L( "No Permission to Play Content" ) );
    }
    else if ( errorCode == KErrCANoRights )
    {
        aError.SetDescription( _L( "No Rights Exists for Content" ) );
    }
    else if ( errorCode == KErrCANotSupported )
    {
        aError.SetDescription( _L( "Unable to Open Content" ) );
    }
    else if ( errorCode == KErrPermissionDenied )
    {
        aError.SetDescription( _L( "No Permission to Open Content" ) );
    }
    else if ( errorCode == KErrAccessDenied )
    {
        aError.SetDescription( _L( "Content Already in Use or No DRM Capability" ) );
    }
    else if ( errorCode == KErrCASizeNotDetermined )
    {
        aError.SetDescription( _L( "Cannot Determine Size of Content" ) );
    }
    else
    {
        aError.SetDescription( _L( "Unknown DRM Error" ) );
    }
    aError.SetIsWarning( EFalse );
    aError.SetErrorCode( ESvgDRMFailure );
    aError.SetSystemErrorCode( errorCode );
}

// ==========================================================================
// Returns true if the first two bytes of the given file could be a gzip file.
// This function is modified from the function in EZGZipFile class.
// ==========================================================================
TBool CSvgDocumentImpl::IsGzipContent( RFile& aFileHandle )
{
    TUint8 ids[2];
    TPtr8 des( ids, 0, sizeof( TUint8 ) * 2 );

    if ( aFileHandle.Read( des ) != KErrNone )
        return EFalse;

    // reset to start of file
    TInt zero = 0;
    aFileHandle.Seek( ESeekStart, zero );
    return ( ids[0] == 31 && ids[1] == 139 );
//    return ( ids[0] == EZGZipFile::ID1 && ids[1] == EZGZipFile::ID2 );
}

// ==========================================================================
// Returns whether the byte-array header matches a GZIP signature
// ==========================================================================
TBool CSvgDocumentImpl::IsGzipContent( const TDesC8& aByteArray )
{
    return aByteArray.Length() >= 2 &&
           aByteArray[0] == 31 && aByteArray[1] == 139;
//           aByteArray[0] == EZGZipFile::ID1 && aByteArray[1] == EZGZipFile::ID2;
}

// ==========================================================================
// Returns whether the byte-array header matches a SVG binary signature
// ==========================================================================
TBool CSvgDocumentImpl::IsBinarySvg( const TDesC8& aByteArray )
{
	//const TUint32 KBinaryFile          = 66737868;
	//const TUint32 KBinaryFile2         = 66737869;

	if ((aByteArray.Length() >= 4 &&
           aByteArray[0] == 0xCC && aByteArray[1] == 0x56 &&
           aByteArray[2] == 0xFA && aByteArray[3] == 0x03) ||
        (aByteArray.Length() >= 4 &&
           aByteArray[0] == 0xCD && aByteArray[1] == 0x56 &&
           aByteArray[2] == 0xFA && aByteArray[3] == 0x03) ||
        (aByteArray.Length() >= 4 &&
           aByteArray[0] == 0xCE && aByteArray[1] == 0x56 &&
           aByteArray[2] == 0xFA && aByteArray[3] == 0x03) ||
        (aByteArray.Length() >= 4 &&
           aByteArray[0] == 0xCF && aByteArray[1] == 0x56 &&
           aByteArray[2] == 0xFA && aByteArray[3] == 0x03) )
	{
		return ETrue;
	}
    else
    {
    	return EFalse;
    }
}

// ==========================================================================
// Returns whether the file header matches a DRM signature
// ==========================================================================
TBool CSvgDocumentImpl::IsDRMContent( RFile& aFileHandle )
{
    TUint8 ids[2];
    TPtr8 des( ids, 0, sizeof( TUint8 ) * 2 );

    if ( aFileHandle.Read( des ) != KErrNone )
        return EFalse;

    // reset to start of file
    TInt zero = 0;
    aFileHandle.Seek( ESeekStart, zero );
    return ( ids[0] == 0x01 && ids[1] == 0x0D );
}

// ==========================================================================
// Returns whether the file header matches a DRM signature
// ==========================================================================
TBool CSvgDocumentImpl::IsDRMContent( const TDesC8& aByteArray )
{
    return ( aByteArray.Length() > 1 && aByteArray[0] == 0x01 && aByteArray[1] == 0x0D );
}

// ==========================================================================
// Generate the "c:/private/<sid>" directory name
// ==========================================================================
TInt CSvgDocumentImpl::GetProcessPrivatePath( RFs& aSession, TFileName& aPath )
{
    aSession.PrivatePath( aPath );
    aPath.Insert( 0, _L( "c:" ) );
    return KErrNone;
}

// ==========================================================================
// Create "c:/private/<sid>" directory if it does not already exist
// ==========================================================================
TInt CSvgDocumentImpl::CreateProcessPrivateDirectory( RFs& aSession, TFileName& aPath )
{
    GetProcessPrivatePath( aSession, aPath );
    return ( !BaflUtils::FolderExists( aSession, aPath ) ) ? aSession.MkDir( aPath ) : KErrNone;
}

// ==========================================================================
// Open a file session, error is indicated in aError
// ==========================================================================
void CSvgDocumentImpl::OpenSession( RFs& aSession, CSvgErrorImpl& aError )
{
    TInt error = aSession.Connect();
    if ( error != KErrNone )
    {
        // couldn't open temp file for writing
        PrepareError( aError, ESvgUnknown, error,
                      _L( "Connecting File Session Failed." ), _L( "" ) );
    }
}

// ==========================================================================
// Session must be opened successfully
// File name of temp file is store in aFilename
// ==========================================================================
TInt CSvgDocumentImpl::WriteToTempFile( RFs& aSession, const TDesC8& aByteArray,
                                        TFileName& aFilename, CSvgErrorImpl& aError )
{
    // Create temporary file for byte-array
    TFileName path;
    CreateProcessPrivateDirectory( aSession, path );
    RFile writeFile;
    TInt tempError = writeFile.Temp( aSession, path, aFilename, EFileWrite );
    if ( tempError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, tempError,
                      _L( "Failed open temp file to write data." ), aFilename );
        return tempError;
    }

    // Write byte-array to file
    TInt writeError = writeFile.Write( aByteArray );
    if ( writeError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, writeError,
                      _L( "Failed to write data to temp file." ), aFilename );
        writeFile.Close();
        aSession.Delete( aFilename );
        return writeError;
    }
    writeFile.Flush();
    writeFile.Close();

    return KErrNone;
}

// ==========================================================================
// Session must be opened successfully
// File name of temp file is store in aFilename
// ==========================================================================
TInt CSvgDocumentImpl::WriteToTempFile( RFs& aSession, RFile& aFileHandle,
                                        TFileName& aFilename, CSvgErrorImpl& aError )
{
    // Create temporary file for byte-array
    TFileName path;
    CreateProcessPrivateDirectory( aSession, path );
    RFile writeFile;
    TInt tempError = writeFile.Temp( aSession, path, aFilename, EFileWrite );
    if ( tempError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, tempError,
                      _L( "Failed open temp file to write data." ), path );
        return tempError;
    }

    // Write data to temp file
    TInt size;
    TInt sizeError = aFileHandle.Size( size );
    if ( sizeError != KErrNone )
    {
        PrepareError( aError, ESvgUnknown, sizeError,
                      _L( "Failed to get data length of file." ), aFilename );
        aSession.Delete( aFilename );
        return sizeError;
    }

    TBuf8<1024> buffer;
    TInt bytesCopied = 0;
    while ( bytesCopied < size )
    {
        buffer.Zero();
        TInt bytesToCopy = ( size - bytesCopied < 1024 ) ? size - bytesCopied : 1024;
        // read data
        TInt readError = aFileHandle.Read( buffer, bytesToCopy );
        if ( readError != KErrNone )
        {
            PrepareError( aError, ESvgUnknown, readError,
                          _L( "Failed to read data to copy to temp file: " ), aFilename );
            writeFile.Close();
            aSession.Delete( aFilename );
            return readError;
        }
        // write data
        TInt writeError = writeFile.Write( buffer, bytesToCopy );
        if ( writeError != KErrNone )
        {
            PrepareError( aError, ESvgUnknown, writeError,
                          _L( "Failed to write to temp file: " ), aFilename );
            writeFile.Close();
            aSession.Delete( aFilename );
            return writeError;
        }
        bytesCopied += bytesToCopy;
    }

    writeFile.Flush();
    writeFile.Close();

    return KErrNone;
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::PrepareError( CSvgErrorImpl& aError,
                                     TSvgErrorCode aSvgErrorCode,
                                     TInt aSystemErrorCode,
                                     const TDesC& aMsg1,
                                     const TDesC& aMsg2 )
{
    aError.SetDescription( aMsg1, aMsg2 );
    aError.SetErrorCode( aSvgErrorCode );
    aError.SetSystemErrorCode( aSystemErrorCode );
    aError.SetIsWarning( EFalse );
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::LoadingCompleted()
{
    if ( iEngine != NULL && iEngine->GraphicsContext() != NULL )
    {
        iEngine->GraphicsContext()->SetDoDithering( iHasGradientElement );
    }
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::SetThumbNailMode( TBool aThumbNailMode )
{
    iIsThumbNailMode = aThumbNailMode;
}

//==========================================================================
// CSvgDocumentImpl::SetDRMRights( TBool aEnable )
//==========================================================================
void CSvgDocumentImpl::SetDRMRights( TBool aEnable)
{
	iDrmRightsConsumptionEnabled = aEnable;
}
// ==========================================================================
// Need method description
// ==========================================================================
TBool CSvgDocumentImpl::IsThumbNailOnly()
{
    return iIsThumbNailMode && iIsDRMProtected;
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::SetWasPrepared( TBool aBool )
{
    iWasPrepared = aBool;

}

// ==========================================================================
// Need method description
// ==========================================================================
TBool CSvgDocumentImpl::WasPrepared()
{
    return iWasPrepared;
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::SetLoadingListeners( const RPointerArray<MSvgLoadingListener>* aList )
    {
    iLoadingListeners = aList;
    }

// ==========================================================================
// Need method description
// ==========================================================================
const RPointerArray<MSvgLoadingListener>* CSvgDocumentImpl::GetLoadingListeners()
    {
    return iLoadingListeners;
    }

/*---------------------------MouseListener---------------------------*/

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::AddInternalMouseListener( const MSvgMouseListener* aListener )
    {
    TInt index = iSvgMouseListeners.Find( aListener );
    if ( aListener != NULL && index == KErrNotFound )
        {
        iSvgMouseListeners.Append( aListener );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::RemoveInternalMouseListener( const MSvgMouseListener* aListener )
    {
    TInt index = iSvgMouseListeners.Find( aListener );
    if ( index != KErrNotFound )
        {
        iSvgMouseListeners.Remove( index );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgDocumentImpl::MouseInternalListenerCount()
    {
    return iSvgMouseListeners.Count();
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyInternalMousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {

    for ( TInt i = 0; i < iSvgMouseListeners.Count(); i++ )
        {
        iSvgMouseListeners[i]->MousePressed( aElements, aX, aY );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyInternalMouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {

    for ( TInt i = 0; i < iSvgMouseListeners.Count(); i++ )
        {
        iSvgMouseListeners[i]->MouseReleased( aElements, aX, aY );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyInternalMouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {

    for ( TInt i = 0; i < iSvgMouseListeners.Count(); i++ )
        {
        iSvgMouseListeners[i]->MouseEntered( aElements, aX, aY );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyInternalMouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {

    for ( TInt i = 0; i < iSvgMouseListeners.Count(); i++ )
        {
        iSvgMouseListeners[i]->MouseExited( aElements, aX, aY );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyInternalMouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {

    for ( TInt i = 0; i < iSvgMouseListeners.Count(); i++ )
        {
        iSvgMouseListeners[i]->MouseMoved( aElements, aX, aY );
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyDocumentStart()
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->DocumentStart();
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyDocumentEnd()
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->DocumentEnd();
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyElementStart( const TDesC& aTagName,
                                           MSvgAttributeList& aAttributeList,
                                           TBool aIsSvgChild )
    {
    // Do not send back attribute list if content was decrypted
    // from DRM protected content.
    if ( iLoadingListeners != NULL && !iIsDRMProtected )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            if ( aIsSvgChild || (*iLoadingListeners)[i]->ReportAllElements() )
                {
                (*iLoadingListeners)[i]->ElementStart( aTagName, aAttributeList );
                }
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyElementEnd( const TDesC& aTagName, TBool aIsSvgChild )
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            if ( aIsSvgChild || (*iLoadingListeners)[i]->ReportAllElements() )
                {
                (*iLoadingListeners)[i]->ElementEnd( aTagName );
                }
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyUnsupportedElement( const TDesC& aTagName,
                                                 MSvgAttributeList& aAttributeList )
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->UnsupportedElement( aTagName, aAttributeList );
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyExternalDataRequested( const TDesC& aUri )
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->ExternalDataRequested( aUri );
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyExternalDataReceived( const TDesC& aUri )
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->ExternalDataReceived( aUri );
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgDocumentImpl::NotifyExternalDataRequestFailed( const TDesC& aUri )
    {
    if ( iLoadingListeners != NULL )
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->ExternalDataRequestFailed( aUri );
            }
        }
    }

// ==========================================================================
// Need method description
// ==========================================================================
TUint32 CSvgDocumentImpl::IsIndefiniteAnimation()
    {
    if ( !iCheckedAnimationDuration )
        {
        AnimationDuration();
        }
    // special value for indefinite animation
    return ( iAnimationDuration == KIndefiniteAnimationDuration );
    }

// ==========================================================================
// Need method description
// ==========================================================================
TUint32 CSvgDocumentImpl::AnimationDuration()
    {
    if ( iCheckedAnimationDuration )
        {
        return IsIndefiniteAnimation() ? 0xFFFFFFFF : iAnimationDuration;
        }
    else if ( iRootElement == NULL )
        {
        return 0;
        }

    if ( iIsInteractive )
        {
        // If there is interactivity in the content, set the animation
        // duration to indefinite so that the timer continues to run.
        iAnimationDuration = KIndefiniteAnimationDuration;
        }
    else
        {
        iAnimationDuration = SubtreeDuration( iRootElement );
        }

    iCheckedAnimationDuration = ETrue;
    return IsIndefiniteAnimation() ? 0xFFFFFFFF : iAnimationDuration;
    }

// ==========================================================================
// Return the longest animation in subtree
// ==========================================================================
TUint32 CSvgDocumentImpl::SubtreeDuration( CSvgElementImpl* aRootElement )
    {
    TUint dur = 0;
		
    if ( aRootElement->IsAnimatedElement() )
        {
        dur = ( ( CSvgAnimationBase * ) aRootElement )->CompleteDuration();
        }

	else if ( aRootElement->ElemID() == KSvgDiscardElement )
		{
			dur = ((CSvgDiscardElementImpl*)aRootElement)->AbsoluteBeginTime();
		}
		
    // check child duration
    TUint childDur;
    CSvgElementImpl* child = (CSvgElementImpl*)aRootElement->FirstChild();
    while ( child != NULL )
        {
        childDur = SubtreeDuration( child );
        if ( childDur > dur )
            {
            dur = childDur;
            }
        child = (CSvgElementImpl*)child->NextSibling();
        }
    return dur;
    }

// ==========================================================================
// notified from outside (AppendChild/RemoveChild)
// ==========================================================================
void CSvgDocumentImpl::ElementAppendedOrRemoved()
    {
    // Clear flag to retrieve animation duration on AnimationDuration() call
    iCheckedAnimationDuration = EFalse;
    }

// ==========================================================================
// Return all elements of the given type
// or all of the elements if the type = -1
// ==========================================================================
void CSvgDocumentImpl::FindAllElements( CSvgElementImpl* aStartElement, TInt aElementId,
                                      RPointerArray<CSvgElementImpl>& aList,
                                      TSvgSearchFlags aFlags )
    {
    if ( aStartElement == NULL )
        return;

    CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
    while ( child != NULL )
        {
        // add to list if child is found
        if ( child->ElemID() == aElementId || aElementId == -1 )
            aList.Append( child );

        // find in grandchildren
        if ( aFlags & ESvgSearchExcludeUseSubtree )
            {
            // If Exclude Use Subtree, ignore the subtree under use element
            if ( child->ElemID() != KSvgUseElement )
                {
                FindAllElements( child, aElementId, aList, aFlags );
                }
            }
        else
            {
            FindAllElements( child, aElementId, aList, aFlags );
            }
        child = (CSvgElementImpl*)child->NextSibling();
        }
    }

// ==========================================================================
// ImageLoadingObserver interface method
// ==========================================================================
void CSvgDocumentImpl::ImageLoadingCompleted( TInt aError )
    {
    iImageElementCnt--;
    
    if ( iLoadingListeners != NULL && iImageElementCnt == 0)
        {
        TInt loadingListenersCnt = iLoadingListeners->Count();
        for ( TInt i = 0; i < loadingListenersCnt; i++ )
            {
            (*iLoadingListeners)[i]->ImagesLoaded(aError);
            }
        }
    

    }

// ==========================================================================
// Loops through the tree and prints out all of the elements
// (almost regenerating the DOM tree)
//
// ==========================================================================
void CSvgDocumentImpl::PrintAllElements( CSvgElementImpl* aStartElement )
{
	if ( aStartElement == NULL )
        return;

	aStartElement->Print( EFalse );
	//===> comment the styles out to generate true SVG content
	//	aStartElement->PrintStyleProperties();

	CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
	PrintAllElements( child );

	child = (CSvgElementImpl*)aStartElement->NextSibling();
	PrintAllElements( child );
}

TBool CSvgDocumentImpl::HasError()
    {
    return iError && iError->HasError();
    }

MSvgError* CSvgDocumentImpl::GetError()
    {
    return iError;
    }

void CSvgDocumentImpl::SetIsWarning( TBool aIsWarning )
{
	if (iError)
	{
		iError->SetIsWarning( aIsWarning );
	}
}
void CSvgDocumentImpl::SetError( TInt aErrorType, const TDesC& aMsg1, const TDesC& aMsg2 )
    {
    if ( !iError )
        {
        TRAPD( error, iError = CSvgErrorImpl::NewL() );
        if ( error != KErrNone )
            {
            return;
            }
        PrepareError( *iError, ESvgUnknown, aErrorType, aMsg1, aMsg2 );
        iError->SetIsWarning( ETrue );
        }
    }


// ---------------------------------------------------------------------------
// Accessor funciton for SyncBehaviorDefault value
// ---------------------------------------------------------------------------
TSvgSyncBehaviour CSvgDocumentImpl::SyncBehaviorDefault()
    {
    return iSyncBehaviorDefault;
    }

// ---------------------------------------------------------------------------
// Accessor funciton for SyncBehaviorDefault value
// ---------------------------------------------------------------------------
TUint32 CSvgDocumentImpl::SyncToleranceDefault()
    {
    return iSyncToleranceDefault;
    }

// ---------------------------------------------------------------------------
// CSvgDocumentImpl::SetCurrentTime
// Set Function for the document's current time
// ---------------------------------------------------------------------------
void CSvgDocumentImpl::SetCurrentTime( TInt32 aCurTime )
    {
    if ( iTimeContainer )
        {
        iTimeContainer->SetCurrentTime( aCurTime );
        }
    }

// ---------------------------------------------------------------------------
// CSvgDocumentImpl::CurrentTime
// Accessor for the Document's current time
// ---------------------------------------------------------------------------
TInt32 CSvgDocumentImpl::CurrentTime()
    {
    if ( iTimeContainer )
        {
        return iTimeContainer->CurrentTime();
        }
    return KErrGeneral;
    }

// ---------------------------------------------------------------------------
// CSvgDocumentImpl::SetSyncBehavior
// Set the Synchronised behaviour for the time container
// ---------------------------------------------------------------------------
void CSvgDocumentImpl::SetTCSyncBehavior( const TSvgSyncBehaviour aValue )
    {
    if ( iTimeContainer )
        {
        iTimeContainer->SetSyncBehavior( aValue );
        }
    }

// ---------------------------------------------------------------------------
// CSvgDocumentImpl::SetSyncTolerance
// Set the Synchronised Tolerance for the time container
// ---------------------------------------------------------------------------
void CSvgDocumentImpl::SetTCSyncTolerance( const TUint32 aValue )
    {
    if ( iTimeContainer )
        {    
        iTimeContainer->SetSyncTolerance( aValue );
        }
    }

// ---------------------------------------------------------------------------
// CSvgDocumentImpl::SetSyncMaster
// Set the time container as Synchronised Master
// ---------------------------------------------------------------------------
void CSvgDocumentImpl::SetTCSyncMaster( const TBool aValue )
    {
    if ( iTimeContainer )
        {    
        iTimeContainer->SetSyncMaster( aValue );
        }
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::TimeContainer
// Accessor for time container
// -----------------------------------------------------------------------------
//
CSvgTimeContainer* CSvgDocumentImpl::TimeContainer()    
    {
    return iTimeContainer;
    }

void CSvgDocumentImpl::DocumentTransform(TGfxAffineTransform& aTr)
    {
        iTransformMatrix = aTr;
    }

TGfxAffineTransform CSvgDocumentImpl::GetDocumentTransform()
    {
        return iTransformMatrix;
    }
    
    
CSvgBitmapFontProvider * CSvgDocumentImpl::GetBitmapFontProvider()
{
	return iSvgBitmapFontProvider ;
}

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::SvgHasAnimationElement
// Checks if Document has Animation Element
// -----------------------------------------------------------------------------

TBool CSvgDocumentImpl::SvgHasAnimationElement()
    {
        RPointerArray<CSvgElementImpl> lAnimationEleList;
        FindAllElements((CSvgElementImpl* )RootElement(),
                   KSvgMediaAnimationElement, lAnimationEleList );
        TInt lAnimationEleCnt = lAnimationEleList.Count();    
        lAnimationEleList.Close();
        if ( lAnimationEleCnt > 0 )
            {
            return ETrue;
            }
       
        return EFalse;     
       
    }

// -----------------------------------------------------------------------------
// CSvgDocumentImpl::IsDocumentContentInteractive
// Checks if Document content is InterActive 
// -----------------------------------------------------------------------------

TBool CSvgDocumentImpl::IsDocumentContentInteractive()
    {
        RPointerArray<CSvgElementImpl> elements;
        FindAllElements( (CSvgElementImpl* )RootElement(),
                                     KSvgAElement, elements );
        if ( elements.Count() != 0 )
        {
            elements.Close();
            return ETrue;
        }
        elements.Close();
       
        if ( iIsInteractive )
            return ETrue;
        
        // Check if animation element is there 
        RPointerArray<CSvgElementImpl> lAnimationEleList;
        FindAllElements((CSvgElementImpl* )RootElement(),
                   KSvgMediaAnimationElement, lAnimationEleList );
                   
        // If no animation element is present then return EFalse           
        TInt lAnimationEleCnt = lAnimationEleList.Count();  
        if ( lAnimationEleCnt == 0 ) 
        {
           lAnimationEleList.Close();  
           return EFalse; 
        }
        
        // Check all animation element and if any child svg has ..
        // interactive content then return ETrue   
        for ( TInt lCurAnimationEle = 0; 
                lCurAnimationEle < lAnimationEleCnt; lCurAnimationEle++ )
        {
           CSvgMediaAnimationElementImpl* lAnimationElement = 
            (CSvgMediaAnimationElementImpl* )lAnimationEleList[ lCurAnimationEle ];
           CSvgDocumentImpl* lChildDoc = lAnimationElement->GetChildDocument();
           // if child svg has interavitve content then return
           if(lChildDoc && lChildDoc->iIsInteractive)
           {
           		lAnimationEleList.Close();                
           		return ETrue;
           }
        }  
        
        lAnimationEleList.Close();                
        return EFalse;   

    }


void CSvgDocumentImpl::SetImageElementsCount(TInt aCnt)
    {
    iImageElementCnt = aCnt;
    }

TInt CSvgDocumentImpl::GetImageElementsCount()
    {
    return iImageElementCnt;
    }
// End of file
 
