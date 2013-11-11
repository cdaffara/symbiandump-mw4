/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder source file
*
*/

 
#include <s32mem.h>
#include <libc/math.h> 
#include <e32math.h>

#include "nvg.h"
#include "nvgfittoviewbox.h"
#include "NVGUtil.h"

#include "NVGCSIcon.h"
#include "NVGTLVIcon.h"
#include "FloatFixPt.h"

#include "NVGIconData.h"

/*
 * may be we should use dynamic_cast?
 */
#define CSICON(icon)      ((CNVGCSIcon *)icon) 
#define TLVICON(icon)     ((CNVGTLVIcon *)icon) 

enum TNVGFormats
    {
    ENVGCS,
    ENVGTLV
    };

/*
 *  NVG-CS version
 */
//const TInt KVersion1   = 1;
const TInt KVersion2   = 2;

/*
 * constants for nvg file header offsets
 */ 
const TUint8 KNVG_SIGNATURE[]       = "nvg";
const TInt KNVG_SIGNATURE_LENGTH    = sizeof(KNVG_SIGNATURE) - sizeof('\0');
const TInt KNVG_COMMANDSECTION_OFS  = 2;
const TInt KNVG_VERSION_OFS         = 3;
const TInt KNVG_RGBA_OFS            = 4;
const TInt KNVG_HEADERSIZE_OFS      = 4;
const TInt KNVG_PATHDATATYPE_OFS    = 26;
const TInt KNVG_SCALE_OFS           = 28;
const TInt KNVG_BIAS_OFS            = 32;
const TInt KNVG_VIEWBOX_X_OFS       = 36;
const TInt KNVG_VIEWBOX_Y_OFS       = 40;
const TInt KNVG_VIEWBOX_WIDTH_OFS   = 44;
const TInt KNVG_VIEWBOX_HEIGHT_OFS  = 48;

const TInt KNVG_PAINTSECTION_LINEARGRAD_TRANSFORM_OFFSET = 20;
const TInt KNVG_PAINTSECTION_RADIALGRAD_TRANSFORM_OFFSET = 24;

/*
 * nvg-cs commands
 */ 
const TInt KCMD_SET_FILL_PAINT           = 4  << 24;
const TInt KCMD_SET_COLOR_RAMP           = 6  << 24;
const TInt KCMD_DRAW_PATH                = 7  << 24;
const TInt KCMD_SET_TRANSFORMATION       = 8  << 24;
const TInt KCMD_SET_STROKE_PAINT         = 5  << 24;
const TInt KCMD_SET_STROKE_WIDTH         = 9  << 24;
const TInt KCMD_SET_STROKE_LINE_JOIN_CAP = 10 << 24;
const TInt KCMD_SET_STROKE_MITER_LIMIT   = 11 << 24;

/*
 * stroke cap style
 */
const TInt KCAP_BUTT        = 1;
const TInt KCAP_SQUARE      = 2;
const TInt KCAP_ROUND       = 3;

/*
 * stroke join style
 */
const TInt KLINE_JOIN_BEVEL = 1;
const TInt KLINE_JOIN_MITER = 2;
const TInt KLINE_JOIN_ROUND = 3;

/*
 * fill paint type
 */
const TInt KPAINT_FLAT      = 1;
const TInt KPAINT_LGRAD     = 2;
const TInt KPAINT_RGRAD     = 3;

/*
 * stroke paint type
 */
const TInt KSTROKE_LGRAD        = 2;
const TInt KSTROKE_RGRAD        = 3;
const TInt KSTROKE_COLOR_RAMP   = 4;

/*
 * nvg offset
 */
const TInt KOffsetReserved1       = 6;

/*
 * transform encoding values
 */
const TInt KTRANSFORM_COMPLETE    = 0;
const TInt KTRANSFORM_SCALING     = 2;
const TInt KTRANSFORM_SHEARING    = 4;
const TInt KTRANSFORM_ROTATION    = 8;
const TInt KTRANSFORM_TRANSLATION = 16;

#ifdef OPENVG_OBJECT_CACHING
const VGfloat KIdentityMatrix[] = 
    {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f ,1.0f
    };
#endif

/*
 * function to delete array, for basic types
 */
GLDEF_C void CleanupArray( TAny * aObj )
    {
    delete [] aObj;
    }

/*
 * function to delete array of TFloatFixArray
 */
GLDEF_C void CleanupTFloatFixArray( TAny * aObj )
    {
    TFloatFixPt * ff = (TFloatFixPt *)aObj;
    delete [] ff;
    }

/**
 * @class   TNVGEngineInternal
 * This class is added to maintain BC.
 * If you want to add new member variable you can add in this class, without breaking BC.
 * It is better to add members which are depend on macro's in this class.
 */
class TNVGEngineInternal
    {
public:
    TNVGEngineInternal()
        {
#ifdef    OPENVG_OBJECT_CACHING
        iCreatingNVGIcon = 0;
        iCurrentNVGIcon  = 0;
#endif
        }
    
#ifdef    OPENVG_OBJECT_CACHING
    TInt        iCreatingNVGIcon;
    MNVGIcon *  iCurrentNVGIcon;
#endif
    };

CNvgEngine::CNvgEngine()
    : iCurrentBufferSize(1, 1),
      iPath(VG_INVALID_HANDLE),
      iPaintFill(VG_INVALID_HANDLE),
      iPaintStroke( VG_INVALID_HANDLE),
      iUserStrokePaintColor(0xFFFFFFFF),
      iRotateApplied(EFalse),
      iLastPathDataType(0),
      iPreserveAspectSetting(ENvgPreserveAspectRatio_XmidYmid),
      iSmilFitSetting(ENvgMeet),
      iVGImageBinder(0),
      iBackgroundColor(0)
    {
    }

