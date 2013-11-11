/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implements CHuiVg10Gc, an OpenVG 1.0 version of the HUITK
*                graphics context.
*
*/



#include "HuiVg10Gc.h"  // Class definition
#include "HuiVg10RenderPlugin.h"
#include "HuiVg10Texture.h"
#include "HuiVg10RenderSurface.h"
#include "uiacceltk/HuiSegmentedTexture.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiImage.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCurvePath.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/huifixmath.h"
#include "HuiRenderSurface.h"

#include "huiextension.h"

#include <e32math.h>
#include <VG/vgu.h>
#ifdef __NVG
#pragma message( "NVG Defined!" )
    #include <nvg.h>
    #include <AknIconHeader.h>
    #include <AknIconUtils.h>
    #include "NVGIcon.h"
#endif

const TInt KHuiMaxVgScissorRects = 32;

/** Only render colorful rectangles instead of real graphics 
 *  content. Useful for debugging broken OpenVG drivers.
 */
//#define RENDER_DEBUG_RECTANGLES

CHuiVg10Gc* CHuiVg10Gc::NewL()
    {
    CHuiVg10Gc* self = CHuiVg10Gc::NewLC();
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10Gc* CHuiVg10Gc::NewLC()
    {
    CHuiVg10Gc* self = new (ELeave) CHuiVg10Gc();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10Gc::CHuiVg10Gc()
        : CHuiGc()
    {
    }


void CHuiVg10Gc::ConstructL()
    {
    // iMatrix stack must be allocated here because InitState() is not allowed to leave
    
    iMatrixStack = CHuiMatrixStack::NewL();
#ifdef __NVG
    CHuiVg10RenderPlugin& renderer = (CHuiVg10RenderPlugin&) CHuiStatic::Renderer();
    iNvgEngine = &renderer.NvgEngine();
#endif    
    }


CHuiVg10Gc::~CHuiVg10Gc()
    {
#ifdef __NVG
    // Don't delete the iNvgEngine, it's owned by the plugin!
    iNvgEngine = NULL;
#endif    
    delete iMatrixStack;
    
    vgDestroyPath(iPath);
    vgDestroyPath(iImagePath);
    vgDestroyPath(iRectPath);
    vgDestroyPath(iRoundRectPath);
    vgDestroyPath(iLinePath);
    vgDestroyPath(iBorderPath);
    vgDestroyPath(iArcPath);
    vgDestroyPaint(iPaint);
    vgDestroyPaint(iGradientPaint);
    vgDestroyPath(iEllipsePath);
    
    iTempRegion.Close();
    }
    

void CHuiVg10Gc::Push(THuiGcMatrix aMatrix)
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        // Replicate the current OpenVG matrix to the matrix stack
        UpdateClientMatrix();
        iMatrixStack->Push();
        
        // Save the matrix flags
        iMatrixFlagsStack.Append(iMatrixFlags);
        }
    }


void CHuiVg10Gc::Pop(THuiGcMatrix aMatrix)
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        // Replicate the current matrix to OpenVG
        iMatrixStack->Pop();
        CHuiMatrixStack::TMatrix& matrix = iMatrixStack->Current();
        vgLoadMatrix((VGfloat*)matrix.iMatrix);
        
        // Restore the matrix flags
        iMatrixFlags = iMatrixFlagsStack[iMatrixFlagsStack.Count() - 1];
        iMatrixFlagsStack.Remove(iMatrixFlagsStack.Count() - 1);
        HUI_VG_INVARIANT();
        }
    }


void CHuiVg10Gc::LoadIdentity(THuiGcMatrix aMatrix)
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        vgLoadIdentity();
        
        TRect displayArea = DisplayArea();            
        TInt h = displayArea.Height();
        TInt w = displayArea.Width();

        // DisplayArea is rotated if orientation is rotated, so we must "counter rotate"
        // to avoid rotating too much.
        if(Orientation() == EOrientationCCW90 || Orientation() == EOrientationCW90)
            {
            TInt tmp = w;
            w = h;
            h = tmp;
            }
        
        // Flip the Y axis to match Hitchcock's coordinate system
        vgTranslate(0, h); 
        vgScale(1.0f, -1.0f);
        
        if(Orientation() == EOrientationCCW90)
            {
            // Rotate around origo and move back to displayarea
            vgRotate(90);
            vgTranslate(0, -w);
            }
        else if(Orientation() == EOrientationCW90)
            {            
            // Rotate around origo and move back to displayarea
            vgRotate(-90);
            vgTranslate(-h, 0);
            }
        else if (Orientation() == EOrientation180)
            {
            // Rotate around origo and move back to displayarea
            vgRotate(-180);
            vgTranslate(-w, -h);            
            }                   
        else
            {
            // Nothing to do ?
            }
        
        // Reset the flags for this matrix
        iMatrixFlags = 0;
        
        HUI_VG_INVARIANT();
        }
    }


void CHuiVg10Gc::Multiply(THuiGcMatrix aStack, TReal32 aMatrix[16])
    {
    if (aStack == EHuiGcMatrixModel)
        {
        const VGfloat m[9] = 
            {
            aMatrix[0], aMatrix[1], aMatrix[2],
            aMatrix[4], aMatrix[5], aMatrix[6],
            aMatrix[8], aMatrix[9], aMatrix[10],
            };
        vgMultMatrix(m);
        HUI_VG_INVARIANT();
        }
    }


void CHuiVg10Gc::Translate(THuiGcMatrix aMatrix,
                           TReal32 aX, TReal32 aY, TReal32 /*aZ*/) __SOFTFP
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        vgTranslate(aX, aY);
        HUI_VG_INVARIANT();
        }
    }


void CHuiVg10Gc::Scale(THuiGcMatrix aMatrix,
                         TReal32 aX, TReal32 aY, TReal32 /*aZ*/) __SOFTFP
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        vgScale(aX, aY);
        HUI_VG_INVARIANT();
        }
    }

void CHuiVg10Gc::Shear(THuiGcMatrix aMatrix,
                       TReal32 aX, TReal32 aY, TReal32 /*aZ*/) __SOFTFP
    {
    if (aMatrix == EHuiGcMatrixModel)
        {
        vgShear(aX, aY);
        HUI_VG_INVARIANT();
        }
    }

void CHuiVg10Gc::Rotate(THuiGcMatrix aMatrix, TReal32 aAngle,
                        TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP
    {
    if (aMatrix != EHuiGcMatrixModel)
        {
        return;
        }

    // Only rotation around the unit Z axis is supported
	if (aX != 0.0f || aY != 0.0f || Abs(aZ) != 1.0f)
	    {
	    return;
	    }
		
    if (aZ < 0)
        {
        aAngle = -aAngle;
        }
    
    iMatrixFlags |= EMatrixFlagsRotation;
    vgRotate(aAngle);
    HUI_VG_INVARIANT();
    }


// Replicate the current OpenVG matrix to the matrix stack
void CHuiVg10Gc::UpdateMatrix(VGMatrixMode aMatrix)
    {
    VGfloat m[9];
    ASSERT(vgGeti(VG_MATRIX_MODE) == VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgGetMatrix(m);
    vgSeti(VG_MATRIX_MODE, aMatrix);
    vgLoadMatrix(m);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::UpdateClientMatrix()
    {
    // Replicate the current OpenVG matrix to the matrix stack
    vgGetMatrix((VGfloat*)iMatrixStack->Current().iMatrix);
    
    // The matrix is *probably* not an identity transform
    iMatrixStack->Current().iIsIdentity = EFalse;
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::CreateVgObjectsL()
    {
    // Make sure we have enough resources to work with
    ASSERT(vgGeti(VG_MAX_SCISSOR_RECTS) >= 1);
    
    // Create the default VG objects
    iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                         5, 8, VG_PATH_CAPABILITY_APPEND_TO);
    iImagePath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                              5, 8, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);
    iArcPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                            2, 7, VG_PATH_CAPABILITY_APPEND_TO);
    iPaint = vgCreatePaint();
    iPaintColor = 0;
    vgSetColor(iPaint, iPaintColor);
    iGradientPaint = vgCreatePaint();
    iBlendMode = VG_BLEND_SRC_OVER;

    // Initialize a rectangle path with dummy coordinates
    iRectPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                             5, 8, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);

    iRoundRectPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                                 5, 8, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);

    iEllipsePath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                                 5, 8, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);
    
    const VGubyte rectSegments[] =
        {
        VG_MOVE_TO_ABS,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_CLOSE_PATH
        };
    const VGfloat rectCoords[] = 
        {
         0.0f, 0.0f,             // (x, y)
         1.0f, 0.0f,             // (width, 0)
         0.0f, 1.0f,             // (0, height)
        -1.0f, 0.0f,             // (-width, 0)
        };
    vgAppendPathData(iRectPath, 5, rectSegments, rectCoords);
    vgRemovePathCapabilities(iRectPath, VG_PATH_CAPABILITY_APPEND_TO);
    vgAppendPathData(iImagePath, 5, rectSegments, rectCoords);
    vgRemovePathCapabilities(iImagePath, VG_PATH_CAPABILITY_APPEND_TO);

    // Initialize a line path with dummy coordinates
    iLinePath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                             2, 4, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);
    const VGubyte lineSegments[] =
        {
        VG_MOVE_TO_ABS,
        VG_LINE_TO_ABS,
        };
    const VGfloat lineCoords[] = 
        {
        0.0f, 0.0f,             // (x, y)
        1.0f, 1.0f,             // (x, y)
        };
    vgAppendPathData(iLinePath, 2, lineSegments, lineCoords);
    vgRemovePathCapabilities(iLinePath, VG_PATH_CAPABILITY_APPEND_TO);
    
    // Initialize a path for drawing borders
    iBorderPath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 
                               11, 10 * 2, VG_PATH_CAPABILITY_APPEND_TO | VG_PATH_CAPABILITY_MODIFY);

    const VGubyte borderSegments[12] =
        {
        VG_MOVE_TO_ABS,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_LINE_TO_REL,
        VG_CLOSE_PATH,
        };

    const VGfloat borderCoords[10 * 2] = 
        {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };

    vgAppendPathData(iBorderPath, 12, borderSegments, borderCoords);
    vgRemovePathCapabilities(iBorderPath, VG_PATH_CAPABILITY_APPEND_TO);
        
    // Get hardware configuration
    CHuiVg10RenderPlugin& renderer = (CHuiVg10RenderPlugin&) CHuiStatic::Renderer();
    TInt hwConf = renderer.GetHwConfigurationFlags();
    
    if ( hwConf & KHuiAntialiasing )
        {
        SetQuality( EHuiQualityAccurate );
        }
    else
        {
        SetQuality( EHuiQualityFast );
        }
    
    iVgObjectsCreated = ETrue;
    }

