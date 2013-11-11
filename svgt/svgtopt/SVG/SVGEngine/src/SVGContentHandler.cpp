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
* Description:  SVG Engine source file
 *
*/


#include "SVGContentHandler.h"
#include "SVGSchemaData.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"

#include "SVGSvgElementImpl.h"
#include "SVGStringTokenizer.h"

#include "SVGUseElementImpl.h"
#include "SVGAnimationElementImpl.h"
#include <xml/rxmlreader.h>

#include "SVGPaintCssValueImpl.h"
#include "SVGPathElementImpl.h"
#include "GfxGeneralPath.h"

#include "SVGUriReferenceImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGAnimationBase.h"

#include "SVGTextAreaElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGScriptElementImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGMediaElementBase.h"

#include "SVGFontFaceElementImpl.h"
#include <languages.hrh>

//#ifdef RD_SVGT_MEDIAANIMATION_SUPPORT
#include "SVGMediaAnimationElementImpl.h"
//#endif
_LIT(WIDTH, "width");
_LIT(HEIGHT, "height");
_LIT(BEGIN, "begin");
_LIT(END, "end");
_LIT(MOUSEDOWN, "mousedown");
_LIT(MOUSEUP, "mouseup");
_LIT(MOUSEOVER, "mouseover");
_LIT(MOUSEMOVE, "mousemove");
_LIT(MOUSEOUT, "mouseout");
_LIT(FOCUSIN, "focusin");
_LIT(FOCUSOUT, "focusout");
_LIT(ACTIVATE, "activate");
_LIT(CLICK, "click");

_LIT(FILL, "fill");
_LIT(STROKE, "stroke");
_LIT(COLOR, "color");
_LIT(STOP_COLOR, "stop-color");
_LIT(ACCESS,"accessKey");
_LIT(OFFSET, "offset");
_LIT(ZEROVALUE, "0");

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CSvgContentHandler::CSvgContentHandler() : iRootElement( NULL ),
                                           iIsColorAnim( EFalse ),
                                           iRemoveFalseSwitchElements( ETrue )
    {
    iCancelRequested = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgContentHandler::~CSvgContentHandler()
    {
    if(iUseElementArray)
        {
        iUseElementArray->Reset();
        iUseElementArray->Close();
        delete iUseElementArray;
        iUseElementArray = NULL;
        }
    if(iAnimRefElementArray)
        {
        iAnimRefElementArray->Reset();
        iAnimRefElementArray->Close();
        delete iAnimRefElementArray;
        iAnimRefElementArray = NULL;
        }
    if(iSwitchElementArray)
        {
        iSwitchElementArray->Reset();
        iSwitchElementArray->Close();
        delete iSwitchElementArray;
        iSwitchElementArray = NULL;
        }
    if (iReqFetAttSysArray)
        {
        iReqFetAttSysArray->Reset();
        iReqFetAttSysArray->Close();
        delete iReqFetAttSysArray;
        iReqFetAttSysArray = NULL;
        }
    if(iAnimationElementArray)
        {
        iAnimationElementArray->Reset();
        iAnimationElementArray->Close();
        delete iAnimationElementArray;
        }
    if(iCData)
        {
        delete iCData;
        iCData = NULL;
        }
    if ( iSystemLanguage )
        {
        delete iSystemLanguage;
        iSystemLanguage = NULL;
        }

    iImageElements.Close();
    }
// ---------------------------------------------------------------------------
// CSvgContentHandler* CSvgContentHandler::NewL()
// ---------------------------------------------------------------------------
CSvgContentHandler* CSvgContentHandler::NewL( TBool aRemoveFalseSwitchElements )
    {
    CSvgContentHandler* self    = new( ELeave ) CSvgContentHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aRemoveFalseSwitchElements );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CSvgContentHandler* CSvgContentHandler::NewLC()
// ---------------------------------------------------------------------------
CSvgContentHandler* CSvgContentHandler::NewLC()
    {
    CSvgContentHandler* self    = new( ELeave ) CSvgContentHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// --------------------------------------------------------------------------
// void CSvgContentHandler::ConstructL()
// ---------------------------------------------------------------------------
void CSvgContentHandler::ConstructL( TBool aRemoveFalseSwitchElements )
    {
    /*Arrays added to support Forward referencing in Animation and Use elements*/
    iUseElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);

    iAnimRefElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);

    // added to hold switch element handles so we can go back and delete false children of the switch
    iSwitchElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);

    // Arrays added to reset event listeners for animation element
    iAnimationElementArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);

    //added to hold elements with required features, attributes, or sys language
    //so we can go back and delete the ones that dont pass at the start.
    iReqFetAttSysArray = new (ELeave)RPointerArray<CSvgElementImpl> (1);
    // Create a 1KB buffer for CData -- this will expand as needed
    iCData = HBufC::NewL( 1024 );

    iSystemLanguage = HBufC::NewL( 5 );

		iRemoveFalseSwitchElements = aRemoveFalseSwitchElements;
    }





// --------------------------------------------------------------------------
// CSvgElementImpl* CSvgContentHandler::ParseByteData( CSvgDocumentImpl* aDocument,
//                                                    const TDesC8& aByteData,
//                                                    CSvgErrorImpl& aError )
// ---------------------------------------------------------------------------
CSvgElementImpl* CSvgContentHandler::ParseByteData( CSvgDocumentImpl* aDocument,
                                                    const TDesC8& aByteData,
                                                    CSvgErrorImpl& aError )
    {
    iDocument = aDocument;

    iSvgError = &aError;
    aError.SetErrorCode( ESvgNoError );



    // Create XML reader
    RXMLReader xmlReader;
    TInt error = KErrNone;
    TRAP( error, xmlReader.CreateL() );
    if ( error != KErrNone )
        {
        _LIT(OUTMEMORY, "Out of memory");
        aError.SetErrorCode( ESvgNoMemory );
        aError.SetDescription( OUTMEMORY );
        aError.SetIsWarning( EFalse );
        return iRootElement;
        }

    // Parse content
    xmlReader.SetContentHandler( this );
    TRAPD( parseError, xmlReader.ParseL( aByteData ) );
    xmlReader.Destroy();
    if ( !iCancelRequested && parseError != KErrNone )
        {
        if ( iSvgError->HasError() )
            return iRootElement;

        aError.SetErrorCode( ESvgUnknown );
        if ( parseError == -4 )
            {
            _LIT(OUTMEMORY, "Out of memory");
            aError.SetDescription( OUTMEMORY );
            }
        else
            {
            _LIT(STRING, "Parsing SVG document failed.");
            aError.SetDescription( STRING );
            }
        aError.SetIsWarning( EFalse );
        aError.SetSystemErrorCode( parseError );
        return iRootElement;
        }
    if ( iRootElement == NULL )
        {
        _LIT(STRING, "Invalid SVG Document.");
        aError.SetErrorCode( ESvgDocumentNotValid );
        aError.SetDescription( STRING );
        aError.SetIsWarning( EFalse );
        return iRootElement;
        }
    else if ( !iCancelRequested )
        {
        SetForwardReferences(aError);
        PostParseProcessing();
        }

    return iRootElement;
    }

// --------------------------------------------------------------------------
// CSvgElementImpl* CSvgContentHandler::Parse16BitData( CSvgDocumentImpl* aDocument,
//                                                    const TDesC8& aByteData,
//                                                    CSvgErrorImpl& aError )
// ---------------------------------------------------------------------------
CSvgElementImpl* CSvgContentHandler::Parse16BitData( CSvgDocumentImpl* aDocument,
                                                    const TDesC16& aSvgString,
                                                    CSvgErrorImpl& aError )
    {
    iDocument = aDocument;

    iSvgError = &aError;
    aError.SetErrorCode( ESvgNoError );

    // Create XML reader
    RXMLReader xmlReader;
    TInt error = KErrNone;
    TRAP( error, xmlReader.CreateL() );
    if ( error != KErrNone )
        {
        _LIT(OUTMEMORY, "Out of memory");
        aError.SetErrorCode( ESvgNoMemory );
        aError.SetDescription( OUTMEMORY );
        aError.SetIsWarning( EFalse );
        return iRootElement;
        }

    // Parse content
    xmlReader.SetContentHandler( this );

    //-------------------------------------------------------------------
    //THIS IS TEMPORARY
    //convert to 8 bit here temporary until I get 16 bit parsing method...
    //TRAPD( parseError, xmlReader.ParseXML( aSvgString ) );

    //convert 16 to 8 bit characters
    HBufC8* lString8 = NULL;
    lString8 = HBufC8::New( aSvgString.Length() );


	if(NULL==lString8)
	{
		_LIT(OUTMEMORY, "Out of memory");
		aError.SetErrorCode( ESvgNoMemory );
        aError.SetDescription( OUTMEMORY );
        aError.SetIsWarning( EFalse );
		return iRootElement;
	}
    
    lString8->Des().Copy(aSvgString);
    TRAPD( parseError, xmlReader.ParseL( *lString8 ) );
    delete lString8 ; 
    //-------------------------------------------------------------------

    xmlReader.Destroy();
    if ( !iCancelRequested && parseError != KErrNone )
        {
        if ( iSvgError->HasError() )
            return iRootElement;

        aError.SetErrorCode( ESvgUnknown );
        if ( parseError == -4 )
            {
            _LIT(OUTMEMORY, "Out of memory");
            aError.SetDescription( OUTMEMORY );
            }
        else
            {
            _LIT(STRING, "Parsing SVG document failed.");
            aError.SetDescription( STRING );
            }
        aError.SetIsWarning( EFalse );
        aError.SetSystemErrorCode( parseError );
        return iRootElement;
        }
    if ( iRootElement == NULL )
        {
        _LIT(STRING, "Invalid SVG Document.");
        aError.SetErrorCode( ESvgDocumentNotValid );
        aError.SetDescription( STRING );
        aError.SetIsWarning( EFalse );
        return iRootElement;
        }
    else if ( !iCancelRequested )
        {
        //POST PARSING PROCESSING
        SetForwardReferences(aError);
        RemoveFalseElementsOrMakeInvisible();
        RemoveFalseSwitchCasesOrMakeInvisible();
        if ( !iSvgError->HasError())
        {
        LoadImages();
        }
        }

    return iRootElement;
    }

// --------------------------------------------------------------------------
// CSvgElementImpl* CSvgContentHandler::ParseFile( CSvgDocumentImpl* aDocument,
// ---------------------------------------------------------------------------
CSvgElementImpl* CSvgContentHandler::ParseFile( CSvgDocumentImpl* aDocument,
                                                    const TDesC& aFileName,
                                                    CSvgErrorImpl& aError )
    {
    iDocument = aDocument;

    iSvgError = &aError;
    aError.SetErrorCode( ESvgNoError );

    RFs session;
    TInt serror = session.Connect ();
    if( serror != KErrNone)
        {
        _LIT(STRING, "Cannot create a session to read the file.");
        aError.SetErrorCode ( ESvgUnknown );
        aError.SetDescription( STRING );
        aError.SetIsWarning( EFalse );
        return NULL;
        }

    // Create XML reader
    RXMLReader xmlReader;
    TInt error = KErrNone;
    TRAP( error, xmlReader.CreateL() );
    if ( !iCancelRequested && error != KErrNone )
        {
        _LIT(OUTMEMORY, "Out of memory");
        aError.SetErrorCode( ESvgNoMemory );
        aError.SetDescription( OUTMEMORY );
        aError.SetIsWarning( EFalse );
        return iRootElement;
        }

    // Parse content
    xmlReader.SetContentHandler( this );
    TRAP( error, xmlReader.ParseL( session, aFileName) );
    xmlReader.Destroy();
    if ( error != KErrNone || iRootElement == NULL )
        {
        _LIT(STRING, "Invalid SVG Document.");
        aError.SetErrorCode( ESvgDocumentNotValid );
        aError.SetDescription( STRING );
        aError.SetIsWarning( EFalse );
        }
    else if ( !iCancelRequested )
        {
        SetForwardReferences(aError);
        PostParseProcessing();
        }

    session.Close();
    return iRootElement;
    }

