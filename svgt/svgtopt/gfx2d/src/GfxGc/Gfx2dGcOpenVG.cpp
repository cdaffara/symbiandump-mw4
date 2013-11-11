/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library source file
*
*/


#include "Gfx2dGcOpenVG.h"
//#include "Gfx2dDevice.h"
#include "GfxLine2D.h"
#include "GfxStroke.h"
#include "GfxGeneralPath.h"
#include "GfxFlatteningPathIterator.h"
#include "GfxColor.h"
#include "GfxImageTransformer.h"
//#include "hal.h"
//#include "hal_data.h"

#include "gdi.h"
#include "biditext.h"

#include "GfxFloatFixPt.h"
#include "GfxShape.h"
#include "GfxEllipse2D.h"
#include "GfxRoundRectangle2D.h"
//#include "svgBackendStd.h"
//#include "gulutil.h"
#include <AknFontAccess.h>
//#include <AknUtils.h>
//#include <featmgr.h>
#include<e32cmn.h>
#include<e32uid.h>//NGA
#include <e32math.h>
// OpenVG Includes
#include <VG/vgu.h>
#include <vg/vgcontext.h>
// OpenVG API Binding Symbian specific
#include <vg/vgcontext_symbian.h>

#include <svgtbitmap.h>
// For allocating path segment type array
// Found 130 to be max size at bootup
const TInt KPathSegmentTypeInitialSize = 130;
const TInt KPathSegmentTypeMaxSize = 1000;

const TInt KRenderQualityLow  = 1;
const TInt KRenderQualityHigh = 2;
_LIT( KDefaultFont, "serif" ); // Similar changes should be applied to \
                                //SvgTextElementImpl.cpp file

// --------------------------------------------------------------------------
// CGfx2dGcOpenVG::CGfx2dGcOpenVG() : iScale( 1 ),
// ---------------------------------------------------------------------------
CGfx2dGcOpenVG::CGfx2dGcOpenVG( TBool aRenderOption ) :
                        iRenderOption(aRenderOption),
                        iGraphicsContextCreated( EFalse ),
                        iFillOpacity( 1 ),
                        iScale( 1 ),
                        iStrokeColor( 0 ),
                        iStrokeOpacity( 1 ),
                        iBackgroundColor( 0xffffff ),
                        iFontSize( 10 ),
                        iFontWeight( -1 ),
                        iFontStyle( -1 ),
                        iFamilies( NULL ),
                        iTextAnchor( EGfxTextAnchorNone ),
                        iTextDecoration( EGfxTextDecorationNone ),
                        iRenderQuality(VG_RENDERING_QUALITY_BETTER),
		       						iCurrentRendererType(ESVGRendererSW)
                       
    {
    }
//
// ==========================================================================
// Create a new instance.
// ==========================================================================
// --------------------------------------------------------------------------
// CGfx2dGcOpenVG* CGfx2dGcOpenVG::NewL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
 CGfx2dGcOpenVG* CGfx2dGcOpenVG::NewL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider, TBool aRenderOption )
    {
    CGfx2dGcOpenVG* self = new( ELeave ) CGfx2dGcOpenVG( aRenderOption );
    CleanupStack::PushL( self );
    self->ConstructL( aBufferSize, aFontSpec, aSvgBitmapFontProvider);
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// void CGfx2dGcOpenVG::ConstructL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
void CGfx2dGcOpenVG::ConstructL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider )
    {
   /* iApacAvailable = EFalse;
    FeatureManager::InitializeLibL();
    iApacAvailable = ( AknLayoutUtils::Variant() == EApacVariant );
    FeatureManager::UnInitializeLib();*/


    iFontSpec = aFontSpec;

    if (iFontSpec.iTypeface.iName.Length() == 0)
        {
    	//just for a safety check in case client didnt put it in
    	iFontSpec = TFontSpec(KDefaultFont, 100);
        }

	
	iSvgBitmapFontProvider = aSvgBitmapFontProvider	;
	
    // Allocate path segment type array
    iPathSegmentTypeCount = KPathSegmentTypeInitialSize;
    iPathSegmentTypes = new (ELeave) VGubyte[KPathSegmentTypeInitialSize];

    iDashArray = new ( ELeave ) CArrayFixFlat<VGfloat>( 32 );
    
    iVgRenderer  = CVGRenderer::NewL(ESVGRendererSW, iRenderOption);
    iVgSurface   = iVgRenderer->GetCurrentSurface();
    ChangeBufferSizeL( aBufferSize );
    }
//NGA
// --------------------------------------------------------------------------
// CGfx2dGcOpenVG* CGfx2dGcOpenVG::NewL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
 CGfx2dGcOpenVG* CGfx2dGcOpenVG::NewL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType,TBool aRenderOption )
    {
    CGfx2dGcOpenVG* self = new( ELeave ) CGfx2dGcOpenVG( aRenderOption );
    CleanupStack::PushL( self );
    self->ConstructL( aBufferSize, aFontSpec, aSvgBitmapFontProvider,aRendererType);
    CleanupStack::Pop();
    return self;
    }
//NGA    
// --------------------------------------------------------------------------
// void CGfx2dGcOpenVG::ConstructL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
void CGfx2dGcOpenVG::ConstructL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType )
    {

    iFontSpec = aFontSpec;

    if (iFontSpec.iTypeface.iName.Length() == 0)
        {
    	//just for a safety check in case client didnt put it in
    	iFontSpec = TFontSpec(KDefaultFont, 100);
        }
		iSvgBitmapFontProvider = aSvgBitmapFontProvider	;
	
    // Allocate path segment type array
    iPathSegmentTypeCount = KPathSegmentTypeInitialSize;
    iPathSegmentTypes = new (ELeave) VGubyte[KPathSegmentTypeInitialSize];

    iDashArray = new ( ELeave ) CArrayFixFlat<VGfloat>( 32 );
    iCurrentRendererType = aRendererType;
    iVgRenderer = CVGRenderer::NewL(aRendererType, iRenderOption);
    iVgSurface   = iVgRenderer->GetCurrentSurface();
    
    iVgSurface->InitializeSurface( aBufferSize, VGI_COLORSPACE_SRGB );
    iVgSurface->CreateSurface(EColor16MA, 0, 0);

    ChangeBufferSizeL( aBufferSize );

    }    
// ==========================================================================
// Destructor.
// ==========================================================================
// --------------------------------------------------------------------------
//  CGfx2dGcOpenVG::~CGfx2dGcOpenVG()
// ---------------------------------------------------------------------------
 CGfx2dGcOpenVG::~CGfx2dGcOpenVG()
    {
    if ( iDashArray )
        {
        delete iDashArray;
        iDashArray = NULL;
        }

    // This is no longer needed, the Font is owned by TextElement
    // now as each text element may have its own specific font
    // and keeping one copy at Gc level will not work.
    // Gc is just refering to the Font from Textelement, so
    // now need to free it here.
    //
    //if (iFont && iWsScreenDevice)
    //	{
	//	iWsScreenDevice->ReleaseFont(iFont);
	//	iFont = NULL;
 	//   }

	
    DestroyContext();

    iGroupOpacityImages.Close();

    TInt bufferCount = iGroupOpacityBuffers.Count();
    for ( TInt i = 0; i < bufferCount; i++ )
        {
        delete [] iGroupOpacityBuffers[i];
        }
    iGroupOpacityBuffers.Close();

    if ( iPathSegmentTypes )
        {
        delete [] iPathSegmentTypes;
        }
    delete iVgRenderer;

    }