EXPORT_C CNvgEngine* CNvgEngine::NewL()
    {
    CNvgEngine* self    = new (ELeave) CNvgEngine;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CNvgEngine::ConstructL()
    {
    vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
    vgSeti(VG_FILL_RULE, VG_NON_ZERO);
    
    iInternal = new (ELeave) TNVGEngineInternal;
    iPaintFill = vgCreatePaint();
    }

EXPORT_C CNvgEngine::~CNvgEngine()
    {
    if (iPath != VG_INVALID_HANDLE)
        {
        vgDestroyPath(iPath);
        }
    
    if (iPaintFill != VG_INVALID_HANDLE)
        {
        vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
        vgDestroyPaint(iPaintFill);
        }
		
    if (iPaintStroke != VG_INVALID_HANDLE)
        {
        vgSetPaint(VG_INVALID_HANDLE, VG_STROKE_PATH);
        vgDestroyPaint(iPaintStroke);
        }

    
#ifdef    OPENVG_OBJECT_CACHING
    if (iInternal)
        {
        delete iInternal->iCurrentNVGIcon;
        }
#endif
    
    delete iInternal;
    }

/**
 * @brief  Set the angle for rotation of the NVG graphic
 * @version
 * @param    aAngle counter-clockwise rotation by a given angle (expressed in degrees)
 *              aX, aY  point around which the rotation must take place
 * @return  None
 */
EXPORT_C void CNvgEngine::Rotate(TReal32 aAngle, TReal32 aCentreX, TReal32 aCentreY) __SOFTFP
    {
    if(aAngle)
        {
        iRotateApplied  = ETrue;
        iCentreX        = aCentreX;
        iCentreY        = aCentreY;
        iRotateAngle    = aAngle;
        }
    else
        {
        iRotateApplied  = EFalse;
        iCentreX        = 0;
        iCentreY        = 0;
        iRotateAngle    = 0;
        }
    }

/**
 * @brief  Gets the viewbox width and height from the NVG bytedata
 * @version
 * @param   aBuf NVG byte data of the file
 * @return  content dimension
 */
EXPORT_C TSize CNvgEngine::ContentDimensions(const TDesC8& aBuf)
    {
    if (aBuf.Length() < KNVG_VIEWBOX_HEIGHT_OFS + sizeof (TReal32))
        {
        return TSize(0, 0);
        }
    
    const TUint8* lBuf = aBuf.Ptr();
    TReal32 lViewboxWidth = * (TReal32*)(lBuf + KNVG_VIEWBOX_WIDTH_OFS);
    TReal32 lViewboxHeight = * (TReal32*)(lBuf + KNVG_VIEWBOX_HEIGHT_OFS);
    
    if (lViewboxWidth > 0 && lViewboxHeight > 0)
        {
        return TSize(lViewboxWidth, lViewboxHeight);
        }
    else
        {
        return TSize(0, 0);
        }
    }

TInt CNvgEngine::InitializeGC()
    {
    if (iPaintFill == VG_INVALID_HANDLE)
        {
        iPaintFill = vgCreatePaint();
        if (iPaintFill == VG_INVALID_HANDLE)
            {
            return OpenVGErrorToSymbianError(vgGetError());
            }
        }
    
    vgSetPaint(iPaintFill, VG_FILL_PATH);
    if (iPaintStroke == VG_INVALID_HANDLE)
        {
        iPaintStroke = vgCreatePaint();
        if (iPaintStroke == VG_INVALID_HANDLE)
            {
            return OpenVGErrorToSymbianError(vgGetError());
            }
        }
    
    vgSetPaint( iPaintStroke, VG_STROKE_PATH);
    
    return KErrNone;
    }

EXPORT_C void CNvgEngine::ResetNvgState()
    {
    if (iPath != VG_INVALID_HANDLE)
        {
        vgDestroyPath(iPath);
        iPath = VG_INVALID_HANDLE;
        }
    
    if (iPaintFill != VG_INVALID_HANDLE)
        {
        vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
        vgDestroyPaint(iPaintFill);
        iPaintFill = VG_INVALID_HANDLE;
        }
		
    if (iPaintStroke != VG_INVALID_HANDLE)
        {
        vgSetPaint(VG_INVALID_HANDLE, VG_STROKE_PATH);
        vgDestroyPaint(iPaintStroke);
        iPaintStroke = VG_INVALID_HANDLE;
        }
    }

EXPORT_C TInt CNvgEngine::DrawNvg(const TDesC8& aBuf, const TSize& aSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask)
    {
    TInt error = KErrNone;
    
    /*
     * Get Matrix modes and all caller matrices (must be restored afterwards)
     */ 
    UpdateClientMatrices();
    
    TRAP(error, DoDrawNVGL(aBuf, aSize, aDstBitmap, aMask));
    
    /*
     * restore everything as we may have changed matrix mode
     */             
    RestoreClientMatrices();
	vgSeti(VG_SCISSORING, VG_FALSE);

	if (error)
	    {
	    NVG_DEBUGP2("Error in NVG rendering %d", error);
	    }
    return error;
    }

EXPORT_C MNVGIcon * CNvgEngine::CreateNVGIcon(const TDesC8& aBuf, const TSize& aSize)
    {    
    NVG_DEBUGP1("Creating NVGCSIcon");
    
    MNVGIcon * nvgIcon = 0;

    /*
     * this is bit unreadable,
     * need to find a better design to separate the object caching solution from normal rendering,
     * but anyway I guess its better than those ifdef's, in that code scanner won't give any warnings
     */
    COND_COM_OC_NOC(
    {
    if (iInternal->iCurrentNVGIcon)
        {
        delete iInternal->iCurrentNVGIcon;
        }
    
    iInternal->iCurrentNVGIcon     = 0;
    iInternal->iCreatingNVGIcon    = 1;
    
    if (DrawNvg(aBuf, aSize, 0, 0) != KErrNone)
        {
        delete iInternal->iCurrentNVGIcon;
        iInternal->iCurrentNVGIcon = 0;
        }
    
    iInternal->iCreatingNVGIcon    = 0;
    nvgIcon = iInternal->iCurrentNVGIcon;
    iInternal->iCurrentNVGIcon = 0;
    },
    {
    (void)aBuf;
    (void)aSize;
    });

    return nvgIcon;
    }

void CNvgEngine::DoDrawNVGL(const TDesC8& aBuffer, const TSize& aSize, CFbsBitmap* aDstBitmap, CFbsBitmap* aMask)
    {
    TInt drawStatus = KErrNone;

    if (iCurrentBufferSize != aSize)
        { 
        iCurrentBufferSize = aSize;
        }
    
    if (iBackgroundColor)
        {
        ClearBackground();
        }

    iDstBimtap = aDstBitmap;
    
    TDereferencer nvgIconData(aBuffer);

    TUint8 * signature = nvgIconData.DerefInt8ArrayL(KNVG_SIGNATURE_LENGTH);

    // checking the 'nvg' signature
    if (Mem::Compare(signature, KNVG_SIGNATURE_LENGTH, KNVG_SIGNATURE, KNVG_SIGNATURE_LENGTH) != 0)
        {
        NVG_DEBUGP1("Not an NVG icon");
        User::Leave(KErrNotSupported);
        }

    // last two bits are for identifying the nvg type. currently nvg-cs or nvg-tlv
    TUint16   nvgType = nvgIconData.DerefInt16L(KOffsetReserved1) & 0x03;
    
    switch (nvgType)
        {
        case ENVGCS:
            {
            drawStatus = DrawCommandSectionL(&nvgIconData, aSize, aDstBitmap, aMask);
            break;
            }
        case ENVGTLV:
            {
            drawStatus = DrawTLVL(aBuffer, aSize, aDstBitmap, aMask);
            break;
            }
        default:
            drawStatus = KErrNotSupported;
            break;
        }
    
    User::LeaveIfError(drawStatus);
    }

TInt CNvgEngine::DrawTLVL(const TDesC8& aBuf, const TSize& aTargetSize, CFbsBitmap* /*aDstBitmap*/, CFbsBitmap * /*aMask*/)
    {
    TInt ret = KErrNone;
    
    // Try to set user's matrix to path matrix
    VGfloat origMatrix[9];
    vgGetMatrix(origMatrix);
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE); 
    vgLoadMatrix(origMatrix);
    
#ifndef __MIRROR_
    vgScale(1.0f, -1.0f);
    vgTranslate(0, (VGfloat)(-iCurrentBufferSize.iHeight));
#endif

    /*
     * this is bit unreadable,
     * need to find a better design to separate the object caching solution from normal rendering,
     * but anyway I guess its better than those ifdef's, in that code scanner won't give any warnings
     */
    COND_COM_OC_NOC(
    {
    if (iInternal->iCreatingNVGIcon)
        {
        iInternal->iCurrentNVGIcon = CNVGTLVIcon::NewL();
        TLVICON(iInternal->iCurrentNVGIcon)->SetVGImageBinder(iVGImageBinder);
        TLVICON(iInternal->iCurrentNVGIcon)->CreateL(aBuf, aTargetSize);
        }
    else
        {
        CNVGTLVIcon * tlvIcon = CNVGTLVIcon::NewL();
        CleanupStack::PushL(tlvIcon);
        tlvIcon->SetVGImageBinder(iVGImageBinder);
        tlvIcon->DirectDrawL(aBuf, aTargetSize);
        CleanupStack::PopAndDestroy(tlvIcon);        
        }

    },
    {
    CNVGTLVIcon * tlvIcon = CNVGTLVIcon::NewL();
    CleanupStack::PushL(tlvIcon);
    TLVICON(tlvIcon)->SetVGImageBinder(iVGImageBinder);
    TLVICON(tlvIcon)->DirectDrawL(aBuf, aTargetSize);
    CleanupStack::PopAndDestroy(tlvIcon);                    
    });
    
    vgSeti(VG_SCISSORING, VG_FALSE);
    
    return ret;
    }

