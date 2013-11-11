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
* Description:  COpenVGRenderer source file
 *
*/

#include "OpenVGRenderer.h"

#include <VG/vgu.h>

#include <e32debug.h>
void COpenVGRenderer::vgClear(TInt x, TInt y, TInt width, TInt height)
    {
    ::vgClear(x, y, width, height);
    }

void COpenVGRenderer::vgSeti(TInt type, TInt value)
    {
    ::vgSeti((VGParamType)type, value);
    }
TInt COpenVGRenderer::vgGeti(TInt type)
    {
    return ::vgGeti((VGParamType)type);
    }

void COpenVGRenderer::vgSetf (TInt type, TReal32 value)
    {
    ::vgSetf((VGParamType)type, value);
    }
void COpenVGRenderer::vgSetiv(TInt type, TInt count, const TInt * values)
    {
    ::vgSetiv((VGParamType)type, count, (const VGint*)values);
    }
    
void COpenVGRenderer::vgSetfv(TInt type, TInt count, const TReal32 * values)
    {
    ::vgSetfv((VGParamType)type, count, values);
    }

void COpenVGRenderer::vgSetParameteri(TUint handle, TInt paramType, TInt value)
    {
    ::vgSetParameteri(handle, paramType, value);
    }

void COpenVGRenderer::vgSetParameterf(TUint handle, TInt paramType, TReal32 value)
    {
    ::vgSetParameterf(handle, paramType, value);
    }

void COpenVGRenderer::vgSetParameterfv(TUint object, TInt paramType, TInt count, const TReal32 * values)
    {
    ::vgSetParameterfv(object, paramType, count, values);
    }
                                  
void COpenVGRenderer::vgSetColor(TUint paint, TUint rgba)
    {
    ::vgSetColor(paint, rgba);
    }

void COpenVGRenderer::vgSetPaint(TUint paint, TUint paintModes)
    {
    ::vgSetPaint(paint, paintModes);
    }

TUint COpenVGRenderer::vgCreatePaint()
    {
    return ::vgCreatePaint();
    }

TUint COpenVGRenderer::vgCreatePath(TInt pathFormat, TInt datatype, TReal32 scale, TReal32 bias,
        TInt segmentCapacityHint, TInt coordCapacityHint, TInt capabilities)
    {
    return ::vgCreatePath(pathFormat, (VGPathDatatype)datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities);
    }

void COpenVGRenderer::vgLoadMatrix(const TReal32 * m)
    {
    ::vgLoadMatrix(m);
    }

void COpenVGRenderer::vgMultMatrix(const TReal32 * m)
    {
    ::vgMultMatrix(m);
    }

void COpenVGRenderer::vgLoadIdentity()
    {
    ::vgLoadIdentity();
    }

void COpenVGRenderer::vgScale(TReal32 sx, TReal32 sy)
    {
    ::vgScale(sx, sy);
    }

void COpenVGRenderer::vgRotate(TReal32 angle)
    {
    ::vgRotate(angle);
    }
void COpenVGRenderer::vgReadPixels(void * data, TInt dataStride,TInt dataFormat,TInt sx, TInt sy,TInt width, TInt height)
    {
    ::vgReadPixels(data,dataStride,(VGImageFormat)dataFormat,sx,sy,width,height);    
    }

void COpenVGRenderer::vgWritePixels(const void * data, TInt dataStride, TInt dataFormat,
            TInt dx, TInt dy, TInt width, TInt height)
    {
    ::vgWritePixels(data, dataStride, (VGImageFormat)dataFormat, dx, dy, width, height);
    }

void COpenVGRenderer::vgTranslate(TReal32 tx, TReal32 ty)
    {
    ::vgTranslate(tx, ty);
    }

void COpenVGRenderer::vgAppendPathData(TUint path, TInt numSegments,
                                            const TUint8 * pathSegments,
                                            const void * pathData)
    {
    ::vgAppendPathData(path, numSegments, pathSegments, pathData);
    }

void COpenVGRenderer::vgDrawPath(TUint path, TUint paintModes)
    {
    ::vgDrawPath(path, paintModes);
    }

void COpenVGRenderer::vgClearPath(TUint path, TUint capabilities)
    {
    ::vgClearPath(path, capabilities);
    }

TInt COpenVGRenderer::vguRect(TUint path, TReal32 x, TReal32 y,
                     TReal32 width, TReal32 height)
    {
    return ::vguRect(path, x, y, width, height);
    }

 TInt COpenVGRenderer::vguEllipse(TUint path,
                                       TReal32 cx, TReal32 cy,
                                       TReal32 width, TReal32 height)
    {
    return ::vguEllipse(path, cx, cy, width, height);
    }
      
TInt COpenVGRenderer::vguRoundRect(TUint path, TReal32 x, TReal32 y,
                                        TReal32 width, TReal32 height, TReal32 arcWidth, TReal32 arcHeight)
    {
    return ::vguRoundRect(path, x, y, width, height, arcWidth, arcHeight);
    }

TInt COpenVGRenderer::vguLine(TUint path, TReal32 x0, TReal32 y0, TReal32 x1, TReal32 y1)
    {
    return ::vguLine(path, x0, y0, x1, y1);
    }
    
void COpenVGRenderer::DumpToLogFile()
    {
// Nothing TO DO here
    }
void COpenVGRenderer::CloseLogFile()
    {
// Nothing TO DO here
    }

TUint COpenVGRenderer::vgCreateImage(TInt format, TInt width, TInt height, TInt allowedQuality)
    {
    return ::vgCreateImage((VGImageFormat)format, width, height, allowedQuality);
    }

void COpenVGRenderer::vgGetPixels(TUint dst, TInt dx, TInt dy, TInt sx, TInt sy, TInt width, TInt height)
    {
    ::vgGetPixels(dst, dx, dy, sx, sy, width, height);
    }
    
void COpenVGRenderer::vgDrawImage(TUint image)
    {
    ::vgDrawImage(image);
    }

void COpenVGRenderer::vgClearImage(TUint image, TInt x, TInt y, TInt width, TInt height)
    {
    return ::vgClearImage(image, x, y, width, height);
    }

void COpenVGRenderer::vgImageSubData(TUint image, const void * data, TInt dataStride,
            TInt dataFormat, TInt x, TInt y, TInt width, TInt height)
    {
    return ::vgImageSubData(image, data, dataStride, (VGImageFormat)dataFormat, x, y, width, height);
    }

void COpenVGRenderer::vgMask(TUint mask, TInt operation, TInt x, TInt y, TInt width, TInt height)
    {
    return ::vgMask(mask, (VGMaskOperation)operation, x, y, width, height);
    }

void COpenVGRenderer::vgDestroyImage(TUint aHandle)
    {
    ::vgDestroyImage(aHandle);
    }
    
void COpenVGRenderer::vgDestroyPaint(TUint aHandle)
    {
    ::vgDestroyPaint(aHandle);
    }
    
void COpenVGRenderer::vgDestroyPath(TUint aHandle)
    {
    ::vgDestroyPath(aHandle);
    }

void COpenVGRenderer::vgFlush()
    {
    ::vgFlush();
    }

TInt COpenVGRenderer::vgGetError()
    {
    return ::vgGetError();
    }
const TPtrC8 COpenVGRenderer::TLVEncodedData() const
    {
    return TPtrC8();
    }