// private method
void CGfx2dGcOpenVG::DestroyContext()
    {
       if ( iGraphicsContextCreated )
       {
            // Destroy OpenVG context
            iVgSurface->TerminateSurface();
            ResetContextHandle();
            iFillOpacity = 1;
            iScale = 1;
            iStrokeOpacity = 1;
       }
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFrameBufferL( CFbsBitmap* aFrameBuffer )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::ChangeBufferSizeL( const TSize aBufferSize )
    {
    if ( aBufferSize.iWidth <= 0 || aBufferSize.iHeight <= 0 || aBufferSize == iColorBufferSize )
        {
        return;
        }

    iColorBufferSize = aBufferSize;

    // First time: creating OpenVG context
    if( !iGraphicsContextCreated )
    {
        // Create OpenVG context
             TInt err = iVgSurface->InitializeSurface( iColorBufferSize, VGI_COLORSPACE_SRGB );
        if (!((err == KErrNone) || (err == KErrAlreadyExists)))
            {
            #ifdef _DEBUG
            RDebug::Printf("SVG can't initialize OpenVG Context %d", err);
            #endif //_DEBUG
            iGraphicsContextCreated = EFalse;
            User::Leave(err);
            }
        iGraphicsContextCreated = ETrue;
        // By default the Rendering quality is VG_RENDERING_QUALITY_BETTER
        iVgRenderer->vgSeti( VG_RENDERING_QUALITY, iRenderQuality );
        }
    // Only need to update context with color-buffer info
    // when frame buffer size changes
    else
    { 
        TInt err = iVgSurface->ResizeSurface( iColorBufferSize );
        
        if( err != KErrNone )
        {
            if(err == KErrNoMemory )
            {
                #ifdef _DEBUG
                RDebug::Printf("OpenVG Context destroyed %d", err);
                #endif //_DEBUG
                // This indicates that OpenVG has destroyed the current context handle
                // and also the Path, StrokePaint and FillPaint handles are no more valid.
                // Thus reinitialize them to 0, so that they can be recreated.
                ResetContextHandle();
            }
            
            #ifdef _DEBUG
            RDebug::Printf("SVG can't RESIZE OpenVG Context %d", err);
            #endif //_DEBUG
            User::Leave(err);
        }
    }

    iRenderInfo.SetDevice( 0, /*this parameter will be ignored */
                           iColorBufferSize.iWidth,
                           iColorBufferSize.iHeight,
                           0 /*this parameter will be ignored */);
    }

// --------------------------------------------------------------------------
// TInt CGfx2dGcOpenVG::OpenVGErrorToSymbianError( int aError )
// ---------------------------------------------------------------------------
TInt CGfx2dGcOpenVG::OpenVGErrorToSymbianError( TInt aError )
{
    TInt error = KErrNone;
    switch( aError )
    {
        case VGI_OK:
        {
            error = KErrNone;
            break;
        }
        
        case VGU_OUT_OF_MEMORY_ERROR:
        case VG_OUT_OF_MEMORY_ERROR:
        case VGI_ERROR_OUT_OF_MEMORY:
        {
            error = KErrNoMemory;   
            break;
        }
        
        case VG_ILLEGAL_ARGUMENT_ERROR:
        case VGI_ERROR_INVALID_ARGUMENTS:
        case VGU_ILLEGAL_ARGUMENT_ERROR:
        {
            error = KErrArgument; 
            break;
        }
        
        case VGI_ERROR_ALREADY_EXISTS:
        {
            error = KErrAlreadyExists;
            break;
        }
        
        case VG_UNSUPPORTED_PATH_FORMAT_ERROR:
        case VGI_ERROR_COLORSPACE_NOT_SUPPORTED:
        case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
        case VGI_ERROR_NOT_SUPPORTED:
        {
            error = KErrNotSupported; 
            break;
        }
            
        case VGI_ERROR_ILLEGAL_IMAGE_HANDLE:
        {
            error = KErrBadHandle; 
            break;
        }
        
        case VG_IMAGE_IN_USE_ERROR:
        case VGI_ERROR_IMAGE_IN_USE:
        {
            error = KErrInUse; 
            break;
        }
            
        case VGI_ERROR_ILLEGAL_OPERATION:
        {
            error = KErrPermissionDenied;       
            break;
        }
        
        case VG_BAD_HANDLE_ERROR:
        case VG_PATH_CAPABILITY_ERROR:
        case VGU_PATH_CAPABILITY_ERROR:
        case VGU_BAD_WARP_ERROR:
        case VGU_BAD_HANDLE_ERROR:
        {
            error = KErrUnknown;
            break;
        }
    
        default:
        {
            error = KErrUnknown; 
        }
    }
    
    return error;
}
// --------------------------------------------------------------------------
// void CGfx2dGcOpenVG::DoDrawL( MGfxShape* aShape )
// ---------------------------------------------------------------------------
void CGfx2dGcOpenVG::DoDrawL( MGfxShape* aShape )
    {
    VGbitfield drawMode = 0x0;
    VGUErrorCode vguret;

    // Do not render if both stroke and fill are none
    if ( iStrokeColor.GetARGB() == KGfxColorNull && iFillColor == NULL )
        return;

    iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE );

    if ( !iPath )
        {
        const float KScale = 1.0f / 65536.0f;
   iPath = iVgRenderer->vgCreatePath( VG_PATH_FORMAT_STANDARD,
                              VG_PATH_DATATYPE_S_32, KScale, 0.0f, 0, 0,
                              VG_PATH_CAPABILITY_APPEND_TO );
        if ( iPath == VG_INVALID_HANDLE )
            {
                 VGErrorCode error = (VGErrorCode)iVgRenderer->vgGetError();                    
            if (error == VG_OUT_OF_MEMORY_ERROR)
                {
				#ifdef _DEBUG
                RDebug::Printf("OOM: vgCreatePath");
				#endif
                User::Leave(KErrNoMemory);
                }
            // handle other errors...
			#ifdef _DEBUG
            RDebug::Printf("SVG GFX2D Error: vgCreatePath");
			#endif
            }
        }

    if (iFillColor)
	    {
	    if(!iFillPaint)
	    	{
                iFillPaint  = iVgRenderer->vgCreatePaint();
            if ( iFillPaint == VG_INVALID_HANDLE )
                {
                    VGErrorCode error = (VGErrorCode)iVgRenderer->vgGetError();                    
                if (error == VG_OUT_OF_MEMORY_ERROR)
                    {
            		RDebug::Printf("SVG GFX2D Error OOMs: vgCreatePaint");
                    User::Leave(KErrNoMemory);
                    }
                // handle other errors...
			#ifdef _DEBUG
                RDebug::Printf("SVG GFX2D Error: vgCreatePaint");
			#endif
                }
	    	}
	    }

    if(iStrokeColor.GetARGB() != KGfxColorNull)
		{
		if(!iStrokePaint)
			{
			iStrokePaint = iVgRenderer->vgCreatePaint();  
            if ( iStrokePaint == VG_INVALID_HANDLE )
                {
                     VGErrorCode error = (VGErrorCode)iVgRenderer->vgGetError();                    
                if (error == VG_OUT_OF_MEMORY_ERROR)
                    {
                    RDebug::Printf("SVGT CGfx2dGcOpenVG::DoDrawL vgCreatePaint OOM");
                    User::Leave(KErrNoMemory);
                    }
                #ifdef _DEBUG
                RDebug::Printf("SVGT CGfx2dGcOpenVG::DoDrawL vgCreatePaint failed %d", error);
                #endif
                }
			}
		}

	TGfxRectangle2D bbRect;
    TGfxAffineTransform identityMat;

    //vgClear( 0, 0, VG_MAXINT, VG_MAXINT );
    switch(aShape->ShapeType())
        {
        case MGfxShape::ERect:
            {
                TGfxRectangle2D* rect = (TGfxRectangle2D*)aShape;

                rect->GetBounds( identityMat, bbRect );
                drawMode = SetFillStroke(iFillPaint, iStrokePaint, bbRect);
                
                vguret =    (VGUErrorCode)iVgRenderer->vguRect(
                                iPath,
                                (VGfloat)rect->iX,
                                (VGfloat)rect->iY,
                                (VGfloat)rect->iWidth,
                                (VGfloat)rect->iHeight );
                                
                if( vguret != KErrNone )                    
                {
                    User::LeaveIfError(OpenVGErrorToSymbianError(vguret));
                }
   iVgRenderer->vgDrawPath( iPath, drawMode );
            }
            break;

        case MGfxShape::ERoundRect:
            {
            float rx, ry;
            TGfxRoundRectangle2D* rect = (TGfxRoundRectangle2D*)aShape;

            TFloatFixPt tWidth = rect->iArcWidth;
            TFloatFixPt tHeight = rect->iArcHeight;

            tWidth.iValue <<= 1;
            tHeight.iValue <<= 1;

            rx = (VGfloat)tWidth;
            ry = (VGfloat)tHeight;

			//If a properly specified value is provided for ry but not for rx,
			// then the user agent must process the 'rect' element with the
			// effective value for rx as equal to ry.  However clamp to 0.
            if ( rx <= 0 )
                {
                rx = ry;
                }
            if ( ry <= 0)
                {
                ry = rx;
                }

			if(rx < 0)
				{
				rx = 0;
				}
			if(ry < 0)
				{
				ry = 0;
				}

    		rect->GetBounds( identityMat, bbRect );
			drawMode = SetFillStroke(iFillPaint, iStrokePaint, bbRect);

            // Rounded rectangle
            if ( rx == 0.0f && ry == 0.0f )
                {
                    vguret =     (VGUErrorCode)iVgRenderer->vguRect(iPath,
                                (VGfloat)rect->iX, (VGfloat)rect->iY,
                                (VGfloat)rect->iWidth, (VGfloat)rect->iHeight );
                }
            else
                {
                  vguret =   (VGUErrorCode)iVgRenderer->vguRoundRect( iPath,
                                        (VGfloat)rect->iX, (VGfloat)rect->iY,
                                        (VGfloat)rect->iWidth, (VGfloat)rect->iHeight,
                                        rx, ry );
                }
                if( vguret != KErrNone )                    
                {
                    User::LeaveIfError(OpenVGErrorToSymbianError(vguret));
                }    
            iVgRenderer->vgDrawPath( iPath, drawMode );
            }
            break;

        case MGfxShape::EEllipse:
            {
            TGfxEllipse2D* ellipse = (TGfxEllipse2D*)aShape;
	#ifdef SVG_FLOAT_BUILD
	TFloatFixPt rx = ellipse->iWidth * TFloatFixPt(.5f);
	TFloatFixPt ry = ellipse->iHeight * TFloatFixPt(.5f);
	#else
            TFloatFixPt rx = ellipse->iWidth >> 1; // rx = width / 2
            TFloatFixPt ry = ellipse->iHeight >> 1; // ry = height / 2
	#endif
    		ellipse->GetBounds( identityMat, bbRect );
			drawMode = SetFillStroke(iFillPaint, iStrokePaint, bbRect);

            // Element
	          vguret =  (VGUErrorCode)iVgRenderer->vguEllipse(iPath,
                                (VGfloat)(ellipse->iX + rx),
                                (VGfloat)(ellipse->iY + ry),
                                (VGfloat)ellipse->iWidth,
                                (VGfloat)ellipse->iHeight);
                if( vguret != KErrNone )                    
                {
                    User::LeaveIfError(OpenVGErrorToSymbianError(vguret));
                }
                iVgRenderer->vgDrawPath(iPath,drawMode);
            }
            break;


        case MGfxShape::EPath:
            {
            CGfxGeneralPath* gPath = (CGfxGeneralPath*)aShape;
            RArray<TFloatFixPt>* coordArray = gPath->PointCoordsArrayAll();
            if (coordArray->Count() > 0)
            {
                TFloatFixPt* coord = gPath->PointCoordsArray();
                TUint8 count = gPath->Count();
                RArray<TUint32>* type = gPath->PointTypeArray();
                TInt typeCount = type->Count();
            	if(count && ( typeCount == count))
            	{
                    iVgRenderer->vgAppendPathData(iPath, count,gPath->PathSegmentTypeArray(),coord);  
            	}
            	else 
            	{
            	//RArray<TUint32>* type = gPath->PointTypeArray();
                //TInt typeCount = type->Count();
                // this is a multi path
                // Using non-ELeave version and checking for NULL to avoid (ELeave) and TRAP
               if ( iPathSegmentTypeCount < typeCount )
               {
                    if ( iPathSegmentTypes )
                    {
                        delete [] iPathSegmentTypes;
                        iPathSegmentTypes = NULL;
                    }
                    iPathSegmentTypes = new VGubyte[typeCount];
                    iPathSegmentTypeCount = typeCount;
               }
                if ( !iPathSegmentTypes )
                {
                    break;
                }
                for (TInt i=0; i < typeCount; i++)
                    {
                    iPathSegmentTypes[i] = (TGfxSegType) (*type)[i];
                    }
                iVgRenderer->vgAppendPathData(iPath, typeCount, iPathSegmentTypes, coord);
                }
                gPath->GetBounds( identityMat, bbRect );
                drawMode = SetFillStroke(iFillPaint, iStrokePaint, bbRect);
                iVgRenderer->vgDrawPath(iPath,drawMode);
              }
            }
            break;

        case MGfxShape::ELine:
            {
                TGfxLine2D* line = (TGfxLine2D*)aShape;

                // Force Stroke color... if not present
                if(iFillColor && iStrokeColor.GetARGB() == KGfxColorNull)
                    {
                    iStrokeColor = iFillColor->GetColor();
                    }

                line->GetBounds( identityMat, bbRect );
                drawMode = SetFillStroke(iFillPaint, iStrokePaint, bbRect);
                
                 vguret =  (VGUErrorCode)iVgRenderer->vguLine(
                            iPath,
                            (VGfloat)line->iX1,
                            (VGfloat)line->iY1,
                            (VGfloat)line->iX2,
                            (VGfloat)line->iY2);
                
                if( vguret != KErrNone )            
                {
                    User::LeaveIfError(OpenVGErrorToSymbianError(vguret));  
                }

                iVgRenderer->vgDrawPath(iPath,drawMode);
            }
            break;
        }
        iVgRenderer->vgClearPath( iPath, VG_PATH_CAPABILITY_APPEND_TO );

        // Delete path segment type array to avoid large memory to persist,
        // created by some content, but the larget size is not needed later.
        if ( iPathSegmentTypeCount > KPathSegmentTypeMaxSize )
        {
            if ( iPathSegmentTypes )
                {
                delete [] iPathSegmentTypes;
                iPathSegmentTypes = NULL;
                iPathSegmentTypeCount = 0;
                }
        }

        // Turn off any settings that are not needed by default
        PostDraw();
    }

void CGfx2dGcOpenVG::PostDraw()
{
    if ( iDashArray && iDashArray->Count() > 0 )
    {
        iVgRenderer->vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
        iDashArray->Reset();
    }
}

