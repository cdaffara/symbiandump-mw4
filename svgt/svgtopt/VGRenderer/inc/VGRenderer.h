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
* Description:  SVG Implementation header file
 *
*/


#ifndef VGRENDERER_H
#define VGRENDERER_H

#include <e32std.h>
#include <e32base.h>

#include "MVGRendererImpl.h"
#include <SVGRendererId.h>

class CVGSurface;

/**
 *  CVGRenderer
 * 
 */
class CVGRenderer : public CBase
    {

public:
    /**
     * Destructor.
     */
    ~CVGRenderer();

    /*!
     * @fn          NewL()
     * @bief        Creates the renderer abstraction
     * @param       aRendererType   Type of the renderer
     * @param       aSurface        Type of the surface
     * @return      Renderer abstraction
     */
    IMPORT_C static CVGRenderer* NewL(SVGRendererId aRendererType = ESVGRendererSW, TInt aSurface = 0);

    /*!
     * @fn          NewLC()
     * @bief        Creates the renderer abstraction
     * @param       aRendererType   Type of the renderer
     * @param       aSurface        Type of the surface
     * @return      Renderer abstraction
     */
    static CVGRenderer* NewLC(SVGRendererId aRendererType = ESVGRendererSW, TInt aSurface = 0);

public:
    
    /*!
     * @fn          GetRendererName()
     * @bief        Retrieves the renderer implementation's version
     * @return      version of renderer implementation
     */
    TVersion GetRendererVersion()
        {
        return iImpl->GetVersion();
        }
    
    /*!
     * @fn          GetRendererName()
     * @bief        Retrieves the renderer implementation's name 
     * @return      name of renderer implementation
     */
    const TDesC GetRendererName()
        {
        return iImpl->GetName();
        }
    
    // OpenVG function calls
    
    void vgClear(TInt x, TInt y, TInt width, TInt height)
        {
        iImpl->vgClear(x, y, width, height);
        }
    
    void vgSeti(TInt type, TInt value)
        {
        iImpl->vgSeti(type, value);
        }
    TInt vgGeti(TInt type)
        {
        return iImpl->vgGeti(type);
        }    
    void vgSetf (TInt type, TReal32 value)
        {
        iImpl->vgSetf(type, value);
        }
    
    void vgSetfv(TInt type, TInt count, const TReal32 * values)
        {
        iImpl->vgSetfv(type, count, values);
        }
    void vgSetiv(TInt type, TInt count, const TInt * values)
        {
        iImpl->vgSetiv(type, count, values);
        }    
    void vgSetParameteri(TUint handle, TInt paramType, TInt value)
        {
        iImpl->vgSetParameteri(handle, paramType, value);
        }
    
    void vgSetParameterf(TUint handle, TInt paramType, TReal32 value)
        {
        iImpl->vgSetParameterf(handle, paramType, value);
        }
    
    void vgSetParameterfv(TUint object, TInt paramType, TInt count, const TReal32 * values)
        {
        iImpl->vgSetParameterfv(object, paramType, count, values);
        }

    void vgSetColor(TUint paint, TUint rgba)
        {
        iImpl->vgSetColor(paint, rgba);
        }
    
    void vgSetPaint(TUint paint, TUint paintModes)
        {
        iImpl->vgSetPaint(paint, paintModes);
        }
    
    TUint vgCreatePaint()
        {
        return iImpl->vgCreatePaint();
        }
    
    TUint vgCreatePath(TInt pathFormat, TInt datatype, TReal32 scale, TReal32 bias,
                        TInt segmentCapacityHint, TInt coordCapacityHint, TInt capabilities)
        {
        return iImpl->vgCreatePath(pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities);
        }
    
    void vgLoadMatrix(const TReal32 * m)
        {
        iImpl->vgLoadMatrix(m);
        }
    
    void vgMultMatrix(const TReal32 * m)
        {
        iImpl->vgMultMatrix(m);
        }
    
    void vgLoadIdentity()
        {
        iImpl->vgLoadIdentity();
        }
    
    void vgScale(TReal32 sx, TReal32 sy)
        {
        iImpl->vgScale(sx, sy);
        }
    
    void vgRotate(TReal32 angle)
        {
        iImpl->vgRotate(angle);
        }
    void vgReadPixels(void * data, TInt dataStride,TInt dataFormat,TInt sx, TInt sy,TInt width, TInt height)
        {
        iImpl->vgReadPixels(data,dataStride,dataFormat,sx,sy,width,height); 
        }   
    
    void vgWritePixels(const void * data, TInt dataStride, TInt dataFormat,
            TInt dx, TInt dy, TInt width, TInt height)
        {
        iImpl->vgWritePixels(data, dataStride, dataFormat, dx, dy, width, height);
        }
    
    void vgTranslate(TReal32 tx, TReal32 ty)
        {
        iImpl->vgTranslate(tx, ty);
        }
              
    void vgAppendPathData(TUint path, TInt numSegments,
                                                const TUint8 * pathSegments,
                                                const void * pathData)
        {
        iImpl->vgAppendPathData(path, numSegments, pathSegments, pathData);
        }
    
    void vgDrawPath(TUint path, TUint paintModes)
        {
        iImpl->vgDrawPath(path, paintModes);
        }
    
    void vgClearPath(TUint path, TUint capabilities)
        {
        iImpl->vgClearPath(path, capabilities);
        }
    
    TInt vguRect(TUint path, TReal32 x, TReal32 y,
                             TReal32 width, TReal32 height)
        {
        return iImpl->vguRect(path, x, y, width, height);
        }
    
    TInt vguEllipse(TUint path, TReal32 cx, TReal32 cy,
                                TReal32 width, TReal32 height)
        {
        return iImpl->vguEllipse(path, cx, cy, width, height);
        }
          
    TInt vguRoundRect(TUint path, TReal32 x, TReal32 y,
                                            TReal32 width, TReal32 height, TReal32 arcWidth, TReal32 arcHeight)
        {
        return iImpl->vguRoundRect(path, x, y, width, height, arcWidth, arcHeight);
        }
    
    TInt vguLine(TUint path, TReal32 x0, TReal32 y0, TReal32 x1, TReal32 y1)
        {
        return iImpl->vguLine(path, x0, y0, x1, y1);
        } 
    
    void DumpToLogFile()
        {
        return iImpl->DumpToLogFile();
        }
    void CloseLogFile()
        {
        return iImpl->CloseLogFile();
        }
    TUint vgCreateImage(TInt format, TInt width, TInt height, TInt allowedQuality)
        {
        return iImpl->vgCreateImage(format, width, height, allowedQuality);
        }
    
    void vgClearImage(TUint image, TInt x, TInt y, TInt width, TInt height)
        {
        return iImpl->vgClearImage(image, x, y, width, height);
        }
    
    void vgImageSubData(TUint image, const void * data, TInt dataStride,
            TInt dataFormat, TInt x, TInt y, TInt width, TInt height)
        {
        return iImpl->vgImageSubData(image, data, dataStride, dataFormat, x, y, width, height);
        }
    
    void vgGetPixels(TUint dst, TInt dx, TInt dy, TInt sx, TInt sy, TInt width, TInt height)
        {
        iImpl->vgGetPixels(dst, dx, dy, sx, sy, width, height);
        }
    
    void vgDrawImage(TUint image)
        {
        iImpl->vgDrawImage(image);
        }
    
    void vgMask(TUint mask, TInt operation, TInt x, TInt y, TInt width, TInt height)
        {
        return iImpl->vgMask(mask, operation, x, y, width, height);
        }
    
    void vgDestroyImage(TUint aHandle)
        {
        iImpl->vgDestroyImage(aHandle);
        }
    
    void vgDestroyPaint(TUint aHandle)
        {
        iImpl->vgDestroyPaint(aHandle);
        }
    
    void vgDestroyPath(TUint aHandle)
        {
        iImpl->vgDestroyPath(aHandle);
        }

    void ToggleReset()
        {
        iImpl->ToggleReset();
        }
    
    TInt vgGetError()
        {
        return iImpl->vgGetError();
        }
    
    void vgFlush()
        {
        iImpl->vgFlush();
        }

    /*!
     * @fn          GetCurrentSurface()
     * @bief        Retrieves the surface associated with the renderer
     * @return      Renderer surface
     */
    CVGSurface * GetCurrentSurface()
        {
        return this->iCurrentSurface;
        }
    const TPtrC8 TLVEncodedData() const
        {
        return (iImpl->TLVEncodedData());    
        }
    
private:

    CVGRenderer();

    /**
     * default constructor for performing 2nd stage construction
     */
    void ConstructL(SVGRendererId , TInt );

    MVGRendererImpl *   iImpl;
    CVGSurface *        iCurrentSurface;
    RLibrary            iLibrary;
    };

#endif // VGRENDERER_H