void CHuiVg10Gc::InitState()
    {
    // Clean matrix stack in case there is anything left
    // We can't construct a new stack here because we are not allowed to leave.
    // We keep the original stack and just delete the contents leaving one 
    // identity matrix that is always added when a new matrix stack is created
    TInt i = 0;
    // One identity matrix will remain.
    // When a new matrix stack is created, one identity matrix is added
    // It can never be removed, the code would panic.
    for ( i = 0; i < iMatrixStack->Count() - 2; i++ )
        {
        iMatrixStack->Pop(); 
        }
    // remove flags from stack, too.
    iMatrixFlagsStack.Reset();

    iScissorRect = TRect(0, 0, 0, 0);
    iMatrixFlags = 0;
    
    if (!iVgObjectsCreated)
        {
        TRAP_IGNORE(CreateVgObjectsL());    
        }

    // Default paint
    vgSetParameteri(iPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetPaint(iPaint, VG_FILL_PATH | VG_STROKE_PATH);
        
    // Gradient paint
    vgSetParameteri(iGradientPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
        
    HUI_VG_INVARIANT();
    
    // Reset default state
    
    Enable(EFeatureBlending);
    Disable(EFeatureDepthTest);
	iStateScissorTest = ETrue;    
    Disable(EFeatureClipping);

    // Use the image matrix as the primary matrix
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

    RestoreState();
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::SetQuality(THuiQuality aQuality)
    {
    CHuiGc::SetQuality(aQuality);
    switch (Quality())
        {
        case EHuiQualityAccurate:
            vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
            vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_BETTER);
            break;
        case EHuiQualityFast:
            vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_FASTER);
            vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_FASTER);
            break;
        default:
            break;
        }
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::SetFillRule( THuiFillMode aFillMode )
    {   
    switch (aFillMode)
       {
       case EHuiFillNonZero:
           vgSeti(VG_FILL_RULE, VG_NON_ZERO);
           break;
       case EHuiFillEvenOdd:
           vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
           break;
       default:
           break;
       }
    HUI_VG_INVARIANT();   
    }

void CHuiVg10Gc::RestoreState() const
    {
    // Restore paint
    vgSetPaint(iPaint, VG_FILL_PATH | VG_STROKE_PATH);        

    // Restore scissor test state
    if(iStateScissorTest)
        {
        vgSeti(VG_SCISSORING, VG_TRUE);
        }
    else
        {
        vgSeti(VG_SCISSORING, VG_FALSE);
        }
    
    // Restore scissor rects
    // This fixes CHuiFxEffect cached draw
    TInt count = iClipRegion.Count();
    VGint coords[KHuiMaxVgScissorRects * 4];

    if(count > KHuiMaxVgScissorRects)
        {
        count = KHuiMaxVgScissorRects; //only 32 scissor rects supported    
        }
        
    TInt numberOfCoords = count*4;
    for ( TInt i = 0, j=0 ; i < numberOfCoords ; j++)
        { 
        coords[i++] = iClipRegion[j].iTl.iX;
        coords[i++] = iClipRegion[j].iTl.iY;
        coords[i++] = iClipRegion[j].Width();
        coords[i++] = iClipRegion[j].Height();      
        }
    
    vgSetiv(VG_SCISSOR_RECTS, numberOfCoords, coords);          

    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::RestoreFlaggedState() const
    {
    // Get the changed stateflags from the renderplugin
    CHuiVg10RenderPlugin& renderer = (CHuiVg10RenderPlugin&) CHuiStatic::Renderer();
    TInt flags = renderer.GetRestoreStateFlags();
    
    // First a quick check, if nothing needs to be done
    if(!flags)
        {
        return;
        }
      
    // Check each flag and act accrodingly!
    if(flags & EHuiVg10GcStateFlagDirtyPaint)
        {
        // Restore paint
        vgSetPaint(iPaint, VG_FILL_PATH | VG_STROKE_PATH);
        }

    if(flags & EHuiVg10GcStateFlagDirtyScissor)
        {
        // Restore scissor test state
        if(iStateScissorTest)
            {
            vgSeti(VG_SCISSORING, VG_TRUE);
            }
        else
            {
            vgSeti(VG_SCISSORING, VG_FALSE);
            }
        }
    
    if(flags & EHuiVg10GcStateFlagDirtyScissorRects)
        {
        // Restore scissor rects
        TInt count = iClipRegion.Count();
        VGint coords[KHuiMaxVgScissorRects * 4];

        if(count > KHuiMaxVgScissorRects)
            {
            count = KHuiMaxVgScissorRects; //only 32 scissor rects supported    
            }
            
        TInt numberOfCoords = count*4;
        for ( TInt i = 0, j=0 ; i < numberOfCoords ; j++)
            { 
            coords[i++] = iClipRegion[j].iTl.iX;
            coords[i++] = iClipRegion[j].iTl.iY;
            coords[i++] = iClipRegion[j].Width();
            coords[i++] = iClipRegion[j].Height();      
            }
        
        vgSetiv(VG_SCISSOR_RECTS, numberOfCoords, coords);
        }

    // Clear the flags!
    renderer.ClearRestoreStateFlags();
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::UpdateProjection()
    {
    // Nothing to do
    }


TInt CHuiVg10Gc::TextureUnits() const
    {
    return 1;
    }


void CHuiVg10Gc::SetTextureUnits(TInt /*aTextureUnitCount*/)
    {
    // Nothing to do
    }


void CHuiVg10Gc::Enable(TFeature aFeature, TBool aDoEnable)
    {
    HUI_VG_INVARIANT();

    switch(aFeature)
        {
        case EFeatureDepthTest:
            // Not supported
            break;

        case EFeatureDepthWrite:
            // Not supported
            break;

        case EFeatureTexturing:
            // Always enabled
            break;

        case EFeatureBlending:
            if(aDoEnable)
                {
                vgSeti(VG_BLEND_MODE, iBlendMode);
                }
            else
                {
                vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);
                }
            break;

        case EFeatureClipping:
            if(aDoEnable)
                {
                // enable clipping
                //if(!iStateScissorTest)
                	{
                	iStateScissorTest = ETrue;
                    vgSeti(VG_SCISSORING, VG_TRUE);
                	}
                }
            else
                {
                // disable clipping
                //if(iStateScissorTest)
                	{
                	iStateScissorTest = EFalse;
                    vgSeti(VG_SCISSORING, VG_FALSE);
                	}
                
                }
            break;

        case EFeatureAntialias:
            if(aDoEnable)
                {
                vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
                }
            else
                {
                vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_FASTER);
                }
            break;

        case EFeatureFog:
            // Not supported
            break;

        default:
            break;
        }

    HUI_VG_INVARIANT();
    }


void CHuiVg10Gc::SetBlendMode(TBlendMode aBlendMode)
    {
    switch(aBlendMode)
        {
        case EBlendAdd:
            iBlendMode = VG_BLEND_ADDITIVE;
            break;

        default:
            iBlendMode = VG_BLEND_SRC_OVER;
            break;
        }
        vgSeti(VG_BLEND_MODE, iBlendMode);
    }


void CHuiVg10Gc::SetTextureMode(TTextureMode aTextureMode, TReal32 /*aParam*/) __SOFTFP
    {
    switch(aTextureMode)
        {
        case ETextureModeNormal:
            break;

        default:
            break;
        }

    HUI_VG_INVARIANT();
    }


void CHuiVg10Gc::SetDimmingFog(const TRgb& /*aColor*/, TReal32 /*aAmount*/) __SOFTFP
    {
    // Fog not supported
    }

void CHuiVg10Gc::Clear(const TRect& aRect)
    {
    HUI_VG_INVARIANT();
    
    UpdateClientMatrix();

    THuiRealRect transformed = aRect;
    iMatrixStack->Current().Multiply(transformed.iTl);
    iMatrixStack->Current().Multiply(transformed.iBr);
        
    // Normalize the rectangle
    if (transformed.iTl.iX > transformed.iBr.iX)
        {
        TReal32 tmp = transformed.iTl.iX;
        transformed.iTl.iX = transformed.iBr.iX;
        transformed.iBr.iX = tmp;
        }
    if (transformed.iTl.iY > transformed.iBr.iY)
        {
        TReal32 tmp = transformed.iTl.iY;
        transformed.iTl.iY = transformed.iBr.iY;
        transformed.iBr.iY = tmp;
        }            
    VGfloat scale = 1.0f / 255.0f;
    VGfloat color[] = 
        {
        PenColor().Red()   * scale,
        PenColor().Green() * scale,
        PenColor().Blue()  * scale,
        PenAlpha() * scale
        };

    vgSetfv(VG_CLEAR_COLOR, 4, color);
    vgClear(transformed.iTl.iX, transformed.iTl.iY, transformed.Width(), transformed.Height());
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::Clear()
    {
    HUI_VG_INVARIANT();
    TInt x = ProjectionViewport().iTl.iX;
    TInt y = ProjectionViewport().iTl.iY;
    TInt w = ProjectionViewport().Width();
    TInt h = ProjectionViewport().Height();
    VGfloat scale = 1.0f / 255.0f;
    VGfloat color[] = 
        {
        PenColor().Red()   * scale,
        PenColor().Green() * scale,
        PenColor().Blue()  * scale,
        PenAlpha() * scale
        };
    vgSetfv(VG_CLEAR_COLOR, 4, color);
    vgClear(x, y, w, h);
    HUI_VG_INVARIANT();
    }

	
void CHuiVg10Gc::ClearDepth()
    {
    Clear();
    }


void CHuiVg10Gc::ClearColorAndDepth()
    {
    Clear();
    }

// Do renderer specific initializations of the frame
void CHuiVg10Gc::InitNewFrame()
	{
	LoadIdentity(EHuiGcMatrixModel);
	}

void CHuiVg10Gc::Clip(const TRect& aClipRect)
    {
    THuiRealRect transformed = aClipRect;
    
    HUI_DEBUGF4(_L("CHuiVg10Gc::Clip() - aClipRect     (%5i, %5i, %5i, %5i)"),
                aClipRect.iTl.iX, aClipRect.iTl.iY,
                aClipRect.iBr.iX, aClipRect.iBr.iY);
    
    // Transform the rectangle using our stacks
    UpdateClientMatrix();
    iMatrixStack->Current().Multiply(transformed.iTl);
    iMatrixStack->Current().Multiply(transformed.iBr);
    
    // Normalize the rectangle
    if (transformed.iTl.iX > transformed.iBr.iX)
    	{
    	TReal32 tmp = transformed.iTl.iX;
    	transformed.iTl.iX = transformed.iBr.iX;
    	transformed.iBr.iX = tmp;
    	}
    if (transformed.iTl.iY > transformed.iBr.iY)
    	{
    	TReal32 tmp = transformed.iTl.iY;
    	transformed.iTl.iY = transformed.iBr.iY;
    	transformed.iBr.iY = tmp;
    	}

    HUI_DEBUGF4(_L("CHuiVg10Gc::Clip() - Transformed = (%5.0f, %5.0f, %5.0f, %5.0f)"),
                transformed.iTl.iX, transformed.iTl.iY,
                transformed.iBr.iX, transformed.iBr.iY);

    CHuiGc::Clip(transformed.Round());
    HUI_VG_INVARIANT();
    }


void CHuiVg10Gc::SetClip(const TRect& aClipRect)
    {
    iScissorRect = aClipRect;
    iScissorRect.Normalize();
    
    CHuiGc::SetClip(iScissorRect);

    VGint coords[4] = 
        {
        iScissorRect.iTl.iX,  iScissorRect.iTl.iY,
        iScissorRect.Width(), iScissorRect.Height()
        };
   
    Enable(EFeatureClipping);    
    vgSetiv(VG_SCISSOR_RECTS, 4, coords);
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::Clip(const TRegion& aRegion)
    {
    UpdateClientMatrix();

    iTempRegion.Clear();
    
    for (TInt i=0; i<aRegion.Count(); i++)
        {
        THuiRealRect transformed = aRegion[i];
        iMatrixStack->Current().Multiply(transformed.iTl);
        iMatrixStack->Current().Multiply(transformed.iBr);
        
        // Normalize the rectangle
        if (transformed.iTl.iX > transformed.iBr.iX)
        	{
        	TReal32 tmp = transformed.iTl.iX;
        	transformed.iTl.iX = transformed.iBr.iX;
        	transformed.iBr.iX = tmp;
        	}
        if (transformed.iTl.iY > transformed.iBr.iY)
        	{
        	TReal32 tmp = transformed.iTl.iY;
        	transformed.iTl.iY = transformed.iBr.iY;
        	transformed.iBr.iY = tmp;
        	}            
        
        iTempRegion.AddRect(transformed.Round());
        }
	    
	CHuiGc::Clip(iTempRegion);	    
    
    HUI_VG_INVARIANT();
    }
    
void CHuiVg10Gc::SetClipRegion(const TRegion& aRegion)
    {
    TInt count = aRegion.Count();
    VGint coords[KHuiMaxVgScissorRects * 4];

    if(count > KHuiMaxVgScissorRects)
        {
        count = KHuiMaxVgScissorRects; //only 32 scissor rects supported    
        }
        
    TInt numberOfCoords = count*4;
    for ( TInt i = 0, j=0 ; i < numberOfCoords ; j++)
        { 
        coords[i++] = aRegion[j].iTl.iX;
        coords[i++] = aRegion[j].iTl.iY;
        coords[i++] = aRegion[j].Width();
        coords[i++] = aRegion[j].Height();      
        }
    
    CHuiGc::SetClipRegion(aRegion);     
    
    Enable(EFeatureClipping);    
    vgSetiv(VG_SCISSOR_RECTS, numberOfCoords, coords);          
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::CancelClipping()
    {
    Disable(EFeatureClipping);    
    }

void CHuiVg10Gc::DrawText(const CHuiTextMesh& aTextMesh,
                          const THuiRealRect& aBounds,
                          TReal32 aShadow) __SOFTFP
    {
    CHuiGc::DrawText(aTextMesh, aBounds, aShadow);
    }


/**
 *  Given a THuiImage, return the corresponding VGImage or 
 *  VG_INVALID_HANDLE if there is none.
 */
static VGImage GetVGImageFromTexture(const MHuiSegmentedTexture& aTexture)
    {
    if (aTexture.SegmentCount() == 1)
        {
        return (VGImage)aTexture.SegmentName(0);
        }
    return VG_INVALID_HANDLE;
    }


/**
 *  Construct a source rectangle based on the texture coordinates
 *  specified for an image.
 */
static THuiRealRect GetSourceRectangleFromImage(const THuiImage& aImage)
    {
    TReal32 texCoords[8];
    aImage.GetTexCoords(texCoords);
    THuiRealRect sourceRect;
    
    sourceRect.iTl.iX = texCoords[0];
    sourceRect.iTl.iY = texCoords[1];
    sourceRect.iBr.iX = texCoords[4];
    sourceRect.iBr.iY = texCoords[5];
    
    return sourceRect;
    }

void CHuiVg10Gc::DoDrawImage(const THuiImage& aImage,
                             const THuiRealPoint& aTopLeft,
                             const THuiRealSize& aSize)
    {
    if(!aImage.HasTexture())
        {
        return;
        }
    
    // Fetch the texture coordinates from the image
    THuiRealRect destRect(aTopLeft, aSize);
    THuiRealRect sourceRect(GetSourceRectangleFromImage(aImage));
    DrawTexture(aImage.Texture(), sourceRect, destRect);
    }


void CHuiVg10Gc::DoDrawImages(const THuiImage& aImage,
                                const THuiImage& aImage2,
                                const THuiRealPoint& aTopLeft,
                                const THuiRealSize& aSize)
    {
    DoDrawImage(aImage,  aTopLeft, aSize);
    DoDrawImage(aImage2, aTopLeft, aSize);
    }


void CHuiVg10Gc::UpdateColor(TReal32 /*aAlphaFactor*/) __SOFTFP
    {
    VGuint color = (PenColor().Internal() << 8) | PenAlpha();
    
#if defined(RENDER_DEBUG_RECTANGLES)
    color = Math::Random() | 0xff;
#endif
    vgSetPaint(iPaint, VG_FILL_PATH | VG_STROKE_PATH);
   // Update the color of the current paint
    // if required
    if (iPaintColor != color)
        {
        vgSetColor(iPaint, color);
        iPaintColor = color;
        }
    
    // Detect white fully opaque color
    if (color == 0xffffffff)
        {
        vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
        }
    else
        {
        vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
        }
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawCroppedTexture(const MHuiSegmentedTexture& aTexture, const THuiRealRect& aSourceRect,
                                    const THuiRealRect& aDestRect)
    {
    // Render state which affects what we do here (roughly decreasing order of probability):
    // - Source cropping
    // - Color modulation
    // - Alpha modulation
    // - Affine matrix transform (i.e. other than scale or translation)
    //
    // Difficult combinations:
    // - Source cropping with color or alpha modulation
    //   - Option 1: Create a child image from the cropped region
    //     - Can't do this because the cropping region is in floating point coords.
    //     - Performance overhead possibly non-negligible.
    //   - Option 2: Use scissoring to do the cropping
    //     - Does not work for rotate/skew transform.
    //   - Option 3: Use a mask to do the cropping
    //     - Auxiliary mask bitmap needed, possible performance hit.
    //   - Option 4: Use a pattern paint
    //     - Color/alpha modulation requires an offscreen image.
    // - Resolution: Go with option 2 and option 1 if rotation/skewing is in effect.
    TSize imageSize = aTexture.Size();
    VGfloat srcW   = aSourceRect.Width()  * imageSize.iWidth;
    VGfloat srcH   = aSourceRect.Height() * imageSize.iHeight;
    VGfloat srcX   = aSourceRect.iTl.iX   * imageSize.iWidth;
    VGfloat srcY   = aSourceRect.iTl.iY   * imageSize.iHeight;

    // Make sure the source rectangle is not degenerate
    srcW = Max(1.0f, srcW);
    srcH = Max(1.0f, srcH);
    
    VGfloat scaleX = aDestRect.Width()  / srcW;
    VGfloat scaleY = aDestRect.Height() / srcH;
    VGImage image = VG_INVALID_HANDLE;
    
	// Check if OpenVg state has changed
    RestoreFlaggedState();
    
#ifdef __NVG
    const CHuiVg10Texture& texture = static_cast<const CHuiVg10Texture&>( aTexture );
    TBool isExtended = texture.IsExtended();
#endif
    
    // Getting the vgImage out of the texture
    image = GetVGImageFromTexture( aTexture );
    
    if (iMatrixFlags & EMatrixFlagsRotation)
        {
        VGImage tmpImage = vgChildImage(image, (VGint)srcX, imageSize.iHeight - (VGint)srcY - (VGint)srcH, (VGint)srcW, (VGint)srcH);
        
        if (tmpImage == VG_INVALID_HANDLE)
            {
            HUI_DEBUG(_L("CHuiVg10Gc::DrawCroppedTexture() - Unable to create temporary child image."));
            // We cannot leave because we are called by a non-leaving exported function
            // If we cannot draw, we just return without doing anything.
            return;
            }
        HUI_VG_INVARIANT();
        
        // Render the child image
        Push(EHuiGcMatrixModel);
#if !defined(RENDER_DEBUG_RECTANGLES)
        vgTranslate(aDestRect.iTl.iX, aDestRect.iTl.iY);
        vgScale(scaleX, scaleY);
        vgDrawImage(tmpImage);
#else
        TRect d(aDestRect.iTl.iX, aDestRect.iTl.iY, aDestRect.iBr.iX, aDestRect.iBr.iY);
        DrawRect(d);
#endif
        Pop(EHuiGcMatrixModel);
        
        vgDestroyImage(tmpImage);
        HUI_VG_INVARIANT();
        return;
        }
    
    // Set a scissor rectangle that corresponds with the target rectangle
    TBool clippingWasEnabled = iStateScissorTest;
    
    if (!clippingWasEnabled)
        {
        Enable(EFeatureClipping);    
        }
    
    PushClip();
    Clip(aDestRect);    
    
    // Render the image
    Push(EHuiGcMatrixModel);
#if !defined(RENDER_DEBUG_RECTANGLES)
    vgTranslate(aDestRect.iTl.iX, aDestRect.iTl.iY);
    vgScale(scaleX, scaleY);
    vgTranslate(-srcX, -srcY);

#ifdef __NVG
    if ( isExtended )
        {
        // Determine the size to which we want to draw the NVG icon paths
        TSize contentSize( HUI_ROUND_FLOAT_TO_INT(aDestRect.Width()),
                HUI_ROUND_FLOAT_TO_INT(aDestRect.Height()) );
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        // Draw the NVG data (be it either the object cached commands or the "normal" nvg data)
        DrawNVG(texture.GetExtendedTextureData(), texture.GetIconCommandsData(), contentSize, EFalse);
        }
    else
        {
        if (image != VG_INVALID_HANDLE)
            {
            //HUI_DEBUG1(_L("HuiVg10Gc::DrawCroppedTexture - drawing with vgDrawImage(%d) instead of DrawNVG"), image);
            vgDrawImage(image);
            }
        else
            {
            HUI_DEBUG1(_L("HuiVg10Gc::DrawCroppedTexture - INVALID HANDLE, vgDrawImage() not called!"), image);
            }
        }
#else
    vgDrawImage(image);
#endif // __NVG

#else // RENDER_DEBUG_RECTANGLES
    TRect d(aDestRect.iTl.iX, aDestRect.iTl.iY, aDestRect.iBr.iX, aDestRect.iBr.iY);
    DrawRect(d);
#endif
    HUI_VG_INVARIANT();
    Pop(EHuiGcMatrixModel);
    
    // Restore state
    PopClip();

    if (!clippingWasEnabled)
        {
        Disable(EFeatureClipping);    
        }
    
    HUI_VG_INVARIANT();
    
#if 0
    // Crop implementation using a paint pattern
    // TODO: Handle color modulation + cropping
    VGfloat srcW   = aSourceRect.Width()  * aImage.Texture().Size().iWidth;
    VGfloat srcH   = aSourceRect.Height() * aImage.Texture().Size().iHeight;
    VGfloat srcX   = aSourceRect.iTl.iX   * aImage.Texture().Size().iWidth;
    VGfloat srcY   = aSourceRect.iTl.iY   * aImage.Texture().Size().iHeight;
    VGfloat scaleX = aDestRect.Width()  / srcW;
    VGfloat scaleY = aDestRect.Height() / srcH;
    VGImage image  = GetVGImageFromTexture(aTexture);

    vgPaintPattern(iPaint, image);
    
    // Transform the paint pattern to fill the destination rectangle as required
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
    vgLoadIdentity();
    vgTranslate(aDestRect.iTl.iX, aDestRect.iTl.iY);
    vgScale(scaleX, scaleY);
    vgTranslate(-srcX, -srcY);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    
    // Update the path transformation matrix
    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);

    // Draw a rectangle which is filled with the image as a pattern
    const VGfloat coords[] = 
        {
        aDestRect.iTl.iX, aDestRect.iTl.iY,
        aDestRect.Width(), 0,
        0, aDestRect.Height(),
        -aDestRect.Width(), 0,
        };
    vgModifyPathCoords(iImagePath, 0, 5, coords);
    vgDrawPath(iImagePath, VG_FILL_PATH);
    
    // Restore the state
    vgPaintPattern(iPaint, VG_INVALID_HANDLE);
    UpdateColor();
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
    vgLoadIdentity();
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    HUI_VG_INVARIANT();
#endif
    }

void CHuiVg10Gc::DrawTexture(const MHuiSegmentedTexture& aTexture, 
                             const THuiRealRect& aSourceRect, 
                             const THuiRealRect& aDestRect)
    {
    // See if the image is cropped
    const VGfloat epsilon = 1e-3;
    if (aSourceRect.iTl.iX > epsilon || 
        aSourceRect.iTl.iY > epsilon ||
        aSourceRect.iBr.iX < 1.0f - epsilon ||
        aSourceRect.iBr.iY < 1.0f - epsilon)
        {
        DrawCroppedTexture(aTexture, aSourceRect, aDestRect);
        return;
        }

	// Check if OpenVg state has changed
    RestoreFlaggedState();
    
    Push(EHuiGcMatrixModel);
#if !defined(RENDER_DEBUG_RECTANGLES)
    vgTranslate(aDestRect.iTl.iX, aDestRect.iTl.iY);
    
#ifdef __NVG
    const CHuiVg10Texture& texture = static_cast<const CHuiVg10Texture&>( aTexture );
    TBool isExtended = texture.IsExtended();
    
    if (isExtended)
        {
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        TSize contentSize(0, 0);
        contentSize.SetSize( HUI_ROUND_FLOAT_TO_INT( aDestRect.Width() ),
            HUI_ROUND_FLOAT_TO_INT( aDestRect.Height() ) );
        // Draw the NVG data (be it either the object cached commands or the "normal" nvg data)
        DrawNVG(texture.GetExtendedTextureData(), texture.GetIconCommandsData(), contentSize, EFalse);
        }
    else
        {
        // "Option C" -way
        VGfloat scaleX = aDestRect.Width()  / aTexture.Size().iWidth;
        VGfloat scaleY = aDestRect.Height() / aTexture.Size().iHeight;
        VGImage image = GetVGImageFromTexture( aTexture );

        if (image != VG_INVALID_HANDLE)
            {
            vgScale(scaleX, scaleY);
            vgDrawImage(image);
            //HUI_DEBUG1(_L("HuiVg10Gc::DrawTexture - vgDrawImage(%d) drawn!"), image);
            }
        else
            {
            HUI_DEBUG(_L("HuiVg10Gc::DrawTexture - INVALID HANDLE, vgDrawImage() not called!"));
            }
        }
#else
    // TODO: Fast paths for unit scale and opaque blending
    VGfloat scaleX = aDestRect.Width()  / aTexture.Size().iWidth;
    VGfloat scaleY = aDestRect.Height() / aTexture.Size().iHeight;
    VGImage image = GetVGImageFromTexture(aTexture);
    
    vgScale(scaleX, scaleY);
    vgDrawImage(image);
#endif // __NVG
    
#else
    TRect d(aDestRect.iTl.iX, aDestRect.iTl.iY, aDestRect.iBr.iX, aDestRect.iBr.iY);
    DrawRect(d);
#endif

    HUI_VG_INVARIANT();
    Pop(EHuiGcMatrixModel);
    }

void CHuiVg10Gc::DrawStretchImage(TStretchMode aMode,
                                  const THuiImage& aImage, 
                                  const THuiRealRect& aRect,
                                  TInt aStartWidth, 
                                  TInt aEndWidth)
    {
    if (!aImage.HasTexture())
        {
        return;
        }

    UpdateColor();

    THuiRealPoint aTopLeft(aRect.iTl);
    THuiRealSize  aSize(aRect.iBr.iX - aRect.iTl.iX, aRect.iBr.iY - aRect.iTl.iY);
    THuiRealRect sourceRect(GetSourceRectangleFromImage(aImage));
    THuiRealSize imageSize = aImage.Texture().Size();
    
	switch(aMode) 
		{
	    case EStretchFull:
			{
			DrawTexture(aImage.Texture(), sourceRect, aRect);
	    	break;
			}
				    
	    case EStretchVertical:
	        {
            THuiRealRect destRect(aRect);
            if (aStartWidth > 0)
                {
                THuiRealRect edgeDestRect(aRect);
                THuiRealRect edgeSourceRect(sourceRect);
                edgeDestRect.iBr.iY   = edgeDestRect.iTl.iY   + aStartWidth;
                edgeSourceRect.iBr.iY = edgeSourceRect.iTl.iY + aStartWidth / imageSize.iHeight;
                DrawTexture(aImage.Texture(), edgeSourceRect, edgeDestRect);
                destRect.iTl.iY   += aStartWidth;
                sourceRect.iTl.iY += aStartWidth / imageSize.iHeight;
                }
            if (aEndWidth > 0)
                {
                THuiRealRect edgeDestRect(aRect);
                THuiRealRect edgeSourceRect(sourceRect);
                edgeDestRect.iTl.iY   = edgeDestRect.iBr.iY   - aEndWidth;
                edgeSourceRect.iTl.iY = edgeSourceRect.iBr.iY - aEndWidth / imageSize.iHeight;
                DrawTexture(aImage.Texture(), edgeSourceRect, edgeDestRect);
                destRect.iBr.iY   -= aEndWidth;
                sourceRect.iBr.iY -= aEndWidth / imageSize.iHeight;
                }
            DrawTexture(aImage.Texture(), sourceRect, destRect);
	        break;
	        }
	    case EStretchHorizontal:
	        {
            THuiRealRect destRect(aRect);
            if (aStartWidth > 0)
                {
                THuiRealRect edgeDestRect(aRect);
                THuiRealRect edgeSourceRect(sourceRect);
                edgeDestRect.iBr.iX   = edgeDestRect.iTl.iX   + aStartWidth;
                edgeSourceRect.iBr.iX = edgeSourceRect.iTl.iX + aStartWidth / imageSize.iWidth;
                DrawTexture(aImage.Texture(), edgeSourceRect, edgeDestRect);
                destRect.iTl.iX   += aStartWidth;
                sourceRect.iTl.iX += aStartWidth / imageSize.iWidth;
                }
            if (aEndWidth > 0)
                {
                THuiRealRect edgeDestRect(aRect);
                THuiRealRect edgeSourceRect(sourceRect);
                edgeDestRect.iTl.iX   = edgeDestRect.iBr.iX   - aEndWidth;
                edgeSourceRect.iTl.iX = edgeSourceRect.iBr.iX - aEndWidth / imageSize.iWidth;
                DrawTexture(aImage.Texture(), edgeSourceRect, edgeDestRect);
                destRect.iBr.iX   -= aEndWidth;
                sourceRect.iBr.iX -= aEndWidth / imageSize.iWidth;
                }
            DrawTexture(aImage.Texture(), sourceRect, destRect);
	        break;
	        }
	
	    default:
	        HUI_DEBUG(_L("CHuiVg10Gc::DrawStretchImage() - ERROR! Tried to use stretch mode that hasn't been implemented. Panicing."));
		    HUI_PANIC(THuiPanic::ENotImplemented);
	}
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawRect(const TRect& aRect)
    {
    if (iPaintPattern)
        {        
        // Add 0.5 to all coordinates to get them in the middle of pixels 
        TRect rect = aRect;
        rect.iTl.iX += 0.5f;
        rect.iTl.iY += 0.5f;
        rect.iBr.iX -= 0.5f;
        rect.iBr.iY -= 0.5f;
        
        const VGfloat coords[] = {
                rect.iTl.iX + 0.5f, rect.iTl.iY + 0.5f,
                rect.Width(), 0.0f,
                0.0f, rect.Height(),
                -rect.Width(), 0.0f,
            };

        UpdateColor();
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        
        UsePaintPattern();
        
        vgModifyPathCoords(iRectPath, 0, 4, coords);
        vgDrawPath(iRectPath, VG_FILL_PATH);
        
        // Restore rect path
        const VGfloat rectCoords[] = 
            {
             0.0f, 0.0f,             // (x, y)
             1.0f, 0.0f,             // (width, 0)
             0.0f, 1.0f,             // (0, height)
            -1.0f, 0.0f,             // (-width, 0)
            };

        vgModifyPathCoords(iRectPath, 0, 4, rectCoords);        

        DiscardPaintPattern();
        }
    else
        {
        // fast path (doen not work with patterned fill)
        UpdateColor();
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    
        vgTranslate(aRect.iTl.iX, aRect.iTl.iY);
        vgScale(aRect.Width(),aRect.Height());
    
        vgDrawPath(iRectPath, VG_FILL_PATH);
    
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        }

    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawPolygon( RArray<THuiRealPoint>& aPoints )
    {
    
    // use the path intended for dynamic shapes
    // it has type float and can hold any number of segments
       
    TInt count = aPoints.Count();
    VGfloat* coords = NULL;
    coords = new VGfloat[count * 2];
    
    VGubyte* segments = NULL;
    segments = new VGubyte[count + 1];
    
    if ( !coords || !segments )
        {
        // ran out of memory, must just return
        delete[] coords;
        delete[] segments;
        return;
        }
    
    for( TInt i = 0; i < count; i++ )
    	{
	    coords[2 * i] = aPoints[i].iX;
	    coords[2 * i + 1] = aPoints[i].iY;
	    segments[i] = VG_LINE_TO_ABS;
    	}
    // fix the first segment into move instead of draw	
    segments[0] = VG_MOVE_TO_ABS;
    // the last segment closes the path to get a solid object
    segments[count] = VG_CLOSE_PATH;
    
    UpdateColor();
    UpdateMatrix( VG_MATRIX_PATH_USER_TO_SURFACE );
    
    UsePaintPattern(); 
    
    vgAppendPathData( iPath, count + 1, segments, coords);

    vgDrawPath( iPath, VG_FILL_PATH );
    vgClearPath( iPath, VG_PATH_CAPABILITY_APPEND_TO );

    DiscardPaintPattern();
    
    // don't leak memory
    delete[] coords;
    delete[] segments;
        
    HUI_VG_INVARIANT();

    }    

void CHuiVg10Gc::DrawLine(const TPoint& aStart,
                          const TPoint& aEnd,
                          const TInt aThickness)
    {

    // Add 0.5 to all coordinates to get them in the middle of pixels 
    const VGfloat coords[] = {
            aStart.iX + 0.5f, aStart.iY + 0.5f,
            aEnd.iX + 0.5f, aEnd.iY + 0.5f,
        };
    
    UpdateColor();
    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    
    vgModifyPathCoords(iLinePath, 0, 2, coords);
    vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_SQUARE);
    vgSeti(VG_STROKE_LINE_WIDTH, aThickness);
    vgDrawPath(iLinePath, VG_STROKE_PATH);
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawEllipse(const TRect& aRect, THuiFillMode aDrawMode, const TInt aThickness)
    {
    // Fix given destination rect to be in th middle of openvg pixels
    THuiRealRect destinationRect = aRect;
    destinationRect.iTl.iX += 0.5f;
    destinationRect.iTl.iY += 0.5f;
    destinationRect.iBr.iX -= 0.5f;
    destinationRect.iBr.iY -= 0.5f;
    
    VGfloat alpha = PenAlpha()/255.0f;
    vgClearPath(iEllipsePath, VG_PATH_CAPABILITY_APPEND_TO);
    
    VGfloat cx = (destinationRect.iTl.iX + destinationRect.iBr.iX) / 2.f; 
    VGfloat cy  = (destinationRect.iTl.iY + destinationRect.iBr.iY) / 2.f;
    VGfloat width = destinationRect.Width();
    VGfloat height = destinationRect.Height();
    
    vguEllipse(iEllipsePath, cx, cy, width, height);
   
    UpdateColor();
    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    
    TInt oldThickness = vgGeti(VG_STROKE_LINE_WIDTH);
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, aThickness);
        }
    VGPaint paint;
    if (aDrawMode != EHuiNoFill)
        {
        paint = vgGetPaint(VG_FILL_PATH);
        }
     else
        {
        paint = vgGetPaint(VG_STROKE_PATH);
        }

    VGfloat color[4], modifiedColor[4];
    
    if (paint != VG_INVALID_HANDLE)
       {
       vgGetParameterfv(paint, VG_PAINT_COLOR, 4, color);
       if (color[3]!=alpha)
           {
           modifiedColor[0] = color[0];
           modifiedColor[1] = color[1];
           modifiedColor[2] = color[2];
           modifiedColor[3] = alpha;
           vgSetParameterfv(paint, VG_PAINT_COLOR, 4, modifiedColor);
           }
       else
           {
           // no reason to set back the old color. it is the same.
           paint = VG_INVALID_HANDLE; 
           }
       }

    UsePaintPattern();
    
    if (aDrawMode != EHuiNoFill)
        {
        vgDrawPath(iEllipsePath, VG_FILL_PATH);
        }
    else
        {
        vgDrawPath(iEllipsePath, VG_STROKE_PATH);
        }

    DiscardPaintPattern();
    
    if (paint != VG_INVALID_HANDLE)
        {
        vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
        HUI_VG_INVARIANT();
        }
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, oldThickness);
        }
    HUI_VG_INVARIANT();
    
    }

void CHuiVg10Gc::DrawArc(const TRect& aRect, THuiFillMode aDrawMode, const TInt aThickness, TReal32 aStart, TReal32 aEnd, TBool aIsPie) __SOFTFP
    {
    // Fix given destination rect to be in th middle of openvg pixels
    THuiRealRect destinationRect = aRect;
    destinationRect.iTl.iX += 0.5f;
    destinationRect.iTl.iY += 0.5f;
    destinationRect.iBr.iX -= 0.5f;
    destinationRect.iBr.iY -= 0.5f;
    
    VGfloat alpha = PenAlpha() / 255.f;
    vgClearPath(iArcPath, VG_PATH_CAPABILITY_APPEND_TO);
    
    VGfloat cx = (destinationRect.iTl.iX + destinationRect.iBr.iX) / 2.f; 
    VGfloat cy  = (destinationRect.iTl.iY + destinationRect.iBr.iY) / 2.f;
    VGfloat width = destinationRect.Width();
    VGfloat height = destinationRect.Height();
    
    VGUArcType arcType = aIsPie ? VGU_ARC_PIE : VGU_ARC_OPEN;

    // Ready to draw the arc. Note that in OpenVG, y axis points upwards. Anti-clockwise goes
    // is towards positive direction. The method CHuiVg10CanvasGc::DoDrawPieAndArc made sure that 
    // angleExtent is always positive.
    TReal32 angleExtent = aEnd - aStart;
    vguArc(iArcPath, cx, cy, width, height, aStart, angleExtent, arcType);
   
    UpdateColor();
    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    
    TInt oldThickness = vgGeti(VG_STROKE_LINE_WIDTH);
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, aThickness);
        }
    VGPaint paint;
    if (aDrawMode != EHuiNoFill)
        {
        paint = vgGetPaint(VG_FILL_PATH);
        }
     else
        {
        paint = vgGetPaint(VG_STROKE_PATH);
        }

    VGfloat color[4], modifiedColor[4];
    
    if (paint != VG_INVALID_HANDLE)
       {
       vgGetParameterfv(paint, VG_PAINT_COLOR, 4, color);
       if (color[3]!=alpha) 
           {
           modifiedColor[0] = color[0];
           modifiedColor[1] = color[1];
           modifiedColor[2] = color[2];
           modifiedColor[3] = alpha;
           vgSetParameterfv(paint, VG_PAINT_COLOR, 4, modifiedColor);
           }
       else
           {
           // no reason to set back the old color. it is the same.
           paint = VG_INVALID_HANDLE; 
           }
       }

    UsePaintPattern();
    
    if (aDrawMode != EHuiNoFill)
        {
        vgDrawPath(iArcPath, VG_FILL_PATH);
        }
    else
        {
        vgDrawPath(iArcPath, VG_STROKE_PATH);
        }

    DiscardPaintPattern();
    
    // Restore old values
    if (paint != VG_INVALID_HANDLE)
        {
        vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
        HUI_VG_INVARIANT();
        }
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, oldThickness);
        }
    HUI_VG_INVARIANT();
    
    }