// --------------------------------------------------------------------------
// VGbitfield CGfx2dGcOpenVG::SetFillStroke(VGPaint aFillPaint, VGPaint aStrokePaint, TGfxRectangle2D& bBox)
// ---------------------------------------------------------------------------
VGbitfield CGfx2dGcOpenVG::SetFillStroke(VGPaint aFillPaint, VGPaint aStrokePaint, TGfxRectangle2D& bBox)
{
	VGbitfield returnVal = 0x0;

    // Fill Color
    if (iFillColor)
        {
            if( aFillPaint != VG_INVALID_HANDLE )
            {
                iFillColor->SetFill(aFillPaint, bBox, iFillOpacity, iVgRenderer);
                returnVal |= VG_FILL_PATH;      
            }
        }

    //
    //  Color Correction for Symbian->OpenVG
    //              Symbian argb
    //               OpenVg rgba
    //
    // Stroke
    //
    if( iStrokeColor.GetARGB() != KGfxColorNull && iStrokeColor.GetARGB() != 0x1ffffff )
		{
            iVgRenderer->vgSetf( VG_STROKE_LINE_WIDTH, (VGfloat)iStroke.StrokeWidth() );
            
            //SET LINECAP INFORMATION
            SVGLineCap lineCap = (SVGLineCap)iStroke.StrokeCap();

            if (lineCap == SVG_CAP_ROUND)
            {
                iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
            }
            else if ( lineCap == SVG_CAP_SQUARE )
            {
                iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_SQUARE);
            }
            else
            {
                iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_BUTT);
            }

            //SET LINEJOIN INFORMATION
            SVGLineJoin  linejoin = (SVGLineJoin)iStroke.StrokeJoin();
            if (linejoin == SVG_JOIN_ROUND)
            {
                iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
            }
            else if (linejoin == SVG_JOIN_BEVEL)
            {
                iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_BEVEL);
            }
            else
            {
                iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
            }

            iVgRenderer->vgSetf( VG_STROKE_MITER_LIMIT, (VGfloat)iStroke.StrokeMiterLimit() );
            if(aStrokePaint != VG_INVALID_HANDLE)
            {
                iVgRenderer->vgSetParameteri( aStrokePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
                TUint32 opacity = (TInt)(iStrokeOpacity * TFloatFixPt(255.0f));
                iVgRenderer->vgSetColor( aStrokePaint, iStrokeColor.GetARGB() << 8 | opacity );
                iVgRenderer->vgSetPaint( aStrokePaint, VG_STROKE_PATH );
                returnVal |= VG_STROKE_PATH;    
            }
		}

	return returnVal;
}

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetWindingRule( TGfxWindingRule aRule )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetWindingRule( TGfxWindingRule aRule )
    {
    if ( aRule == EGfxWindEvenOdd )
        {
             iVgRenderer->vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
        }
    else
        {
             iVgRenderer->vgSeti(VG_FILL_RULE, VG_NON_ZERO);
        }
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::Draw( MGfxShape* aShape )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::DrawL( MGfxShape* aShape )
    {
    DoDrawL( aShape );
    }



// --------------------------------------------------------------------------
//  TGfxColor CGfx2dGcOpenVG::ForegroundColor()
// ---------------------------------------------------------------------------
 TGfxColor CGfx2dGcOpenVG::ForegroundColor()
    {
    return iStrokeColor;
    }


// --------------------------------------------------------------------------
//  MGfxPaint* CGfx2dGcOpenVG::Paint()
// ---------------------------------------------------------------------------
 MGfxPaint* CGfx2dGcOpenVG::Paint()
    {
    return iFillColor;
    }

// --------------------------------------------------------------------------
//  TGfxRenderingHints* CGfx2dGcOpenVG::RenderingHints()
// ---------------------------------------------------------------------------
 TGfxRenderingHints* CGfx2dGcOpenVG::RenderingHints()
    {
    //return &iRenderingHints;
    return NULL;
    }

// --------------------------------------------------------------------------
//  TGfxStroke CGfx2dGcOpenVG::Stroke()
// ---------------------------------------------------------------------------
 TGfxStroke CGfx2dGcOpenVG::Stroke()
    {
    return iStroke;
    }



// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetDashOffset( TFloatFixPt aDashOffset )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetDashOffset( TFloatFixPt aDashOffset )
{
	iDashOffset = aDashOffset;
    iVgRenderer->vgSetf(VG_STROKE_DASH_PHASE, (VGfloat)iDashOffset);
//	svgSetStrokeDashOffset(iHyb2dContext, svgScalarFromFloat(iDashOffset));
}


// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetForegroundColor( const TGfxColor& aColor )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetForegroundColor( const TGfxColor& aColor )
    {
    // Both GDI and Gfx2D need to keep color value...
    iStrokeColor = aColor;
    }
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetStrokeWidth( const TGfxStroke& aColor )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetStrokeWidth( const TFloatFixPt& aWidth )
    {
    // Both GDI and Gfx2D need to keep stroke-width value...
    iStroke.SetStrokeWidth(aWidth);
    
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetBackgroundColor( const TGfxColor& aColor )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetBackgroundColor( const TGfxColor& aColor )
    {
    // Only Gfx2D need to keep background color value.
    iBackgroundColor = aColor;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetPaint( MGfxPaint* aPaint )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetPaint( MGfxPaint* aPaint )
    {
    iFillColor = aPaint;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetStroke( TGfxStroke aStroke )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetStroke( TGfxStroke aStroke )
    {
    iStroke = aStroke;
        iVgRenderer->vgSetf( VG_STROKE_LINE_WIDTH, (VGfloat)iStroke.StrokeWidth() ); 

	//SET LINECAP INFORMATION
	SVGLineCap lineCap = (SVGLineCap)iStroke.StrokeCap();

	if (lineCap == SVG_CAP_ROUND)
	{
		    iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
	}
	else if ( lineCap == SVG_CAP_SQUARE )
	{
		    iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_SQUARE);
	}
	else
	{
    		iVgRenderer->vgSeti( VG_STROKE_CAP_STYLE, VG_CAP_BUTT);
	}

	//SET LINEJOIN INFORMATION
	SVGLineJoin  linejoin = (SVGLineJoin)iStroke.StrokeJoin();
	if (linejoin == SVG_JOIN_ROUND)
	{
		    iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
	}
	else if (linejoin == SVG_JOIN_BEVEL)
	{
            iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_BEVEL);
	}
	else
	{
		    iVgRenderer->vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
	}

	iVgRenderer->vgSetf( VG_STROKE_MITER_LIMIT, (VGfloat)iStroke.StrokeMiterLimit() );

    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetRenderingHints( const TGfxRenderingHints& aRenderingHints )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetRenderingHints( const TGfxRenderingHints& /*aRenderingHints*/ )
    {
     //iRenderingHints = aRenderingHints;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetTransform( const TGfxAffineTransform& aTransform )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetTransform( const TGfxAffineTransform& aTransform )
    {
    iTransform = aTransform;

    // Calculate scale
    TGfxPoint2D ep( 1, 0 ), org( 2, 0 );
    iTransform.Transform( &ep, & ep, 1 );
    iTransform.Transform( &org, & org, 1 );
    ep.iX -= org.iX;
    ep.iY -= org.iY;
    iScale = TFloatFixPt::Sqrt( ep.iX * ep.iX + ep.iY * ep.iY );

    // activate Graphics 2D engine
    // set viewport coordinate system to map from canvas coordinates to pixels
    // in order to provide resolution independent results
    // SVGMatrix2x3 vmat;
    // svgMatrix2x3Set( &vmat,
    //    iTransform.iM00.iValue,
    //    iTransform.iM01.iValue,
    //    iTransform.iM02.iValue,
    //    iTransform.iM10.iValue,
    //    iTransform.iM11.iValue,
    //    iTransform.iM12.iValue
    //    );
//    svgSetViewportCoordinateSystem( iHyb2dContext, vmat.matrix );
	ApplyTransform(iTransform);
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetClip( const TGfxRectangle2D& aClip )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetClip( const TGfxRectangle2D& aClip )
    {
    iClip = aClip;
    TFloatFixPt newY = iColorBufferSize.iHeight - (TInt)aClip.iHeight - (TInt)aClip.iY;
    VGint clipRect[4] =
    	{
    		(TInt)aClip.iX, (TInt)newY, (TInt)aClip.iWidth, (TInt)aClip.iHeight
    	};

    // Set VG_SCISSORING to True
         iVgRenderer->vgSeti(VG_SCISSORING, VG_TRUE);

    // Set Clippling rectangle
    iVgRenderer->vgSetiv(VG_SCISSOR_RECTS, 4,(const TInt*) clipRect);
    }

 void CGfx2dGcOpenVG::SetClipMediaElement( TSize aSize )
    {
        TFloatFixPt width = (TFloatFixPt)aSize.iWidth;
        TFloatFixPt height = (TFloatFixPt)aSize.iHeight;
        TFloatFixPt x = (TFloatFixPt)0;
        TFloatFixPt y = (TFloatFixPt)0;
        TGfxRectangle2D aClip(x,y,width,height);
        iClipMain = iClip;
        iClip = aClip;
       // TFloatFixPt newY = aSize.iHeight - (TInt)aClip.iHeight - (TInt)aClip.iY;
        VGint clipRect[4] =
    	{
    		(TInt)aClip.iX, (TInt)aClip.iY, (TInt)aClip.iWidth, (TInt)aClip.iHeight
    	};

    // Set VG_SCISSORING to True
    iVgRenderer->vgSeti(VG_SCISSORING, VG_TRUE);

    // Set Clippling rectangle
    iVgRenderer->vgSetiv(VG_SCISSOR_RECTS, 4, (const TInt*)clipRect);    
        
    }
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetDashArrayL( CArrayFix<TFloatFixPt>* aArray )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetDashArrayL( CArrayFix<TFloatFixPt>* aArray )
    {
    if ( !aArray || !iDashArray )
        {
        return;
        }

    TInt32 count = aArray->Count();

    if ( count == 0 )
        {
        return;
        }

    if ( iDashArray->Count() > 0 )
        {
        iDashArray->Reset();
        }

    for ( TInt32 i = 0; i < count; i++ )
        {
            iDashArray->AppendL( (VGfloat)aArray->At( i ) );
        }

    if ( count & 0x00000001 )
        {
        //count was odd so we need to duplicate the current ones on the array
        for (TInt32 i=0; i < count; i++)
            {
            iDashArray->AppendL( (VGfloat)aArray->At( i ) );
            }
        }

    iVgRenderer->vgSetfv(VG_STROKE_DASH_PATTERN, iDashArray->Count(), (VGfloat*)&(*iDashArray)[0] );
    }

// --------------------------------------------------------------------------
//  TSize CGfx2dGcOpenVG::DeviceBounds()
// ---------------------------------------------------------------------------
 TSize CGfx2dGcOpenVG::DeviceBounds()
    {
    return iColorBufferSize;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetAntialiasingMode( const TInt32 /*aAntialiasingEnable*/)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetAntialiasingMode( const TInt32 aAntialiasingMode )
    {
    
    // JSR226 has two values for Render Quality
    //--------------------------------------------------------------------------------
    //	JSR226/M2G Values       :   TInt Value  :       OpenVG Constants
    //--------------------------------------------------------------------------------
    // RENDERING_QUALITY_LOW    :    = 1 =      : VG_RENDERING_QUALITY_NONANTIALIASED
    // RENDERING_QUALITY_HIGH   :    = 2 =      : VG_RENDERING_QUALITY_BETTER
    //--------------------------------------------------------------------------------.
    
    if( aAntialiasingMode == KRenderQualityLow ) // RENDERING_QUALITY_LOW
    	{
    	iRenderQuality = VG_RENDERING_QUALITY_NONANTIALIASED ;
    	}
    else if( aAntialiasingMode == KRenderQualityHigh ) // RENDERING_QUALITY_HIGH
    	{
    	iRenderQuality = VG_RENDERING_QUALITY_BETTER ;
    	}
    	
    iVgRenderer->vgSeti(VG_RENDERING_QUALITY, iRenderQuality );
   	
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFont( const CFont* aFont )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFont( const CFont* aFont )
    {
     iFont = (CFont*)aFont;
    }
// --------------------------------------------------------------------------
//  CFont* CGfx2dGcOpenVG::Font()
// ---------------------------------------------------------------------------
 CFont* CGfx2dGcOpenVG::Font()
    {
	if (iFont == NULL)
	{
	iSvgBitmapFontProvider->GetNearestFontToDesignHeightInTwips( iFont, iFontSpec ); 
    
	}
    return iFont;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::GetSystemFontScaled(TFloatFixPt aHeight,
//                                     const TDesC& aTypefaceName,
//                                     CFont*& aFont,
//                                     TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::GetFontScaled( TFloatFixPt /* aHeight */,
                             const TDesC& /* aTypefaceName */,
                             CFont*& /* aFont */,
                             TFontSpec& /* aFontSpec */ )
{
}

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFontSize( const TFloatFixPt aFontSize )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFontSize( const TFloatFixPt aFontSize )
    {
    iFontSize = aFontSize;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFontWeight( const TInt32 aFontWeight )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFontWeight( const TInt32 aFontWeight )
    {
    iFontWeight = aFontWeight;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFontStyle( const TInt32 aFontStyle )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFontStyle( const TInt32 aFontStyle )
    {
    iFontStyle = aFontStyle;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFontFamily( CDesCArrayFlat* aFamilies )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFontFamily( CDesCArrayFlat* aFamilies )
    {
    iFamilies = aFamilies;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetTextanchor( const TInt32 aTextAnchor )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetTextanchor( const TInt32 aTextAnchor )
    {
    iTextAnchor = (TGfxTextAnchor)aTextAnchor;
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetTextDecoration( const TInt32 aTextDecoration )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetTextDecoration( const TInt32 aTextDecoration )
    {
    iTextDecoration = (TGfxTextDecoration)aTextDecoration;
    }
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::GetBBoxForSystemText( TSvgFourPointRect& aFourPointBbox )
// ---------------------------------------------------------------------------
TGfxRectangle2D CGfx2dGcOpenVG::GetBBoxForSystemText( const TDesC& aText, 
                                     TFloatFixPt aX,
                                     TFloatFixPt aY )
{
	//bbox for bitmap fonts
    TGfxRectangle2D lBbox;
	TGfxPoint2D pt(aX,aY);
	//iScale = GetCurrentScale();

	//scaled already for bitmap text...
	
    TInt textAdvance = iFont->TextWidthInPixels( aText );
    TInt fontMaxHeight = iFont->FontMaxHeight();
    TFloatFixPt lascent = iFont->AscentInPixels();
    TFloatFixPt ldescent = TFloatFixPt(fontMaxHeight) - (TFloatFixPt)iFont->AscentInPixels();


	lBbox.iWidth = textAdvance;
    lBbox.iHeight = fontMaxHeight;

	//need to work in the rotation factor somehow...
	//without doing the scaling again
    //at this point the bbox already scaled,translated but not rotated

    //should we create 1/scale factor or create with scale factor then get inverse
    //iScale scaling factor from the graphics context
    TReal32 linverseScale = 1.0/(TReal)iScale;
    TGfxAffineTransform lInverseScaleMat = TGfxAffineTransform::GetScaleInstance(linverseScale, linverseScale);

		//this is with scale and rotation
    TGfxPoint2D transformedPoint( pt.iX, pt.iY );

    const TGfxAffineTransform& ctm = iTransform;
    ctm.Transform(&pt, &transformedPoint, 1);

    //remove the double scaling effect that we would have
    TGfxAffineTransform ctmWithoutScale = iTransform;
    ctmWithoutScale.Concatenate(lInverseScaleMat);

	TGfxPoint2D scaledPoint( pt.iX * iScale, pt.iY * iScale );

		//set up the initial box with just scale in it and not rotation.
		// The base line is position at x,y co-ordinate of the text, but
		// the bottom of bound box would be further below by amount ldescent.
    TGfxPoint2D lTopLeftCorner(scaledPoint.iX, scaledPoint.iY + ldescent );
    TGfxPoint2D lTopRightCorner(scaledPoint.iX + lBbox.iWidth , scaledPoint.iY + ldescent );
    TGfxPoint2D lBottomLeftCorner(scaledPoint.iX, scaledPoint.iY - lascent);
	TGfxPoint2D lBottomRightCorner(scaledPoint.iX + lBbox.iWidth, scaledPoint.iY - lascent );

    TGfxPoint2D lTransTopLeftCorner;
    TGfxPoint2D lTransBottomLeftCorner;
    TGfxPoint2D lTransTopRightCorner;
    TGfxPoint2D lTransBottomRightCorner;

		//applying current rotation transform without the scale.
    ctmWithoutScale.Transform(&lTopLeftCorner, &lTransTopLeftCorner,1);
    ctmWithoutScale.Transform(&lTopRightCorner, &lTransTopRightCorner,1);
    ctmWithoutScale.Transform(&lBottomLeftCorner, &lTransBottomLeftCorner,1);
    ctmWithoutScale.Transform(&lBottomRightCorner, &lTransBottomRightCorner,1);

    TFloatFixPt lXTranslation = transformedPoint.iX - lTransTopLeftCorner.iX;
    //TFixPt lXTranslation = transformedPoint.iX - iPoint.iX;

    TFloatFixPt lYTranslation = transformedPoint.iY - lTransTopLeftCorner.iY;
		//TFixPt lYTranslation = transformedPoint.iY - iPoint.iY;

    // X position is affected by anchor
    switch( iTextAnchor )
    {
       	case EGfxTextAnchorMiddle:
       		lXTranslation = lXTranslation - ( lBbox.iWidth / TFloatFixPt( 2 ) );
        break;
       	case EGfxTextAnchorEnd:
           	lXTranslation = lXTranslation - lBbox.iWidth;
        break;
       	default:
       	 	//do nothing
       	break;
    }

    lTransTopLeftCorner.iX += lXTranslation;
    lTransTopLeftCorner.iY += lYTranslation;

    lTransTopRightCorner.iX += lXTranslation;
    lTransTopRightCorner.iY += lYTranslation;

    lTransBottomLeftCorner.iX += lXTranslation;
    lTransBottomLeftCorner.iY += lYTranslation;

    lTransBottomRightCorner.iX += lXTranslation;
    lTransBottomRightCorner.iY += lYTranslation;

    //at this point I have the exact path coordinates of the bbox
    //aFourPointBbox.SetRectPoints(lTransTopLeftCorner, lTransTopRightCorner, lTransBottomLeftCorner, lTransBottomRightCorner);
    return GetTRect(lTransTopLeftCorner, lTransTopRightCorner, lTransBottomLeftCorner, lTransBottomRightCorner);
}


TGfxRectangle2D CGfx2dGcOpenVG::GetTRect(TGfxPoint2D lTransTopLeftCorner,TGfxPoint2D lTransTopRightCorner,TGfxPoint2D lTransBottomLeftCorner,TGfxPoint2D lTransBottomRightCorner)
{
	TFloatFixPt lMaxX = lTransTopLeftCorner.iX;

    if (lTransTopRightCorner.iX > lMaxX)
    {
    	lMaxX = lTransTopRightCorner.iX;
    }
    if (lTransBottomLeftCorner.iX > lMaxX)
    {
    	lMaxX = lTransBottomLeftCorner.iX;
    }
    if (lTransBottomRightCorner.iX > lMaxX)
    {
    	lMaxX = lTransBottomRightCorner.iX;
    }

    TFloatFixPt lMaxY = lTransTopLeftCorner.iY;

    if (lTransTopRightCorner.iY > lMaxY)
    {
    	lMaxY = lTransTopRightCorner.iY;
    }
    if (lTransBottomLeftCorner.iY > lMaxY)
    {
    	lMaxY = lTransBottomLeftCorner.iY;
    }
    if (lTransBottomRightCorner.iY > lMaxY)
    {
    	lMaxY = lTransBottomRightCorner.iY;
    }

   	TFloatFixPt lMinX = lTransTopLeftCorner.iX;

    if (lTransTopRightCorner.iX < lMinX)
    {
    	lMinX = lTransTopRightCorner.iX;
    }
    if (lTransBottomLeftCorner.iX < lMinX)
    {
    	lMinX = lTransBottomLeftCorner.iX;
    }
    if (lTransBottomRightCorner.iX < lMinX)
    {
    	lMinX = lTransBottomRightCorner.iX;
    }

    TFloatFixPt lMinY = lTransTopLeftCorner.iY;

    if (lTransTopRightCorner.iY < lMinY)
    {
    	lMinY = lTransTopRightCorner.iY;
    }
    if (lTransBottomLeftCorner.iY < lMinY)
    {
    	lMinY = lTransBottomLeftCorner.iY;
    }
    if (lTransBottomRightCorner.iY < lMinY)
    {
    	lMinY = lTransBottomRightCorner.iY;
    }
	TGfxRectangle2D aRect;
    aRect.iX = lMinX;
    aRect.iY = lMinY;
    aRect.iWidth = lMaxX - lMinX;
    aRect.iHeight = lMaxY - lMinY;
    return aRect;

}

// Text Drawing
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::DrawStringL( const TDesC& aDesc,
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::DrawStringL( const TDesC& aDesc,
                                     TFloatFixPt aX,
                                     TFloatFixPt aY,
                                     TInt8 ,// aTextAnchor
                                     TInt8 ,// aTextDecoration
                                     CDesCArrayFlat*, // aFamilies,
                                     TReal aWordSpacing,
                                     TReal aLetterSpacing,
                                     CArrayFix<TReal32>* aArrayRotate,
                                     CArrayFix<TFloatFixPt>* aArrayX,
                                     CArrayFix<TFloatFixPt>* aArrayY,
				     TSize& aBoundingBox,
				     CFont*& aFont,
				     TFontSpec& /* aFontSpec */ )
    {

    if ( iStrokeColor.GetARGB() == KGfxColorNull &&
         iFillColor == NULL )
        return;

	if(iFillOpacity == TFloatFixPt(0))
		return;

    if ( aDesc.Length() <= 0 )
        return;

    TPtrC outputText;
    if ( *( aDesc.Right( 1 ).Ptr() ) < ' ' )
        outputText.Set( aDesc.Left( aDesc.Length() - 1 ) );
    else
        outputText.Set( aDesc );

    TBool rtl=EFalse;
    if(TBidiText::TextDirectionality(outputText) == TBidiText::ERightToLeft)
    {
        rtl=ETrue;
    }
    
    if ( aFont == NULL )
        {
        return;
        }
    iFont = aFont;

	float aFloat = 0.5;

	//put in .50 for rounding issues 10.99 was going to 10
	// iFontSize has been initialised at the SetGCL time for the
	// this specific text element in question.
	TInt32 fontsize = (TInt32)( iScale * iFontSize + TFloatFixPt(aFloat) );

    if ( fontsize == 0 )
        {
        return;
        }


    CFont::TMeasureTextOutput myTextOutput;

    TInt textAdvance = iFont->TextWidthInPixels( outputText );
    if ( textAdvance == 0 )
        {
        return;
        }
    TInt fontMaxHeight = iFont->FontMaxHeight() + 2; //Tweak factor needed
    TInt fontMaxAscent = iFont->FontMaxAscent() + 1; //Tweak factor needed
    TInt fontAscent = iFont->AscentInPixels();
    TInt fontDescent = iFont->DescentInPixels();
    TInt maxDescent = fontMaxHeight-fontMaxAscent; //iFont->FontMaxDescent();
    TInt fontHeightInPixels = iFont->HeightInPixels();
    //added to correct for text clipping that system font has
    TInt fontTweakFactor = (TInt)(TFloatFixPt( 1.5f ) * iScale);
    TInt fontHorizontalTweakFactor = (TInt)(TFloatFixPt( 5.0f ) * iScale);
    TInt numSpaces = myTextOutput.iSpaces;
    TInt numChars = myTextOutput.iChars;
    TGfxRectangle2D tempRect( aX,
                              aY,
                              textAdvance,
                              fontMaxHeight );

    /******************************************************************
    * Boundary Check to see if the Text to be drawn is within Bounds. *
    ******************************************************************/

    TGfxAffineTransform tMatrix;
    tMatrix = iTransform;
	TInt32 transx = tMatrix.TranslateX();
	TInt32 transy = tMatrix.TranslateY();
    TGfxRectangle2D aRect = GetBBoxForSystemText( outputText , aX , aY );
    
	TInt32 rectmaxx = aRect.MaxX(); //+ (TFloatFixPt)transx;
	TInt32 rectmaxy = aRect.MaxY();//- (TFloatFixPt)fontMaxAscent; //+ (TFloatFixPt)transy;
	TInt32 rectminx = aRect.MinX(); //- (TFloatFixPt)transx;
	TInt32 rectminy = aRect.MinY();// - (TFloatFixPt)fontMaxHeight;//- (TFloatFixPt)fontMaxAscent; //- (TFloatFixPt)transy;
	TInt32 clipminx = iRenderInfo.iClipMinX;// + transx;
	TInt32 clipmaxx = iRenderInfo.iClipMaxX;// + transx;
	TInt32 clipmaxy = iRenderInfo.iClipMaxY;// + transy;
	TInt32 clipminy = iRenderInfo.iClipMinY;// + transy;
	
    TInt advanceWidth = 0;
    TInt advanceX = 0;
    TInt excessSpace = 0;
    TInt aMultipleXY = 0;

    //Checking for multiple XY values.
    if ( aArrayX != NULL && aArrayY != NULL )
        {
        if ( ( aArrayX->Count() > 1 ) || ( aArrayY->Count() > 1 ) )
            aMultipleXY = 1;
        }

	// setting the boundbox for simple text with no xy and rotate 
	//this is set here because bound box should get always set as it is used in GetBBoxForSystemText in TextElement.
	if ( ( aArrayRotate == NULL ) &&
	( aMultipleXY == 0 ) &&
	( aLetterSpacing == 0.0 ) &&
	( aWordSpacing == 0.0 ) )
		{
			aBoundingBox.iWidth = textAdvance;
			aBoundingBox.iHeight = fontMaxHeight;
		}
	
	if( rectmaxx < clipminx || rectmaxy < clipminy ||
	rectminx > clipmaxx || rectminy > clipmaxy )
	return;
	
    /****************************************************************************
    *   Code to Draw Simple Text with only x, y and transform values and return.*
    ****************************************************************************/

    if ( ( aArrayRotate == NULL ) &&
         ( aMultipleXY == 0 ) &&
         ( aLetterSpacing == 0.0 ) &&
         ( aWordSpacing == 0.0 ) )
        {
        CFbsBitmap* offScreenBitmap = new ( ELeave ) CFbsBitmap();

        User::LeaveIfError( offScreenBitmap->Create( TSize( textAdvance, fontMaxHeight), EGray256 ) );
//        User::LeaveIfError( offScreenBitmap->Create( TSize( textAdvance + fontHorizontalTweakFactor, fontMaxHeight + fontTweakFactor ), EGray256 ) );
        CleanupStack::PushL( offScreenBitmap );

        CGraphicsContext* bitmapContext = NULL;
        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( offScreenBitmap );

        CleanupStack::PushL( bitmapDevice );
        User::LeaveIfError( bitmapDevice->CreateContext( bitmapContext ) );
        CleanupStack::PushL( bitmapContext );

        bitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
        bitmapContext->SetBrushColor( 0x000000 );
        bitmapContext->DrawRect( TRect( TPoint( 0, 0 ), offScreenBitmap->SizeInPixels() ) );
        bitmapContext->UseFont( iFont );
        bitmapContext->SetPenSize( TSize( 1, 1 ) );
        bitmapContext->SetPenColor( 0xFFFFFF );
        bitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
        bitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );

        if (iTextAnchor == EGfxTextAnchorEnd)
        {
        	aX -= TFloatFixPt(textAdvance) / iScale;
        }
        if (iTextAnchor == EGfxTextAnchorMiddle)
        {
        	aX -=  (TFloatFixPt(textAdvance) / iScale) / TFloatFixPt(2);
        }
		
		// While drawing the text which has both arabic & normal left-to-right
		// characters the DrawTextExtended should be used since there is no way
		// to split the text according to the directionality.
		
      	CGraphicsContext::TDrawTextExtendedParam param;
      	param.iParRightToLeft = rtl;
        bitmapContext->DrawTextExtended(outputText,
                                        TPoint( 0, fontMaxAscent ), param);
        
		VGint width = offScreenBitmap->SizeInPixels().iWidth;
		VGint height = offScreenBitmap->SizeInPixels().iHeight;

		/* The bilinear causes the text to blur a bit, but rotated (& scaled) text will look better, of course */
          VGImage img = iVgRenderer->vgCreateImage( VG_A_8, width, height, VG_IMAGE_QUALITY_NONANTIALIASED );
		//VGImage img = vgCreateImage( VG_A_8, width, height, VG_IMAGE_QUALITY_BETTER );
		//VGImage img = vgCreateImage( VG_A_8, width, height, VG_IMAGE_QUALITY_FASTER );

		    VGPaint paint = iVgRenderer->vgCreatePaint();

		if( img && paint )
		{
		    offScreenBitmap->LockHeap();

		    /* OpenVG images have their y-axis flipped,
		     that is why when calling vgImageSubData we pass the address of offScreenBitmap's last scanline,
		     and use a negated stride value */
		    VGint stride = CFbsBitmap::ScanLineLength( width, EGray256 ); /* EGray256 == offScreenBitmap->DisplayMode() */
			VGbyte *lastScanline = ( (VGbyte*)offScreenBitmap->DataAddress() ) + ( height - 1 ) * stride;

            iVgRenderer->vgImageSubData( img, lastScanline, -stride, VG_A_8, 0, 0, width, height );    

		    offScreenBitmap->UnlockHeap();

            TGfxColor color( iFillColor ? iFillColor->GetColor() : iStrokeColor.GetColor() );
        	VGuint argb = color.GetARGB() | ( 0xff000000 ); /* set alpha to 255 */
        	VGfloat rgba_f[4];

			int tr, tg, tb;

        	tr = (argb >> 16) & 0xff;
        	tg = (argb >> 8 ) & 0xff;
        	tb = (argb 		) & 0xff;

	        rgba_f[0] = tr / 255.0f;
	        rgba_f[1] = tg / 255.0f;
	        rgba_f[2] = tb / 255.0f;
	        //rgba_f[3] = ( ( argb >> 24 ) & 0xff ) / 255.0f;
	        rgba_f[3] = 1.0f; /* was set to 255 */
            if( paint != VG_INVALID_HANDLE )
            {
                iVgRenderer->vgSetParameteri( paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
                iVgRenderer->vgSetParameterfv( paint, VG_PAINT_COLOR, 4, rgba_f );
                iVgRenderer->vgSetPaint( paint, VG_FILL_PATH );
            }

            // Get the current blend mode (set back at end of operation)
            const TInt KBlendModeValue = iVgRenderer->vgGeti( VG_BLEND_MODE );
            const TInt KImageModeValue = iVgRenderer->vgGeti( VG_IMAGE_MODE );

			           
            iVgRenderer->vgSeti( VG_BLEND_MODE, VG_BLEND_SRC_OVER );
			iVgRenderer->vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY );
			VGfloat mat[9];

        	/* Retreive the Path transformation matrix */
            GetMatrix( mat );   
	        iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE );
            
      
	    	iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE );   	
        
	    	/*	Since the text rendering has to match AA done by ext, we'll round the transformation.
	    		Seems like Symbian does not have a 32-bit float round... */

	    	TReal tx = mat[6], ty = mat[7];
	    	Math::Round(tx, tx, 0); Math::Round(ty, ty, 0);

	    	mat[6] = (VGfloat)tx;
	    	mat[7] = (VGfloat)ty;

        	/* Load Path transformation matrix to Image transformation matrix */
/*nga	    	vgLoadMatrix( mat );

//            TFloatFixPt aY1( aY * iScale );
//            aY -= ( iFont->AscentInPixels());
	    	//vgTranslate( (VGfloat)aX, (VGfloat)aY + fontTweakFactor);
	    	vgTranslate( (VGfloat)aX, (VGfloat)aY );
	    	vgScale( 1.0f/(VGfloat)iScale, -1.0f/(VGfloat)iScale );
	    	vgTranslate( (VGfloat)0, -(VGfloat)maxDescent);
            vgDrawImage( img );
            
            vgSeti( VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE );
            vgLoadIdentity();

            vgSeti( VG_IMAGE_MODE, KImageModeValue );
            vgSeti( VG_BLEND_MODE, KBlendModeValue );*/
   	    	iVgRenderer->vgLoadMatrix( mat );
	    	iVgRenderer->vgTranslate( (VGfloat)aX, (VGfloat)aY );
	    	iVgRenderer->vgScale( 1.0f/(VGfloat)iScale, -1.0f/(VGfloat)iScale );
	    	iVgRenderer->vgTranslate( (VGfloat)0, -(VGfloat)maxDescent);
            iVgRenderer->vgDrawImage( img );
            
            iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE );
            iVgRenderer->vgLoadIdentity();

            iVgRenderer->vgSeti( VG_IMAGE_MODE, KImageModeValue );
            iVgRenderer->vgSeti( VG_BLEND_MODE, KBlendModeValue );
		}

		if( img )
		    {
		        iVgRenderer->vgDestroyImage( img );
		    }
		if( paint )
		    {
		        iVgRenderer->vgDestroyPaint( paint );
		    }

        bitmapContext->DiscardFont();

        CleanupStack::PopAndDestroy( 3 );

        if ( iTextDecoration == EGfxTextDecorationUnderLine )
            {
            TGfxLine2D aLine( aX,
                              aY,
                             (aX + ( TFloatFixPt )textAdvance / iScale ),
                              aY); //+ ( TFloatFixPt )(2));
            DrawL( &aLine );
            }
        else  if ( iTextDecoration == EGfxTextDecorationOverLine )
            {
            TGfxLine2D aLine( aX,
//                              aY - ( TFloatFixPt )fontMaxHeight / iScale,
                              aY,
                             (aX + ( TFloatFixPt )textAdvance / iScale  ),
                              aY);// - ( TFloatFixPt )fontMaxHeight / iScale );
            DrawL( &aLine );
            }
         else  if ( iTextDecoration == EGfxTextDecorationLineThrough )
            {
            TGfxLine2D aLine( aX ,
                              aY - ( TFloatFixPt )( fontMaxHeight / 2 ) / iScale,
                             (aX + ( TFloatFixPt )textAdvance /iScale  ),
                              aY - ( TFloatFixPt )( fontMaxHeight / 2 ) / iScale );
            DrawL( &aLine );
            }

     //-----------------------------------------------------------------
        return;
        }


    //Check Word/Letter Spacing.
    if ( aWordSpacing > 0.0 )
        excessSpace += ( TInt )
                       ( aWordSpacing * ( TReal ) iScale ) * numSpaces;
    if ( aLetterSpacing > 0.0 )
        excessSpace += ( TInt )
                       ( aLetterSpacing * ( TReal ) iScale ) * numChars;

    textAdvance += excessSpace;

    /*****************************************************************************
    * Drawing one glyph(character) at a time for multiple XY and rotation Values.*
    ******************************************************************************/
    if (rtl)
        {
        TBidiText* biditext = TBidiText::NewL(outputText,1);
        biditext->WrapText(textAdvance,*iFont,0,1);
        outputText.Set(biditext->DisplayText());
        delete biditext;
    	biditext = NULL;

        }
    TLex aOutputText( outputText );
    for ( TInt count = 0; count < outputText.Length() ; count++ )
        {
        TPtrC outputChar;
        TChar ch( aOutputText.Get() );
        outputChar.Set( &( outputText.operator[]( count ) ), 1 );

        TInt charWidth = iFont->CharWidthInPixels( ch );

        CFbsBitmap* offScreenBitmap = new ( ELeave ) CFbsBitmap();
        User::LeaveIfError( offScreenBitmap->Create( TSize( charWidth,
                                                            fontMaxHeight + fontTweakFactor ),
                                                     EColor16MU ) );
        CleanupStack::PushL( offScreenBitmap );

        CGraphicsContext* bitmapContext = NULL;
        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( offScreenBitmap );

        CleanupStack::PushL( bitmapDevice );
        User::LeaveIfError( bitmapDevice->CreateContext( bitmapContext ) );
        CleanupStack::PushL( bitmapContext );

        TBool isWhite = EFalse;
        if ( iFillColor != NULL )
            {
            if ( ( ( TGfxColor * ) iFillColor )->GetARGB() == 0xffffff )
                isWhite = ETrue;
            }
        else
            {
            if ( iStrokeColor.GetARGB() == 0xffffff )
                isWhite = ETrue;
            }
        if ( isWhite )
            {
            // Fill with black if text color is white
            TRect bmprect( 0, 0, charWidth, fontMaxHeight );
            bitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
            bitmapContext->SetBrushColor( 0x000000 );
            bitmapContext->SetPenColor( 0x000000 );
            bitmapContext->DrawRect( bmprect );
            }

        bitmapContext->UseFont( iFont );
        bitmapContext->SetPenSize( TSize( 1, 1 ) );
        if ( iFillColor != NULL )
            {
            TGfxColor lColor(iFillColor->GetColor());
            bitmapContext->SetBrushColor( lColor.GetABGR() );
            bitmapContext->SetPenColor( lColor.GetABGR() );
            }
        else
            {
            bitmapContext->SetBrushColor( iStrokeColor.GetABGR() );
            bitmapContext->SetPenColor( iStrokeColor.GetABGR() );
            }

        bitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
        bitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );


        if ( ( aArrayX != NULL ) && ( count < aArrayX->Count() ) )
            aX = aArrayX->At( count );

        if ( ( aArrayY != NULL ) && ( count < aArrayY->Count() ) )
            aY = aArrayY->At( count );

        TFloatFixPt aY1( aY * iScale ), aX1( aX * iScale );

        TFloatFixPt ScaleAA;
        TGfxAffineTransform tempMatrix;
        tempMatrix = iTransform;

        ScaleAA = 1;

        #ifdef SVG_FLOAT_BUILD
	tempMatrix.Scale( ( TFloatFixPt( 1 ) / ( iScale ) ),
                          ( TFloatFixPt( 1 ) / ( iScale ) ));
	#else
	tempMatrix.Scale( ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ),
                          ( TFloatFixPt( 0x10000,ETrue ) / ( iScale ) ));
	#endif



        if ( aMultipleXY == 1 )
            {
            aX1 = aX1 + ( TFloatFixPt ) advanceX;
            }
        else
            aX1 = aX1 + ( TFloatFixPt ) advanceWidth;

        aY1 -= ( iFont->AscentInPixels() );

        if ( aMultipleXY == 0 )
            {
            if ( iTextAnchor == EGfxTextAnchorMiddle )
                {
                aX1 -= textAdvance / 2;
                }
            else if ( iTextAnchor == EGfxTextAnchorEnd )
                {
                aX1 -= textAdvance;
                }
            }

        TGfxPoint2D p( aX1, aY1 );

        TFloatFixPt trnsx, trnsy;

        trnsx = aX1 ;
        trnsy = aY1 + ( TFloatFixPt ) ( (fontMaxHeight / 2) + fontDescent);

        //"rotate" Attribute Implementation
        // If only one value is present, it is applied to all the glyphs
        //otherwise, one-to-one mapping is taken. Any extra values are ignored!
        if ( aArrayRotate != NULL )
            {
                if(count < aArrayRotate->Count())
                {
                    TFloatFixPt KZero;
                    tempMatrix.Translate( trnsx, trnsy );
                    TReal32 aAngle( ( TReal32 ) aArrayRotate->At( count ) );
                    tempMatrix.Rotate( aAngle * 3.1415926f / 180.0f );
                    TFloatFixPt negX( KZero - trnsx );
                    TFloatFixPt negY( KZero - trnsy );
                    tempMatrix.Translate( negX, negY );
	            }
            }

        TGfxImageTransformer imgtrns( &tempMatrix, iVgRenderer);


        TRect clip(iClip.iX, iClip.iY,
            iClip.iWidth + iClip.iX, iClip.iHeight + iClip.iY);

		p.iY -= fontTweakFactor;
        imgtrns.ImageBlend( offScreenBitmap,
                      p,
                      iRenderInfo.iWidth,
                      iRenderInfo.iHeight,
                      clip,
                      ETrue);

        if (rtl) // IsRightToLeft(outputText)
            {
            CGraphicsContext::TDrawTextExtendedParam param;
            param.iParRightToLeft = ETrue;

            bitmapContext->DrawTextExtended(outputText,
                                        TPoint( 0, fontHeightInPixels - fontDescent + fontTweakFactor ), param);
            }
        else
            {
            bitmapContext->DrawText( outputChar, TPoint( 0, fontMaxAscent ) );
//            bitmapContext->DrawText( outputChar, TPoint( 0, fontHeightInPixels - fontDescent + fontTweakFactor ));
            }
        TGfxRectangle2D textRect(p.iX, p.iY, textAdvance, fontMaxHeight);

        imgtrns.ImageBlend( offScreenBitmap,
                      p,
                      iRenderInfo.iWidth,
                      iRenderInfo.iHeight,
                      clip);

        bitmapContext->DiscardFont();
        CleanupStack::PopAndDestroy( 3 );

        if ( aMultipleXY == 0 )
            {
            TInt aSpacing = 0;
            if ( aLetterSpacing > 0.0 )
                {
                if ( ch != 0x0020 )
                    aSpacing += ( TInt )
                                ( aLetterSpacing * ( TReal ) iScale ) ;
                }
            if ( aWordSpacing > 0.0 )
                {
                if ( ch == 0x0020 )
                    aSpacing += ( TInt ) ( aWordSpacing * ( TReal ) iScale ) ;
                }

            if ( iStrokeColor.GetARGB() == KGfxColorNull )
                {
                if ( iFillColor != NULL )
                    this->iStrokeColor = TGfxColor( iFillColor->GetColor() );
                }

            if ( iTextDecoration == EGfxTextDecorationUnderLine )
                {
                if ( iTextAnchor == EGfxTextAnchorMiddle )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt )
                                      ( advanceWidth - textAdvance / 2 ) /
                                      iScale),
                                      aY,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt )
                                               ( advanceWidth + charWidth + aSpacing - textAdvance / 2 ) /
                                               iScale ) ) ),
                                      aY );
                    DrawL( &aLine );
                    }
                else if ( iTextAnchor == EGfxTextAnchorEnd )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt )
                                      ( advanceWidth - textAdvance ) /
                                      iScale),
                                      aY,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt )
                                               ( advanceWidth + charWidth + aSpacing - textAdvance ) /
                                               iScale ) ) ),
                                      aY );
                    DrawL( &aLine );
                    }
                else                    //Start
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt )
                                      advanceWidth /
                                      iScale),
                                      aY,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt )
                                               ( advanceWidth + charWidth + aSpacing ) /
                                               iScale ) ) ),
                                      aY );
                    DrawL( &aLine );
                    }
                }
            if ( iTextDecoration == EGfxTextDecorationOverLine )
                {
                if ( iTextAnchor == EGfxTextAnchorMiddle )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt )
                                      ( advanceWidth - textAdvance / 2 ) /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt )
                                      fontMaxHeight /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt )
                                               ( advanceWidth + charWidth + aSpacing - textAdvance / 2 ) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      fontMaxHeight /
                                      iScale );
                    DrawL( &aLine );
                    }
                else if ( iTextAnchor == EGfxTextAnchorEnd )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt  )
                                      ( advanceWidth - textAdvance ) /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt  )
                                      fontMaxHeight /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt  )
                                               ( advanceWidth + charWidth + aSpacing - textAdvance ) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      fontMaxHeight /
                                      iScale );
                    DrawL( &aLine );
                    }
                else                    //Start
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt  )
                                      advanceWidth /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt  )
                                      fontMaxHeight /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt  )
                                               ( advanceWidth + charWidth + aSpacing ) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      fontMaxHeight /
                                      iScale );
                    DrawL( &aLine );
                    }
                }
            if ( iTextDecoration == EGfxTextDecorationLineThrough )
                {
                if ( iTextAnchor == EGfxTextAnchorMiddle )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt  )
                                      ( advanceWidth - (textAdvance >> 1)  ) /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt  )
                                               ( advanceWidth + charWidth + aSpacing - ( textAdvance >> 1 )) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale );
                    DrawL( &aLine );
                    }
                else if ( iTextAnchor == EGfxTextAnchorEnd )
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt  )
                                      ( advanceWidth - textAdvance ) /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt  )
                                               ( advanceWidth + charWidth + aSpacing - textAdvance ) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale );
                    DrawL( &aLine );
                    }
                else                    //Start
                    {
                    TGfxLine2D aLine( aX + (ScaleAA *
                                      ( TFloatFixPt  )
                                      advanceWidth /
                                      iScale),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale,
                                      ( aX + (ScaleAA * ( ( TFloatFixPt  )
                                               ( advanceWidth + charWidth + aSpacing ) /
                                               iScale ) ) ),
                                      aY -
                                      ( TFloatFixPt  )
                                      ( fontMaxHeight >> 1 ) /
                                      iScale );
                    DrawL( &aLine );
                    }
                }
            advanceWidth += charWidth + aSpacing;
            }

        if ( aMultipleXY == 1 )
            {
            if ( count >= aArrayX->Count() - 1 )
                advanceX += charWidth;
            }
        }  //end for(Drawing single glyph)
    }