TInt CNvgEngine::CreatePathHandle(TInt16 aPathDataType, TReal32 aScale, TReal32 aBias)
    {
    (void) aScale;
    (void) aBias;
    
    TInt error = KErrNone;
    
    if (iLastPathDataType != aPathDataType)
        {
        if (iPath != VG_INVALID_HANDLE)
            {
            vgDestroyPath(iPath);
            iPath = VG_INVALID_HANDLE;
            }
        }
    
    if (iPath == VG_INVALID_HANDLE)
        {
        switch (aPathDataType)
            {
            case EEightBitEncoding:
                {
                iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                VG_PATH_DATATYPE_S_16, 1.0f/2.0f, 0.0f, 0, 0,
                VG_PATH_CAPABILITY_APPEND_TO);            
                }
                break;
                       
            case ESixteenBitEncoding:
                {
                iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                VG_PATH_DATATYPE_S_16, 1.0f/16.0f, 0.0f, 0, 0,
                VG_PATH_CAPABILITY_APPEND_TO);            
                }
                break;
                       
            case EThirtyTwoBitEncoding:
                {
                iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                VG_PATH_DATATYPE_S_32, 1.0f/65536.0f, 0.0f, 0, 0,
                VG_PATH_CAPABILITY_APPEND_TO);            
                }
                break;
                       
            default:
                {   
                return KErrCorrupt;
                }
            }
        }
    
    if( iPath == VG_INVALID_HANDLE )
        {
        // get the symbian error code
        error = OpenVGErrorToSymbianError(vgGetError());
        
        if (error == KErrNoMemory)
            {
            NVG_DEBUGP1("NVG Error OOM");
            ResetNvgState();
            }
        return error;
        }
    
    iLastPathDataType   = aPathDataType;

    return error;
    }

TInt CNvgEngine::DrawCommandSectionL(TDereferencer * aIconData, const TSize& aTargetSize, CFbsBitmap * /*aDstBitmap*/, CFbsBitmap * /*aMask*/)
    {
    TInt16 lHeaderSize  = aIconData->DerefInt16L(KNVG_HEADERSIZE_OFS);
    TInt8 NVGVersion    = aIconData->DerefInt8L(KNVG_VERSION_OFS);
    
    User::LeaveIfError(InitializeGC());

    TInt16 lPathDataType = aIconData->DerefInt16L(KNVG_PATHDATATYPE_OFS);
    TReal32 lScale       = aIconData->DerefReal32L(KNVG_SCALE_OFS);
    TReal32 lBias        = aIconData->DerefReal32L(KNVG_BIAS_OFS);

    User::LeaveIfError(CreatePathHandle(lPathDataType, lScale, lBias));
    
    vgSetPaint(iPaintFill, VG_FILL_PATH);
    
    COND_COM_OC_OOC(
    if (iInternal->iCreatingNVGIcon)
        {
        iInternal->iCurrentNVGIcon = CNVGCSIcon::NewL(aIconData->GetPtr());
        });
    
    VGfloat lCurrentPathMatrix[9];
    vgGetMatrix(lCurrentPathMatrix);
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);  
    
    /*
     * We use the caller's base batrix regardless of which mode the caller was.
     * The caller may have set the matrix in VG_MATRIX_IMAGE_USER_TO_SURFACE mode
     * as it thinks it is drawing images (textures).
     * But even though the texture gets stroked instead, we must use the caller's
     * transformation matrix.
     * Everything gets restored to the original values before we return.
     */
    vgLoadMatrix(lCurrentPathMatrix);
    
    ApplyScissoring(lCurrentPathMatrix, aTargetSize);

    /*
     * set the rotation angle if available
     */
    if (iRotateApplied)
        {           
        ApplyRotation();
        }
    
#ifdef __MIRROR_    
    vgScale(1.0f, -1.0f);
    vgTranslate(0, (VGfloat)(-iCurrentBufferSize.iHeight) );
#endif
        
    TReal32 lViewboxX   = aIconData->DerefReal32L(KNVG_VIEWBOX_X_OFS);
    TReal32 lViewboxY   = aIconData->DerefReal32L(KNVG_VIEWBOX_Y_OFS);
    TReal32 lViewboxW   = aIconData->DerefReal32L(KNVG_VIEWBOX_WIDTH_OFS);
    TReal32 lViewboxH   = aIconData->DerefReal32L(KNVG_VIEWBOX_HEIGHT_OFS);
     
    ApplyViewboxToViewPortTransformationL(aTargetSize, lViewboxX, lViewboxY, lViewboxW, lViewboxH);
    
    TPtr8 ptr = aIconData->GetPtr();
    
    TInt offsetSectionLength = aIconData->GetLength() - lHeaderSize;
    TUint8 * offsetPtr = aIconData->DerefInt8ArrayL(offsetSectionLength, lHeaderSize);
    
    TDereferencer offsetSection(offsetPtr, offsetSectionLength);

    TUint16 lOffsetVectorCount  = offsetSection.DerefInt16L();

    offsetPtr = aIconData->DerefInt8ArrayL(offsetSectionLength - sizeof(TUint16), lHeaderSize + sizeof(TUint16));
    TDereferencer offsetVector(offsetPtr, offsetSectionLength - sizeof(TUint16));

    idoFill     = VG_FALSE;
    idoStroke   = VG_FALSE;
    
    TPtr8 offsetTPtr = offsetVector.GetPtr();
    
    TInt commandSectionOffset = lOffsetVectorCount * sizeof(TUint16);
    TDereferencer commandSection((unsigned char*)(offsetTPtr.Ptr() + commandSectionOffset), 
            aIconData->GetLength() - commandSectionOffset - lHeaderSize - sizeof(TUint16));

    // from version 2 onwards command section will start on word boundary
    if (NVGVersion >= KVersion2 && ((lOffsetVectorCount  & 0x01) == 0))
        {
        commandSection.SkipL(2);
        }
    
    TUint16 lCmdCount = commandSection.DerefInt16L();
    
    commandSection.SkipL(KNVG_COMMANDSECTION_OFS);

    /*
     * from version 2 onwards there will be a padding added
     * after the command count to make it word aligned
     */
    if (NVGVersion >= KVersion2)
        {
        commandSection.SkipL(2);
        }

    ExecuteNVGCSCommandLoopL(lCmdCount, aIconData, &offsetVector, &commandSection, NVGVersion);
    
    return KErrNone;
    }

void CNvgEngine::ApplyRotation()
    {
    vgTranslate(iCentreX, iCentreY);  
    vgRotate(iRotateAngle);
    vgTranslate(-iCentreX, -iCentreY);
    }

