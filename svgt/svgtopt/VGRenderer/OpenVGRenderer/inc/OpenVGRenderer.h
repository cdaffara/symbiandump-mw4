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
* Description:  COpenVGRenderer header file
 *
*/

#ifndef __OPENVGRENDERER_H__
#define __OPENVGRENDERER_H__

#include <e32base.h>
#include <e32std.h>

#include "MVGRendererImpl.h"


class COpenVGRenderer : public CBase, public MVGRendererImpl
    {
public:
    ~COpenVGRenderer(){}
    COpenVGRenderer() {}

public:
    virtual void vgClear(TInt x, TInt y, TInt width, TInt height);
    virtual void vgSeti(TInt type, TInt value);
    virtual void vgSetf (TInt type, TReal32 value);
    virtual void vgSetfv(TInt type, TInt count, const TReal32 * values);
    virtual void vgSetiv(TInt type, TInt count, const TInt * values);
    virtual void vgSetParameteri(TUint handle, TInt paramType, TInt value);      
    virtual void vgSetParameterf(TUint handle,TInt paramType,TReal32 value);
    virtual void vgSetParameterfv(TUint object,TInt paramType,TInt count, const TReal32 * values);
    virtual TInt vgGeti(TInt type);                                                                    
                                      

    virtual void vgSetColor(TUint paint, TUint rgba);
    virtual void vgSetPaint(TUint paint, TUint paintModes);                        
    virtual TUint vgCreatePaint();
    virtual TUint vgCreatePath(TInt pathFormat, TInt datatype, TReal32 scale, TReal32 bias,
                               TInt segmentCapacityHint, TInt coordCapacityHint, TInt capabilities);
    
    virtual void vgLoadMatrix(const TReal32 * m);
    virtual void vgMultMatrix(const TReal32 * m);
    virtual void vgLoadIdentity();
    virtual void vgScale(TReal32 sx, TReal32 sy);
    virtual void vgRotate(TReal32 angle);
    virtual void vgTranslate(TReal32 tx, TReal32 ty);
    virtual void vgReadPixels(void * data, TInt dataStride,TInt dataFormat,TInt sx, TInt sy,TInt width, TInt height);              
    void vgWritePixels(const void * data, TInt dataStride, TInt dataFormat,
                TInt dx, TInt dy, TInt width, TInt height);
    
    virtual void vgAppendPathData(TUint path,TInt numSegments,
                                                const TUint8 * pathSegments,
                                                const void * pathData);                                  
    
    virtual void vgDrawPath(TUint path, TUint paintModes);
    virtual void vgClearPath(TUint path, TUint capabilities);

    virtual TInt vguRect(TUint path, TReal32 x, TReal32 y,
                         TReal32 width, TReal32 height);
    
    virtual  TInt vguEllipse(TUint path, TReal32 cx, TReal32 cy,
                             TReal32 width, TReal32 height);
          
    virtual TInt vguRoundRect(TUint path, TReal32 x, TReal32 y,
                                            TReal32 width, TReal32 height, TReal32 arcWidth, TReal32 arcHeight);
    virtual TInt vguLine(TUint path,TReal32 x0, TReal32 y0,TReal32 x1, TReal32 y1); 
    virtual void DumpToLogFile();
    virtual void CloseLogFile();    
    virtual TUint vgCreateImage(TInt format, TInt width, TInt height, TInt allowedQuality);
    virtual void vgGetPixels(TUint dst, TInt dx, TInt dy, TInt sx, TInt sy, TInt width, TInt height);        
    virtual void vgDrawImage(TUint image);


    virtual void vgClearImage(TUint image, TInt x, TInt y, TInt width, TInt height);

    virtual void vgImageSubData(TUint image, const void * data, TInt dataStride,
                TInt dataFormat, TInt x, TInt y, TInt width, TInt height);


    virtual void vgMask(TUint mask, TInt operation, TInt x, TInt y, TInt width, TInt height);

    virtual void vgDestroyImage(TUint aHandle);
    virtual void vgDestroyPaint(TUint aHandle);        
    virtual void vgDestroyPath(TUint aHandle);

    virtual void vgFlush();
    virtual TInt vgGetError();
    //Dummy added for getting TLV data from pseudorenderer.
    virtual const TPtrC8 TLVEncodedData() const;
    
   };

#endif

