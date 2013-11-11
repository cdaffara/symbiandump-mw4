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
* Description:  CPseodoVGRendererImpl header file
 *
*/

#ifndef PSEODOVGRENDERERIMPL_H
#define PSEODOVGRENDERERIMPL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <VG/vgu.h>
#include <platform/vg/vgcontext_symbian.h>

#include "PseudoVG.h"
#include "MVGRendererImpl.h"
#include "NVGIconHeader.h"
#ifdef VGRENDERER_LOG
#include<flogger.h>   
#endif

/**
 *  CPseodoVGRendererImpl
 * 
 */
class CPseodoVGRendererImpl : public CBase, public  MVGRendererImpl
    {
    static const TUint KMAJOR_VERSION;
    static const TUint KMINOR_VERSION;
    static const TUint KBUILD_VERSION;
    
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPseodoVGRendererImpl();

    /**
     * Two-phased constructor.
     */
    static CPseodoVGRendererImpl* NewL();

    /**
     * Two-phased constructor.
     */
    static CPseodoVGRendererImpl* NewLC();
    
    virtual TVersion GetVersion() const;
    virtual const TDesC GetName() const;
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
    virtual void vgWritePixels(const void * data, TInt dataStride, TInt dataFormat,
                TInt dx, TInt dy, TInt width, TInt height);
    virtual void vgAppendPathData(TUint path,TInt numSegments,
                                                const TUint8 * pathSegments,
                                                const void * pathData);                                  
    virtual void vgDrawPath(TUint path, TUint paintModes);
    virtual void vgClearPath(TUint path, TUint capabilities);
    virtual TInt vguRect(TUint path,
                                        TReal32 x, TReal32 y,
                                        TReal32 width, TReal32 height);
    virtual  TInt vguEllipse(TUint path,
                                           TReal32 cx, TReal32 cy,
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
    virtual void ToggleReset();
    virtual void vgFlush();
    virtual TInt vgGetError();
    virtual MVGSurfaceImpl* CreateVGSurfaceL(TInt aOption);
    
    const TPtr8 GetEncodedData()
        {
        return iEncodedData->Des();
        }
    const TPtrC8 TLVEncodedData() const
            {
            const TPtr8 ptr=iEncodedData->Des();
            const TPtrC8 data(const_cast<unsigned char*>(ptr.Ptr()+ KIconHeaderLength), ptr.Length() - KIconHeaderLength);
            return data;
            }
    
    void EmptyEncodedData();
    TInt AddCommand(TInt aType, TUint8 * aValue, TInt aLength);
    void SetCommonHeader(const TDesC8& aHeader);
    void SetMaskFlag(TBool aVal = ETrue);
        
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPseodoVGRendererImpl();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    void WriteHeader();
    
    TUint8 GetPathCoordianteSize(TUint8 aHandle);
    
    TInt EncodeInt8(TUint8 aVal);
    
    TInt EncodeInt16(TUint16 aVal);
    
    TInt EncodeInt32(TUint32 aVal);
    
    TInt EncodeReal32(TReal32 aVal);
    
    TInt EncodeReal64(TReal64 aVal);
    
    TInt EncodeData(const TAny *aData, TUint aLength);
    
    TInt ExpandEncodedData(TUint aNewLength);
    
    HBufC8 *            iEncodedData;
    TUint               iPaintHandleCount;
    TUint               iPathHandleCount;
    TUint               iHandle;
    static const TUint  ENCODEDDATALENGTH;
    static const TUint  ENCODEDDATAGRANULARITY;
    TInt                iCommonHeaderLength;
    const TUint8* iCommonHeader;
    
    //Command specific logging methods
#ifdef VGRENDERER_LOG
    RFileLogger iLog;//logs    
    void LogvgSeti(VGParamType type, VGint fvalue,TInt cmdsize);
    void LogvgSetf (VGParamType type, VGfloat fvalue,TInt cmdsize);
    void LogvgSetParameteri(VGHandle handle, VGint paramType, VGint pvalue,TInt cmdsize,TInt Lpvalue);		
    void LogvgSetPaint(VGPaint paint, VGbitfield paintModes,TInt cmdsize,TInt Lpvalue);                        
    void LogvgDrawPath(VGbitfield paintModes,int cmdsize);
#endif
    };

#endif // PSEODOVGRENDERERIMPL_H