void CNvgEngine::ApplyScissoring(VGfloat aMatrix[], const TSize& aTargetSize)
    {
    /*
     * calculate the rectangle with respect to the transformation applied
     * and set the scissoring rect
     */ 
    TPoint leftBottom  = GetTranslatedPoint(aMatrix, TPoint(0, 0));
    TPoint leftTop     = GetTranslatedPoint(aMatrix, TPoint(0, aTargetSize.iHeight));
    TPoint rightBottom = GetTranslatedPoint(aMatrix, TPoint(aTargetSize.iWidth, 0));
    TPoint rightTop    = GetTranslatedPoint(aMatrix, TPoint(aTargetSize.iWidth,aTargetSize.iHeight));

    VGfloat minX = leftBottom.iX;
    VGfloat minY = leftBottom.iY;
    VGfloat maxX = leftBottom.iX;
    VGfloat maxY = leftBottom.iY;

    minX = MinVal4(leftBottom.iX, leftTop.iX, rightBottom.iX, rightTop.iX);
    minY = MinVal4(leftBottom.iY, leftTop.iY, rightBottom.iY, rightTop.iY);

    maxX = MaxVal4(leftBottom.iX, leftTop.iX, rightBottom.iX, rightTop.iX);
    maxY = MaxVal4(leftBottom.iY, leftTop.iY, rightBottom.iY, rightTop.iY);

    VGfloat newW = maxX - minX;
    VGfloat newH = maxY - minY;

    VGint clipRect[] = {minX, minY, newW, newH};

    vgSeti(VG_SCISSORING, VG_TRUE);
    vgSetiv(VG_SCISSOR_RECTS, 4,(const TInt32*) clipRect);
    }

void CNvgEngine::ApplyViewboxToViewPortTransformationL(const TSize& aTargetSize, TReal32 aViewboxX, TReal32 aViewboxY, TReal32 aViewboxW, TReal32 aViewboxH)
    {
    CNvgFitToViewBoxImpl * viewboxTrnsfr =  CNvgFitToViewBoxImpl::NewL();
    CleanupStack::PushL(viewboxTrnsfr);
    /*
     * this is bit unreadable,
     * need to find a better design to separate the object caching solution from normal rendering,
     * but anyway I guess its better than those ifdef's, in that code scanner won't give any warnings
     */
    COND_COM_OC_NOC(
    {
    if (iInternal->iCreatingNVGIcon)
        {
        CSICON(iInternal->iCurrentNVGIcon)->SetViewBox(aViewboxX, aViewboxY, aViewboxW, aViewboxH);
        CSICON(iInternal->iCurrentNVGIcon)->SetPreserveAspectRatio(iPreserveAspectSetting, iSmilFitSetting);
        CSICON(iInternal->iCurrentNVGIcon)->Rotate(iRotateAngle, iCentreX, iCentreY);
        }
    else
        {
        viewboxTrnsfr->SetAlign(iPreserveAspectSetting);
        viewboxTrnsfr->SetScaling(iSmilFitSetting);
        
        if (aViewboxW > 0 && aViewboxH > 0)
            {
            viewboxTrnsfr->SetViewBox(aViewboxX, aViewboxY, aViewboxW, aViewboxH);
            }
        
        TInt lWidth = aTargetSize.iWidth;
        TInt lHeight = aTargetSize.iHeight;
        
        viewboxTrnsfr->SetWindowViewportTrans(TRect(0, 0, lWidth, lHeight), TSize(0, 0));
        
        }
    },
    {
    viewboxTrnsfr->SetAlign(iPreserveAspectSetting);
    viewboxTrnsfr->SetScaling(iSmilFitSetting);
    
    if (aViewboxW > 0 && aViewboxH > 0)
        {
        viewboxTrnsfr->SetViewBox(aViewboxX, aViewboxY, aViewboxW, aViewboxH);
        }
    
    TInt lWidth = aTargetSize.iWidth;
    TInt lHeight = aTargetSize.iHeight;
    
    viewboxTrnsfr->SetWindowViewportTrans(TRect(0, 0, lWidth, lHeight), TSize(0, 0));
    });
    
    CleanupStack::PopAndDestroy(viewboxTrnsfr);
    }

TDereferencer GetCommandSectionL(TUint16 aOffset, TDereferencer * aIconData, TInt aNVGVersion)
    {
    // the max length that the command section can have
    TInt commandSectionLength = aIconData->GetLength() - aOffset;

    if (commandSectionLength <= 0)
        {
        User::Leave(KErrCorrupt);
        }

    TDereferencer section(aIconData->DerefInt8ArrayL(commandSectionLength, aOffset), commandSectionLength);

    /*
     * all the section are expected to be word aligned
     * all of the nvg-cs icon will be version 2 or above
     * the else won't be there as nvg version will always be greater than 2
     */
    if (aNVGVersion >= KVersion2)
        {
        if (!IsAligned4(aOffset))
            {
            User::Leave(KErrCorrupt);
            }
        }
    else
        {
        /*
         * no need to do anything here as once the nvgdecoder release
         * its version will be always greater than 2
         * infact the check for version will be removed
         */ 
        }
    
    return section;
    }

