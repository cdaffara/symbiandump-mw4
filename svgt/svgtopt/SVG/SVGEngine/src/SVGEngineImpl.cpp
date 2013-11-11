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




#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <utf.h>
#include <s32mem.h>

#include "SVGDOMImplementationImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSvgElementImpl.h"
#include "SVGUseElementImpl.h"
#include "SVGAnimationElementImpl.h"
#include "SVGTextElementImpl.h"

#include "SVGAudioElementImpl.h"


#include "Gfx2dGcOpenVG.h"

#include "SVGSchemaData.h"
#include "SVGPathElementImpl.h"
#include "SVGAnimationBase.h"
#include "SVGElementTimeControl.h"

#include "SVGRequestObserver.h"
#include "SVGHyperlinkListener.h"
#include "SVGListener.h"
#include "SVGAnimationListener.h"
#include "SVGAElementImpl.h"
#include "SVGTextAreaElementImpl.h"

#include "SVGTimer.h"
#include "SVGEventHandler.h"
#include "SVGEngineImpl.h"
#include "SVGErrorImpl.h"

#include "SVGFloatCssValueImpl.h"
#include "SVGTimeContainer.h"
#include "SVGMediaAnimationElementImpl.h"

#include <svgtbitmap.h>
// Constants
// length of </text>
const TInt KClosingTextTagLength = 7;
// Length of </textArea>
const TInt KClosingTextAreaTagLength = 11;
// Length of <
const TInt KClosingBracesLength = 1;
// Number of chars to be converted at a time to Unicode
const TInt KMaxConversionChars = 20;
_LIT(KClosingTextTag,"</text>");
_LIT(KClosingTextAreaTag,"</textArea>");
_LIT(KClosingBraces,">");

// ---------------------------------------------------------------------------
// Two phase constructor for this class
// JSR 226 API
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewL(CSvgBitmapFontProvider *aSvgBitmapFontProvider)
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl();
    CleanupStack::PushL( self );
    self->ConstructL(aSvgBitmapFontProvider);
    CleanupStack::Pop();
    return self;
    }


// --------------------------------------------------------------------------
// CSvgEngineImpl* CSvgEngineImpl::NewLC()
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewLC(CSvgBitmapFontProvider *aSvgBitmapFontProvider)
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl();
    CleanupStack::PushL( self );
    self->ConstructL(aSvgBitmapFontProvider);
    return self;
    }

// --------------------------------------------------------------------------
// CSvgEngineImpl::CSvgEngineImpl() : iTextAreaHandle( NULL ),
// ---------------------------------------------------------------------------
CSvgEngineImpl::CSvgEngineImpl() : iTextAreaHandle( NULL ),
                                   iRequestObserver( NULL ),
                                   iFrameBuffer( NULL ),
                                   iFrameBufferSize(TSize(0,0)),
                                   iFontHashMap ( NULL ),
                                   iSvgDocument( NULL ),
                                   iBackgroundColor(0x00000000),
                                   iShowDebugInfo( EFalse ),
                                   iSvgEngineState(ESVGEngineNotStarted),
                                   iTimeoutSeconds( 0 ),
                                   iRenderQuality(2), // Set To Rendering quality "high"
                                   iCustomOption( ETrue ),
                                   iFrameBufferOverridesViewport( EFalse ),
                                   iClientDefinedViewPort(EFalse)
                                   
    {
    }


// --------------------------------------------------------------------------
// void CSvgEngineImpl::ConstructL()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ConstructL(CSvgBitmapFontProvider *aSvgBitmapFontProvider)
    {
    iGfxContext = NULL;

	iSvgBitmapFontProvider = aSvgBitmapFontProvider ; 
    // create CSvgErrorImpl object
    iSvgError = CSvgErrorImpl::NewL();

    iFontHashMap = CSvgFontHashMap::NewL();

    SetIgnoreUpdateScreen( EFalse );

   // The iCustomOption need to be initialized for JSR

	iCustomOption = ETrue;

    }



// ---------------------------------------------------------------------------
// Two phase constructor for this class
// Accepts a frame buffer and a MSvgRequestObserver object from the client
// Buffer is used for rasterization of SVG content
// Observer object, if provided, is used for notifying the client on updates
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewL( CFbsBitmap* aFrameBuffer,
                                      MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider )
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl( aFrameBuffer,
                                                             aReqObserver );
    CleanupStack::PushL( self );
    self->ConstructL(aFontSpec, aSvgBitmapFontProvider);
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// Two phase constructor for this class
// Accepts a frame buffer and a MSvgRequestObserver object from the client
// Buffer is used for rasterization of SVG content
// Observer object, if provided, is used for notifying the client on updates
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewLC( CFbsBitmap* aFrameBuffer,
                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider )
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl( aFrameBuffer, aReqObserver );
    CleanupStack::PushL( self );
    self->ConstructL(aFontSpec, aSvgBitmapFontProvider);

    return self;
    }

// ---------------------------------------------------------------------------
// Symbian style private method that is used to construct heap objects
// Builds the Graphics device objects with the client's frame buffer info
// Builds the event handler object
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ConstructL( TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider)
    {
    iBitmapFontSpec = aFontSpec;
// iGfxContext is created through CSvgEngineInterfaceImpl::ConstructL
   // iGfxContext = CGfx2dGcVGR::NewL( iFrameBuffer->SizeInPixels(), iBitmapFontSpec );
    iSvgBitmapFontProvider = aSvgBitmapFontProvider ; 

    iGfxContext = CGfx2dGcOpenVG::NewL( iFrameBuffer->SizeInPixels(), iBitmapFontSpec, iSvgBitmapFontProvider );
    // create CSvgErrorImpl object
    iSvgError = CSvgErrorImpl::NewL();
    iFontHashMap = CSvgFontHashMap::NewL();
    iSvgNames = new (ELeave) CDesC16ArrayFlat(5);
    }

// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewL( CFbsBitmap* aFrameBuffer,
                                      MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider ,SVGRendererId aRendererType)
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl( aFrameBuffer,
                                                             aReqObserver );
    CleanupStack::PushL( self );
    self->ConstructL(aFontSpec, aSvgBitmapFontProvider,aRendererType);
    CleanupStack::Pop();
	
    return self;
    }


// ---------------------------------------------------------------------------
// Two phase constructor for this class
// Accepts a frame buffer and a MSvgRequestObserver object from the client
// Buffer is used for rasterization of SVG content
// Observer object, if provided, is used for notifying the client on updates.
//This contains the renderer selector parameter
// ---------------------------------------------------------------------------
CSvgEngineImpl* CSvgEngineImpl::NewLC( CFbsBitmap* aFrameBuffer,
                                       MSvgRequestObserver* aReqObserver, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType )
    {
    CSvgEngineImpl* self    = new ( ELeave ) CSvgEngineImpl( aFrameBuffer, aReqObserver );
    CleanupStack::PushL( self );
    self->ConstructL(aFontSpec, aSvgBitmapFontProvider,aRendererType);

    return self;
    }

// ---------------------------------------------------------------------------
// Symbian style private method that is used to construct heap objects
// Builds the Graphics device objects with the client's frame buffer info
// Builds the event handler object.
//This contains the renderer selector parameter NGA
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ConstructL( TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType)
    {
    iBitmapFontSpec = aFontSpec;
// iGfxContext is created through CSvgEngineInterfaceImpl::ConstructL
   // iGfxContext = CGfx2dGcVGR::NewL( iFrameBuffer->SizeInPixels(), iBitmapFontSpec );
    iSvgBitmapFontProvider = aSvgBitmapFontProvider ; 

    iGfxContext = CGfx2dGcOpenVG::NewL( iFrameBufferSize, iBitmapFontSpec, iSvgBitmapFontProvider ,aRendererType);
    // create CSvgErrorImpl object
    iSvgError = CSvgErrorImpl::NewL();
    iFontHashMap = CSvgFontHashMap::NewL();
    iSvgNames = new (ELeave) CDesC16ArrayFlat(5);
    }