void CHuiVg10Gc::DrawRoundRect(const TRect& aDestinationRect, const THuiRealSize& aSize, THuiFillMode aDrawMode, const TInt aThickness)
    {
    // Fix given destination rect to be in th middle of openvg pixels
    THuiRealRect destinationRect = aDestinationRect;
    destinationRect.iTl.iX += 0.5f;
    destinationRect.iTl.iY += 0.5f;
    destinationRect.iBr.iX -= 0.5f;
    destinationRect.iBr.iY -= 0.5f;
    
    VGfloat alpha = PenAlpha();
    vgClearPath(iRoundRectPath, VG_PATH_CAPABILITY_APPEND_TO);
    vguRoundRect(iRoundRectPath, destinationRect.iTl.iX, destinationRect.iTl.iY, destinationRect.Width(), destinationRect.Height(), aSize.iWidth, aSize.iHeight);
   
    UpdateColor();
    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    
    TInt oldThickness = vgGeti(VG_STROKE_LINE_WIDTH);
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, aThickness);
        }
    VGPaint paint;
    if (aDrawMode != EHuiNoFill)
        {
        paint = vgGetPaint(VG_FILL_PATH);
        }
     else
        {
        paint = vgGetPaint(VG_STROKE_PATH);
        }

    VGfloat color[4], modifiedColor[4];
    
    if (paint != VG_INVALID_HANDLE)
       {
       vgGetParameterfv(paint, VG_PAINT_COLOR, 4, color);
       if (color[3]!=alpha)
           {
           modifiedColor[0] = color[0];
           modifiedColor[1] = color[1];
           modifiedColor[2] = color[2];
           modifiedColor[3] = alpha;
           vgSetParameterfv(paint, VG_PAINT_COLOR, 4, modifiedColor);
           }
       else
           {
           // no reason to set back the old color. it is the same.
           paint = VG_INVALID_HANDLE; 
           }
       }

    UsePaintPattern();
    
    if (aDrawMode != EHuiNoFill)
        {
        vgDrawPath(iRoundRectPath, VG_FILL_PATH);
        }
    else
        {
        vgDrawPath(iRoundRectPath, VG_STROKE_PATH);
        }

    DiscardPaintPattern();
    
    if (paint != VG_INVALID_HANDLE)
        {
        vgSetParameterfv(paint, VG_PAINT_COLOR, 4, color);
        HUI_VG_INVARIANT();
        }
    if(oldThickness != aThickness)
        {
        vgSeti(VG_STROKE_LINE_WIDTH, oldThickness);
        }
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawBorders(const TRect& aOuterRect, 
				  TReal32 aLeftBorderWidth,
                  TReal32 aRightBorderWidth, 
				  TReal32 aTopBorderHeight,
                  TReal32 aBottomBorderHeight, 
                  TBorderMode aBorderMode,
                  const THuiImage* aImage) __SOFTFP
	{	
    UpdateColor();
    
    if (aBorderMode == EBorderImage)
        {
        TReal32 avgBorderWidth     = (aLeftBorderWidth + aRightBorderWidth) * .5f;
        TReal32 avgBorderHeight    = (aTopBorderHeight + aBottomBorderHeight) * .5f;
        THuiRealPoint borderOffset = THuiRealPoint((aRightBorderWidth   - aLeftBorderWidth) * .5f,
                                                   (aBottomBorderHeight - aTopBorderHeight) * .5f);
        DrawBorderImages(aOuterRect, avgBorderWidth, avgBorderHeight, borderOffset, aImage);
        }
    else if (aBorderMode == EBorderFixedCorners)
        {
        DrawBorderFixedCorners(aOuterRect, aLeftBorderWidth, aRightBorderWidth, 
                               aTopBorderHeight, aBottomBorderHeight, aImage);
        }
    else
        {
        // We cannot leave here because we are a non-leaving function,
        // and we cannot return an error either. 
        // If the caller gives us invalid parameters, we do nothing.
        return;
        }

    HUI_VG_INVARIANT();
	}