void CSvgContentHandler::PostParseProcessing()
{

    // This situation occurs when parsing is done in a separate thread then the draw thread
    // Wait for drawing to complete or invalid element pointer may occur during drawing
    if ( iDocument->iIsRendering )
        {
        TInt waitCount = 0;
        // total of 1 seconds
        while ( waitCount++ < 200000 && iDocument->iIsRendering )
            {
            User::After( 50 ); // 50 microseconds
            }

        // Cannot wait any longer, prevent being stuck here
        if ( iDocument->iIsRendering )
            {
            return;
            }
        }

    iDocument->iIsPruningTree = ETrue;

    RemoveFalseElementsOrMakeInvisible();
    RemoveFalseSwitchCasesOrMakeInvisible();

    iDocument->iIsPruningTree = EFalse;
    iDocument->SetImageElementsCount(iImageElements.Count());
    if ( (!iSvgError->HasError())  || iSvgError->IsWarning() )
        {
    LoadImages();
        }
}

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::RemoveInternalReferences( CSvgElementImpl* )
// ---------------------------------------------------------------------------
void CSvgContentHandler::RemoveInternalReferences( CSvgElementImpl* aElement )
    {
    // Remove the references of aElement in content handler's lists

    // Use Elements 
    TInt lIndex = 0;
    if ( aElement->ElemID() == KSvgUseElement )
        {
        lIndex = iUseElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iUseElementArray->Remove( lIndex );
            }
        }
    else if ( aElement->ElemID() == KSvgImageElement )
        {
        // Image elements
        lIndex = iImageElements.Find( (CSvgImageElementImpl* )aElement );
        if ( lIndex != KErrNotFound )
            {
            iImageElements.Remove( lIndex );
            }
        }
    else if ( aElement->ElemID() == KSvgSwitchElement )
        {
        // Switch Elements    
        lIndex = iSwitchElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iSwitchElementArray->Remove( lIndex );
            }
        }
    else if ( aElement->IsAnimatedElement() )
        {
        // Animation elements include animate* elements, set, 
        // "animation",  audio elements etc.
        lIndex = iAnimationElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iAnimationElementArray->Remove( lIndex );
            }        
        lIndex = iAnimRefElementArray->Find( aElement );
        if ( lIndex != KErrNotFound )
            {
            iAnimRefElementArray->Remove( lIndex );
            }
        }
    // Elements with test attributes - requiredFeatures, 
    // requiredExtensions, systemLanguage
    lIndex = iReqFetAttSysArray->Find( aElement );
    if ( lIndex != KErrNotFound )
        {
        iReqFetAttSysArray->Remove( lIndex );
        }

    // Remove Internal references of subtree elements as well
    CSvgElementImpl* lChild = ( CSvgElementImpl* )aElement->FirstChild();
    while ( lChild != NULL )
        {
        RemoveInternalReferences( lChild );
        lChild = ( CSvgElementImpl* )lChild->NextSibling();
        }

    }