// ---------------------------------------------------------------------------
// Private constructor
// Initializes private attributes
// ---------------------------------------------------------------------------
CSvgEngineImpl::CSvgEngineImpl( CFbsBitmap* aFrameBuffer,
                                MSvgRequestObserver* aReqObserver ) : iTextAreaHandle( NULL ),
                                                                      iRequestObserver( aReqObserver ),
                                                                      iFrameBuffer( aFrameBuffer ),
                                                                      iFrameBufferSize(TSize(0,0)),
                                                                      iFontHashMap ( NULL),
                                                                      iSvgDocument( NULL ),
                                                                      iBackgroundColor(0x00000000),
                                                                      iShowDebugInfo( EFalse ),
                                                                      iSvgEngineState(ESVGEngineNotStarted),
                                                                      iCustomOption( ETrue ),
                                                                      iFrameBufferOverridesViewport( EFalse )
                                                                      

    {
        if(aFrameBuffer && aFrameBuffer->Handle()>0)
            {
                iFrameBufferSize = aFrameBuffer->SizeInPixels();
            }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgEngineImpl::~CSvgEngineImpl()
    {
    Destroy();

    if ( iFontHashMap )
    {
		delete iFontHashMap;
		iFontHashMap = NULL;
    }

	iSvgTextBoundingBoxes.ResetAndDestroy();
	iSvgTextBoundingBoxes.Close();

    if ( iGfxContext )
        {
        delete iGfxContext;
        iGfxContext = NULL;
        }
    if ( iSvgError )
        {
        delete iSvgError;
        iSvgError = NULL;
        }
    if (iTextAreaHandle)
    {
    delete iTextAreaHandle;
    iTextAreaHandle = NULL;
    }

    iTextAreaListeners.Close();
    iTextListeners.Close();
    iHyperlinkListeners.Close();
    iAnimationListeners.Close();
    iSvgMouseListeners.Close();
    iInteractiveElementListeners.Close();
    delete iSvgNames;
    }

// ---------------------------------------------------------------------------
// Draws aRootElement and calls itself for children.
// Handles switch element differently, as it only draws one of its children.
// ---------------------------------------------------------------------------

void CSvgEngineImpl::DrawElementsL( CSvgElementImpl* aRootElement)
    {
    if ( aRootElement != NULL)
        {
        TInt32 displayValue = 0;
        //  while we have next elements
        CSvgElementImpl* newElement = aRootElement;
        while( newElement != NULL )
            {
		    if(newElement->GetAttributeIntL( KCSS_ATTR_DISPLAY, displayValue ) == KErrNoAttribute)
		        {
		        newElement = ( CSvgElementImpl* ) newElement->FirstChild();
		        while ( newElement != NULL )
		            {
		            DrawElementsL( newElement );
		            newElement = ( CSvgElementImpl * ) newElement->NextSibling();
		            }
		            return;
		         }

            if(displayValue != KDisplayEnumNone) // is it a hidden element
                {
                CCssValue*  CssValue = NULL;
                TReal32 opacity = 1;

                // check visisbility
                TInt32  visbilityValue = 0;
                TInt visibility = newElement->GetAttributeIntL( KCSS_ATTR_VISIBILITY, visbilityValue );

                newElement->FindProperty( KCSS_ATTR_GROUP_OPACITY, CssValue, newElement );
                if ( CssValue )
                    {
                    opacity = ((CFloatCssValueImpl*)CssValue)->Value();
                    }

                if( opacity != 1 && !newElement->IsInherited( KCSS_ATTR_GROUP_OPACITY ) )
                    {
                    iGfxContext->BindToImageL();
                    // Group opacity
                    if ( newElement->ElemID() == KSvgGElement && newElement->FirstChild() )
                        {
                        // recurse with right context.
                        DrawElementsL( ( CSvgElementImpl* )newElement->FirstChild());
                        }
                    // Element opacity
                    else
                        {
                        TBool canDraw = ETrue;
                        if( (visibility == KErrNone) && (visbilityValue == 0) )
                            {
                            canDraw = newElement->DrawL( iGfxContext, NULL ) ;
                             }
                        if( canDraw && newElement->FirstChild() )
                            {
                            DrawElementsL( ( CSvgElementImpl* )newElement->FirstChild() );
                            }
                        }

                    // Blend opacity context buffer with parent's gfx context buffer
                    iGfxContext->UnbindFromImageL( opacity );
                    }
                else // no opacity or is inherited from above....
                    {
                    TBool canDraw = ETrue;
                    if( (visibility == KErrNone) && (visbilityValue == 0) )
                        {
                        canDraw = newElement->DrawL(iGfxContext, NULL );
                        }
                    if( canDraw && newElement->FirstChild() )
                        {
                        DrawElementsL( ( CSvgElementImpl* )newElement->FirstChild());
                        }
                    }
				}
            newElement = ( CSvgElementImpl * )newElement->NextSibling();

            }// end while....
        }
    }

//--------------------------------------------------------------------
//CFbsBitmap* CSvgEngineImpl::CreateOpacityFrameBufferL()
//--------------------------------------------------------------------
CFbsBitmap* CSvgEngineImpl::CreateOpacityFrameBufferL()
{
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();

    if ( iFrameBuffer != NULL )
        {
/*NGA reverse*/if ( bitmap->Create( iFrameBufferSize, EColor16MU ) != KErrNone )
            {
            delete bitmap;
            bitmap = NULL;
            }
        }
    return bitmap;
    }

// ---------------------------------------------------------------------------
// Copy 32-bit buffer
// ---------------------------------------------------------------------------

void CSvgEngineImpl::CopyBuffer( TUint32* aSrc, TUint32* aDest, const TSize aSize )
    {
    if ( aSrc && aDest )
        {
        Mem::Copy( aDest, aSrc, aSize.iWidth * aSize.iHeight * sizeof( TUint32 ) );
        }
    }

// ---------------------------------------------------------------------------
// Positions the root element and calls itself (recursively) for all the
// descendant elements
// CTM stands for 'Current Transformation Matrix'.
// ---------------------------------------------------------------------------
void CSvgEngineImpl::UpdateCTM(CSvgDocumentImpl* aSvgDocument)
    {

    if (aSvgDocument)
    {
        ((CSvgElementImpl*)aSvgDocument->RootElement())->UpdateCTM();
    }


    }

// ---------------------------------------------------------------------------
// Initializes the SVG Engine primarily with width and height informtion
// This is based on the attribute specifications for the root 'svg' element
// ---------------------------------------------------------------------------
void CSvgEngineImpl::InitializeEngineL()
    {
    // VIEWBOX
    // If x, y, w, h of outmost svg not defined, then put default values: 0 0 100% 100%
    // return;
    // Set locale, so that there is no thousands separator

    if( !iSvgDocument )
        return;

    iSvgDocument->iInitialDrawFlag = ETrue;
    if(!((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iIsWidthSet)
    {
        ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iIsWidthSet = ETrue;
    }
    if(!((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iIsHeightSet)
    {
        ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iIsHeightSet = ETrue;
    }

    TLocale locale; // locale object
    locale.SetCurrencyTriadsAllowed( EFalse ); // change setting
    TSize   lSize   = GetSize();
    iSvgDocument->iReqExReqFtrSysLTested = EFalse;
    if ( iSvgDocument->RootElement() != NULL &&
         ((CSvgElementImpl*)iSvgDocument->RootElement())->ElemID() == KSvgSvgElement )

        {
        // Scale width & height to fit to screen size
        CSvgSvgElementImpl*     el      = ( CSvgSvgElementImpl* )
                                          (iSvgDocument->RootElement());
        TFloatFixPt                  width, height;
        TInt                    widthInt, heightInt;
        TFloatFixPt scrnW( lSize.iWidth ), scrnH( lSize.iHeight );
        TFloatFixPt                  zero    ( 0 );
        
        if ( iFrameBufferOverridesViewport )
        {
        		//special case for Aknicon
    				//the don't care what the contents viewport is they just want it to scale to the framebuffer's size
        		width = scrnW;
        		height= scrnH;
        }
        else
        {
            //bitmap from the client overrides width height specified in svg
      
            if(iViewPortListener != NULL)
            {
                TInt tempWidth=0,tempHeight=0;
                iViewPortListener->GetViewPort(
                ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iWidthInUserCoordinate, 
                ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iHeightInUserCoordinate, 
                ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iWidthInPercentage, 
                ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iHeightInPercentage,
                tempWidth,
                tempHeight);
            
                width = (TFloatFixPt) tempWidth;
                height = (TFloatFixPt) tempHeight;
                
                //used in SetWindowViewportTrans of svgSvgElementImpl
                iClientDefinedViewPort = ETrue;
                     
            }
        else
            {
        
                TReal32 per = 100;
                if(((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iWidthInPercentage)
                {   
                    width = ( TFloatFixPt ) (( lSize.iWidth/per ) * ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iWidthInUserCoordinate);
                }
                else
                {   
                    width  = (TFloatFixPt)  (((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iWidthInUserCoordinate);
                }
        
                if(((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iHeightInPercentage)
                {
                    height = ( TFloatFixPt ) (( lSize.iHeight/per ) * ((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iHeightInUserCoordinate);
                }
                else
                {
                    height  = (TFloatFixPt)  (((CSvgSvgElementImpl *)(iSvgDocument->RootElement()))->iHeightInUserCoordinate);
            }
        
        //default to XmidYmid_meet
        TSvgPreserveAspectAlignType lPreserverAspectAlignment = ESvgPreserveAspectRatio_XmidYmid;
  			TSvgMeetOrSliceType lMeetOrSlice = ESvgMeetOrSlice_Meet;
  		
  		MSvgPreserveAspectRatio* aPreserveAspectRatio;
        el->GetPreserveAspectRatio(aPreserveAspectRatio);
        
        if (aPreserveAspectRatio)
        {
        	lPreserverAspectAlignment = aPreserveAspectRatio->GetAlign();
        	lMeetOrSlice = aPreserveAspectRatio->GetMeetOrSlice();
        	
        	//el->SetPreserveAspectRatioL(  lPreserverAspectAlignment, lMeetOrSlice);
        	if( lPreserverAspectAlignment == ESvgPreserveAspectRatio_None && lMeetOrSlice == ESvgMeetOrSlice_Meet )
        	{
            width = scrnW;
            height = scrnH;
        	}
        }
            
        if( lMeetOrSlice == ESvgMeetOrSlice_Meet && !(lPreserverAspectAlignment == ESvgPreserveAspectRatio_None) )
            {
            widthInt = ( TInt ) width;
            heightInt = ( TInt ) height;

            // to avoid division by zero.
            TInt    rh  = lSize.iWidth ;
            if (widthInt != 0)
                {
                rh  = heightInt* lSize.iWidth / widthInt;
                }

            if ( rh <= lSize.iHeight )
                {
                width = scrnW;
                height = TFloatFixPt( rh );
                }
            else
                {
                // to avoid division by zero.
                if (heightInt != 0)
                    {
                    width = TFloatFixPt( ((TReal32)(widthInt * lSize.iHeight)) / heightInt );
                    }
                height = scrnH;
                }
            }
        else if( lMeetOrSlice == ESvgMeetOrSlice_Slice )
            {
            widthInt = ( TInt ) width;
            heightInt = ( TInt ) height;

            // to avoid division by zero.
            TInt    rh  = lSize.iWidth ;
            if (widthInt != 0)
                {
                rh  = heightInt* lSize.iWidth / widthInt;
                }

            if ( rh <= lSize.iHeight )
                {
                // to avoid division by zero.
                if (heightInt != 0)
                    {
                    width = TFloatFixPt( ((TReal32)(widthInt * lSize.iHeight)) / heightInt );
                    }
                height = scrnH;

                }
            else
                {
                width = scrnW;
                height = TFloatFixPt( rh );
                }
            }
        else
            {
            }
            }//Normal viewer| end of else
        }

        if ( ( width <= zero ) || ( height <= zero ) ) //.
        {
            width = 0;
            height = 0;
            return;
        }
        // Set initial viewport
        el->SetAttributeFloatL( KAtrX, zero );
        el->SetAttributeFloatL( KAtrY, zero );
        el->SetAttributeFloatL( KAtrWidth, width );
        el->SetAttributeFloatL( KAtrHeight, height );
        }
    }





// ---------------------------------------------------------------------------
// This method is called after the SVG file is read and all elements are
// constructed
// It also looks for rendering properties such as Anti Aliasing at this
// point
// It calibrates the system for rasterization quality based on contents
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::StartFrameLoopL()
    {
    if( iRequestObserver != NULL )
        {
        if(iSvgDocument != NULL && iSvgDocument->iEventHandler != NULL)
            {
            iRequestObserver->UpdatePresentation(
                         iSvgDocument->iEventHandler->AnimationElementsCount());
            }
        }
    if(iSvgDocument != NULL)
        {
        if ( iSvgDocument->RootElement() )
            {
            InitializeEngineL(); // set the viewport and viewbox
            return ETrue;
            }
        }
    return EFalse;
    }
// ---------------------------------------------------------------------------
// UpdatePresentation()
// ---------------------------------------------------------------------------
void  CSvgEngineImpl::UpdatePresentation(const TInt32& aParam)
    {
    if( iRequestObserver != NULL )
        {
        iRequestObserver->UpdatePresentation (aParam);
        }
    }
    
    
//---------------------------------------------------------------------------
// SetRenderingQuality
//
// @param : aRenderQuality
// The function is called from JSR226 API. Specific to M2G/JSR226 only.
//---------------------------------------------------------------------------
void  CSvgEngineImpl::SetRenderingQuality( const TInt32 aRenderQuality)
{
	// Bydefault iRenderQuality is set to 2 i.e. Rendering Quality high.
	
	if( iRenderQuality != aRenderQuality )
	{
		iRenderQuality = aRenderQuality;
		
		if( iGfxContext )
		{
			iGfxContext->SetAntialiasingMode( iRenderQuality );
		}
	}
}


// ---------------------------------------------------------------------------
// Invalidate the current raster (and frame buffer) and
// update the buffer with new  raster.
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RedrawL()
    {
    // Clear the frame buffer
    if( iGfxContext && iSvgDocument)
        {
        // Set Clipping rectangle
        CSvgSvgElementImpl* el  = ( CSvgSvgElementImpl* ) (iSvgDocument->RootElement());
        if ( !el )
            {
            return; // No DOM tree, nothing to draw.
            }
            
            
        
     	// If viewBox has been set and width or height is zero
     	// Do not render the content    
        TGfxRectangle2D viewBoxRect;    
        if(el->GetViewBox(viewBoxRect))
            {
                if(viewBoxRect.iWidth == TFloatFixPt(0) || 
                    viewBoxRect.iHeight == TFloatFixPt(0))
                    {
                    return;
                    }
            }
           
        iGfxContext->SetupContextL();
        iGfxContext->Clear(iBackgroundColor);
        iGfxContext->SetFillOpacity( 1.0f );
        iGfxContext->SetStrokeOpacity( 1.0f );

        TFloatFixPt  width, height;
        el->GetAttributeFloat( KAtrWidth, width );
        el->GetAttributeFloat( KAtrHeight, height );
        TGfxRectangle2D clipRect    ( 0, 0, width, height );
        iGfxContext->SetClip( clipRect );
        iClipRect = clipRect;

        UpdateCTM(iSvgDocument);

        iSvgDocument->iIsRendering = ETrue;
        DrawElementsL( (CSvgElementImpl*)iSvgDocument->RootElement());
        iSvgDocument->iIsRendering = EFalse;

        iGfxContext->Flush();

        // Get the redering result onto CFbsBitmap.
        if(iFrameBufferSize.iWidth > 0)
            {
            if(iIsTargetRenderingEnabled) 
                { // M2G
                iGfxContext->UpdateFramebufferL( iTargetBitmapBuffer, iTargetMaskBuffer,iFrameBufferSize,
                        iTargetBitmapBuffer->DisplayMode(),iTargetMaskBuffer->DisplayMode());
                }
            else 
                iGfxContext->UpdateFramebufferL( iFrameBuffer, iMask,iFrameBufferSize,iRenderDspMode,
                        iMaskDspMode );
            }
        else
            {
            if(iIsTargetRenderingEnabled)  //M2G
                iGfxContext->UpdateFramebufferL( iTargetBitmapBuffer, iTargetMaskBuffer );
            else
                iGfxContext->UpdateFramebufferL( iFrameBuffer, iMask );

            }

        if ( !iIgnoreUpdateScreen && iRequestObserver != NULL )
            {
            iRequestObserver->UpdateScreen();
            }
        if ( iSvgDocument->iInitSortList )
            {
            iSvgDocument->SortEventList();
            }
        else
            {
            iSvgDocument->iInitSortList=EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// Delete the objects associated with the last loaded SVG file
// ---------------------------------------------------------------------------
void CSvgEngineImpl::Destroy()
    {
    //if ( iSvgDocument )    
    //    {
        // Stop timer and reset time
    //    iSvgDocument->TimeContainer()->UserStop();
    //    iSvgDocument->TimeContainer()->UserResetTime();
    //    }
    }


// ---------------------------------------------------------------------------
// Set the URI information
// ---------------------------------------------------------------------------
void CSvgEngineImpl::LinkRequest( const TDesC& aUri )
    {
    iLinkUri.Set( aUri );
    }
// ---------------------------------------------------------------------------
// Set the Link:Show information
// ---------------------------------------------------------------------------


void CSvgEngineImpl::LinkRequestWithShow(const TDesC& aUri ,const TDesC& aLinkShow )
    {
    iLinkUri.Set( aUri );
    iLinkShow.Set( aLinkShow );
    }

// ---------------------------------------------------------------------------
// Initiate the process of getting an svg font from the client
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle )
    {
		if ( iRequestObserver == NULL )
        {
        return EFalse;
        }
    else
        {
        if (iRequestObserver->FetchFont( aUri, aSession, aFileHandle ) == KErrNone)
        {
            return ETrue;
        }
            return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Initiate the process of getting an embedded image from the client
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle )
    {
    if ( iRequestObserver == NULL )
        {
        return EFalse;
        }
    else
        {
        if (iRequestObserver->FetchImage( aUri, aSession, aFileHandle ) == KErrNone)
        {
            return ETrue;
        }
            return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Accessor for Graphics Context object
// ---------------------------------------------------------------------------
CGfx2dGc* CSvgEngineImpl::GraphicsContext()
    {
    return iGfxContext;
    }

// ---------------------------------------------------------------------------
// Accessor for the Engine's current time
// ---------------------------------------------------------------------------
TInt32 CSvgEngineImpl::CurrentTIme()
    {
    if ( iSvgDocument )
        {
        return iSvgDocument->CurrentTime();        
        }
    return KErrGeneral;
    }


// --------------------------------------------------------------------------
// void CSvgEngineImpl::ImageLoadingCompleted( TInt /* aErrorStatus */ )
// ---------------------------------------------------------------------------
// ImageLoadingObserver interface
void CSvgEngineImpl::ImageLoadingCompleted( TInt /* aErrorStatus */ )
{
    TRAP_IGNORE(RedrawL());
    //aErrorStatus = 0;
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::UpdatePath( CGfxGeneralPath* hPath, CSvgElementImpl* hElement )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::UpdatePath( CGfxGeneralPath* hPath, CSvgElementImpl* hElement )
{
    if ((hElement != NULL) && (hElement->ElemID() == KSvgPathElement))
    {
        TRAPD(error,hElement->SetAttributePathL(KAtrData, hPath));
        if ( error != KErrNone )
           {
            // Error Processing
            return;
           }
    }

}

// ---------------------------------------------------------------------------
// Accessor for the document object for the currently loaded SVG source
// ---------------------------------------------------------------------------
CSvgDocumentImpl* CSvgEngineImpl::Document()
    {
    return iSvgDocument;
    }

// ---------------------------------------------------------------------------
// Restrict Thumbnail documents
// Do not render to frame buffer when is in thumbnail mode and
// frame buffer size width or height is larger than 80x80.
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::PassesThumbNailRestriction()
{
    if ( iSvgDocument && iFrameBuffer && iSvgDocument->IsThumbNailOnly() )
    {
        TSize size = iFrameBuffer->SizeInPixels();
        if ( size.iWidth > 80 || size.iHeight > 80 )
        {
           return EFalse;
        }
    }
    return ETrue;
}

//***************************************************************
// Event handling
// ---------------------------------------------------------------------------
// Initiate the process of handling an event from the client for the
// SVG source
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ProcessEventL( CSvgDocumentImpl* aSvgDocument, 
    MSvgEvent* aEvent, TBool aRedraw )
    {
    // Check for Thumbnail restrictions
    if ( !PassesThumbNailRestriction() )
    {
        return;
    }

    // Timer event
    //if(!aEvent || !iGfxContext)
    if(!aEvent)
    return;
    if ( aEvent->EventType() == ESvgEngineEventTimer )
        {
        // Update current time
        if ( aSvgDocument )
            {
            aSvgDocument->SetCurrentTime( 
                ( ( MSvgTimerEvent * ) aEvent )->Time() );
            }
        }

    if( aSvgDocument &&  aSvgDocument->iEventHandler)
        {
        if (( aSvgDocument->iEventHandler->ProcessEventL( aEvent ) ||
              aSvgDocument->iInitialDrawFlag) && aEvent->EventType() == ESvgEngineEventTimer )
        {

      if(aSvgDocument->iInitSortList)
            {
            aSvgDocument->SortEventList();
            aSvgDocument->iInitSortList = EFalse;
            }
        aSvgDocument->iInitialDrawFlag = EFalse;
        aSvgDocument->iEventHandler->DoAnimProcL(aEvent);

        if (aRedraw)
            {
        	// Redraw is performed for iSvgDocument
            RedrawL();
            }
        }
        else
            {
        // this is to keep the dom in its final state.
            if(aEvent->EventType() == ESvgEngineEventTimer)
                {
                aSvgDocument->iEventHandler->DoAnimProcL(aEvent);
                }

            }

    // Process Link
    if ( aEvent->EventMask() == KSvgEventMaskExternalUI )
            {
            if ( iLinkUri.Length() > 0 )
                {
                DoHyperLinkingL();
                // reset link description
                iLinkUri.Set( NullString );
                }
            }
        }

// Show FPS debug info. note. only write data to STI port
#ifdef _DEBUG

    if ( aEvent->EventType() == ESvgEngineInternalEvent )
        {
        MSvgInternalEvent*  evt = ( MSvgInternalEvent* ) aEvent;
        if (evt->SvgEvent() == ESvgEventEndEvent)
            {
            iAnimationState = KAnimFinished;
            return;
            }
        else
        if (evt->SvgEvent() == ESvgEventBeginEvent)
            {
            iAnimationState = KAnimActive;
            return;
            }

        }

    // time display for debug
    if ( iShowDebugInfo && iGfxContext && aEvent->EventType() == ESvgEngineEventTimer
        && iAnimationState != KAnimFinished /*&& iTimer*/)
        {
        TInt fps = iSvgDocument->TimeContainer()->UserFps() / 10; //iTimer->Fps() / 10;
        _LIT(msg, "\n%3d.%d\n");
        RDebug::Print(msg, fps, iSvgDocument->TimeContainer()->UserFps()
             - fps * 10 );

        }
#endif //_DEBUG

    }


// ---------------------------------------------------------------------------
// This method is for future extension, in which an external script engine
// could be used to evaluate a script description.
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::ScriptCall( const TDesC& aScript,
                                  CSvgElementImpl* aCallerElement )
    {
    if ( iRequestObserver == NULL )
        {
        return EFalse;
        }
    else
        {
        return iRequestObserver->ScriptCall( aScript, aCallerElement );
        }
    }


//***************************************************************
//
// ---------------------------------------------------------------------------
// Obtain the longest duration for an animation given document
// NULL will assume the attached document.
// ---------------------------------------------------------------------------
TUint32 CSvgEngineImpl::Duration( CSvgDocumentImpl* aDocument )
    {
    if ( aDocument == NULL )
        {
        if ( iSvgDocument == NULL )
            {
            return 0;
            }
        return iSvgDocument->AnimationDuration();
        }
    return aDocument->AnimationDuration();
    }

//*******************************************************/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::SwitchDebugInfo()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SwitchDebugInfo(TBool debug)
    {
    iShowDebugInfo = debug;
    }

void CSvgEngineImpl::CustomOption(TBool aCustomOption )
    {
        iCustomOption = aCustomOption;
        delete iGfxContext;
        iGfxContext = NULL;
        
/*NGA reverse*/  TRAP_IGNORE( iGfxContext = CGfx2dGcOpenVG::NewL( iFrameBufferSize, iBitmapFontSpec, iSvgBitmapFontProvider ));            
            
           
    }




// ---------------------------------------------------------------------------
// Initiate the process of opening and reading an embedded hyperlink
// ---------------------------------------------------------------------------
void CSvgEngineImpl::DoHyperLinkingL()
    {

    if( iSvgDocument == NULL )
        return;

    // Not full support of XPointer

    _LIT( KTmpHttp, "http:" );
    _LIT( KTmpFile, "file:" );
    _LIT( KTmpHash, "#" );
    _LIT( KTmpSvgView, "SvgView" );
    if ( iLinkUri.Left( 1 ) == KTmpHash )
        {
        // internal linking
        // Extract id
        TLex    lex ( iLinkUri );
        lex.Inc();  // skip '#'
        lex.Mark();
        while ( !lex.Eos() && lex.Peek().IsAlpha() )
            lex.Inc();
        TPtrC   targetId    = lex.MarkedToken();
        if ( targetId == KTmpSvgView )
            {
            // need to parse next token = {viewBoxSpec| preserveAspectRatioSpec
            // |transformSpec|zoomAndPanSpec|viewTargetSpec}

            // Not yet supported
            }
        else
            {

            CSvgElementImpl*targetElement   = ( CSvgElementImpl* )
                                              iSvgDocument->GetElementById( targetId );
            if ( targetElement == NULL )
                return;
            
            if  (   targetElement->IsAnimatedElement( ) )
                {
                // start animation
                ( ( CSvgAnimationBase * ) targetElement )->BeginElementL();
                }
            else
                {
                }
            }
        }
    else if ( iLinkUri.Length() >= 5 )
        {
        if ( iLinkUri.Left( 5 ) == KTmpHttp || iLinkUri.Left( 5 ) == KTmpFile )
            {
            // notify observer of desire to follow http link
            NotifyHyperlinkActivated( iLinkUri );
            }
        else
            {
            // notify observer of desire to follow link
            if ( iLinkShow.Length() > 0 )
                NotifyHyperlinkActivatedWithShow( iLinkUri, iLinkShow );
            else
                NotifyHyperlinkActivated( iLinkUri );
            }
        }
    else if ( iLinkUri.Length() > 0 )
        {
        // notify observer of desire to follow link
        if ( iLinkShow.Length() > 0 )
            NotifyHyperlinkActivatedWithShow( iLinkUri, iLinkShow );
        else
            NotifyHyperlinkActivated( iLinkUri );
        }
    }

// ---------------------------------------------------------------------------
// Dump a completed raster to the off screen buffer provided by the
// client
// ---------------------------------------------------------------------------
TInt CSvgEngineImpl::RenderFileToBuffer( const TDesC8& aSvgData,
                                           CFbsBitmap* aFrameBuffer,
                                           CFbsBitmap* aMask,
                                           TBool aPreserveAspectRatio )
    {
    if ( !iGfxContext )
        {
        return EFalse;
        }

    CFbsBitmap* OrgFrameBuffer;
    CFbsBitmap* OrgMask;
	OrgFrameBuffer = iFrameBuffer;
	OrgMask = iMask;
	CSvgDocumentImpl* lsvgorgdoc = iSvgDocument;
	// We have moved all the leaving functions into RenderFileL 
	TRAPD(err,RenderFileL(aSvgData,aFrameBuffer,aMask,aPreserveAspectRatio));
	// No matter what...whether the RenderFileL returns properly or leave we need to set the 
	//GDI context back to the original frame buffer and mask... and set back the engine to
	//how it was before all this happened..
	
    SetDocument(lsvgorgdoc);
    lsvgorgdoc->SetEngine(this);
	// the setGDIContextL error is a very serious error and if it occurs then 
	// there is no way of recovery. 
	TRAP_IGNORE(SetGdiContextL(OrgFrameBuffer,OrgMask));
	//So we are propagating the more probable error of the two which is the leaving of 
	// RenderFileL.
    return err;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Helper leaving function for the function RenderFileToBuffer.
// The actual rendering happens in this function..while the calling function uses this to
// render the file.
// ---------------------------------------------------------------------------
    

void CSvgEngineImpl::RenderFileL( const TDesC8& aSvgData,
                                           CFbsBitmap* aFrameBuffer,
                                           CFbsBitmap* aMask,
                                           TBool aPreserveAspectRatio)
   
{
    CFont *lFont = iGfxContext->Font();
    TFontSpec lFontSpec = lFont->FontSpecInTwips();
		
    TSize imageSize;
    imageSize = aFrameBuffer->SizeInPixels();
	//iFrameBuffer = aFrameBuffer;

    SetGdiContextL(aFrameBuffer, aMask);
    CSvgDocumentImpl* lSvgDocument = CSvgDocumentImpl::NewLC(iSvgBitmapFontProvider);
    SetDocument(lSvgDocument);
    lSvgDocument->SetEngine(this);

    // fileHandle is valid from client
    lSvgDocument->Load( aSvgData, *iSvgError );
    if ( iSvgError->HasError() && !iSvgError->IsWarning() )
        {
        CleanupStack::PopAndDestroy(1);
        return;
        }

    StartFrameLoopL();
    CSvgSvgElementImpl* lRoot = (CSvgSvgElementImpl*)lSvgDocument->RootElement();
    _LIT( KPreserveAspectRatio, "preserveAspectRatio" );

    if(lRoot)
        {
        if ( aPreserveAspectRatio )
            {
            _LIT( KXMidYMid, "xMidYMid" );
            lRoot->SetAttributeL( KPreserveAspectRatio, KXMidYMid );
            lRoot->SetWidth( imageSize.iWidth );
            lRoot->SetHeight( imageSize.iHeight );
            }
        else {
            _LIT( KNone, "none" );
            lRoot->SetAttributeL( KPreserveAspectRatio, KNone );
            lRoot->SetWidth( imageSize.iWidth );
            lRoot->SetHeight( imageSize.iHeight );
            }
        RedrawL();
        }
    CleanupStack::PopAndDestroy( 1 );
}
    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Generate a mask bitmap from alpha channel of the framebuffer.
// ---------------------------------------------------------------------------
void CSvgEngineImpl::GenerateMask(CFbsBitmap* aMask)
    {
    if ( iGfxContext )
        {
        iGfxContext->GenerateMask( aMask );
        }
    }

void CSvgEngineImpl::GenerateMask(CSvgtBitmap* aMask)
    {
    if ( iGfxContext )
        {
        iGfxContext->GenerateMask( aMask );
        }
    }
//
// ---------------------------------------------------------------------------
// set background color
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SetBackgroundColor(TUint32 aRGBA8888Color)
    {
    iBackgroundColor = aRGBA8888Color;
    }

//
// ---------------------------------------------------------------------------
// Get Size of render buffer
// ---------------------------------------------------------------------------
TSize CSvgEngineImpl::GetSize()
    {
    	  return iFrameBufferSize;
    	  

        
    }


//
// ---------------------------------------------------------------------------
// Return SVG Engine State
// ---------------------------------------------------------------------------

TSvgEngineState CSvgEngineImpl::SVGEngineState()
    {
    return iSvgEngineState;
    }

//
// ---------------------------------------------------------------------------
// Set SVG Engine State
// ---------------------------------------------------------------------------

void CSvgEngineImpl::SetSVGEngineState(TSvgEngineState aState)
    {
    iSvgEngineState = aState;
    if( iSvgEngineState == ESVGEnginePaused )
        {
        if ( iSvgDocument )
        	{
            iSvgDocument->TimeContainer()->UserPause();
            NotifyAnimationPaused();
        	}
        }
    else if( iSvgEngineState == ESVGEngineRunning )
        {
        if (iSvgDocument && iSvgDocument->IsAnimationFile())
            {
           	iSvgDocument->TimeContainer()->UserResume();	
            }
        }
    else if(iSvgEngineState == ESVGEngineStopped ) 
        {
            if ( iSvgDocument )
            {
                iSvgDocument->TimeContainer()->UserStop();
       	    }
     
        }   
    }

void CSvgEngineImpl::SeekEngine( TUint32 aTime)
    {
    iSvgEngineState = ESVGEngineRunning;
    if ( iSvgDocument )
    	{
    	iSvgDocument->TimeContainer()->UserSeek( aTime );
    	}
    }

void CSvgEngineImpl::ResetTimer()
    {
    if ( iSvgDocument )
        {
        iSvgDocument->TimeContainer()->UserResetTime();
        }
    }

// ---------------------------------------------------------------------------
// SetGdiContext File
// ---------------------------------------------------------------------------

void CSvgEngineImpl::SetGdiContextL(CFbsBitmap* aCurrentBitmap, CFbsBitmap* aMask)
    {
    // Handle for both Non-NULL and NULL parameter
    iFrameBuffer = aCurrentBitmap;
    iMask = aMask;

    if ( aCurrentBitmap )
        {
        if ( !iGfxContext )
            {
                iGfxContext = CGfx2dGcOpenVG::NewL( iFrameBuffer->SizeInPixels(), iBitmapFontSpec, iSvgBitmapFontProvider );
                
                // The API is called Only in case of JSR226. Incase if the midlet developer
                // has changed the RedenderQuality of the midlet.ByDefualt the value of 
                // iRenderQuality is 2 i.e. VG_RENDERING_QUALITY_BETTER.
                iGfxContext->SetAntialiasingMode( iRenderQuality );
            }
        else
            {
            iGfxContext->ChangeBufferSizeL( iFrameBuffer->SizeInPixels() );
            }
        }
    else if ( iGfxContext )
        {
        iGfxContext->ChangeBufferSizeL( TSize( 0,0 ) );
        }
    }
//----------------------------------------------------------------------------------------------
//M2G: SetGdiContextL() is overloaded to accept CSvgtBitmap to enable rendering on target buffer.
//-----------------------------------------------------------------------------------------------
void CSvgEngineImpl::SetGdiContextL(CSvgtBitmap* aCurrentBitmap, CSvgtBitmap* aMask)
    {

    // Handle for both Non-NULL and NULL parameter
    iTargetBitmapBuffer = aCurrentBitmap;
    iTargetMaskBuffer = aMask;

    if ( aCurrentBitmap )
        {
        if ( !iGfxContext )
            {
                iGfxContext = CGfx2dGcOpenVG::NewL( iTargetBitmapBuffer->SizeInPixels(), iBitmapFontSpec, iSvgBitmapFontProvider, EFalse );
                
                // The API is called Only in case of JSR226. Incase if the midlet developer
                // has changed the RedenderQuality of the midlet.ByDefualt the value of 
                // iRenderQuality is 2 i.e. VG_RENDERING_QUALITY_BETTER.
                iGfxContext->SetAntialiasingMode( iRenderQuality );
            }
        else
            {
            iGfxContext->ChangeBufferSizeL( iTargetBitmapBuffer->SizeInPixels() );
            }
        }
    else if ( iGfxContext )
        {
        iGfxContext->ChangeBufferSizeL( TSize( 0,0 ) );
        }
        
    }
// ---------------------------------------------------------------------------
// void CSvgEngineImpl::StartEngine(CSvgErrorImpl* aError)
// ---------------------------------------------------------------------------

void CSvgEngineImpl::StartEngine(CSvgErrorImpl* aError)
    {
    if ( !iFrameBuffer || !iSvgDocument )
        {
        return;
        }

    iMouseoverElement = NULL;
    if ( ((iFrameBufferSize.iHeight <= 0 )||(iFrameBufferSize.iWidth <= 0 )) &&(( iFrameBuffer->SizeInPixels().iWidth == 0 ) ||
         ( iFrameBuffer->SizeInPixels().iHeight == 0) ))
        {
        return;
        }

    TBool initialized = EFalse;

    TRAPD( initError, initialized = StartFrameLoopL() );

    if ( initError != KErrNone || !initialized )
        {
        return;
        }

    // Get the Engine State to Running
    SetSVGEngineState(ESVGEngineRunning);

    // Document is an animation
    if( iSvgDocument->IsAnimationFile() )
        {
            { // Atleast first frame should get drawn
                TRAPD(error, RedrawL() );
                {
                    if ( error != KErrNone && aError != NULL )
                    {
                       if( error == KErrNoMemory )
                       {
                           CSvgDocumentImpl::PrepareError( *aError, ESvgNoMemory, error,
                           _L( "Unable to Draw: " ),
                           _L( "" ) );  
                           return;  
                       }
                       
                       CSvgDocumentImpl::PrepareError( *aError, ESvgUnknown, error,
                       _L( "Unable to Draw: " ),
                       _L( "" ) );  
                       return;
                    }
                }
            
            }
            
        iSvgDocument->TimeContainer()->UserPlay();
        iSvgDocument->iAnimationResetNeeded = ETrue;
        }
    // Static svg file
    else
        {
            TRAPD(error, RedrawL() );
            {
                if ( error != KErrNone && aError != NULL )
                {
                   if( error == KErrNoMemory )
                   {
                       CSvgDocumentImpl::PrepareError( *aError, ESvgNoMemory, error,
                       _L( "Unable to Draw: " ),
                       _L( "" ) );  
                       return;  
                   }
                   
                   CSvgDocumentImpl::PrepareError( *aError, ESvgUnknown, error,
                   _L( "Unable to Draw: " ),
                   _L( "" ) );  
                   return;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RenderFrame( TUint aCurrentTime )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RenderFrame( TUint aCurrentTime )
    {
    if(iIsTargetRenderingEnabled) //M2G: If target rendering is enabled
        {
        if ( !iTargetBitmapBuffer || !iSvgDocument ||
              iTargetBitmapBuffer->SizeInPixels().iWidth == 0 || 
              iTargetBitmapBuffer->SizeInPixels().iHeight == 0 )
            {
            return;
            }
        }
    else
        {
        if (!iFrameBuffer || !iSvgDocument
            || iFrameBuffer->SizeInPixels().iWidth == 0
            || iFrameBuffer->SizeInPixels().iHeight == 0)
            {
            return;
            }
        }
    if ( aCurrentTime == 0 )
        {
        SeekEngine( 0 );
        }

    iMouseoverElement = NULL;

     	CSvgSvgElementImpl* rootElement = (CSvgSvgElementImpl*)iSvgDocument->RootElement();

     	if (rootElement == NULL)
     	{
     		return;
     	}

     	TGfxRectangle2D viewBoxRect;
     	if (rootElement->GetViewBox(viewBoxRect))
     	{
     		//viewbox has been defined or set already
   			if (viewBoxRect.iWidth == TFloatFixPt(0) || viewBoxRect.iHeight == TFloatFixPt(0))
   			{
   				//viewbox has no area so dont render
   				return;
   			}
   		}

    if( iRequestObserver && iSvgDocument->iEventHandler )
        {
        iRequestObserver->UpdatePresentation( iSvgDocument->iEventHandler->AnimationElementsCount() );
        }

    iSvgDocument->iInitialDrawFlag = ETrue;

    // Fix for animation element for testApp   
    TSvgTick lTick;
    lTick.iRealTimeTick = aCurrentTime;
    lTick.iParentTcTick = 0;
    iSvgDocument->TimeContainer()->ParentTimeContainerTick(lTick) ;
    // Fix for animation element for testApp   
    }


// ---------------------------------------------------------------------------
// Set SvgEngine Document
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SetDocument(CSvgDocumentImpl* aDocument)
    {
    iSvgDocument = aDocument;
    }


// ---------------------------------------------------------------------------
// Change the duration of the timer
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ChangeDuration( TUint32 aTimerDuration )
    {
    if ( iSvgDocument )
        {
        iSvgDocument->TimeContainer()->UserChangeFrameDuration( 
            aTimerDuration );
        
        // Check if there is animation element in the parent svg
    RPointerArray<CSvgElementImpl> lAnimationEleList;
    iSvgDocument->FindAllElements((CSvgElementImpl* )iSvgDocument->RootElement(),
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
            lChildDoc->TimeContainer()->UserChangeFrameDuration( 
            aTimerDuration ); // Change Frame duration for child svg
            }
        }
        lAnimationEleList.Close();     
    	}	   
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::Resume(TInt32 aTime)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::Resume(TInt32 aTime)
    {
    if ( iSvgDocument )
        {
        iSvgDocument->TimeContainer()->UserResume( aTime );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::FindBBoxesForRotatedText( const TDesC& aSearchString,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::FindBBoxesForRotatedText( const TDesC& aSearchString,
                           RPointerArray<MRect>& aBoundingBoxes,
                                        RArray<TPtrC>& aTexts,
                                        RArray<TInt>& aElementIds,
                                        TBool aCaseSensitive )
{
    if ( iSvgDocument == NULL )
        return;

    if ( iSvgTextBoundingBoxes.Count() > 0 )
    {
    	iSvgTextBoundingBoxes.ResetAndDestroy();
    }

    // Find all text elements
	HBufC* searchStr = aSearchString.AllocLC();    
	TPtr searchStrPtr = searchStr->Des();
    RPointerArray<CSvgElementImpl> textElements;
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                     KSvgTextElement, textElements );
    // Himanshu: Find all animation elements
    RPointerArray<CSvgElementImpl> lAllAnimationElements;                                
    iSvgDocument->FindAllElements( (CSvgElementImpl*)iSvgDocument->RootElement(),
                                KSvgMediaAnimationElement,
                                lAllAnimationElements );                                
    TInt lAllAnimationEleCnt = lAllAnimationElements.Count();                            
    for ( TInt i = 0; i < lAllAnimationEleCnt; i++ )
        {
          CSvgMediaAnimationElementImpl* element = (CSvgMediaAnimationElementImpl*)lAllAnimationElements[i];
          CSvgDocumentImpl* ldoc = element->GetChildDocument();
            if(ldoc)
            {
                // Find all text elements in child document
                ldoc->FindAllElements( (CSvgElementImpl*)ldoc->RootElement(),
                                KSvgTextElement,
                                textElements );  
            }
          
        }
        lAllAnimationElements.Close();

    TSvgFourPointRect boundingBox;
    TInt textEleCnt = textElements.Count();
    for ( TInt i = 0; i < textEleCnt; i++ )
    {
        CSvgTextElementImpl* textElement = (CSvgTextElementImpl*)textElements[i];
        TPtrC remainder( textElement->GetText() );

        TInt index = ( aCaseSensitive ) ? remainder.Find( aSearchString ) :
                                          remainder.FindF( aSearchString );

        // continue to next element if nothing found
        if ( index == KErrNotFound )
            continue;

		searchStrPtr.Copy( remainder.Mid(index,aSearchString.Length()) );
        // Get the bounding box for the whole text element
        textElement->GetFourPointBBox( boundingBox );

        // Get bounding box for every occurrence
        TReal32 textAdvance = (TReal32)(textElement->TextAdvance( *searchStr, index ));
        TReal32 leftAdvance = 0.0f;
        //TReal32 aHeight = textElement->Ascent() + textElement->Descent();

        while ( index != KErrNotFound )
        {
            // Calculate boundingbox for searched-text
            // indeterminate results with a string only one character long
            leftAdvance = leftAdvance + (TReal32)(textElement->TextAdvance( remainder.Left( index )));

            // need to setup slope calculation here to determine where the new
            // four point rect should be for partial pieces of text

            // need to do something for both vertical and horizontal text 0 and na slopes

            TReal32 dy = (TReal32)boundingBox.iPoint2.iY - (TReal32)boundingBox.iPoint1.iY;
            TReal32 dx = (TReal32)boundingBox.iPoint2.iX - (TReal32)boundingBox.iPoint1.iX;

            double aSlopeRan;
            Math::ATan(aSlopeRan, dy, dx);

            double aSinVal;
            Math::Sin(aSinVal, aSlopeRan);

            double aCosVal;
            Math::Cos(aCosVal, aSlopeRan);


            TReal32 x1 = aCosVal * leftAdvance + (TReal32)boundingBox.iPoint1.iX;
            TReal32 x3 = aCosVal * leftAdvance + (TReal32)boundingBox.iPoint3.iX;
            TReal32 y1 = aSinVal * leftAdvance + (TReal32)boundingBox.iPoint1.iY;
            TReal32 y3 = aSinVal * leftAdvance + (TReal32)boundingBox.iPoint3.iY;

            TReal32 x2 = aCosVal * textAdvance + x1;
            TReal32 x4 = aCosVal * textAdvance + x3;

            TReal32 y2 = aSinVal * textAdvance + y1;
            TReal32 y4 = aSinVal * textAdvance + y3;

			TSvgFourPointRect* bbox = NULL;

            TRAP_IGNORE( bbox = new (ELeave) TSvgFourPointRect( TPoint(x1, y1),
                        TPoint(x2, y2),
                        TPoint(x3, y3),
                        TPoint(x4, y4)) );

            if ( !bbox )
            {
            	#ifdef _DEBUG
            		RDebug::Printf("Four Point Rect Failed: Out of Memory");
            	#endif
            	return;
            }

            // store bound box pointers to go back to client
            aBoundingBoxes.Append( (MRect*)bbox );

            iSvgTextBoundingBoxes.Append( bbox );

            // store point to text
            aTexts.Append( TPtrC( textElement->GetText() ) );
            aElementIds.Append( (TInt)textElement );

            remainder.Set( remainder.Right( remainder.Length() - index - aSearchString.Length() ) );
            leftAdvance = leftAdvance + textAdvance;
            index = ( aCaseSensitive ) ? remainder.Find( aSearchString ) :
                                         remainder.FindF( aSearchString );
        }
    }
    CleanupStack::PopAndDestroy(searchStr);
    textElements.Close();
}

//this is the old 2point rect way that only works for horizontal text
/*void CSvgEngineImpl::FindBBoxesForHorizontalText( const TDesC& aSearchString,
                           RArray<TRect>& aBoundingBoxes,
                                        RArray<TPtrC>& aTexts,
                                        RArray<TInt>& aElementIds,
                                        TBool aCaseSensitive                                        )
{
    if ( iSvgDocument == NULL )
        return;

    // Find all text elements
    RPointerArray<CSvgElementImpl> textElements;
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                     KSvgTextElement, textElements );

    TGfxRectangle2D boundingBox;
    for ( TInt i = 0; i < textElements.Count(); i++ )
    {
        CSvgTextElementImpl* textElement = (CSvgTextElementImpl*)textElements[i];
        TPtrC remainder( textElement->GetText() );

        TInt index = ( aCaseSensitive ) ? remainder.Find( aSearchString ) :
                                          remainder.FindF( aSearchString );

        // continue to next element if nothing found
        if ( index == KErrNotFound )
            continue;

        // Get the bounding box for the whole text element
        textElement->GetBBox( boundingBox );

        // Get bounding box for every occurrence
        TFloatFixPt textAdvance = textElement->TextAdvance( aSearchString, index );
        TFloatFixPt leftAdvance = 0.0f;
        while ( index != KErrNotFound )
        {
            // Calculate boundingbox for searched-text
            leftAdvance = leftAdvance + textElement->TextAdvance( remainder.Left( index ), index );

            TRect bbox( (TInt)boundingBox.iX + (TInt)leftAdvance,
                        (TInt)boundingBox.iY,
                        (TInt)boundingBox.iX + (TInt)leftAdvance + (TInt)textAdvance,
                        (TInt)boundingBox.iY + (TInt)boundingBox.iHeight );

            // store bound box
            aBoundingBoxes.Append( bbox );

            // store point to text
            aTexts.Append( TPtrC( textElement->GetText() ) );
            aElementIds.Append( (TInt)textElement );

            remainder.Set( remainder.Right( remainder.Length() - index - aSearchString.Length() ) );
            leftAdvance = leftAdvance + textAdvance;
            index = ( aCaseSensitive ) ? remainder.Find( aSearchString ) :
                                         remainder.FindF( aSearchString );
        }
    }
    textElements.Close();
}
*/

/************************ HyperLinking Functions ***********************/

// --------------------------------------------------------------------------
// void CSvgEngineImpl::CheckForHyperlinkMouseover( TInt aX, TInt aY )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::CheckForHyperlinkMouseover( TInt aX, TInt aY )
{
   if ( iHyperlinkListeners.Count() == 0 )
        return;

    if ( iSvgDocument == NULL )
        return;

    // Gather <a> elements if first time
    RPointerArray<CSvgElementImpl> iAElementList;
    if ( iAElementList.Count() == 0 )
        {
        iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                         KSvgAElement, iAElementList );
        }

    // find the bounding box containing point
    TGfxPoint2D point( aX, aY );
    TGfxRectangle2D boundingBox;
    TInt aEleListCnt = iAElementList.Count();
    for ( TInt i = 0; i < aEleListCnt; i++ )
        {
        CSvgElementImpl* element = iAElementList[i];
        CSvgGElementImpl::GetGroupBounding( boundingBox, element );
        if ( boundingBox.Contains( point ) )
            {
            // no existing mouse over element: notify HyperlinkEntered
            if ( iMouseoverElement == NULL )
                {
                NotifyHyperlinkEntered( element->Href() );
                }
            // otherwise: notify HyperlinkExited, HyperlinkEntered
            else if ( iMouseoverElement != element )
                {
                NotifyHyperlinkExited( iMouseoverElement->Href() );
                NotifyHyperlinkEntered( element->Href() );
                }
            iMouseoverElement = element;
            return;
            }
        }

    // no bounding box containing point: notify HyperlinkExited
    if ( iMouseoverElement != NULL )
    {
        NotifyHyperlinkExited( iMouseoverElement->Href() );
        iMouseoverElement = NULL;
    }

    iAElementList.Close();
}

/*-------------------------Animation Listeners-----------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddAnimationListener( MSvgAnimationListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddAnimationListener( MSvgAnimationListener* aListener )
{
    if ( aListener != NULL && iAnimationListeners.Find( aListener ) == KErrNotFound )
        iAnimationListeners.Append( aListener );

}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveAnimationListener( MSvgAnimationListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveAnimationListener( MSvgAnimationListener* aListener )
{
    TInt index = iAnimationListeners.Find( aListener );
    if ( index != KErrNotFound )
        iAnimationListeners.Remove( index );
}

/*-------------------------Animation listener notification to client-------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyAnimationStarted()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyAnimationStarted()
{

    TBool isAnimationIndefinite = EFalse;
    if ( ( iSvgDocument == NULL ) || ( iSvgDocument->RootElement() == NULL ) ||
         ( iAnimationListeners.Count() == 0 ) )
    {
        return;
    }

    RPointerArray<CSvgElementImpl> AnimationElementList;

    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgSetElement, AnimationElementList );
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateElement, AnimationElementList );
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateMotionElement, AnimationElementList );
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateTransformElement, AnimationElementList );
    iSvgDocument->FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateColorElement, AnimationElementList );

    if ( AnimationElementList.Count() != 0 )
    {
    	TInt animationEleListCnt = AnimationElementList.Count();
        for ( TInt i=0; i < animationEleListCnt; i++ )
        {
             CSvgElementImpl * element = AnimationElementList[i];
              if ( ((CSvgAnimationBase *)element)->CompleteDuration() == KTimeIndefinite )  
                {
                 isAnimationIndefinite = ETrue;
                 break;
                }
        }

    }
   AnimationElementList.Close();

   TInt animationListenersCnt = iAnimationListeners.Count();
   for ( TInt i = 0; i < animationListenersCnt; i++ )
        iAnimationListeners[i]->AnimationStarted( isAnimationIndefinite );


}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyAnimationPaused()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyAnimationPaused()
{
	TInt animationListenersCnt = iAnimationListeners.Count();
    for ( TInt i = 0; i < animationListenersCnt; i++ )
        iAnimationListeners[i]->AnimationPaused();
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyAnimationEnded()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyAnimationEnded()
{
    if ( ( iSvgDocument == NULL ) || ( iSvgDocument->RootElement() == NULL ) ||
         ( iAnimationListeners.Count() == 0 ) )
    {
        return;
    }
 #if 0
    RPointerArray<CSvgElementImpl> AnimationElementList;

    FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgSetElement, AnimationElementList );
    FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateElement, AnimationElementList );
    FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateMotionElement, AnimationElementList );
    FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateTransformElement, AnimationElementList );
    FindAllElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                                 KSvgAnimateColorElement, AnimationElementList );

    if ( AnimationElementList.Count() != 0 )
    {
    	TInt animationEleListCnt = AnimationElementList.Count();
        for ( TInt i=0; i < animationEleListCnt; i++ )
        {
        CSvgElementImpl * element = AnimationElementList[i];
        if ( ((CSvgAnimationBase *)element)->iAnimStatus == KAnimActive )
            {
             AnimationElementList.Close();
             return;
            }
        }

    }
     AnimationElementList.Close();
#endif

	TInt animationListenersCnt = iAnimationListeners.Count();
    for ( TInt i = 0; i < animationListenersCnt; i++ )
        iAnimationListeners[i]->AnimationEnded();

}

/*------------------Register Client Text Area listeners----------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddTextAreaListener( MSvgTextAreaListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddTextAreaListener( MSvgTextAreaListener* aListener )
{
    if ( aListener != NULL && iTextAreaListeners.Find( aListener ) == KErrNotFound )
    {
        iTextAreaListeners.Append( aListener );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveTextAreaListener( MSvgTextAreaListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveTextAreaListener( MSvgTextAreaListener* aListener )
{
    TInt index = iTextAreaListeners.Find( aListener );
    if ( index != KErrNotFound )
    {
        iTextAreaListeners.Remove( index );
    }
}

/*----------------Text Area listener notification to client--------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextAreaEntered( CSvgTextAreaElementImpl* aTextAreaHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextAreaEntered( CSvgTextAreaElementImpl* aTextAreaHandle )
{
	TInt textAreaListenersCnt = iTextAreaListeners.Count();
    for (TInt i=0; i < textAreaListenersCnt; i++)
    {
        iTextAreaListeners[i]->TextAreaEntered( (TInt)aTextAreaHandle);
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextAreaExited( CSvgTextAreaElementImpl* aTextAreaHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextAreaExited( CSvgTextAreaElementImpl* aTextAreaHandle )
{
	TInt textAreaListenersCnt = iTextAreaListeners.Count();
    for (TInt i=0; i < textAreaListenersCnt; i++)
    {
        iTextAreaListeners[i]->TextAreaExited( (TInt)aTextAreaHandle);
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextAreaActivated( CSvgTextAreaElementImpl* aTextAreaHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextAreaActivated( CSvgTextAreaElementImpl* aTextAreaHandle )
{
	TInt textAreaListenersCnt = iTextAreaListeners.Count();
	for (TInt i=0; i < textAreaListenersCnt; i++)
    {
        iTextAreaListeners[i]->TextAreaActivated( (TInt)aTextAreaHandle );
    }
}

/*------------------Register Client Text listeners----------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddTextListener( MSvgTextListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddTextListener( MSvgTextListener* aListener )
{
    if ( aListener != NULL && iTextListeners.Find( aListener ) == KErrNotFound )
    {
        iTextListeners.Append( aListener );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveTextListener( MSvgTextListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveTextListener( MSvgTextListener* aListener )
{
    TInt index = iTextListeners.Find( aListener );
    if ( index != KErrNotFound )
    {
        iTextListeners.Remove( index );
    }
}

/*----------------Text listener notification to client--------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextEntered( CSvgTextElementImpl* aTextHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextEntered( CSvgTextElementImpl* aTextHandle )
{
	TInt textListenersCnt = iTextListeners.Count();
    for (TInt i=0; i < textListenersCnt; i++)
    {
        iTextListeners[i]->TextEntered( (TInt)aTextHandle);
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextExited( CSvgTextElementImpl* aTextHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextExited( CSvgTextElementImpl* aTextHandle )
{
	TInt textListenersCnt = iTextListeners.Count();
    for (TInt i=0; i < textListenersCnt; i++)
    {
        iTextListeners[i]->TextExited( (TInt)aTextHandle);
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyTextActivated( CSvgTextElementImpl* aTextHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyTextActivated( CSvgTextElementImpl* aTextHandle )
{
	TInt textListenersCnt = iTextListeners.Count();
    for (TInt i=0; i < textListenersCnt; i++)
    {
        iTextListeners[i]->TextActivated( (TInt)aTextHandle );
    }
}

/*---------------------Register Client Hyperlink listeners----------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddHyperlinkListener( MSvgHyperlinkListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddHyperlinkListener( MSvgHyperlinkListener* aListener )
{
    if ( aListener != NULL && iHyperlinkListeners.Find( aListener ) == KErrNotFound )
        iHyperlinkListeners.Append( aListener );
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveHyperlinkListener( MSvgHyperlinkListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveHyperlinkListener( MSvgHyperlinkListener* aListener )
{
    TInt index = iHyperlinkListeners.Find( aListener );
    if ( index != KErrNotFound )
        iHyperlinkListeners.Remove( index );
}

void CSvgEngineImpl::AddViewPortListener(MSvgViewPortListener* aListener)
{
    if ( aListener != NULL )
    {
        iViewPortListener = aListener ;
    }
        
}
  
        
void CSvgEngineImpl::RemoveViewPortListener(MSvgViewPortListener* 
        /* aListener */ )
{
    if(iViewPortListener != NULL)
    {
        iViewPortListener = NULL;
    }
}
/*----------------Hyperlink listener notification to client--------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkEntered( CSvgAElementImpl* aAElementHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkEntered( CSvgAElementImpl* aAElementHandle )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        if (aAElementHandle)
        {
            iHyperlinkListeners[i]->LinkEntered( aAElementHandle->Href() );
        }
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkEntered( const TDesC& aUri )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkEntered( const TDesC& aUri )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        iHyperlinkListeners[i]->LinkEntered( aUri );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkExited( CSvgAElementImpl* aAElementHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkExited( CSvgAElementImpl* aAElementHandle )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        if (aAElementHandle)
        {
            iHyperlinkListeners[i]->LinkExited( aAElementHandle->Href() );
        }
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkExited( const TDesC& aUri )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkExited( const TDesC& aUri )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        iHyperlinkListeners[i]->LinkExited( aUri );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkActivated( CSvgAElementImpl* aAElementHandle )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkActivated( CSvgAElementImpl* aAElementHandle )
{
    if ( !aAElementHandle ||
          aAElementHandle->Href().Length() == 0 ||
          aAElementHandle->Href()[0] == '#' )
        {
        return;
        }

	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        iHyperlinkListeners[i]->LinkActivated( aAElementHandle->Href() );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkActivated( const TDesC& aUri )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkActivated( const TDesC& aUri )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
    {
        iHyperlinkListeners[i]->LinkActivated( aUri );
    }
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyHyperlinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyHyperlinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow )
{
	TInt hyperLinkListenersCnt = iHyperlinkListeners.Count();
    for ( TInt i = 0; i < hyperLinkListenersCnt; i++ )
        iHyperlinkListeners[i]->LinkActivatedWithShow( aUri, aShow );
}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyElementActivatedLinkEntered( CSvgElementImpl* aElement)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyElementActivatedLinkEntered( CSvgElementImpl* aElement)
    {
      if(iMouseoverElement  == NULL)
        {
        const TDesC* myId = aElement->Id();

        if (myId)
        NotifyHyperlinkEntered( *(myId));
        iMouseoverElement = aElement;
        }
    }
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyElementActivatedLinkExited(  CSvgElementImpl* aElement)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyElementActivatedLinkExited(  CSvgElementImpl* aElement)
    {
        if(iMouseoverElement == aElement )
        {
        const TDesC* myId = aElement->Id();

        if (myId)
        NotifyHyperlinkExited( *(myId));
        iMouseoverElement  = NULL;
        }
    }
void CSvgEngineImpl::GetViewPort(TInt getWidth, TInt getHeight, TBool isWidthInPer, TBool isHeightInPer, TInt &setWidth, TInt &setHeight)
{
    if(iViewPortListener != NULL)
    {
        iViewPortListener->GetViewPort(getWidth, getHeight, isWidthInPer, isHeightInPer, setWidth, setHeight);
    }
}

/*------------------Register Client Interactive Element listeners------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddInteractiveElementListener( 
//                                  MSvgInteractiveElementListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddInteractiveElementListener( 
                                    MSvgInteractiveElementListener* aListener )
    {
    if ( 
        aListener != NULL &&
        iInteractiveElementListeners.Find( aListener ) == KErrNotFound )
        {
        iInteractiveElementListeners.Append( aListener );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveInteractiveElementListener( 
//                                  MSvgInteractiveElementListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveInteractiveElementListener( 
                                    MSvgInteractiveElementListener* aListener )
    {
    TInt index = iInteractiveElementListeners.Find( aListener );
    if ( index != KErrNotFound )
        {
        iInteractiveElementListeners.Remove( index );
        }
    }

/*-----------Interactive Element listener notification to client------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyInteractiveElementEntered(CSvgElementImpl* aElement)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyInteractiveElementEntered(
                        CSvgElementImpl* aElement)
    {
    TUint16 lsubeventmask=0; 
    TBool linteractivity;
    linteractivity=this->iSvgDocument->iEventHandler->
    CheckInteractivityAndGetSubEventMask(aElement,lsubeventmask);
    if(linteractivity)
        {
        TInt linteractiveElementListenersCnt = iInteractiveElementListeners.Count();
        for ( TInt i = 0; i < linteractiveElementListenersCnt; i++ )
            {
            const TDesC* myId = aElement->Id();
            TPtrC16 ptr;
            if(myId)
                {
                ptr.Set(*myId);               
                }
            else
                {
                _LIT(KEmptyString,"");
                ptr.Set( KEmptyString);                      
                }
            iInteractiveElementListeners[i]->InteractiveElementEntered( 
                                               ptr,lsubeventmask  ); 
            }
        }
    }
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyInteractiveElementExited(CSvgElementImpl* aElement)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyInteractiveElementExited(CSvgElementImpl* aElement)
    {
	TInt linteractiveElementListenersCnt = iInteractiveElementListeners.Count();
    for ( TInt i = 0; i < linteractiveElementListenersCnt; i++ )
        {
        const TDesC* myId = aElement->Id();
        TPtrC16 ptr;
        if(myId)
            {            
            ptr.Set(*myId);
            }
        else
            {
            _LIT(KEmptyString,"");
            ptr.Set(KEmptyString);
            }               
        iInteractiveElementListeners[i]->InteractiveElementExited(ptr);
        }
	}

// ---------------------------------------------------------------------------
// Set SVG Dimension to frame buffer size
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SetSvgDimensionToFrameBufferL(TUint aWidth, TUint aHeight)
    {
     if (iSvgDocument == NULL )   return;
     if (iSvgDocument->RootElement() != NULL)
         {
         CSvgElementImpl* theElement = (CSvgElementImpl*)(iSvgDocument->RootElement());
         ((CSvgSvgElementImpl*)theElement)->iWidthInUserCoordinate = 0;
         ((CSvgSvgElementImpl*)theElement)->iHeightInUserCoordinate = 0;
         TFloatFixPt wFix( (TInt)aWidth );
         TFloatFixPt hFix( (TInt)aHeight );
         theElement->SetAttributeFloatL(KAtrWidth, wFix );
         theElement->SetAttributeFloatL(KAtrHeight, hFix );
         }

    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::SetMediaTime(TUint32 aTimeInMilliSeconds)
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SetMediaTime(TUint32 aTimeInMilliSeconds)
    {
    // Set Current Time in the document
    if(iSvgDocument != NULL)
        {
        //iSvgDocument->SetCurrentTime( aTimeInMilliSeconds );
        
        iSvgDocument->iAnimationResetNeeded = ETrue;
        }
    if(this->iSvgDocument && this->iSvgDocument->iEventHandler)
        {
        this->iSvgDocument->iEventHandler->SetCurrentTime(aTimeInMilliSeconds);
        }
   // Find all animation elements in the document    
    RPointerArray<CSvgElementImpl> lAnimationEleList;
    if(iSvgDocument)
	    {
    iSvgDocument->FindAllElements((CSvgElementImpl* )iSvgDocument->RootElement(),
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
            lChildDoc->iAnimationResetNeeded = ETrue;
            if ( lChildDoc->iEventHandler )    
                {
                lChildDoc->iEventHandler->SetCurrentTime(aTimeInMilliSeconds); 
                }
            
            }
        }
    lAnimationEleList.Close();     
	    	
	    }
    this->SetSVGEngineState(ESVGEnginePaused);
    this->SeekEngine(aTimeInMilliSeconds);

    }

// --------------------------------------------------------------------------
// TBool CSvgEngineImpl::IsElementVisible( TInt aElementId )
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::IsElementVisible( TInt aElementId )
    {
    if ( aElementId == 0 || iFrameBuffer == NULL )
        {
        return EFalse;
        }

    CSvgElementImpl* element = (CSvgElementImpl*)aElementId;

    TInt32 visibleAttribute = -1;

    TRAPD(error,element->GetAttributeIntL( KCSS_ATTR_VISIBILITY, visibleAttribute ));
    if (error != KErrNone)
       {
       // error processing
       }


    return visibleAttribute == 0;
    }

// Set whether to call request observer's UpdateScreen method
// --------------------------------------------------------------------------
// void CSvgEngineImpl::SetIgnoreUpdateScreen( TBool aBool )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SetIgnoreUpdateScreen( TBool aBool )
{
    iIgnoreUpdateScreen = aBool;
}

/*---------------------------MouseListener---------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::AddMouseListener( const MSvgMouseListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::AddMouseListener( const MSvgMouseListener* aListener )
    {
    TInt index = iSvgMouseListeners.Find( aListener );
    if ( aListener != NULL && index == KErrNotFound )
        {
        iSvgMouseListeners.Append( aListener );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::RemoveMouseListener( const MSvgMouseListener* aListener )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::RemoveMouseListener( const MSvgMouseListener* aListener )
    {
    TInt index = iSvgMouseListeners.Find( aListener );
    if ( index != KErrNotFound )
        {
        iSvgMouseListeners.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// TInt CSvgEngineImpl::MouseListenerCount()
// ---------------------------------------------------------------------------
TInt CSvgEngineImpl::MouseListenerCount()
    {
    return iSvgMouseListeners.Count();
    }

/*-------------MouseListener Notifications back to client---------------------------*/
// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyMousePressed( RPointerArray<CSvgElementImpl>& aElements,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyMousePressed( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {
    TInt svgMouseListenersCnt = iSvgMouseListeners.Count();
    for ( TInt i = 0; i < svgMouseListenersCnt; i++ )
        {
        iSvgMouseListeners[i]->MousePressed( aElements, aX, aY );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyMouseReleased( RPointerArray<CSvgElementImpl>& aElements,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyMouseReleased( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {
    TInt svgMouseListenersCnt = iSvgMouseListeners.Count();
    for ( TInt i = 0; i < svgMouseListenersCnt; i++ )
        {
        iSvgMouseListeners[i]->MouseReleased( aElements, aX, aY );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyMouseEntered( RPointerArray<CSvgElementImpl>& aElements,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyMouseEntered( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {
    TInt svgMouseListenersCnt = iSvgMouseListeners.Count();
    for ( TInt i = 0; i < svgMouseListenersCnt; i++ )
        {
        iSvgMouseListeners[i]->MouseEntered( aElements, aX, aY );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyMouseExited( RPointerArray<CSvgElementImpl>& aElements,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyMouseExited( RPointerArray<CSvgElementImpl>& aElements,
                                          TInt aX, TInt aY )
    {
    TInt svgMouseListenersCnt = iSvgMouseListeners.Count();
    for ( TInt i = 0; i < svgMouseListenersCnt; i++ )
        {
        iSvgMouseListeners[i]->MouseExited( aElements, aX, aY );
        }
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::NotifyMouseMoved( RPointerArray<CSvgElementImpl>& aElements,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::NotifyMouseMoved( RPointerArray<CSvgElementImpl>& aElements,
                                       TInt aX, TInt aY )
    {
    TInt svgMouseListenersCnt = iSvgMouseListeners.Count();
    for ( TInt i = 0; i < svgMouseListenersCnt; i++ )
        {
        iSvgMouseListeners[i]->MouseMoved( aElements, aX, aY );
        }
    }

/*---------------------------------------------------------------------*/

// --------------------------------------------------------------------------
// void CSvgEngineImpl::GetViewableElementsAtPoint( RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::GetViewableElementsAtPoint( CSvgDocumentImpl* aSvgDocument, RPointerArray<CSvgElementImpl>& aElements, TInt aX, TInt aY )
    {
   if ( aSvgDocument == NULL )
        return;

	//JSR226 Change this could slow down event handling but it seems to be required to work
   UpdateCTM(aSvgDocument);

    // Gather all "viewable" elements
    RPointerArray<CSvgElementImpl> iViewableElementList;
    if ( iViewableElementList.Count() == 0 )
        {
        FindAllViewableElements( (CSvgElementImpl* )aSvgDocument->RootElement(),
                                 iViewableElementList );
        }

    // find the bounding box containing point
    TGfxPoint2D point( aX, aY );
    TGfxRectangle2D boundingBox;
    TInt viewableEleListCnt =  iViewableElementList.Count();
    for ( TInt i = 0; i < viewableEleListCnt; i++ )
        {
        CSvgElementImpl* element = iViewableElementList[i];
        element->GetBBox( boundingBox );
        if ( boundingBox.Contains( point ) )
            {
            aElements.Append( element );
            }
        }
    // Display of list
    iViewableElementList.Close();
    }

// Return all viewable elements
// --------------------------------------------------------------------------
// void CSvgEngineImpl::FindAllViewableElements( CSvgElementImpl* aStartElement,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::FindAllViewableElements( CSvgElementImpl* aStartElement,
                                              RPointerArray<CSvgElementImpl>& iElementList )
    {
    if ( aStartElement == NULL )
        return;
    // The child elements of the defs element should not be viewable. skip the
    // tree traversing if the element id is defs element
    if(aStartElement->ElemID() == KSvgDefsElement)
        return;

    CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
    while ( child != NULL )
        {
        // add to list if child is found
        TInt32 displayValue = 0;
        TRAP_IGNORE(child->GetAttributeIntL( KCSS_ATTR_DISPLAY, displayValue ));
        if ( child->IsViewable() && child->IsVisible() && displayValue != KDisplayEnumNone )
            {
				//JSR226 CHANGE do we care if multiple signals are sent to some elements - would be faster without this check
            	if ( iElementList.Find( child ) == KErrNotFound )
            	{
            		iElementList.Append( child );
				}
			}
        // find in grandchildren
        FindAllViewableElements( child, iElementList );
        child = (CSvgElementImpl*)child->NextSibling();
        }
    }

// Return all non-viewable elements
// --------------------------------------------------------------------------
// void CSvgEngineImpl::FindAllNonViewableElements( CSvgElementImpl* aStartElement,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::FindAllNonViewableElements( CSvgElementImpl* aStartElement,
                                                 RPointerArray<CSvgElementImpl>& iElementList )
    {
    if ( aStartElement == NULL )
        return;

    CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
    while ( child != NULL )
        {
        // add to list if child is found
        if ( !child->IsViewable() && !child->IsVisible() )
            iElementList.Append( child );

        // find in grandchildren
        FindAllNonViewableElements( child, iElementList );
        child = (CSvgElementImpl*)child->NextSibling();
        }
    }

/**
 * Return the data pointed to by the given URI string,
 * from an external source.
 */
// --------------------------------------------------------------------------
// HBufC8* CSvgEngineImpl::FetchExternalDataL( const TDesC& aUri )
// ---------------------------------------------------------------------------
HBufC8* CSvgEngineImpl::FetchExternalDataL( const TDesC& aUri )
    {
    if ( iRequestObserver == NULL )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineImpl::FetchExternalData--requestobserver is NULL\n");
        #endif //_DEBUG
        return NULL;
        }
    // Connect session
    RFs session;
    TInt connectError = session.Connect();
    if ( connectError != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineImpl::FetchExternalData--session.Connect() failed: %d\n", connectError );
        #endif //_DEBUG
        return NULL;
        }

    RFile fileHandle;
    // Check for FetchImage error code
    TInt fetchError = iRequestObserver->FetchImage( aUri, session, fileHandle );
    if ( fetchError != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineImpl::FetchExternalData--FetchImage error: %d\n", fetchError );
        #endif //_DEBUG
        session.Close();
        return NULL;
        }

    // Read file size
    TInt fileSize = 0;
    TInt sizeError = fileHandle.Size( fileSize );
    if ( sizeError != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineImpl::FetchExternalData--fileHandle.Size error: %d\n", sizeError );
        #endif //_DEBUG
        session.Close();
        return NULL;
        }

    // Allocate memory for file
    HBufC8* data = HBufC8::NewL( fileSize );
    TPtr8 des = data->Des();
    TInt readError = fileHandle.Read( des );
    if ( readError != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("CSvgEngineImpl::FetchExternalData--fileHandle.Read error: %d\n", readError );
        #endif //_DEBUG
        session.Close();
        delete data;
        return NULL;
        }

    // Successful
    session.Close();
    return data;
    }
// --------------------------------------------------------------------------
// void CSvgEngineImpl::SetAudioVolume( TInt aPercentage );
// --------------------------------------------------------------------------
void CSvgEngineImpl::SetAudioVolume( TInt aPercentage )
    {
    
    if ( !iSvgDocument )
        {
        return;
        }
    // Locate all the active audio elements
    RPointerArray<CSvgElementImpl> lAudioElementList;
    
    iSvgDocument->FindAllElements( 
            (CSvgElementImpl* )iSvgDocument->RootElement(),
            KSvgAudioElement, lAudioElementList );
    // Set the volume on each audio element
    TInt lAudEleCnt = lAudioElementList.Count();
    for ( TInt lCurAudioEle = 0; lCurAudioEle < lAudEleCnt; lCurAudioEle++ )
        {
        CSvgAudioElementImpl* lAudioElement = (CSvgAudioElementImpl*)lAudioElementList[ lCurAudioEle ];
        lAudioElement->SetVolume( aPercentage );
        }
    lAudioElementList.Close();
    }

// --------------------------------------------------------------------------
// TBool CSvgEngineImpl::ReadyToRender()
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::ReadyToRender()
    {
    return iSvgDocument != NULL;
    }

// --------------------------------------------------------------------------
// TBool CSvgEngineImpl::IsEndedWithCompleteTextTag( TDes &BufferPtr,
//                                                   TInt EndOffset)
// ---------------------------------------------------------------------------
TBool CSvgEngineImpl::IsEndedWithCompleteTextTag(TDes &aBufferPtr,TInt aEndOffset)
    {
    TChar KOpeningBrace = '<';
    TChar KSlash = '/';

    // This function searches the buffer in reverse order from the end offset
    // to check whether the last element had a complete text tag.
    // eg. of complete text tags
    //          <text>Hello</text>
    //          <text></text>
    // eg. of an incomplete text tag
    //          <text />

    TPtrC currentSelectionPtr = aBufferPtr.Left( aEndOffset );
    TInt OpeningBracePos = currentSelectionPtr.LocateReverse( KOpeningBrace );
    TInt SlashPos = currentSelectionPtr.LocateReverse( KSlash );
    TBool retVal = EFalse;

    // In case of a complete text tag the opening brace is one position behind
    // slash.
    if ( SlashPos == ( OpeningBracePos + 1 ) )
        {
        retVal = ETrue;
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// void CSvgEngineImpl::SaveSvgL( const TDesC& aFileName )
// ---------------------------------------------------------------------------
void CSvgEngineImpl::SaveSvgL( const TDesC& aFileName )
    {
    RFs  lFs;
    RFileWriteStream  lStream;
    TInt error ;
    TChar KOpeningBrace = '<';
    TChar KSlash = '/';
    // Connect to File Server to write the changed content to a file
    CleanupClosePushL(lFs);
    User::LeaveIfError(lFs.Connect() );

    TBool isOpened = EFalse;
    error = lFs.IsFileOpen( aFileName, isOpened ) ;
    // dont leave if file doesnt exist or there is no error
    if(error!= KErrNone && error!=KErrNotFound)
        {
        //usually will come here with KErrNotFound
        //since file wont exist
        User::Leave(error);
        }

    if ( isOpened )
        {
        #ifdef _DEBUG
        RDebug::Printf( "Cannot SaveSvgL: File is still opened: ");
        RDebug::RawPrint( aFileName );
        #endif
        CleanupStack::PopAndDestroy(1);
        return;
        }

    // Try to create the write stream using the file name
    if ( lStream.Create( lFs, aFileName, EFileWrite ) != KErrNone )
        {
        // If creation fails, file exists already, try to open the
        // existing file
        User::LeaveIfError( lStream.Open(lFs, aFileName, EFileWrite) );
        }
    CleanupClosePushL(lStream);

    // Array to store list of all editable elements
    RPointerArray<CSvgElementImpl> lTextElementList;

    FindEditableElements( (CSvgElementImpl* )iSvgDocument->RootElement(),
                          lTextElementList );
    CleanupClosePushL(lTextElementList);
    TInt index=0, startoffset = 0, endoffset = 0;

    // The content of the file is stored in the root <svg> node
    HBufC*  lBufferContent = ((CSvgSvgElementImpl* )iSvgDocument->RootElement())->iContent;



    // While there are remaining editable elements do
    while(index < lTextElementList.Count())
        {
        TBool EndedWithCompleteTextTag= true;
        TInt OrgLength;

        if ( lTextElementList[index]->ElemID() == KSvgTextElement )
            {
            // If the element is a <text> element invoke element specific function
            // to retrieve the offset and original length
            endoffset = ((CSvgTextElementImpl *)lTextElementList[index])->iOffset;
            OrgLength = ((CSvgTextElementImpl *)lTextElementList[index])->GetOrgLength();
            }
        else
            {
            // If the element is a <textArea> element invoke element specific function
            // to retrieve the offset and original length
            endoffset = ((CSvgTextAreaElementImpl *)lTextElementList[index])->iOffset;
            OrgLength = ((CSvgTextAreaElementImpl *)lTextElementList[index])->GetOrgLength();
            }

        // These are the cases which have to be processed
        //     a. Text Element: No Closing Tag: <text  ... />
        //     b. Text Element: No CData : <text ...></text>
        //     c. Text Element: CData : <text ...>Sample Text</text>
        //     d. TextArea Element: No Closing Tag: <textArea ... />
        //     e. TextArea Element: No CData: <textArea ...></textArea>
        //     f. TextArea Element: CData: <textArea ...>Sample TextArea</textArea>
        //     -> for a,b,d,e the endoffset represents the offset after the closing braces.
        //        for e.g.  <text ... />
        //                              ^
        //        <text></text>
        //                     ^
        //     -> for c,f the EndOffset represents the offset to the first character in
        //        content.
        //        <text>Hello</text>
        //
        //              ^
        if(!OrgLength)
            {
            //case a,b,d,e
            TPtr BufferPtr=lBufferContent->Des();
            EndedWithCompleteTextTag = IsEndedWithCompleteTextTag(BufferPtr,endoffset);
            if ( EndedWithCompleteTextTag )
                {
                // case b,e
                if(endoffset < startoffset)
                    {
                    User::Leave(KErrNotFound);
                    }
                TPtrC lPtrtoPresentText = lBufferContent->Mid( startoffset, ( endoffset - startoffset ) );
                //end offset modified ot point to <text></text>
                //                                      ^
                endoffset = startoffset + lPtrtoPresentText.LocateReverse( KOpeningBrace );
                }
            else
                {
                //case a,d
                if(endoffset < startoffset)
                    {
                    User::Leave(KErrNotFound);
                    }
                TPtrC lPtrtoPresentText = lBufferContent->Mid( startoffset, ( endoffset - startoffset ) );
                 endoffset = startoffset + lPtrtoPresentText.LocateReverse( KSlash );
                //end offset modified ot point to <text/>
                //                                     ^
                }
            }
        if(endoffset < startoffset)
            {
            User::Leave(KErrNotFound);
            }
        TPtrC lPtrtoWrBuffer = lBufferContent->Mid( startoffset, ( endoffset - startoffset ) );
        TInt lWrBufLength = lPtrtoWrBuffer .Length();
        User::LeaveIfError(WriteToStream(lStream,lPtrtoWrBuffer));
        // If it was a text element ...
        if ( lTextElementList[index]->ElemID() == KSvgTextElement )
            {
            HBufC *lBufTextContent= NULL;
            //make startoffset point to opening tag '<' or '/' in case of a,d
            TPtrC lPtrtoTextContent= ( (CSvgTextElementImpl* ) lTextElementList[ index ] )->GetText();
            startoffset = endoffset + ( (CSvgTextElementImpl* ) lTextElementList[ index ] )->iOrgLength;
            lBufTextContent = TextTagProcessingL(&lPtrtoTextContent,
                                    startoffset,OrgLength,EndedWithCompleteTextTag);
            CleanupStack::PushL(lBufTextContent);
            TPtr lTempPtr(lBufTextContent->Des());
            lPtrtoTextContent.Set(lTempPtr);
            User::LeaveIfError(WriteToStream(lStream,lPtrtoTextContent));
            CleanupStack::PopAndDestroy(lBufTextContent);
            }
       else
            {
            HBufC *lBufTextAreaContent= NULL;
            HBufC* lBufPresentText = NULL;
            TInt textlength = ((CSvgTextAreaElementImpl *)lTextElementList[index])->GetLength();
            lBufTextAreaContent = HBufC::NewLC( textlength );

            TPtr lPtrtoGetTextAreaContent = lBufTextAreaContent->Des();
            ((CSvgTextAreaElementImpl *)lTextElementList[index])->GetText( lPtrtoGetTextAreaContent );
            TPtrC lPtrtoTextAreaContent=lPtrtoGetTextAreaContent;
            startoffset = endoffset + ((CSvgTextAreaElementImpl *)lTextElementList[index])->iOrgLength;
            lBufPresentText = TextAreaTagProcessingL(&lPtrtoTextAreaContent,startoffset,OrgLength,EndedWithCompleteTextTag);
            CleanupStack::PushL(lBufPresentText);
            TPtr lTempPtr(lBufPresentText->Des());
            lPtrtoTextAreaContent.Set(lTempPtr);
            User::LeaveIfError(WriteToStream(lStream,lPtrtoTextAreaContent));
            CleanupStack::PopAndDestroy(lBufPresentText);
            CleanupStack::PopAndDestroy(lBufTextAreaContent);
            }
            index++;
        }

    // Done using the list, close the list to avoid a memory leak
    CleanupStack::PopAndDestroy(&lTextElementList);

    // After processing all the editable text elements, are there any characters in the buffer
    if( startoffset < lBufferContent->Length() )
        {
        TPtrC lPtrtoRemainText = lBufferContent->Mid(startoffset,(lBufferContent->Length()-startoffset));
        User::LeaveIfError(WriteToStream(lStream,lPtrtoRemainText));
        }

    // Close the stream
    CleanupStack::PopAndDestroy(&lStream);

    // Close the file server session
    CleanupStack::PopAndDestroy(&lFs);
    }

//-----------------------------------------------------------------------------
//Function to process the text and add tags at the end
//HBufC* CSvgEngineImpl::TextTagProcessing(TPtr* aCData, TInt aStartOffset, TInt aOrgLength,
//                              TBool aEndingWithTag)
//-----------------------------------------------------------------------------

HBufC* CSvgEngineImpl::TextTagProcessingL(TPtrC* aCData,
                    TInt &aStartOffset, TInt aOrgLength,TBool aEndingWithTag)
    {
        HBufC * lTextContentBuffer = NULL;
        TPtrC lPtrToTextContent(*aCData) ;
        //Ending with complete text tag and original length is 0
        if( aEndingWithTag && aOrgLength == 0 )
            {//case b
            lTextContentBuffer = HBufC::NewLC(aCData->Length()
                                    + KClosingTextTagLength );
            TPtr lPtrToTempBuf(lTextContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextTag);//append closing text tag
            aStartOffset += KClosingTextTagLength;
            }
        else if( !aEndingWithTag && aOrgLength == 0 )
            {//case a
            lTextContentBuffer = HBufC::NewLC(aCData->Length()
                                    + KClosingTextTagLength
                                    + KClosingBracesLength );
            TPtr lPtrToTempBuf(lTextContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(KClosingBraces);
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextTag);//append closing text tag
            aStartOffset += 2;// the length of /> which is 2 characters.
            }
        else if(aEndingWithTag && aOrgLength!= 0)
            {
            lTextContentBuffer = HBufC::NewLC(aCData->Length()
                                    + KClosingTextTagLength );
            TPtr lPtrToTempBuf(lTextContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextTag);//append closing text tag
            aStartOffset += KClosingTextTagLength ;
            }
        else
            {
            //i.e !aEndingWithTag && OrgLength!=0
            //this case should never occur
            //becoz there is no possibility of
            //the quoted kind of text content definition in xml as shown below
            //"<text /> this is a text content"
            }
        // Pop the buffer from the cleanup stack as ownership is transferred
        // to caller
        CleanupStack::Pop(lTextContentBuffer);
        return lTextContentBuffer;

    }

//-----------------------------------------------------------------------------
//Function to process the text and add tags at the end
//HBufC* CSvgEngineImpl::TextAreaTagProcessing(TPtr* aCData, TInt aStartOffset, TInt aOrgLength,
//                              TBool aEndingWithTag)
//-----------------------------------------------------------------------------

HBufC* CSvgEngineImpl::TextAreaTagProcessingL(TPtrC* aCData, TInt &aStartOffset, TInt aOrgLength,
                                TBool aEndingWithTag)
    {
        HBufC * lTextAreaContentBuffer = NULL;
        TPtrC lPtrToTextAreaContent(*aCData) ;


        //Ending with complete text tag and original length is 0
        if( aEndingWithTag && aOrgLength == 0 )
            {//case e
            lTextAreaContentBuffer = HBufC::NewLC(aCData->Length()
                                        + KClosingTextAreaTagLength);
            TPtr lPtrToTempBuf(lTextAreaContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextAreaTag);//append closing text tag
            aStartOffset += KClosingTextAreaTagLength;
            }
        else if( !aEndingWithTag && aOrgLength == 0 )
            {//case d
            lTextAreaContentBuffer = HBufC::NewLC(aCData->Length()
                                        + KClosingBracesLength
                                        + KClosingTextAreaTagLength );
            TPtr lPtrToTempBuf(lTextAreaContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(KClosingBraces);
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextAreaTag);//append closing text tag
            aStartOffset += 2;// the length of /> which is 2 characters.
            }
        else if(aEndingWithTag && aOrgLength!= 0)
            {//case f
            lTextAreaContentBuffer = HBufC::NewLC(aCData->Length()
                                        + KClosingTextAreaTagLength );
            TPtr lPtrToTempBuf(lTextAreaContentBuffer->Des());//point to buffer
            lPtrToTempBuf.Append(*aCData);//append cdata
            lPtrToTempBuf.Append(KClosingTextAreaTag);//append closing text tag
            aStartOffset += KClosingTextAreaTagLength;
            }
        else
            {
            //i.e !aEndingWithTag && OrgLength!=0
            //this case should never occur
            //becoz there is no possibility of
            //the quoted kind of text content definition in xml as shown below
            //"<textArea /> this is a text content"
            }
        // Pop the buffer from the cleanup stack as ownership is transferred
        // to caller
        CleanupStack::Pop(lTextAreaContentBuffer);
        return lTextAreaContentBuffer;

    }


//-----------------------------------------------------------------------------
//Function to Write the buffer to the stream.
//In SaveSVGL while editing text - used to write partial buffers into the stream.
//TInt CSvgEngineImpl::WriteToStream(RFileWriteStream* aStream,TPtrC aWriteBuffer)
//-----------------------------------------------------------------------------
TInt CSvgEngineImpl::WriteToStream(RFileWriteStream &aStream,TPtrC aWriteBuffer)
    {
    // Create a small output buffer
    TBuf8<KMaxConversionChars> outputBuffer;

    for(;;) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the
        // number of characters that were not converted
        const TInt returnValue = CnvUtfConverter::ConvertFromUnicodeToUtf8(
            outputBuffer,   // Destination
            aWriteBuffer ); // Source


        // check to see that the descriptor isn’t corrupt - return with
        // err if it is
        if ( returnValue == CnvUtfConverter::EErrorIllFormedInput )
            {
            return( KErrCorrupt );
            }
        else
            {
            if ( returnValue < 0 ) // future-proof against "TError" expanding
                {
                return( KErrGeneral );
                }
            }

        // Store the converted contents of the output buffer into the stream
        TRAPD( err1, aStream.WriteL( outputBuffer ) );
        if ( err1 )
            {
            #ifdef _DEBUG
            RDebug::Printf("CSvgEngineImpl::WriteToStream error trapped=%d", err1);
            return err1;
            #endif
            }

        // Finish conversion if there are no unconverted characters in the
        // remainder buffer
        if ( returnValue == 0 )
            {
            break;
            }

        // Remove the converted source text from the remainder buffer.
        // The remainder buffer is then fed back into loop
        aWriteBuffer.Set( aWriteBuffer.Right( returnValue ));
        }
    return KErrNone;
    }


// --------------------------------------------------------------------------
// void CSvgEngineImpl::FindEditableElements( CSvgElementImpl* aStartElement,
// ---------------------------------------------------------------------------
void CSvgEngineImpl::FindEditableElements( CSvgElementImpl* aStartElement,
                                           RPointerArray<CSvgElementImpl>& aList )
    {
    if ( aStartElement == NULL )
        {
        return;
        }

    CSvgElementImpl* child = (CSvgElementImpl*)aStartElement->FirstChild();
    while ( child != NULL )
    {
        // add to list if child is found
        if ( child->ElemID() == KSvgTextElement || child->ElemID() == KSvgTextAreaElement  )
            aList.Append( child );

        // find in grandchildren if element is not a textArea
        if ( child->ElemID() != KSvgTextAreaElement )
            {
        FindEditableElements( child, aList );
            }
        child = (CSvgElementImpl*)child->NextSibling();
    }
}
TBool CSvgEngineImpl::IsSVGEnginePaused()
{
	return (iSvgEngineState == ESVGEnginePaused );

}

// --------------------------------------------------------------------------
// void CSvgEngineImpl::ResetContext()
// ---------------------------------------------------------------------------
void CSvgEngineImpl::ResetContext()
{
	if( iGfxContext )
		{
		iGfxContext->DestroyContext();
		}
}

void CSvgEngineImpl::SetBitmapHeader(const TDesC* aHeaderData)
    {
    if (iGfxContext)
        {
        ((CGfx2dGcOpenVG *)iGfxContext)->SetBitmapHeader(aHeaderData);
        }
    }
//---------------------------------------------------------------
// M2G: Enable rendering on target buffer.
//----------------------------------------------------------------
void CSvgEngineImpl::EnableTargetRendering(TBool aTargetRendering )
    {
    iIsTargetRenderingEnabled = aTargetRendering; 
    }

//---------------------------------------------------------------
// M2G: TBool CSvgEngineImpl::IsTargetRenderingEnabled() const 
//----------------------------------------------------------------
TBool CSvgEngineImpl::IsTargetRenderingEnabled() const 
    {
    return iIsTargetRenderingEnabled; 
    }
//NGA
// ---------------------------------------------------------------------------
// SetGdiContext File
// ---------------------------------------------------------------------------

void CSvgEngineImpl::SetGdiContextL(CFbsBitmap* aCurrentBitmap, CFbsBitmap* aMask,TSize aCurrentBitmapSize,TDisplayMode aRenderDspMode,TDisplayMode aMaskDspMode)

    {
    // Handle for both Non-NULL and NULL parameter
       iFrameBufferSize = aCurrentBitmapSize;
       iRenderDspMode   = aRenderDspMode;
       iMaskDspMode     = aMaskDspMode; 
       iFrameBuffer     = aCurrentBitmap;
       iMask            = aMask;

    if ( aCurrentBitmapSize.iWidth > 0 && aCurrentBitmapSize.iHeight > 0 )
        {
        if ( !iGfxContext )
            {
                iGfxContext = CGfx2dGcOpenVG::NewL( aCurrentBitmapSize, iBitmapFontSpec, iSvgBitmapFontProvider,1 );
            }
        else
            {
            iGfxContext->ChangeBufferSizeL( aCurrentBitmapSize );
            }
        }
    else if ( iGfxContext )
        {
        iGfxContext->ChangeBufferSizeL( TSize( 1, 1 ) );
        }
    }
const TPtrC8 CSvgEngineImpl::TLVEncodedData() const
    {
    return (iGfxContext->TLVEncodedData());
    }