void CHuiVg10Gc::DrawBorderImages(const TRect& aOuterRect,
                                  TReal32 aBorderWidth,
                                  TReal32 aBorderHeight, 
                                  THuiRealPoint aOffset,
                                  const THuiImage* aImage) __SOFTFP
    {
    // If there is no image, draw a solid border
    if (!aImage || !aImage->HasTexture())
        {
        //
        // The border is drawn like this:
        //
        //      +-----------------+
        //      |                 |
        //      +---+---------+   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   +---------+   |
        //      |                 |
        //      +-----------------+
        //
        // We do not use a path with a hole, because some of the edges might become too thin.
        //
        TReal32 w = aOuterRect.Width();
        TReal32 h = aOuterRect.Height();

        const VGfloat borderCoords[10 * 2] = 
            {
            aOuterRect.iTl.iX, aOuterRect.iTl.iY,
            w, 0.0f,
            0.0f, h,
            -w, 0.0f,
            0.0f, -h + aBorderWidth,
            aBorderWidth, 0.0f,
            0.0f, h - aBorderHeight - aBorderHeight,
            w - aBorderHeight - aBorderHeight, 0.0f,
            0.0f, -h + aBorderHeight + aBorderHeight,
            -w + aBorderWidth, 0.0f,
            };

        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        vgModifyPathCoords(iBorderPath, 0, 12, borderCoords);
        vgDrawPath(iBorderPath, VG_FILL_PATH);
        
        HUI_VG_INVARIANT();
        return;
        }
    
    THuiRealRect texCoords = GetSourceRectangleFromImage(*aImage);
    THuiRealPoint midTexCoord((texCoords.iTl.iX + texCoords.iBr.iX) * .5f,
                              (texCoords.iTl.iY + texCoords.iBr.iY) * .5f);

    THuiRealRect outerRect(aOuterRect);
    outerRect.iTl += aOffset;
    outerRect.iBr += aOffset;
    THuiRealRect innerRect(aOuterRect);
    innerRect.Shrink(aBorderWidth, aBorderHeight);

    // Top left segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(texCoords.iTl, midTexCoord), 
                       THuiRealRect(outerRect.iTl, innerRect.iTl));

    // Top segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(THuiRealPoint(midTexCoord.iX, texCoords.iTl.iY), midTexCoord),
                       THuiRealRect(THuiRealPoint(innerRect.iTl.iX, outerRect.iTl.iY), innerRect.TopRight()));

    // Top right segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(THuiRealPoint(midTexCoord.iX, texCoords.iTl.iY),
                                    THuiRealPoint(texCoords.iBr.iX, midTexCoord.iY)),
                       THuiRealRect(THuiRealPoint(innerRect.iBr.iX, outerRect.iTl.iY),
                                    THuiRealPoint(outerRect.iBr.iX, innerRect.iTl.iY)));

    // Right segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(midTexCoord, 
                                    THuiRealPoint(texCoords.iBr.iX, midTexCoord.iY)),
                       THuiRealRect(innerRect.TopRight(),
                                    THuiRealPoint(outerRect.iBr.iX, innerRect.iBr.iY)));

    // Bottom right segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(midTexCoord, texCoords.iBr),
                       THuiRealRect(innerRect.iBr, outerRect.iBr));

    // Bottom segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(midTexCoord, 
                                    THuiRealPoint(midTexCoord.iX, texCoords.iBr.iY)),
                       THuiRealRect(innerRect.BottomLeft(),
                                    THuiRealPoint(innerRect.iBr.iX, outerRect.iBr.iY)));

    // Bottom left segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(THuiRealPoint(texCoords.iTl.iX, midTexCoord.iY),
                                    THuiRealPoint(midTexCoord.iX, texCoords.iBr.iY)),
                       THuiRealRect(THuiRealPoint(outerRect.iTl.iX, innerRect.iBr.iY),
                                    THuiRealPoint(innerRect.iTl.iX, outerRect.iBr.iY)));

    // Left segment
    DrawCroppedTexture(aImage->Texture(), 
                       THuiRealRect(THuiRealPoint(texCoords.iTl.iX, midTexCoord.iY), 
                                    midTexCoord),
                       THuiRealRect(THuiRealPoint(outerRect.iTl.iX, innerRect.iTl.iY),
                                    innerRect.BottomLeft()));
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawBorderFixedCorners(const TRect& aOuterRect, 
                                        TReal32 aLeftBorderWidth,
                                        TReal32 aRightBorderWidth, 
                                        TReal32 aTopBorderHeight,
                                        TReal32 aBottomBorderHeight, 
                                        const THuiImage* aImage) __SOFTFP
    {
    // If there is no image, draw a solid border
    if (!aImage || !aImage->HasTexture())
        {
        //
        // The border is drawn like this:
        //
        //      +-----------------+
        //      |                 |
        //      +---+---------+   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   |         |   |
        //      |   +---------+   |
        //      |                 |
        //      +-----------------+
        //
        // We do not use a path with a hole, because some of the edges might become too thin.
        //
        TReal32 w = aOuterRect.Width();
        TReal32 h = aOuterRect.Height();

        const VGfloat borderCoords[10 * 2] = 
            {
            aOuterRect.iTl.iX, aOuterRect.iTl.iY,
            w, 0.0f,
            0.0f, h,
            -w, 0.0f,
            0.0f, -h + aTopBorderHeight,
            aLeftBorderWidth, 0.0f,
            0.0f, h - aTopBorderHeight - aBottomBorderHeight,
            w - aLeftBorderWidth - aRightBorderWidth, 0.0f,
            0.0f, -h + aTopBorderHeight + aBottomBorderHeight,
            -w + aRightBorderWidth, 0.0f,
            };

        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        vgModifyPathCoords(iBorderPath, 0, 12, borderCoords);
        vgDrawPath(iBorderPath, VG_FILL_PATH);
        
        HUI_VG_INVARIANT();
        return;
        }
    
    THuiRealRect texCoords = GetSourceRectangleFromImage(*aImage);
    THuiRealRect innerTexCoords(texCoords);
    THuiRealSize texSize = aImage->Texture().Size();
    THuiRealRect outerRect(aOuterRect);
    THuiRealRect innerRect(aOuterRect);
    
    innerTexCoords.iTl.iX += aLeftBorderWidth    / texSize.iWidth;
    innerTexCoords.iTl.iY += aTopBorderHeight    / texSize.iHeight;
    innerTexCoords.iBr.iX -= aRightBorderWidth   / texSize.iWidth;
    innerTexCoords.iBr.iY -= aBottomBorderHeight / texSize.iHeight;

    innerRect.iTl.iX += aLeftBorderWidth;
    innerRect.iTl.iY += aTopBorderHeight;
    innerRect.iBr.iX -= aRightBorderWidth;
    innerRect.iBr.iY -= aBottomBorderHeight;

    // Top left segment
    if (aTopBorderHeight > 0.0f && aLeftBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                         THuiRealRect(texCoords.iTl, innerTexCoords.iTl), 
                         THuiRealRect(outerRect.iTl, innerRect.iTl));
        }

    // Top segment
    if (aTopBorderHeight > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(THuiRealPoint(innerTexCoords.iTl.iX, texCoords.iTl.iY), 
                                        innerTexCoords.TopRight()),
                           THuiRealRect(THuiRealPoint(innerRect.iTl.iX, outerRect.iTl.iY), 
                                        innerRect.TopRight()));
        }

    // Top right segment
    if (aTopBorderHeight > 0.0f && aRightBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(THuiRealPoint(innerTexCoords.iBr.iX, texCoords.iTl.iY),
                                        THuiRealPoint(texCoords.iBr.iX, innerTexCoords.iTl.iY)),
                           THuiRealRect(THuiRealPoint(innerRect.iBr.iX, outerRect.iTl.iY),
                                        THuiRealPoint(outerRect.iBr.iX, innerRect.iTl.iY)));
        }

    // Right segment
    if (aRightBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(innerTexCoords.TopRight(), 
                                        THuiRealPoint(texCoords.iBr.iX, innerTexCoords.iBr.iY)),
                           THuiRealRect(innerRect.TopRight(),
                                        THuiRealPoint(outerRect.iBr.iX, innerRect.iBr.iY)));
        }

    // Bottom right segment
    if (aBottomBorderHeight > 0.0f && aRightBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(innerTexCoords.iBr, texCoords.iBr),
                           THuiRealRect(innerRect.iBr, outerRect.iBr));
        }

    // Bottom segment
    if (aRightBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(innerTexCoords.BottomLeft(),
                                        THuiRealPoint(innerTexCoords.iBr.iX, texCoords.iBr.iY)),
                           THuiRealRect(innerRect.BottomLeft(),
                                        THuiRealPoint(innerRect.iBr.iX, outerRect.iBr.iY)));
        }

    // Bottom left segment
    if (aBottomBorderHeight > 0.0f && aRightBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(THuiRealPoint(texCoords.iTl.iX, innerTexCoords.iBr.iY),
                                        THuiRealPoint(innerTexCoords.iTl.iX, texCoords.iBr.iY)),
                           THuiRealRect(THuiRealPoint(outerRect.iTl.iX, innerRect.iBr.iY),
                                        THuiRealPoint(innerRect.iTl.iX, outerRect.iBr.iY)));
        }

    // Left segment
    if (aLeftBorderWidth > 0.0f)
        {
        DrawCroppedTexture(aImage->Texture(), 
                           THuiRealRect(THuiRealPoint(texCoords.iTl.iX, innerTexCoords.iTl.iY), 
                                        innerTexCoords.BottomLeft()),
                           THuiRealRect(THuiRealPoint(outerRect.iTl.iX, innerRect.iTl.iY),
                                        innerRect.BottomLeft()));
        }
    HUI_VG_INVARIANT();
    }