// Raster image drawing
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::DrawImage( CFbsBitmap* aImage,
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::DrawImage( CFbsBitmap* aImage,
                                         const TGfxRectangle2D& aImageRect,
                                         TBool aHasAlpha )
    {
    if ( !aImage )
        return;

    TSize imageSize = aImage->SizeInPixels();

    aImage->LockHeap();
    VGImage vgImage = CreateVGImage( aImage->DataAddress(), imageSize, aImage->DisplayMode(), EFalse );

    aImage->UnlockHeap();

    // Transform to flip drawing to be like screen drawing (from Cartesian)
    TGfxAffineTransform flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );

    // Shift x=0 line to top of screen (buffer)
    flipTransform.Translate( TFloatFixPt ( 0 ), TFloatFixPt ( -iColorBufferSize.iHeight ) );

    // Application transformations from element
    flipTransform.Concatenate( iTransform );

    // Set the topleft corner of image to draw
    flipTransform.Translate( aImageRect.iX, aImageRect.iY );

    // Caller to DrawImage already check for image size zero
    float scaleX = (VGfloat)( TFloatFixPt ( aImageRect.iWidth ) / TFloatFixPt ( imageSize.iWidth ) );
    float scaleY = (VGfloat)( TFloatFixPt ( aImageRect.iHeight ) / TFloatFixPt ( imageSize.iHeight ) );

    // Scale to fit image to drawing area
    flipTransform.Scale( scaleX, scaleY );

    VGfloat matrix[] = { (VGfloat)flipTransform.iM00, (VGfloat)flipTransform.iM10, 0,
                         (VGfloat)flipTransform.iM01, (VGfloat)flipTransform.iM11, 0,
                         (VGfloat)flipTransform.iM02, (VGfloat)flipTransform.iM12, 1 };

    iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE );
    iVgRenderer->vgLoadMatrix( matrix );

    // Get current blend-mode
         const TInt KBlendMode = iVgRenderer->vgGeti( VG_BLEND_MODE );

         iVgRenderer->vgSeti( VG_BLEND_MODE, aHasAlpha ? VG_BLEND_SRC_OVER : VG_BLEND_SRC );

    /* Disable bilinear filtering -> faster, but lower rendering quality */
        iVgRenderer->vgSeti( VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_NONANTIALIASED );

    iVgRenderer->vgDrawImage( vgImage );

    iVgRenderer->vgSeti( VG_BLEND_MODE, KBlendMode );

    iVgRenderer->vgDestroyImage( vgImage );
    }


