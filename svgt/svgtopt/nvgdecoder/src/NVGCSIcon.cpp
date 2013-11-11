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

#include "NVGCSIcon.h"
#include "NVGIconData.h"
#include "nvgfittoviewbox.h"
#include "nvg.h"
#include "NVGUtil.h"
#include "OpenVGHandleStore.h"

#include <string.h>
#include <e32math.h>


CNVGCSIcon::CNVGCSIcon()
    :   iNVGIconData(0),
        iPath(VG_INVALID_HANDLE),
        iFillPaint(VG_INVALID_HANDLE),
        iStrokePaint(VG_INVALID_HANDLE),
        iLastFillPaintType(0),
        iLastStrokePaintType(0),
        iLastFillPaintColor(0),
        iLastStrkePaintColor(0),
        iResetFillPaint(0),
        iResetStrokePaint(0)
    {
    }

CNVGCSIcon::~CNVGCSIcon()
    {
    vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
    vgSetPaint(VG_INVALID_HANDLE, VG_STROKE_PATH);

    delete iNVGIconData;
    delete iOpenVGHandles;
    }

CNVGCSIcon * CNVGCSIcon::NewL(const TDesC8& aBuf)
    {
    CNVGCSIcon* self    = CNVGCSIcon::NewLC(aBuf);
    
    CleanupStack::Pop(self);
    return self;
    }
    
CNVGCSIcon * CNVGCSIcon::NewLC(const TDesC8& aBuf)
    {
    CNVGCSIcon* self    = new (ELeave) CNVGCSIcon;
    CleanupStack::PushL(self);

    self->ConstructL(aBuf);
    
    return self;
    }

void CNVGCSIcon::ConstructL(const TDesC8& aBuf)
    {
    iNVGIconData = CNVGIconData::NewL(aBuf.Length());
    iOpenVGHandles = COpenVGHandleStore::NewL();
    
    if (iPath == VG_INVALID_HANDLE)
        {
        iPath = vgCreatePath(VG_PATH_FORMAT_STANDARD,
                        VG_PATH_DATATYPE_S_32, 1.0f/65536.0f, 0.0f, 0, 0,
                        VG_PATH_CAPABILITY_APPEND_TO);
        iOpenVGHandles->AddPathDHL(iPath);
        }
    
    if (iFillPaint == VG_INVALID_HANDLE)
        {
        iFillPaint = vgCreatePaint();
        iOpenVGHandles->AddPaintDHL(iFillPaint);
        }
    vgSetPaint(iFillPaint, VG_FILL_PATH);
    
    if (iStrokePaint == VG_INVALID_HANDLE)
        {
        iStrokePaint = vgCreatePaint();
        iOpenVGHandles->AddPaintDHL(iStrokePaint);
        }
    vgSetPaint(iStrokePaint, VG_STROKE_PATH);
    }

TInt CNVGCSIcon::SetViewBox(TReal32 x, TReal32 y, TReal32 w, TReal32 h) __SOFTFP
    {
    iViewBoxX = x;
    iViewBoxY = y;
    iViewBoxW = w;
    iViewBoxH = h;
    
    return KErrNone;
    }

TInt CNVGCSIcon::SetPreserveAspectRatio(TInt aPreserveAspectSetting, 
        TInt aSmilFitSetting)
    {
    iPreserveAspectSetting = aPreserveAspectSetting;
    iSmilFitSetting = aSmilFitSetting;

    return KErrNone;
    }

TInt CNVGCSIcon::Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP
    {
    iRotationAngle  = aAngle;
    iRotationX      = aX;
    iRotationY      = aY;
    
    return KErrNone;
    }
    
void CNVGCSIcon::AddPathDataL(VGint numSegments, const VGubyte * pathSegments, const void * pathData)
    {
    iNVGIconData->EncodeInt32L(EPathData);
    iNVGIconData->EncodeInt32L(numSegments);
    iNVGIconData->EncodeDataL(pathSegments, numSegments);

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
    iNVGIconData->EncodeInt16L(coordinateCount);
    iNVGIconData->EncodeDataL(pathData, coordinateCount * 4);
    }