/************* Start of MXMLContentHandler methods ************/

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::StartDocument()
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::StartDocument()
    {
    if ( iCancelRequested )
        {
        return EFalse;
        }
    iDocument->NotifyDocumentStart();
    iBeginEndAtr = EFalse;
    return ETrue;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::EndDocument()
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::EndDocument()
    {
    if ( iCancelRequested )
        {
        return EFalse;
        }
    iDocument->NotifyDocumentEnd();
    return ETrue;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::StartElement( TDesC& aURI,
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::StartElement( TDesC& aURI,
                                       TDesC& aLocalName,
                                       TDesC& aName,
                                       MXMLAttributes *aAttributeList )
    {
    if ( iCancelRequested )
        {
        return EFalse;
        }

    // Clear cdata buffer
    iCData->Des().Zero();

    TInt result = EFalse;
    TRAPD( error, result = StartElementL( aURI, aLocalName, aName, aAttributeList ) );
    if ( error != KErrNone )
    {
        _LIT(STRING, "Parsing SVG Content Failed -- Element: ");
        iSvgError->SetDescription( STRING, aName );
        iSvgError->SetErrorCode( ESvgUnknown );
        iSvgError->SetSystemErrorCode( error );
        iSvgError->SetIsWarning( EFalse );
        return EFalse;
    }
    return result;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::StartElementL( TDesC& /* aURI */,
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::StartElementL( TDesC& /* aURI */,
                                          TDesC& /* aLocalName */,
                                          TDesC& aName,
                                          MXMLAttributes *aAttributeList )
    {
    if ( iRootElement == NULL && aName != _L( "svg" ) )
    {
        iSvgError->SetDescription( _L( "Invalid SVG document: Missing Root <svg> Element." ) );
        iSvgError->SetErrorCode( ESvgUnknown );
        iSvgError->SetSystemErrorCode( KErrNone );
        iSvgError->SetIsWarning( EFalse );
        return EFalse;
    }
    if ( iRootElement != NULL && aName == _L( "svg" ) )
    {
        iSvgError->SetDescription( _L( "Invalid SVG document: Nested <svg> Element." ) );
        iSvgError->SetErrorCode( ESvgUnknown );
        iSvgError->SetSystemErrorCode( KErrNone );
        iSvgError->SetIsWarning( EFalse );
        return EFalse;
    }

    if ( iIgnoreDepth > 0 )
        {
        iIgnoreDepth++;
        return ETrue;
        }
    MXmlElement* newElement;

    newElement  = iDocument->CreateElementL( aName );

    if ( newElement != NULL)
        {
        iIsSvgElement = ETrue;
        }
    else
        {
        iDocument->NotifyUnsupportedElement( aName, *aAttributeList );
        iIsSvgElement = EFalse;
        iIgnoreDepth = 1;
        return ETrue;
        }

    iCurrentElement = ( CSvgElementImpl * ) newElement;

    // Set flag to indicate element is incomplete
    if ( iDocument->GetLoadingListeners() &&
         iDocument->GetLoadingListeners()->Count() > 0 )
         {
         iCurrentElement->SetAllAttributesAdded( EFalse );
         }

    _LIT( KElmSvg, "svg" );
    if ( aName == KElmSvg && !iRootElement )
        {
        iDocument->AppendChildL( newElement );
        iRootElement = iCurrentElement;
        }
    // for all other elements
    else
        {
        iCurrentParentElement->AppendChildL( newElement );
        }


    // create separate lists for names and values
    TInt lCount = aAttributeList->GetLength();
    for ( TInt i =0 ; i< lCount ;i++)

        {

        ProcessNConvertAttrL( aName,
                              aAttributeList->GetName( i ),
                              aAttributeList->GetValue( i ) );
        }

    TInt elementID = iCurrentElement->ElemID();

    //keep lists so invalid ones can be removed at the end of parsing
    //all elements that have required features, extensions, sys language
    if (iCurrentElement->HasAnyTests())
        {
        iReqFetAttSysArray->AppendL(iCurrentElement);
        }
    else if (elementID == KSvgSwitchElement)
        {
        iSwitchElementArray->AppendL(iCurrentElement);
        }

    if ( iBeginEndAtr )
        {
        ((CSvgDocumentImpl*)iDocument)->iIsInteractive = ETrue;
        }

    if(iIsColorAnim)
        {
        ProcessColorAttributeL(_L("from"),iFromVal);
        ProcessColorAttributeL(_L("to"),iToVal);
        ProcessColorAttributeL(_L("by"),iByVal);
        }
    else
        {
        if(iFromVal.Length())
            {
            iCurrentElement->SetAttributeL(_L("from"),iFromVal );
            }
        if(iToVal.Length())
            {
            iCurrentElement->SetAttributeL(_L("to"),iToVal );
            }
        if(iByVal.Length())
            {
            iCurrentElement->SetAttributeL(_L("by"),iByVal );
            }
        }
    if(iValues.Length())
        {
        iCurrentElement->SetAttributeL(_L("values"),iValues );
        }
    if(iTimes.Length())
        {
    	iCurrentElement->SetAttributeL(_L("keyTimes"), iTimes);
        }
    if(iSplines.Length())
        {
    	iCurrentElement->SetAttributeL(_L("keySplines"), iSplines);
        }
    if (elementID == KSvgUseElement)
        {
        ((CSvgUseElementImpl *)iCurrentElement)->SetReferenceElementL();
        }
    // font-face-src is currently not implemented. Hence such a font is removed
    // from the font hash-map in order to avoid creating iSvgFont pointer
    // while drawing the text. 
    if (elementID == KSvgFontfacesrcElement )
    	{
    	//Get the font-face element which is the parent of font-face-src	
		CSvgFontFaceElementImpl* currentFontFaceElem = 
					(CSvgFontFaceElementImpl*)(iCurrentElement)->ParentNode();
	
		const TDesC* lFontFamily=currentFontFaceElem->GetFontFamily();
		
		currentFontFaceElem->RemoveFontFamily(*lFontFamily);
	 
    	}
// comment out for future AnimationElement
//    else if (elementID == KSvgAnimationElement)
//        {
//        ((CSvgAnimationElementImpl *)iCurrentElement)->SetReferenceElementL();
//        if(((CSvgAnimationElementImpl *)iCurrentElement)->RecursionVariable())
//            {
//            iSvgError->SetErrorCode( ESvgDocumentNotValid );
//            iSvgError->SetIsWarning( EFalse );
//            iSvgError->SetDescription( _L( "Invalid Document \n" ) );
//            iSvgError->AppendDescription( _L("Animation element in loop") );
//            return EFalse;
//            }
//        User::LeaveIfError(iAnimationElementArray->Append(iCurrentElement));
//        }

    // For DOM reuse
    if ( iCurrentElement->IsAnimatedElement() )
            {
            ((CSvgAnimationBase*)iCurrentElement)->SetOriginalValues_DOMReuse() ;
            iAnimationElementArray->AppendL(iCurrentElement);
            }


    if ( elementID >= KSvgLinearGradientElement &&
        elementID <= KSvgStopElement )
            {
            iCurrentElement->SetAttributeIntL(KCSS_ATTR_DISPLAY,0);
            }

    // special case: <animationMotion> may have <mpath> child instead
    // of "path" attribute.

    if ( elementID == KSvgMpathElement &&
         iCurrentParentElement->ElemID() == KSvgAnimateMotionElement &&
         iSvgError->ErrorCode() == ESvgMissingRequiredAttribute &&
         iSvgError->IsWarning()
           )
        {
            iSvgError->SetErrorCode( ESvgNoError );
            // add to event receiver list will have only one copy
            ((CSvgDocumentImpl*)iDocument)->AddToEventReceiverListL( iCurrentParentElement, KSvgEventMaskTimer );
        }


    if ( iCurrentElement->iReqAttrFlag != 0)
        {
        iSvgError->SetErrorCode( ESvgMissingRequiredAttribute );
        iSvgError->SetIsWarning( ETrue );
        iSvgError->SetDescription( _L( "Missing required attribute \"" ) );
        switch(iCurrentElement->iReqAttrFlag)
        {
        case KAtrSVGRec:
            iSvgError->AppendDescription( WIDTH );
            iSvgError->AppendDescription( _L( "\" and \"" ) );
            iSvgError->AppendDescription(_L("height"));
            break;
        case KAtrSVGElp:
            iSvgError->AppendDescription( _L("Rx") );
            iSvgError->AppendDescription( _L( "\" and \"" ) );
            iSvgError->AppendDescription(_L("Ry"));
            break;
        case KAtrSVGTrf:
            iSvgError->AppendDescription( _L("attributeName") );
            iSvgError->AppendDescription( _L( "\" and \"" ) );
            iSvgError->AppendDescription(_L("type"));
            break;
        case KSVG_ANIMATE_ELEMFLAG:
            iSvgError->AppendDescription( _L("attributeName") );
            break;
        case KSVG_CIRCLE_ELEMFLAG:
            iSvgError->AppendDescription( _L("r") );
            break;
        case KSVG_HKERN_ELEMFLAG:
            iSvgError->AppendDescription( _L("k") );
            break;
        case KSVG_PATH_ELEMFLAG:
            iSvgError->AppendDescription( _L("d") );
            break;
        case KSVG_POLYLINE_ELEMFLAG:
            iSvgError->AppendDescription( _L("points") );
            break;
        case KAtrType:
            iSvgError->AppendDescription( _L("type") );
            break;
        case KAtrRy:
            iSvgError->AppendDescription( _L("Ry") );
            break;
        case KAtrRx:
            iSvgError->AppendDescription( _L("Rx") );
            break;
        case KAtrWidth:
            iSvgError->AppendDescription( _L("width") );
            break;
        case KAtrHeight:
            iSvgError->AppendDescription( _L("height") );
            break;
        case KAtrXlinkhref:
            iSvgError->AppendDescription (_L("Xlink:href"));
            break;
        case KAtrSVGAmo:
            iSvgError->AppendDescription( _L("path") );
            ((CSvgDocumentImpl*)iDocument)->RemoveFromEventReceiverList( iCurrentElement );
            break;
        case KAtrToBy:
            iSvgError->AppendDescription( _L("to/by") );
            ((CSvgDocumentImpl*)iDocument)->RemoveFromEventReceiverList( iCurrentElement );
            break;
        }
        iSvgError->AppendDescription( _L( "\" for <" ) );
        // access schema data to get the name of the attribute which is missing
        // currently the error msg doesnt not report the name of the attribute
        iSvgError->AppendDescription( aName );
        iSvgError->AppendDescription( _L( ">." ) );
        
        TInt32 displayValue = KDisplayEnumNone;
        TInt   lRetValue = ((CSvgElementImpl*)iCurrentElement)->GetAttributeIntL( KCSS_ATTR_DISPLAY, displayValue );
        // turn off element if the display attribute is not declared OR if declared its value is "!none"
        if( (lRetValue == KErrNoAttribute)||(displayValue != KDisplayEnumNone ) )
        {
           ((CSvgElementImpl*)iCurrentElement)->SetPropertyL(KCSS_ATTR_DISPLAY,_L("none"));
           
           // the following boolean is added to show that the element is explicitly
           // "turned off" due to the absence of the required attribute for the element,
           // this element can again be "turned on" when the required attribute is present.
           ((CSvgElementImpl*)iCurrentElement)->SetTurnOff( ETrue );	
        }
       
    }

    if ( elementID == KSvgImageElement )
        {
        iImageElements.Append( (CSvgImageElementImpl*)iCurrentElement );
        }
    else if ( elementID == KSvgGElement && !((CSvgDocumentImpl*)iDocument)->iHasGroupOpacity)
    {
       if ( ((CSvgDocumentImpl*)iDocument)->isGroupOpacity( iCurrentElement ))
       {
            ((CSvgDocumentImpl*)iDocument)->iHasGroupOpacity = ETrue;
       }
    }

    // Check for invalid child element of leave elements
    if ( KSvgRectElement == elementID && iCurrentElement->ParentNode() &&
         KSvgRectElement == ((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID() )
        {
        _LIT( KMsg, "Invalid child of <rect> element: " );
        iDocument->SetError( KErrNotFound, KMsg, aName );
        }

    // Notify Loading Listeners of element-start
    // Send info if element is <svg> or immediate child of <svg>
    TBool isRootOrChild = (elementID == KSvgSvgElement);

    if (iCurrentElement->ParentNode())
    {
        isRootOrChild = (iCurrentElement->ElemID() == KSvgSvgElement) || (((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID() == KSvgSvgElement);
    }

    // Update CTM for new element (if there is a LoadingListener)
    if ( iDocument->GetLoadingListeners() &&
         iDocument->GetLoadingListeners()->Count() > 0 )
         {
         iCurrentElement->UpdateCTM();
         iCurrentElement->SetAllAttributesAdded( ETrue );
         }

    iDocument->NotifyElementStart( aName, *aAttributeList, isRootOrChild );

    // Current element becomes new parent for higher depths
    iCurrentParentElement = iCurrentElement;
    return ETrue;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::EndElement( TDesC& /* aURI */, TDesC& /* aLocalName */, TDesC& aTagName )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::EndElement( TDesC& /* aURI */, TDesC& /* aLocalName */, TDesC& aTagName )
    {
    if ( iCancelRequested )
        {
        return EFalse;
        }

//    iCurrentElement = NULL; //will do it in ReaderIndex

    if ( iIgnoreDepth > 0 )
        {
        iIgnoreDepth--;
        if ( iIgnoreDepth == 0 )
            {
            iIsSvgElement = ETrue;
            }
        return ETrue;
        }
    // Notify Loading Listeners of element-end
    // Send info if element is <svg> or immediate child of <svg>
    TBool isRootOrChild = ( iRootElement == iCurrentParentElement ) ||
                        ( iRootElement == iCurrentParentElement->ParentNode() );
    iDocument->NotifyElementEnd( aTagName, isRootOrChild );

    if ( iCurrentParentElement != iRootElement )
        {
        // Depth is decreased, so the current parent should be one level up
        iCurrentParentElement = ( CSvgElementImpl * )
                                iCurrentParentElement->ParentNode();
        }
        
    if( ((CSvgElementImpl*)iCurrentElement->ParentNode()) != NULL )
       {
       if((((((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID()) == KSvgRadialGradientElement) ||
	   ((((CSvgElementImpl*)iCurrentElement->ParentNode())->ElemID()) == KSvgLinearGradientElement) ) &&
	   ( ((CSvgElementImpl*)iCurrentElement)->ElemID() == KSvgStopElement))
	      {
	      CSvgGradientElementImpl *parent = ((CSvgGradientElementImpl *)iCurrentElement->ParentNode());
        
          if(parent)
		     {
		    
	         // Initialize the offset value to 0 if its still -1.
	         TFloatFixPt lOffsetValue;
	         TFloatFixPt lDefaultOffsetValue(-1);
	         TBuf<6>     lAttributeName;
	         TBuf<1>     lValueBuffer;
	                      
	         lAttributeName.Append(OFFSET);
	         lValueBuffer.Append(ZEROVALUE);
	            
	         ((CSvgStopElementImpl*)iCurrentElement)->GetOffset( lOffsetValue );
	            
	         // Offset Value of -1 indicates that Offset Attribute is not declared in
	         // stop element. 
	         if( lOffsetValue == lDefaultOffsetValue )
	            {   
	            // Deliberately calling SetAttributeL in place of SetAttributeFloatL as the latter inturn
	            // calls UpdateOffsetValues which should be called on any Stop element once it is added to
	            // to the Stop element array Owned by parent Gradient element.  
	            TRAP_IGNORE( ((CSvgStopElementImpl*)iCurrentElement)->SetAttributeL( lAttributeName, lValueBuffer ) );
	            }
	            
	         // The function not only adds the element in Stop element array but also
             // adjusts the offset values of all the previously added elements such that
             // each gradient offset value is greater than the previous gradient stop's
             // offset value. It calls UpdateOffsetValues to adjust the values.
	         ((CSvgGradientElementImpl *)parent)->AddStopElementInArray((CSvgStopElementImpl*)iCurrentElement);
		     }
	      }
    }
    
    
    iIsColorAnim = EFalse;
    iFromVal.Set( TPtrC() );
    iToVal.Set( TPtrC() );
    iByVal.Set( TPtrC() );
    iValues.Set(TPtrC() );
    iTimes.Set( TPtrC() );
    iSplines.Set( TPtrC());
    return ETrue;
    }

// this method is called by xml-parser for cdata
// --------------------------------------------------------------------------
// TInt CSvgContentHandler::Charecters( TDesC& aBuf, TInt aStart , TInt aLength )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::Charecters( TDesC& aBuf, TInt aStart , TInt aLength )
    {
    if ( iCancelRequested )
        {
        return EFalse;
        }

    AppendToCData( aBuf );
    if ( !iCurrentElement )
        {
        return ETrue;
        }

    TUint8 id = iCurrentElement->ElemID();
    if ( id == KSvgTextElement || id == KSvgTextAreaElement || id == KSvgDescElement || id == KSvgTitleElement || id == KSvgMetadataElement)
        {

        if(id == KSvgTextElement)
            {
            ((CSvgTextElementImpl*)iCurrentElement)->SetFileOffset(aStart);
            ((CSvgTextElementImpl*)iCurrentElement)->SetOrgLength(aLength);
            }
        else if (id == KSvgTextAreaElement)
            {
            // KSvgTextAreaElement
            ((CSvgTextAreaElementImpl*)iCurrentElement)->SetFileOffset(aStart);
            ((CSvgTextAreaElementImpl*)iCurrentElement)->SetOrgLength(aLength);
            }

        //First get the elements previous text & append the new text
        TPtrC elementText;
        iCurrentElement->GetAttributeDes(KAtrCdata , elementText);
        iCData->Des().Zero();
        AppendToCData(elementText);
        AppendToCData(aBuf);
        TRAPD( error, iCurrentElement->SetAttributeDesL( KAtrCdata, *iCData ) );
        if (error != KErrNone)
            {
                //do something here.
            }
        }
    else if ( id == KSvgScriptElement  )
        {
        RDebug::Printf("Calling SetScriptL\n" );
        TRAPD( error2, ((CSvgScriptElementImpl*)iCurrentElement)->SetScriptL( *iCData ) );
        if (error2 != KErrNone)
            {
            //do something here
            }
        }

    return ETrue;
    }
// --------------------------------------------------------------------------
// void CSvgContentHandler::DataBuffer( const TDesC16& aBuf)
// ---------------------------------------------------------------------------
void CSvgContentHandler::DataBuffer( const TDesC16& aBuf)
    {

    if(iRootElement && !((CSvgSvgElementImpl *)iRootElement)->iContent )
        {

        ((CSvgSvgElementImpl *)iRootElement)->iContent = HBufC::New(aBuf.Length());
        if( ((CSvgSvgElementImpl *)iRootElement)->iContent == NULL)
            return;
        else
            ((CSvgSvgElementImpl *)iRootElement)->iContent->Des().Copy(aBuf);
        }
    return;

    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::Comment( TDesC& /* aComment */ )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::Comment( TDesC& /* aComment */ )
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::ProcessingInstructions( TDesC& /* aTarget */, TDesC& /* aData */ )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::ProcessingInstructions( TDesC& /* aTarget */, TDesC& /* aData */ )
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::IgnoreWhiteSpace( TDesC& /* aString */ )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::IgnoreWhiteSpace( TDesC& /* aString */ )
    {
    return !iCancelRequested;
    }

// ---------------------------------------------------------------------------
// Called by XML parser when a valid '&...;' is encountered
// The entity character must be appended to call previous called of "Charecters"
// & 'StartEntity".
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::StartEntity( TDesC& aName  )
    {
    // Parse entity for possible svg subtree
    // ParseEntity return ETrue if it has valid subtree and parsing successful.
    // Otherwise append entity as part of CData.
    if ( ParseEntity( aName ) )
        {
        return ETrue;
        }

    if ( iCancelRequested )
        {
        return EFalse;
        }

    // content generated with tool uses 0x2019 for apostrophe
    if ( aName.Length() == 1 && aName[0] == 0x2019 )
        {
        AppendToCData( _L( "'" ) );
        }
    else
    {
        AppendToCData( aName );
    }
    // Set CData attribute

    if (!iCurrentElement)
    {
    	return ETrue;
    }

    TUint8 id = iCurrentElement->ElemID();

    if ( (id == KSvgTextElement) || (id == KSvgTextAreaElement) || (id == KSvgTitleElement) || (id == KSvgDescElement) || (id == KSvgMetadataElement))
        {
        TRAPD( error, iCurrentElement->SetAttributeDesL( KAtrCdata, *iCData ) );
        if ( error != KErrNone )
           {
            #ifdef _DEBUG
            RDebug::Printf("CSvgContentHandler::StartEntity error occurred.");
            #endif
           }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Check to see if entity may be needed for parsing
// If so, attemp to parse it as parse of the document
// Return ETrue if parse was successfull, EFalse otherwise.
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ParseEntity( const TDesC& aEntity )
{
    // Look for '<' and '>' to narrow possibility of svg entity
    TInt startPos = aEntity.Locate( TChar( '<' ) );
    if ( startPos == KErrNotFound )
        {
        return EFalse;
        }

    TInt endPos = aEntity.LocateReverse( TChar( '>' ) );
    if ( endPos == KErrNotFound || endPos < startPos )
        {
        return EFalse;
        }

    // Create XML parser
    RXMLReader xmlReader;
    TInt error = KErrNone;
    TRAP( error, xmlReader.CreateL() );
    if ( error != KErrNone )
        {
        return EFalse;
        }

    // Copy entity string to byte-array
    // Enclosed within <g> element to ensure all elements are parsed
    _LIT8( KBeginTag, "<g>" );
    _LIT8( KEndTag, "</g>" );
    HBufC8* buf8 = NULL;
    // create length + room for KBeginTag & KEndTag
    buf8 = HBufC8::New( aEntity.Length() + 7 );
    if ( buf8 == NULL )
        {
        xmlReader.Destroy();
        return EFalse;
        }
    TPtr8 des = buf8->Des();
    des.Copy( KBeginTag );
    des.Append( aEntity );
    des.Append( KEndTag );

    // Parse content
    xmlReader.SetContentHandler( this );
    TRAPD( parseError, xmlReader.ParseL( *buf8 ) );

    // Cleanup
    delete buf8;
    xmlReader.Destroy();

    return parseError == KErrNone;
}


// ---------------------------------------------------------------------------
// Called by XML parser when a valid '&...;' is encountered
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::EndEntity( TDesC&  /*aName*/ )
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::SkippedEntity( TDesC& /*aName*/ )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::SkippedEntity( TDesC& /*aName*/ )
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::StartCDATA()
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::StartCDATA()
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::EndCDATA()
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::EndCDATA()
    {
    return !iCancelRequested;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::Error( TInt /* aErrorCode */, TInt /* aSeverity */ )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::Error( TInt /* aErrorCode */, TInt /* aSeverity */ )
    {
    return EFalse;
    }

/************* End of MXMLContentHandler methods ************/

// --------------------------------------------------------------------------
// void CSvgContentHandler::AppendToCData( const TDesC& aText )
// ---------------------------------------------------------------------------
void CSvgContentHandler::AppendToCData( const TDesC& aText )
{

    // Check to see if appending will overflow
    if ( iCData->Des().MaxLength() < iCData->Length() + aText.Length() )
    {
        HBufC* newBuf = HBufC::NewMax( iCData->Length() + aText.Length() );
        // Fail to allocate new buffer
        if ( newBuf == NULL )
            return;

        newBuf->Des().Copy( *iCData );

        delete iCData;
        iCData = newBuf;
    }

    iCData->Des().Append( aText );
}



// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ProcessColorAttributeL( const TDesC& aName,
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ProcessColorAttributeL( const TDesC& aName,
                                                  const TDesC& aValue )
    {
    TUint32 tempColor;
    if( aValue.Length() )
        {
        if(aValue == _L("freeze") )
            {
            iCurrentElement->SetAttributeL( aName, aValue);
            return ETrue;
            }
        if ( aValue == _L( "inherit" ) )
            {
            return EFalse;
            }
        if ( aValue != _L( "none" ) && aValue != _L( "currentColor" ) && aValue.Left(3)!=_L("url") )

            {
            TBufC<24>   lColBuf;
            TSvgColor tColor(KGfxColorNull);
            if ( tColor.GetStringL( aValue, tempColor ) == EFalse )
                {
                TInt32  defColor  = 0;//default color
                ( ( CSvgElementImpl * ) iCurrentElement->ParentNode() )->GetAttributeIntL( KAtrFill,
                                                                                           defColor );
                _LIT( KFixptFormat, "%d" );
                ( lColBuf.Des() ).Format( KFixptFormat, defColor );
                }
            else
                {
                _LIT( KFixptFormat, "%d" );
                ( lColBuf.Des() ).Format( KFixptFormat, tempColor );
                }
            iCurrentElement->SetAttributeL( aName, lColBuf);
            }
        else
            {
            iCurrentElement->SetAttributeL( aName, aValue);
            }
        return ETrue;
        }
    return EFalse;

    }


// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ProcessStyleL( const TDesC& aValue )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ProcessStyleL( const TDesC& aValue )
    {
    TPtrC remaining;
    remaining.Set( aValue );
    TPtrC name;
    TPtrC value;

    TInt semicolonIndex = remaining.Find( _L( ";" ) );
    while ( semicolonIndex != KErrNotFound )
    {
        // make substring, excluding ';'
        TPtrC nameValuePair;
        nameValuePair.Set( remaining.Ptr(), semicolonIndex );

        // Extract name, value
        if ( ExtractStyleValuePair( nameValuePair, name, value ) )
        {
            // store property
            TInt attributeID = (((CSvgDocumentImpl*)iDocument)->SchemaData())->GetPresentationAttributeId( name );
            if ( attributeID != KErrNotFound )
                iCurrentElement->SetPropertyL( attributeID, value );
            else
                iCurrentElement->SetAttributeL( name, value );
        }

        // shorten remaining, start at first character after semicolon
        remaining.Set( (TUint16*)&remaining.Ptr()[semicolonIndex+1],
                       remaining.Length() - nameValuePair.Length() - 1 );
        semicolonIndex = remaining.Find( _L( ";" ) );
    }

    // process remaining pair if exists
    if ( ExtractStyleValuePair( remaining, name, value ) )
    {
        // store property
        TInt attributeID = (((CSvgDocumentImpl*)iDocument)->SchemaData())->GetPresentationAttributeId( name );
        if ( attributeID != KErrNotFound )
            iCurrentElement->SetPropertyL( attributeID, value );
        else
            iCurrentElement->SetAttributeL( name, value );
    }

    return ETrue;
}


// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ProcessTransformL( const TDesC& /*aName*/,
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ProcessTransformL( const TDesC& /*aName*/,
                                             const TDesC& aValue )
    {

    _LIT( KCb, ")" );
    _LIT(KTransform ,"transform");

    // Prepare the style value string for parsing
    // Should be like transform=matrix( 1 0 0 1 6 6 )
    // Notice the space after and not before ( and the space before )

    HBufC*  tFinal  = HBufC::NewLC( aValue.Length() + 1000 );
    TPtr    tPf     = tFinal->Des();

    // 1. Copy to a temp buffer
    HBufC*  tBufC   = HBufC::NewLC( aValue.Length() );// In case we need room for two extra spaces
    TPtr    tPtr    = tBufC->Des();
    tPtr.Copy( aValue );

    // 2. Replace "funny" chars to spaces
    tPtr.TrimAll();

    TStringTokenizer    lTokenizer  ( tPtr, KCb );
    while ( lTokenizer.HasMoreTokens() )
        {
        TPtrC   lToken  ( lTokenizer.NextToken( KCb ) );

        TInt    pos;
        _LIT( KSp, " " );

        HBufC*  tBufC2  = HBufC::NewLC( lToken.Length() + 4 );
        TPtr    tPtr2   = tBufC2->Des();
        tPtr2.Copy( lToken );
        const TUint16*  lValStr = tPtr2.Ptr();

        // Remove spaces between transformation type keyword and (
        pos = tPtr2.Locate( '(' );
        while ( lValStr[pos - 1] == ' ' )
            {
            tPtr2.Delete( pos - 1, 1 );
            lValStr = tPtr2.Ptr();
            pos = tPtr2.Locate( '(' );
            }

        // Add space after (
        pos = tPtr2.Locate( '(' );
        if ( pos != KErrNotFound &&
             pos < ( tPtr2.Length() - 1 ) &&
             lValStr[pos + 1] != ' ' )
            {
            tPtr2.Insert( pos + 1, KSp );
            lValStr = tPtr2.Ptr();
            }

        // Replace ',' with space
        pos = tPtr2.Locate( ',' );
        while ( pos != KErrNotFound )
            {
            tPtr2.Replace( pos, 1, KSp );
            lValStr = tPtr2.Ptr();
            pos = tPtr2.Locate( ',' );
            }

        // Add space at end
        tPtr2.Append( KSp );
        // Add ) to end
        tPtr2.Append( KCb );
        // Add space at end
        tPtr2.Append( KSp );

        const TUint16*  lVal2   = tPtr2.Ptr();

        tPf.Append( lVal2, tPtr2.Length() );
        CleanupStack::PopAndDestroy( 1 ); // tBufC2
        }

        TBool rVal = iCurrentElement->SetTransform(KTransform,tPf);
        CleanupStack::PopAndDestroy( 2 ); // tBufC & TFinal
                
        if (!rVal)
        {
            return EFalse;
        }

    return ETrue;
    }

// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ProcessUnitsL( const TDesC& /*aName*/, const TDesC& /*aValue*/ )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ProcessUnitsL( const TDesC& /*aName*/, const TDesC& /*aValue*/ )
    {
    return ETrue;
    }

/************** XML Entity Character Methods ****************/

// --------------------------------------------------------------------------
// void CSvgContentHandler::ConvertEntitiesToCharsL( CDesCArrayFlat& aArray )
// ---------------------------------------------------------------------------
void CSvgContentHandler::ConvertEntitiesToCharsL( CDesCArrayFlat& aArray )
    {

    TInt lCount = aArray.Count();
    for ( TInt i = 0; i < lCount; i++ )
    {
        HBufC* substituteString = ConvertEntitiesToCharsL( aArray[i] );
        
        if ( substituteString != NULL )
            {
            CleanupStack::PushL( substituteString );
            aArray.Delete( i );
            aArray.InsertL( i, *substituteString );
            CleanupStack::PopAndDestroy(substituteString);
    				}
        }
    }

// --------------------------------------------------------------------------
// HBufC* CSvgContentHandler::ConvertEntitiesToCharsL( const TDesC& aString )
// ---------------------------------------------------------------------------
HBufC* CSvgContentHandler::ConvertEntitiesToCharsL( const TDesC& aString )
    {
    // no entity, return NULL
    TInt pos = Find( aString, 0, '&' );
    if ( pos == -1 )
        {
        return NULL;
        }

    HBufC* substituteString = HBufC::NewLC( aString.Length() );
    TPtr ptr = substituteString->Des();

    TInt endPos = 0;
    TChar substituteChar = ' ';
    while ( pos != -1 )
        {
        // copy up to pos
        Append( ptr, aString, endPos, pos );

        // no entity ending character, copy to end of string
        endPos = Find( aString, pos, ';' );
        if ( endPos == -1 )
            {
            endPos = aString.Length()-1;
            }

        endPos++;
        HBufC* entity = SubstringL( aString, pos, endPos );
        // entity is converted to char
        if ( ConvertEntityToChar( substituteChar, *entity ) )
            {
            ptr.Append( substituteChar );
            }
        // no such entity, copy original string
        else
            {
            ptr.Append( *entity );
            }
        delete entity;
        pos = Find( aString, endPos, '&' );
        }

    // check end leftover
    if ( endPos < aString.Length() )
        {
        Append( ptr, aString, endPos, aString.Length() );
        }

    CleanupStack::Pop( substituteString );
    return substituteString;
    }

// --------------------------------------------------------------------------
// TInt CSvgContentHandler::Find( const TDesC& aString, TInt aStartIndex, TChar aChar )
// ---------------------------------------------------------------------------
TInt CSvgContentHandler::Find( const TDesC& aString, TInt aStartIndex, TChar aChar )
    {

    TInt lCount = aString.Length();
    for ( TInt i = aStartIndex; i < lCount; i++ )
        {
        if ( aString[i] == (TUint16)aChar )
            {
            return i;
            }
        }
    return -1;
    }


// --------------------------------------------------------------------------
// HBufC* CSvgContentHandler::SubstringL( const TDesC& aString, TInt aStartIndex, TInt aEndIndex )
// ---------------------------------------------------------------------------
HBufC* CSvgContentHandler::SubstringL( const TDesC& aString, TInt aStartIndex, TInt aEndIndex )
    {
    HBufC* substring = HBufC::NewL( aEndIndex - aStartIndex );
    TPtr ptr = substring->Des();

    TInt lCount = aString.Length();
    for ( TInt i = aStartIndex; i < aEndIndex && i < lCount ; i++ )
        {
        ptr.Append( aString[i] );
        }

    return substring;
    }

// --------------------------------------------------------------------------
// void CSvgContentHandler::Append( TDes& aDest, const TDesC& aSource, TInt aStartIndex, TInt aEndIndex )
// ---------------------------------------------------------------------------
void CSvgContentHandler::Append( TDes& aDest, const TDesC& aSource, TInt aStartIndex, TInt aEndIndex )
    {

    TInt lCount = aSource.Length();
    for ( TInt i = aStartIndex; i < aEndIndex && i < lCount; i++ )
        {
        aDest.Append( aSource[i] );
        }
    }

// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ConvertEntityToChar( TChar& aChar, const TDesC& aString )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ConvertEntityToChar( TChar& aChar, const TDesC& aString )
    {
    if ( aString == _L( "&amp;" ) )
        {
        aChar = '&';
        }
    else if ( aString == _L( "&quot;" ) )
        {
        aChar = '"';
        }
    else if ( aString == _L( "&apos;" ) )
        {
        aChar = '\'';
        }
    else if ( aString == _L( "&lt;" ) )
        {
        aChar = '<';
        }
    else if ( aString == _L( "&gt;" ) )
        {
        aChar = '>';
        }
    // need to check for '&#xx'
    else if ( ConvertHexStringToChar( aChar, aString ) )
        {
        }
    else if ( ConvertDecimalStringToChar( aChar, aString ) )
        {
        }

    return ETrue;
    }

// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ConvertDecimalStringToChar( TChar& aChar, const TDesC& aString )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ConvertDecimalStringToChar( TChar& aChar, const TDesC& aString )
    {
    TLex parser( aString );
    // skip '&'
    parser.Inc();
    if ( !parser.Eos() && parser.Peek() == '#' )
        {
        parser.Inc();
        TUint value = 0;
        if ( parser.Val( value, EDecimal ) == KErrNone )
            {
            aChar = value;
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ConvertHexStringToChar( TChar& aChar, const TDesC& aString )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ConvertHexStringToChar( TChar& aChar, const TDesC& aString )
    {
    TLex parser( aString );
    // skip '&'
    parser.Inc();
    if ( !parser.Eos() && parser.Peek() == '#' )
        {
        parser.Inc();
        if ( !parser.Eos() && ( parser.Peek() == 'x' || parser.Peek() == 'X' ) )
            {
            parser.Inc();
            TUint value = 0;
            if ( parser.Val( value, EHex ) == KErrNone )
                {
                aChar = value;
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Filter entity characters for cdata characters
// ---------------------------------------------------------------------------
void CSvgContentHandler::FilterCharactersL( TDesC& aString )
    {
    HBufC* filteredValue = ConvertEntitiesToCharsL( aString );
   
    if ( filteredValue == NULL )
        {
        iCurrentElement->SetAttributeDesL( KAtrCdata, aString );
        }
    else
        {
        CleanupStack::PushL( filteredValue );
        iCurrentElement->SetAttributeDesL( KAtrCdata, *filteredValue );
        CleanupStack::PopAndDestroy(filteredValue);
        }

    }

// ---------------------------------------------------------------------------
// File Offset for the closing tag
// ---------------------------------------------------------------------------
void CSvgContentHandler::ReaderIndex(NW_Uint32 aIndex)
    {
     if ( iCurrentElement )
        {
        TUint8 id = iCurrentElement->ElemID();
        TInt OrgLength;
        if ( id == KSvgTextElement || id == KSvgTextAreaElement)
            {
                if(id==KSvgTextElement)
                    {
                    OrgLength = ((CSvgTextElementImpl *)iCurrentElement)->GetOrgLength();
                    if(!OrgLength)
                        ((CSvgTextElementImpl *)iCurrentElement)->SetFileOffset(aIndex);
                    }
                else
                    {
                    OrgLength = ((CSvgTextAreaElementImpl *)iCurrentElement)->GetOrgLength();
                    if(!OrgLength)
                        ((CSvgTextAreaElementImpl *)iCurrentElement)->SetFileOffset(aIndex);
                    }
            }
        }
        // Set the current element pointer back to parent as we are 
        // going up the tree now.
        iCurrentElement = iCurrentParentElement;
    }


// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ProcessNConvertAttrL( const TDesC& aName, const TDesC& aAttrName,const TDesC& aAttrValue )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ProcessNConvertAttrL( const TDesC& aName, const TDesC& aAttrName,const TDesC& aAttrValue )
    {

    TInt            tmpAttrID = 0;
	TBool isSvgElement = EFalse ;

      if ( 
      (iCurrentElement->ElemID() == KSvgSvgElement) 
      && ( aAttrName == WIDTH )
      )
        {
        TInt lFindValue = aAttrValue.Locate('%');
        isSvgElement = ETrue ;
        if (lFindValue != KErrNotFound)
            {
            ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = ETrue;
            TLex lLex(aAttrValue);
            TReal32 attValue=0;
            if (lLex.Val(attValue, '.' ) == KErrNone)
                {
                ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInUserCoordinate = attValue;
                }
            }
        else
            {
            ((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage = EFalse;
            }
        
        }
     if ( 
      (iCurrentElement->ElemID() == KSvgSvgElement) 
      && ( aAttrName == HEIGHT )
      )
        {
        TInt lFindValue = aAttrValue.Locate('%');
        isSvgElement = ETrue ;
        if (lFindValue != KErrNotFound)
            {
            ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = ETrue;
            TLex lLex(aAttrValue);
            TReal32 attValue=0;
            if (lLex.Val(attValue, '.' ) == KErrNone)
                {
                ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInUserCoordinate = attValue;
                }
            }
        else
            {
            ((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage = EFalse;
            }
        }
        //#ifdef RD_SVGT_MEDIAANIMATION_SUPPORT
   if (aName ==_L("animation")  )
        {
            
            _LIT(KX, "x");
            _LIT(KY, "y");
            if(aAttrName == WIDTH || aAttrName == HEIGHT )
                {
                    TLex lLex(aAttrValue);
                    TReal32 attValue=0;
                    if (lLex.Val(attValue, '.' ) == KErrNone)
                    {
                    	if ( attValue < 0 )//if values are negative then default to 0
                    	attValue = 0;
                        if( aAttrName == WIDTH )
                         ((CSvgMediaAnimationElementImpl *)iCurrentElement)->SetWidth(attValue);
                         else
                         ((CSvgMediaAnimationElementImpl *)iCurrentElement)->SetHeight(attValue);
                    
                    }
                    return ETrue;    
                }
            
             if(!(aAttrName == KX) && !(aAttrName == KY ))
                {
                    HBufC*  tBufC   = HBufC::NewLC( aAttrValue.Length() );
                    TPtr    tPtr    ( tBufC->Des() );
                    tPtr.Copy( aAttrValue );
                    tPtr.TrimAll();
                    iCurrentElement->SetAttributeL(aAttrName,tPtr );   
                    CleanupStack::PopAndDestroy( 1 ); // tBufC
                    return ETrue;
                }
             
                            
        }     
        //endif
    if ( !iBeginEndAtr && ( aAttrName == BEGIN || aAttrName == END ) )
    {

        if ( ( aAttrValue.Find( MOUSEDOWN ) != KErrNotFound ) ||
             ( aAttrValue.Find( MOUSEUP )   != KErrNotFound ) ||
             ( aAttrValue.Find( MOUSEOVER ) != KErrNotFound ) ||
             ( aAttrValue.Find( MOUSEMOVE ) != KErrNotFound ) ||
             ( aAttrValue.Find( MOUSEOUT )  != KErrNotFound ) ||
             ( aAttrValue.Find( FOCUSIN )   != KErrNotFound ) ||
             ( aAttrValue.Find( FOCUSOUT )  != KErrNotFound ) ||
             ( aAttrValue.Find( ACTIVATE )  != KErrNotFound ) ||
             ( aAttrValue.Find( CLICK )     != KErrNotFound ) ||
             ( aAttrValue.Find( ACCESS )    != KErrNotFound ) )

        iBeginEndAtr = ETrue;
    }

    if( iCurrentElement->ElemID() < KSvgAnimateElement || iCurrentElement->ElemID() > KSvgSetElement )
        {
        tmpAttrID = (( ( CSvgDocumentImpl* ) iDocument )->SchemaData())->GetPresentationAttributeId(aAttrName);
        if( tmpAttrID != KErrNotFound )
            {
            iCurrentElement->SetPropertyL( tmpAttrID, aAttrValue);
            return ETrue;
            }
        }

    tmpAttrID = (( ( CSvgDocumentImpl* ) iDocument )->SchemaData())->GetSVGTAttributeId(aAttrName);

    if ( iCurrentElement->ElemID() == KSvgTextAreaElement && aAttrName == _L("editable") )
        {
            ((CSvgTextAreaElementImpl*)iCurrentElement)->SetEditable( aAttrValue );
        }
    if ( iCurrentElement->ElemID() == KSvgTextElement && aAttrName == _L("editable") )
        {
            ((CSvgTextElementImpl*)iCurrentElement)->SetEditable( aAttrValue );
        }
    
    if ( iCurrentElement->ElemID() >= KSvgMediaElemsStartIndex &&
                 iCurrentElement->ElemID() <= KSvgMediaElemsEndIndex )
        {
        TBool lIsAttrProcessed = ETrue;
        switch ( tmpAttrID )
            {
            case KAtrSyncBehavior:
                ((CSvgMediaElementBase* )iCurrentElement)->SetSyncBehavior( 
                    aAttrValue );
                break;
            case KAtrSyncTolerance:
                ((CSvgMediaElementBase* )iCurrentElement)->SetSyncTolerance( 
                    aAttrValue );
                break;
            case KAtrSyncMaster:
                ((CSvgMediaElementBase* )iCurrentElement)->SetSyncMaster( 
                    aAttrValue );
                break;
            default:
                lIsAttrProcessed = EFalse;
            }
        if ( lIsAttrProcessed )
            {
            return ETrue;
            }
        }
    
    if ( iCurrentElement->ElemID() == KSvgSvgElement )
        {
        TBool lIsAttrProcessed = ETrue;
        switch ( tmpAttrID )
            {
            case KAtrSyncBehaviorDefault:
                ((CSvgSvgElementImpl* )iCurrentElement)->SetSyncBehaviorDefault(
                    aAttrValue );
                break;
            case KAtrSyncToleranceDefault:
                ((CSvgSvgElementImpl* )iCurrentElement)->SetSyncToleranceDefault(
                    aAttrValue );
                break;                
            default:
                lIsAttrProcessed = EFalse;
            }
        if ( lIsAttrProcessed )
            {
            return ETrue;
            }            
        }
    
    if ( iCurrentElement->ElemID() == KSvgMediaAnimationElement && 
        tmpAttrID == KAtrInitialVisibility )
        {
        // Merge with the animation->SetInitialVisibility here.
        return ETrue;
        }
    
    
    if((tmpAttrID == KAtrOffset)|| ((tmpAttrID >= KSvgNotHandledAttrsIndex
                    ||  (tmpAttrID==KAtrRotate && iCurrentElement->ElemID()==KSvgAnimateMotionElement)) && (tmpAttrID != KAtrValues )
                    && (tmpAttrID != KAtrKeySplines) && ( tmpAttrID != KAtrKeyTimes))
                    &&(tmpAttrID != KAtrFx)&& (tmpAttrID != KAtrFy)&& (tmpAttrID != KAtrGradientTransform)
          && (aName !=_L("animation") )          )
        {
          HBufC*  tBufC   = HBufC::NewLC( aAttrValue.Length() );
          TPtr    tPtr    ( tBufC->Des() );
          tPtr.Copy( aAttrValue );
          tPtr.TrimAll();
          TInt errorCode = iCurrentElement->SetAttributeL(aAttrName,tPtr );
          if(errorCode == KErrReferencedElementNotFound)
              {
              //Error Code returned By USE Element when Forward reference element not Found
              iUseElementArray->AppendL(iCurrentElement);
              }
          if(errorCode == KErrAnimateReferenceElementNotFound )
            {
            //Error Code returned By Animation Element when Forward reference element not Found
            iAnimRefElementArray->AppendL(iCurrentElement);
            }

          CleanupStack::PopAndDestroy( 1 );

        }
    else if( tmpAttrID == KAtrAttributeName )
            {
            if( aAttrValue == FILL || aAttrValue == STROKE || aAttrValue == COLOR
                || aAttrValue == STOP_COLOR )
                {
                iIsColorAnim = ETrue;
                }
                HBufC*  tBufC   = HBufC::NewLC( aAttrValue.Length() );
                TPtr    tPtr    ( tBufC->Des() );
                tPtr.Copy( aAttrValue );
                tPtr.TrimAll();
            iCurrentElement->SetAttributeL(aAttrName,tPtr );
            CleanupStack::PopAndDestroy( 1 );
            }
    else if( tmpAttrID == KAtrValues )
            {
            iValues.Set(TPtrC( aAttrValue ) );
            }
    else if( tmpAttrID == KAtrKeyTimes )
        {
    	iTimes.Set(TPtrC( aAttrValue ));
        }
    else if( tmpAttrID == KAtrKeySplines )
        {
    	iSplines.Set(TPtrC (aAttrValue));
            }
    else if( tmpAttrID == KAtrTransform &&
            (iCurrentElement->ElemID() != KSvgRadialGradientElement && iCurrentElement->ElemID()
             != KSvgLinearGradientElement)
             )
            {
            ProcessTransformL(aAttrName,aAttrValue );
            }
            //Added the below else-if block  to support  the % values for X1,Y1,X2,Y2 of LinearGradient element.
    else if( tmpAttrID >= KAtrY1 && tmpAttrID <= KAtrX2 && iCurrentElement->ElemID() == KSvgLinearGradientElement )
            {
            iCurrentElement->SetAttributeL( aAttrName , aAttrValue);
            }
    else if (tmpAttrID == KAtrGradientTransform &&
            (iCurrentElement->ElemID() == KSvgRadialGradientElement || iCurrentElement->ElemID()
             == KSvgLinearGradientElement))
            {
            ProcessTransformL(aAttrName,aAttrValue );

            }
    else if( tmpAttrID == KAtrStyle )
            {
            ProcessStyleL(aAttrValue);
            }

    else if( tmpAttrID == KAtrFrom)
            {
            iFromVal.Set( TPtrC( aAttrValue ) );
            }
    else if( tmpAttrID == KAtrTo)
            {
            iToVal.Set( TPtrC( aAttrValue ) );
            }
    else if ( tmpAttrID == KAtrBy )
            {
            iByVal.Set( TPtrC( aAttrValue ) );
            }
    else if (( tmpAttrID >= KSvgDesAttrsIndex ||
             (iCurrentElement->ElemID() == KSvgTextElement &&
                (tmpAttrID == KAtrX || tmpAttrID == KAtrY))) && (tmpAttrID != KAtrFx)&& (tmpAttrID != KAtrFy)
               &&(aName !=_L("animation") ) )


            {
            iCurrentElement->SetAttributeDesL( tmpAttrID, aAttrValue);
            }
    else if ( iCurrentElement->ElemID() == KSvgScriptElement )
        {
            iCurrentElement->SetAttributeL( aAttrName, aAttrValue);
        }
    else
        {
            // check if the datatype is lenth.datatype or coordinate.datatype
        TUint8 datatype = ( (TUint8) (tmpAttrID < KSvgCoordAttrStartIndex ? KSVG_LENGTH_TYPE : KSVG_COORDINATE_TYPE) );
        TReal32 convertedValue;
        if( CSvgAttributeVerifier::ValidAttrValue(aAttrValue, convertedValue, datatype, aAttrName, isSvgElement) )
                {
                //check for valid width height 
                //specific checking for svgElement width height
                //particularly the case when width height values 
                //in percentages cannot be greater than 100
                if( iCurrentElement->ElemID() == KSvgSvgElement )
                {
                    if(
                    (aAttrName == WIDTH )
                    &&(((CSvgSvgElementImpl *)iCurrentElement)->iWidthInPercentage)
                    )
                    {
                        if(convertedValue > 100)
                            convertedValue = 100;
                    }
                    
                    if(
                    (aAttrName == HEIGHT)
                    &&(((CSvgSvgElementImpl *)iCurrentElement)->iHeightInPercentage)
                    )
                    {
                        if(convertedValue > 100)
                            convertedValue = 100;
                    }
                }
                if((tmpAttrID < KSvgDesAttrsIndex) || (tmpAttrID == KAtrFy || tmpAttrID == KAtrFx))
                    {
                    iCurrentElement->SetAttributeFloatL( tmpAttrID, ((TFloatFixPt) convertedValue));
                    }
                else
                    {
                    iCurrentElement->SetAttributeIntL( tmpAttrID, (TInt) convertedValue );
                    }
                }
            else
                {
                //if invalid width height specified then 
                //set width/height values as zero
                 if(
                 ( iCurrentElement->ElemID() == KSvgSvgElement )
                 &&(aAttrName == WIDTH || aAttrName == HEIGHT)
                 )
                 {
                    convertedValue = 0;
                    iCurrentElement->SetAttributeFloatL( tmpAttrID, (TFloatFixPt) convertedValue );
                 }
                _LIT(STRING1, "Invalid \"");
                _LIT(STRING2, "\" value for <");
                _LIT(STRING3, "> : \"");
                _LIT(STRING4, "\"" );
                iSvgError->SetErrorCode( ESvgInvalidAttributeValue );
                iSvgError->SetIsWarning( ETrue );
                iSvgError->SetDescription( STRING1 );
                iSvgError->AppendDescription( aAttrName);
                iSvgError->AppendDescription( STRING2 );
                iSvgError->AppendDescription( aName );
                iSvgError->AppendDescription( STRING3 );
                iSvgError->AppendDescription( aAttrValue );
                iSvgError->AppendDescription( STRING4 );
                }
                return ETrue;
        }
    return ETrue;
    }

// --------------------------------------------------------------------------
// TBool CSvgContentHandler::ExtractStyleValuePair( const TDesC& aString, TPtrC& aName, TPtrC& aValue )
// ---------------------------------------------------------------------------
TBool CSvgContentHandler::ExtractStyleValuePair( const TDesC& aString, TPtrC& aName, TPtrC& aValue )
{
    // find pair divide char index ":"
    TInt colonIndex = aString.Find( _L( ":" ) );

    // ':' not found
    if ( colonIndex == KErrNotFound )
        return EFalse;

    // Skip front white spaces
    TInt end = aString.Length();
    TInt frontIndex = 0;
    while ( frontIndex < end && TChar( aString[frontIndex] ).IsSpace() )
        frontIndex++;

    // skip back white spaces
    TInt backIndex = colonIndex - 1;
    while ( backIndex > frontIndex && TChar( aString[backIndex] ).IsSpace() )
        backIndex--;

    aName.Set( (TUint16*)&aString.Ptr()[frontIndex], backIndex - frontIndex + 1 );

    // parse for value
    // skip front spaces
    frontIndex = colonIndex + 1;
    while ( frontIndex < end && TChar( aString[frontIndex] ).IsSpace() )
        frontIndex++;

    // assuming ';' is not passed in at the end
    // start from end and skip spaces
    backIndex = end - 1;
    while ( backIndex > frontIndex && TChar( aString[backIndex] ).IsSpace() )
        backIndex--;

    aValue.Set( (TUint16*)&aString.Ptr()[frontIndex], backIndex - frontIndex + 1 );

    return ETrue;
}

// --------------------------------------------------------------------------
// void CSvgContentHandler::SetForwardReferences(CSvgErrorImpl& aError)
// ---------------------------------------------------------------------------
void CSvgContentHandler::SetForwardReferences(CSvgErrorImpl& aError)
{
    /*Once the whole DOM is constructed, Forward references need to be set*/
    /*For USE elements*/
    TInt lCount = iUseElementArray->Count();
    TInt lIndex = 0;
    TInt error = KErrNone;

    while(lIndex < lCount)
    {
        TPtrC tPtr2 = ((CSvgElementImpl*)iUseElementArray->operator[](lIndex ))->Href();
        if(((CSvgUseElementImpl *)iUseElementArray->operator[](lIndex ))->SetRefElemById(tPtr2) != KErrNotFound)
        {
            TRAP(error ,((CSvgUseElementImpl *)iUseElementArray->operator[](lIndex ))->SetReferenceElementL());
        }
        else
        {
            aError.SetErrorCode( ESvgInvalidAttributeValue );
            _LIT(STRING1, "Invalid attribute \n");
            _LIT(STRING2, "USE element has invalid xlink:href attribute");
            aError.SetDescription( STRING1 );
            aError.AppendDescription( STRING2 );
            aError.SetIsWarning( EFalse );
            break;
        }

        lIndex ++;
    }

    /*For Animation Elements*/
    lCount = iAnimRefElementArray->Count();
    lIndex = 0;
    while(lIndex < lCount)
    {
        TPtrC tPtr2 = ((CSvgElementImpl*)iAnimRefElementArray->operator[](lIndex ))->Href();
        ((CSvgAnimationBase *)iAnimRefElementArray->operator[](lIndex ))->SetRefElemById(tPtr2);
        lIndex ++;
    }


    /*For Animation elements */
    lCount = iAnimationElementArray->Count();
    lIndex = 0;
    while(lIndex < lCount)
    {
//        ((CSvgAnimationElementImpl *)iAnimationElementArray->operator[](lIndex))->ResetEventListener();
		((CSvgAnimationBase *)iAnimationElementArray->operator[](lIndex))->CheckBeginTime();
        lIndex ++;
    }

}

//---------------------------------------------------
//Removal of elements that dont pass
//required extensions, required features, and system language
//requirements
//will flag as invisible if remove flag not turned on
//switch statement is special case were only the first one
//that passes these requirements will be used so toss the rest
//---------------------------------------------------
void CSvgContentHandler::RemoveFalseElementsOrMakeInvisible()
    {
    if ( iReqFetAttSysArray == NULL )
        {
        // Error Case
        return;        
        }

    //loop through the list of elements with required features, attributes, or sys language
    TInt reqFetAttSysArrayCnt = iReqFetAttSysArray->Count();
    // Start processing from the end of the list so to follow the LIFO principle (process 
    // from leaf node to root of DOM tree).
    while ( reqFetAttSysArrayCnt > 0 )
        {
        // Access the last element in the array
        CSvgElementImpl* lCurElement = ( CSvgElementImpl* )
                iReqFetAttSysArray->operator[]( reqFetAttSysArrayCnt - 1 );

        if ( lCurElement != NULL )
            {
            // Save the parent node as this would be required for removing 
            // this element from the DOM tree
            CSvgElementImpl* lParent = ( CSvgElementImpl* )
                lCurElement->ParentNode();

            if ( lParent == NULL )
                {
                iReqFetAttSysArray->Reset();
                return;
                }

            //just a normal element check it and remove it if it doesnt pass
            TBool lResult = EFalse;
            TRAPD(error, lResult  = VerifyReqExReqFtrSysL( lCurElement ));
            if ( error == KErrNone && !lResult )
                {
                if ( iRemoveFalseSwitchElements )
                    {
                    // Remove references of this element in Content Handler
                    // This function removes the element from the iReqFetAttSysArray as well
                    RemoveInternalReferences( lCurElement );
                    lParent->RemoveChild( lCurElement); 
                    delete lCurElement; 
                    lCurElement = NULL; 
                    }
                else
                    {
                    //if not removing elements then at least set to false
                    TRAP_IGNORE( 
                            lCurElement->SetAttributeIntL( 
                                KCSS_ATTR_VISIBILITY, 1 ) );
                    // Remove the element from the iReqFetAttSysArray
                    iReqFetAttSysArray->Remove( reqFetAttSysArrayCnt - 1 );
                    } // if ( iRemoveFalseSwitchElements )
                }
            else 
                {
                // Remove the element from the iReqFetAttSysArray
                iReqFetAttSysArray->Remove( reqFetAttSysArrayCnt - 1 );
                } // if ( error == KErrNone && !lResult )
            }
        else
            {
            // Remove the element from the iReqFetAttSysArray
            iReqFetAttSysArray->Remove( reqFetAttSysArrayCnt - 1 );
            } // if ( lCurElement != NULL )

        // Update the array count
        reqFetAttSysArrayCnt = iReqFetAttSysArray->Count();            
        } // while ( reqFetAttSysArrayCnt > 0 )
    // Reset the array as it is no longer required.
    iReqFetAttSysArray->Reset();
    }



// --------------------------------------------------------------------------
// void CSvgContentHandler::RemoveStyleProperties()
// ---------------------------------------------------------------------------
/*

//MEMORY MANAGER now in charge of style deletion
void CSvgContentHandler::RemoveStyleProperties(CSvgElementImpl* aElement)
    {
    CSvgElementImpl* child = (CSvgElementImpl*)aElement->FirstChild();
    while ( child != NULL )
    	{
    	child->DeleteStyleProperties(child);
        RemoveStyleProperties( child );
        child = (CSvgElementImpl*)child->NextSibling();
    	}
    }
*/


// --------------------------------------------------------------------------
// void CSvgContentHandler::RemoveFalseSwitchCases()
// ---------------------------------------------------------------------------
void CSvgContentHandler::RemoveFalseSwitchCasesOrMakeInvisible()
    {
    if ( iSwitchElementArray == NULL )
        {
        // Error Case
        return;
        }

    TInt switchEleArrayCnt = iSwitchElementArray->Count();

    while ( switchEleArrayCnt > 0 )
        {
        TBool foundTrue = EFalse;
        CSvgElementImpl* curSwitchElem = 
            (CSvgElementImpl*)iSwitchElementArray->operator[]( switchEleArrayCnt - 1 );
        // Remove the switch element from the array. This is done here as 
        // below statements could result in iSwitchElementArray getting modified 
        // in RemoveInternalReferences. 
        iSwitchElementArray->Remove( switchEleArrayCnt - 1 );
        if ( curSwitchElem != NULL )
            {
            //get the first child...which is where the first
            CSvgElementImpl* curCaseElem = (CSvgElementImpl*)curSwitchElem->FirstChild();

            while ( curCaseElem != NULL )
                {
                CSvgElementImpl* nextCaseElem = (CSvgElementImpl*)curCaseElem->NextSibling();
                // foundTrue is set to TRUE when an element whose test passes is found. The
                // subsequent elements are to be removed.
                if ( foundTrue )
                    {
                    if ( iRemoveFalseSwitchElements )
                        {
                        //already found the true case in the switch delete the rest
                        // Remove references of this element in Content Handler
                        // This function removes the element from the 
                        // iSwitchElementArray as well
                        RemoveInternalReferences( curCaseElem );
                        curSwitchElem->RemoveChild( curCaseElem );
                        delete ( CXmlElementImpl* )curCaseElem;
                        curCaseElem = NULL;
                        }
                    else
                        {
                        //not removing elements from tree just make invisible
                        TRAP_IGNORE( curCaseElem->SetAttributeIntL( KCSS_ATTR_VISIBILITY, 1 ) );
                        }
                    }
                else
                    {
                    // Not yet found an element whose test passes,
                    // perform the test on the current element to check 
                    // whether it does.
                    TBool result= EFalse;
                    TRAPD( error, result= VerifyReqExReqFtrSysL( curCaseElem ) );
                    if ( error == KErrNone )
                        {
                        if ( !result )
                            {
                            if ( iRemoveFalseSwitchElements )
                                {
                                // this element doesnt meet the switch requirements 
                                // delete it and its children
                                // Remove references of this element in Content Handler
                                // This function removes the element from the 
                                // iSwitchElementArray as well
                                RemoveInternalReferences( curCaseElem );                                    
                                curSwitchElem->RemoveChild( curCaseElem );
                                delete ( CXmlElementImpl* )curCaseElem;
                                curCaseElem = NULL;
                                }
                            else
                                {
                                //not removing elements from tree just make invisible
                                TRAP_IGNORE( curCaseElem->SetAttributeIntL( 
                                            KCSS_ATTR_VISIBILITY, 1 ) );
                                }
                            }
                        else
                            {
                            //one evaluated to true so keep it but go ahead and delete the rest in the switch
                            //should only be one child for switch in end
                            foundTrue = ETrue;
                            }
                        }
                    } // if ( foundTrue )

                // Check the next case element
                curCaseElem = nextCaseElem;
                } // while ( curCaseElem != NULL )
            } // if ( curSwitchElem != NULL )
        // Update the count of elements
        switchEleArrayCnt = iSwitchElementArray->Count();
        } // while ( switchEleArrayCnt > 0 )
    // Reset the array as it is no longer required.
    iSwitchElementArray->Reset();
    }

//---------------------------------------------------
//Check to see
//required extensions, required features, and system language
//requirements
//---------------------------------------------------
TBool CSvgContentHandler::VerifyReqExReqFtrSysL( CSvgElementImpl* aElement )
{
    const CDesCArrayFlat* tempTestArray = NULL; // 'const' keyword added due to S60 (CW) build team recommendation

    TBool   doDraw  = ETrue;

    // First check for requiredExtensions
    aElement->GetRequiredExtensions( tempTestArray );

    if ( tempTestArray && tempTestArray->MdcaCount() )
    {
        // Check for all entries in requiredExtensions

        TInt lCount = tempTestArray->MdcaCount();

        for ( TInt i = 0; i < lCount; i++ )
        {
            TPtrC tmpPtr = tempTestArray->MdcaPoint( i );
            if ( tmpPtr.Length() )
            {
                doDraw = EFalse;
                break;
            }
            else if ( aElement->HasExtension( tmpPtr ) == EFalse )
            {
                doDraw = EFalse;
                break;
            }
        }
    }

    // Second, check for requiredFeatures
    aElement->GetRequiredFeatures( tempTestArray );
    if ( tempTestArray && tempTestArray->MdcaCount() && doDraw)
    {
        // Check for all entries in requiredFeatures
      TInt lCount = tempTestArray->MdcaCount();
      for ( TInt i = 0; i < lCount; i++ )
      {
        TPtrC tmpPtr = tempTestArray->MdcaPoint( i );
        if ( aElement->HasFeature( tmpPtr ) == EFalse )
        {
            doDraw = EFalse;
            break;
        }
      }
    }

    //set up so it only gets it once...
    //just fetch it one time...
    if (iSystemLanguage->Length() == 0)
    {
        SystemLanguage( iSystemLanguage->Des() );
    }

    // Third, check for systemLanguage
    // SYSTEM LANGUAGE DOESNT NEED TO BE STORED IN SEPERATE ARRAY INDEXES
    // COULD HAVE JUST STORED IT AS ONE BIG STRING AND USE FINDF without the loop
    TBool lIsNotEmptyString = aElement->GetSystemLanguage( tempTestArray );

    if (!lIsNotEmptyString)
    {
    	//system language was empty string so dont draw
    	return EFalse;
    }

    TBool doDrawSystem = EFalse;
    if ( tempTestArray && tempTestArray->MdcaCount() && doDraw)
    {
        TInt lCount = tempTestArray->MdcaCount();

        for ( TInt i = 0; i < lCount; i++ )
        {
            TPtrC tmpPtr = tempTestArray->MdcaPoint( i );

			//tmpPtr is the language that was on the element
            //match entire content specified string to system systemlanguage
            if ( tmpPtr.FindF( iSystemLanguage->Des() ) == 0 )
            {
                doDrawSystem = ETrue;
                break;
            }

            //match just the prefix of the system systemlanugage to the content specified syslang
            if ( iSystemLanguage->Des().Left(2).FindF(tmpPtr) == 0 )
            {
            	doDrawSystem = ETrue;
            	break;
            }
        }

        if (doDrawSystem == EFalse)
        {
            doDraw = EFalse;
        }
     }

    return doDraw;
}

// --------------------------------------------------------------------------
// void CSvgContentHandler::SystemLanguage( TPtr aValue )
// ---------------------------------------------------------------------------
void CSvgContentHandler::SystemLanguage( TPtr aValue )
    {
    _LIT( KEn, "en" );
    _LIT( KFr, "fr" );
    _LIT( KDe, "de" );
    _LIT( KEs, "es" );

    _LIT( KAf, "af" );
    _LIT( KAm, "am" );
    _LIT( KAr, "ar" );
    _LIT( KBg, "bg" );
    _LIT( KBn, "bn" );
    _LIT( KBo, "bo" );
    _LIT( KCa, "ca" );
    _LIT( KCs, "cs" );
    _LIT( KCy, "cy" );
    _LIT( KDa, "da" );
    _LIT( KEl, "el" );
    _LIT( KEt, "et" );
    _LIT( KFa, "fa" );
    _LIT( KFi, "fi" );
    _LIT( KGa, "ga" );
    _LIT( KGd, "gd" );
    _LIT( KGu, "gu" );
    _LIT( KHe, "he" );
    _LIT( KHi, "hi" );
    _LIT( KHu, "hu" );
    _LIT( KHr, "hr" );
    _LIT( KHy, "hy" );
    _LIT( KId, "id" );
    _LIT( KIs, "is" );
    _LIT( KIt, "it" );
    _LIT( KJa, "ja" );
    _LIT( KKa, "ka" );
    _LIT( KKk, "kk" );
    _LIT( KKm, "km" );
    _LIT( KKn, "kn" );
    _LIT( KKo, "ko" );
    _LIT( KLo, "lo" );
    _LIT( KLt, "lt" );
    _LIT( KLv, "lv" );
    _LIT( KMk, "mk" );
    _LIT( KMl, "ml" );
    _LIT( KMn, "mn" );
    _LIT( KMo, "mo" );
    _LIT( KMr, "mr" );
    _LIT( KMs, "ms" );
    _LIT( KMy, "my" );
    _LIT( KNo, "no" );
    _LIT( KNl, "nl" );
    _LIT( KPa, "pa" );
    _LIT( KPl, "pl" );
    _LIT( KPt, "pt" );
    _LIT( KRo, "ro" );
    _LIT( KRu, "ru" );
    _LIT( KSi, "si" );
    _LIT( KSk, "sk" );
    _LIT( KSl, "sl" );
    _LIT( KSo, "so" );
    _LIT( KSr, "sr" );
    _LIT( KSq, "sq" );
    _LIT( KSv, "sv" );
    _LIT( KSw, "sw" );
    _LIT( KTa, "ta" );
    _LIT( KTe, "te" );
    _LIT( KTh, "th" );
    _LIT( KTi, "ti" );
    _LIT( KTk, "tk" );
    _LIT( KTl, "tl" );
    _LIT( KTr, "tr" );
    _LIT( KUk, "uk" );
    _LIT( KUr, "ur" );
    _LIT( KVi, "vi" );
    //_LIT( KZh, "zh" );
    _LIT( KZu, "zu" );

    _LIT( KEnB, "en-UK");
    _LIT( KEnUS, "en-US");
    _LIT( KZhTW, "zh-TW");
    _LIT( KZhHK, "zh-HK");
    _LIT( KZhCN, "zh-CN");
    _LIT( KFrCA, "fr-CA");
    _LIT( KPtBR, "pt-BR");
    _LIT( KEnTW, "en-TW");
    _LIT( KEnHK, "en-HK");
    _LIT( KEnCN, "en-CN");
    _LIT( KEnJP, "en-JP");
    _LIT( KEnTH, "en-TH");
    _LIT( KEsAR, "es-AR");
    _LIT( KMsAP, "ms-AP");
    _LIT( KEnAP, "en-AP" );    // KLangApacEnglish
    _LIT( KIdAP, "id-AP" );    // KLangApacIndonesian
    _LIT( KEu, "eu" );    // KLangBasque
    _LIT( KGl, "gl" );    // KLangGalician


    _LIT(KDefault, "qqqqq");

    switch ( User::Language() )
        {
        case ELangTest:
        aValue = KEn;
        break;

        case ELangEnglish:
        aValue = KEnB;
        break;
        case ELangAmerican:
        aValue = KEnUS;
        break;
        case ELangAustralian:
        case ELangNewZealand:
        case ELangCanadianEnglish:
        case ELangSouthAfricanEnglish:
        case ELangInternationalEnglish:
        aValue = KEn;
        break;

        case ELangFrench:
        case ELangSwissFrench:
        case ELangBelgianFrench:
        aValue = KFr;
        break;

        case ELangGerman:
        case ELangAustrian:
        case ELangSwissGerman:
        aValue = KDe;
        break;

        case ELangSpanish:
        case ELangInternationalSpanish:
        aValue = KEs;
        break;

        case ELangLatinAmericanSpanish:
        aValue = KEsAR;
        break;

        case ELangItalian:
        case ELangSwissItalian:
        aValue = KIt;
        break;

        case ELangSwedish:
        case ELangFinlandSwedish:
        aValue = KSv;
        break;

        case ELangDanish:
        aValue = KDa;
        break;

        case ELangNorwegian:
        case ELangNorwegianNynorsk:
        aValue = KNo;
        break;

        case ELangFinnish:
        aValue = KFi;
        break;

        case ELangBrazilianPortuguese:
        aValue = KPtBR;
        break;

        case ELangPortuguese:
        aValue = KPt;
        break;

        case ELangTurkish:
        case ELangCyprusTurkish:
        aValue = KTr;
        break;

        case ELangIcelandic:
        aValue = KIs;
        break;

        case ELangRussian:
        case ELangBelarussian:
        aValue = KRu;
        break;

        case ELangHungarian:
        aValue = KHu;
        break;

        case ELangDutch:
        case ELangBelgianFlemish:
        aValue = KNl;
        break;

        case ELangCzech:
        aValue = KCs;
        break;

        case ELangSlovak:
        aValue = KSk;
        break;

        case ELangPolish:
        aValue = KPl;
        break;

        case ELangSlovenian:
        aValue = KSl;
        break;

        case ELangPrcChinese:
        aValue = KZhCN;
        break;
        case ELangTaiwanChinese:
        aValue = KZhTW;
        break;
        case ELangHongKongChinese:
        aValue = KZhHK;
        break;

        case ELangJapanese:
        aValue = KJa;
        break;

        case ELangThai:
        aValue = KTh;
        break;

        case ELangAfrikaans:
        aValue = KAf;
        break;

        case ELangAlbanian:
        aValue = KSq;
        break;

        case ELangAmharic:
        aValue = KAm;
        break;

        case ELangArabic:
        aValue = KAr;
        break;

        case ELangArmenian:
        aValue = KHy;
        break;

        case ELangTagalog:
        aValue = KTl;
        break;

        case ELangBengali:
        aValue = KBn;
        break;

        case ELangBulgarian:
        aValue = KBg;
        break;

        case ELangBurmese:
        aValue = KMy;
        break;

        case ELangCatalan:
        aValue = KCa;
        break;

        case ELangCroatian:
        aValue = KHr;
        break;

        case ELangEstonian:
        aValue = KEt;
        break;

        case ELangFarsi:
        aValue = KFa;
        break;

        case ELangCanadianFrench:
        aValue = KFrCA;
        break;

        case ELangScotsGaelic:
        aValue = KGd;
        break;

        case ELangGeorgian:
        aValue = KKa;
        break;

        case ELangGreek:
        case ELangCyprusGreek:
        aValue = KEl;
        break;

        case ELangGujarati:
        aValue = KGu;
        break;

        case ELangHebrew:
        aValue = KHe;
        break;

        case ELangHindi:
        aValue = KHi;
        break;

        case ELangIndonesian:
        aValue = KId;
        break;

        case ELangIrish:
        aValue = KGa;
        break;

        case ELangKannada :
        aValue = KKn;
        break;


        case ELangKazakh:
        aValue = KKk;
        break;

        case ELangKhmer:
        aValue = KKm;
        break;

        case ELangKorean:
        aValue = KKo;
        break;

        case ELangLao:
        aValue = KLo;
        break;

        case ELangLatvian:
        aValue = KLv;
        break;

        case ELangLithuanian:
        aValue = KLt;
        break;

        case ELangMacedonian:
        aValue = KMk;
        break;

        case ELangMalay:
        aValue = KMs;
        break;

        case ELangMalayalam:
        aValue = KMl;
        break;

        case ELangMarathi:
        aValue = KMr;
        break;

        case ELangMoldavian:
        aValue = KMo;
        break;

        case ELangMongolian:
        aValue = KMn;
        break;

        case ELangPunjabi:
        aValue = KPa;
        break;

        case ELangRomanian:
        aValue = KRo;
        break;

        case ELangSerbian:
        aValue = KSr;
        break;

        case ELangSinhalese:
        aValue = KSi;
        break;

        case ELangSomali:
        aValue = KSo;
        break;

        case ELangSwahili:
        aValue = KSw;
        break;

        case ELangTamil:
        aValue = KTa;
        break;

        case ELangTelugu:
        aValue = KTe;
        break;

        case ELangTibetan:
        aValue = KBo;
        break;

        case ELangTigrinya:
        aValue = KTi;
        break;

        case ELangTurkmen:
        aValue = KTk;
        break;

        case ELangUkrainian:
        aValue = KUk;
        break;

        case ELangUrdu:
        aValue = KUr;
        break;

        case ELangVietnamese:
        aValue = KVi;
        break;

        case ELangWelsh:
        aValue = KCy;
        break;

        case ELangZulu:
        aValue = KZu;
        break;

        // from \\epoc32\\include\\oem\\languages.hrh

        case KLangTaiwanEnglish:
        aValue = KEnTW;
        break;

        case KLangHongKongEnglish:
        aValue = KEnHK;
                break;

        case KLangPrcEnglish:
        aValue = KEnCN;
        break;

        case KLangJapaneseEnglish:
        aValue = KEnJP;
        break;

        case KLangThaiEnglish:
        aValue = KEnTH;
        break;

        case KLangApacMalay:
        aValue = KMsAP;
        break;

        case KLangApacEnglish:
            {
            aValue = KEnAP;
            break;
            }
        case KLangApacIndonesian:
            {
            aValue = KIdAP;
            break;
            }
        case KLangBasque:
            {
            aValue = KEu;
            break;
            }
        case KLangGalician:
            {
            aValue = KGl;
            break;
            }

        // Cingular English
        case 6154:
        aValue = KEnUS;
        break;

        default:
            {
            aValue = KDefault;
            }

        break;
        }
    }

// --------------------------------------------------------------------------
// void CSvgContentHandler::CancelParsing()
// ---------------------------------------------------------------------------
void CSvgContentHandler::CancelParsing()
    {
        iCancelRequested = ETrue;
    }

// --------------------------------------------------------------------------
// void CSvgContentHandler::LoadImages()
// ---------------------------------------------------------------------------
void CSvgContentHandler::LoadImages()
    {
    TInt imageEleCnt = iImageElements.Count();
    for ( TInt i = 0; i < imageEleCnt; i++ )
        {
        TRAPD( error, iImageElements[i]->LoadUriL() );
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("LoadUriL failed: %d\n", error );
            #endif
            }
        }
    }