// ==========================================================================
// Clear the whole graphics context with the background color
// ==========================================================================
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::Clear(TUint32 aRGBA8888Color)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::Clear(TUint32 aARGB )
    {
    TUint32 rgba = ( aARGB << 8 ) | ( aARGB >> 24 );
    TInt32 r, g, b, a;
    r = (TInt)((rgba & 0xFF000000) >> 24);
    g = (TInt)((rgba & 0x00FF0000) >> 16);
    b = (TInt)((rgba & 0x0000FF00) >> 8);
    a = (TInt)( rgba & 0x000000FF);

	r += r >> 7; g += g >> 7; b += b >> 7; a += a >> 7;

    const TFloatFixPt  KInverse255 = TFloatFixPt ( 1.0f/256.0f );
    const VGfloat clearColor[4] = { (VGfloat)(KInverse255 * TFloatFixPt (r)),
                                    (VGfloat)(KInverse255 * TFloatFixPt (g)),
                                    (VGfloat)(KInverse255 * TFloatFixPt (b)),
                                    (VGfloat)(KInverse255 * TFloatFixPt (a)) };

    iVgRenderer->vgSeti( VG_SCISSORING, VG_FALSE );
    iVgRenderer->vgSetfv( VG_CLEAR_COLOR, 4, clearColor );
    iVgRenderer->vgClear( 0, 0, iColorBufferSize.iWidth, iColorBufferSize.iHeight );
    iVgRenderer->vgSeti( VG_SCISSORING, VG_TRUE );
    }