void CNVGCSIcon::AddDrawPathCommandL(VGPath aPath, VGbitfield aPaintMode)
    {
    iOpenVGHandles->AddPathDHL(aPath);
    iNVGIconData->EncodeInt32L(EPath);
    iNVGIconData->EncodeInt32L(aPath);    
    iNVGIconData->EncodeInt32L(aPaintMode);    
    }

void CNVGCSIcon::AddLinearGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint)
    {
    iOpenVGHandles->AddPaintDHL(aPaint);
    iNVGIconData->EncodeInt32L(EPaint);
    AddLinearGradientCommandDataL(aPaint, aCount, aGradientData, aGradientMatrix);
    }

void CNVGCSIcon::AddRadialGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint)
    {
    iOpenVGHandles->AddPaintDHL(aPaint);
    iNVGIconData->EncodeInt32L(EPaint);
    AddRadialGradientCommandDataL(aPaint, aCount, aGradientData, aGradientMatrix);
    }

void CNVGCSIcon::AddSetColorCommandL(VGuint aRgba)
    {
    iNVGIconData->EncodeInt32L(EPaint);
    iNVGIconData->EncodeInt32L(VG_PAINT_TYPE_COLOR);
    iNVGIconData->EncodeInt32L(aRgba);
    }

void CNVGCSIcon::AddColorRampCommandL(VGPaint aPaint)
    {
    iNVGIconData->EncodeInt32L(EColorRamp);
    iNVGIconData->EncodeInt32L(aPaint);
    }

void CNVGCSIcon::AddSetTransformCommandL(const VGfloat* aTransformMatrix, TInt aFlag)
    {
    iNVGIconData->EncodeInt32L(ETransform);
    iNVGIconData->EncodeDataL(aTransformMatrix, 9 * sizeof(VGfloat));
    iNVGIconData->EncodeInt32L(aFlag);
    }

void CNVGCSIcon::AddSetStrokeWidthCommandL(VGfloat aStrokeWidth)
    {
    iNVGIconData->EncodeInt32L(EStrokeWidth);
    iNVGIconData->EncodeReal32L(aStrokeWidth);
    }

void CNVGCSIcon::AddSetStrokeMiterLimitCommandL(VGfloat aMiterLimit)
    {
    iNVGIconData->EncodeInt32L(EStrokeMiterLimit);
    iNVGIconData->EncodeReal32L(aMiterLimit);
    }

void CNVGCSIcon::AddStrokeLineJoinCapCommandL(VGint aCapStyle, VGint aJoinStyle)
    {
    iNVGIconData->EncodeInt32L(EStrokeLineJoinCap);
    iNVGIconData->EncodeInt32L(aCapStyle);
    iNVGIconData->EncodeInt32L(aJoinStyle);
    }

void CNVGCSIcon::AddStrokeLinearGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint)
    {
    iOpenVGHandles->AddPaintDHL(aPaint);
    iNVGIconData->EncodeInt32L(EStrokePaint);
    AddLinearGradientCommandDataL(aPaint, aCount, aGradientData, aGradientMatrix);
    }

void CNVGCSIcon::AddStrokeRadialGradientCommandL(VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix, VGPaint aPaint)
    {
    iOpenVGHandles->AddPaintDHL(aPaint);
    iNVGIconData->EncodeInt32L(EStrokePaint);
    AddRadialGradientCommandDataL(aPaint, aCount, aGradientData, aGradientMatrix);
    }

void CNVGCSIcon::AddStrokeSetColorCommandL(VGuint aRgba)
    {
    iNVGIconData->EncodeInt32L(EStrokePaint);
    AddSetColorCommandDataL(aRgba);
    }

void CNVGCSIcon::AddStrokeColorRampCommandL(VGPaint aPaint)
    {
    iNVGIconData->EncodeInt32L(EStrokeColorRamp);
    iNVGIconData->EncodeInt32L(aPaint);
    }