void CNvgEngine::ExecuteNVGCSCommandLoopL(TUint16 aCommandCount, TDereferencer * aIconData, TDereferencer * aOffsetVector,
                                          TDereferencer * aCommandSection, TInt aNVGVersion)
    {
    TUint32 transVal;

    VGfloat lCurrentPathMatrix[9];
    
    vgGetMatrix(lCurrentPathMatrix);
    
    TInt32 lOffsetIx = 0;
    for (TInt i=0; i < aCommandCount; i++)
        {
        TUint32 currentCommand = aCommandSection->DerefInt32L();
        lOffsetIx = currentCommand & 0x0000ffff;
                
        switch (currentCommand & 0xff000000)
            {
            case KCMD_SET_FILL_PAINT:
                {
                iFillAlpha        = (currentCommand & 0x00ff0000) >> 16;
                TUint16 offset    = aOffsetVector->DerefInt16L(lOffsetIx * sizeof(TUint16));
                
                TDereferencer section = GetCommandSectionL(offset, aIconData, aNVGVersion);
                                
                SetFillPaintL(&section);
                
                break;
                }
                
            case KCMD_SET_COLOR_RAMP:
                {
                TUint16 offset = aOffsetVector->DerefInt16L(lOffsetIx * sizeof(TUint16));

                TDereferencer section = GetCommandSectionL(offset, aIconData, aNVGVersion);

                SetColorRampL(&section);
                
                break;
                }
                
            case KCMD_DRAW_PATH:
                {
                if ((currentCommand & 0x00010000)) 
                    {
                    idoStroke = VG_TRUE;
                    }
                
                if ((currentCommand & 0x00020000))
                    {
                    idoFill = VG_TRUE;
                    }
                
                if (idoStroke != VG_FALSE || idoFill != VG_FALSE)
                    {
                    TUint16 offset        = aOffsetVector->DerefInt16L(lOffsetIx * sizeof(TUint16));    
                    TDereferencer section = GetCommandSectionL(offset, aIconData, aNVGVersion);
    
                    DrawPathL(&section);
                    }
                
                break;
                }
                
            case KCMD_SET_TRANSFORMATION:
                {
                SetTransformL(aCommandSection, transVal, lCurrentPathMatrix);
                aCommandSection->SkipL(transVal * sizeof(TUint32));
                break;
                }
                
            case KCMD_SET_STROKE_PAINT:
                {
                iStrokeAlpha = (currentCommand & 0x00ff0000) >> 16;
                TUint16 offset = aOffsetVector->DerefInt16L(lOffsetIx * sizeof(TUint16));

                TDereferencer section = GetCommandSectionL(offset, aIconData, aNVGVersion);

                SetStrokePaintL(&section);
                
                break;
                }

            case KCMD_SET_STROKE_WIDTH:
                {
                TReal32 lStrokeWidth;
                aCommandSection->SkipL(sizeof(TUint32));
                
                /*
                 * check for alignment and copy data if not aligned, else directly convert
                 * version 2 or above guarantees that is always word aligned
                 */
                TUint8 * cptr = aCommandSection->DerefInt8ArrayL(sizeof(TReal32), 0); 
                if (aNVGVersion < KVersion2 && !IsAligned4(cptr))
                    {
                    
                    Mem::Copy(reinterpret_cast<void *>(&lStrokeWidth),
                              reinterpret_cast<void *>(cptr), sizeof(lStrokeWidth));
                    }
                else
                    {
                    lStrokeWidth = aCommandSection->DerefReal32L();
                    }
                
                COND_COM_OC(iInternal->iCreatingNVGIcon,
                        CSICON(iInternal->iCurrentNVGIcon)->AddSetStrokeWidthCommandL(lStrokeWidth),
                        vgSetf(VG_STROKE_LINE_WIDTH, lStrokeWidth));
                break;
                }
                
            case KCMD_SET_STROKE_MITER_LIMIT:
                {
                TReal32 lMiterLimit;
                aCommandSection->SkipL(sizeof(TUint32));

                /*
                 * check for alignment and copy data if not aligned, else directly convert
                 * version 2 or above guarantees that is always word aligned
                 */
                TUint8 * cptr = aCommandSection->DerefInt8ArrayL(sizeof(TReal32), 0); 

                if (aNVGVersion < KVersion2 && !IsAligned4(cptr))
                    {
                    Mem::Copy(reinterpret_cast<void *>(&lMiterLimit),
                              reinterpret_cast<void *>(cptr), sizeof(lMiterLimit));
                    }
                else
                    {
                    lMiterLimit = aCommandSection->DerefReal32L();
                    }
                
                COND_COM_OC(iInternal->iCreatingNVGIcon,
                        CSICON(iInternal->iCurrentNVGIcon)->AddSetStrokeMiterLimitCommandL(lMiterLimit),
                        vgSetf(VG_STROKE_MITER_LIMIT, lMiterLimit));
                
                break;
                }
                
            case KCMD_SET_STROKE_LINE_JOIN_CAP:
                {
                TUint8 lJoinType =(currentCommand & 0x0000ff00)>>8;
                TUint8 lCapType = (currentCommand & 0x000000ff);
                
                VGCapStyle capStyle;
                switch(lCapType)
                    {
                    case KCAP_SQUARE:
                        capStyle = VG_CAP_SQUARE;
                        break;
                    case KCAP_ROUND:
                        capStyle = VG_CAP_ROUND;
                        break;
                    case KCAP_BUTT:
                    default:
                        capStyle = VG_CAP_BUTT;
                        break;
                    }
   
                VGJoinStyle lineJoinStyle;
                switch(lJoinType)
                    {
                    case KLINE_JOIN_BEVEL:
                        lineJoinStyle = VG_JOIN_BEVEL;
                        break;
                    case KLINE_JOIN_ROUND:
                        lineJoinStyle = VG_JOIN_ROUND;
                        break;
                    case KLINE_JOIN_MITER:
                    default:
                        lineJoinStyle = VG_JOIN_MITER;
                        break;
                    }
                
                COND_COM_OC(iInternal->iCreatingNVGIcon,
                        CSICON(iInternal->iCurrentNVGIcon)->AddStrokeLineJoinCapCommandL(capStyle, lineJoinStyle),
                        vgSeti(VG_STROKE_CAP_STYLE, capStyle);
                        vgSeti(VG_STROKE_JOIN_STYLE, lineJoinStyle););
                break;
                }
                
            default:
                {
                User::Leave(KErrCorrupt);
                break;
                }
            }
        
        // go to the next command
        aCommandSection->SkipL(sizeof(TUint32));
        }
    }

EXPORT_C void CNvgEngine::SetPreserveAspectRatio(
        TNvgAlignStatusType aPreserveAspectSetting, 
        TNvgMeetOrSliceType aSmilFitSetting)
    {
    iPreserveAspectSetting = aPreserveAspectSetting;
    iSmilFitSetting = aSmilFitSetting;
    }

EXPORT_C void CNvgEngine::SetBackgroundColor(TUint32 aRGBA8888Color)
    {
    iBackgroundColor = aRGBA8888Color;
    }

TInt CNvgEngine::SetFillPaintL(TDereferencer * aIconData)
    {
    COND_COM_OC_OOC(register int drawingMode = iInternal->iCreatingNVGIcon);
    
    TUint32 lCommonData  = aIconData->DerefInt32L();
    TUint lPaintType     = lCommonData & 0x07;
    TUint16 lSpecifcData = (lCommonData >> 16) & 0xff;
    
    switch (lPaintType)
        {
        case KPAINT_LGRAD:
            {
            iGradPaintFill = iPaintFill;
            
            COND_COM_OC_OOC(
            if (iInternal->iCreatingNVGIcon)
                {
                // CNVGCSIcon will destroy the paint handle
                iGradPaintFill = vgCreatePaint();
                if (iGradPaintFill == VG_INVALID_HANDLE)
                    {
                    User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
                    }
                });

            // gradient data, the data will be word aligned
            TReal32* lGradData = (TReal32*)aIconData->DerefInt8ArrayL(4 * sizeof(VGfloat), sizeof(TUint32));
            
            vgSetParameteri(iGradPaintFill, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT);
            vgSetParameterfv(iGradPaintFill, VG_PAINT_LINEAR_GRADIENT, 4, lGradData);
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
            
            if (lSpecifcData & 0x1)
                {
                TReal32* lGradMatrix1 = (TReal32*) aIconData->DerefInt8ArrayL(6 * sizeof (VGfloat),
                                                    KNVG_PAINTSECTION_LINEARGRAD_TRANSFORM_OFFSET);
                
                TReal32 lGradMatrix[9] = {lGradMatrix1[0], lGradMatrix1[3], 0.0f,  
                        lGradMatrix1[1], lGradMatrix1[4], 0.0f,
                        lGradMatrix1[2], lGradMatrix1[5], 1.0f};
                
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddLinearGradientCommandL(4, lGradData, lGradMatrix, iGradPaintFill),
                        vgLoadMatrix(lGradMatrix););
                }
            else
                {
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddLinearGradientCommandL(4, lGradData, (VGfloat*)KIdentityMatrix, iGradPaintFill),
                        vgLoadIdentity());
                }
            
            COND_COM_OC(drawingMode, ;,
                     vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE));
            
            break;
            }
            
        case KPAINT_RGRAD:
            {
            // gradient data, the data will be word aligned
            TReal32* lGradData = (TReal32*)aIconData->DerefInt8ArrayL(5 * sizeof(VGfloat), sizeof(TUint32));
            iGradPaintFill = iPaintFill;
            
            COND_COM_OC_OOC(
            if (iInternal->iCreatingNVGIcon)
                {
                iGradPaintFill = vgCreatePaint();
                if (iGradPaintFill == VG_INVALID_HANDLE)
                    {
                    User::LeaveIfError(CNvgEngine::OpenVGErrorToSymbianError(vgGetError()));
                    }                
                });

            vgSetParameteri(iGradPaintFill, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT);
            vgSetParameterfv(iGradPaintFill, VG_PAINT_RADIAL_GRADIENT, 5, lGradData);
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);


            if (lSpecifcData & 0x1)
                {
                TReal32* lGradMatrix1 = (TReal32*)aIconData->DerefInt8ArrayL(6 * sizeof (VGfloat),
                                                   KNVG_PAINTSECTION_RADIALGRAD_TRANSFORM_OFFSET);
                
                TReal32 lGradMatrix[9] = {lGradMatrix1[0], lGradMatrix1[3], 0.0f,  
                        lGradMatrix1[1], lGradMatrix1[4], 0.0f,
                        lGradMatrix1[2], lGradMatrix1[5], 1.0f};
                
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddRadialGradientCommandL(5, lGradData, lGradMatrix, iGradPaintFill),
                        vgLoadMatrix(lGradMatrix));
                }
            else
                {
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddRadialGradientCommandL(5, lGradData, (VGfloat*)KIdentityMatrix, iGradPaintFill),
                        vgLoadIdentity());
                }
            
            COND_COM_OC(drawingMode, ;,
                    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE));
            break;
            }
            
        case KPAINT_FLAT:
            {
            TUint32 lRgba = aIconData->DerefInt32L(KNVG_RGBA_OFS);

            lRgba = (lRgba & 0xffffff00) | iFillAlpha;
            
            COND_COM_OC(drawingMode,
                    CSICON(iInternal->iCurrentNVGIcon)->AddSetColorCommandL(lRgba),
                    vgSetParameteri(iPaintFill, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
                    vgSetColor(iPaintFill, lRgba));            
            break;
            }
            
        default:
            {
            User::Leave(KErrCorrupt);
            break;
            }
        }
    return KErrNone;
    }