void CHuiVg10Gc::DrawPath(const CHuiCurvePath& aPath,
                            const TPoint& aOrigin,
                            TReal32 aStartPos,
                            TReal32 aEndPos,
                            MHuiMappingFunction* aAlphaFunction,
                            MHuiMappingFunction* aWidthFunction) __SOFTFP
    {
    // Refresh the vertices.
    CHuiCurvePath* nonConstCurvePath = const_cast<CHuiCurvePath*>(&aPath);
    nonConstCurvePath->Update(aStartPos, aEndPos, PenAlpha() / 255.f,
                              aAlphaFunction, aWidthFunction);

    aPath.Draw(aOrigin, this);
    HUI_VG_INVARIANT();
    }


void CHuiVg10Gc::DrawMesh(const CHuiMesh& /*aMesh*/, const THuiImage* /*aImage*/,
                            const THuiImage* /*aSecondaryImage*/,
                            TReal32 /*aSecondaryAlpha*/) __SOFTFP
    {
    HUI_DEBUGF(_L("CHuiVg10Gc::DrawMesh() - Method has not been implemented! Sorry."));     
    }

void CHuiVg10Gc::DrawArc(const TPoint& /*aOrigin*/, const TSize& /*aRadius*/,
                           TReal32 /*aEnd*/, TReal32 /*aStart*/, TInt /*aAnglePerSegment*/,
                           TReal32 /*aWidth*/, const THuiImage& /*aImage*/,
                           TReal32 (aAlphaFunc)(TReal32), TBool /*aAbsoluteAngleParm*/) __SOFTFP
    {
	 // not supported
	aAlphaFunc(1.0); // just to disable compiler warning
    return;
    }