void CNVGCSIcon::AddLinearGradientCommandDataL(VGPaint aPaint, VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix)
    {
    iNVGIconData->EncodeInt32L(VG_PAINT_TYPE_LINEAR_GRADIENT);
    iNVGIconData->EncodeInt32L(aPaint);
    iNVGIconData->EncodeInt32L(aCount);
    iNVGIconData->EncodeDataL(aGradientData, aCount * sizeof(VGfloat));
    iNVGIconData->EncodeDataL(aGradientMatrix, 9 * sizeof(VGfloat));    
    }

void CNVGCSIcon::AddRadialGradientCommandDataL(VGPaint aPaint, VGint aCount, VGfloat* aGradientData, VGfloat* aGradientMatrix)
    {
    iNVGIconData->EncodeInt32L(VG_PAINT_TYPE_RADIAL_GRADIENT);
    iNVGIconData->EncodeInt32L(aPaint);
    iNVGIconData->EncodeInt32L(aCount);
    iNVGIconData->EncodeDataL(aGradientData, aCount * sizeof(VGfloat));
    iNVGIconData->EncodeDataL(aGradientMatrix, 9 * sizeof(VGfloat));
    }

void CNVGCSIcon::AddSetColorCommandDataL(VGuint aRgba)
    {
    iNVGIconData->EncodeInt32L(VG_PAINT_TYPE_COLOR);
    iNVGIconData->EncodeInt32L(aRgba);
    }

TInt CNVGCSIcon::Draw(const TSize aSize, CNvgEngine * aNVGEngine)
    {
    NVG_DEBUGP2("DRAWING NVGCSIcon %s, ", __FUNCTION__);

    TInt error = KErrNone;
    
    iNVGEngine = aNVGEngine;
    
    // Get Matrix modes and all caller matrices (must be restored afterwards)
    UpdateClientMatrices();
    
    TRAP(error, DoDrawL(aSize));
    
    // restore everything as we may have changed matrix mode            
    RestoreClientMatrices();
    
    return error;
    }

TInt CNVGCSIcon::DoDrawL(const TSize aSize)
    {
    TInt ret = KErrNone;
    
    vgSetPaint(iFillPaint,   VG_FILL_PATH);
    vgSetPaint(iStrokePaint, VG_STROKE_PATH);
    iLastFillPaintColor     = 0;
    iLastStrkePaintColor    = 0;
    iLastFillPaintType      = 0;
    iLastStrokePaintType    = 0;

    VGfloat lCurrentPathMatrix[9];
    vgGetMatrix(lCurrentPathMatrix);
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);  
    vgLoadMatrix(lCurrentPathMatrix);
    SetRotation();
#ifdef __MIRROR_    
    vgScale(1.0f, -1.0f);
    vgTranslate(0, (VGfloat)(-aSize.iHeight) );
