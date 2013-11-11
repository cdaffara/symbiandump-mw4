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


#include "SVGEngineImpl.h"
#include "SVGEngineInterfaceImpl.h"

#include "GfxAffineTransform.h"

#include "SVGTimer.h"
#include "SVGSvgElementImpl.h"
//#include "SVGTransformableImpl.h"
//#include "SVGSchemaData.h"

//#include "GfxPoint2D.h"
//#include "GfxRectangle2D.h"

#include "SVGErrorImpl.h"

// added for JSR support
#include  "SVGRectElementImpl.h"
#include "SVGAnimationBase.h"
#include "e32math.h"

//#include "SVGFourPointRect.h"
#include "SVGTextElementImpl.h"
#include "SVGTextAreaElementImpl.h"
#include "SVGListener.h"
#include "SVGUseElementImpl.h"
//#include "SvgEventHandler.h"
#include "SVGMediaAnimationElementImpl.h"
#include <AknUtils.h>
#include <avkon.hrh>
#include "SvgBitmapFontProvider.h"
#include "SvgStopElementImpl.h"
#include "SVGGradientElementImpl.h"
#include <utf.h>

#define  SvgUnitPixels  1
#define  SvgUnitPercent 2
#define  KBackgroundColor 0xffffffff
_LIT(OFFSET, "offset");
_LIT(ZEROVALUE, "0");

// --------------------------------------------------------------------------
// EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewL( CFbsBitmap* aFrameBuffer,
// ---------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewL( CFbsBitmap* aFrameBuffer,
                                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec )
    {
    CSvgEngineInterfaceImpl* self    = new ( ELeave ) CSvgEngineInterfaceImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aFrameBuffer, aReqObserver,aFontSpec );
    CleanupStack::Pop();

    return self;
    }

// --------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewL( CFbsBitmap* aFrameBuffer,

                                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec ,SVGRendererId aRendererType)

    {

    CSvgEngineInterfaceImpl* self    = new ( ELeave ) CSvgEngineInterfaceImpl;

    CleanupStack::PushL( self );

    self->ConstructL( aFrameBuffer, aReqObserver,aFontSpec,aRendererType );

    CleanupStack::Pop();



    return self;

    }
// EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewLC( CFbsBitmap* aFrameBuffer,
// ---------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewLC( CFbsBitmap* aFrameBuffer,
                                                        MSvgRequestObserver* aReqObserver,TFontSpec& aFontSpec )
    {
    CSvgEngineInterfaceImpl* self    = new ( ELeave ) CSvgEngineInterfaceImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aFrameBuffer, aReqObserver,aFontSpec );

    return self;
    }

// --------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl* CSvgEngineInterfaceImpl::NewLC( CFbsBitmap* aFrameBuffer,

                                                        MSvgRequestObserver* aReqObserver,TFontSpec& aFontSpec ,SVGRendererId aRendererType)

    {

    CSvgEngineInterfaceImpl* self    = new ( ELeave ) CSvgEngineInterfaceImpl;

    CleanupStack::PushL( self );

    self->ConstructL( aFrameBuffer, aReqObserver,aFontSpec,aRendererType );



    return self;

    }