// ==========================================================================
// This method copies the internal RGBA8888 framebuffer to CfbsBitmap that
// is specified in constructor. The CfbsBitmap could be 4k color (RGB0444),
// 64k color (RGB565), or 16M color (RGB888).
// ==========================================================================
// --------------------------------------------------------------------------
// void CGfx2dGcOpenVG::UpdateFramebufferL()
// --------------------------------------------------------------------------
void CGfx2dGcOpenVG::UpdateFramebufferL( CFbsBitmap* aBitmap, CFbsBitmap* aMask)
    {
    if ( !aBitmap || aBitmap->SizeInPixels() != iColorBufferSize )
        {
        return;
        }

    const TDisplayMode KBitmapDisplayMode = aBitmap->DisplayMode();
    // This check just before VGISymbianCopyToBitmap avoid putting lot
    // many similar error checks in the code. If there is any problem 
    // during OpenVG call we simply leave without drawing any thing.  
         VGErrorCode vgret = (VGErrorCode)iVgRenderer->vgGetError();                    
        
    if(vgret != VG_NO_ERROR )
    {
        User::LeaveIfError(OpenVGErrorToSymbianError(vgret));
    }

    // EGray2 is not support in VGISymbianCopyToBitmap API
    if ( KBitmapDisplayMode == EGray2 )
        {
        const TInt KStride = CFbsBitmap::ScanLineLength( iColorBufferSize.iWidth, KBitmapDisplayMode );

        aBitmap->LockHeap();

        // Get data address of last line and move upwards (negative stride)
        // OpenVG uses Cartesian coordinate system and Symbian uses Screen coordinate system.
        TUint* data = (TUint*)((TUint)aBitmap->DataAddress() + ( KStride * ( iColorBufferSize.iHeight - 1  ) ) );

        iVgRenderer->vgReadPixels( data, -KStride, VG_BW_1, 0, 0,
                      iColorBufferSize.iWidth, iColorBufferSize.iHeight );

        aBitmap->UnlockHeap();

        if ( aMask )
            {
            GenerateMask( aMask );
            }
        }
    // All other color modes
    else
        {
        // No Mask -- to be generated in GenerateMask
        TInt error=KErrNone;
 		
 		//In cases, where the mask/aMaskBitmap != NULL, the hint should be defined as VGI_SKIP_TRANSPARENT_PIXELS.
		//In all other cases, i.e., when mask/aMaskBitmap == NULL, the definition should be VGI_COPY_TRANSPARENT_PIXELS
        error = iVgSurface->CopyBitmap(ENone,ENone, aBitmap, aMask, iColorBufferSize);
        
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
                RDebug::Printf("VGISymbianCopyToBitmap failed: error code: %d", error );
                RDebug::Printf(" - Color mode: %d", aBitmap->DisplayMode() );
                RDebug::Printf(" - Bitmap size: %dx%d", iColorBufferSize.iWidth, iColorBufferSize.iHeight );
            #endif
            User::LeaveIfError(error);
            }

        // VGISymbianCopyToBitmap only generates Gray256 mask
        if ( aMask && aMask->DisplayMode() == EGray2 )
            {
            GenerateMask( aMask );
            }
        }
    }
//NGA
// ---------------------------------------------------------------------------------------------------------------------------------------
//void UpdateFramebufferL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,TSize BitmapSize,TDisplayMode aBitmapDspMode,TDisplayMode aMaskDspMode )
// ---------------------------------------------------------------------------------------------------------------------------------------

void CGfx2dGcOpenVG::UpdateFramebufferL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,TSize /*BitmapSize*/,TDisplayMode aBitmapDspMode,TDisplayMode aMaskDspMode )
    {
    if ( !aBitmap)
        {
        return;
        }

    const TDisplayMode KBitmapDisplayMode = aBitmap->DisplayMode();
    // This check just before VGISymbianCopyToBitmap avoid putting lot
    // many similar error checks in the code. If there is any problem 
    // during OpenVG call we simply leave without drawing any thing.  
         VGErrorCode vgret = (VGErrorCode)iVgRenderer->vgGetError();                    
                 
    if(vgret != VG_NO_ERROR )
    {
        User::LeaveIfError(OpenVGErrorToSymbianError(vgret));
    }

    // EGray2 is not support in VGISymbianCopyToBitmap API
    if ( KBitmapDisplayMode == EGray2 )
        {
        const TInt KStride = CFbsBitmap::ScanLineLength( iColorBufferSize.iWidth, KBitmapDisplayMode );

        aBitmap->LockHeap();

        // Get data address of last line and move upwards (negative stride)
        // OpenVG uses Cartesian coordinate system and Symbian uses Screen coordinate system.
        TUint* data = (TUint*)((TUint)aBitmap->DataAddress() + ( KStride * ( iColorBufferSize.iHeight - 1  ) ) );
      
    
        iVgRenderer->vgReadPixels( data, -KStride, VG_BW_1, 0, 0,
                      iColorBufferSize.iWidth, iColorBufferSize.iHeight );
                      
        aBitmap->UnlockHeap();

        if ( aMask )
            {
            GenerateMask( aMask );
            }
        }
    // All other color modes
    else
        {
        // No Mask -- to be generated in GenerateMask
        TInt error=KErrNone;
 		
 		//In cases, where the mask/aMaskBitmap != NULL, the hint should be defined as VGI_SKIP_TRANSPARENT_PIXELS.
		//In all other cases, i.e., when mask/aMaskBitmap == NULL, the definition should be VGI_COPY_TRANSPARENT_PIXELS
        
        error = iVgSurface->CopyBitmap(aBitmapDspMode, aMaskDspMode, aBitmap, aMask, iColorBufferSize);
        
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
                RDebug::Printf("VGISymbianCopyToBitmap failed: error code: %d", error );
                RDebug::Printf(" - Color mode: %d", aBitmap->DisplayMode() );
                RDebug::Printf(" - Bitmap size: %dx%d", iColorBufferSize.iWidth, iColorBufferSize.iHeight );
            #endif
            User::LeaveIfError(error);
            }

        // VGISymbianCopyToBitmap only generates Gray256 mask
        if ( aMask && aMask->DisplayMode() == EGray2 )
            {
            GenerateMask( aMask );
            }
        }
    }

// ====================================================================================
// Blend bitmap with background according to group opacity
// This function gets called with the opacity framebuffer and the group opacity value
// The opacity buffer is initialized with the 0xAA55AA55 value. The pixels not matching
// the initialiazation value are the pixels that have to be blended with the background.
// ====================================================================================

// --------------------------------------------------------------------------
//  void CGfx2dGcVGR::BlendWithBackground(TUint32* aFrameBuffer, TReal32 aOpacity)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::BlendWithBackground(TUint32* /* aColorBuffer */, TReal32 /* aOpacity */)
    {
/*
    if ( !iColorBuffer || !aColorBuffer )
        {
        return;
        }

    TReal32 inverseAlpha = 1.0f - aOpacity;
    const TInt KMax = iColorBufferSize.iWidth * iColorBufferSize.iHeight;
    TUint32* dst = iColorBuffer;
    TUint32* src = aColorBuffer;

    for ( TInt i = KMax - 1; i >= 0; i-- )
        {
        TUint32 srcColor = src[i];
        TUint32 dstColor = dst[i];
        if ( srcColor != dstColor )
            {
            dst[i] = ((((TUint)((srcColor >> 24       ) * aOpacity) +
                        (TUint)((dstColor >> 24       ) * inverseAlpha))) << 24) | // red
                     ((((TUint)((srcColor >> 16 & 0xff) * aOpacity) +
                        (TUint)((dstColor >> 16 & 0xff) * inverseAlpha))) << 16) | // green
                     ((((TUint)((srcColor >> 8  & 0xff) * aOpacity) +
                        (TUint)((dstColor >> 8  & 0xff) * inverseAlpha))) << 8 ) | // blue
                        (srcColor & 0x000000FF);                                   // keep orignal alpha
            }
        }
*/
    }

// ==========================================================================
// Generate 8 bit gray or 1bit B/W mask from internal ARGB8888 frame buffer.
// ==========================================================================
// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::GenerateMask(CFbsBitmap* aMask)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::GenerateMask(CFbsBitmap* aMask)
    {
    if ( !aMask || aMask->SizeInPixels() != iColorBufferSize )
        {
        return;
        }

    const TDisplayMode KMaskDisplayMode = aMask->DisplayMode();

    if ( KMaskDisplayMode != EGray256 && KMaskDisplayMode != EGray2 )
        {
        return;
        }

    const TInt KOriginalFilterMasks = iVgRenderer->vgGeti( VG_FILTER_CHANNEL_MASK );//
    const TInt KStride = CFbsBitmap::ScanLineLength( iColorBufferSize.iWidth, KMaskDisplayMode );

    // Change to get alpha values from OpenVG
    iVgRenderer->vgSeti( VG_FILTER_CHANNEL_MASK, VG_ALPHA );

    VGImageFormat format = ( KMaskDisplayMode == EGray256 ) ? VG_A_8 : VG_BW_1;

    aMask->LockHeap();

    // Get data address of last line and move upwards (negative stride)
    // OpenVG uses Cartesian coordinate system and Symbian uses Screen coordinate system.
    TUint* data = (TUint*)((TUint)aMask->DataAddress() + ( KStride * ( iColorBufferSize.iHeight - 1  ) ) );

    iVgRenderer->vgReadPixels( data, -KStride, format, 0, 0,
                  iColorBufferSize.iWidth, iColorBufferSize.iHeight );
    aMask->UnlockHeap();

    // Set back the original filter-masks
    iVgRenderer->vgSeti( VG_FILTER_CHANNEL_MASK, KOriginalFilterMasks );
    }

 void CGfx2dGcOpenVG::GenerateMask(CSvgtBitmap* aMask)
     {
     if ( !aMask || aMask->SizeInPixels() != iColorBufferSize )
         {
         return;
         }

     const TDisplayMode KMaskDisplayMode = aMask->DisplayMode();

     if ( KMaskDisplayMode != EGray256 && KMaskDisplayMode != EGray2 )
         {
         return;
         }

     const TInt KOriginalFilterMasks = iVgRenderer->vgGeti( VG_FILTER_CHANNEL_MASK );//
     const TInt KStride = CFbsBitmap::ScanLineLength( iColorBufferSize.iWidth, KMaskDisplayMode );

     // Change to get alpha values from OpenVG
     iVgRenderer->vgSeti( VG_FILTER_CHANNEL_MASK, VG_ALPHA );

     VGImageFormat format = ( KMaskDisplayMode == EGray256 ) ? VG_A_8 : VG_BW_1;

     // Get data address of last line and move upwards (negative stride)
     // OpenVG uses Cartesian coordinate system and Symbian uses Screen coordinate system.

     iVgRenderer->vgReadPixels( aMask->BitmapBuffer(), -KStride, format, 0, 0,
                   iColorBufferSize.iWidth, iColorBufferSize.iHeight );
     // Set back the original filter-masks
     iVgRenderer->vgSeti( VG_FILTER_CHANNEL_MASK, KOriginalFilterMasks );
     }

// ==========================================================================
//  This function calls the low level function to set fill opacity value
// ==========================================================================

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetFillOpacity(TFloatFixPt  aFillOpacity)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetFillOpacity(TFloatFixPt  aFillOpacity)
    {
    iFillOpacity = aFillOpacity;
    }
// ==========================================================================
//  This function calls the low level function to set stroke opacity value
// ==========================================================================

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetStrokeOpacity(TFloatFixPt  aStrokeOpacity)
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetStrokeOpacity(TFloatFixPt  aStrokeOpacity)
    {
    iStrokeOpacity = aStrokeOpacity;
    }
// ==========================================================================
//  This function is called to set the dithering flag
// ==========================================================================

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::SetDoDithering( TBool aDoDithering )
// ---------------------------------------------------------------------------
 void CGfx2dGcOpenVG::SetDoDithering( TBool /*aDoDithering*/ )
    {
    }