void CHuiVg10Gc::DrawGradient(TGradientType aType, const TRect& aRect,
                              const TRgb& aStartColor, const TRgb& aEndColor,
                              TReal32 aStartOpacity, TReal32 aEndOpacity,
                              const THuiRealRect* aTexCoords) __SOFTFP
    {
    VGfloat gradientCoords[4];
    VGfloat x1 = (VGfloat)aRect.iTl.iX;
    VGfloat y1 = (VGfloat)aRect.iTl.iY;
    VGfloat x2 = (VGfloat)aRect.iBr.iX;
    VGfloat y2 = (VGfloat)aRect.iBr.iY;
    
    switch (aType)
        {
        case EGradientLinearUp:
            gradientCoords[0] = x1;  gradientCoords[1] = y2;
            gradientCoords[2] = x1;  gradientCoords[3] = y1;
            break;
        case EGradientLinearRight:
            gradientCoords[0] = x1;  gradientCoords[1] = y1;
            gradientCoords[2] = x2;  gradientCoords[3] = y1;
            break;
        case EGradientLinearDown:
            gradientCoords[0] = x1;  gradientCoords[1] = y1;
            gradientCoords[2] = x1;  gradientCoords[3] = y2;
            break;
        case EGradientLinearLeft:
            gradientCoords[0] = x2;  gradientCoords[1] = y1;
            gradientCoords[2] = x1;  gradientCoords[3] = y1;
            break;
        }

    VGfloat scale = 1.0f / 255.0f;
    const VGfloat gradientStops[] =
        {
        0.0f,  aStartColor.Red() * scale, aStartColor.Green() * scale, aStartColor.Blue() * scale, aStartOpacity,
        1.0f,  aEndColor.Red()   * scale, aEndColor.Green()   * scale, aEndColor.Blue()   * scale, aEndOpacity,
        };
    

    vgSetParameterfv(iGradientPaint, VG_PAINT_LINEAR_GRADIENT, 2 * 2, gradientCoords);
    vgSetParameterfv(iGradientPaint, VG_PAINT_COLOR_RAMP_STOPS, 2 * 5, gradientStops);
    vgSetPaint(iGradientPaint, VG_FILL_PATH);
    HUI_VG_INVARIANT();

    UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
    
    // Draw the gradient as an image if we have a texture, otherwise draw a regular rectangle
    if (aTexCoords)
        {
        /*
        CHuiDisplay* display = CHuiStatic::Env().CurrentDisplay();

        if (!display)
            {
            THuiPanic::Panic(THuiPanic::EVg10Invariant);
            }
        CHuiVg10RenderSurface* surface = static_cast<CHuiVg10RenderSurface*>(&display->RenderSurface());
		*/
		
        CHuiVg10RenderSurface* surface = static_cast<CHuiVg10RenderSurface*>(CHuiStatic::CurrentRenderSurface());

        if (!surface)
            {
            THuiPanic::Panic(THuiPanic::EVg10Invariant);
            }
        
        const MHuiSegmentedTexture* tex = surface->BoundTexture();

        if (!tex)
            {
            UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
            
            vgTranslate(aRect.iTl.iX, aRect.iTl.iY);
            vgScale(aRect.Width(),aRect.Height());
            
            vgDrawPath(iRectPath, VG_FILL_PATH);
            
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
            UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
            }
        else
            {
            vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY); // Will be restored by next call to UpdateColor()
            DrawTexture(*tex, *aTexCoords, aRect);
            }
        }
    else
        {
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        
        vgTranslate(aRect.iTl.iX, aRect.iTl.iY);
        vgScale(aRect.Width(),aRect.Height());
        
        vgDrawPath(iRectPath, VG_FILL_PATH);
        
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        UpdateMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
        }
    HUI_VG_INVARIANT();
    
    // Restore the original color & paint
    UpdateColor();
    }