TInt CNvgEngine::SetColorRampL(TDereferencer * aIconData)
    {
    TUint32 lCommonData = aIconData->DerefInt32L();
    
    TInt lStopCount = (lCommonData >> 16) & 0x00ff;
    TReal32* lStopData = (TReal32*)aIconData->DerefInt8ArrayL(lStopCount * 5 * sizeof(TReal32), sizeof(TUint32));
    
    VGfloat * colorRamps = new (ELeave) VGfloat[lStopCount * 5];
    CleanupStack::PushL(TCleanupItem(CleanupArray, colorRamps));
        
    if (iFillAlpha == 0xff)
        {
        vgSetParameteri(iGradPaintFill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
        vgSetParameterfv(iGradPaintFill, VG_PAINT_COLOR_RAMP_STOPS, lStopCount * 5, lStopData);
        }
    else
        {
        // Copy color ramps and modify alpha
        memcpy(colorRamps, lStopData, lStopCount * 5 * sizeof(VGfloat));
        
        VGfloat lAlphaInFloat = iFillAlpha * (1.0f/255.0f);
        VGfloat* lAlphaValue  = &colorRamps[4];
        
        for (int i=0; i<lStopCount; i++)
            {
            *lAlphaValue *= lAlphaInFloat;
            lAlphaValue += 5;
            }
        
        vgSetParameteri(iGradPaintFill, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
        vgSetParameterfv(iGradPaintFill, VG_PAINT_COLOR_RAMP_STOPS, lStopCount * 5, colorRamps);       
        }
    
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

void CNvgEngine::SetStrokePaintL(TDereferencer * aIconData)
    {
    COND_COM_OC_OOC(register int drawingMode = iInternal->iCreatingNVGIcon;);
    
    TUint32 lCommonData  = aIconData->DerefInt32L();
    TUint lStrokeType    = lCommonData & 0x07;
    TUint16 lSpecifcData = (lCommonData >> 16) & 0xff;
        
    switch (lStrokeType)
        {
        case KSTROKE_LGRAD:
            {
            iGradPaintStroke = iPaintStroke;
            
            COND_COM_OC_OOC(
            if (iInternal->iCreatingNVGIcon)
                {
                iGradPaintStroke = vgCreatePaint();
                User::LeaveIfNull((TAny *)iGradPaintStroke);
                });

            // gradient data, the data will be word aligned
            TReal32* lGradData = (TReal32*)aIconData->DerefInt8ArrayL(4 * sizeof(VGfloat), sizeof(TUint32));
            
            COND_COM_OC(drawingMode, ;,
                    vgSetParameteri( iGradPaintStroke, VG_PAINT_TYPE, VG_PAINT_TYPE_LINEAR_GRADIENT );
                    vgSetParameterfv( iGradPaintStroke, VG_PAINT_LINEAR_GRADIENT, 4, lGradData);                
                    vgSeti(VG_MATRIX_MODE, VG_MATRIX_STROKE_PAINT_TO_USER));
                        
            if (lSpecifcData & 0x1)
                {
                TReal32* lGradMatrix1 = (TReal32*)aIconData->DerefInt8ArrayL(6 * sizeof(VGfloat),
                                                                             4 + 4 * sizeof(VGfloat));
                TReal32 lGradMatrix[9] = {lGradMatrix1[0], lGradMatrix1[3], 0.0f,  
                        lGradMatrix1[1], lGradMatrix1[4], 0.0f,
                        lGradMatrix1[2], lGradMatrix1[5], 1.0f};
                
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddStrokeLinearGradientCommandL(4, lGradData, lGradMatrix, iGradPaintStroke),
                        vgLoadMatrix(lGradMatrix));                
                }
            else
                {
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddStrokeLinearGradientCommandL(4, lGradData, (VGfloat*)KIdentityMatrix, iGradPaintStroke),
                        vgLoadIdentity());                
                }
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
            break;
            }

        case KSTROKE_RGRAD:
            {
            iGradPaintStroke = iPaintStroke;
            
            COND_COM_OC_OOC(
            if (iInternal->iCreatingNVGIcon)
                {
                iGradPaintStroke = vgCreatePaint();
                User::LeaveIfNull((TAny *)iGradPaintStroke);
                });
            
            // gradient data, the data will be word aligned
            TReal32* lGradData = (TReal32*)aIconData->DerefInt8ArrayL(5 * sizeof(VGfloat), sizeof(TUint32));
            
            COND_COM_OC(drawingMode, ;,
                     vgSetParameteri( iGradPaintStroke, VG_PAINT_TYPE, VG_PAINT_TYPE_RADIAL_GRADIENT );
                     vgSetParameterfv( iGradPaintStroke, VG_PAINT_RADIAL_GRADIENT, 5, lGradData);            
                     vgSeti(VG_MATRIX_MODE, VG_MATRIX_STROKE_PAINT_TO_USER));
            
            if (lSpecifcData & 0x1)
                {
                TReal32* lGradMatrix1 = (TReal32*)aIconData->DerefInt8ArrayL(6 * sizeof(VGfloat),
                                                                             4 + 5 * sizeof(VGfloat));
                TReal32 lGradMatrix[9] = {lGradMatrix1[0], lGradMatrix1[3], 0.0f,  
                        lGradMatrix1[1], lGradMatrix1[4], 0.0f,
                        lGradMatrix1[2], lGradMatrix1[5], 1.0f};
                
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddStrokeRadialGradientCommandL(4, lGradData, lGradMatrix, iGradPaintStroke),
                        vgLoadMatrix(lGradMatrix));                
                }
            else
                {
                COND_COM_OC(drawingMode,
                        CSICON(iInternal->iCurrentNVGIcon)->AddStrokeRadialGradientCommandL(4, lGradData, (VGfloat*)KIdentityMatrix, iGradPaintStroke),
                        vgLoadIdentity());
                }
            vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
            break;
            }
            
        case KSTROKE_COLOR_RAMP:
            {
            TInt lStopCount = lSpecifcData;
            TReal32* lStopData = (TReal32*) aIconData->DerefInt8ArrayL(lStopCount * 5 * sizeof(VGfloat), 4);
            
            if (iStrokeAlpha == 0xff)
                {
                 vgSetParameteri(iGradPaintStroke, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
                 vgSetParameterfv(iGradPaintStroke, VG_PAINT_COLOR_RAMP_STOPS, lStopCount*5, lStopData);                
                }
            else
                {
                VGfloat * colorRamps = new (ELeave) VGfloat[lStopCount * 5];
                CleanupStack::PushL(TCleanupItem(CleanupArray, colorRamps));

                memcpy(colorRamps, lStopData, lStopCount * 5 * sizeof(VGfloat));
                
                VGfloat lAlphaInFloat = iStrokeAlpha * (1.0f/255.0f);
                VGfloat* lAlphaValue  = &colorRamps[4];
                
                for (int i=0; i<lStopCount; i++)
                    {
                    *lAlphaValue *= lAlphaInFloat;
                    lAlphaValue += 5;
                    }
                
                vgSetParameteri(iGradPaintStroke, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
                vgSetParameterfv(iGradPaintStroke, VG_PAINT_COLOR_RAMP_STOPS, lStopCount * 5, colorRamps);
                CleanupStack::PopAndDestroy();
                }
            break;
            } 
            
        default:
            {
            TUint32 lRgba = aIconData->DerefInt32L(KNVG_RGBA_OFS);
            lRgba = (lRgba & 0xffffff00)|iStrokeAlpha; // replace alpha
            
            COND_COM_OC(drawingMode,
                    CSICON(iInternal->iCurrentNVGIcon)->AddStrokeSetColorCommandL(lRgba),
                    vgSetParameteri( iPaintStroke, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );
                    vgSetColor(iPaintStroke, lRgba));            
            break;
            }
        }
    }

void CNvgEngine::DrawPathL(TDereferencer * aIconData)
    {
    TInt numSegments = aIconData->DerefInt16L();
    const VGubyte * pathSegments  = aIconData->DerefInt8ArrayL(numSegments, sizeof(TUint16));
    
    /*
     * verify that what we got is proper data
     * for that calculate the path co-ordinate length
     * and check that the path data does not overflow
     */    
    TInt coordinateCount = 0;
    for (TInt i = 0; i < numSegments; ++i)
        {
        switch (pathSegments[i])
            {
            case VG_HLINE_TO:
            case VG_VLINE_TO:
                coordinateCount += 1;
                break;
            case VG_MOVE_TO:
            case VG_LINE_TO:
            case VG_SQUAD_TO:
                coordinateCount += 2;
                break;                
            case VG_QUAD_TO:
            case VG_SCUBIC_TO:
                coordinateCount += 4;
                break;
            case VG_SCCWARC_TO:
            case VG_SCWARC_TO:
            case VG_LCCWARC_TO:
            case VG_LCWARC_TO:
                coordinateCount += 5;
                break;
            case VG_CUBIC_TO:
                coordinateCount += 6;
                break;
            default:
                break;
            }
        }

    // this one is just to check the alignment
    TUint8* pathData = aIconData->DerefInt8ArrayL(sizeof(TUint32), sizeof(TUint16) + numSegments);
    
    /*
     * path data need to be word aligned
     * alignment are done according to the path format
     */
    TUint sizeofpathdata = sizeof(TUint32);
    TUint alignSkip = 0;
    TUint8 * alignedPtr = 0;
    if (iLastPathDataType == ESixteenBitEncoding)
        {
        alignedPtr = Align2(pathData);
        sizeofpathdata = sizeof(TUint16);
        }
    else if (iLastPathDataType == EThirtyTwoBitEncoding)
        {        
        alignedPtr = Align4(pathData);
        }
    else
        {
        User::Leave(KErrCorrupt); // no other path data type is supported
        }
    
    alignSkip = alignedPtr - pathData; 
    
    /*
     * check to see whether we have enough path data
     */
    aIconData->IsSafeL(coordinateCount * sizeofpathdata + alignSkip, sizeof(TUint16) + numSegments);
    
    pathData = alignedPtr;
    
    VGint paintMode = (idoFill ? VG_FILL_PATH : 0)|(idoStroke ? VG_STROKE_PATH : 0);
    if (paintMode == 0)
        {
        paintMode = VG_FILL_PATH;
        }
    
    COND_COM_OC(iInternal->iCreatingNVGIcon,
            {
                VGPath path = CreatePath();
                
                if (path != VG_INVALID_HANDLE)
                    {
                    vgAppendPathData(path, numSegments, pathSegments, pathData);
                    }
                else
                    {
                    CSICON(iInternal->iCurrentNVGIcon)->AddPathDataL(numSegments, pathSegments, pathData);
                    }        
                CSICON(iInternal->iCurrentNVGIcon)->AddDrawPathCommandL(path, paintMode);
            }, 
            {
                vgClearPath(iPath, VG_PATH_CAPABILITY_APPEND_TO);
                
                vgAppendPathData(iPath, numSegments, pathSegments, pathData);
                vgDrawPath(iPath, paintMode);
            });
    
    idoStroke   = VG_FALSE;
    idoFill     = VG_FALSE;
    }

#ifdef    OPENVG_OBJECT_CACHING
VGPath CNvgEngine::CreatePath()
    {
    VGPath path = VG_INVALID_HANDLE;
    switch (iLastPathDataType)
        {
        case EEightBitEncoding:
            {
            path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
            VG_PATH_DATATYPE_S_16, 1.0f/2.0f, 0.0f, 0, 0,
            VG_PATH_CAPABILITY_APPEND_TO);           
            }
            break;
            
        case ESixteenBitEncoding:
            {
            path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
            VG_PATH_DATATYPE_S_16, 1.0f/16.0f, 0.0f, 0, 0,
            VG_PATH_CAPABILITY_APPEND_TO);            
            }
            break;
           
        case EThirtyTwoBitEncoding:
            {
            path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
            VG_PATH_DATATYPE_S_32, 1.0f/65536.0f, 0.0f, 0, 0,
            VG_PATH_CAPABILITY_APPEND_TO);            
            }
            break;
    
        default:
            {
        
            }
           break;
           }
    return path;
    }