// --------------------------------------------------------------------------
//  void CGfx2dGcOpenVG::ApplyTransform
// ---------------------------------------------------------------------------
void CGfx2dGcOpenVG::ApplyTransform( TGfxAffineTransform& aAffineTransform )
    {
    iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE );

    TUint transformType = aAffineTransform.iTransType;

    // Only translation is applied
    if ( transformType == KTransformTranslate )
        {
        iVgRenderer->vgLoadIdentity();          
        iVgRenderer->vgScale( 1.0f, -1.0f );
        iVgRenderer->vgTranslate( (VGfloat)aAffineTransform.iM02,
                     (VGfloat)(TFloatFixPt (-iColorBufferSize.iHeight) + aAffineTransform.iM12 ) );
        }
    // General case: Rotate and/or scaling, translation
    else
        {
        TGfxAffineTransform flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );
        flipTransform.Translate( TFloatFixPt ( 0 ), TFloatFixPt ( -iColorBufferSize.iHeight ) );
        flipTransform.Concatenate( aAffineTransform );

        VGfloat matrix[] = { (VGfloat)flipTransform.iM00, (VGfloat)flipTransform.iM10, 0,
                             (VGfloat)flipTransform.iM01, (VGfloat)flipTransform.iM11, 0,
                             (VGfloat)flipTransform.iM02, (VGfloat)flipTransform.iM12, 1 };
        iVgRenderer->vgLoadMatrix( matrix );
        }
    }
 void CGfx2dGcOpenVG::ApplyTransformMediaElement( const TGfxAffineTransform& aAffineTransform,TSize& /*aSize*/ )
    {
        iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE );

    // TUint transformType = aAffineTransform.iTransType;

    // Only translation is applied
  /*  if ( transformType == KTransformTranslate )
        {
        vgLoadIdentity();
        vgScale( 1.0f, -1.0f );
        vgTranslate( (VGfloat)aAffineTransform.iM02,
                     (VGfloat)(TFloatFixPt (-iColorBufferSize.iHeight) + aAffineTransform.iM12 ) );
        }
    // General case: Rotate and/or scaling, translation
    else
        {
        TGfxAffineTransform flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );
        flipTransform.Translate( TFloatFixPt ( 0 ), TFloatFixPt ( -iColorBufferSize.iHeight ) );
        flipTransform.Concatenate( aAffineTransform );

        VGfloat matrix[] = { (VGfloat)flipTransform.iM00, (VGfloat)flipTransform.iM10, 0,
                             (VGfloat)flipTransform.iM01, (VGfloat)flipTransform.iM11, 0,
                             (VGfloat)flipTransform.iM02, (VGfloat)flipTransform.iM12, 1 };
        vgLoadMatrix( matrix );
        }*/
        TGfxAffineTransform flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );
        flipTransform.Translate( TFloatFixPt ( 0 ), TFloatFixPt ( -iColorBufferSize.iHeight ) );
        //flipTransform.Concatenate( aAffineTransform );
        TGfxAffineTransform flipTransformTemp = aAffineTransform;
        flipTransformTemp.Concatenate( flipTransform );
        flipTransform.Concatenate( flipTransformTemp );
       
        
        VGfloat matrix[] = { (VGfloat)flipTransform.iM00, (VGfloat)flipTransform.iM10, 0,
                             (VGfloat)flipTransform.iM01, (VGfloat)flipTransform.iM11, 0,
                             (VGfloat)flipTransform.iM02, (VGfloat)flipTransform.iM12, 1 };
            iVgRenderer->vgLoadMatrix( matrix );
    }   

VGImage CGfx2dGcOpenVG::CreateVGImage( void* aBuffer, TSize aSize, TDisplayMode aDisplayMode, TBool aPreMultiplied )
    {
    VGImage vgImage = VGImage( -1 );

    if ( aBuffer )
        {
        VGint stride = CFbsBitmap::ScanLineLength( aSize.iWidth, aDisplayMode );

        if ( aDisplayMode == EColor64K )
            {
                 vgImage = iVgRenderer->vgCreateImage( VG_sRGB_565, aSize.iWidth, aSize.iHeight, VG_IMAGE_QUALITY_FASTER );
                 iVgRenderer->vgImageSubData( vgImage, aBuffer, stride, VG_sRGB_565,
                            0, 0, aSize.iWidth, aSize.iHeight );                            
            }
        else if ( aDisplayMode == EColor16MU )
            {
            VGImageFormat format = aPreMultiplied ? VG_sRGBA_8888_PRE :VG_sRGBA_8888;
            vgImage = iVgRenderer->vgCreateImage( format, aSize.iWidth, aSize.iHeight, VG_IMAGE_QUALITY_FASTER );
                        iVgRenderer->vgImageSubData( vgImage, aBuffer, stride, format,
                            0, 0, aSize.iWidth, aSize.iHeight );
            }
        else
            {
            #ifdef _DEBUG
            RDebug::Printf("SVGT CreateVGImage:: Color Mode not supported: %d ", aDisplayMode );
            #endif
            }
        }

    return vgImage;
    }

 void CGfx2dGcOpenVG::BindToImageL()
    {
    TUint32* buffer = NULL;
    buffer = new (ELeave) TUint32[ iColorBufferSize.iWidth *   iColorBufferSize.iHeight ] ;
    
    iVgSurface->PrepareToBindClientBuffer();
    //VGImage vgImage = CreateVGImage( buffer, iColorBufferSize, EColor16MU, ETrue );
    VGImage vgImage  = iVgRenderer->vgCreateImage( VG_sRGBA_8888_PRE,iColorBufferSize.iWidth, iColorBufferSize.iHeight, VG_IMAGE_QUALITY_FASTER );
    //Clear VGImage
    VGfloat color[4] = { 1.0f, 1.0f, 1.0f, 0.0f }; 
        iVgRenderer->vgSetfv(VG_CLEAR_COLOR, 4, color);
iVgRenderer->vgClearImage(vgImage,0,0,iColorBufferSize.iWidth,iColorBufferSize.iHeight);
    //Clear VGImage
    TInt error = iVgSurface->BindClientBuffer( vgImage );
    if ( error != KErrNone )
    {
        #ifdef _DEBUG
        RDebug::Printf("SVGT CGfx2dGcOpenVG::BindToImageL--VGISymbianBindToImageL failed: %d", error);
        #endif
            
        iVgRenderer->vgDestroyImage(vgImage) ;
        // Delete color-buffer to match count with vgImage
        delete[] buffer;
        
        if( error == KErrNoMemory )
        {
            // Need to reset values at our side as OpenVG calls VGISymbianTerminate.
            ResetContextHandle();
        }
        
        User::Leave(error);
     }

     iGroupOpacityBuffers.Append( buffer );
     iGroupOpacityImages.Append( vgImage );

        iVgRenderer->vgLoadIdentity();
    }
    
    
 void CGfx2dGcOpenVG::BindToMediaImageL(TInt aWidth, TInt aHeight)
    {
        TUint32* buffer = NULL;
        buffer = new (ELeave) TUint32[ iColorBufferSize.iWidth *   iColorBufferSize.iHeight ];
        
        iVgSurface->PrepareToBindClientBuffer();
        VGImage vgImage = CreateVGImage( buffer, iColorBufferSize, EColor16MU, ETrue );
        VGfloat color[4] = { 1.0f, 1.0f, 1.0f, 0.0f }; 
         iVgRenderer->vgSetfv(VG_CLEAR_COLOR, 4, color);
         iVgRenderer->vgClearImage(vgImage,0,0,iColorBufferSize.iWidth,iColorBufferSize.iHeight);                
        //vgClearImage(vgImage,0,0,aWidth,aHeight);
        TInt error = iVgSurface->BindClientBuffer( vgImage );
        if ( error != KErrNone )
        {
           #ifdef _DEBUG
           RDebug::Printf("SVGT CGfx2dGcOpenVG::BindToImage--VGISymbianBindToImage failed: %d", error);
           #endif
           
            iVgRenderer->vgDestroyImage(vgImage) ;
           // Delete color-buffer to match count with vgImage
           delete[] buffer;
           
           if( error == KErrNoMemory )
           {
                // Need to reset values at our side as OpenVG calls VGISymbianTerminate.
                ResetContextHandle();
           }
            
           User::Leave(error);
        }
        
        iGroupOpacityBuffers.Append( buffer );
        iGroupOpacityImages.Append( vgImage );
        iVgRenderer->vgLoadIdentity();
        
        TGfxRectangle2D aClip(0,0,aWidth,aHeight); 
        iClipMain = iClip;
        SetClip(aClip);
      }

 void CGfx2dGcOpenVG::UnbindFromImageL( TReal32 aOpacity )
    {
    if ( iGroupOpacityImages.Count() == 0 )
        {
        return;
        }

    TInt error = iVgSurface->UnBindClientBuffer();
    if ( error != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromImage--VGISymbianUnBindImage failed: %d", error);
        #endif
        
        if( error == KErrNoMemory )
            {
            // Need to reset values at our side as OpenVG calls VGITerminate.
            ResetContextHandle();
            }
        User::LeaveIfError(error);
        }

    // Need to bind to previous level image
    // Else VGISymbianUnBindImage binds to non-image
    if ( iGroupOpacityImages.Count() > 1 )
        {
        iVgSurface->PrepareToBindClientBuffer();
        TInt error = iVgSurface->BindClientBuffer( iGroupOpacityImages[iGroupOpacityImages.Count()-2] );
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromImage--VGISymbianBindToImage failed: %d", error);
            #endif
            
            iVgSurface->UnBindClientBuffer();	
            
            // Remove and Destroy all the previous images and buffer
            TInt iIndex = iGroupOpacityImages.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
                iVgRenderer->vgDestroyImage( iGroupOpacityImages[ iIndex ] );
                iGroupOpacityImages.Remove( iIndex );
                }
            
            iIndex = iGroupOpacityBuffers.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
            	delete [] iGroupOpacityBuffers[ iIndex ];
                iGroupOpacityBuffers.Remove( iIndex );
                }
            
            if( error == KErrNoMemory )
                {
                // Need to reset values at our side as OpenVG calls VGISymbianTerminate.
                ResetContextHandle();
                }
            
            User::LeaveIfError(error);
            }
        }

    // Check if default fill paint needs to be created
    if ( iFillPaint == VG_INVALID_HANDLE )
        {
             iFillPaint = iVgRenderer->vgCreatePaint();
        }
    
    // Check if default stroke paint needs to be created
    if ( iStrokePaint == VG_INVALID_HANDLE )        
        {
             iStrokePaint = iVgRenderer->vgCreatePaint();            
        }

    VGfloat opaquePaint[] = { 1.0f, 1.0f, 1.0f, aOpacity };

    // Create paint object
         VGPaint paint = iVgRenderer->vgCreatePaint();

    if ( paint != VG_INVALID_HANDLE )
        {
        
          // Set solid paint of opacity
        iVgRenderer->vgSetParameteri( paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
        iVgRenderer->vgSetParameterfv( paint, VG_PAINT_COLOR, 4, opaquePaint);
        iVgRenderer->vgSetPaint( paint, VG_FILL_PATH | VG_STROKE_PATH );

        // Set image mode to multiply
        iVgRenderer->vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY );

        // Draw opacity image
        iVgRenderer->vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE );
        iVgRenderer->vgLoadIdentity();
        iVgRenderer->vgDrawImage( iGroupOpacityImages[iGroupOpacityImages.Count()-1] );

        // Reset the image draw mode
        iVgRenderer->vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL );

        // Restore the fill and paint
        if ( iFillPaint != VG_INVALID_HANDLE )
            {
            iVgRenderer->vgSetPaint( iFillPaint, VG_FILL_PATH );            
            }

        if ( iStrokePaint != VG_INVALID_HANDLE )
            {
            iVgRenderer->vgSetPaint( iStrokePaint, VG_STROKE_PATH );            
            }

        iVgRenderer->vgDestroyPaint( paint );
        }
    else
        {
             VGErrorCode error = (VGErrorCode)iVgRenderer->vgGetError();                    
        if (error == VG_OUT_OF_MEMORY_ERROR)
            {
            RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromImage vgCreatePaint OOM");
            // Remove and Destroy all the previous images and buffer
            TInt iIndex = iGroupOpacityImages.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
                    iVgRenderer->vgDestroyImage( iGroupOpacityImages[ iIndex ] );
                iGroupOpacityImages.Remove( iIndex );
                }
            
            iIndex = iGroupOpacityBuffers.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
            	delete [] iGroupOpacityBuffers[ iIndex ];
                iGroupOpacityBuffers.Remove( iIndex );
                }
            ResetContextHandle();
            User::Leave(KErrNoMemory);
            }
        #ifdef _DEBUG
        RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromImage vgCreatePaint failed");
        #endif
        }

    // Remove/Delete OpenVG Image object
    TInt imageIndex = iGroupOpacityImages.Count() - 1;
    if ( imageIndex >= 0 )
        {
             iVgRenderer->vgDestroyImage( iGroupOpacityImages[ imageIndex ] );
        iGroupOpacityImages.Remove( imageIndex );
        }

    // Remove/Delete color-buffer
    TInt bufferIndex = iGroupOpacityBuffers.Count() - 1;
    if ( bufferIndex >= 0 )
        {
        delete [] iGroupOpacityBuffers[ bufferIndex ];
        iGroupOpacityBuffers.Remove( bufferIndex );
        }

    }