void CHuiVg10Gc::SetDepthOffset(TInt /*aOffset*/)
    {
    // No depth offset on OpenVG
    }


TUint8* CHuiVg10Gc::CaptureLC(TSize& aSize) const
    {
    TRect displayArea = RawDisplayArea();

    TUint8* buffer = new (ELeave) TUint8[displayArea.Width() *
                                         displayArea.Height() * 4];
    CleanupStack::PushL(buffer);

    aSize = displayArea.Size();
    vgReadPixels(buffer, displayArea.Width() * 4, VG_lARGB_8888, 0, 0, 
                 displayArea.Width(), displayArea.Height());

    return buffer;
    }

#ifdef __NVG
void CHuiVg10Gc::DrawNVG(HBufC8* aNVGData, MNVGIcon* aIconCmds, const TSize& aImageSize, TBool aIgnoreAspectRatio)
    {
    HUI_VG_INVARIANT();
    
    if (!aNVGData)
        return;
    // Parse the icon header info from the extended data
    TPtr8 IconHeaderPtr((TUint8 *)aNVGData->Des().Ptr(), KIconHeaderLength, KIconHeaderLength);
    TAknIconHeader iconheader(IconHeaderPtr);

    // The rest of the data (after the iconheader) are the OVG drawing instructions
    TInt lengthAfterHeader = aNVGData->Length() - KIconHeaderLength;
    TPtr8 nvgDataVoidIC((TUint8 *)aNVGData->Des().Ptr() + KIconHeaderLength, lengthAfterHeader, lengthAfterHeader);
    
    // Set the rotation angle
    iNvgEngine->Rotate(iconheader.GetRotation(), aImageSize.iWidth >>1, aImageSize.iHeight >>1);
    
    if (aIgnoreAspectRatio == EFalse)
    {
        // Set preserve aspect ratio (if not ignored)
        TNvgAlignStatusType alignTypeValue = ENvgPreserveAspectRatio_XmidYmid;
        TNvgMeetOrSliceType meetOrSliceTypeValue = ENvgMeet;
        
        switch ( iconheader.GetScaleMode() )
            {
            case EAspectRatioPreserved:
                {
                // Use default
                break;
                }
            // Ensures NVG content fully covers the area
            // of the icon whilst preserving aspect ratio.
            case EAspectRatioPreservedSlice:
                {
                // alignTypeValue use default
                meetOrSliceTypeValue = ENvgSlice;
                break;
                }
            // EAspectRatioPreservedAndUnusedSpaceRemoved is mapped to the same
            // values as EAspectRatioNotPreserved because we already have a
            // frame buffer with the dimensions that preserves the aspect ratio.
            // This mapping ensures that NVG engine does not calculate aspect
            // ratio twice and potentially resulting in precision loss.
            case EAspectRatioPreservedAndUnusedSpaceRemoved:
            case EAspectRatioNotPreserved:
                {            
                alignTypeValue = ENvgPreserveAspectRatio_None;
                // meetOrSliceTypeValue use default
                break;
                }
            }
        iNvgEngine->SetPreserveAspectRatio(alignTypeValue, meetOrSliceTypeValue);
    }
    
    // Draw the NVG path onto the current surface using the NVGEngine
    if (aIconCmds)
        {
        aIconCmds->Draw(aImageSize, iNvgEngine);
        }
    else
        {
        // Fall back drawing directly from the nvg data
        HUI_DEBUG(_L("CHuiVg10Gc::DrawNVG() - Fallback to draw the old way (via DrawNvg)"));
        iNvgEngine->DrawNvg(nvgDataVoidIC, aImageSize, NULL, NULL);
        }
    
    // The NVG draw messes up the paint, scissoring & rects, so mark them as dirty
    TInt dirtyFlags = EHuiVg10GcStateFlagDirtyPaint |
                    EHuiVg10GcStateFlagDirtyScissor | 
                    EHuiVg10GcStateFlagDirtyScissorRects;
    ((CHuiVg10RenderPlugin&)CHuiStatic::Renderer()).AddRestoreStateFlags(dirtyFlags);
    // Restore the paint etc.
    RestoreFlaggedState();

    HUI_VG_INVARIANT();
    }
