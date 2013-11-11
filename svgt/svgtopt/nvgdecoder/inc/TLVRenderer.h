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
* Description:  NVG Decoder header file
 *
*/


#ifndef _TLVRENDERER_H_
#define _TLVRENDERER_H_

#include <e32base.h>
#include <openvg.h>
#include <vgu.h>
#include "FloatFixPt.h"
#include "PseudoVG.h"
#include "NVGUtil.h"

class CNVGTLVIcon;
class CNVGIconData;

class MVGImageBinder;

class CTLVRenderer : public CBase
    {
    
public:
    static CTLVRenderer * NewL(const TDesC8& aPtr, TInt aWidth, TInt aHeight);
 
    static CTLVRenderer * NewLC(const TDesC8& aPtr, TInt aWidth, TInt aHeight);
  
    virtual ~CTLVRenderer();
    
    virtual TInt ExecuteL();
    
    void SetVGImageBinder(MVGImageBinder *aImageBinder)
        {
        iVGImageBinder = aImageBinder;
        }

protected:
    void ConstructL(const TDesC8& aPtr);
    CTLVRenderer(TInt aWidth, TInt aHeight);

    void DvgSetiL( );    
    void DvgSetfL( );   
    void DvgSetParameteriL( );
    void DvgSetParameterfL( );    
    void DvgSetParameterfvL( );    
    void DvgSetColorL( );    
    void DvgSetPaintL( );    
    void DvgLoadMatrixL( );  
    void DvgMultMatrixL( );  
    void DvgLoadIdentityL( );    
    void DvgScaleL( );    
    void DvgTranslateL( );   
    void DvgAppendPathDataL( );    
    void DvgDrawPathL( );
    void DvgClearPathL();
    void DvguRectL( );    
    void DvguEllipseL( );    
    void DvguRoundRectL( );    
    void DvguLineL( );    
    void DvgCreatePaintL( );   
    void DvgSetivL( );                
    void DvgClearL();
    void DvgSetfvL();
    void DvgRotateL();
    void DvgCreatePathL();
    void DvgCreateImageL();
    void DvgGetPixelsL();
    void DvgClearImageL();
    void DvgImageSubDataL();
    void DvgDrawImageL();
    void DvgDestroyImageL();
    void DvgDestroyPaintL();
    void DvgDestroyPathL();
    
    void DvgPrepareToBindImageL();
    void DvgBindImageL();
    void DvgUnBindImageL();
    
    void DvgFlushL(); 

    virtual TInt ExecuteL(TInt index);

#ifdef NVG_DEBUG
    //Command specific logging methods
    void LogvgSeti(VGParamType type, VGint fvalue);
    void LogvgSetf (VGParamType type, VGfloat fvalue);
    void LogvgSetXv(VGParamType type, VGint count, TInt8 vtype, VGint * LogvgSetXv);    
    void LogvgSetParameteri(VGHandle handle, VGint paramType, VGint pvalue, TInt Lpvalue);		
    void LogvgSetPaint(VGPaint paint, VGbitfield paintModes, TInt Lpvalue);                        
    void LogvgDrawPath(VGuint path, VGbitfield paintModes);
    void LogvgSetParameterfv(VGPaint handle, VGint paramtype, TInt count, VGfloat * data);
    void LogvgImageSubData(VGImage imageHandle, const void * dataPtr, TInt dataLength, VGint dataStride,
                VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height);
#endif
    
protected:

    typedef void (CTLVRenderer::*pvgapi)( );
    pvgapi vgapi[EvgFlush + 1];
    
    CNVGIconData *  iNVGIconData;
    TInt            iDataLength;
    TSize           iResizedCanvasSize;

    struct TLVVGHandlePair
        {
        enum THandleType
            {
            EVGPATH,
            EVGPAINT,
            EVGIMAGE
            };
        VGHandle    iVGHandle;
        TInt        iTLVHandle;
        THandleType iHandleType;
        TInt        iCustomData;
        
        explicit TLVVGHandlePair(VGHandle aVGHandle = 0, TInt aTLVHandle = 0,
                        THandleType aHandleType = EVGPAINT, TInt aCustomData = 0)
            : iVGHandle(aVGHandle),
              iTLVHandle(aTLVHandle),
              iHandleType(aHandleType),
              iCustomData(aCustomData)
            {
            }
        };

    VGHandle GetHandleL(int aIndex);
    VGHandle GetHandle(int aIndex);
    VGHandle RemoveHandleL(int aIndex);
    
    void GetHandlePair(int aIndex, TLVVGHandlePair & aPair);

    void SetHandle(int aIndex, VGHandle aHandle);

    TPoint  GetTranslatedPoint(VGfloat matrix[9], TPoint aPoint);
    VGfloat MinVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4);
    VGfloat MaxVal4(VGfloat x1, VGfloat x2, VGfloat x3, VGfloat x4);
    
    VGPath          iPathHandle;
    
    TUint8          iNvgHeader[KNVGHeaderSize];
    TInt            iTargetWidth;
    TInt            iTargetHeight;
    TBool           iScaled;
    
    TReal32             iUserMatrix[9];
    MVGImageBinder *    iVGImageBinder;
    TUint               iPrepareToBindImage;
    RArray<TLVVGHandlePair> iTLVVGHandleList;
    
    CNVGTLVIcon *   iNVGTLVIcon;
    friend class CNVGTLVIcon;
    
    DECLARE_HANDLECHECKER()
    };

#endif