#endif

void CNvgEngine::SetTransformL(TDereferencer * aIconData, TUint32 & aCounter, const VGfloat* aCurrentMatrix)
    {
    
    COND_COM_OC(iInternal->iCreatingNVGIcon, ;,
             vgLoadMatrix(aCurrentMatrix));
    
    TUint32 lCommonData    = aIconData->DerefInt32L();
    TUint32 lTransformType = (lCommonData & 0x00ff0000)>>16 ;
    
    VGfloat matrixTemp[9] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f };   
    
    aCounter = 0;
    
    if (lTransformType != 1)
        {
        if (lTransformType == KTRANSFORM_COMPLETE)
            {
            matrixTemp[0] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            matrixTemp[4] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            matrixTemp[3] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            matrixTemp[1] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            matrixTemp[6] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            matrixTemp[7] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
            }
        else
            {
            if (lTransformType & KTRANSFORM_ROTATION)
                {
                //vgScale
                matrixTemp[0] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                matrixTemp[4] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                
                //vgShear
                matrixTemp[3] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                matrixTemp[1] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                }
            else
                {
                if (lTransformType & KTRANSFORM_SCALING)
                    {
                    //vgScale
                    matrixTemp[0] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                    matrixTemp[4] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                    }
                
                if (lTransformType & KTRANSFORM_SHEARING)
                    {
                    //vgShear
                    matrixTemp[3] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                    matrixTemp[1] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                    }
                }
            
            if (lTransformType & KTRANSFORM_TRANSLATION)
                {
                //vgTranslate
                matrixTemp[6] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                matrixTemp[7] = aIconData->DerefReal32L((++aCounter) * sizeof (VGfloat));
                }
            }
        
        COND_COM_OC(iInternal->iCreatingNVGIcon,
                CSICON(iInternal->iCurrentNVGIcon)->AddSetTransformCommandL(matrixTemp, 1),
                vgMultMatrix(matrixTemp));        
        }
    else
        {
        COND_COM_OC(iInternal->iCreatingNVGIcon,
                CSICON(iInternal->iCurrentNVGIcon)->AddSetTransformCommandL(matrixTemp, 0), ;);
        }
    }