#endif

void CHuiVg10Gc::TransformDirtyRect(THuiRealRect& aRect)
    {
    THuiRealRect transformed = aRect;
    
    // Transform the rectangle using our stacks
    UpdateClientMatrix();
    iMatrixStack->Current().Multiply(transformed.iTl);
    iMatrixStack->Current().Multiply(transformed.iBr);
    
    // Normalize the rectangle
    if (transformed.iTl.iX > transformed.iBr.iX)
        {
        TReal32 tmp = transformed.iTl.iX;
        transformed.iTl.iX = transformed.iBr.iX;
        transformed.iBr.iX = tmp;
        }
    if (transformed.iTl.iY > transformed.iBr.iY)
        {
        TReal32 tmp = transformed.iTl.iY;
        transformed.iTl.iY = transformed.iBr.iY;
        transformed.iBr.iY = tmp;
        }    

    aRect = transformed;
    }

void CHuiVg10Gc::SetPaintPattern(CHuiTexture* aTexture, const TPoint& aTextureOrigin)
    {
    if (aTexture)
        {
        // TODO: This does not work if texture has NVG data only
        iPaintPattern = GetVGImageFromTexture( *aTexture );
        }
    else
        {
        iPaintPattern = VG_INVALID_HANDLE;
        }

    if (iPaintPattern)
        {
        vgSetParameteri(iPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
        vgSetParameteri(iPaint, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REPEAT);        
        iPaintPatternOrigin = aTextureOrigin;
        }
    else
        {
        vgSetParameteri(iPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
        vgSetParameteri(iPaint, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_FILL);        
        iPaintPatternOrigin = TPoint(0,0);
        }
    
    vgPaintPattern(iPaint, iPaintPattern);
    }

void CHuiVg10Gc::UsePaintPattern()
    {
    if (iPaintPattern)
        {
        // Transform the paint pattern to fill the destination rectangle as required
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
        vgLoadIdentity();
        vgTranslate(iPaintPatternOrigin.iX, iPaintPatternOrigin.iY);
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        }
    }

void CHuiVg10Gc::DiscardPaintPattern()
    {
    if (iPaintPattern)
        {
        // Restore
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
        vgLoadIdentity();
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
        }
    }

void CHuiVg10Gc::GcExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    if ( aExtensionUid == KHuiGcCopyScreenToBitmap && aExtensionParams && *aExtensionParams)
        {
        THuiGcCopyScreenToBitmapParams* params = static_cast<THuiGcCopyScreenToBitmapParams*>(*aExtensionParams);
        params->iErrorCode = DoCopyScreenToBitmap(params->iBitmap);
        }
    else
        {
        CHuiGc::GcExtension(aExtensionUid, aExtensionParams);
        }
    }

TInt CHuiVg10Gc::DoCopyScreenToBitmap(CFbsBitmap* aBitmap)
    {
    if (!aBitmap || 
        !aBitmap->Handle() || 
        aBitmap->IsCompressedInRAM() || 
        aBitmap->ExtendedBitmapType() != KNullUid )
        {
        return KErrNotSupported;
        }

    VGImageFormat dataFormat = VG_sARGB_8888_PRE;
    TBool displayModeSupported = ETrue;    
    switch (aBitmap->DisplayMode())
        {
    case EColor16MAP:
        dataFormat = VG_sARGB_8888_PRE;
        break;
    case EColor64K:
        dataFormat = VG_sRGB_565;
        break;
    case EColor16MU:
        dataFormat = VG_sXRGB_8888;
        break;
    case EColor16MA:
        dataFormat = VG_sARGB_8888;
        break;

    default:
        displayModeSupported = EFalse;
        break;
        }
    
    if (!displayModeSupported)
        {
        return KErrNotSupported;
        }
        
    TRect copyRect(aBitmap->SizeInPixels());
    if (copyRect.IsEmpty())
        {
        return KErrArgument;
        }
    
    TInt dataStride = CFbsBitmap::ScanLineLength(copyRect.Width(), 
        aBitmap->DisplayMode());

    aBitmap->BeginDataAccess();
    TUint8* data = (TUint8*)aBitmap->DataAddress();
    
    // image is upside down..
    data += (copyRect.Height() - 1)*dataStride;
    dataStride = -dataStride;
    
    vgReadPixels(
        data,
        dataStride,
        dataFormat,
        0,                // sx
        0,                // sy
        copyRect.Width(), // width
        copyRect.Height() // height
        );
    
    aBitmap->EndDataAccess(EFalse);
    return KErrNone;
    }

// End of file