#endif
    
    SetViewBoxToViewTransformationL(aSize);
   
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    
    VGfloat currentMatrix[9];
    
    vgGetMatrix(currentMatrix);
    
    iNVGIconData->BeginRead();

    while (!iNVGIconData->EOF())
        {
        switch (iNVGIconData->ReadInt32L())
            {
            case EPath:
                {
                VGPath path = (VGPath)iNVGIconData->ReadInt32L();
                VGPaintMode paintMode = (VGPaintMode)iNVGIconData->ReadInt32L();

                if (path == VG_INVALID_HANDLE)
                    {
                    vgDrawPath(iPath, paintMode);
                    }
                else
                    {
                    vgDrawPath(path, paintMode);
                    }
                
                break;
                }
            case EPathData:
                {                
                if (iPath != VG_INVALID_HANDLE)
                    {
                    VGint numSegments;
                    VGubyte * pathSegments = 0;
                    VGubyte * pathData = 0;
                    
                    numSegments  = iNVGIconData->ReadInt32L();
                    pathSegments = new (ELeave) VGubyte[numSegments];
                    CleanupStack::PushL(TCleanupItem(CleanupArray, pathSegments));
                    if (pathSegments)
                        {
                        iNVGIconData->ReadL(pathSegments, numSegments);
                        VGint coordinateCount = iNVGIconData->ReadInt32L();
                        pathData = new (ELeave) VGubyte[coordinateCount * 4];
                        if (pathData)
                            {
                            CleanupStack::PushL(TCleanupItem(CleanupArray, pathData));
                            iNVGIconData->ReadL(pathData, coordinateCount * 4);
                            vgClearPath(iPath, VG_PATH_CAPABILITY_APPEND_TO);
                            vgAppendPathData(iPath, numSegments, pathSegments, pathData);
                            CleanupStack::PopAndDestroy();
                            }                        
                        }
                    CleanupStack::PopAndDestroy();
                    }
                break;
                }
            case EPaint:
                {
                DrawPaintL(iFillPaint, VG_MATRIX_FILL_PAINT_TO_USER, iLastFillPaintType, iLastFillPaintColor, VG_FILL_PATH);
                break;
                }
            case EColorRamp:
                {
                iNVGIconData->ReadInt32L();
                break;
                }
            case ETransform:
                {
                TInt flag;
                VGfloat transformMatrix[9];
                
                TPtr8 tmPtr((TUint8 *)transformMatrix, 9 * sizeof(VGfloat));
                
                iNVGIconData->ReadL(tmPtr, 9 * sizeof(VGfloat));
                flag = iNVGIconData->ReadInt32L();
                
                vgLoadMatrix(currentMatrix);
                if (flag)
                    {
                    vgMultMatrix(transformMatrix);
                    }
                }
                break;
            case EStrokeWidth:
                {
                VGfloat strokeWidth = iNVGIconData->ReadReal32L();
                vgSetf(VG_STROKE_LINE_WIDTH, strokeWidth);
                break;
                }
            case EStrokeMiterLimit:
                {
                VGfloat miterLimit = iNVGIconData->ReadReal32L();
                vgSetf(VG_STROKE_MITER_LIMIT, miterLimit);
                break;
                }
            case EStrokeLineJoinCap:
                {
                VGint lineJoin = iNVGIconData->ReadInt32L();
                VGint cap = iNVGIconData->ReadInt32L();
                
                vgSeti(VG_STROKE_JOIN_STYLE, (VGJoinStyle)lineJoin);
                vgSeti(VG_STROKE_CAP_STYLE, (VGCapStyle)cap);
                break;
                }
            case EStrokePaint:
                {
                DrawPaintL(iStrokePaint, VG_MATRIX_STROKE_PAINT_TO_USER, iLastStrokePaintType, iLastStrkePaintColor, VG_STROKE_PATH);
                break;
                }
            case EStrokeColorRamp:
                {
                iNVGIconData->ReadInt32L();
                break;
                }
            default:
                {
                User::Leave(KErrCorrupt);
                break;
                }
            }
        }
    
    iNVGIconData->EndRead();
    
    return ret;
    }

void CNVGCSIcon::DrawColorRampL(VGPaint aPaint)
    {
    TInt stopCount = iNVGIconData->ReadInt32L();
    VGfloat * colorRamps = new (ELeave) VGfloat[stopCount];
    CleanupStack::PushL(TCleanupItem(CleanupArray, colorRamps));

    iNVGIconData->ReadL((TUint8 *)colorRamps, stopCount * sizeof(VGfloat));
    vgSetParameteri(aPaint, VG_PAINT_COLOR_RAMP_SPREAD_MODE, VG_COLOR_RAMP_SPREAD_PAD);
    vgSetParameterfv(aPaint, VG_PAINT_COLOR_RAMP_STOPS, stopCount, colorRamps);                    
    
    CleanupStack::PopAndDestroy();
    }