// --------------------------------------------------------------------------
// void CGfx2dGcOpenVG::UnbindFromMediaImageL( 
//    const TGfxAffineTransform& aAffineTransform,
//    TReal32 aOpacity,
//    TInt aWidth, 
//    TInt aHeight )
// ---------------------------------------------------------------------------
void CGfx2dGcOpenVG::UnbindFromMediaImageL(
    const TGfxAffineTransform& aAffineTransform,
    TReal32 aOpacity,
    TInt aWidth, 
    TInt aHeight, TInt aDraw  )
    {
    if ( iGroupOpacityImages.Count() == 0 )
        {
        return;
        }

    TInt error = iVgSurface->UnBindClientBuffer();
    if ( error != KErrNone )
        {
        #ifdef _DEBUG
        RDebug::Printf( "SVGT CGfx2dGcOpenVG::UnbindFromMediaImage--VGISymbianUnBindImage failed: %d", error);
        #endif
        if( error == KErrNoMemory )
            {
            // Need to reset values at our side as OpenVG calls 
            // VGITerminate.
            ResetContextHandle();
            }
        User::LeaveIfError(error);
        }
    
    // Restore the clipping window back to main window
    SetClip(iClipMain); 
    
    // Need to bind to previous level image
    // Else VGISymbianUnBindImage binds to non-image
    if ( iGroupOpacityImages.Count() > 1 )
        {
        iVgSurface->PrepareToBindClientBuffer();
        TInt error = iVgSurface->BindClientBuffer( iGroupOpacityImages[iGroupOpacityImages.Count()-2] );
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
            RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromMediaImage--VGISymbianBindToImage failed: %d", error);
            #endif
            
            iVgSurface->UnBindClientBuffer();	
            
            // Remove and Destroy all the previous images and buffer
            TInt iIndex = iGroupOpacityImages.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
                iVgRenderer->vgDestroyImage( iGroupOpacityImages[ iIndex ] );
                iGroupOpacityImages.Remove( iIndex );
                }
            
            iIndex = iGroupOpacityBuffers.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
                {
            	delete [] iGroupOpacityBuffers[ iIndex ];
                iGroupOpacityBuffers.Remove( iIndex );
                }
            
            // Destroy the Previous two Opacity buffer
            if( error == KErrNoMemory )
                {
                // Need to reset values at our side as OpenVG calls VGISymbianTerminate.
                ResetContextHandle();
                }
            
            User::LeaveIfError(error);
            }
        }
    
    // Create opaque paint
    VGfloat opaquePaint[] = { 1.0f, 1.0f, 1.0f, aOpacity };
    
    // Check if default fill paint needs to be created
    if ( iFillPaint == VG_INVALID_HANDLE )
        {
             iFillPaint = iVgRenderer->vgCreatePaint();   
        }
    
    // Check if default stroke paint needs to be created
    if ( iStrokePaint == VG_INVALID_HANDLE )        
        {
             iStrokePaint = iVgRenderer->vgCreatePaint();   
        }
    
    // Create paint object
         VGPaint paint = iVgRenderer->vgCreatePaint();           
    
    if ( paint != VG_INVALID_HANDLE )
        {
                // Set solid paint of opacity
        iVgRenderer->vgSetParameteri( paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
        iVgRenderer->vgSetParameterfv( paint, VG_PAINT_COLOR, 4, opaquePaint);
        iVgRenderer->vgSetPaint( paint, VG_FILL_PATH | VG_STROKE_PATH );

        // Set image mode to normal
        iVgRenderer->vgSeti( VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL );
        
        // Draw the animation element with transforms applied on the
        // main colour buffer
        TSize size(aWidth,aHeight);
        ApplyTransformMediaElement(aAffineTransform,size);
        
        // Check so that animation element is drawn only when animation has started
        if ( aDraw == KMediaAnimationVisible  )
            {
                    iVgRenderer->vgDrawImage( iGroupOpacityImages[iGroupOpacityImages.Count()-1] );    
            }
        

        // Restore the fill and paint
        if ( iFillPaint != VG_INVALID_HANDLE )
            {
                iVgRenderer->vgSetPaint( iFillPaint, VG_FILL_PATH );            
            }

        if ( iStrokePaint != VG_INVALID_HANDLE )
            {
                iVgRenderer->vgSetPaint( iStrokePaint, VG_STROKE_PATH );            
            }

             iVgRenderer->vgDestroyPaint( paint );   
        }
    else
        {
            VGErrorCode error = (VGErrorCode)iVgRenderer->vgGetError();
        if (error == VG_OUT_OF_MEMORY_ERROR)
            {
            RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromMediaImage vgCreatePaint OOM");
            // Remove and Destroy all the previous images and buffer
            TInt iIndex = iGroupOpacityImages.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
            {
            	iVgRenderer->vgDestroyImage( iGroupOpacityImages[ iIndex ] );            	
                iGroupOpacityImages.Remove( iIndex );
            }
            
            iIndex = iGroupOpacityBuffers.Count()-1;
            for( ;iIndex >= 0 ; iIndex-- )
            {
            	delete [] iGroupOpacityBuffers[ iIndex ];
                iGroupOpacityBuffers.Remove( iIndex );
            }
            ResetContextHandle();
            User::Leave(KErrNoMemory);
            }
        #ifdef _DEBUG
        RDebug::Printf("SVGT CGfx2dGcOpenVG::UnbindFromMediaImage vgCreatePaint failed");
        #endif
        }
    // Remove/Delete OpenVG Image object
    TInt imageIndex = iGroupOpacityImages.Count() - 1;
    if ( imageIndex >= 0 )
        {
        iVgRenderer->vgDestroyImage( iGroupOpacityImages[ imageIndex ] );
        iGroupOpacityImages.Remove( imageIndex );
        }

    // Remove/Delete color-buffer
    TInt bufferIndex = iGroupOpacityBuffers.Count() - 1;
    if ( bufferIndex >= 0 )
        {
        delete [] iGroupOpacityBuffers[ bufferIndex ];
        iGroupOpacityBuffers.Remove( bufferIndex );
        }

    }
// These are for work-around for OpenVG to simulate multiple contexts
void CGfx2dGcOpenVG::SetupContextL()
{
    // Ensure OpenVG is initialized: could have been terminated by another Svg engine
    // Returns immediately if already initialized
         TInt err = iVgSurface->InitializeSurface( iColorBufferSize, VGI_COLORSPACE_SRGB );
        if (!((err == KErrNone) || (err == KErrAlreadyExists)))
            {
            #ifdef _DEBUG
            RDebug::Printf("SVG can't initialize OpenVG Context %d", err);
            #endif
        iGraphicsContextCreated = EFalse;
            User::Leave(err);
            }

    iGraphicsContextCreated = ETrue;
    // Ensure context size is correct: could have been change by another Svg engine
    // Returns immediately if size is the same
    
         err = iVgSurface->ResizeSurface( iColorBufferSize );
    
    if( err != KErrNone )
    {
        if(err == KErrNoMemory )
        {
            #ifdef _DEBUG
            RDebug::Printf("OpenVG Context destroyed %d", err);
            #endif //_DEBUG
            // This indicates that OpenVG has destroyed the current context handle
            // and also the Path, StrokePaint and FillPaint handles are no more valid.
            // Thus reinitialize them to 0, so that they can be recreated.
            ResetContextHandle();
        }
            
        #ifdef _DEBUG
        RDebug::Printf("SVG can't RESIZE OpenVG Context %d", err);
        #endif //_DEBUG
        User::Leave(err);
    }
}

void CGfx2dGcOpenVG::Flush()
    {
    if (iCurrentRendererType == ESVGRendererTLV)
        {
        iPath = VG_INVALID_HANDLE;
        iFillPaint = VG_INVALID_HANDLE;
        iStrokePaint = VG_INVALID_HANDLE;
        }

    iVgRenderer->vgFlush();    
    }
/**
* Sets the Paint,Stroke and Path handle back to VG_INVALID_HANDLE
*
* @since 
* @return void
*/    
void CGfx2dGcOpenVG::ResetContextHandle()
    {
        iPath = VG_INVALID_HANDLE;
        iFillPaint = VG_INVALID_HANDLE;
        iStrokePaint = VG_INVALID_HANDLE;
        iGraphicsContextCreated = EFalse;
    }

void CGfx2dGcOpenVG::GetMatrix(TReal32 * m)
    {
        //Below is the replacement for vgGetMatrix
            TGfxAffineTransform flipTransform;
            if ( iTransform.iTransType == KTransformTranslate )
                {

                    flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );//get and identity matrix and scale the x,y axis to (1,-1)
                    flipTransform.Translate((VGfloat)iTransform.iM02, (VGfloat)(TFloatFixPt (-iColorBufferSize.iHeight) + iTransform.iM12 ) );
            
                    m[0] = (VGfloat)flipTransform.iM00;                                     
                    m[1] = (VGfloat)flipTransform.iM10;
                    m[2] =  0;
                    m[3] = (VGfloat)flipTransform.iM01;
                    m[4] = (VGfloat)flipTransform.iM11;
                    m[5] =  0;                                    
                    m[6] = (VGfloat)flipTransform.iM02;
                    m[7] = (VGfloat)flipTransform.iM12;
                    m[8] =  1;               
                                                             
                }
            // General case: Rotate and/or scaling, translation
            else
                {
                    flipTransform = TGfxAffineTransform::GetScaleInstance( 1.0f, -1.0f );
                    flipTransform.Translate( TFloatFixPt ( 0 ), TFloatFixPt ( -iColorBufferSize.iHeight ) );
                    flipTransform.Concatenate( iTransform );

                    m[0] = (VGfloat)flipTransform.iM00;                                     
                    m[1] = (VGfloat)flipTransform.iM10;
                    m[2] =  0;
                    m[3] = (VGfloat)flipTransform.iM01;
                    m[4] = (VGfloat)flipTransform.iM11;
                    m[5] =  0;                                    
                    m[6] = (VGfloat)flipTransform.iM02;
                    m[7] = (VGfloat)flipTransform.iM12;
                    m[8] =  1;             
                
                }
   
    }

void CGfx2dGcOpenVG::SetBitmapHeader(const TDesC* aHeaderData)
    {
    iVgSurface->SetConfiguration(CVGSurface::BTIMAP_HEADER, aHeaderData);
    }

const TPtrC8 CGfx2dGcOpenVG::TLVEncodedData() const
    {
    return(iVgRenderer->TLVEncodedData());
    }

//--------------------------------------------------------------------------------
// M2G: UpdateFramebufferL() is overloaded to enable rendering on target buffer.
//--------------------------------------------------------------------------------
void CGfx2dGcOpenVG::UpdateFramebufferL( CSvgtBitmap* aBitmap, CSvgtBitmap* aMask )
    {
    if ( !aBitmap || aBitmap->SizeInPixels() != iColorBufferSize )
        {
        return;
        }

    const TDisplayMode KBitmapDisplayMode = aBitmap->DisplayMode();
    // This check just before VGISymbianCopyToBitmap avoid putting lot
    // many similar error checks in the code. If there is any problem 
    // during OpenVG call we simply leave without drawing any thing.  
         VGErrorCode vgret = (VGErrorCode)iVgRenderer->vgGetError();                    
        
    if(vgret != VG_NO_ERROR )
    {
        User::LeaveIfError(OpenVGErrorToSymbianError(vgret));
    }

    // EGray2 is not support in VGISymbianCopyToBitmap API
    if ( KBitmapDisplayMode == EGray2 )
        {
        const TInt KStride = aBitmap->Stride();

        iVgRenderer->vgReadPixels( aBitmap->BitmapBuffer(), -KStride, VG_BW_1, 0, 0,
                      iColorBufferSize.iWidth, iColorBufferSize.iHeight );
        }
    // All other color modes
    else
        {
        // No Mask -- to be generated in GenerateMask
        TInt error=KErrNone;
        
        //In cases, where the mask/aMaskBitmap != NULL, the hint should be defined as VGI_SKIP_TRANSPARENT_PIXELS.
        //In all other cases, i.e., when mask/aMaskBitmap == NULL, the definition should be VGI_COPY_TRANSPARENT_PIXELS
        error = iVgSurface->CopyBitmap(ENone,ENone, aBitmap, aMask, iColorBufferSize);
        
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
                RDebug::Printf("VGICopyToTarget failed: error code: %d", error );
                RDebug::Printf(" - Color mode: %d", aBitmap->DisplayMode() );
                RDebug::Printf(" - Bitmap size: %dx%d", iColorBufferSize.iWidth, iColorBufferSize.iHeight );
            #endif
            User::LeaveIfError(error);
            }
        }
    }

//--------------------------------------------------------------------------------
// M2G: UpdateFramebufferL() is overloaded to enable rendering on target buffer.
//--------------------------------------------------------------------------------
void CGfx2dGcOpenVG::UpdateFramebufferL( CSvgtBitmap* aBitmap, CSvgtBitmap* aMask,TSize /*BitmapSize*/,TDisplayMode aBitmapDspMode,TDisplayMode aMaskDspMode )
    {
    if ( !aBitmap)
        {
        return;
        }

    const TDisplayMode KBitmapDisplayMode = aBitmap->DisplayMode();
    // This check just before VGISymbianCopyToBitmap avoid putting lot
    // many similar error checks in the code. If there is any problem 
    // during OpenVG call we simply leave without drawing any thing.  
         VGErrorCode vgret = (VGErrorCode)iVgRenderer->vgGetError();                    
                 
    if(vgret != VG_NO_ERROR )
    {
        User::LeaveIfError(OpenVGErrorToSymbianError(vgret));
    }

    // EGray2 is not support in VGISymbianCopyToBitmap API
    if ( KBitmapDisplayMode == EGray2 )
        {
        const TInt KStride = aBitmap->Stride();

        iVgRenderer->vgReadPixels( aBitmap->BitmapBuffer(), -KStride, VG_BW_1, 0, 0,
                      iColorBufferSize.iWidth, iColorBufferSize.iHeight );

        }
    // All other color modes
    else
        {
        // No Mask -- to be generated in GenerateMask
        TInt error=KErrNone;
        
        //In cases, where the mask/aMaskBitmap != NULL, the hint should be defined as VGI_SKIP_TRANSPARENT_PIXELS.
        //In all other cases, i.e., when mask/aMaskBitmap == NULL, the definition should be VGI_COPY_TRANSPARENT_PIXELS
        
        error = iVgSurface->CopyBitmap(aBitmapDspMode, aMaskDspMode, aBitmap, aMask, iColorBufferSize);
        
        if ( error != KErrNone )
            {
            #ifdef _DEBUG
                RDebug::Printf("VGICopyToTarget failed: error code: %d", error );
                RDebug::Printf(" - Color mode: %d", aBitmap->DisplayMode() );
                RDebug::Printf(" - Bitmap size: %dx%d", iColorBufferSize.iWidth, iColorBufferSize.iHeight );
            #endif
            User::LeaveIfError(error);
            }
        }
    
    }
