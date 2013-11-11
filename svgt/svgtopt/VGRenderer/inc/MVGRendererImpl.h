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
* Description: MVGSurfaceImpl header file
 *
*/

#ifndef MVGRENDERERIMPL_H_
#define MVGRENDERERIMPL_H_

class MVGSurfaceImpl;

class MVGRendererImpl
    {
protected:
    MVGRendererImpl(){}

public:
    virtual ~MVGRendererImpl(){}
    
    virtual TVersion GetVersion() const = 0;
    virtual const TDesC GetName() const = 0;
    
    virtual void vgClear(TInt x, TInt y, TInt width, TInt height) = 0;
    
    virtual void vgSeti(TInt type, TInt value) = 0;
    virtual void vgSetf (TInt type, TReal32 value) = 0;
    virtual void vgSetfv(TInt type, TInt count, const TReal32 * values) = 0;
    virtual void vgSetiv(TInt type, TInt count, const TInt * values) = 0; 
    virtual void vgSetParameteri(TUint handle, TInt paramType, TInt value) = 0;      
    virtual void vgSetParameterf(TUint handle,TInt paramType,TReal32 value) = 0;
    virtual void vgSetParameterfv(TUint object,TInt paramType,TInt count, const TReal32 * values) = 0;
    virtual TInt vgGeti(TInt type)=0;                                  
                                      

    virtual void vgSetColor(TUint paint, TUint rgba) = 0;
    virtual void vgSetPaint(TUint paint, TUint paintModes) = 0;                        
    virtual TUint vgCreatePaint() = 0;
    virtual TUint vgCreatePath(TInt pathFormat, TInt datatype, TReal32 scale, TReal32 bias,
                       TInt segmentCapacityHint, TInt coordCapacityHint, TInt capabilities) = 0;
    
    virtual void vgLoadMatrix(const TReal32 * m) = 0;
    virtual void vgMultMatrix(const TReal32 * m) = 0;
    virtual void vgLoadIdentity() = 0;
    virtual void vgScale(TReal32 sx, TReal32 sy) = 0;
    virtual void vgRotate(TReal32 angle) = 0;
    virtual void vgTranslate(TReal32 tx, TReal32 ty) = 0;
    virtual void vgReadPixels(void * data, TInt dataStride,TInt dataFormat,TInt sx, TInt sy,TInt width, TInt height)=0;              
    virtual void vgWritePixels(const void * data, TInt dataStride, TInt dataFormat,
            TInt dx, TInt dy, TInt width, TInt height) = 0;
    
    virtual void vgAppendPathData(TUint path,TInt numSegments,
                                                const TUint8 * pathSegments,
                                                const void * pathData) = 0;                                  
    
    virtual void vgDrawPath(TUint path, TUint paintModes) = 0;
    
    virtual void vgClearPath(TUint path, TUint capabilities) = 0;
    
    virtual TInt vguRect(TUint path,
                                        TReal32 x, TReal32 y,
                                        TReal32 width, TReal32 height) = 0;
    virtual  TInt vguEllipse(TUint path,
                                           TReal32 cx, TReal32 cy,
                                           TReal32 width, TReal32 height) = 0;
    virtual TInt vguRoundRect(TUint path, TReal32 x, TReal32 y,
                                            TReal32 width, TReal32 height, TReal32 arcWidth, TReal32 arcHeight) = 0;
    virtual TInt vguLine(TUint path,TReal32 x0, TReal32 y0,TReal32 x1, TReal32 y1) = 0; 

    virtual TUint vgCreateImage(TInt format, TInt width, TInt height, TInt allowedQuality) = 0;
    virtual void vgGetPixels(TUint dst, TInt dx, TInt dy, TInt sx, TInt sy, TInt width, TInt height) = 0;
    virtual void vgDrawImage(TUint image) = 0;

    virtual void vgClearImage(TUint image, TInt x, TInt y, TInt width, TInt height) = 0;
    virtual void vgImageSubData(TUint image, const void * data, TInt dataStride,
                TInt dataFormat, TInt x, TInt y, TInt width, TInt height) = 0;
    virtual void vgMask(TUint mask, TInt operation, TInt x, TInt y, TInt width, TInt height) = 0;

    virtual void vgDestroyImage(TUint aHandle) = 0;
    virtual void vgDestroyPaint(TUint aHandle) = 0;
    virtual void vgDestroyPath(TUint aHandle) = 0;

    virtual  void ToggleReset() = 0;
    virtual void vgFlush() = 0;
    virtual TInt vgGetError() = 0;
    virtual void DumpToLogFile() =0;
    virtual void CloseLogFile() =0;
    virtual MVGSurfaceImpl* CreateVGSurfaceL(TInt aOption) = 0;
    virtual const TPtrC8 TLVEncodedData() const=0; 
        
};

#endif /*MVGRENDERERIMPL_H_*/