void CNVGCSIcon::DrawPaintL(VGPaint aPaint, VGMatrixMode aMatrixMode, TUint & aLastPaintType, TUint & aLastPaintColor, VGPaintMode aPaintMode)
    {
    VGPaintType paintType = (VGPaintType)iNVGIconData->ReadInt32L();
    
    if (paintType == VG_PAINT_TYPE_LINEAR_GRADIENT ||
        paintType == VG_PAINT_TYPE_RADIAL_GRADIENT)
        {
        VGPaintParamType paintPType = VG_PAINT_LINEAR_GRADIENT;
        if (paintType == VG_PAINT_TYPE_RADIAL_GRADIENT)
            {
            paintPType = VG_PAINT_RADIAL_GRADIENT;
            }
        
        VGPaint paintHandle = iNVGIconData->ReadInt32L();
        TInt count = iNVGIconData->ReadInt32L();
        VGfloat gradientData[5];
        VGfloat gradientMatrix[9];
        
        iNVGIconData->ReadL((TUint8 *)gradientData, count * sizeof(VGfloat));
        iNVGIconData->ReadL((TUint8 *)gradientMatrix, 9 * sizeof(VGfloat));
        
        if (paintHandle)
            {
            vgSetPaint(paintHandle,   aPaintMode);
            vgSeti(VG_MATRIX_MODE, aMatrixMode);
            vgLoadMatrix(gradientMatrix);
            if (aPaintMode == VG_FILL_PATH)
                {
                iResetFillPaint = 1;
                }
            else
                {
                iResetStrokePaint = 1;
                }
            }
        else
            {
        if (aLastPaintType != paintType)
            {
            vgSetParameteri(aPaint, VG_PAINT_TYPE, paintType);
            }
        vgSetParameterfv(aPaint, paintPType, count, gradientData);
        
        vgSeti(VG_MATRIX_MODE, aMatrixMode);
        vgLoadMatrix(gradientMatrix);
            }
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        }
    else if (paintType == VG_PAINT_TYPE_COLOR)
        {
        if (aPaintMode == VG_FILL_PATH && iResetFillPaint)
            {
            iResetFillPaint = 0;
            vgSetPaint(aPaint, aPaintMode);
            }
        else if (aPaintMode == VG_STROKE_PATH && iResetStrokePaint)
            {
            iResetStrokePaint = 0;
            vgSetPaint(aPaint, aPaintMode);
            }
        TUint color = static_cast<TUint>(iNVGIconData->ReadInt32L());
        if (aLastPaintType != paintType)
            {
            vgSetParameteri(aPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
            vgSetColor(aPaint, color);
            }
        else
            {
            if (aLastPaintColor != color)
                {
                vgSetColor(aPaint, color);
                }
            }
        aLastPaintColor = color;
        }
    else
        {
        User::Leave(KErrCorrupt);
        }
    aLastPaintType = paintType;
    }

void CNVGCSIcon::SetViewBoxToViewTransformationL(const TSize aSize)
    {
    CNvgFitToViewBoxImpl * fitToViewBoxImpl = CNvgFitToViewBoxImpl::NewLC();

    fitToViewBoxImpl->SetAlign((TNvgAlignStatusType)iPreserveAspectSetting);
    fitToViewBoxImpl->SetScaling((TNvgMeetOrSliceType)iSmilFitSetting);
    
    fitToViewBoxImpl->SetViewBox(iViewBoxX, iViewBoxY, iViewBoxW, iViewBoxH);

    fitToViewBoxImpl->SetWindowViewportTrans(TRect(0, 0, aSize.iWidth, aSize.iHeight), TSize(0, 0));
    
    CleanupStack::PopAndDestroy(fitToViewBoxImpl);
    }

void CNVGCSIcon::SetRotation()
    {
    if (iRotationAngle)
        {
       	vgTranslate(iRotationX, iRotationY);  
           
        vgRotate(iRotationAngle);
           
        vgTranslate(-iRotationX, -iRotationY);
        }
    }

void CNVGCSIcon::UpdateClientMatrices()
    {
    iMatrixMode = vgGeti(VG_MATRIX_MODE);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgGetMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgGetMatrix(iImageMatrix);
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }

void CNVGCSIcon::RestoreClientMatrices()
    {
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadMatrix(iPathMatrix);
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgLoadMatrix(iImageMatrix);
    vgSeti(VG_MATRIX_MODE, iMatrixMode);
    }   