// EXPORT_C CSvgEngineInterfaceImpl::CSvgEngineInterfaceImpl()
// ---------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl::CSvgEngineInterfaceImpl()
    {
    iIsLoadingRequest = EFalse;
    iMouseDownFlag = EFalse;
    iDrmRightsConsumptionEnabled = ETrue;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ConstructL( CFbsBitmap* aFrameBuffer,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ConstructL( CFbsBitmap* aFrameBuffer,
                                     MSvgRequestObserver* aReqObserver,TFontSpec& aFontSpec )
    {
    iSvgBitmapFontProvider = CSvgBitmapFontProvider::NewL( aFontSpec );
    iSvgEngine = CSvgEngineImpl::NewL( aFrameBuffer, aReqObserver,aFontSpec, iSvgBitmapFontProvider );
    iSvgEnginePointers.Append(iSvgEngine);

    SetDRMMode( ETrue );
    SetThumbNailMode( EFalse );

    iFileIsLoaded = EFalse;

    iTotalRotation= 0;
    iSvgError = CSvgErrorImpl::NewL();
    
//	Not needed since no more VGR. Causes issues when multiple 
//  contexts are created by multiple engines...	
//  CustomOption(ETrue);	
    }

//NGA :: Renderer selector included

// --------------------------------------------------------------------------

// EXPORT_C void CSvgEngineInterfaceImpl::ConstructL( CFbsBitmap* aFrameBuffer,

// ---------------------------------------------------------------------------

EXPORT_C void CSvgEngineInterfaceImpl::ConstructL( CFbsBitmap* aFrameBuffer,

                                     MSvgRequestObserver* aReqObserver,TFontSpec& aFontSpec,SVGRendererId aRendererType )

    {

    iSvgBitmapFontProvider = CSvgBitmapFontProvider::NewL( aFontSpec );

    iSvgEngine = CSvgEngineImpl::NewL( aFrameBuffer, aReqObserver,aFontSpec, iSvgBitmapFontProvider ,aRendererType);

    iSvgEnginePointers.Append(iSvgEngine);



    SetDRMMode( ETrue );

    SetThumbNailMode( EFalse );



    iFileIsLoaded = EFalse;



    iTotalRotation= 0;

    iSvgError = CSvgErrorImpl::NewL();

    

//	Not needed since no more VGR. Causes issues when multiple 

//  contexts are created by multiple engines...	

//  CustomOption(ETrue);	

    }
// EXPORT_C void CSvgEngineInterfaceImpl::ConstructL()
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ConstructL()
    {
    iSvgEngine = NULL;

    iFileIsLoaded = EFalse;
    iTotalRotation= 0;
    iSvgError = CSvgErrorImpl::NewL();

    //Added to populated the font spec with variant default font
    const TInt KApacFontId = EApacPlain16;
    const TInt KLatintFontId = ELatinBold12;
    TInt fontId = KLatintFontId;

    switch ( AknLayoutUtils::Variant() )
        {
        case EApacVariant:
            {
            fontId = KApacFontId;
            }
            break;
        case EEuropeanVariant:
        default:
            break;
        }

    const CFont* font = AknLayoutUtils::FontFromId( fontId );
    TFontSpec spec = font->FontSpecInTwips();
	iSvgBitmapFontProvider = CSvgBitmapFontProvider::NewL( spec );

//	Not needed since no more VGR. Causes issues when multiple 
//  contexts are created by multiple engines...		
//  CustomOption(ETrue);

    }

EXPORT_C void CSvgEngineInterfaceImpl::ConstructL(TFontSpec& aFontSpec)
    {
    iSvgEngine = NULL;
    iFileIsLoaded = EFalse;
    iTotalRotation= 0;
    iSvgError = CSvgErrorImpl::NewL();
	iSvgBitmapFontProvider = CSvgBitmapFontProvider::NewL( aFontSpec );
    }
// --------------------------------------------------------------------------
// EXPORT_C CSvgEngineInterfaceImpl::~CSvgEngineInterfaceImpl()
// ---------------------------------------------------------------------------
EXPORT_C CSvgEngineInterfaceImpl::~CSvgEngineInterfaceImpl()
    {
    iSvgDocumentPointers.ResetAndDestroy();
    iSvgDocumentPointers.Close();

	iSvgEnginePointers.ResetAndDestroy();
	iSvgEnginePointers.Close();

    delete iSvgBitmapFontProvider ;
    //delete iTimer;
    if(iSvgError)
        {
    delete iSvgError;
        iSvgError = NULL;
        }

    iSvgLoadingListeners.Close();
    iSvgMouseEnteredElements.Close();

    }

//DEPRECATED API
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Destroy()
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Destroy()
    {
    	DestroyDocument(iSvgLoadedDocument);
    	iSvgLoadedDocument = NULL;
    }

// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::PostLoadProcessing( TInt aDocumentHandle )
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::PostLoadProcessing( TInt /* aDocumentHandle */ )
    {
    }

// ---------------------------------------------------------------------------
//Deprecated for platform security
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::Load( const TDesC& aFileName )
    {
    TInt documentHandle = 0;
    iTotalRotation = 0;
    iSvgError->SetErrorCode( ESvgNoError );

    // Destroy current loaded document.
    DestroyDocument(iSvgLoadedDocument);

    iIsLoadingRequest = ETrue;
    TRAPD( error, iSvgEngine->iSvgNames->AppendL(aFileName));
        
    if( error != KErrNone )
        {
        _LIT( KErrorMsg, "Failed to Append filename" );
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, error,KErrorMsg, aFileName );
         return iSvgError;
        }
    
    PrepareDom( aFileName, documentHandle );
    iIsLoadingRequest = EFalse;

    if ( iSvgError->HasError() && !iSvgError->IsWarning() )
    {
        return iSvgError;
    }

    PostLoadProcessing( documentHandle );

    iSvgLoadedDocument = (CSvgDocumentImpl*)documentHandle;
    return iSvgError;
}

// ---------------------------------------------------------------------------
// Modified Load function to take RFile aHandle.
// ---------------------------------------------------------------------------
EXPORT_C MSvgError*  CSvgEngineInterfaceImpl::Load( RFile& aFileHandle)
    {
    TInt documentHandle = 0;
    iTotalRotation = 0;
    iSvgError->SetErrorCode( ESvgNoError );

    // Destroy current loaded document.
    DestroyDocument(iSvgLoadedDocument);
    
    TFileName fileName;
    aFileHandle.Name(fileName);
    iSvgEngine->iSvgNames->AppendL(fileName);
    iIsLoadingRequest = ETrue;
    PrepareDom( aFileHandle, documentHandle );
    iIsLoadingRequest = EFalse;

    if ( iSvgError->HasError() && !iSvgError->IsWarning() )
    {
        return iSvgError;
    }

    PostLoadProcessing( documentHandle );
	iSvgLoadedDocument = (CSvgDocumentImpl*)documentHandle;
    return iSvgError;
    }

// ---------------------------------------------------------------------------
// Modified Load function which in turn calls the generic LoadL function
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::Load( const TDesC8& aByteData )
    {
    TInt documentHandle = 0;
    iTotalRotation = 0;
    iSvgError->SetErrorCode( ESvgNoError );

    // Destroy current loaded document.
    DestroyDocument(iSvgLoadedDocument);

    iIsLoadingRequest = ETrue;
    PrepareDom( aByteData, documentHandle );
    iIsLoadingRequest = EFalse;

    if ( iSvgError->HasError() && !iSvgError->IsWarning() )
    {
        return iSvgError;
    }

    PostLoadProcessing( documentHandle );
	iSvgLoadedDocument = (CSvgDocumentImpl*)documentHandle;
    return iSvgError;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Start( CSvgEngineImpl* aEngine
//                                                  , TBool /*aIsMainThread*/ )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Start( CSvgEngineImpl* aEngine,
                                              TBool /*aIsMainThread*/ )
    {

    if ( ChooseEngine((TInt)aEngine) )
        {
        iSvgEngine->SetMediaTime(0);
        iSvgEngine->ResetTimer();
        iSvgEngine->StartEngine();
        }
    }


// -----------------------------------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Start( CSvgEngineImpl* aEngine,TBool /*aIsMainThread*/ )
// -----------------------------------------------------------------------------------------------------

EXPORT_C void CSvgEngineInterfaceImpl::Start( MSvgError*& aError,
                                              CSvgEngineImpl* aEngine
                                            )
    {
    
    if ( ChooseEngine((TInt)aEngine) )
        {
            iSvgEngine->SetMediaTime(0);
            iSvgEngine->ResetTimer();
            iSvgEngine->StartEngine(iSvgError); 
        }
        
        aError = iSvgError;
    }
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Pause( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Pause( CSvgEngineImpl* aEngine )
    {
    	if ( ChooseEngine((TInt)aEngine) )
    	{
    	    iSvgEngine->SetSVGEngineState(ESVGEnginePaused);
    	}
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Stop( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Stop( CSvgEngineImpl* aEngine )
    {
    	if ( ChooseEngine((TInt)aEngine) )
        {
            iSvgEngine->SetSVGEngineState(ESVGEngineStopped);

            if (iSvgEngine->Document() && iSvgEngine->Document()->IsAnimationFile())
            {
            // restore some properties that changed during animation
 //           RestoreAnimStyleProperties();
            }
        }
    }

// Store properties that were changed during animation
//
// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::RestoreAnimStyleProperties()
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::RestoreAnimStyleProperties( TInt aEngine )
    {
    if ( ChooseEngine(aEngine) && iSvgEngine->Document())
        {
        RPointerArray<CSvgElementImpl> all_elements;
        iSvgEngine->Document()->FindAllElements(
            (CSvgElementImpl* )iSvgEngine->Document()->RootElement(), -1, all_elements );

		TInt allEleCnt = all_elements.Count();
        for (TInt i=0; i < allEleCnt; i++ )
            {
            // make sure that this is an animation element.
            if( all_elements[i]->IsAnimatedElement() )
                {
                CSvgAnimationBase*
                    lAnimationBasePtr = (CSvgAnimationBase*)all_elements[i];
                if ( !lAnimationBasePtr->IsFillFreeze() )
                    {
                    TRAPD(err, lAnimationBasePtr->ResetAttrValueL());    
                        if ( err != KErrNone )
                        {
                         #ifdef _DEBUG
                         RDebug::Printf("CSvgEngineInterfaceImpl::RestoreAnimStylePropertiesL() failed");
                         #endif
                        }
                    }
                
                
                }
            }
        all_elements.Close();
        }
    }



// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Resume( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Resume( CSvgEngineImpl* aEngine )
    {
    	if ( ChooseEngine((TInt)aEngine) )
    	{
            iSvgEngine->SetSVGEngineState(ESVGEngineRunning);
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C TUint32 CSvgEngineInterfaceImpl::Duration()
// ---------------------------------------------------------------------------
EXPORT_C TUint32 CSvgEngineInterfaceImpl::Duration( TInt aEngine )
    {

    	if ( ChooseEngine(aEngine) )
    	{
    		return ( iSvgEngine->Duration() );
    	}
    	else
    	{
    		return 0;
   		}
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::OriginalView()
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::OriginalView( TInt aEngine )
    {
    MSvgTransformList*  trList;
    iTotalRotation= 0;
    if ( ChooseEngine(aEngine) && iSvgEngine->Document() )
        {
        CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();
        if ( lRoot && lRoot->GetZoomAndPan() == ESvgZoomAndPanMagnify )
            {
            lRoot->GetTransform( trList );
            trList->ReplaceItem( TGfxAffineTransform(), 0 );
            }
        }
    }

//
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetRenderQuality( TRenderingQuality aQualityLevel )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetRenderQuality( TRenderingQuality aQualityLevel, TInt aEngine )
    {
    
    if( ChooseEngine(aEngine) )
    {
    	iSvgEngine->SetRenderingQuality(aQualityLevel);
    }

    }


// --------------------------------------------------------------------------
// EXPORT_C TRenderingQuality CSvgEngineInterfaceImpl::GetRenderQuality()
// ---------------------------------------------------------------------------
EXPORT_C TRenderingQuality CSvgEngineInterfaceImpl::GetRenderQuality()
    {
    return ( KLowAA );
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetAnimFrameDuration( TUint aAFDur )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetAnimFrameDuration( TUint aAFDur, TInt aEngine )
    {
    	if (ChooseEngine(aEngine))
    	{
    		iSvgEngine->ChangeDuration( aAFDur );
    	}
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Redraw( TBool aIsMainThread )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Redraw( TBool /*aIsMainThread*/, TInt aEngine )
    {
    if ( !ChooseEngine(aEngine) )
        {
        return;
        }

    CSvgDocumentImpl* document = iSvgEngine->Document();
    if ( !document )
        {
        return;
        }

    // This situation occurs when parsing is done in a separate thread then the draw thread
    // Wait for pruning to complete or invalid element pointer may occur during drawing
    if ( document->iIsPruningTree )
        {
        TInt waitCount = 0;
        while ( waitCount++ < 100 && document->iIsPruningTree )
            {
            User::After( 10000 ); // 10 millseconds
            }

        // Cannot wait any longer, prevent being stuck here
        if ( document->iIsPruningTree )
            {
            return;
            }
        }

    TRAPD(redrawError, iSvgEngine->RedrawL() );
    if( redrawError != KErrNone )
        {
        document->iIsRendering = EFalse;
        }

    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SwitchDebugInfo( TInt aEngine )
// ----------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SwitchDebugInfo( TInt aEngine )
    {
    	if (ChooseEngine( aEngine ))
    	{

    	}
    }


//*****************************
// UI event
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::MouseDown( TInt aX, TInt aY )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::MouseDown( TInt aX, TInt aY, TInt aEngine )
    {

    if ( iMouseDownFlag || !ChooseEngine(aEngine) )
    {
    	return NULL;
    }

    iMouseDownFlag = ETrue;


    TSvgUiMouseEvent    uiev    ( ESvgEngineEventMouseDown, aX, aY );
    TRAPD( error, iSvgEngine->ProcessEventL( iSvgEngine->Document(), &uiev ) );
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KProcessEventFailed, "CSvgEngineInterfaceImpl::MouseDown: ProcessEventL error.");
        RDebug::Print(KProcessEventFailed);
        #endif //_DEBUG
        }
    // Process for MousePress
    TInt listenerCount = iSvgEngine->MouseListenerCount();

    if (iSvgEngine->Document())
    {
        listenerCount += iSvgEngine->Document()->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        return 0;
        }
    RPointerArray<CSvgElementImpl> viewableElements;
    TInt topClickedElement = 0;

    iSvgEngine->GetViewableElementsAtPoint(iSvgEngine->Document(),viewableElements, aX, aY );
    if ( viewableElements.Count() > 0 )
        {
        iSvgEngine->NotifyMousePressed( viewableElements, aX, aY );
        if ( iSvgEngine->Document() )
            {
            iSvgEngine->Document()->NotifyInternalMousePressed( viewableElements, aX, aY );
            }

        topClickedElement = (TInt)viewableElements[viewableElements.Count()-1];
        }
    viewableElements.Close();

    return topClickedElement;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::MouseUp( TInt aX, TInt aY )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::MouseUp( TInt aX, TInt aY, TInt aEngine )
    {
    iMouseDownFlag = EFalse;

    if (!ChooseEngine( aEngine ))
    {
    	return;
    }

    TSvgUiMouseEvent    uiev    ( ESvgEngineEventMouseUp, aX, aY );
    TRAPD( error, iSvgEngine->ProcessEventL( iSvgEngine->Document(), &uiev ) );
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KProcessEventFailed, "CSvgEngineInterfaceImpl::MouseUp: ProcessEventL error.");
        RDebug::Print(KProcessEventFailed);
        #endif //_DEBUG
        }

    // Process for MouseRelease
    TInt listenerCount = iSvgEngine->MouseListenerCount();

    if (iSvgEngine->Document())
    {
        listenerCount += iSvgEngine->Document()->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        return;
        }
    RPointerArray<CSvgElementImpl> viewableElements;
    iSvgEngine->GetViewableElementsAtPoint(iSvgEngine->Document(), viewableElements, aX, aY );
    if ( viewableElements.Count() > 0 )
        {
        iSvgEngine->NotifyMouseReleased( viewableElements, aX, aY );
        if ( iSvgEngine->Document() )
            {
            iSvgEngine->Document()->NotifyInternalMouseReleased( viewableElements, aX, aY );
            }
        }
    viewableElements.Close();
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::MouseMove( TInt aX, TInt aY )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::MouseMove( TInt aX, TInt aY, TInt aEngine )
    {

    if (!ChooseEngine( aEngine ))
    {
    	return;
    }
    RPointerArray<CSvgElementImpl> viewableElements;
    iSvgEngine->GetViewableElementsAtPoint(iSvgEngine->Document(), viewableElements, aX, aY );
    if( iSvgEngine->Document() && viewableElements.Count() )
		{
		iSvgEngine->Document()->SetCurFocusObject(viewableElements[viewableElements.Count()-1]);	
		}

    TSvgUiMouseEvent    uiev    ( ESvgEngineEventMouseMove, aX, aY );
    TRAPD( error, iSvgEngine->ProcessEventL( iSvgEngine->Document(), &uiev ) );
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KProcessEventFailed, "CSvgEngineInterfaceImpl::MouseMove: ProcessEventL error.");
        RDebug::Print(KProcessEventFailed);
        #endif //_DEBUG
        }

    // Process for Mouse enter/exit
    TInt listenerCount = iSvgEngine->MouseListenerCount();

    if (iSvgEngine->Document())
    {
        listenerCount += iSvgEngine->Document()->MouseInternalListenerCount();
    }
    if ( listenerCount == 0 )
        {
        viewableElements.Close();
        return;
        }
    // Get list of elements containing pointer (bounding-box)
    RPointerArray<CSvgElementImpl> enteringElements;
    RPointerArray<CSvgElementImpl> stillInsideElements;
    iSvgEngine->GetViewableElementsAtPoint(iSvgEngine->Document(), viewableElements, aX, aY );

    // check for mouse entering of elements
    TInt viewableEleCnt = viewableElements.Count();
    for ( TInt i = 0; i < viewableEleCnt; i++ )
        {
        // If an element is in the view-list but NOT in the entered-list
        // then, it's copied to the entered-list
        // otherwise, ignore (still inside)
        if ( iSvgMouseEnteredElements.Find( viewableElements[i] ) == KErrNotFound )
            {
            enteringElements.Append( viewableElements[i] );
            }
        else
            {
            stillInsideElements.Append( viewableElements[i] );
            }
        }

    // check for mouse exiting of elements
    RPointerArray<CSvgElementImpl> exitingElements;
    TInt svgMouseEnteredEleCnt = iSvgMouseEnteredElements.Count();
    for ( TInt i = 0; i < svgMouseEnteredEleCnt; i++ )
        {
        // If an element is in the entered-list but not the view-list
        // then, it's copied to the exiting list
        // otherwise, ignore (
        if ( viewableElements.Find( iSvgMouseEnteredElements[i] ) == KErrNotFound )
            {
            exitingElements.Append( iSvgMouseEnteredElements[i] );
            }
        }

    // Remove exiting elements from entered list
    TInt exitingEleCnt = exitingElements.Count();
    for ( TInt i = 0; i < exitingEleCnt; i++ )
        {
        TInt index = iSvgMouseEnteredElements.Find( exitingElements[i] );
        if ( index != KErrNotFound )
            {
            iSvgMouseEnteredElements.Remove( index );
            }
        }

    // Add entering elements to entered-list
    TInt enteringEleCnt = enteringElements.Count();
    for ( TInt i = 0; i < enteringEleCnt; i++ )
        {
        iSvgMouseEnteredElements.Append( enteringElements[i] );
        }

    // Notify exiting
    if ( exitingElements.Count() > 0 )
        {
        iSvgEngine->NotifyMouseExited( exitingElements, aX, aY );
        if ( iSvgEngine->Document() )
            {
            iSvgEngine->Document()->NotifyInternalMouseExited( exitingElements, aX, aY );
            }
        }

    // Notify entering
    if ( enteringElements.Count() > 0 )
        {
        iSvgEngine->NotifyMouseEntered( enteringElements, aX, aY );
        if ( iSvgEngine->Document() )
            {
            iSvgEngine->Document()->NotifyInternalMouseEntered( enteringElements, aX, aY );
            }
        }

    // Notify moved
    if ( stillInsideElements.Count() > 0 )
        {
        iSvgEngine->NotifyMouseMoved( stillInsideElements, aX, aY );
        if ( iSvgEngine->Document() )
            {
            iSvgEngine->Document()->NotifyInternalMouseMoved( stillInsideElements, aX, aY );
            }
        }

    exitingElements.Close();
    enteringElements.Close();
    stillInsideElements.Close();
    viewableElements.Close();
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::KeyPress( const TKeyEvent &aKeyEvent )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::KeyPress( const TKeyEvent &aKeyEvent, TInt aEngine )
    {

    if (!ChooseEngine( aEngine ))
    {
    	return;
    }

    TSvgUiKeyEvent  uiev    ( aKeyEvent.iCode );
    TRAPD( error, iSvgEngine->ProcessEventL( iSvgEngine->Document(), &uiev ) );
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KProcessEventFailed, "CSvgEngineInterfaceImpl::KeyPress: ProcessEventL error.");
        RDebug::Print(KProcessEventFailed);
        #endif //_DEBUG
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::SvgDocument()
// ---------------------------------------------------------------------------
EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::SvgDocument()
    {
    //this method is a bad idea and should be removed
    return iSvgLoadedDocument;
    }


//*****************************
//
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Zoom( TReal32 aScaleFactor )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Zoom( TReal32 aScaleFactor, TInt aEngine ) __SOFTFP
    {
    TGfxAffineTransform zT;
    MSvgTransformList*  trList = NULL;
    TFloatFixPt cx,cy;
    if( ChooseEngine( aEngine ) && iSvgEngine->Document() && iSvgEngine->GraphicsContext())
        {
        CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();
        if ( lRoot )
            {
            if ( lRoot->GetZoomAndPan() == ESvgZoomAndPanMagnify )
                {
                // Calculate screen center in svg element's coordinate system
                TSize               devSize = iSvgEngine->GraphicsContext()->DeviceBounds();

				TGfxRectangle2D devbound;
                lRoot->GetBBox( devbound );

				    cx = TFloatFixPt(devSize.iWidth/2);
				    cy = TFloatFixPt(devSize.iHeight/2);

                lRoot->GetTransform( trList );
                if( trList )
                    {
                    zT = trList->GetItem( 0 );
                    zT.UserZoom(aScaleFactor,cx,cy);
                    trList->ReplaceItem( zT, 0 );

                    lRoot->SetTransform(trList);
                    }


                }
            }
        }
    }


// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Pan( TInt aX, TInt aY )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Pan( TInt aX, TInt aY, TInt aEngine )

    {
    TInt32 PanX = aX;
    TInt32 PanY = aY;
    TGfxAffineTransform pT;
    MSvgTransformList*  trList = NULL;

    if( ChooseEngine( aEngine ) && iSvgEngine->Document() )
        {

        CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();
        if ( lRoot )
            {
            if ( lRoot->GetZoomAndPan() == ESvgZoomAndPanMagnify )
                {

                // Do panning
                lRoot->GetTransform( trList );
                if( trList )
                    {
                    pT = trList->GetItem( 0 );


                    pT.UserPan( PanX , PanY  );

                    trList->ReplaceItem( pT, 0 );

                    lRoot->SetTransform(trList);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Panning indicator
// ---------------------------------------------------------------------------

EXPORT_C TBool CSvgEngineInterfaceImpl::IsPanPossible( TInt aX, TInt aY, TInt aEngine )

    {
    if( !ChooseEngine( aEngine ) || (iSvgEngine->Document() == NULL) )
    {
        return EFalse;
    }

    CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();
    if ( lRoot == NULL)
        return EFalse;
    if ( lRoot->GetZoomAndPan() != ESvgZoomAndPanMagnify )
        return EFalse;

    // Get the clipping size
    TFloatFixPt  clipWidthFP, clipHeightFP;
    TInt    clipWidth, clipHeight;
    lRoot->GetAttributeFloat( KAtrWidth, clipWidthFP );
    lRoot->GetAttributeFloat( KAtrHeight, clipHeightFP );
    clipWidth = ( TInt ) clipWidthFP;
    clipHeight = ( TInt ) clipHeightFP;
    // Get bounding box of svg element
    TFloatFixPt          zero    ( 0 );
    TGfxRectangle2D bound;
    lRoot->GetBBox( bound );
    TInt32  xmax    = ( bound.iX + bound.iWidth );
    TInt32  ymax    = ( bound.iY + bound.iHeight );

    // check  them
    if ( xmax <= clipWidth && aX < 0 )
        aX = 0;
    if ( bound.iX >= zero && aX > 0 )
        aX = 0;
    if ( ymax <= clipHeight && aY < 0 )
        aY = 0;
    if ( bound.iY >= zero && aY > 0 )
        aY = 0;
    // Do panning
    if ( ( aY == 0 ) && ( aX == 0 ) )
        return EFalse;
    else
       return ETrue;

    }

// ---------------------------------------------------------------------------
// Panning indicator
// ---------------------------------------------------------------------------

EXPORT_C void CSvgEngineInterfaceImpl::IsPanPossibleFourWay
    ( TBool& left, TBool& right, TBool& up, TBool& down, TInt aEngine )

    {

    left = right = up = down = EFalse;

    if( !ChooseEngine( aEngine ) || (iSvgEngine->Document() == NULL) )
    {
        return ;
    }

    CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();
    if ( lRoot == NULL)
        return ;
    if ( lRoot->GetZoomAndPan() != ESvgZoomAndPanMagnify )
        return ;

    // Get the clipping size
    TFloatFixPt  clipWidthFP, clipHeightFP;
    TInt    clipWidth, clipHeight;
    lRoot->GetAttributeFloat( KAtrWidth, clipWidthFP );
    lRoot->GetAttributeFloat( KAtrHeight, clipHeightFP );
    clipWidth = ( TInt ) clipWidthFP;
    clipHeight = ( TInt ) clipHeightFP;
    //the svg element has no width or height no reason to pan.
    if (clipWidth <= 0 || clipHeight <= 0)
    {
    	return;
    }
    // Get bounding box of svg element
    TFloatFixPt          zero    ( 0 );
    TGfxRectangle2D bound;
    lRoot->GetBBox( bound );
    //fix if no width or heigth is there.
    if((TInt32)bound.iWidth <=0 ||(TInt32) bound.iHeight <=0 )
        return;
    TInt32  xmax    = ( bound.iX + bound.iWidth );
    TInt32  ymax    = ( bound.iY + bound.iHeight );

    // check the panning in four directions now.
    TInt aX,aY;
    // Check in Left direction
    aX = 1; aY = 0;
    //Coverity fixes
    if ( bound.iX >= zero )
        aX = 0;
 
    // Do panning
    left = ((aY)||(aX));

    // Check in Right direction
    aX = -1; aY = 0;
    if ( xmax <= clipWidth )
        aX = 0;
    
    // Do panning
    right = ((aY)||(aX));

    // Check in Up direction
    aX = 0; aY = 1;
    if ( bound.iY >= zero )
        aY = 0;
    
    // Do panning
    up = ((aY)||(aX));

    // Check in down direction
    aX = 0; aY = -1;
    if ( ymax <= clipHeight )
        aY = 0;
    
    // Do panning
    down = ((aY)||(aX));

    }
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::Rotate( TReal32 aAngle,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::Rotate( TReal32 aAngle,
                                               TInt aX,
                                               TInt aY,
                                               TInt aEngine ) __SOFTFP

    {
    TGfxAffineTransform rT;
    MSvgTransformList*  trList = NULL;

    if( ChooseEngine( aEngine ) && iSvgEngine->Document() )
        {
        CSvgSvgElementImpl* lRoot   = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();

        if ( lRoot )
            {
            if ( lRoot->GetZoomAndPan() == ESvgZoomAndPanMagnify )
                {
                lRoot->GetTransform( trList );
                if( trList )
                    {
                    rT = trList->GetItem( 0 );

                    rT.UserRotate( aAngle, aX, aY );
                    trList->ReplaceItem( rT, 0 );
                    iTotalRotation +=  aAngle;
                    }
                lRoot->SetTransform(trList); //added
                }
            }
        }
    }

// ==========================================================================
// ContentDimensions integer value version
//1)  Return the size of the <svg> width/height if given in non-percentage.
//
//2)  If <svg> width/height are given as percentages:
//    A) If viewbox attribute is specified, return the viewbox width/height times the <svg> width/height percentages.
//    B) Return the content bounding-box size times times the <svg> width/height percentages.
// ==========================================================================
EXPORT_C TSize CSvgEngineInterfaceImpl::ContentDimensions( TInt aEngine )
    {
    if ( !ChooseEngine( aEngine ) || !iSvgEngine->Document() )
        {
        return TSize( 0, 0 );
        }

    CSvgSvgElementImpl* svgElement = (CSvgSvgElementImpl* )iSvgEngine->Document()->RootElement();

    if ( !svgElement )
        {
        return TSize( 0, 0 );
        }

    // width/height are given as user-units
        if ( !( svgElement->iWidthInPercentage ) && !(svgElement->iHeightInPercentage))
        {
        return TSize( (TInt)svgElement->iWidthInUserCoordinate,
                      (TInt)svgElement->iHeightInUserCoordinate );
        }

    // width/height are given as percentages
    TInt percentWidth = (TInt)svgElement->iWidthInUserCoordinate;
    TInt percentHeight = (TInt)svgElement->iHeightInUserCoordinate;
    // Multiply percentages with viewbox if available
    TGfxRectangle2D viewBox;
    if ( svgElement->GetViewBox( viewBox ) )
        {
            TInt tempWidth = 0, tempHeight = 0;
            
            if( svgElement->iWidthInPercentage )
            {
                tempWidth  = ((TReal32)viewBox.iWidth)  * percentWidth / 100.0;    
            }
            else
            {
                tempWidth = viewBox.iWidth;
            }
        
            if( svgElement->iHeightInPercentage )
            {
                tempHeight = ((TReal32)viewBox.iHeight) * percentHeight / 100.0;    
            }
            else
            {
                tempHeight = viewBox.iHeight;
            }
            return TSize(tempWidth, tempHeight);
        }
    // return the <svg> bounding box
    else
        {
        TSize bbSize = GetSvgBoundingBox().Size();
            if( svgElement->iWidthInPercentage )
            {
                bbSize.iWidth  = ((TReal32)bbSize.iWidth)  * percentWidth / 100.0;    
            }
        
            if( svgElement->iHeightInPercentage )
            {
                bbSize.iHeight = ((TReal32)bbSize.iHeight) * percentHeight / 100.0;    
            }
        return bbSize;
        }
    }

// ==========================================================================
// ContentDimensions floating-point value version
//1)  Return the size of the <svg> width/height if given in non-percentage.
//
//2)  If <svg> width/height are given as percentages:
//    A) If viewbox attribute is specified, return the viewbox width/height times the <svg> width/height percentages.
//    B) Return the content bounding-box size times times the <svg> width/height percentages.
// ==========================================================================
EXPORT_C void CSvgEngineInterfaceImpl::ContentDimensions( TReal32& aWidth, TReal32& aHeight, TInt aEngine ) __SOFTFP
    {
    aWidth  = 0.0;
    aHeight = 0.0;
    if ( !ChooseEngine( aEngine ) || !iSvgEngine->Document() )
        {
        return;
        }

    CSvgSvgElementImpl* svgElement = (CSvgSvgElementImpl* )iSvgEngine->Document()->RootElement();

    if ( !svgElement )
        {
        return;
        }

    // width/height are given as user-units
        if ( !( svgElement->iWidthInPercentage ) && !( svgElement->iHeightInPercentage) )
        {
        #ifdef _DEBUG
        RDebug::Printf("content size: %.4fx%.4f",
                      svgElement->iWidthInUserCoordinate,
                      svgElement->iHeightInUserCoordinate );

        #endif
        aWidth  = svgElement->iWidthInUserCoordinate;
        aHeight = svgElement->iHeightInUserCoordinate;
        return;
        }

    // width/height are given as percentages
    TReal32 percentWidth  = svgElement->iWidthInUserCoordinate;
    TReal32 percentHeight = svgElement->iHeightInUserCoordinate;

    // Multiply percentages with viewbox if available
    TGfxRectangle2D viewBox;
    if ( svgElement->GetViewBox( viewBox ) )
        {
        #ifdef _DEBUG
        RDebug::Printf("viewbox is defined: %.4fx%.4f",
                      ((TReal32)viewBox.iWidth) * percentWidth / 100.0,
                      ((TReal32)viewBox.iHeight) * percentHeight / 100.0 );

        #endif
            if( svgElement->iWidthInPercentage )
            {
        aWidth  = ((TReal32)viewBox.iWidth)  * percentWidth / 100.0;
            }
            else
            {
                aWidth = viewBox.iWidth;
            }
        
            if( svgElement->iHeightInPercentage )
            {
        aHeight = ((TReal32)viewBox.iHeight) * percentHeight / 100.0;
            }
            else
            {
                aHeight = viewBox.iHeight;
            }
        return;
        }
    // return the <svg> bounding box
    else
        {
        CSvgSvgElementImpl* lRoot = ( CSvgSvgElementImpl* )iSvgEngine->Document()->RootElement();
        if( lRoot )
            {
            TReal32 x, y;
            GetElementBoundingbox( lRoot, x, y, aWidth, aHeight );
                if(svgElement->iWidthInPercentage )
                {
            aWidth  = aWidth  * percentWidth / 100.0;
                }
                if(svgElement->iHeightInPercentage )
                {
            aHeight = aHeight * percentHeight / 100.0;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C TSize CSvgEngineInterfaceImpl::ContentDimensions()
// ---------------------------------------------------------------------------
EXPORT_C TSize CSvgEngineInterfaceImpl::ContentDimensionsInPercentage( TInt aEngine )
    {
    TSize lSize;

    if( ChooseEngine( aEngine ) && (iSvgEngine->Document() != NULL) )
        {
        CSvgSvgElementImpl* rootElement = ( CSvgSvgElementImpl* )
                                      iSvgEngine->Document()->RootElement();

        if ( rootElement != NULL )
            {
             if(rootElement->iWidthInPercentage)
                {
                lSize.iWidth = ( TFloatFixPt ) ( rootElement->iWidthInUserCoordinate );
             }
             else
             {  
                lSize.iWidth = -1;
             }
             if(rootElement->iHeightInPercentage)
             {
                lSize.iHeight = ( TFloatFixPt ) ( rootElement->iHeightInUserCoordinate );
                }
             else
             {
                lSize.iHeight = -1;
             }
            }

        }
    return lSize;
    }
// --------------------------------------------------------------------------
// EXPORT_C TRect CSvgEngineInterfaceImpl::FocusNext()
// ---------------------------------------------------------------------------
EXPORT_C TRect CSvgEngineInterfaceImpl::FocusNext()
    {
    if(iSvgEngine->Document())
    iSvgEngine->Document()->IncCurFocusIndex();
    return (GetFocusBbox(ETrue));
    }

// --------------------------------------------------------------------------
// EXPORT_C TRect CSvgEngineInterfaceImpl::FocusPrevious()
// ---------------------------------------------------------------------------
EXPORT_C TRect CSvgEngineInterfaceImpl::FocusPrevious()
    {
    if(iSvgEngine->Document())
    iSvgEngine->Document()->DecCurFocusIndex();
    return (GetFocusBbox(EFalse));
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ActivateObjectInFocus()
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ActivateObjectInFocus()
    {
    TGfxRectangle2D lBbox;

    if( iSvgEngine->Document() && iSvgEngine->Document()->GetCurFocusObject() )
        {
        iSvgEngine->Document()->GetCurFocusObject()->GetBBox( lBbox );
        // Send mouse down and mouse up events to the engine
        // This will send message back to the application to handle the activation
        MouseDown( lBbox.CenterX(), lBbox.CenterY() );
        MouseUp( lBbox.CenterX(), lBbox.CenterY() );
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C TInt32 CSvgEngineInterfaceImpl::MediaTime()
// ---------------------------------------------------------------------------
EXPORT_C TInt32 CSvgEngineInterfaceImpl::MediaTime( TInt aEngine )
    {
    	if (ChooseEngine( aEngine ))
    	{
    		return iSvgEngine->CurrentTIme();
    	}

    	return 0;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetMediaTime(TInt32 aTime)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetMediaTime(TInt32 aTime, TInt aEngine )
    {

    // Time given is in milliseconds whereas the time needed by timer is microseconds
    // Resume(aTime * 1000);
    	if ( ChooseEngine( aEngine ) )
    	{
    		iSvgEngine->SetMediaTime(aTime* 1000);
    	}
    }

// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::ResetFocusIndex(TInt32 aIndex)
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::ResetFocusIndex(TInt32 /*aIndex*/)
    {
    //iSvgEngine->Document()->SetCurIndex(TInt32 aVal); aIndex;
    }


// --------------------------------------------------------------------------
// TRect CSvgEngineInterfaceImpl::GetFocusBbox( TBool aNextObj )
// ---------------------------------------------------------------------------
TRect CSvgEngineInterfaceImpl::GetFocusBbox( TBool aNextObj, TInt aEngine )
    {
    TGfxRectangle2D lBbox;
    TInt32 lNewIndex;

    if( ChooseEngine( aEngine ) && iSvgEngine->Document() )
        {
        // Obtain the current focussable object starting with the current index
        
        // The GetEventReceiverElement function takes in parameters as the 
        //current focus objects index and whether we are going next or previous
        // and the mask of the kind of input that we are accepting..
        // It returns the value of the new index through the lNewIndex.
        iSvgEngine->Document()->SetCurFocusObject ( 
         						iSvgEngine->Document()->GetEventReceiverElement(
         						 iSvgEngine->Document()->GetCurFocusIndex(),
         						  aNextObj, KSvgEventMaskExternalUI,
         						   lNewIndex ));
        // Update the index to where the focussable object was found
        iSvgEngine->Document()->SetCurFocusIndex(lNewIndex);

        if( iSvgEngine->Document()->GetCurFocusObject() )
            {
            iSvgEngine->Document()->GetCurFocusObject()->GetBBox(lBbox);

            // Send a mouse move event to the engine
            MouseMove( lBbox.CenterX(), lBbox.CenterY() );
            }
        }

    return TRect( lBbox.iX, lBbox.iY, lBbox.iX + lBbox.iWidth, lBbox.iY + lBbox.iHeight );
    }


// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::Resume(TInt32 aTime)
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::Resume(TInt32 aTime, TInt aEngine )
    {
    	if ( ChooseEngine( aEngine ) )
    	{
    		iSvgEngine->Resume(aTime);
    	}
    }
// --------------------------------------------------------------------------
// EXPORT_C TSize CSvgEngineInterfaceImpl::Size()
// ---------------------------------------------------------------------------
EXPORT_C TSize CSvgEngineInterfaceImpl::Size( TInt aEngine )
    {
    TSize lSize(0,0);

    if( ChooseEngine( aEngine ) )
        {
        CSvgDocumentImpl* lSvgDocument = iSvgEngine->Document();
        if( lSvgDocument )
            {
            CSvgSvgElementImpl* lSvgRootElement = (CSvgSvgElementImpl *)lSvgDocument->RootElement();
            if( lSvgRootElement )
                {
                lSize.iWidth = (TInt)lSvgRootElement->Width();
                lSize.iHeight = (TInt)lSvgRootElement->Height();
                }
            }
        }
    return lSize;
    }
// --------------------------------------------------------------------------
// EXPORT_C TPoint CSvgEngineInterfaceImpl::Position()
// ---------------------------------------------------------------------------
EXPORT_C TPoint CSvgEngineInterfaceImpl::Position( TInt aEngine )
    {
    TPoint lPoint(0,0);

    if(ChooseEngine( aEngine ))
        {
        CSvgDocumentImpl* lSvgDocument = iSvgEngine->Document();
        if( lSvgDocument )
            {
            CSvgSvgElementImpl* lSvgRootElement = (CSvgSvgElementImpl *)lSvgDocument->RootElement();
            if( lSvgRootElement )
                {
                lPoint.iX = (TInt)lSvgRootElement->X();
                lPoint.iY = (TInt)lSvgRootElement->Y();
                }
            }
        }
    return lPoint;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::GenerateMask(CFbsBitmap* aMask)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::GenerateMask(CFbsBitmap* aMask, TInt aEngine )
    {
        if ( ChooseEngine( aEngine ) )
        {
            iSvgEngine->GenerateMask(aMask);
        }
    }

EXPORT_C void CSvgEngineInterfaceImpl::GenerateMask(CSvgtBitmap* aMask, TInt aEngine )
    {
        if ( ChooseEngine( aEngine ) )
        {
            iSvgEngine->GenerateMask(aMask);
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetBackgroundColor(TUint32 aRGBA8888Color, CSvgEngineImpl* aEngine)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetBackgroundColor(TUint32 aRGBA8888Color, CSvgEngineImpl* aEngine)
    {
    	if (ChooseEngine( (TInt)aEngine ))
    	{
        	iSvgEngine->SetBackgroundColor(aRGBA8888Color);
        }
    }


// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::CurrentState()
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::CurrentState( TInt aEngine )
    {
        if (ChooseEngine(aEngine))
        {
            return (TInt) iSvgEngine->SVGEngineState();
        }

    return 0;
    }



// --------------------------------------------------------------------------
// EXPORT_C TPoint CSvgEngineInterfaceImpl::PanPosition()
// ---------------------------------------------------------------------------
EXPORT_C TPoint CSvgEngineInterfaceImpl::PanPosition( TInt aEngine )
    {
    TPoint lPoint;

    if( ChooseEngine(aEngine) && iSvgEngine->Document() )
        {
        CSvgSvgElementImpl* lRoot = ( CSvgSvgElementImpl* )
                                        iSvgEngine->Document()->RootElement();
        if ( lRoot )
            {
            MSvgTransformList*  trList = NULL;

            lRoot->GetTransform( trList );

            if( trList )
                {
                TGfxAffineTransform pT = trList->GetItem( 0 );

                lPoint.iX = (TInt) pT.TranslateX();
                lPoint.iY = (TInt) pT.TranslateY();
                }
            }
        }

    return lPoint;
    }



// --------------------------------------------------------------------------
// EXPORT_C MSvgError*  CSvgEngineInterfaceImpl::Replay()
// ---------------------------------------------------------------------------
EXPORT_C MSvgError*  CSvgEngineInterfaceImpl::Replay( TInt aEngine )
    {

    iTotalRotation = 0;

    if( ChooseEngine(aEngine) )
    {
        iSvgEngine->SetMediaTime( 0 );
        return NULL;
    }
    return iSvgError;
    }

/*SVGT DOM API Caching APIs*/
// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom(const TDesC& aFileName, TInt& aHandle)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom(const TDesC& aFileName, TInt& aHandle, TInt aEngine)
{
    // Create session to open file
    RFs session;
    CSvgDocumentImpl::OpenSession( session, *iSvgError );
    if ( iSvgError->HasError() )
    {
        return iSvgError;
    }

    // Open file handle
    RFile fileHandle;
    TInt openError = fileHandle.Open( session, aFileName, EFileRead );
    if ( openError != KErrNone )
    {
        _LIT(msg, "Fail to open file for reading: ");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, openError,
                                        msg, aFileName );
    }
    else
    {
        PrepareDom( fileHandle, aHandle, aEngine );
    }
    session.Close();
    return iSvgError;
}

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom( RFile& aFileHandle, TInt& aHandle )
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom( RFile& aFileHandle, TInt& aHandle, TInt aEngine )
	{
  	// reset to start of file
    TInt zero = 0;
    aFileHandle.Seek( ESeekStart, zero );

    TInt fileSize = 0;
    TInt sizeError = aFileHandle.Size( fileSize );
    if ( sizeError != KErrNone )
    	{
        _LIT( KErrorMsg, "Failed to Load Svg Content: Read File Size Error." );
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, sizeError,
                                        KErrorMsg, KNullDesC );
        return iSvgError;
    	}

    //It is the fix for ESMA-7PN9WU
    //file size should be more than 2 bytes
    if ( fileSize < 2)
        {
        _LIT( KErrorMsg, "File is empty" );
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, sizeError,
                                        KErrorMsg, KNullDesC );
        return iSvgError;
        }

	HBufC8* binaryBuffer=NULL;
  	
	binaryBuffer = HBufC8::New ( fileSize ); 
	 if (!binaryBuffer)
    	{
        _LIT( KErrorMsg, "Failed to Load Svg Content: Out of memory");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, KErrNoMemory,
                                        KErrorMsg, KNullDesC );
        return iSvgError;
    	}
	
	TPtr8 binaryBufferPtr = binaryBuffer->Des();
	TInt readError = aFileHandle.Read(binaryBufferPtr, fileSize);
	if ( readError != KErrNone )
    	{
        _LIT( KErrorMsg, "Failed to Load Svg Content: Read File Data Error");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, readError,
                                        KErrorMsg, KNullDesC );
        delete binaryBuffer;
        return iSvgError;
    	}
		
	// For files which are in UTF-16 or unicode format the first 2 bytes
	// will be FF and FE. In UTF-16 and unicode format every single character
	// is represented by two bytes. 
	TInt hichar = (CEditableText::EReversedByteOrderMark & 0xFF00)>>8;
	TInt lochar = CEditableText::EReversedByteOrderMark & 0x00FF;
	TInt bytesPerChar = 1;

		if(binaryBufferPtr[0] == hichar && binaryBufferPtr[1] == lochar)
		{
		bytesPerChar = 2;

		HBufC* dataBuffer=NULL;

		dataBuffer = HBufC::New(fileSize/bytesPerChar);
		if ( !dataBuffer)
    		{
        	_LIT( KErrorMsg, "Failed to Load Svg Content: Out of memory");
        	CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, KErrNoMemory,
                                        KErrorMsg, KNullDesC );
        	delete binaryBuffer;
        	return iSvgError;
    		}
	
		TPtr dataBufferPtr = dataBuffer->Des();
		
		// Skip two bytes and set the pointer to the next location
		// from where the actual data starts.
		dataBufferPtr.Set((TUint16*)binaryBufferPtr.Ptr()+1,
							fileSize/bytesPerChar-1, 
							fileSize/bytesPerChar-1);
			
		HBufC8* outputBuffer= NULL;
		outputBuffer=HBufC8::New(fileSize);
		if(!outputBuffer)
			{
			_LIT( KErrorMsg, "Failed to Load Svg Content: Out of memory");
        	CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, KErrNoMemory,
                                        KErrorMsg, KNullDesC );
        	delete dataBuffer;
        	delete binaryBuffer;
        	return iSvgError;
			}
		TPtr8 outputBufferptr=outputBuffer->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8(
            							outputBufferptr,   // Destination
            							dataBufferPtr );
		PrepareDom( *outputBuffer, aHandle, aEngine );		
		delete outputBuffer;
		delete dataBuffer;
		}
	else
		{
		PrepareDom( *binaryBuffer, aHandle, aEngine );
		}
    
    delete binaryBuffer;
  	return iSvgError;
	}

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom(const TDesC8& aByteData, TInt& aHandle)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::PrepareDom(const TDesC8& aByteData, TInt& aHandle, TInt aEngine )
    {
    aHandle = 0;

    // Clear Error
    iSvgError->SetErrorCode( ESvgNoError );

    if( !ChooseEngine(aEngine) )
        {
        _LIT(KEngineFailed, "SvgEngine Internal Failure: SvgEngineImpl not present.");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                        KEngineFailed, KNullDesC );
        return iSvgError;
        }

    CSvgDocumentImpl* newDocument = NULL;
    // Pass EFalse to document creation to indicate it has no parent
    TRAPD( error, newDocument = CSvgDocumentImpl::NewL( iSvgBitmapFontProvider, EFalse ) ); 

    // Check for error
    if(error != KErrNone)
        {
        _LIT(KEngineFailed, "Failed to Create Svg Document: Out of Memory");
        #ifdef _DEBUG
        RDebug::Print(KEngineFailed);
        #endif //_DEBUG
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, error,
                                        KEngineFailed, KNullDesC );
        return iSvgError;
        }

	iSvgDocumentPointers.Append(newDocument);

    // Set appropriate flags to documents
    newDocument->SetWasPrepared( !iIsLoadingRequest );
    newDocument->SetDRMMode( iDrmEnabled );
    newDocument->SetThumbNailMode( iIsThumbNailMode );
    newDocument->SetDRMRights( iDrmRightsConsumptionEnabled);

    CSvgDocumentImpl* currentDocument = iSvgEngine->Document();

    // Hook engine to document
    iSvgEngine->SetDocument( newDocument );
    newDocument->SetEngine( iSvgEngine );
    newDocument->SetLoadingListeners( &iSvgLoadingListeners );

    // Parse document
    newDocument->Load( aByteData, *iSvgError );

    // Put back the document that was attached to document,
    // since this is preparing the dom tree only
    if ( !iIsLoadingRequest )
        {
        iSvgEngine->SetDocument( currentDocument );
        }

    #ifdef _DEBUG
    if ( newDocument->GetError() && newDocument->GetError()->HasError() )
        {
        RDebug::Printf("Load warning: ");
        RDebug::RawPrint( newDocument->GetError()->Description() );
        }
    #endif

    iFileIsLoaded = !iSvgError->HasError() || iSvgError->IsWarning();

    // Parsing error or out of memory
    if ( !iFileIsLoaded )
        {
        DestroyDocument(newDocument);
        return iSvgError;
        }

    aHandle = (TInt)newDocument;

    return iSvgError;
    }

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::RenderDom(TInt aHandle, CFbsBitmap* aRenderBuffer)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::RenderDom(TInt aHandle, CFbsBitmap* aRenderBuffer, CFbsBitmap* aMaskBuffer)
    {
    UseDom( aHandle, aRenderBuffer, aMaskBuffer );

    if ( !iSvgError->HasError() || iSvgError->IsWarning() )
    {
        iSvgEngine->StartEngine();
        iFileIsLoaded = ETrue;
    }

    return iSvgError;
    }

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::DeleteDom(TInt aHandle)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::DeleteDom(TInt aHandle)
    {
    // Clear Error
    iSvgError->SetErrorCode( ESvgNoError );

    DestroyDocument((CSvgDocumentImpl*)aHandle);

    return iSvgError;
    }


// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetDRMMode(TBool aEnable)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetDRMMode(TBool aEnable, TInt aEngine)
    {
    iDrmEnabled = aEnable;
    if( ChooseEngine( aEngine )  && iSvgEngine->Document())
        iSvgEngine->Document()->SetDRMMode(aEnable);
    }

//---------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetDRMRights(TBool aEnable)
//---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetDRMRights(TBool aEnable)
    {
	iDrmRightsConsumptionEnabled = aEnable;
	if ( iSvgEngine && iSvgEngine->Document())
	    {
	    iSvgEngine->Document()->SetDRMRights(aEnable);
	    }
	    
	}

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::UseDom(TInt aHandle, CFbsBitmap* aRenderBuffer)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::UseDom(TInt aHandle, CFbsBitmap* aRenderBuffer, CFbsBitmap* aMask, TInt aEngine)
    {
    // Clear Error
    iSvgError->SetErrorCode( ESvgNoError );

    // Check for engine: Should not occur
    if( !ChooseEngine( aEngine ) )
        {
        _LIT(KEngineFailed, "SvgEngine Internal Failure: SvgEngineImpl not present.");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                        KEngineFailed, KNullDesC );
        return iSvgError;
        }

    // Check for valid DOM
    if( !IsDomCached( aHandle ) )
        {
        _LIT(KEngineFailed, "Invalid DOM, Use Prepare() to create DOM");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                        KEngineFailed, KNullDesC );
        return iSvgError;
        }

    iTotalRotation = 0;

    iFileIsLoaded = EFalse;

    // Switch GDI context
    TRAPD( gdiError, iSvgEngine->SetGdiContextL( aRenderBuffer, aMask ) );
    if ( gdiError != KErrNone )
        {
        if ( gdiError == KErrNoMemory )
            {
            _LIT(KEngineGDIFailed, "SVG Engine Set Gdi Context Failed");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, KErrNoMemory,
                                             KEngineGDIFailed, KNullDesC );
            return iSvgError;
            }
        _LIT(KEngineGDIFailed, "SVG Engine Set Gdi Context Failed");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                             KEngineGDIFailed, KNullDesC );
        return iSvgError;
        }

    CSvgDocumentImpl* document = (CSvgDocumentImpl*)aHandle;

    CSvgDocumentImpl* lExistDoc = iSvgEngine->Document();

    if (lExistDoc != document)
        {
        //this is a new document so swap it out...
        iSvgEngine->SetDocument( document );
        document->SetEngine( iSvgEngine );
        document->ReInitialize();
        }

	if (lExistDoc == iSvgLoadedDocument)
	{
		DestroyDocument(iSvgLoadedDocument);
		iSvgLoadedDocument = NULL;
	}

    // Check for thumbnail restrictions
    if ( !iSvgEngine->PassesThumbNailRestriction() )
        {
        _LIT( KThumbNailRestriction, "Frame buffer is larger than allowable size for thumbnail mode." );
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgThumbNailRestriction, KErrNone,
                                         KThumbNailRestriction, KNullDesC );
        return iSvgError;
        }

    iFileIsLoaded = ETrue;

    return iSvgError;
    }

// ---------------------------------------------------------------------------
// Is DOM cached
// ---------------------------------------------------------------------------
TBool CSvgEngineInterfaceImpl::IsDomCached(TInt aHandle)
    {
    return iSvgDocumentPointers.Find( (CSvgDocumentImpl*)aHandle ) != KErrNotFound;
    }

//---------------------------------------------------------------------------=
//Resize the view to the rendering buffer
//---------------------------------------------------------------------------=
EXPORT_C void CSvgEngineInterfaceImpl::SetSvgDimensionToFrameBuffer(TUint aWidth, TUint aHeight, TInt aEngine)
{

     if ( ChooseEngine(aEngine) )
        {
        TRAPD(error,iSvgEngine->SetSvgDimensionToFrameBufferL(aWidth, aHeight) );
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KSetFrameBufferFailed,
                "CSvgEngineInterfaceImpl::SetSvgDimensionToFrameBuffer: SetSvgDimensionToFrameBufferL error.");
            RDebug::Print(KSetFrameBufferFailed);
            #endif //_DEBUG
            }
        }


}

//---------------------------------------------------------------------------=
//Check if content is interactive
//---------------------------------------------------------------------------=

EXPORT_C TBool CSvgEngineInterfaceImpl::IsContentInteractive(CSvgDocumentImpl* aDocument, TInt aEngine )
{
    if ( ( aDocument == NULL ) || ( !ChooseEngine(aEngine) ) || ( aDocument->RootElement() == NULL ) )
        return EFalse;

    return aDocument->IsDocumentContentInteractive();
   /* RPointerArray<CSvgElementImpl> elements;
    aDocument->FindAllElements( (CSvgElementImpl* )aDocument->RootElement(),
                                 KSvgAElement, elements );
    if ( elements.Count() != 0 )
    {
        elements.Close();
        return ETrue;
    }
    elements.Close();

    if ( aDocument->iIsInteractive )
        return ETrue;
    else
        return EFalse;*/
}

//---------------------------------------------------------------------------=
//Copies a new path vector into the the given element's path
//---------------------------------------------------------------------------=

EXPORT_C void CSvgEngineInterfaceImpl::UpdatePath(TInt hPath, CSvgElementImpl* hElement)
{
    if ((hElement != NULL) && (hElement->ElemID() == KSvgPathElement))
    {
        TRAPD(error,hElement->SetAttributePathL(KAtrData, (CGfxGeneralPath*)hPath ));
        if ( error != KErrNone )
           {
            // Error Processing
            return;
           }
    }
}

//---------------------------------------------------------------------------=
//Returns the number of external resources in an svg document (A elements,IMG elements)
//---------------------------------------------------------------------------=

EXPORT_C TInt CSvgEngineInterfaceImpl::GetExternalListSize(CSvgDocumentImpl* aDocument)
{
    if ( ( aDocument == NULL ) || ( aDocument->RootElement() == NULL ) )
        return 0;

    TInt sum = 0;

    RPointerArray<CSvgElementImpl> a_elements;
    RPointerArray<CSvgElementImpl> image_elements;

    aDocument->FindAllElements( (CSvgElementImpl* )aDocument->RootElement(),
                                 KSvgAElement, a_elements );
    if ( a_elements.Count() != 0 )
    {
        sum = sum + a_elements.Count();
    }
        a_elements.Close();

    aDocument->FindAllElements( (CSvgElementImpl* )aDocument->RootElement(),
                                 KSvgImageElement, image_elements );
    if ( image_elements.Count() != 0 )
    {
        sum = sum + image_elements.Count();
    }
        image_elements.Close();

    return sum;

}

//---------------------------------------------------------------------------=
//Returns the uri of an external resource in an svg document (A elements,IMG elements)
//---------------------------------------------------------------------------=

EXPORT_C void CSvgEngineInterfaceImpl::GetExternalListItemL(
										CSvgDocumentImpl* aDocument, TInt aIndex, TPtrC16& aUri)
	{
	TDesC* uri = NULL;
	if ( ( aDocument != NULL ) && ( aDocument->RootElement() != NULL ) )
		{
		RPointerArray<CSvgElementImpl> a_elements;
		RPointerArray<CSvgElementImpl> image_elements;
		RPointerArray<CSvgElementImpl> all_externals;
		// Find & add all <a> elements
		aDocument->FindAllElements( (CSvgElementImpl* )aDocument->RootElement(),
								 KSvgAElement, a_elements );
		for (TInt i=0; i < a_elements.Count(); i++ )
			{
			all_externals.Append(a_elements[i]);
			}
		// Find & add all <image> elements
		aDocument->FindAllElements( (CSvgElementImpl* )aDocument->RootElement(),
								 KSvgImageElement, image_elements );
		for ( TInt i=0; i < image_elements.Count(); i++ )
			{
			all_externals.Append( image_elements[ i ] );
			}
		// Get element according to the given index
		if ( aIndex < all_externals.Count() )
			{

			CSvgElementImpl* myElement = (CSvgElementImpl*)all_externals[ aIndex ];
			if ( myElement )
				{
        		if ( (myElement->ElemID() ==
					KSvgImageElement) && ( ( ( CSvgImageElementImpl* ) myElement )->iIsBase64 ) )
					{
				   //do nothing because this is base64.
				   //want to return null for aValue
					}
				// Get an external resource uri
				else
					{
            		uri = myElement->HrefPointer();
            		// Check if the uri is already downloaded
            		if( ( uri != NULL ) && ( uri->Length() >= 0 ) )
						{
						TDesC* duplicateUri = NULL;
						for ( int index = aIndex - 1; index >= 0; index-- )
							{
							myElement = (CSvgElementImpl*)all_externals[ index ];
							if (myElement)
								{
								duplicateUri = myElement->HrefPointer();
								if ( ( duplicateUri != NULL ) && ( *uri == *duplicateUri ) )
									{
									// External resource is already downloaded. Don't have
									// to equest it again
									return;
									}
								}
							}
						}
					}
				}
			}
		image_elements.Close();
		a_elements.Close();
		all_externals.Close();
		}
	if( uri != NULL )
		{
		// Allocate heap memory
		/*TInt length = uri->Length() + 1;
		TUint16* result = new ( ELeave ) TUint16[ length ];
		TPtr16 resultPtr( result, length );
		resultPtr.Copy( *uri );
		resultPtr.ZeroTerminate();
		return CONST_CAST( TUint16*, resultPtr.Ptr() );*/

		aUri.Set(*uri);

		}
	return;
	}


//---------------------------------------------------------------------------=
// Finds the element with the corresponding uri and then adds the data that was
// passed in by an external resource handler
//---------------------------------------------------------------------------=
// This code was copied from the CSvgEngineImpl
EXPORT_C CSvgElementImpl* CSvgEngineInterfaceImpl::AddExternalData(
                                        CSvgDocumentImpl* aDocument,
                                        const TDesC& aUri,
                                        const TDesC8& aData,
                                        TBool aMakeCopy,
                                        TInt aSize,
                                        TInt aEngine )
    {
    // Use the internal engine's document if parameter is NULL
    if ( aDocument == NULL )
        {
        if ( ChooseEngine( aEngine) )
            {
            aDocument = iSvgEngine->Document();
            }
        }

    // Check whether document, root element are valid and URI is of 
    // non-zero length
    if ( ( aDocument == NULL ) || ( aDocument->RootElement() == NULL ) ||
         ( aUri.Length() == 0 ) )
        {
        return NULL;
        }

    // Find all <image> elements in current document
    RPointerArray<CSvgElementImpl> lAllImageElements;
    RPointerArray<CSvgElementImpl> lMatchingImageElements;
    aDocument->FindAllElements( (CSvgElementImpl*)aDocument->RootElement(),
                                KSvgImageElement,
                                lAllImageElements,
                                ESvgSearchExcludeUseSubtree ); // Ignore the 
                                                               // cloned image elements
                                                               // under use element
   
    // Set Owner Document to default Doc
    for ( TInt lIndex = 0; lIndex < lAllImageElements.Count(); lIndex++ )
        {
        lAllImageElements[ lIndex ]->SetOwnerDocument( aDocument );
        } // for ( TInt lIndex = 0; ...

    // Search for Image elements inside child document as well
    // First create a list of animation elements. 
    RPointerArray<CSvgElementImpl> lAllAnimationElements;                                
    aDocument->FindAllElements( ( CSvgElementImpl* )aDocument->RootElement(),
                                KSvgMediaAnimationElement,
                                lAllAnimationElements );                                
    // Each animation element has a child document, append the list of image elements
    // found in the child document to lAllImageElements
    TInt lAllAnimationEleCnt = lAllAnimationElements.Count();                            
    for ( TInt i = 0; i < lAllAnimationEleCnt; i++ )
        {
        CSvgMediaAnimationElementImpl* element = 
            ( CSvgMediaAnimationElementImpl* )lAllAnimationElements[i];
        CSvgDocumentImpl* ldoc = element->GetChildDocument();
        if ( ldoc )
            {
            RPointerArray<CSvgElementImpl> lChildImageElements;
            ldoc->FindAllElements( (CSvgElementImpl*)ldoc->RootElement(),
                                KSvgImageElement,
                                lChildImageElements,
                                ESvgSearchExcludeUseSubtree ); // Ignore the 
                                                               // cloned image elements
                                                               // under use element
            // Set Owner Document to child Doc
            for ( TInt lIndex = 0; lIndex < lChildImageElements.Count(); lIndex++ )
                {
                lChildImageElements[ lIndex ]->SetOwnerDocument( ldoc );
                // Also append the image elements into the image list
                lAllImageElements.Append( lChildImageElements[ lIndex ] );
                } // for ( TInt lIndex = 0; ...
              lChildImageElements.Close();  
            }
        } // for ( TInt i = 0; i < lAllAnimationEleCnt; i++ )

    lAllAnimationElements.Close();
    // lAllImageElements now contains image elements in parent as well as child documents
    // Generate a list with only those image elements with same URI
    TInt lAllImageEleCnt = lAllImageElements.Count();
    for ( TInt i = 0; i < lAllImageEleCnt; i++ )
        {
        CSvgImageElementImpl* element = ( CSvgImageElementImpl* )
            lAllImageElements[ i ];
        TPtrC href = element->Href();
        if ( href == aUri )
            {
            lMatchingImageElements.Append( element );
            }
        }
    lAllImageElements.Close();

    // Return if no <image> element matching URI
    if ( lMatchingImageElements.Count() == 0 )
        {
        lMatchingImageElements.Close();
        return NULL;
        }

    // Only first <image> element owns the imageData and all other image 
    // elements refer to this data for decoding.
    CSvgImageElementImpl* firstElement = ( CSvgImageElementImpl* )
        lMatchingImageElements[0];

    // This loop decodes the imageData for every image element which has 
    // the same URI. 
    // Two image elements could have the same URI, yet they may have different sizes
    // hence every image element decodes the same imageData.
    TInt lImageEleCnt = lMatchingImageElements.Count();
    for (TInt j=0; j < lImageEleCnt; j++)
        {
        CSvgImageElementImpl* lImageElement = ( CSvgImageElementImpl* )
            lMatchingImageElements[j];

        // Create bitmap if needed
        CFbsBitmap* bitmap = lImageElement->GetBitmap();
        if ( bitmap == NULL )
            {
            // Can't use ELeave because that this function can't leave
            bitmap = new CFbsBitmap();
            // Check if bitmap was allocated
            if ( !bitmap )
                {
                #ifdef _DBEUG
                RDebug::Printf("ERROR IN BITAMP CREATION: new (ELeave) CFbsBitmap(): %d", bitmapError);
                #endif
                // Return firstElement so that the image data is not deleted.
                // The data needs to be valid as this is happening inside a 
                // loop and previous image elements which have started decoding
                // this image data should not get affected.
                return firstElement;
                }
 
            // Create minimum bitmap size
            TInt createError = bitmap->Create( TSize( 1, 1 ), EColor64K );
            if ( createError != KErrNone )
                {
                #ifdef _DEBUG
                RDebug::Printf( "ERROR IN BITAMP CREATION: bitmap->Create: %d", createError );
                #endif
                delete bitmap;
                // Return firstElement so that the image data is not deleted.
                // The data needs to be valid as this is happening inside a 
                // loop and previous image elements which have started decoding
                // this image data should not get affected.
                return firstElement;
                }
            // Transfer ownership of bitmap to image element    
            lImageElement->SetBitmap( bitmap );
            } // if ( !bitmap )

        if ( aMakeCopy )
            {
            // Make a copy of the image data and transfer ownership for this
            // data to the particular image element
            HBufC8* lImageDataCopy = NULL;
            TRAPD( error, lImageDataCopy = HBufC8::NewL( aData.Length() ) );
 
            if ( error != KErrNone )
                {
                #ifdef _DEBUG
                    RDebug::Printf( "Failed to copy Image: Out of memory" );
                #endif
 
                return NULL;
                }
            // copy the data into the image element 
            lImageDataCopy->Des().Copy(aData);
 
            if (aData.Length() == 0 || aSize == 0)
                {
                // No data available, cleanup allocated memory
                delete lImageDataCopy;
                delete lImageElement->iBitmap;
                lImageElement->iBitmap = NULL;
                }
            else
                {
                // Begin image decoding (asynchronously).
                TInt lImgDecErr = lImageElement->StartImageDecoding( 
                        *lImageDataCopy, 
                        ETrue ); // ETrue means Sync decoding
                if ( lImgDecErr != KErrNone )
                    {
                    // No error processing
                    }
                lImageElement->AssignImageData( lImageDataCopy );
                }
 
            }
        else // if ( aMakeCopy )
            {
            // Do not make a copy of the imageData.
            // Begin image decoding (asynchronously).
            // An asynchronous approach was chosen as AddExternalData is called
            // to asynchronously notify the engine of availability of image 
            // data.
            // The underlying ImageDecoder is an asynchronous API and an 
            // ActiveSchedulerWait object is used to make it synchronous. In 
            // this case the client Active objects get a chance to execute. 
            // While decoding is in progress, if the client exits it used to
            // delete the engine and the document but async service provider is
            // not yet complete. When the async service provider completes 
            // it would cause the access violation error as it would 
            // try to use image element object which has been deleted.
            TInt lImgDecErr = lImageElement->StartImageDecoding( aData, 
                    EFalse ); // EFalse means async decoding

            if ( lImgDecErr != KErrNone )
                {
                // No error processing
                }
            } // if ( aMakeCopy )
        } // for (TInt j=0; j < lImageEleCnt; j++)

    lMatchingImageElements.Close();

    return firstElement;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AssignImageData( const TDesC& aUri,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AssignImageData( const TDesC& aUri,
                                                        HBufC8* aData )
    {
    CSvgImageElementImpl* element =
        (CSvgImageElementImpl*)AddExternalData( NULL, aUri, *aData, EFalse );
    if ( element == NULL )
        {
        // delete image data now -- due to failure
        delete aData;
        }
    else
        {
        // <image> element will delete it after decoding
        element->AssignImageData( aData );
        }
    }

/********************** Wrapper functions for new avoid new exports *******************/
// --------------------------------------------------------------------------
// EXPORT_C CSvgEngineImpl* CSvgEngineInterfaceImpl::SvgEngineNewL()
// ---------------------------------------------------------------------------
EXPORT_C CSvgEngineImpl* CSvgEngineInterfaceImpl::SvgEngineNewL()
{
    //this is the JSR side creating the engine
    iSvgEngine = CSvgEngineImpl::NewL(iSvgBitmapFontProvider);
    iSvgEnginePointers.Append(iSvgEngine);
    return iSvgEngine;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::StartEngine( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::StartEngine( CSvgEngineImpl* aEngine )
{
    if( ChooseEngine ( (TInt)aEngine ) )
        {
        iSvgEngine->StartEngine();
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RenderFrame( CSvgEngineImpl* aEngine, TUint aCurrentTime )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RenderFrame( CSvgEngineImpl* aEngine, TUint aCurrentTime )
{
    if( ChooseEngine ( (TInt)aEngine ) )
        {
        iSvgEngine->RenderFrame( aCurrentTime );
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ViewportInit( CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ViewportInit( CSvgDocumentImpl* aDocumentHandle )
{
    if( !aDocumentHandle )
        return;

    aDocumentHandle->iInitialDrawFlag = ETrue;


    TSize Size = TSize(100, 100); // default viewport size
    aDocumentHandle->iReqExReqFtrSysLTested = EFalse;
    if ( aDocumentHandle->RootElement() != NULL &&
         ((CSvgElementImpl*)aDocumentHandle->RootElement())->ElemID() == KSvgSvgElement )

        {
        // Scale width & height to fit to screen size
        CSvgSvgElementImpl*     el      = ( CSvgSvgElementImpl* )
                                          (aDocumentHandle->RootElement());
        TFloatFixPt                  width, height;
        TFloatFixPt scrnW( Size.iWidth ), scrnH( Size.iHeight );
        TFloatFixPt                  zero    ( 0 );
        if (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iWidthInPercentage)
            {
            // since we are mapping the percentage to a viewport that is 100x100 the percentage value can be used directly
            width = ( TFloatFixPt ) (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iWidthInUserCoordinate);
            }
        else
            {
            width  = (TFloatFixPt)  (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iWidthInUserCoordinate);
            }

        if (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iHeightInPercentage)
        {
            height = ( TFloatFixPt ) (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iHeightInUserCoordinate);
        }
        else
        {
            height = (TFloatFixPt)  (((CSvgSvgElementImpl *)(aDocumentHandle->RootElement()))->iHeightInUserCoordinate);
        }
        if ( ( width <= zero ) && ( height <= zero ) )
            {
            width = 0;
            height = 0;
            return;
            }

        // Set initial viewport
        TInt err;
        TRAP(err, el->SetAttributeFloatL( KAtrX, zero ));
        TRAP(err, el->SetAttributeFloatL( KAtrY, zero ));
        TRAP(err, el->SetAttributeFloatL( KAtrWidth, width ));
        TRAP(err, el->SetAttributeFloatL( KAtrHeight, height ));
        }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetGdiContextL( CSvgEngineImpl* aEngine, CFbsBitmap* aFrameBuffer )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetGdiContextL( CSvgEngineImpl* aEngine, CFbsBitmap* aFrameBuffer )
{
    if( aEngine )
        {
        aEngine->SetGdiContextL( aFrameBuffer );
        }
}

// --------------------------------------------------------------------------
//  M2G: Overloaded SetGdiContextL() for CSvgtBitmap to enable rendering on target buffer.
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetGdiContextL( CSvgEngineImpl* aEngine, CSvgtBitmap* aFrameBuffer )
    {
    if( aEngine )
        {
        aEngine->EnableTargetRendering(ETrue);
        aEngine->SetGdiContextL( aFrameBuffer );
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetDocument( CSvgEngineImpl* aEngine, CSvgDocumentImpl* aDocument )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetDocument( CSvgEngineImpl* aEngine, CSvgDocumentImpl* aDocument )
{
    iTotalRotation = 0;
    if( aEngine && aDocument)
        {
        aEngine->SetDocument( aDocument );
        aDocument->SetEngine( aEngine );

        // costly calls per frame
        SvgElementAssignDocument((CSvgElementImpl*)aDocument->RootElement(), aDocument);

        ((CSvgElementImpl*)aDocument->RootElement())->InitAllSvgStyleProperties((CSvgElementImpl*)aDocument->RootElement());

        }
}

// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::SvgElementAssignDocument( CSvgElementImpl* aElement, CSvgDocumentImpl* aDoc )
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::SvgElementAssignDocument( CSvgElementImpl* aElement, CSvgDocumentImpl* aDoc )
{
    if ( aElement == NULL )
        return;

    aElement->SetOwnerDocument( aDoc );

    CSvgElementImpl* child = (CSvgElementImpl*)aElement->FirstChild();
    if ( child )
        {
        SvgElementAssignDocument( child, aDoc );
        }

    CSvgElementImpl* sibling = (CSvgElementImpl*)aElement->NextSibling();
    if ( sibling )
        {
        SvgElementAssignDocument( sibling, aDoc );
        }
}


// --------------------------------------------------------------------------
// EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::SvgDocumentNewL()
// ---------------------------------------------------------------------------
EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::SvgDocumentNewL()
{
	CSvgDocumentImpl* document = CSvgDocumentImpl::NewL(iSvgBitmapFontProvider);
	iSvgDocumentPointers.Append(document);

    return document;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::FillDocumentL( CSvgDocumentImpl* aDocument, const TDesC16& aSvgString )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::FillDocumentL( CSvgDocumentImpl* aDocument, const TDesC16& aSvgString )
{
    if( aDocument )
        {
        aDocument->SetDRMMode( iDrmEnabled );
        SetThumbNailMode( iIsThumbNailMode );
        SetDRMRights( iDrmRightsConsumptionEnabled );
        aDocument->Load16BitString(aSvgString, *iSvgError, EFalse );
        if ( aDocument->HasError() )
            {
            iSvgError->SetErrorCode( aDocument->GetError()->ErrorCode() );
            iSvgError->SetSystemErrorCode( aDocument->GetError()->SystemErrorCode() );
            iSvgError->SetDescription( aDocument->GetError()->Description() );
            User::Leave( iSvgError->SystemErrorCode() );
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::DestroyDocument( CSvgDocumentImpl* aDocument )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::DestroyDocument( CSvgDocumentImpl* aDocument )
{
    if (aDocument )
    {
        TInt index = iSvgDocumentPointers.Find( aDocument );
        if ( index != KErrNotFound )
        	{
        		//make sure document isnt attached to an engine
        		TInt svgEnginePointersCnt = iSvgEnginePointers.Count();
        		for (TInt i=0; i < svgEnginePointersCnt; i++)
       			{
       				CSvgEngineImpl* engine = iSvgEnginePointers[i];
       				CSvgDocumentImpl* docAttachedToEngine = engine->Document();

       				if (docAttachedToEngine == aDocument)
       				{
       					//remove document attachment to engine
       					engine->SetDocument(NULL);
       				}
       			}

        		delete aDocument;
        		iSvgDocumentPointers.Remove( index );
        	}
    }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::DestroyEngine( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::DestroyEngine( CSvgEngineImpl* aEngine )
{
    if (aEngine)
    {
    	TInt index = iSvgEnginePointers.Find( aEngine );
        if ( index != KErrNotFound )
        	{
        		//make sure no document has this engine attached
        		TInt svgDocPointersCnt = iSvgDocumentPointers.Count();
        		for (TInt i=0; i < svgDocPointersCnt; i++)
       			{
       				CSvgDocumentImpl* document = iSvgDocumentPointers[i];
       				CSvgEngineImpl* engineAttachedToDocument = document->Engine();

       				if (engineAttachedToDocument == aEngine)
       				{
       					//remove engine from document it is attached to
       					document->SetEngine(NULL);
       				}
       			}

       			//make sure there is no handle to this left around
       			if (iSvgEngine == aEngine)
       			{
       				iSvgEngine = NULL;
       			}

        		delete aEngine;
        		iSvgEnginePointers.Remove( index );
        	}
   	}
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetRootElement( CSvgDocumentImpl* aDocument )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetRootElement( CSvgDocumentImpl* aDocument )
{
    if( aDocument )
        {
        return (CXmlElementImpl*)aDocument->RootElement();
        }
    else
        return NULL;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::InitRootElement( CSvgDocumentImpl* aDocument )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::InitRootElement( CSvgDocumentImpl* aDocument )
{
    if( aDocument )
        {
        TRAPD(error,aDocument->AppendChildL(CreateElementL(aDocument,KSvgSvgElement)));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KAppendChildFailed,
                "CSvgEngineInterfaceImpl::InitRootElement: AppendChildL error.");
            RDebug::Print(KAppendChildFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetElementById( CSvgDocumentImpl* aDocument, const TDesC& aId )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetElementById( CSvgDocumentImpl* aDocument, const TDesC& aId )
{
    if( aDocument )
        {
        return (CXmlElementImpl*)aDocument->GetElementById( aId );
        }
    else
        return NULL;
}

// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetNumberOfIds( CSvgDocumentImpl* aDocument )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetNumberOfIds( CSvgDocumentImpl* aDocument )
{
    if ( aDocument )
        return (aDocument->GetNumberOfIds( aDocument->RootElement()));
    else
        return 0;
}

// --------------------------------------------------------------------------
// EXPORT_C TDesC* CSvgEngineInterfaceImpl::GetId(CSvgDocumentImpl* aDocument, TInt index)
// ---------------------------------------------------------------------------
EXPORT_C TDesC* CSvgEngineInterfaceImpl::GetId(CSvgDocumentImpl* aDocument, TInt index)
{
    if ( aDocument )
        return (aDocument->GetId(index));
    else
        return NULL;
}

// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetParentElement( CXmlElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetParentElement( CXmlElementImpl* aElement )
{
    if( aElement )
        return (CXmlElementImpl*)aElement->ParentNode();
    else
        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetParentElement( CXmlElementImpl* aElement, CXmlElementImpl* aParentElement )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetParentElement( CXmlElementImpl* aElement, CXmlElementImpl* aParentElement )
{
    if( aElement )
        aElement->SetParentNode( aParentElement );
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetFirstChildElement( CXmlElementImpl* aElement, CXmlElementImpl* aParentElement )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetFirstChildElement( CXmlElementImpl* aFirstChildElement, CXmlElementImpl* aParentElement )
{
    if( aFirstChildElement && aParentElement )
        aParentElement->SetFirstChild( aFirstChildElement );
}

// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::CreateElementL( CSvgDocumentImpl* aDocument, TInt aElementType )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::CreateElementL( CSvgDocumentImpl* aDocument, TInt aElementType )
{
    if( aDocument )
        return (CXmlElementImpl*)aDocument->CreateElementL( (TUint8)aElementType );
    else
        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::DestroyElement( CXmlElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::DestroyElement( CXmlElementImpl* aElement )
{
    delete aElement;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AppendChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement, TBool aIsJSR226Element)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AppendChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement, TBool aIsJSR226Element)
{
    // Note: AppendChildL does not have any leaveble calls
    if( aParentElement && aChildElement )
        {

        MXmlElement* childExisted = NULL;
        // Avoid appending child to parent twice
        if ( aChildElement->ParentNode() == aParentElement )
            {
            //mark the first copy of the element that is already
            //appended as a child that to be removed
            MXmlElement* child = aParentElement->FirstChild();
            if ( child == aChildElement )
                {
                childExisted = child;
                }
            while ( child && !childExisted )
                {
                child = child->NextSibling();
                if ( child == aChildElement )
                    {
                    	//mark the first copy of the element that is already
                    	//appended as a child that to be removed
                    	childExisted = child;
                    	break;
                    }
                }
            }

				if ( childExisted )
				{
					aParentElement->RemoveChild( childExisted );
				}

        TRAPD( error, aParentElement->AppendChildL( aChildElement, aIsJSR226Element ) );
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KAppendChildFailed,
                "CSvgEngineInterfaceImpl::AppendChild: AppendChildL error.");
            RDebug::Print(KAppendChildFailed);
            #endif //_DEBUG
            }

        if ( aChildElement->ElemID() == KSvgUseElement)
        {
        	//we need to set the forward references up now
        	((CSvgUseElementImpl*)aChildElement)->SetRefElemById( ((CSvgUseElementImpl*)aChildElement)->Href() );

        	TRAP_IGNORE( ((CSvgUseElementImpl *)aChildElement)->SetReferenceElementL() );
        }
        
        
        // If aChildElement is a Stop element then add it to Gradient's stop element array.
        TInt lParentId =  ((CXmlElementImpl*)aParentElement)->ElemID();
        TInt lChildId  =  ((CXmlElementImpl*)aChildElement)->ElemID();
            
        if(  ( ( ( lParentId == KSvgRadialGradientElement )||
                 ( lParentId == KSvgLinearGradientElement ) ) &&
                 ( lChildId == KSvgStopElement ) ) )

	        {
		    // Initialize the offset value to 0 if its still -1.
	        TFloatFixPt lOffsetValue;
	        TFloatFixPt lDefaultOffsetValue(-1);
	        TBuf<6>     lAttributeName;
	        TBuf<1>     lValueBuffer;
	                      
	        lAttributeName.Append(OFFSET);
	        lValueBuffer.Append(ZEROVALUE);
	            
	        ((CSvgStopElementImpl*)aChildElement)->GetOffset( lOffsetValue );
	         
	        // Offset Value of -1 indicates that Offset Attribute is not declared in
	        // stop element.      
	        if( lOffsetValue == lDefaultOffsetValue )
	           {  
	           // Deliberately calling SetAttributeL in place of SetAttributeFloatL as the latter inturn
	           // calls UpdateOffsetValues which should be called on any Stop element once it is added to
	           // to the Stop element array Owned by parent Gradient element.
	           TRAP_IGNORE(((CSvgStopElementImpl*)aChildElement)->SetAttributeL( lAttributeName, lValueBuffer ));
	           }
	                
	         // The function not only adds the element in Stop element array but also
	         // Adjusts the offset values of all the previously added elements such that
	         // each gradient offset value is greater than the previous gradient stop's
	         // offset value.It calls UpdateOffsetValues to adjust the values.
	         ((CSvgGradientElementImpl*)aParentElement)->AddStopElementInArray((CSvgStopElementImpl*)aChildElement);
	                                  
	         }                          

        //CTM needs to be updated after appending child so it gets propogated to new children
        ((CSvgElementImpl*)aParentElement)->UpdateCTM();

        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RemoveChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RemoveChild( CXmlElementImpl* aParentElement, CXmlElementImpl* aChildElement)
{
    if( aParentElement && aChildElement )
        {
        aParentElement->RemoveChild( aChildElement );
        
        if( ((CXmlElementImpl*)aChildElement)->ElemID() == KSvgTextElement )
        	{
        	((CSvgTextElementImpl*)aChildElement)->FreeFontData();
        	}
        
        }
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetFirstChild( CXmlElementImpl* aParentElement )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetFirstChild( CXmlElementImpl* aParentElement )
{
    if( aParentElement )
        return (CXmlElementImpl*)aParentElement->FirstChild();
    else
        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetNextSibling( CXmlElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetNextSibling( CXmlElementImpl* aElement )
{
    if( aElement )
        return (CXmlElementImpl*)aElement->NextSibling();
    else
        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetNextSibling( CXmlElementImpl* aElement, CXmlElementImpl* aSibling )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetNextSibling( CXmlElementImpl* aElement, CXmlElementImpl* aSibling )
{
    if( aElement && aSibling )
        aElement->SetNextSibling( aSibling );
}

// --------------------------------------------------------------------------
// EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::GetOwnerDocument( CXmlElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C CSvgDocumentImpl* CSvgEngineInterfaceImpl::GetOwnerDocument( CXmlElementImpl* aElement )
{
    if( aElement)
        return (CSvgDocumentImpl*)aElement->OwnerDocument();
    else
        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetElementType( CXmlElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetElementType( CXmlElementImpl* aElement )
{
    if( aElement )
        return aElement->ElemID();
    else
        return -1;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetRequestObserver( CSvgEngineImpl* aEngine, MSvgRequestObserver* aObserver )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetRequestObserver( CSvgEngineImpl* aEngine, MSvgRequestObserver* aObserver )
{
    if( aEngine )
        aEngine->iRequestObserver = aObserver;
}
// --------------------------------------------------------------------------
// EXPORT_C TInt  CSvgEngineInterfaceImpl::GetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TDes& aValue)
// ---------------------------------------------------------------------------
EXPORT_C TInt  CSvgEngineInterfaceImpl::GetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId, TPtrC16& aValue)
{
    return aElementHandle->GetAttributeDes(aAttributeId, aValue);
}
// --------------------------------------------------------------------------
// EXPORT_C TReal32 CSvgEngineInterfaceImpl::GetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId)
// ---------------------------------------------------------------------------
EXPORT_C TReal32 CSvgEngineInterfaceImpl::GetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId) __SOFTFP
{

    TFloatFixPt lValue ;

    //hack for JSR 226 so it can return the original base value when the attribute is animated
    //instead of returning the animated value in the element class
    if ( aElementHandle->HasAnimation() )
    {
    	TInt lResult = ((CSvgAnimationBase*)aElementHandle->iHasAnimationBase)->GetAttributeFloat( aAttributeId, lValue);

    	if(lResult != KErrNoAttribute)
        {
 			return lValue;
        }
    }

    TInt lResult = aElementHandle->GetAttributeFloat( aAttributeId, lValue);
    if(lResult == KErrNoAttribute)
        {
        return KInvalidFloatAttribute;
        }
    return lValue;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId, TReal32 aValue)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetElementFloatAttribute( CSvgElementImpl* aElementHandle, const TInt aAttributeId, TReal32 aValue) __SOFTFP
{
    TFloatFixPt lValue = TFloatFixPt(aValue);
    TRAPD(error,aElementHandle->SetAttributeFloatL(aAttributeId,lValue));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KSetAttributeFloatFailed,
            "CSvgEngineInterfaceImpl::SetElementFloatAttribute: SetElementFloatAttribute error.");
        RDebug::Print(KSetAttributeFloatFailed);
        #endif //_DEBUG
        }

   //hack for JSR 226 so it can return the original base value when the attribute is animated
    //instead of returning the animated value in the element class
    if ( aElementHandle->HasAnimation() )
    {
    	TRAP_IGNORE( ((CSvgAnimationBase*)aElementHandle->iHasAnimationBase)->SetAttributeFloatL( aAttributeId, lValue) );
    }

}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId, TDesC& lAttributeValue)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetElementDesAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId, const TDesC& lAttributeValue)
{
    
    const TDesC* ReferenceElementID = NULL;
    
    // Incase if the xlink-href attribute is set for USE element from JSR226 we need to 
    // create a clone tree under the USE element explicitly same as done in Content handler.
    if( aElementHandle->ElemID() == KSvgUseElement && aAttributeId == KAtrXlinkhref )
    {
        // Check if the USE element is already refereing to some element. If 'YES' we need to 
        // remove the previously refered element.
        CSvgElementImpl* lReferenceElement = ((CSvgUseElementImpl*)aElementHandle)->GetReferenceElementL();
        
        if( lReferenceElement != NULL)
        {
        	// Get the ID of this element.
        	ReferenceElementID = lReferenceElement->Id();
                    	
        	// Get the first child of USE element and compare its ID with previously
        	// refered element's ID. If they are same remove the referenced element branch
        	// from the USE element. Else, continue finding some element under 'USE' element
        	// that has same ID as the previously refered element has.
        	CSvgElementImpl* ChildOfUseElement = (CSvgElementImpl*)aElementHandle->FirstChild();
        	
        	while( ChildOfUseElement && ReferenceElementID )
        	{
        	    const TDesC16* UseElementChildID = ChildOfUseElement->Id();
        	    
        	    // Compare the Child ID with reference element's ID
				if( UseElementChildID && ( ReferenceElementID->Compare(*UseElementChildID) == 0 ))
				{
					// delete the same child and get out of the loop.
					aElementHandle->RemoveChild(ChildOfUseElement);
					break;
				}
				
				ChildOfUseElement = (CSvgUseElementImpl*)ChildOfUseElement->NextSibling();
        	}
        }
    }
        
    TRAPD(error,aElementHandle->SetAttributeDesL(aAttributeId,lAttributeValue));
    
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KSetAttributeDesFailed,
            "CSvgEngineInterfaceImpl::SetElementDesAttribute: SetAttributeDesL error.");
        RDebug::Print(KSetAttributeDesFailed);
        #endif //_DEBUG
        }
    
    if( ( aElementHandle->ElemID() == KSvgUseElement ) && aAttributeId == KAtrXlinkhref )
    {
    	// Create a new branch under the USE element
    	TRAP(error,((CSvgUseElementImpl *)aElementHandle)->SetReferenceElementL());
    	
    	if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KSetReferenceElementLFailed,
            "CSvgUseElementImpl::SetReferenceElementL: SetReferenceElementL error.");
        RDebug::Print(KSetReferenceElementLFailed);
        #endif //_DEBUG
        }
    }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl:: GetElementBoundingbox(CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight)
// this is used for ScreenBbox (actual bbox of an element on the screen)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl:: GetElementBoundingbox(CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight) __SOFTFP
{
    if (aElementHandle)
    {
    	//updates CTM from root
        aElementHandle->UpdateCTM();

        TGfxRectangle2D     bbox;
        aElementHandle->GetBBox(bbox);
        aX = (TReal32)bbox.iX;
        aY = (TReal32)bbox.iY;
        aWidth = (TReal32)bbox.iWidth;
        aHeight = (TReal32)bbox.iHeight;
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl:: GetElementUnScaledBoundingBox(CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight)
// this is used for UserBBox (unscaled bbox)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::GetElementUnScaledBoundingBox( CSvgElementImpl* aElementHandle,TReal32& aX ,TReal32& aY,TReal32& aWidth,TReal32& aHeight) __SOFTFP
{
		if (aElementHandle)
    {
		//updates CTM from root
        aElementHandle->UpdateCTM();

        TGfxRectangle2D     bbox;
        aElementHandle->GetUnscaledBBox(bbox);
        aX = (TReal32)bbox.iX;
        aY = (TReal32)bbox.iY;
        aWidth = (TReal32)bbox.iWidth;
        aHeight = (TReal32)bbox.iHeight;
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::GetFourPointElementBoundingBox(CSvgTextElementImpl* aTextElementHandle, TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::GetFourPointElementBoundingBox(CSvgTextElementImpl* aTextElementHandle, TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4)
{
    TSvgFourPointRect fourPointRect;

	//updates CTM from root
    if (aTextElementHandle)
    {
        aTextElementHandle->UpdateCTM();
        aTextElementHandle->GetFourPointBBox(fourPointRect);
    }

    fourPointRect.GetPoints(aPoint1, aPoint2, aPoint3, aPoint4);
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl:: SetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 lColorValue)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl:: SetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 lColorValue)
{
    TRAPD(error, aElementHandle->SetAttributeIntL(aAttributeId,lColorValue));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KSetAttributeIntFailed,
            "CSvgEngineInterfaceImpl::SetElementColorAttribute: SetAttributeIntL( error.");
        RDebug::Print(KSetAttributeIntFailed);
        #endif //_DEBUG
        }

}
// --------------------------------------------------------------------------
// EXPORT_C TInt32 CSvgEngineInterfaceImpl::GetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId)
// ---------------------------------------------------------------------------
EXPORT_C TInt32 CSvgEngineInterfaceImpl::GetElementColorAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId)
{
    TInt lResult = KErrNone;
    TInt32 lValue = KErrNone;
    TRAPD(error,lResult = aElementHandle->GetAttributeIntL( aAttributeId, lValue));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KGetAttributeIntFailed,
            "CSvgEngineInterfaceImpl::GetElementColorAttribute: GetElementColorAttribute( error.");
        RDebug::Print(KGetAttributeIntFailed);
        #endif //_DEBUG
        }

    if (lResult == KErrInvalidAttribute && error == KErrNone)
    {
    	//attribute requested was invalid return an error
    	return KInvalidEnumAttribute;
    }
    else if ( lResult == KErrNone && error == KErrNone)
    {
    	//got the attribute ok.
    	return lValue;
    }
    else if(lResult == KErrNoAttribute && error == KErrNone)
    {
        //everything went ok that element just didnt have the attribute requested
        return KErrNoAttribute;
    }
    else
        {
        return KInvalidEnumAttribute;
        }

}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetPreserveApsectRatio( TPreserveAspectRatio aType )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetPreserveAspectRatio( CSvgDocumentImpl* aDocument, TSvgPreserveAspectAlignType aPreserveAspectRatioSetting, TSvgMeetOrSliceType aMeetSliceType, TBool aFrameBuffOverridesViewport)
{
	if ( !aDocument )
	{
		aDocument = iSvgLoadedDocument;

		if ( !aDocument )
		{
			if (iSvgEngine)
			{
				aDocument = iSvgEngine->Document();
			}
		}

		if ( !aDocument )
		return;
	}

	if (iSvgEngine)
			{
				iSvgEngine->iFrameBufferOverridesViewport = aFrameBuffOverridesViewport;
			}

	CSvgSvgElementImpl* el = ( CSvgSvgElementImpl* )(aDocument->RootElement());

	if (el == NULL)
	{
		return;
	}

	TRAPD(error, el->SetPreserveAspectRatioL( aPreserveAspectRatioSetting, aMeetSliceType));

	if (error != KErrNone)
  	{
    	#ifdef _DEBUG
      _LIT(KSetPreserveAspectRatioFailed,
      "CSvgEngineInterfaceImpl::SetPreserveAspectRatio: error.");
      RDebug::Print(KSetPreserveAspectRatioFailed);
      #endif //_DEBUG
    }

}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 lValue)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32 lValue)
    {

    if(aAttributeId == KAtrPreserveAspectRatio)
        {
        // convert in to desc and Call set attribute
        _LIT( KNone, "none" );
        _LIT( KXMidYMid, "xMidYMid" );
        _LIT( KPreserveAspectRatio, "preserveAspectRatio" );
        if(!lValue)
            {
            TRAPD(error,aElementHandle->SetAttributeL(KPreserveAspectRatio,KNone));
            if (error != KErrNone)
                {
                #ifdef _DEBUG
                _LIT(KSetAttributeFailed,
                    "CSvgEngineInterfaceImpl::SetEnumAttribute: SetAttributeL error.");
                RDebug::Print(KSetAttributeFailed);
                #endif //_DEBUG
                }
            }
        if(lValue == 1)
            {
            TRAPD(error,aElementHandle->SetAttributeL(KPreserveAspectRatio,KXMidYMid));
            if (error != KErrNone)
                {
                #ifdef _DEBUG
                _LIT(KSetAttributeFailed,
                    "CSvgEngineInterfaceImpl::SetEnumAttribute: SetAttributeL error.");
                RDebug::Print(KSetAttributeFailed);
                #endif //_DEBUG
                }
            }
        return ;
        }
    if( aAttributeId == KAtrZoomAndPan)
        {
        if( aElementHandle->ElemID() == KSvgSvgElement )
            {
            ((CSvgSvgElementImpl*)aElementHandle)->SetZoomAndPan((TSvgZoomAndPanType)lValue);
            }
        return;
        }
    // for fill and stroke if the value is specified as enum need to
    // convert it in to "string"
    // also check that it is not a animation elements.
    // get enum for these values is not possible.
    if((aAttributeId == KCSS_ATTR_FILL || aAttributeId == KCSS_ATTR_STROKE)
         && ! (   aElementHandle->IsAnimatedElement() ))
        {
        if(lValue == 2)
            {
            _LIT( KNone, "none" );
            TRAPD(error,aElementHandle->SetPropertyL(aAttributeId,KNone));
            if (error != KErrNone)
                {
                #ifdef _DEBUG
                _LIT(KSetPropertyFailed,
                    "CSvgEngineInterfaceImpl::SetEnumAttribute: SetPropertyL error.");
                RDebug::Print(KSetPropertyFailed);
                #endif //_DEBUG
                }
            return;
            }
        if(lValue == 3)
            {
            _LIT( KCurrentColor, "currentColor" );
            TRAPD(error,aElementHandle->SetPropertyL(aAttributeId,KCurrentColor));
            if (error != KErrNone)
                {
                #ifdef _DEBUG
                _LIT(KSetPropertyFailed,
                    "CSvgEngineInterfaceImpl::SetEnumAttribute: SetPropertyL error.");
                RDebug::Print(KSetPropertyFailed);
                #endif //_DEBUG
                }
            return;
            }
        if(lValue == 4)
            {
            if (!(aElementHandle->StyleInParent(aElementHandle, aAttributeId)))
            {
            aElementHandle->RemoveAttribute(aAttributeId);
            }
            return;
            }
        }

    TRAPD(error, aElementHandle->SetAttributeIntL(aAttributeId,lValue));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KSetAttributeIntFailed,
            "CSvgEngineInterfaceImpl::SetEnumAttribute: SetAttributeIntL error.");
        RDebug::Print(KSetAttributeIntFailed);
        #endif //_DEBUG
        }
    }


// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32& aValue)
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetEnumAttribute(CSvgElementImpl* aElementHandle,const TInt aAttributeId,TInt32& aValue)
{

    if(aAttributeId == KAtrPreserveAspectRatio)
        {
        // convert in to desc and Call set attribute

        if(aElementHandle->ElemID() == KSvgSvgElement)
            {
            MSvgPreserveAspectRatio *lTestObject;
            ((CSvgSvgElementImpl*)aElementHandle)->GetPreserveAspectRatio(lTestObject);
            if(lTestObject != NULL)
                {
                aValue = (TInt32)lTestObject->GetAlign();
                return KErrNone;
                }
            return KInvalidEnumAttribute;
            }
        if(aElementHandle->ElemID() == KSvgImageElement)
            {
            _LIT( KNone, "none" );
            _LIT( KXMidYMid, "xMidYMid" );
            TPtrC16 lParValue;
            aElementHandle->GetAttributeDes( KAtrPreserveAspectRatio, lParValue );
            if(lParValue == KNone)
                {
                aValue = 0;
                return KErrNone;
                }
            if(lParValue == KXMidYMid)
                {
                aValue = 1;
                return KErrNone;
                }
            return KInvalidEnumAttribute;
            }

        }
    if( aAttributeId == KAtrZoomAndPan)
        {
        if( aElementHandle->ElemID() == KSvgSvgElement )
            {
            aValue = (TInt32)((CSvgSvgElementImpl*)aElementHandle)->GetZoomAndPan();
            return KErrNone;
            }
        return KInvalidEnumAttribute;
        }
    TInt lResult = KErrNone;
    TRAPD(error, lResult = aElementHandle->GetAttributeIntL(aAttributeId,aValue));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KGetAttributeIntFailed,
            "CSvgEngineInterfaceImpl::GetEnumAttribute: GetAttributeIntL error.");
        RDebug::Print(KGetAttributeIntFailed);
        #endif //_DEBUG
        }
    if(lResult == KErrNoAttribute)
        {
         return KInvalidEnumAttribute;
        }
    return KErrNone;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetRectAttribute( CXmlElementImpl* aElementHandle,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetRectAttribute( CXmlElementImpl* aElementHandle,
                                                        float aX, float aY, float aWidth, float aHeight )
{
    if ( aElementHandle->ElemID() ==  KSvgRectElement )
        {
        CSvgRectElementImpl*  rect = (CSvgRectElementImpl*)aElementHandle;
        rect->SetRectValues(TFloatFixPt(aX), TFloatFixPt(aY), TFloatFixPt(aWidth), TFloatFixPt(aHeight));
        }
    // added as a part of update of get and set API's.
    if ( aElementHandle->ElemID() ==  KSvgSvgElement)
        {
        // for svg element it must be viewBox.
        TGfxRectangle2D     viewBox(aX, aY,aWidth,aHeight);
        CSvgSvgElementImpl *lSvgSvgElement = (CSvgSvgElementImpl*)(aElementHandle);
        TRAPD(error,lSvgSvgElement->SetViewBoxL( viewBox));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KSetRectAttributeFailed,
                "CSvgEngineInterfaceImpl::SetRectAttribute: SetViewBoxL error.");
            RDebug::Print(KSetRectAttributeFailed);
            #endif //_DEBUG
            }

         //CTM must be updated since viewbox changed
         lSvgSvgElement->UpdateCTM();

        }
}
// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::GetRectAttribute( CXmlElementImpl* aElementHandle,
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::GetRectAttribute( CXmlElementImpl* aElementHandle,
                                                        float* aX, float* aY, float* aWidth, float* aHeight )
{
    // added as a part of update of get and set API's.
    TBool viewBoxExisted = EFalse;

    if ( aElementHandle->ElemID() ==  KSvgSvgElement)
        {
        // for svg element it must be viewBox.
        TGfxRectangle2D     viewBox;
        CSvgSvgElementImpl *lSvgSvgElemet = (CSvgSvgElementImpl*)(aElementHandle);

        viewBoxExisted = lSvgSvgElemet->GetViewBox(viewBox);
        *aX = viewBox.iX;
        *aY = viewBox.iY;
        *aWidth  = viewBox.iWidth;
        *aHeight = viewBox.iHeight;
        }
    else if ( aElementHandle->ElemID() ==  KSvgRectElement )
        {
        CSvgRectElementImpl*  rect = (CSvgRectElementImpl*)aElementHandle;
        *aX = rect->X();
        *aY = rect->Y();
        *aWidth  = rect->Width();
        *aHeight = rect->Height();
        }
     else
        {
         aX = NULL;
        }

        return viewBoxExisted;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::GetMatrixAttribute( CXmlElementImpl* aElementHandle, float* aAVal,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::GetMatrixAttribute( CXmlElementImpl* aElementHandle, float* aAVal,
                                                           float* aBVal, float* aCVal, float* aDVal,
                                                           float* aEVal, float* aFVal )
{
		//this should have been taken care of in the set only -
		//what about updating from the root all the way down here.
		//((CSvgElementImpl*)aElementHandle)->UpdateCTM();

    const TGfxAffineTransform& matrix = ((CSvgElementImpl*)aElementHandle)->GetCTM();

    *aAVal = matrix.iM00;
    *aBVal = matrix.iM10;
    *aCVal = matrix.iM01;
    *aDVal = matrix.iM11;
    *aEVal = matrix.iM02;
    *aFVal = matrix.iM12;
}


// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::GetMatrixAttribute( CXmlElementImpl* aElementHandle, float* aAVal,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::GetMatrixAttribute( CXmlElementImpl* aElementHandle, TInt aAttributeType,
														   float* aAVal, float* aBVal, 
														   float* aCVal, float* aDVal,
                                                           float* aEVal, float* aFVal )
{
    if( aAttributeType == KAtrTransform)
    {
	    MSvgTransformList*  trList;
	    ((CSvgElementImpl*)aElementHandle)->GetTransform( trList );
	    TGfxAffineTransform matrix = 
	      trList->Consolidate( ((CSvgElementImpl*)aElementHandle)->IsOverwriteTransforms() );

	    *aAVal = matrix.iM00;
	    *aBVal = matrix.iM10;
	    *aCVal = matrix.iM01;
	    *aDVal = matrix.iM11;
	    *aEVal = matrix.iM02;
	    *aFVal = matrix.iM12;
    }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetMatrixAttribute( CXmlElementImpl* aElementHandle, float aAVal,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetMatrixAttribute( CXmlElementImpl* aElementHandle, float aAVal,
                                                           float aBVal, float aCVal, float aDVal,
                                                           float aEVal, float aFVal )
{
    if ((CSvgElementImpl*)aElementHandle)
    {
        TGfxAffineTransform matrix;
        matrix.SetTransform( TReal32(aAVal), TReal32(aBVal), TReal32(aCVal),
                         TReal32(aDVal), TReal32(aEVal), TReal32(aFVal) );
        MSvgTransformList*  trList;
        ((CSvgElementImpl*)aElementHandle)->GetTransform( trList );
        trList->ReplaceItem( matrix, 0 );

        // we need to update the CTM on this element and below it....

        ((CSvgElementImpl*)aElementHandle)->UpdateCTM();
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ClearFrameBuffer(CFbsBitmap* aFrameBuffer, TUint32 aClearingColor)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ClearFrameBuffer(CFbsBitmap* /* aFrameBuffer */,
                                                        TUint32 /* aClearingColor */,
                                                        TInt /* aEngine */ )
{
}


// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl:: SetFrameBuffer(CFbsBitmap* aRenderBuffer)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl:: SetFrameBuffer(CFbsBitmap* aRenderBuffer, TInt aEngine)
    {
    if( ChooseEngine ( aEngine ) )
        {
        iSvgEngine->SetSVGEngineState(ESVGEnginePaused);
        TRAPD( error1,iSvgEngine->SetGdiContextL(aRenderBuffer));

         if( error1 == KErrNone )
            {

            TRAPD( error2, iSvgEngine->InitializeEngineL()); // set the viewport and viewbox
            if (error2 != KErrNone)
                {
                #ifdef _DEBUG
                _LIT(KSetFrameBufferFailed,
                    "CSvgEngineInterfaceImpl::SetFrameBuffer: InitializeEngineL error.");
                RDebug::Print(KSetFrameBufferFailed);
                #endif //_DEBUG
                }

            if( iSvgEngine->Document() )
                {
                if( iSvgEngine->Document()->IsAnimationFile() ) // If it is animation file
                    {
                    iSvgEngine->SetSVGEngineState(ESVGEngineRunning);
                    }
                else
                    {
                    // If static file, just redraw it
                    TSvgTimerEvent tmev( 0 );
                    TRAPD( error2,iSvgEngine->ProcessEventL( 
                        iSvgEngine->Document(), &tmev ));
                    if (error2 != KErrNone)
                        {
                        #ifdef _DEBUG
                        _LIT(KRedrawFailed, "CSvgEngineInterfaceImpl::SetFrameBuffer: Redraw error.");
                        RDebug::Print(KRedrawFailed);
                        #endif //_DEBUG
                        }
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::SearchForText( const TDesC& aSearchString,
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::SearchForText( const TDesC& aSearchString,
                                                       RPointerArray<MRect>& aBoundingBoxes,
                                                       RArray<TPtrC>& aTexts,
                                                       RArray<TInt>& aElementIds,
                                                       TBool aCaseSensitive,
                                                       TInt aEngine )
{
    if ( !ChooseEngine ( aEngine ) )
    {
        return EFalse;
    }

    TInt initialCount = aBoundingBoxes.Count();

    iSvgEngine->FindBBoxesForRotatedText( aSearchString, aBoundingBoxes, aTexts, aElementIds, aCaseSensitive );

    return aBoundingBoxes.Count() > initialCount;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::AddTextListener( MSvgTextListener* aListener)
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::AddTextListener( MSvgTextListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->AddTextListener( aListener );
    }

    return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::RemoveTextListener( MSvgTextListener* aListener)
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::RemoveTextListener( MSvgTextListener* aListener, TInt aEngine )
{
     if ( ChooseEngine ( aEngine ) )
     {
        iSvgEngine->RemoveTextListener( aListener );
     }

     return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::SetTextForTextElement( TInt aTextElementId, TDesC& aXmlString )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::SetTextForTextElement( TInt aTextElementId, TDesC& aXmlString )
{
    TRAPD(err, ((CSvgTextElementImpl*)aTextElementId)->SetTextL(aXmlString));
    if ( err != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineInterfaceImpl::SetTextForTextElement failed");
        #endif
        return EFalse;
        }

    return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::GetTextForTextElement( TInt aTextElementId, TBool& aEditable, TDes& aXmlString )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::GetTextForTextElement( TInt aTextElementId, TBool& aEditable, TDes& aXmlString )
{
    if(aXmlString.MaxLength() < ((CSvgTextElementImpl*)aTextElementId)->GetText().Length() )
        {
        aXmlString = ((CSvgTextElementImpl*)aTextElementId)->GetText().Left( aXmlString.MaxLength() );
        }
    else
        {
        aXmlString = ((CSvgTextElementImpl*)aTextElementId)->GetText();
        }

    aEditable = ((CSvgTextElementImpl*)aTextElementId)->IsEditable();

    return aEditable;
}


// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::AddTextAreaListener( MSvgTextAreaListener* aListener)
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::AddTextAreaListener( MSvgTextAreaListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->AddTextAreaListener( aListener );
    }

    return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::RemoveTextAreaListener( MSvgTextAreaListener* aListener)
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::RemoveTextAreaListener( MSvgTextAreaListener* aListener, TInt aEngine )
{
     if ( ChooseEngine ( aEngine ) )
     {
        iSvgEngine->RemoveTextAreaListener( aListener );
     }

     return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::SetTextForTextAreaElement( TInt aTextAreaElementId, TDesC& aXmlString )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::SetTextForTextAreaElement( TInt aTextAreaElementId, TDesC& aXmlString )
{
    TRAPD(err, ((CSvgTextAreaElementImpl*)aTextAreaElementId)->SetTextL(aXmlString));

    if ( err != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineInterfaceImpl::SetTextForTextElement failed");
        #endif
        return EFalse;
        }

    return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::GetTextForTextAreaElement( TInt aTextAreaElementId, TBool& aEditable, TDes& aXmlString )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::GetTextForTextAreaElement( TInt aTextAreaElementId, TBool& aEditable, TDes& aXmlString )
{
    ((CSvgTextAreaElementImpl*)aTextAreaElementId)->GetText( aXmlString );

    aEditable = ((CSvgTextAreaElementImpl*)aTextAreaElementId)->IsEditable();

    return aEditable;
}


// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AddHyperlinkListener( MSvgHyperlinkListener* aListener )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AddHyperlinkListener( MSvgHyperlinkListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->AddHyperlinkListener( aListener );
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RemoveHyperlinkListener( MSvgHyperlinkListener* aListener )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RemoveHyperlinkListener( MSvgHyperlinkListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->RemoveHyperlinkListener( aListener );
    }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AddAnimationListener( MSvgAnimationListener* aListener )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AddAnimationListener( MSvgAnimationListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->AddAnimationListener( aListener );
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RemoveAnimationListener( MSvgAnimationListener* aListener )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RemoveAnimationListener( MSvgAnimationListener* aListener, TInt aEngine )
{
    if ( ChooseEngine ( aEngine ) )
    {
        iSvgEngine->RemoveAnimationListener( aListener );
    }
}

// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportWidth( CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportWidth( CSvgDocumentImpl* aDocumentHandle )
{
    TFloatFixPt width = 0;
    CSvgSvgElementImpl*  el = (CSvgSvgElementImpl*)(aDocumentHandle->RootElement());

    if ( el != NULL )
    {
    	width = el->Width( );
    	    
    	// this means the values were provided in % and the Engine is not initialized.
      if((TInt)(width) == -1)
        {
        return (TInt)(el->iWidthInUserCoordinate);
        }
   	}


    return (TInt)width;
}

// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportHeight( CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportHeight( CSvgDocumentImpl* aDocumentHandle )
{
    TFloatFixPt height = 0;
    CSvgSvgElementImpl*  el = (CSvgSvgElementImpl*)(aDocumentHandle->RootElement());

    if ( el != NULL )
    {
    	height = el->Height( );
    	
    	// this means the values were provided in % and the Engine is not initialized.
        if((TInt)(height) == -1)
        {
        return (TInt)(el->iHeightInUserCoordinate);
        }
   	}
    
    return (TInt)height;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetViewportWidth( CSvgDocumentImpl* aDocumentHandle, TInt aWidth )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetViewportWidth( CSvgDocumentImpl* aDocumentHandle, TInt aWidth )
{
    CSvgSvgElementImpl*  el = (CSvgSvgElementImpl*)( aDocumentHandle->RootElement());
    if ( el != NULL )
        {
        TRAP_IGNORE( el->SetAttributeFloatL( KAtrWidth, (TFloatFixPt)aWidth ) );
        el->iWidthInPercentage = EFalse;
        }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetViewportHeight( CSvgDocumentImpl* aDocumentHandle, TInt aHeight )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetViewportHeight( CSvgDocumentImpl* aDocumentHandle, TInt aHeight )
{
    CSvgSvgElementImpl*  el = (CSvgSvgElementImpl*)( aDocumentHandle->RootElement());
    if ( el != NULL )
        {
        TRAP_IGNORE( el->SetAttributeFloatL( KAtrHeight, (TFloatFixPt)aHeight ) );
        el->iHeightInPercentage = EFalse;
        }
}

// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportUnits( CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetViewportUnits( CSvgDocumentImpl* aDocumentHandle )
{
    if ( 
    ((CSvgSvgElementImpl*)aDocumentHandle->RootElement() )->iWidthInPercentage 
    && ((CSvgSvgElementImpl*)aDocumentHandle->RootElement() )->iHeightInPercentage 
    )
    {
        return SvgUnitPercent;
    }
    else
    {
        return SvgUnitPixels;
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SvgBeginElementAt(CXmlElementImpl* aElementHandle , TUint32 aTimeInMilliSeconds, CSvgDocumentImpl* aDocumentHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SvgBeginElementAt( 
        CXmlElementImpl* aElementHandle , 
        TUint32 aTimeInMilliSeconds, 
        CSvgDocumentImpl* /* aDocumentHandle */ )
    {
    if  ( ((CSvgElementImpl*)aElementHandle)->IsAnimatedElement() )
        {
        // do this only when this is  an animation element.
		CSvgAnimationBase*  lAnimationBasePtr = (CSvgAnimationBase*)aElementHandle;
		TRAPD( err, lAnimationBasePtr->SetBeginByEventL( lAnimationBasePtr,
			aTimeInMilliSeconds, ETrue ));

        lAnimationBasePtr->SaveBeginTimeToList(aTimeInMilliSeconds);
        if ( err != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgEngineInterfaceImpl::SvgBeginElementAt SetBeginByEventL() failed");
            #endif
            }
        }
    }

//---------------------------------------------------------------------------=
//Old begine element at...
//---------------------------------------------------------------------------=
/*EXPORT_C void CSvgEngineInterfaceImpl::SvgBeginElementAt(CXmlElementImpl* aElementHandle , TUint32 aTimeInMilliSeconds)
{
  if  (   aElementHandle->ElemID() >= KSvgAnimElemsStartIndex &&
                        aElementHandle->ElemID() <= KSvgAnimElemsEndIndex
        )
        {
        // do this only when this is  an animation element.
        CSvgAnimationBase*  lAnimationBasePtr = (CSvgAnimationBase*)aElementHandle;
        lAnimationBasePtr->SetBeginTime(aTimeInMilliSeconds);
        // what if the animation has already started. reflect the change in the end Time.
        TUint32 lSimpleDuration = lAnimationBasePtr->SimpleDuration();
        if( lSimpleDuration )
            {
            lAnimationBasePtr->SetEndTime(lSimpleDuration);
            }
        // should we change the animation status.
        // if the animation has finished then it will not start here again
        // Reset the status and reset the repeatCount.

        }
}*/

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SvgEndElementAt(CXmlElementImpl* aElementHandle , TUint32 aTimeInMilliSeconds, CSvgDocumentImpl* aDocumentHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SvgEndElementAt( CXmlElementImpl* aElementHandle , TUint32 aTimeInMilliSeconds, CSvgDocumentImpl*  aDocumentHandle )
{
  if  ( ((CSvgElementImpl*)aElementHandle)->IsAnimatedElement() )
        {
        // do this only when this is  an animation element.
        CSvgAnimationBase*  lAnimationBasePtr = (CSvgAnimationBase*)aElementHandle;
        lAnimationBasePtr->SetEndByEvent(lAnimationBasePtr, aTimeInMilliSeconds);

        //this may be more efficient then SetMediaTime below because it only does this element.
        //lAnimationBasePtr->ReInitializeAnimation();

        SvgSetMediaTime(aDocumentHandle, SvgGetMediaTime(aDocumentHandle) );

        }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SvgSetMediaTime(CSvgDocumentImpl* aDocumentHandle ,TUint32 aTimeInMilliSeconds)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SvgSetMediaTime(CSvgDocumentImpl* aDocumentHandle ,TUint32 aTimeInMilliSeconds)
{
    if(aDocumentHandle && aDocumentHandle->Engine())
    {
        aDocumentHandle->Engine()->SetMediaTime(aTimeInMilliSeconds);
        //Reset the document only if time is 0. This change is just to meet JSR request.
        if(aTimeInMilliSeconds == 0 && !aDocumentHandle->Engine()->iTimer)
        {
        	TSvgTimerEvent timeEvent( aTimeInMilliSeconds );
	       	aDocumentHandle->Reset( &timeEvent );	
        }
    }
    else if (aDocumentHandle && !aDocumentHandle->Engine())
 	{
	//JSR226 Change in question
    		#ifdef _DEBUG
    		RDebug::Printf("CSvgEngineInterfaceImpl::SvgSetMediaTime--doc doesnt know its engine!");
    		#endif

    		//creating a fake engine for the JSR 226 to be able to get valid numbers
    		//before rendering.  Don't try this at home kids

			CSvgEngineImpl* fakeEngine = NULL;

    		TRAP_IGNORE( fakeEngine  = CSvgEngineImpl::NewL(iSvgBitmapFontProvider) );

    		if (fakeEngine == NULL)
    		{
    			return;
    		}

    		aDocumentHandle->SetEngine(fakeEngine );
    		fakeEngine ->SetDocument(aDocumentHandle);

    		aDocumentHandle->Engine()->SetMediaTime(aTimeInMilliSeconds);

    		TSvgTimerEvent timeEvent( aTimeInMilliSeconds );

        	//AJD I think this needs to be down in the event handler where it sets the time...
        	fakeEngine ->Document()->Reset( &timeEvent );

        	TRAPD( error3, fakeEngine ->ProcessEventL( 
        	    aDocumentHandle, &timeEvent, EFalse ));

            if( error3 != KErrNone)
            {
                // Error Processing
            }

        	fakeEngine ->UpdateCTM(aDocumentHandle);

    		aDocumentHandle->SetEngine(NULL);
    		fakeEngine ->SetDocument(NULL);

    		delete fakeEngine ;

    		//store off time on document because this was with a fake engine
    		//because at this point it hasn't rendered
    		aDocumentHandle->iTimeForJSR226 = aTimeInMilliSeconds;
    }
    else
    {
    		#ifdef _DEBUG
    		RDebug::Printf("CSvgEngineInterfaceImpl::SvgSetMediaTime-- engine or doc null!");
    		#endif
    }
}

// --------------------------------------------------------------------------
// EXPORT_C TReal32 CSvgEngineInterfaceImpl::SvgGetMediaTime(CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C TReal32 CSvgEngineInterfaceImpl::SvgGetMediaTime(CSvgDocumentImpl* aDocumentHandle ) __SOFTFP
{
    // returns time in  milliseconds
    if(aDocumentHandle && aDocumentHandle->Engine())
    {
        return (TReal32)(aDocumentHandle->Engine()->CurrentTIme());
    }
    else
    {
    	if (aDocumentHandle && !aDocumentHandle->Engine())
    	{
    		#ifdef _DEBUG
    		RDebug::Printf("CSvgEngineInterfaceImpl::SvgGetMediaTime--doc doesnt know its engine!");
    		#endif

    		return ( (TReal32)(aDocumentHandle->iTimeForJSR226) );
    	}
    	else
    	{
    		#ifdef _DEBUG
    		RDebug::Printf("CSvgEngineInterfaceImpl::SvgGetMediaTime-- engine or doc null!");
    		#endif
    	}
    }

    return 0;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::SvgHasAnimation(CSvgDocumentImpl* aDocumentHandle )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::SvgHasAnimation(CSvgDocumentImpl* aDocumentHandle )
{
    if (aDocumentHandle)
    {
        return aDocumentHandle->IsAnimationFile();
    }

    return EFalse;
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetFocusedElement(CSvgDocumentImpl* aDocument)
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::GetFocusedElement(CSvgDocumentImpl* aDocument)
{
    if(aDocument)
    {
        return aDocument->GetFocusElement();
    }

        return 0;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetFocusElement(CXmlElementImpl* aElement , CSvgDocumentImpl* aDocument)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetFocusElement(CXmlElementImpl* aElement , CSvgDocumentImpl* aDocument)
{
    if(aDocument)
    {
        aDocument->SetFocusElement(aElement);
    }
}
// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::SVGElementInDom(CSvgDocumentImpl* aDocument,  CXmlElementImpl* aElement)
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::SVGElementInDom(CSvgDocumentImpl* aDocument,  CXmlElementImpl* aElement)
{
    if (aDocument)
    {
        return aDocument->SvgElementPresent((CSvgElementImpl*) aElement);
    }

    return EFalse;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SvgActivateAnimation(CSvgDocumentImpl* aDocument)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SvgActivateAnimation(CSvgDocumentImpl* aDocument)
{
    // this means that if the focus element is an animation element
    // than the animation element should begin. // SVGACTIVATEANIMATION.
    if(aDocument)
        {
        CXmlElementImpl* lCurrentFocusElement= aDocument->GetFocusElement();
        if(lCurrentFocusElement)
            {
            // Get the element ID
            TInt lId = lCurrentFocusElement->ElemID() ;

            // make sure that this is an animation element.
            if( ((CSvgElementImpl*)lCurrentFocusElement)->IsAnimatedElement() )
                {
                // now we have to start the animations
                // get the current media time for the engine and set
                // the animations to begine at this time only.
                SvgBeginElementAt( (CXmlElementImpl*) lCurrentFocusElement , (TUint32)SvgGetMediaTime(aDocument), aDocument);

                //dont need this anymore because it is done in begin element at now
                //CSvgAnimationBase*  lAnimationBasePtr = (CSvgAnimationBase*)lCurrentFocusElement;
                //lAnimationBasePtr->ReInitializeAnimation();
                }

            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::SVGElementGetUsedElement(CXmlElementImpl* aElement)
// ---------------------------------------------------------------------------
EXPORT_C CXmlElementImpl* CSvgEngineInterfaceImpl::SVGElementGetUsedElement(CXmlElementImpl* aElement)
{
    // this is for the reason that id's are not cloned.
    // should there be a check for the element type for the parent element.
    if(aElement && aElement->ParentNode())
        {
        if(aElement->OwnerDocument())
            {
            // get the xlink:href of the use element
            TPtrC refElemId =  ( ((CSvgElementImpl*)(aElement->ParentNode()))->Href() );

            return (CXmlElementImpl*)((CSvgDocumentImpl*)(aElement->OwnerDocument()))->GetElementById(refElemId);
            }
        }
    return 0;

}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::IsRemoveable( CSvgElementImpl* hElement )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::IsRemoveable( CSvgElementImpl* aElement, TBool aCheckSibling )
{
    if ( !aElement )
        {
        return ETrue;
        }

    if ( aElement->Id() )
        {
        return EFalse;
        }

    CSvgElementImpl* child = (CSvgElementImpl*)aElement->FirstChild();
    if ( !IsRemoveable( child, ETrue ) )
        {
        return EFalse;
        }

    if (aCheckSibling)
    {
    	return IsRemoveable( (CSvgElementImpl*)aElement->NextSibling(), ETrue );
	}

	return ETrue;
}

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::IsElementActive( CSvgElementImpl* hElement )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::IsElementActive( CSvgElementImpl* hElement )
{
    if (hElement)
    {
        return  (hElement->IsAnimating());
    }

    return EFalse;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AddToEventReceiverList(CXmlElementImpl* aElement, const TUint8 aEventMask)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AddToEventReceiverList(CXmlElementImpl* aElement, const TUint8 aEventMask)
{
    TRAPD(error,((CSvgDocumentImpl*)(aElement->OwnerDocument()))->AddToEventReceiverListL((CSvgElementImpl*)aElement, aEventMask));
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KAddToEventReceiverListFailed,
            "CSvgEngineInterfaceImpl::AddToEventReceiverList: AddToEventReceiverList error.");
        RDebug::Print(KAddToEventReceiverListFailed);
        #endif //_DEBUG
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RemoveFromEventReceiverList(CXmlElementImpl* aElement)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RemoveFromEventReceiverList(CXmlElementImpl* aElement)
{
    if ( aElement )
    {
        ((CSvgDocumentImpl*)(aElement->OwnerDocument()))->RemoveFromEventReceiverList((CSvgElementImpl*)aElement );
    }
}
// --------------------------------------------------------------------------
// EXPORT_C int CSvgEngineInterfaceImpl::DispatchMouseEventsAt(CSvgDocumentImpl* aDocumentHandle, TInt aX, TInt aY)
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::DispatchMouseEventsAt(CSvgDocumentImpl* aDocumentHandle, TInt aX, TInt aY, MSvgMouseListener* aListener)
{
    if(aDocumentHandle)
    {
        CSvgEngineImpl *lEngine= aDocumentHandle->Engine();

        TBool fakeEngine  = EFalse;

        if (!lEngine)
        {
        	fakeEngine  = ETrue;
        	//no engine with the document pass in so use a fake one
        	TRAPD(error, lEngine = SvgEngineNewL());
        	if (error != KErrNone)
        	{
        		delete lEngine;
        		return 0;
        	}

        	if ( fakeEngine  && lEngine && aListener )
            {
            	lEngine->AddMouseListener( (MSvgMouseListener*)aListener );
            }

   			aDocumentHandle->SetEngine(lEngine);
   			lEngine->SetDocument(aDocumentHandle);
        }

        TInt topSelectedElement  = 0;

        // Framework generates MouseDown,MouseMove and MouseUp
        // The order needs to be maintained
        topSelectedElement  = MouseDown( aX, aY, (TInt)lEngine);
        MouseMove(aX, aY, (TInt)lEngine);
        MouseUp(aX, aY, (TInt)lEngine);

		//TSize lSize = ContentDimensions( (TInt)lEngine );

		if (fakeEngine )
		{
			DestroyEngine(lEngine);
			aDocumentHandle->SetEngine(NULL) ;
		}

		//this code was to make sure the click was on the screen, but this isn't the right way to do that
		//at all since the screen can be scaled but from negative coordinates...etc.

        //if ( (aX < lSize.iWidth) && (aX > 0 ) && (aY < lSize.iHeight) && (aY > 0))
        //{
             //really should return a handle to the element that was clicked here
             return topSelectedElement ;
        //}
    }

    return 0;
}

//Triggers the Focusin event for the SvgElement passed as a paramenter
EXPORT_C void CSvgEngineInterfaceImpl::DispatchFocusInEvent(CSvgDocumentImpl* aDocumentHandle,CSvgElementImpl *aElement)
{
	 if(aDocumentHandle)
    	 {
       	CSvgEngineImpl *lEngine = aDocumentHandle->Engine();
		TBool fakeEngine  = EFalse;

        	if (!lEngine)
        	{
			lEngine = SvgEngineNewL() ;
		 	fakeEngine = ETrue ;
			//Set Engine for the document
			aDocumentHandle->SetEngine(lEngine );
	  		}
       	//Set document Handle for the engine
       	lEngine->SetDocument(aDocumentHandle);
        //sets the focus as well as the mouse inside the element on which
        //this function is called   	
       	TGfxRectangle2D boundingBox;
       	aElement->GetBBox( boundingBox );
        MouseMove (boundingBox.CenterX() , boundingBox.CenterY());

		if(fakeEngine)
		{
			DestroyEngine(lEngine) ;
			aDocumentHandle->SetEngine(NULL) ;
		}
	}
}

//Triggers the Focusout event for the SvgElement passed as a paramenter
EXPORT_C void CSvgEngineInterfaceImpl::DispatchFocusOutEvent(CSvgDocumentImpl* aDocumentHandle,CSvgElementImpl* /*aElement */)
{
	 if(aDocumentHandle)
    	 {
       	CSvgEngineImpl *lEngine = aDocumentHandle->Engine();
		TBool fakeEngine  = EFalse;

        	if (!lEngine)
        	{
			lEngine = SvgEngineNewL() ;
		 	fakeEngine = ETrue ;
			//Set Engine for the document
			aDocumentHandle->SetEngine(lEngine );
	  		}
       	//Set document Handle for the engine
       	lEngine->SetDocument(aDocumentHandle);

		// Move the mouse outside the screen so that mouse is exitting
		// from all the elements in the DOM tree. 
		// After this, "focus out" internal event is generated by the 
		// element which is in focus. 
		MouseMove( KMINFLOATFIXVALUE, KMINFLOATFIXVALUE );

		if(fakeEngine)
		{
			DestroyEngine(lEngine) ;
			aDocumentHandle->SetEngine(NULL) ;
		}
	}
}


// --------------------------------------------------------------------------
// EXPORT_C CGfxGeneralPath* CSvgEngineInterfaceImpl::SvgCreatePath()
// ---------------------------------------------------------------------------
EXPORT_C CGfxGeneralPath* CSvgEngineInterfaceImpl::SvgCreatePath()
{
    CGfxGeneralPath* path = NULL;
    TRAPD(error , path = CGfxGeneralPath::NewL());
    if (error != KErrNone)
        {
        #ifdef _DEBUG
        _LIT(KCreateCGfxGeneralPathFailed,
            "CSvgEngineInterfaceImpl::SvgCreatePath: Can't create CGfxGeneralPath.");
        RDebug::Print(KCreateCGfxGeneralPathFailed);
        #endif //_DEBUG
        }
    return path;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SvgDestroyPath(CGfxGeneralPath* aPathHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SvgDestroyPath(CGfxGeneralPath* aPathHandle)
{
    if (aPathHandle)
    {
        delete aPathHandle;
    }
}
// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetSegmentCount(CGfxGeneralPath* aPathHandle)
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetSegmentCount(CGfxGeneralPath* aPathHandle)
{
    if(aPathHandle)
    {
        return (aPathHandle)->GetSegmentCount();
    }

    return 0;
}
// --------------------------------------------------------------------------
// EXPORT_C TInt CSvgEngineInterfaceImpl::GetSegmentType(CGfxGeneralPath* aPathHandle, TInt aSegmentIndex)
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgEngineInterfaceImpl::GetSegmentType(CGfxGeneralPath* aPathHandle, TInt aSegmentIndex)
{
    if(aPathHandle)
    {
        return ( aPathHandle)->GetSegmentType(aSegmentIndex);
    }

    return 0; // this should be verified that this is an error code.
}
// --------------------------------------------------------------------------
// EXPORT_C TReal32 CSvgEngineInterfaceImpl::GetSegmentParameter(CGfxGeneralPath* aPathHandle,TInt aSegmentIndex,TInt aSegmentParameterIndex)
// ---------------------------------------------------------------------------
EXPORT_C TReal32 CSvgEngineInterfaceImpl::GetSegmentParameter(CGfxGeneralPath* aPathHandle,TInt aSegmentIndex,TInt aSegmentParameterIndex) __SOFTFP
{

    if(aPathHandle)
        {
        return (aPathHandle)->GetSegmentParameter( aSegmentIndex,  aSegmentParameterIndex);
        }
    return 0; // this should be treated as error.
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ADDMoveTo(CGfxGeneralPath* aPathHandle, TReal32 aX,TReal32 aY)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ADDMoveTo(CGfxGeneralPath* aPathHandle, TReal32 aX,TReal32 aY) __SOFTFP
{
     if(aPathHandle)
        {
        TFloatFixPt lX = TFloatFixPt(aX);
        TFloatFixPt lY = TFloatFixPt(aY);
        TRAPD(error ,aPathHandle->MoveToL( lX, lY, ETrue));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KMoveToFailed,
                "CSvgEngineInterfaceImpl::ADDMoveTo: MoveToL error.");
            RDebug::Print(KMoveToFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ADDLineTo(CGfxGeneralPath* aPathHandle,TReal32 aX,TReal32 aY)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ADDLineTo(CGfxGeneralPath* aPathHandle,TReal32 aX,TReal32 aY) __SOFTFP
{
    if(aPathHandle)
        {
        TFloatFixPt lX = TFloatFixPt(aX);
        TFloatFixPt lY = TFloatFixPt(aY);
        TRAPD(error , aPathHandle->LineToL( lX, lY, ETrue));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KLineToLFailed,
                "CSvgEngineInterfaceImpl::ADDLineTo: LineToL error.");
            RDebug::Print(KLineToLFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ADDQuadTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ADDQuadTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2) __SOFTFP
{
    if(aPathHandle)
        {

        TFloatFixPt lX1 = TFloatFixPt(aX1);
        TFloatFixPt lY1 = TFloatFixPt(aY1);
        TFloatFixPt lX2 = TFloatFixPt(aX2);
        TFloatFixPt lY2 = TFloatFixPt(aY2);
        TRAPD(error ,aPathHandle->QuadToL( lX1, lY1, lX2, lY2, ETrue ));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KQuadToLLFailed,
                "CSvgEngineInterfaceImpl::ADDQuadTo: QuadToL error.");
            RDebug::Print(KQuadToLLFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ADDCurveTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2, TReal32 aX3, TReal32 aY3)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ADDCurveTo(CGfxGeneralPath* aPathHandle,TReal32 aX1,TReal32 aY1,TReal32 aX2,TReal32 aY2, TReal32 aX3, TReal32 aY3) __SOFTFP
{

    if(aPathHandle)
        {

        TFloatFixPt lX1 = TFloatFixPt(aX1);
        TFloatFixPt lY1 = TFloatFixPt(aY1);
        TFloatFixPt lX2 = TFloatFixPt(aX2);
        TFloatFixPt lY2 = TFloatFixPt(aY2);
        TFloatFixPt lX3 = TFloatFixPt(aX3);
        TFloatFixPt lY3 = TFloatFixPt(aY3);

        TRAPD(error ,aPathHandle->CubicToL( lX1,
                                         lY1,
                                         lX2,
                                         lY2,
                                         lX3,
                                         lY3,
                                         ETrue ));
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KCubicToLFailed,
                "CSvgEngineInterfaceImpl::ADDCurveTo: CubicToL error.");
            RDebug::Print(KCubicToLFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ADDCloseTo(CGfxGeneralPath* aPathHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ADDCloseTo(CGfxGeneralPath* aPathHandle)
{
    if(aPathHandle)
        {
        TRAPD(error ,aPathHandle->ClosePathL());
        if (error != KErrNone)
            {
            #ifdef _DEBUG
            _LIT(KClosePathLFailed,
                "CSvgEngineInterfaceImpl::ADDCloseTo: ClosePathL error.");
            RDebug::Print(KClosePathLFailed);
            #endif //_DEBUG
            }
        }
}
// --------------------------------------------------------------------------
// EXPORT_C CGfxGeneralPath* CSvgEngineInterfaceImpl::GetPathAttribute(CXmlElementImpl* aElementHandle, TInt aAttributeId)
// ---------------------------------------------------------------------------
EXPORT_C CGfxGeneralPath* CSvgEngineInterfaceImpl::GetPathAttribute(CXmlElementImpl* aElementHandle, TInt aAttributeId)
{
    if(aElementHandle)
        {

            CGfxGeneralPath* path =
                ((CSvgElementImpl*)aElementHandle)->GetPathAttribute(aAttributeId);

        if( path )
            {
            CGfxGeneralPath*  lClonePath = NULL;
            TInt error = KErrNone;
            TRAP(error,lClonePath = path->CloneL());
            return lClonePath;
            }

        }

        return NULL;
}
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetPathAttribute(CSvgElementImpl* aElementHandle, TInt lSvgAttrId, CGfxGeneralPath* aPathHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetPathAttribute(CSvgElementImpl* aElementHandle, TInt lSvgAttrId, CGfxGeneralPath* aPathHandle)
{
    if(aElementHandle)
        {
        if(aPathHandle)
            {
            CGfxGeneralPath* clone = NULL;
            TInt error = KErrNone;
            TRAP(error,clone = aPathHandle->CloneL());
            aElementHandle->SetPathAttribute(lSvgAttrId, clone);
            }
        }
}

// --------------------------------------------------------------------------
// EXPORT_C TRect  CSvgEngineInterfaceImpl::GetSvgBoundingBox()
// ---------------------------------------------------------------------------
EXPORT_C TRect  CSvgEngineInterfaceImpl::GetSvgBoundingBox( TInt aEngine )
{
    TRect lBBox(TSize(0,0));

    if( ChooseEngine(aEngine) )
        {
        if( iSvgEngine->Document() )
            {
            CSvgSvgElementImpl* lRoot = ( CSvgSvgElementImpl* )
                                    iSvgEngine->Document()->RootElement();
            if(lRoot)
                {
                TGfxRectangle2D     bbox;
                lRoot->GetBBox(bbox );

                TInt lX = (TInt)bbox.iX;
                TInt lY = (TInt)bbox.iY;
                TInt lWidth = (TInt)bbox.iWidth;
                TInt lHeight = (TInt)bbox.iHeight;
                lBBox.SetRect(TPoint(lX,  lY),TSize(lWidth, lHeight));
                }

           }

    }
    return lBBox;
}

//---------------------------------------------------------------------------=
//Getting BoundingBox of Root Element
//---------------------------------------------------------------------------
EXPORT_C TSize CSvgEngineInterfaceImpl::GetUnscaledContentSize( TInt aDomHandle )
    {
    TSize size( 0, 0 );
    if ( IsDomCached( aDomHandle ) )
        {
        CSvgDocumentImpl* document = (CSvgDocumentImpl*)aDomHandle;
        CSvgSvgElementImpl* root = (CSvgSvgElementImpl*)document->RootElement();
        if ( root )
            {
            TGfxRectangle2D bbox;
            root->GetUnscaledBBox( bbox );
            size.iWidth = (TInt)bbox.iWidth;
            size.iHeight = (TInt)bbox.iHeight;
            }
        }
    return size;
    }

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::IsElementVisible( TInt aElementHandle )
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::IsElementVisible( TInt aElementHandle, TInt aEngine )
{
	if (ChooseEngine( aEngine ))
	{
		return iSvgEngine->IsElementVisible( aElementHandle );
	}
	else
	{
    	return EFalse;
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetThumbNailMode( TBool aThumbNailMode )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetThumbNailMode( TBool aThumbNailMode, TInt aEngine )
{
    iIsThumbNailMode = aThumbNailMode;
    if ( ChooseEngine(aEngine) && iSvgEngine->Document() )
    {
        iSvgEngine->Document()->SetThumbNailMode( aThumbNailMode );
    }
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::CustomOption( TBool aThumbNailMode )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::CustomOption(TBool aCustomOption, TInt aEngine  )
    {
    if ( ChooseEngine(aEngine) )
        {
        iSvgEngine->CustomOption(aCustomOption);
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C TReal CSvgEngineInterfaceImpl::FramesPerSecond(TBool aShowOn)
// ---------------------------------------------------------------------------
EXPORT_C TReal CSvgEngineInterfaceImpl::FramesPerSecond(TBool aShowOn, TInt aEngine ) __SOFTFP
    {

    //take the width in here..
    CSvgDocumentImpl* doc = SvgDocument();

    if (aShowOn && ChooseEngine(aEngine) && iSvgEngine->Document() )
    	{
        TReal ret = 0.0;
        TRAPD( error, ret = FramesPerSecondL() );
        if ( error != KErrNone )
        	{
            RDebug::Printf("CSvgEngineInterfaceImpl::FramesPerSecond() Error in FramesPerSecondL");
        	}

        return ret;
    	}
    else
    	{
    	_LIT(fps, "fps");
        if (iSvgEngine && iSvgEngine->Document() && iSvgEngine->Document()->iPerfText && GetElementById(doc, fps))
        	{
            RemoveChild( (CXmlElementImpl*)iSvgEngine->Document()->RootElement(), (CXmlElementImpl*) iSvgEngine->Document()->iPerfText);
            delete iSvgEngine->Document()->iPerfText;
            iSvgEngine->Document()->iPerfText = NULL;
        	}

        if ((iSvgEngine) && ( iSvgEngine->iTimer ))
        	{
            return (iSvgEngine->iTimer->Fps() / 10);
        	}
        else
        	{
            return 0;
        	}
    	}
	}

// --------------------------------------------------------------------------
// TReal CSvgEngineInterfaceImpl::FramesPerSecondL()
// ---------------------------------------------------------------------------
TReal CSvgEngineInterfaceImpl::FramesPerSecondL( TInt aEngine )
    {

    if (!ChooseEngine( aEngine ))
	{
	return 0;
	}

    CSvgDocumentImpl* doc = SvgDocument();

    TSize svgSize = ContentDimensions();
    TBuf<10> fps;
    _LIT(msg, "FPS: ");
    _LIT(FPS, "fps");
    fps.Append(msg);

    fps.AppendNum((iSvgEngine->iTimer->Fps() / 10));

    if (!iSvgEngine->Document()->iPerfText || !(GetElementById(doc, FPS)))
    {
        TRAPD(err, iSvgEngine->Document()->iPerfText = (CSvgElementImpl*)doc->CreateElementL(25));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::FramesPerSecond() Error in CreateElementL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }
        AppendChild((CXmlElementImpl*)doc->RootElement(), (CXmlElementImpl*)iSvgEngine->Document()->iPerfText);
    }

    const TGfxAffineTransform& tmtx  = ((CSvgSvgElementImpl* )iSvgEngine->Document()->RootElement())->GetCTM();
    TGfxPoint2D whPt(svgSize.iWidth,svgSize.iHeight), whDstPt(1,1);
    tmtx.Transform(&whPt,&whDstPt,1);

    //<text x="x" y ="y" font-family="verdana" font-size="6" fill= "grey">Testing 'text' element: </text>
    iSvgEngine->Document()->iPerfText->SetAttributeDesL(KAtrCdata,fps);
    iSvgEngine->Document()->iPerfText->SetAttributeDesL(KAtrId, FPS);
    iSvgEngine->Document()->iPerfText->SetAttributeFloatL(KAtrX, (float)whDstPt.iX * (float).95);
    iSvgEngine->Document()->iPerfText->SetAttributeFloatL(KAtrY, (float)whDstPt.iY * (float).05);
    iSvgEngine->Document()->iPerfText->SetAttributeFloatL(KAtrWidth, ((float).05 * (float)whDstPt.iX));
    iSvgEngine->Document()->iPerfText->SetAttributeFloatL(KAtrHeight,((float).05 * (float)whDstPt.iY));

    _LIT(RED, "red");
    iSvgEngine->Document()->iPerfText->SetPropertyL( KCSS_ATTR_STROKE, RED);
    iSvgEngine->Document()->iPerfText->SetPropertyL( KCSS_ATTR_FILL, RED);

    return (iSvgEngine->iTimer->Fps() / 10);
    }

//---------------------------------------------------------------------------=
////debugging method to draw a rectangle on the screen
//---------------------------------------------------------------------------=
// --------------------------------------------------------------------------
// EXPORT_C CSvgElementImpl* CSvgEngineInterfaceImpl::DrawBox(TRect aRect)
// ---------------------------------------------------------------------------
EXPORT_C CSvgElementImpl* CSvgEngineInterfaceImpl::DrawBox(TRect aRect, TInt aEngine)
{
	if (!ChooseEngine( aEngine ))
	{
	return NULL;
	}

    CSvgElementImpl* lBox = NULL;
    TRAPD(err, lBox = (CSvgElementImpl*)CreateElementL(SvgDocument(), KSvgRectElement));
    if (err)
        {
        _LIT(msg1, "DrawBox error: ");
        _LIT(msg2, "Can't create element");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, err, msg1, msg2);
        return lBox;
        }

    AppendChild((CXmlElementImpl*)iSvgEngine->Document()->RootElement(), (CXmlElementImpl*)lBox);

    if (lBox)
    {
        //<rect fill="none" x="lBbox.iX" y="lBbox.iY" width="iBbox.iWidth" height="iBbox.iHeight" stroke="blue"/>
        //fill
        _LIT(NONE, "none");
        TRAPD(err, lBox->SetPropertyL( KCSS_ATTR_FILL, NONE));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }

        //stroke
        _LIT(BLUE, "blue");
        TRAP(err, lBox->SetPropertyL( KCSS_ATTR_STROKE, BLUE));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }

        //x
        TRAP(err, lBox->SetAttributeFloatL( KAtrX, ((TFloatFixPt)(float) aRect.iTl.iX)));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }

        //y
        TRAP(err, lBox->SetAttributeFloatL( KAtrY, ((TFloatFixPt)(float) aRect.iTl.iY)));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }

        //width
        TRAP(err, lBox->SetAttributeFloatL( KAtrWidth, ((TFloatFixPt)(float) aRect.Width())));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }
        //height
        TRAP(err, lBox->SetAttributeFloatL( KAtrHeight, ((TFloatFixPt)(float) aRect.Height())));
        if (err)
            {
            #ifdef _DEBUG
            _LIT( KErrMsg, "CSvgEngineInterfaceImpl::DrawBox) Error in SetPropertyL" );
            RDebug::Print(KErrMsg);
            #endif //_DEBUG
            }
    }

    return lBox;
}

//---------------------------------------------------------------------------=
//Returns array of pointers to all elements of a certain type or all elements if aElementId = -1
//---------------------------------------------------------------------------=
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::FindAllElements( CSvgElementImpl* aStartElement, TInt aElementId,
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::FindAllElements( CSvgElementImpl* aStartElement, TInt aElementId,
                                      RPointerArray<CSvgElementImpl>& aList, TInt aEngine )
{
    if ( ChooseEngine(aEngine) && iSvgEngine->Document())
    {
        iSvgEngine->Document()->FindAllElements(aStartElement, aElementId, aList);
    }
}

//---------------------------------------------------------------------------=
////DOM Element Traversal
//---------------------------------------------------------------------------=

//---------------------------------------------------------------------------=
//Returns the first child element node of this element. null if this element has no child elements.
//---------------------------------------------------------------------------=
CSvgElementImpl* CSvgEngineInterfaceImpl::FirstElementChild(CSvgElementImpl* aParentElement)
{
    if ( aParentElement )
    {
        return (CSvgElementImpl*)aParentElement->FirstChild();
    }

    return NULL;
}

//---------------------------------------------------------------------------=
//last child element node of this element. null if this element has no child elements.
//---------------------------------------------------------------------------=
CSvgElementImpl* CSvgEngineInterfaceImpl::LastElementChild(CSvgElementImpl* aParentElement)
{
    if ( aParentElement )
    {
        return (CSvgElementImpl*)aParentElement->LastChild();
    }

    return NULL;

}

//---------------------------------------------------------------------------=
//Returns the next sibling element node of this element. null if this element has no element sibling nodes that come after this one in the document tree.
//---------------------------------------------------------------------------=
CSvgElementImpl* CSvgEngineInterfaceImpl::PreviousElementSibling(CSvgElementImpl* aSiblingElement)
{
    if ( aSiblingElement )
    {
        return (CSvgElementImpl*)aSiblingElement->PreviousSibling();
    }

    return NULL;
}

//---------------------------------------------------------------------------=
//previous sibling element node of this element. null if this element has no element sibling nodes that come before this one in the document tree.
//---------------------------------------------------------------------------=
CSvgElementImpl* CSvgEngineInterfaceImpl::NextElementSibling(CSvgElementImpl* aSiblingElement)
{
    if ( aSiblingElement )
    {
        return (CSvgElementImpl*)aSiblingElement->NextSibling();
    }

    return NULL;
}


//---------------------------------------------------------------------------=
// Precondition: bitmap A & B has same dimension and display mode.
// Return ETrue if both bitmaps are the same, bit for bit.
// Used by RenderFrames
//---------------------------------------------------------------------------=
TBool CSvgEngineInterfaceImpl::AreBitmapsTheSame( CFbsBitmap& aBitmapA, CFbsBitmap& aBitmapB )
{

    TInt bitsPerPixel = TDisplayModeUtils::NumDisplayModeBitsPerPixel( aBitmapA.DisplayMode() );
    TSize size = aBitmapA.SizeInPixels();

    // Number of bytes of each scan line
    // This is aligned to a 4 byte(DWORD) boundary for performance reasons.
    TInt scanLineLength = CFbsBitmap::ScanLineLength( size.iWidth, aBitmapA.DisplayMode() );

    aBitmapA.LockHeap();
    aBitmapB.LockHeap();
    TUint32* dataA = aBitmapA.DataAddress();
    TUint32* dataB = aBitmapB.DataAddress();
    aBitmapA.UnlockHeap();
    aBitmapB.UnlockHeap();
    TInt dwords = scanLineLength / 4;
    TInt bytesLeftOver = scanLineLength - ( 4 * dwords );

	TInt sizeHeight = size.iHeight;
	for ( TInt y = 0; y < sizeHeight; y++ )
    {
        // compare 4 bytes are at time
        TInt i = 0;
        for ( ; i < dwords; i++ )
            if ( dataA[i] != dataB[i] )
                return EFalse;

        // compare left over bytes
        if ( bytesLeftOver > 0 )
        {
            TUint8* bytesA = (TUint8*)dataA[i];
            TUint8* bytesB = (TUint8*)dataB[i];
            for ( TInt k = 0; k < bytesLeftOver; k++ )
                if ( bytesA[k] != bytesB[k] )
                    return EFalse;
            // next scanline
            dataA = &dataA[dwords+1];
            dataB = &dataB[dwords+1];
        }
        else
        {
            // next scanline
            dataA = &dataA[dwords];
            dataB = &dataB[dwords];
        }
    }
    return ETrue;
}

//---------------------------------------------------------------------------=
// Create a bitmap with the specified size, colormode.
// Used by RenderFrames
//---------------------------------------------------------------------------=
CFbsBitmap* CSvgEngineInterfaceImpl::CreateBitmapL( TSize aSize, TDisplayMode aColorMode,
                                                   CSvgErrorImpl& aError )
{
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);

    User::LeaveIfError( bitmap->Create( aSize, aColorMode ));

    CleanupStack::Pop(); // bitmap

    aError.SetErrorCode( ESvgNoError );
    return bitmap;
}

//---------------------------------------------------------------------------=
// Generate animation frames
//---------------------------------------------------------------------------=

EXPORT_C MSvgError* CSvgEngineInterfaceImpl::RenderFrames(
                                  TInt aDomHandle,
                                  const TSize& aDimensionOfBitmaps,
                                  TUint aStartTime,
                                  TUint aDuration,
                                  TUint aMillisecondsPerFrame,
                                  TDisplayMode aColorDepth,
                                  TDisplayMode aMaskType,
                                  RPointerArray<CFbsBitmap>& aBitmapFrames,
                                  RPointerArray<CFbsBitmap>& aMaskFrames,
                                  RArray<TUint>& aDelayIntervals,
                                  TBool aRemoveRedundantFrames )
{
    iSvgError->SetErrorCode( ESvgNoError );

    //-----------------------------------------------------
    // Validate parameters
    //-----------------------------------------------------
    if ( aDimensionOfBitmaps.iWidth < 1 || aDimensionOfBitmaps.iHeight < 1 )
        {
        TBuf<100> buf;
        buf.AppendNum( aDimensionOfBitmaps.iWidth );
        _LIT(X, "x");
        buf.Append( X );
        buf.AppendNum( aDimensionOfBitmaps.iHeight );
        _LIT(msg, "RenderFrames: Invalid bitmap dimensions: ");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone, msg, buf );
        return iSvgError;
        }
    if ( aMillisecondsPerFrame == 0 )
        {
        _LIT(msg1, "RenderFrames: Invalid delay per frame: 0");
        _LIT(msg2, "");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone, msg1, msg2);
        return iSvgError;
        }
    if ( !( aMaskType == EGray2 || aMaskType == EGray256 ) )
        {
        _LIT(msg1, "RenderFrames: Unsupported Mask Type.");
        _LIT(msg2, "Must be EGray2 or EGray256");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone, msg1, msg2);
        return iSvgError;
        }

    if ( !IsDomCached( aDomHandle ) )
        {
        _LIT(msg1, "RenderFrames: Invalid Dom Handle.");
        _LIT(msg2, "");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone, msg1, msg2);
        return iSvgError;
        }

    // Create separate svg engine for operation, leaving intact
    // default engine.
    CSvgEngineImpl* svgEngine = NULL;
    TRAPD( engineError, svgEngine = CSvgEngineImpl::NewL( iSvgEngine->iFrameBuffer,
                                                          iSvgEngine->iRequestObserver,
                                                          iSvgEngine->iBitmapFontSpec, iSvgBitmapFontProvider ) );
    CleanupStack::PushL(svgEngine);                                                      
    if ( engineError != KErrNone )
        {
        _LIT(msg1, "RenderFrames: Failed to instantiate SvgEngine.");
        _LIT(msg2, "");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, engineError, msg1, msg2);
        return iSvgError;
        }
	svgEngine->SetBackgroundColor(KBackgroundColor);
    // Initialize engine with document
    CSvgDocumentImpl* document = (CSvgDocumentImpl*)aDomHandle;
    
	if(!document)
	   {
        _LIT(msg1, "RenderFrames: Failed to get document.");
        _LIT(msg2, "");
        CSvgDocumentImpl::PrepareError( *iSvgError, ESvgDocumentNotAvailable, KErrUnknown, msg1, msg2);
        return iSvgError;
        }
		
    // Before changing the engine, if the current engine is valid
    // and it is pointing to this document set the engine's document to NULL
    if (document->Engine() )
        {
        CSvgEngineImpl* lCurEngine = document->Engine();
        lCurEngine->SetDocument( NULL );
        }
	
        
    svgEngine->SetDocument( document );
    document->SetEngine( svgEngine );

    svgEngine->SetIgnoreUpdateScreen( ETrue );

    // Flag to indicate content is an animation
    TBool isAnimationContent = SvgHasAnimation( document );

    CFbsBitmap* bitmap = NULL;
    TBool usePreviousBitmap = EFalse;
    TBool engineInitialized = EFalse;

    for ( TUint time = 0; time <= aDuration; time += aMillisecondsPerFrame )
        {
        //-----------------------------------------------------
        // Create bitmap if needed
        //-----------------------------------------------------
        if ( !usePreviousBitmap )
            {
            TRAPD(err, bitmap = CreateBitmapL( aDimensionOfBitmaps,
                aColorDepth, *iSvgError ));
            if ( err != KErrNone )
                {
                _LIT(msg1, "RenderFrames: Failed to create bitmap.");
                _LIT(msg2, "");
                CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, engineError, msg1, msg2);
                break;
                }
            //-----------------------------------------------------
            // Set the bitmap to render with the dom
            //-----------------------------------------------------
            TRAPD( setGdiError, svgEngine->SetGdiContextL( bitmap ) );
            if ( setGdiError != KErrNone )
               {
               // Delete the bitmap
               delete bitmap;
               bitmap = NULL;
               
               CleanupStack::PopAndDestroy(svgEngine);
                
               if ( setGdiError == KErrNoMemory )
                  {
                  _LIT(msg1, "RenderFrames: Set Bitmap to SVG Engine failed.");
                  _LIT(msg2, "");
                  CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, setGdiError, msg1, msg2);
                  return iSvgError;
                  }
                  
                _LIT(msg1, "RenderFrames: Set Bitmap to SVG Engine failed.");
                _LIT(msg2, "");
                CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, setGdiError, msg1, msg2);
                return iSvgError;
                }
            // Need to initialize after new bitmap is set
            // for engine to set viewbox, etc.
            if ( !engineInitialized )
                {
                TRAPD( initError, svgEngine->InitializeEngineL() );
                if ( initError != KErrNone )
                    {
                    _LIT(msg1, "RenderFrames: Initializing Svg Engine failed.");
                    _LIT(msg2, "");
                    CSvgDocumentImpl::PrepareError(
                        *iSvgError, ESvgUnknown, initError, msg1, msg2);
                    break;
                    }
                engineInitialized = ETrue;
                }
            }

        //-----------------------------------------------------
        // draw to bitmap
        //-----------------------------------------------------
        TSvgTimerEvent timeEvent( aStartTime + time );
        TRAPD( renderError, svgEngine->ProcessEventL( 
            document, &timeEvent ) );
        if ( renderError != KErrNone )
            {
            _LIT(msg1, "RenderFrames: Rendering image failed.");
            _LIT(msg2, "");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, renderError,msg1, msg2);
            break;
            }
        //-----------------------------------------------------
        // compare new bitmap with previous bitmap to combine delay time
        // if images are the same
        //-----------------------------------------------------
        usePreviousBitmap = aRemoveRedundantFrames &&
                            ( aBitmapFrames.Count() > 0 ) &&
                            AreBitmapsTheSame( *bitmap, *aBitmapFrames[aBitmapFrames.Count()-1] );

        //-----------------------------------------------------
        // Use previous bitmap: add delay time
        //-----------------------------------------------------
        if ( usePreviousBitmap )
            {
            TInt index = aDelayIntervals.Count() - 1;
            aDelayIntervals[index] += aMillisecondsPerFrame;
            // In case this is the very last frame and it's the same with previous frame,
            // then destroy it
            if ( time + aMillisecondsPerFrame > aDuration )
                {
                delete bitmap;
                bitmap = NULL;
                }
            continue;
            }
        //-----------------------------------------------------
        // add new bitmap to array: bitmap is different than previous
        //-----------------------------------------------------
        TInt bitmapAppendError = aBitmapFrames.Append( bitmap );
        if ( bitmapAppendError != KErrNone )
            {
            _LIT(msg1, "RenderFrames: Failed to append new bitmap to frames.");
            _LIT(msg2, "");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, bitmapAppendError, msg1, msg2);
            break;
            }

        // Set the bitmap NULL as the ownership is transferred to
        // aBitmapFrames
        bitmap = NULL;

        //-----------------------------------------------------
        // Render mask
        //-----------------------------------------------------
        // Create Mask
        CFbsBitmap* maskBitmap = NULL;
        TRAPD( err, maskBitmap = CreateBitmapL( aDimensionOfBitmaps, aMaskType, *iSvgError ));
        if ( err != KErrNone || maskBitmap == NULL )
            {
            break;
            }
        GenerateMask( maskBitmap );
        TInt appendMaskError = aMaskFrames.Append( maskBitmap );
        if ( appendMaskError != KErrNone )
            {
            _LIT(msg1, "RenderFrames: Failed to append new mask to mask frames.");
            _LIT(msg2, "");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, appendMaskError, msg1, msg2);
            delete maskBitmap;
            break;
            }

        // Set the mask NULL as the ownership is transferred to
        // aMaskFrames
        maskBitmap = NULL;

        //-----------------------------------------------------
        // Append delay interval
        //-----------------------------------------------------
        TUint delay = ( aDuration - time < aMillisecondsPerFrame ) ?
                      aDuration - time : aMillisecondsPerFrame;
        TInt appendIntervalError = aDelayIntervals.Append( delay );
        if ( appendIntervalError != KErrNone )
            {
            _LIT(msg1, "RenderFrames: Failed to append delay interval to list.");
            _LIT(msg2, "");
            CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, appendIntervalError,msg1, msg2);
            break;
            }

        // Only process one frame for non-animation content
        if ( !isAnimationContent )
            {
            // Set the duration for the whole frame for non-animation
            if ( aDelayIntervals.Count() > 0 )
                {
                aDelayIntervals[0] = aDuration;
                }
            break;
            }
        }
    // Cleanup the bitmap
    delete bitmap;
    //remove engine from document it is attached to
    document->SetEngine( NULL );
    // Delete  the engine
    iSvgDocumentPointers.ResetAndDestroy();
    iSvgDocumentPointers.Close();
    CleanupStack::PopAndDestroy(svgEngine);
    return iSvgError;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::AddListener( 
//  const MSvgListener* aListener, 
//  TSvgListenerType aType )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::AddListener( 
        const MSvgListener* aListener, 
        TSvgListenerType aType, 
        TInt aEngine )
    {
    if ( aListener == NULL )
        {
        return;
        }

	ChooseEngine( aEngine );
    
    TInt index = 0;
    // Loading listeners
    switch( aType )
        {
        case ESvgLoadingListener:
            index = iSvgLoadingListeners.Find( 
                    (MSvgLoadingListener*)aListener );
            if (  index == KErrNotFound )
                {
                iSvgLoadingListeners.Append( (MSvgLoadingListener*)aListener );
                }
                break;
        // Mouse listeners
        case ESvgMouseListener:
            if ( iSvgEngine != NULL )
                {
                iSvgEngine->AddMouseListener( (MSvgMouseListener*)aListener );
                }
            break;
        case ESvgTextAreaListener:
            if ( iSvgEngine != NULL )
                {
                iSvgEngine->AddTextAreaListener( 
                        (MSvgTextAreaListener*)aListener );
                }
            break;
        case ESvgTextListener:
            if ( iSvgEngine != NULL )
                {
                iSvgEngine->AddTextListener( (MSvgTextListener*)aListener );
                }
            break;
        case ESvgViewPortListener:
            if ( iSvgEngine != NULL )
                {
                iSvgEngine->AddViewPortListener( 
                        (MSvgViewPortListener*)aListener );
                }
            break;
        case ESvgInteractiveElementListener:
            if ( iSvgEngine != NULL )
                {
                iSvgEngine->AddInteractiveElementListener( 
                        (MSvgInteractiveElementListener*)aListener );
                }
            break;
        
        
        default:
            break;
        }
    }
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::RemoveListener( const MSvgListener* aListener, TSvgListenerType aType )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::RemoveListener( const MSvgListener* aListener, TSvgListenerType aType, TInt aEngine )
    {

    ChooseEngine(aEngine);

    // Loading listeners
    if ( aType == ESvgLoadingListener )
        {
        TInt index = iSvgLoadingListeners.Find( (MSvgLoadingListener*)aListener );
        if ( index != KErrNotFound )
            {
            iSvgLoadingListeners.Remove( index );
            }
        }
    // Mouse listeners
    else if ( aType == ESvgMouseListener )
        {
            if ( iSvgEngine != NULL )
            {
                iSvgEngine->RemoveMouseListener( (MSvgMouseListener*)aListener );
            }
        }
    // Text area listeners
    else if ( aType == ESvgTextAreaListener )
        {
            if ( iSvgEngine != NULL )
            {
                iSvgEngine->RemoveTextAreaListener( (MSvgTextAreaListener*)aListener );
            }
        }
    else if ( aType == ESvgTextListener )
        {
            if ( iSvgEngine != NULL )
            {
                iSvgEngine->RemoveTextListener( (MSvgTextListener*)aListener );
            }

        }
    else if ( aType == ESvgInteractiveElementListener )
        {
        if ( iSvgEngine != NULL )
            {
            iSvgEngine->RemoveInteractiveElementListener( (MSvgInteractiveElementListener*)aListener );
            }
         }
    }
// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::InitSvgStylePropertiesWithNullL( CSvgElementImpl* aElement )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::InitSvgStylePropertiesWithNullL( CSvgElementImpl* aElement )
{
    if ( aElement )
    {
        aElement->InitSvgStylePropertiesWithNullL();
    }
}

//---------------------------------------------------------------------------=
// Initialize engine with <svg> information in attached document.
//---------------------------------------------------------------------------=
EXPORT_C void CSvgEngineInterfaceImpl::InitializeEngine(
                                       CSvgEngineImpl* aEngine,
                                       TBool /*aIsMainThread*/ )
    {
        if ( ChooseEngine( (TInt)aEngine ) )
        {

        	TRAPD( error, iSvgEngine->InitializeEngineL() );
        	if ( error != KErrNone )
            {
            	return;
            }

        	iSvgEngine->SetMediaTime( 0 );
        	// pause is so progressive parsing doesnt animate
        	// before everything is there. This could be removed to
        	// load and animate on the fly
        	iSvgEngine->SetSVGEngineState( ESVGEnginePaused );

    	}
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetDataRetrievalTimeOut( TUint aTimeoutSeconds )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetDataRetrievalTimeOut( TUint /*aTimeoutSeconds*/, TInt /*aEngine*/ )
{
// Function is no more required
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::CancelLoad()
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::CancelLoad( TInt aEngine )
    {
    	if ( ChooseEngine(aEngine) )
    	{
    		if ( iSvgEngine->Document() )
        	{
        	iSvgEngine->Document()->CancelParsing();
       	 	}
    	}
    }

// --------------------------------------------------------------------------
// EXPORT_C TBool CSvgEngineInterfaceImpl::IsLoading()
// ---------------------------------------------------------------------------
EXPORT_C TBool CSvgEngineInterfaceImpl::IsLoading( TInt aEngine )
    {
    	if ( ChooseEngine(aEngine) )
    	{
    		if ( iSvgEngine->Document() )
        	{
        	return iSvgEngine->Document()->IsParsing();
        	}
    	}

    return EFalse;
	}

//---------------------------------------------------------------------------
//Print all of the elements in the DOM tree and most of their attributes (only works in _DEBUG mode)
//---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::PrintAllElements( CSvgDocumentImpl* aDocument )
{
	if (aDocument)
	{
		aDocument->PrintAllElements((CSvgElementImpl*)aDocument->RootElement());
	}
	else if (iSvgEngine && iSvgEngine->Document())
	{
		iSvgEngine->Document()->PrintAllElements((CSvgElementImpl*)	iSvgEngine->Document()->RootElement());
	}
}

//---------------------------------------------------------------------------
//Print all of the styles that the element has access to (only works in _DEBUG mode)
//---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::PrintElementsStyles( CSvgElementImpl* aElement )
{
	if (aElement)
	{
		aElement->PrintStyleProperties();
	}
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::WaitForImages( TBool aBool )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::WaitForImages( TBool /* aBool */, TInt /* aEngine */ )
    {
    // Method is obsolete: image loading is synchronous now
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SetClientWindow( RWindow* /*aWindow*/ )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetClientWindow( RWindow* /*aWindow*/ )
    {

    }

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::SaveSvgDom(TInt aHandle, const TDesC& aFileName)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::SaveSvgDom(TInt /*aHandle*/, const TDesC& aFileName, TInt aEngine)
    {

    	if (!ChooseEngine(aEngine))
    	{
    		return iSvgError;
    	}

    	//UseDom( aHandle, NULL );

    	TRAPD(error, iSvgEngine->SaveSvgL(aFileName));

    	if (error != KErrNone)
    	{
    		#ifdef _DEBUG
    		RDebug::Printf("CSvgEngineInterfaceImpl: SaveSvgL threw an exception while saving");
    		#endif
    	}

   	return iSvgError;
    }

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::SaveSvg( TBool aIsEncodeOn, const TDesC& aFileName, TInt aEngine )
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SaveSvg( TBool /*aIsEncodeOn*/, const TDesC& /*aFileName*/, TInt aEngine )
    {

    	if (!ChooseEngine(aEngine))
    	{

    	}

    	//UseDom( aHandle, NULL );

    	//TRAPD(error, iSvgEngine->SaveSvgL(aFileName));
    }

// --------------------------------------------------------------------------
// EXPORT_C void SetAudioVolume( TInt aPercentage , TInt aEngine );
// --------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::SetAudioVolume( TInt aPercentage ,
        TInt aEngine )
    {
    if ( ChooseEngine(aEngine) )
        {
       	iSvgEngine->SetAudioVolume( aPercentage );
        }
    }

// --------------------------------------------------------------------------
// EXPORT_C void MuteAudioVolume();
// --------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::MuteAudioVolume( TInt aEngine )
    {
    if ( ChooseEngine(aEngine) )
        {
       	iSvgEngine->SetAudioVolume( 0 );
        }
    }

// --------------------------------------------------------------------------
// TInt CSvgEngineInterfaceImpl::ChooseEngine( CSvgEngineImpl* aEngine )
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineInterfaceImpl::ChooseEngine( TInt aEngine )
	{
		if (aEngine)
		{
			iSvgEngine = (CSvgEngineImpl*)aEngine;
			return (CSvgEngineImpl*)aEngine;
		}
		else if (iSvgEngine)
		{
			return iSvgEngine;
		}
		else
		{
			return NULL;
		}
	}

// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::ChooseViewBox(TInt aDomHandle)
// ---------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ChooseViewBoxIfNotSet(TInt aDomHandle)
    {
        if ( IsDomCached( aDomHandle ) )
        {
        CSvgDocumentImpl* document = (CSvgDocumentImpl*)aDomHandle;
        CSvgSvgElementImpl* root = (CSvgSvgElementImpl*)document->RootElement();
        if ( root )
            {
            if(!root->ViewBoxDefined())
                {
                 // If viewbox is missing in the content, the content
                 // rendering in the frame buffer is many time as expected
                 // Some part of the content can get clipped, since content
                 // doesn't scale proportionately to the framebuffer.
                 TGfxRectangle2D bbox;
                 root->GetUnscaledBBox( bbox );
                 TFloatFixPt lDefault= TFloatFixPt(0);
                 if(bbox.iWidth > lDefault && bbox.iHeight > lDefault)
                    {
                    TRAP_IGNORE( root->SetViewBoxL( bbox ) );
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineInterfaceImpl::ConvertBitmapToFileL(CFbsBitmap* aBitmap, const TDesC& aText)
//
// Utility function to dump out the svg engine framebuffer
// ---------------------------------------------------------------------------
void CSvgEngineInterfaceImpl::ConvertBitmapToFileL(CFbsBitmap* aBitmap, const TDesC& /*aText*/)
{
	//set size for bitmap
	//if it is new
	/*TInt aWidth = 200, aHeight = 200;

	aBitmap = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(aBitmap);
	User::LeaveIfError(aBitmap->Create(TSize(aWidth, aHeight), EColor64K));
	*/

	//draw text on bitmap here!!
	/*CFbsBitmapDevice* bitDevice = CFbsBitmapDevice::NewL(aBitmap);
	CFbsBitGc* bitGc;
	User::LeaveIfError(bitDevice->CreateContext(bitGc));

	CleanupStack::Pop(aBitmap);

	bitGc->SetPenColor(KRgbBlack);
	bitGc->UseFont( LatinBold19() ); // a font has to be set otherwise it panics!
	bitGc->SetPenStyle(CGraphicsContext::ESolidPen);
	bitGc->DrawText(aText, TPoint(10, 10));
	*/

	RFs fs;
	User::LeaveIfError( fs.Connect() );

	// constructing encoder
	CImageEncoder* imgEncoder = CImageEncoder::FileNewL( fs, _L("svg_engine_bitmap.bmp"), _L8("image/bmp"),
	CImageEncoder::EOptionAlwaysThread );

	// Following settings are for JPG file format
	// TJpegImageData* imageData = new (ELeave) TJpegImageData;
	// Set some format specific data
	// imageData->iSampleScheme = TJpegImageData::EColor444;
	// imageData->iQualityFactor = 95;

	// Following settings are for BMP file format
	TBmpImageData* imgData = new(ELeave) TBmpImageData;
	imgData->iBitsPerPixel = 24;

	CFrameImageData* lFrameImageData = CFrameImageData::NewL();

	// frameData - ownership passed to lFrameImageData after AppendImageData
	User::LeaveIfError(lFrameImageData->AppendImageData(imgData)); // or append imageData for jpg file

	// Do the convert
	TRequestStatus iRequesStatus;
	imgEncoder->Convert( &iRequesStatus, *aBitmap, lFrameImageData);
	User::WaitForRequest( iRequesStatus );

	fs.Close();
	delete lFrameImageData;
	//delete aBitmap;
	delete imgEncoder;
	//delete bitGc;
	//delete bitDevice;
}

// --------------------------------------------------------------------------
// EXPORT_C void CSvgEngineInterfaceImpl::ResetContext( TInt aEngine )
// --------------------------------------------------------------------------
EXPORT_C void CSvgEngineInterfaceImpl::ResetContext( TInt aEngine )
{
	if ( ChooseEngine((TInt)aEngine) )
        {
        iSvgEngine->ResetContext();
        }	
}


EXPORT_C void CSvgEngineInterfaceImpl::Start(MSvgError*& aError, const TDesC8* aHeaderData ,
        CSvgEngineImpl* aEngine )
    {
    if (aHeaderData)
            {
            iSvgEngine->SetBitmapHeader((const TDesC16 *)aHeaderData);
            }
    Start(aError, aEngine);
    }

EXPORT_C void CSvgEngineInterfaceImpl::Start( const TDesC8* aHeaderData, CSvgEngineImpl* aEngine,
        TBool aIsMainThread)
    {

    if (aHeaderData)
        {
        iSvgEngine->SetBitmapHeader((const TDesC16 *)aHeaderData);
        }
    Start(aEngine, aIsMainThread);
    }
// Get the NVG-TLV data
// --------------------------------------------------------------------------
// EXPORT_C TPtr CSvgEngineInterfaceImpl::TLVEncodedData()
// ---------------------------------------------------------------------------
EXPORT_C const TPtrC8 CSvgEngineInterfaceImpl::TLVEncodedData() const
    {
    return (iSvgEngine->TLVEncodedData());
    }
//NGA
// --------------------------------------------------------------------------
// EXPORT_C MSvgError* CSvgEngineInterfaceImpl::UseDom(TInt aHandle, TSize aRenderBufferSize,TDisplayMode aRenderDisplayMode,TDisplayMode aMaskDisplayMode,TInt aEngine = NULL)
// ---------------------------------------------------------------------------
EXPORT_C MSvgError* CSvgEngineInterfaceImpl::UseDom(TInt aHandle,CFbsBitmap* aRenderBuffer, CFbsBitmap* aMaskBuffer,TSize aRenderBufferSize,TDisplayMode aRenderDisplayMode,TDisplayMode aMaskDisplayMode,TInt aEngine)
    {
      // Clear Error
      iSvgError->SetErrorCode( ESvgNoError );

      // Check for engine: Should not occur
      if( !ChooseEngine( aEngine ) )
          {
          _LIT(KEngineFailed, "SvgEngine Internal Failure: SvgEngineImpl not present.");
          CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                          KEngineFailed, KNullDesC );
          return iSvgError;
          }

      // Check for valid DOM
      if( !IsDomCached( aHandle ) )
          {
          _LIT(KEngineFailed, "Invalid DOM, Use Prepare() to create DOM");
          CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                          KEngineFailed, KNullDesC );
          return iSvgError;
          }

      iTotalRotation = 0;

      iFileIsLoaded = EFalse;

      // Switch GDI context
      TRAPD( gdiError, iSvgEngine->SetGdiContextL( aRenderBuffer, aMaskBuffer,aRenderBufferSize,aRenderDisplayMode,aMaskDisplayMode ) );//Modify this setgdi context
      if ( gdiError != KErrNone )
          {
          if ( gdiError == KErrNoMemory )
              {
              _LIT(KEngineGDIFailed, "SVG Engine Set Gdi Context Failed");
              CSvgDocumentImpl::PrepareError( *iSvgError, ESvgNoMemory, KErrNoMemory,
                                               KEngineGDIFailed, KNullDesC );
              return iSvgError;
              }
          _LIT(KEngineGDIFailed, "SVG Engine Set Gdi Context Failed");
              CSvgDocumentImpl::PrepareError( *iSvgError, ESvgUnknown, KErrNone,
                                               KEngineGDIFailed, KNullDesC );
          return iSvgError;
          }

      CSvgDocumentImpl* document = (CSvgDocumentImpl*)aHandle;

      CSvgDocumentImpl* lExistDoc = iSvgEngine->Document();

      if (lExistDoc != document)
          {
          //this is a new document so swap it out...
          iSvgEngine->SetDocument( document );
          document->SetEngine( iSvgEngine );
          document->ReInitialize();
          }

      if (lExistDoc == iSvgLoadedDocument)
      {
          DestroyDocument(iSvgLoadedDocument);
          iSvgLoadedDocument = NULL;
      }

      // Check for thumbnail restrictions
      if ( !iSvgEngine->PassesThumbNailRestriction() )
          {
          _LIT( KThumbNailRestriction, "Frame buffer is larger than allowable size for thumbnail mode." );
          CSvgDocumentImpl::PrepareError( *iSvgError, ESvgThumbNailRestriction, KErrNone,
                                           KThumbNailRestriction, KNullDesC );
          return iSvgError;
          }

      iFileIsLoaded = ETrue;

      return iSvgError;
      }