void CNvgEngine::GenerateMask(CFbsBitmap* aMask)
    {
    if (!aMask || aMask->SizeInPixels() != iCurrentBufferSize)
        {
        return;
        }
    
    const TDisplayMode KMaskDisplayMode = aMask->DisplayMode();
    
    if (KMaskDisplayMode != EGray256 && KMaskDisplayMode != EGray2)
        {
        return;
        }
    
    const TInt KOriginalFilterMasks = vgGeti(VG_FILTER_CHANNEL_MASK);
    const TInt KStride = CFbsBitmap::ScanLineLength(iCurrentBufferSize.iWidth, KMaskDisplayMode);
    
    // Change to get alpha values from OpenVG
    vgSeti(VG_FILTER_CHANNEL_MASK, VG_ALPHA);
    
    VGImageFormat format = (KMaskDisplayMode == EGray256) ? VG_A_8 : VG_BW_1;
    
    aMask->LockHeap();

    /*
     * Get data address of last line and move upwards (negative stride)
     * OpenVG uses Cartesian coordinate system and Symbian uses Screen coordinate system.
     */
    TUint* data = (TUint*)((TUint)aMask->DataAddress() + (KStride * (iCurrentBufferSize.iHeight - 1 )));
    
    vgReadPixels(data, -KStride, format, 0, 0,
            iCurrentBufferSize.iWidth, iCurrentBufferSize.iHeight);
    aMask->UnlockHeap();
    
    // Set back the original filter-masks
    vgSeti(VG_FILTER_CHANNEL_MASK, KOriginalFilterMasks);
    }

void CNvgEngine::ClearBackground()
    {
    TUint32 rgba = iBackgroundColor;//(iBackgroundColor << 8) | (iBackgroundColor >> 24);
    TInt32 r, g, b, a;
    r = (TInt)((rgba & 0xFF000000) >> 24);
    g = (TInt)((rgba & 0x00FF0000) >> 16);
    b = (TInt)((rgba & 0x0000FF00) >> 8);
    a = (TInt)(rgba & 0x000000FF);
    
    //r += r >> 7; g += g >> 7; b += b >> 7; a += a >> 7;
    
    const VGfloat KInverse255 =  1.0f/256.0f;
    const VGfloat clearColor[4] = { (KInverse255 * VGfloat (r)),
            (KInverse255 * VGfloat (g)),
            (KInverse255 * VGfloat (b)),
            (KInverse255 * VGfloat (a)) };
    
    //vgSeti(VG_SCISSORING, VG_FALSE);
    vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
    vgClear(0, 0, iCurrentBufferSize.iWidth, iCurrentBufferSize.iHeight);
    //vgSeti(VG_SCISSORING, VG_TRUE);
    }

TBool CNvgEngine::IsIdentity(VGfloat array[])
    {
    return ((array[0] == 1.0f && array[4] == 1.0f && array[8] == 1.0f)&& 
            (array[1] == 0.0f && array[2] == 0.0f && array[3] == 0.0f &&
             array[5] == 0.0f && array[6] == 0.0f && array[7] == 0.0f));
    }

TInt CNvgEngine::OpenVGErrorToSymbianError( TInt aError )
    {
    TInt error = KErrNone;
    switch (aError)
        {            
        case VGU_OUT_OF_MEMORY_ERROR:
        case VG_OUT_OF_MEMORY_ERROR:
            {
            error = KErrNoMemory;   
            break;
            }
            
        case VG_ILLEGAL_ARGUMENT_ERROR:
        case VGU_ILLEGAL_ARGUMENT_ERROR:
            {
            error = KErrArgument; 
            break;
            }
            
        case VG_UNSUPPORTED_PATH_FORMAT_ERROR:
        case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
            {
            error = KErrNotSupported; 
            break;
            }
            
        case VG_IMAGE_IN_USE_ERROR:
            {
            error = KErrInUse; 
            break;
            }
            
        case VG_BAD_HANDLE_ERROR:
        case VGU_BAD_HANDLE_ERROR:
            {
            error = KErrBadHandle;
            break;
            }
            
        case VG_PATH_CAPABILITY_ERROR:
        case VGU_PATH_CAPABILITY_ERROR:
        case VGU_BAD_WARP_ERROR:
            {
            error = KErrUnknown;
            break;
            }
            
        case VG_NO_CONTEXT_ERROR:
            {
            error = KErrNotReady;
            break;
            }
        
        default:
            {
            error = KErrUnknown; 
            }
        }
    
    return error;
    }

void CNvgEngine::UpdateClientMatrices()
    {
    iMatrixMode = vgGeti(VG_MATRIX_MODE);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgGetMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgGetMatrix(iImageMatrix);
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }

void CNvgEngine::RestoreClientMatrices()
    {
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgLoadMatrix(iImageMatrix);
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }   
    
TPoint CNvgEngine::GetTranslatedPoint(VGfloat aTRMatrix[9], TPoint aPoint)
    {
    TPoint trPoint;
    
    trPoint.iX = aTRMatrix[0] * aPoint.iX + aTRMatrix[3] * aPoint.iY + aTRMatrix[6];
    trPoint.iY = aTRMatrix[1] * aPoint.iX + aTRMatrix[4] * aPoint.iY + aTRMatrix[7];
    
    return trPoint;
    }

VGfloat CNvgEngine::MinVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 )
    {
    VGfloat min = x1;
    
    if (min > x2)
        {
        min = x2;
        }
    if (min > x3)
        {
        min = x3;
        }
    if (min > x4)
        {
        min = x4;
        }
    
    return min;
    }

VGfloat CNvgEngine::MaxVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4 )
    {
    VGfloat max = x1;
    
    if (max < x2)
        {
        max = x2;
        }
    if (max < x3)
        {
        max = x3;
        }
    if (max < x4)
        {
        max = x4;
        }
    
    return max;
    }

