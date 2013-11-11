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
* Description:  CPseodoVGRendererImpl source file
 *
*/

#include "PseodoVGRendererImpl.h"
#include "PseudoVGSurfaceImpl.h"
#include <f32file.h>
#include <s32mem.h>
#include <VG/vgu.h>
const TUint CPseodoVGRendererImpl::ENCODEDDATALENGTH        = 1024;
const TUint CPseodoVGRendererImpl::ENCODEDDATAGRANULARITY   = 64;
const TUint CPseodoVGRendererImpl::KMAJOR_VERSION           = 1;
const TUint CPseodoVGRendererImpl::KMINOR_VERSION           = 0;
const TUint CPseodoVGRendererImpl::KBUILD_VERSION           = 1;

EXPORT_C MVGRendererImpl* CreateVGRendererImplL()
    {
    return CPseodoVGRendererImpl::NewL();
    }

CPseodoVGRendererImpl::CPseodoVGRendererImpl()
    {
    iPaintHandleCount   = 0;
    iPathHandleCount    = 0;
    iEncodedData        = 0;
    iHandle             = 1;
    iCommonHeaderLength = 0;
    iCommonHeader = 0;
    }

CPseodoVGRendererImpl::~CPseodoVGRendererImpl()
    {
    delete iEncodedData;
    }

CPseodoVGRendererImpl* CPseodoVGRendererImpl::NewLC()
    {
    CPseodoVGRendererImpl* self = new (ELeave)CPseodoVGRendererImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPseodoVGRendererImpl* CPseodoVGRendererImpl::NewL()
    {
    CPseodoVGRendererImpl* self=CPseodoVGRendererImpl::NewLC();
    CleanupStack::Pop();
    return self;
    }

inline void CPseodoVGRendererImpl::ConstructL()
    {
    iEncodedData = HBufC8::NewL(CPseodoVGRendererImpl::ENCODEDDATALENGTH);
    WriteHeader();
    }

TVersion CPseodoVGRendererImpl::GetVersion() const
    {
    return TVersion(KMAJOR_VERSION, KMINOR_VERSION, KBUILD_VERSION);
    }

const TDesC CPseodoVGRendererImpl::GetName() const
    {
    _LIT(KPseudoVGRenderer, "TLVVGRenderer");
    return KPseudoVGRenderer; 
    }

void CPseodoVGRendererImpl::WriteHeader()
    {
    TUint8 EmptyHeader[KIconHeaderLength]; 
    if(iCommonHeader)
        {
        EncodeData(iCommonHeader, KIconHeaderLength);
        }
    else
        {
        EncodeData(EmptyHeader, KIconHeaderLength);
        }
    
    const TInt8 *   KNVGFileIdentifier   = (const TInt8*)"nvg";

    EncodeData(KNVGFileIdentifier, 3);
    EncodeInt8(KNVGFileVersion);
    EncodeInt16(KNVGHeaderSize);
    EncodeInt16(KNVGReserved1);

    // Rest of the headers are not used in TLV format
    const TInt8   KUnusedHeaderLength  = 0x2C;

    for (TInt i = 0; i < KUnusedHeaderLength; ++i)
        {
        EncodeInt8(0);
        }
    }

void CPseodoVGRendererImpl::vgClear(TInt x, TInt y, TInt width, TInt height)
    {
    EncodeInt8(EvgClear);
    EncodeReal32(x);
    EncodeReal32(y);
    EncodeReal32(width);
    EncodeReal32(height);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgClear"));
#endif
    }

void CPseodoVGRendererImpl::vgSeti(TInt type, TInt value)
    {
    EncodeInt8(EvgSeti);
    EncodeInt16(type);
    EncodeInt16(value);
#ifdef VGRENDERER_LOG
    LogvgSeti((VGParamType)type,value,0);   
#endif
    }
TInt CPseodoVGRendererImpl::vgGeti(TInt type)
    {
    	
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgGeti"));
#endif
        switch( type )
        {
            case VG_IMAGE_MODE:
                return (VGint)VG_DRAW_IMAGE_MULTIPLY;
            case VG_BLEND_MODE:
                return (VGint)VG_BLEND_SRC_OVER;
            case VG_FILTER_CHANNEL_MASK:
                return (VGint)0;  
            default:
                return 0;        
        }
    }

void CPseodoVGRendererImpl::vgSetf (TInt type, TReal32 value)
    {
    EncodeInt8(EvgSetf);
    EncodeInt16(type);
    EncodeReal32(value);
#ifdef VGRENDERER_LOG
    LogvgSetf((VGParamType)type,value,0); 
#endif
    }

void CPseodoVGRendererImpl::vgSetfv(TInt type, TInt count, const TReal32 * values)
    {
    EncodeInt8(EvgSetfv);
    EncodeInt16(type);
    EncodeInt16(count);
    EncodeData(values, count * sizeof(TReal32));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgSetfv"));
#endif
    }
void CPseodoVGRendererImpl::vgSetiv(TInt type, TInt count, const TInt * values)
    {
     EncodeInt8(EvgSetiv);
    EncodeInt16(type);
    EncodeInt16(count);
    EncodeData(values, count * sizeof(TInt));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgSetiv"));
#endif
    }
void CPseodoVGRendererImpl::vgSetParameteri(TUint handle, TInt paramType, TInt value)
    {
    EncodeInt8(EvgSetParameteri);
    EncodeInt32(handle);
    EncodeInt16(paramType);
    EncodeInt16(value);
#ifdef VGRENDERER_LOG
    LogvgSetParameteri(handle,paramType,value,0,handle);				   
#endif
    }

void CPseodoVGRendererImpl::vgSetParameterf(TUint handle,TInt paramType,TReal32 value)
    {
    EncodeInt8(EvgSetParameterf);
    EncodeInt32(handle);
    EncodeInt16(paramType);
    EncodeReal32(value);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgSetParameterf"));
#endif
    }

void CPseodoVGRendererImpl::vgSetParameterfv(TUint handle,TInt paramType,TInt count, const TReal32 * values)
    {
    EncodeInt8(EvgSetParameterfv);
    EncodeInt32(handle);
    EncodeInt16(paramType);
    EncodeInt32(count);
    EncodeData(values, count * sizeof(TReal32));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgSetParameterfv"));
#endif
    }
                                  
void CPseodoVGRendererImpl::vgSetColor(TUint paint, TUint rgba)
    {
    EncodeInt8(EvgSetColor);
    EncodeInt32(paint);
    EncodeInt32(rgba);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgSetColor"));
#endif
    }

void CPseodoVGRendererImpl::vgSetPaint(TUint paint, TUint paintModes)
    {
    EncodeInt8(EvgSetPaint);
    EncodeInt32(paint);
    EncodeInt8(paintModes);
#ifdef VGRENDERER_LOG
    LogvgSetPaint(paint,paintModes,0,paint);      
#endif
    }

TUint CPseodoVGRendererImpl::vgCreatePaint()
    {
    iHandle++;
    EncodeInt8(EvgCreatePaint);
    EncodeInt32(iHandle);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgCreatePaint"));
#endif
    return iHandle;
    }

TUint CPseodoVGRendererImpl::vgCreatePath(TInt pathFormat, TInt datatype, TReal32 scale, TReal32 bias,
        TInt segmentCapacityHint, TInt coordCapacityHint, TInt capabilities)
    {
    iHandle++;
    EncodeInt8(EvgCreatePath);
    EncodeInt32(pathFormat);
    EncodeInt8(datatype);
    EncodeReal32(scale);
    EncodeReal32(bias);
    EncodeInt32(segmentCapacityHint);
    EncodeInt32(coordCapacityHint);
    EncodeInt32(capabilities);
    EncodeInt32(iHandle);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgCreatePath"));
#endif
    return iHandle;
    }

void CPseodoVGRendererImpl::vgLoadMatrix(const TReal32 * m)
    {
    EncodeInt8(EvgLoadMatrix);
    EncodeData(m, 9 * sizeof(TReal32));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgLoadMatrix"));
#endif
    }

void CPseodoVGRendererImpl::vgMultMatrix(const TReal32 * m)
    {
    EncodeInt8(EvgMultMatrix);
    EncodeData(m, 9 * sizeof(TReal32));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgMultMatrix"));
#endif
    }

void CPseodoVGRendererImpl::vgLoadIdentity()
    {
    EncodeInt8(EvgLoadIdentity);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgLoadIdentity"));
#endif
    }

void CPseodoVGRendererImpl::vgScale(TReal32 sx, TReal32 sy)
    {
    EncodeInt8(EvgScale);
    EncodeReal32(sx);
    EncodeReal32(sy);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgScale"));
#endif
    }

void CPseodoVGRendererImpl::vgTranslate(TReal32 tx, TReal32 ty)
    {
    EncodeInt8(EvgTranslate);
    EncodeReal32(tx);
    EncodeReal32(ty);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgTranslate"));
#endif
    }

void CPseodoVGRendererImpl::vgRotate(TReal32 angle)
    {
    EncodeInt8(EvgRotate);
    EncodeReal32(angle);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgRotate"));
#endif
    }

void CPseodoVGRendererImpl::vgReadPixels(void * /*data*/, TInt /*dataStride*/, TInt /*dataFormat*/, TInt /*sx*/, TInt /*sy*/, TInt /*width*/, TInt /*height*/)
    {
    //TODO
    return;
    }

void CPseodoVGRendererImpl::vgWritePixels(const void * /*data*/, TInt /*dataStride*/, TInt /*dataFormat*/,
                TInt /*dx*/, TInt /*dy*/, TInt /*width*/, TInt /*height*/)
    {
    //TODO
    return;
    }

void CPseodoVGRendererImpl::vgAppendPathData(TUint path,TInt numSegments,
                                            const TUint8 * pathSegments,
                                            const void * pathData)
    {
    EncodeInt8(EvgAppendPathData);
    EncodeInt32(path);
    EncodeInt16(numSegments);
    EncodeData(pathSegments, numSegments);
    
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
    EncodeInt16(coordinateCount);
    EncodeData(pathData, coordinateCount * GetPathCoordianteSize(path));
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vgAppendPathData"));
#endif
    }

void CPseodoVGRendererImpl::vgDrawPath(TUint path, TUint capabilities)

    {
    EncodeInt8(EvgDrawPath);
    EncodeInt32(path);
    EncodeInt16(capabilities);

#ifdef VGRENDERER_LOG
    LogvgDrawPath(paintModes,0);   
#endif

    }

void CPseodoVGRendererImpl::vgClearPath(TUint path, TUint capabilities)
    {
    EncodeInt8(EvgClearPath);
    EncodeInt32(path);
    EncodeInt16(capabilities);
    }

TInt CPseodoVGRendererImpl::vguRect(TUint path,
                                    TReal32 x, TReal32 y,
                                    TReal32 width, TReal32 height)
    {
    EncodeInt8(EvguRect);
    EncodeInt32(path);
    EncodeReal32(x);
    EncodeReal32(y);
    EncodeReal32(width);
    EncodeReal32(height);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vguRect"));
#endif
    return KErrNone;
    }

TInt CPseodoVGRendererImpl::vguEllipse(TUint path,
                                       TReal32 cx, TReal32 cy,
                                       TReal32 width, TReal32 height)
    {
    EncodeInt8(EvguEllipse);
    EncodeInt32(path);
    EncodeReal32(cx);
    EncodeReal32(cy);
    EncodeReal32(width);
    EncodeReal32(height);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vguEllipse"));
#endif
    return KErrNone;
    }

TInt CPseodoVGRendererImpl::vguRoundRect(TUint path, TReal32 x, TReal32 y,
                                        TReal32 width, TReal32 height, TReal32 arcWidth, TReal32 arcHeight)
    {
    EncodeInt8(EvguRoundRect);
    EncodeInt32(path);
    EncodeReal32(x);
    EncodeReal32(y);
    EncodeReal32(width);
    EncodeReal32(height);
    EncodeReal32(arcWidth);
    EncodeReal32(arcHeight);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vguRoundRect"));
#endif
    return KErrNone;
    }

TInt CPseodoVGRendererImpl::vguLine(TUint path,TReal32 x0, TReal32 y0,TReal32 x1, TReal32 y1)
    {
    EncodeInt8(EvguLine);
    EncodeInt32(path);
    EncodeReal32(x0);
    EncodeReal32(y0);
    EncodeReal32(x1);
    EncodeReal32(y1);
#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("vguLine"));
#endif
    return KErrNone;
    }

TUint CPseodoVGRendererImpl::vgCreateImage(TInt format, TInt width, TInt height, TInt allowedQuality)
    {
    iHandle++;
    EncodeInt8(EvgCreateImage);
    EncodeInt32(format);
    EncodeInt32(width);
    EncodeInt32(height);
    EncodeInt8(allowedQuality);
    EncodeInt32(iHandle);
    return iHandle;
    }

void CPseodoVGRendererImpl::vgGetPixels(TUint /*dst*/, TInt /*dx*/, TInt /*dy*/, TInt /*sx*/, TInt /*sy*/, TInt /*width*/, TInt /*height*/)
    {
    //TODO: not used in SVGEngine
    }

void CPseodoVGRendererImpl::vgDrawImage(TUint image)
    {
    EncodeInt8(EvgDrawImage);
    EncodeInt32(image);
    }

void CPseodoVGRendererImpl::vgClearImage(TUint image, TInt x, TInt y, TInt width, TInt height)
    {
    EncodeInt8(EvgClearImage);
    EncodeInt32(image);
    EncodeInt32(x);
    EncodeInt32(y);
    EncodeInt32(width);
    EncodeInt32(height);
    }

void CPseodoVGRendererImpl::vgImageSubData(TUint image, const void * data, TInt dataStride,
            TInt dataFormat, TInt x, TInt y, TInt width, TInt height)
    {
    EncodeInt8(EvgImageSubData);
    EncodeInt32(image);
    EncodeInt32(dataStride);
    EncodeInt32(dataFormat);
    EncodeInt32(x);
    EncodeInt32(y);
    EncodeInt32(width);
    EncodeInt32(height);

    // append the actual data
    struct
    {

        VGImageFormat       format;
        VGuint              depth;
        VGuint              bytePerPixels;
        VGuint              redBits;
        VGuint              greenBits;
        VGuint              blueBits;
        VGuint              alphaBits;
    } const static vgiSurfaceFormatDesc[] =

    {
        {   VG_sRGBX_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_sRGBA_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_sRGBA_8888_PRE, 32, 32/8, 8, 8, 8, 8 },
        {   VG_sRGB_565,       16, 16/8, 5, 6, 5, 0 },
        {   VG_sRGBA_5551,     16, 16/8, 5, 5, 5, 1 },
        {   VG_sRGBA_4444,     16, 16/8, 4, 4, 4, 4 },
        {   VG_sL_8,            8, 8/8, 8, 0, 0, 0 },
        {   VG_lRGBX_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_lRGBA_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_lRGBA_8888_PRE, 32, 32/8, 8, 8, 8, 8 },
        {   VG_lL_8,            8, 8/8, 8, 0, 0, 0 },
        {   VG_A_8,             8, 8/8, 0, 0, 0, 8 },
        {   VG_BW_1,            8, 8/8, 8, 0, 0, 0 },

        {   VG_sXRGB_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_sARGB_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_sARGB_8888_PRE, 32, 32/8, 8, 8, 8, 8 },
        {   VG_sARGB_1555,     16, 16/8, 5, 5, 5, 1 },
        {   VG_sARGB_4444,     16, 16/8, 4, 4, 4, 4 },
        {   VG_lXRGB_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_lARGB_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_lARGB_8888_PRE, 32, 32/8, 8, 8, 8, 8 },

        {   VG_sBGRX_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_sBGRA_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_sBGRA_8888_PRE, 32, 32/8, 8, 8, 8, 8 },
        {   VG_sBGR_565,       16, 16/8, 5, 6, 5, 0 },
        {   VG_sBGRA_5551,     16, 16/8, 5, 5, 5, 1 },
        {   VG_sBGRA_4444,     16, 16/8, 4, 4, 4, 4 },
        {   VG_lBGRX_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_lBGRA_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_lBGRA_8888_PRE, 32, 32/8, 8, 8, 8, 8 },

        {   VG_sXBGR_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_sABGR_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_sABGR_8888_PRE, 32, 32/8, 8, 8, 8, 8 },
        {   VG_sABGR_1555,     16, 16/8, 5, 5, 5, 1 },

        {   VG_sABGR_4444,     16, 16/8, 4, 4, 4, 4 },
        {   VG_lXBGR_8888,     32, 32/8, 8, 8, 8, 0 },
        {   VG_lABGR_8888,     32, 32/8, 8, 8, 8, 8 },
        {   VG_lABGR_8888_PRE, 32, 32/8, 8, 8, 8, 8 }
    };

    TInt bytePerPixels  = 0;

    for (TInt i = 0; i < sizeof(vgiSurfaceFormatDesc)/sizeof(vgiSurfaceFormatDesc[0]); ++i)
        {
        if (dataFormat == vgiSurfaceFormatDesc[i].format)
            {
            bytePerPixels = vgiSurfaceFormatDesc[i].bytePerPixels;
            break;
            }
        }

    TUint dataLength;

    if (dataStride < 0)
        {
        dataLength = -dataStride * height;
        }
    else
        {
        dataLength = dataStride * height;
        }

    TInt M = width * bytePerPixels;
    TInt imageSize = M * height;

    if (imageSize > dataLength)
        {
        dataLength = imageSize; 
        }

    if (dataLength == 0)
        {
        EncodeInt32(0);
        return;
        }

    TUint8 * dstData    = new TUint8[dataLength];
    if (dstData)
        {
        Mem::FillZ(dstData, dataLength);
        TUint8 * dstDataPtr;
        TUint8 * srcDataPtr  = (TUint8 *)data;
        EncodeInt32(dataLength);
        if (dataStride < 0)
            {
            dstDataPtr = dstData + ( height - 1 ) * (-dataStride);
            }
        else
            {
            dstDataPtr = dstData;
            }

        do
            {
                Mem::Copy(dstDataPtr, srcDataPtr, M );
                srcDataPtr += dataStride;
                dstDataPtr += dataStride;
            } while( --height );


        EncodeData(dstData, dataLength);
        delete [] dstData;
        }
    else
        {

#ifdef VGRENDERER_LOG
        iLog.WriteFormat(_L("Allocating memory in vgImageSubData failed"));
#endif

        EncodeInt32(0);
        }
    }

void CPseodoVGRendererImpl::vgMask(TUint /*mask*/, TInt /*operation*/, TInt /*x*/, TInt /*y*/, TInt /*width*/, TInt /*height*/)
    {
    //TODO: not used in SVGEngine
    }

void CPseodoVGRendererImpl::ToggleReset()
    {
    }

void CPseodoVGRendererImpl::vgDestroyImage(TUint aHandle)
    {
    EncodeInt8(EvgDestroyImage);
    EncodeInt32(aHandle);
    }

void CPseodoVGRendererImpl::vgDestroyPaint(TUint /*aHandle*/)
    {
    //TODO
    }

void CPseodoVGRendererImpl::vgDestroyPath(TUint /*aHandle*/)
    {
    //TODO
    }

void CPseodoVGRendererImpl::vgFlush()
    {
    //TODO
    }

TInt CPseodoVGRendererImpl::vgGetError()
    {
    //TODO
    return 0;
    }

MVGSurfaceImpl* CPseodoVGRendererImpl::CreateVGSurfaceL(TInt /*aOption*/)
    {
    return CPseudoVGSurfaceImpl::NewL(this);
    }

TUint8 CPseodoVGRendererImpl::GetPathCoordianteSize(TUint8 /*aHandle*/)
    {
    return sizeof(TReal32);
    }

inline TInt CPseodoVGRendererImpl::EncodeInt8(TUint8 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CPseodoVGRendererImpl::EncodeInt16(TUint16 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CPseodoVGRendererImpl::EncodeInt32(TUint32 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CPseodoVGRendererImpl::EncodeReal32(TReal32 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

inline TInt CPseodoVGRendererImpl::EncodeReal64(TReal64 aVal)
    {
    return EncodeData(&aVal, sizeof(aVal));
    }

TInt CPseodoVGRendererImpl::EncodeData(const TAny *aData, TUint aLength)
    {
    TInt result = KErrNone;
    TPtr8 lPtr( iEncodedData->Des() );
    TInt encodedDataLength      = lPtr.Length() + aLength;
    TInt encodedDataMaxLength   = lPtr.MaxLength();

    if (encodedDataLength >= encodedDataMaxLength)
        {
        if ((result = ExpandEncodedData(encodedDataLength)) == KErrNone)
            {
            TPtr8 lPtr1( iEncodedData->Des() );
            lPtr1.Append((TUint8*)(aData), aLength);
            }
        }
    else
        {
        lPtr.Append((TUint8*)(aData), aLength);
        }

    return result;
    }

TInt CPseodoVGRendererImpl::ExpandEncodedData(TUint aNewLength)
    {
    TInt result = KErrNone;
    TPtr8 lPtr( iEncodedData->Des() );
    TInt encodedDataMaxLength   = lPtr.MaxLength();
    TUint granularities = ((aNewLength - encodedDataMaxLength) / CPseodoVGRendererImpl::ENCODEDDATAGRANULARITY) + 1;
    HBufC8 * tmpBuf = HBufC8::New(encodedDataMaxLength + granularities * CPseodoVGRendererImpl::ENCODEDDATAGRANULARITY);

    if (tmpBuf == 0)
        {
        result = KErrNoMemory;
        }

    else
        {
        TPtr8 tmpBufPtr (tmpBuf->Des());
        tmpBufPtr.Copy(*iEncodedData);

        delete iEncodedData;
        iEncodedData = tmpBuf;
        }

    return result;
    }


void CPseodoVGRendererImpl::EmptyEncodedData()
    {
    TPtr8 lPtr( iEncodedData->Des() );
    lPtr.Zero();
    WriteHeader();

#ifdef VGRENDERER_LOG
    iLog.WriteFormat(_L("InitializeSurface-EmptyEncodedData and WriteheaderData"));
#endif
    }

TInt CPseodoVGRendererImpl::AddCommand(TInt aType, TUint8 * aValue, TInt aLength)
    {
    TInt ret = 0;
    ret = EncodeInt8(aType);
    if (aValue && aLength > 0)
        {
        ret |= EncodeData(aValue, aLength);
        }

    return ret;
    }

void CPseodoVGRendererImpl::SetCommonHeader(const TDesC8& aHeader)
    {
    iCommonHeader = aHeader.Ptr();
    iCommonHeaderLength = aHeader.Length();
    TPtr8 lPtr( iEncodedData->Des() );

    if (iCommonHeaderLength != 0)
        {
        lPtr.Replace(0, iCommonHeaderLength, aHeader);
        }
    }

void CPseodoVGRendererImpl::SetMaskFlag(TBool aVal)
    {
    TPtr8 lPtr( iEncodedData->Des() );
    TNVGIconHeader iconheader(lPtr);
    iconheader.SetIsMask(aVal);
    }

void CPseodoVGRendererImpl::DumpToLogFile()
    {
#ifdef VGRENDERER_LOG
    TInt err = iLog.Connect();
    TRAPD(logerror,iLog.CreateLog(_L("PseodoEncoder"),_L("PseodoEncoder.txt"),EFileLoggingModeOverwrite));
#endif    
    }

void CPseodoVGRendererImpl::CloseLogFile()
    {       
#ifdef VGRENDERER_LOG
    iLog.CloseLog();
    iLog.Close();
#endif
    }         



#ifdef VGRENDERER_LOG
void CPseodoVGRendererImpl::LogvgSetf(VGParamType type, VGfloat value,TInt cmdsize)
    {
    TBufC8<70> logbuf;
    TPtr8 logptr = logbuf.Des();	 

    logptr.Append(_L("vgSetf("));

    switch( type )
        {
        case VG_STROKE_LINE_WIDTH:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;

        case VG_STROKE_MITER_LIMIT:
            {
            logptr.Append(_L("VG_STROKE_MITER_LIMIT")); 
            }
            break;
        case VG_STROKE_DASH_PHASE:
            {
            logptr.Append(_L("VG_STROKE_DASH_PHASE")); 
            }
            break;

            
        case VG_MATRIX_MODE:
            {
            logptr.Append(_L("VG_MATRIX_MODE"));
            }
            break;
        case VG_FILL_RULE:
            {
            logptr.Append(_L("VG_FILL_RULE"));
            }
            break;
        case VG_IMAGE_QUALITY:
            {
            logptr.Append(_L("VG_IMAGE_QUALITY"));
            }
            break;
        case VG_IMAGE_MODE:
            {
            logptr.Append(_L("VG_IMAGE_MODE"));
            }
            break;
        case VG_RENDERING_QUALITY:
            {
            logptr.Append(_L("VG_RENDERING_QUALITY"));
            }
            break;
        case VG_BLEND_MODE:
            {
            logptr.Append(_L("VG_BLEND_MODE"));
            }
            break;
        case VG_MASKING:
            {
            logptr.Append(_L("VG_MASKING"));
            }
            break;
        case VG_SCISSORING:
            {
            logptr.Append(_L("VG_SCISSORING"));
            }
            break;
        case VG_PIXEL_LAYOUT:
            {
            logptr.Append(_L("VG_PIXEL_LAYOUT"));
            }
            break;
        case VG_FILTER_FORMAT_LINEAR:
            {
            logptr.Append(_L("VG_FILTER_FORMAT_LINEAR"));
            }
            break;
        case VG_FILTER_FORMAT_PREMULTIPLIED:
            {
            logptr.Append(_L("VG_FILTER_FORMAT_PREMULTIPLIED"));
            }
            break;
        case VG_FILTER_CHANNEL_MASK:
            {
            logptr.Append(_L("VG_FILTER_CHANNEL_MASK"));
            }
            break;
        case VG_STROKE_CAP_STYLE:
            {
            logptr.Append(_L("VG_STROKE_CAP_STYLE"));
            }
            break;
        case VG_STROKE_JOIN_STYLE:
            {
            logptr.Append(_L("VG_STROKE_JOIN_STYLE"));
            }
            break;    
        case VG_STROKE_DASH_PHASE_RESET:
            {
            logptr.Append(_L("VG_STROKE_DASH_PHASE_RESET"));
            }
            break;
            /* Implementation limits (read-only) */
        case VG_SCREEN_LAYOUT:
            {
            logptr.Append(_L("VG_SCREEN_LAYOUT"));
            }
            break;    
        case VG_MAX_SCISSOR_RECTS:
            {
            logptr.Append(_L("VG_MAX_SCISSOR_RECTS"));
            }
            break;    
        case VG_MAX_DASH_COUNT:
            {
            logptr.Append(_L("VG_MAX_DASH_COUNT"));
            }
            break;    
        case VG_MAX_KERNEL_SIZE:
            {
            logptr.Append(_L("VG_MAX_KERNEL_SIZE"));
            }
            break;    
        case VG_MAX_SEPARABLE_KERNEL_SIZE:
            {
            logptr.Append(_L("VG_MAX_SEPARABLE_KERNEL_SIZE"));
            }
            break;    
        case VG_MAX_COLOR_RAMP_STOPS:
            {
            logptr.Append(_L("VG_MAX_COLOR_RAMP_STOPS"));
            }
            break;    
        case VG_MAX_IMAGE_WIDTH:
            {
            logptr.Append(_L("VG_MAX_IMAGE_WIDTH"));
            }
            break;    
        case VG_MAX_IMAGE_HEIGHT:
            {
            logptr.Append(_L("VG_MAX_IMAGE_HEIGHT"));
            }
            break;    
        case VG_MAX_IMAGE_PIXELS:
            {
            logptr.Append(_L("VG_MAX_IMAGE_PIXELS"));
            }
            break;    
        case VG_MAX_IMAGE_BYTES:
            {
            logptr.Append(_L("VG_MAX_IMAGE_BYTES"));
            }
            break;    
        case VG_MAX_FLOAT:
            {
            logptr.Append(_L("VG_MAX_FLOAT"));
            }
            break;    
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            {
            logptr.Append(_L("VG_MAX_GAUSSIAN_STD_DEVIATION"));
            }
            break;
            
        default:
            {
            logptr.Append(_L("INVALID PARAMTYPE"));
            }
            break;    
        };
    
    logptr.Append(_L(","));
    logptr.AppendNum(value);
    logptr.Append(_L("):size="));
    logptr.AppendNum(cmdsize);
    iLog.WriteFormat(logbuf);
    return;
    }
  
void CPseodoVGRendererImpl::LogvgSeti (VGParamType type, VGint value,TInt cmdsize)
    {
    TBufC8<70> logbuf;
    TPtr8 logptr = logbuf.Des();	 
    logptr.Append(_L("vgSeti("));	

    switch( type )
        {
        case VG_STROKE_LINE_WIDTH:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;
        case VG_STROKE_MITER_LIMIT:
            {
            logptr.Append(_L("VG_STROKE_MITER_LIMIT")); 
            }
            break;
        case VG_STROKE_DASH_PHASE:
            {
            logptr.Append(_L("VG_STROKE_DASH_PHASE")); 
            }
            break;
            
        case VG_MATRIX_MODE:
            {
            logptr.Append(_L("VG_MATRIX_MODE"));
            }
            break;
        case VG_FILL_RULE:
            {
            logptr.Append(_L("VG_FILL_RULE"));
            }
            break;
        case VG_IMAGE_QUALITY:
            {
            logptr.Append(_L("VG_IMAGE_QUALITY"));
            }
            break;
        case VG_IMAGE_MODE:
            {
            logptr.Append(_L("VG_IMAGE_MODE"));
            }
            break;
        case VG_RENDERING_QUALITY:
            {
            logptr.Append(_L("VG_RENDERING_QUALITY"));
            }
            break;
        case VG_BLEND_MODE:
            {
            logptr.Append(_L("VG_BLEND_MODE"));
            }
            break;
        case VG_MASKING:
            {
            logptr.Append(_L("VG_MASKING"));
            }
            break;
        case VG_SCISSORING:
            {
            logptr.Append(_L("VG_SCISSORING"));
            }
            break;
        case VG_PIXEL_LAYOUT:
            {
            logptr.Append(_L("VG_PIXEL_LAYOUT"));
            }
            break;
        case VG_FILTER_FORMAT_LINEAR:
            {
            logptr.Append(_L("VG_FILTER_FORMAT_LINEAR"));
            }
            break;
        case VG_FILTER_FORMAT_PREMULTIPLIED:
            {
            logptr.Append(_L("VG_FILTER_FORMAT_PREMULTIPLIED"));
            }
            break;
        case VG_FILTER_CHANNEL_MASK:
            {
            logptr.Append(_L("VG_FILTER_CHANNEL_MASK"));
            }
            break;
        case VG_STROKE_CAP_STYLE:
            {
            logptr.Append(_L("VG_STROKE_CAP_STYLE"));
            }
            break;
        case VG_STROKE_JOIN_STYLE:
            {
            logptr.Append(_L("VG_STROKE_JOIN_STYLE"));
            }
            break;    
        case VG_STROKE_DASH_PHASE_RESET:
            {
            logptr.Append(_L("VG_STROKE_DASH_PHASE_RESET"));
            }
            break;
            /* Implementation limits (read-only) */
        case VG_SCREEN_LAYOUT:
            {
            logptr.Append(_L("VG_SCREEN_LAYOUT"));
            }
            break;    
        case VG_MAX_SCISSOR_RECTS:
            {
            logptr.Append(_L("VG_MAX_SCISSOR_RECTS"));
            }
            break;    
        case VG_MAX_DASH_COUNT:
            {
            logptr.Append(_L("VG_MAX_DASH_COUNT"));
            }
            break;    
        case VG_MAX_KERNEL_SIZE:
            {
            logptr.Append(_L("VG_MAX_KERNEL_SIZE"));
            }
            break;    
        case VG_MAX_SEPARABLE_KERNEL_SIZE:
            {
            logptr.Append(_L("VG_MAX_SEPARABLE_KERNEL_SIZE"));
            }
            break;    
        case VG_MAX_COLOR_RAMP_STOPS:
            {
            logptr.Append(_L("VG_MAX_COLOR_RAMP_STOPS"));
            }
            break;    
        case VG_MAX_IMAGE_WIDTH:
            {
            logptr.Append(_L("VG_MAX_IMAGE_WIDTH"));
            }
            break;    
        case VG_MAX_IMAGE_HEIGHT:
            {
            logptr.Append(_L("VG_MAX_IMAGE_HEIGHT"));
            }
            break;    
        case VG_MAX_IMAGE_PIXELS:
            {
            logptr.Append(_L("VG_MAX_IMAGE_PIXELS"));
            }
            break;    
        case VG_MAX_IMAGE_BYTES:
            {
            logptr.Append(_L("VG_MAX_IMAGE_BYTES"));
            }
            break;    
        case VG_MAX_FLOAT:
            {
            logptr.Append(_L("VG_MAX_FLOAT"));
            }
            break;    
        case VG_MAX_GAUSSIAN_STD_DEVIATION:
            {
            logptr.Append(_L("VG_MAX_GAUSSIAN_STD_DEVIATION"));
            }
            break;
            
        default:
            {
            logptr.Append(_L("INVALID PARAMTYPE"));
            }
            break;    
        };

    logptr.Append(_L(","));
    switch(value)    
        {
        case VG_RENDERING_QUALITY_NONANTIALIASED:
            {
            logptr.Append(_L("VG_RENDERING_QUALITY_NONANTIALIASED"));
            }
            break;
        case VG_RENDERING_QUALITY_FASTER:
            {
            logptr.Append(_L("VG_RENDERING_QUALITY_FASTER"));
            }
            break;
        case VG_RENDERING_QUALITY_BETTER:
            {
            logptr.Append(_L("VG_RENDERING_QUALITY_BETTER"));
            }
            break;
        case VG_MATRIX_PATH_USER_TO_SURFACE:
            {
            logptr.Append(_L("VG_MATRIX_PATH_USER_TO_SURFACE"));
            }
            break;    
        case VG_MATRIX_IMAGE_USER_TO_SURFACE:
            {
            logptr.Append(_L("VG_MATRIX_IMAGE_USER_TO_SURFACE"));
            }
            break;  
        case VG_MATRIX_FILL_PAINT_TO_USER :
            {
            logptr.Append(_L("VG_MATRIX_FILL_PAINT_TO_USER"));
            }
            break;  
        case VG_MATRIX_STROKE_PAINT_TO_USER:
            {
            logptr.Append(_L("VG_MATRIX_STROKE_PAINT_TO_USER"));
            }
            break;
        case VG_CAP_BUTT:
            {
            logptr.Append(_L("VG_CAP_BUTT"));
            }
            break;    
        case VG_CAP_ROUND:
            {
            logptr.Append(_L("VG_CAP_ROUND"));
            }
            break;  
        case VG_CAP_SQUARE:
            {
            logptr.Append(_L("VG_CAP_SQUARE"));
            }
            break;  
        case VG_BLEND_SRC:
            {
            logptr.Append(_L("VG_BLEND_SRC"));
            }
            break;    
        case VG_BLEND_SRC_OVER:
            {
            logptr.Append(_L("VG_BLEND_SRC_OVER"));
            }
            break;  
        case VG_BLEND_DST_OVER:
            {
            logptr.Append(_L("VG_BLEND_DST_OVER"));
            }
            break;    
        case VG_BLEND_SRC_IN:
            {
            logptr.Append(_L("VG_BLEND_SRC_IN"));
            }
            break;  
        case VG_BLEND_DST_IN:
            {
            logptr.Append(_L("VG_BLEND_DST_IN"));
            }
            break;  
        case VG_BLEND_MULTIPLY:
            {
            logptr.Append(_L("VG_BLEND_MULTIPLY"));
            }
            break;  
        case VG_BLEND_SCREEN:
            {
            logptr.Append(_L("VG_BLEND_SCREEN"));
            }
            break;  
        case VG_BLEND_DARKEN:
            {
            logptr.Append(_L("VG_BLEND_DARKEN"));
            }
            break;  
        case VG_BLEND_LIGHTEN:
            {
            logptr.Append(_L("VG_BLEND_LIGHTEN"));
            }
            break;  
        case VG_BLEND_ADDITIVE:
            {
            logptr.Append(_L("VG_BLEND_ADDITIVE"));
            }
            break;  
        case VG_IMAGE_QUALITY_NONANTIALIASED:
            {
            logptr.Append(_L("VG_IMAGE_QUALITY_NONANTIALIASED"));
            }
            break;    
        case VG_IMAGE_QUALITY_FASTER:
            {
            logptr.Append(_L("VG_IMAGE_QUALITY_FASTER"));
            }
            break;  
        case VG_IMAGE_QUALITY_BETTER:
            {
            logptr.Append(_L("VG_IMAGE_QUALITY_BETTER"));
            }
            break;  
        case VG_FALSE:
            {
            logptr.Append(_L("VG_FALSE"));
            }
            break;    
        case VG_RED:
            {
            logptr.Append(_L("VG_RED"));
            }
            break;    
            
        case VG_DRAW_IMAGE_NORMAL:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_NORMAL"));
            }
            break;    
        case VG_DRAW_IMAGE_MULTIPLY:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_MULTIPLY"));
            }
            break;  
        case VG_DRAW_IMAGE_STENCIL:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_STENCIL"));
            }
            break;  
        case VG_JOIN_MITER:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_STENCIL"));
            }
            break;      
        case VG_JOIN_ROUND:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_STENCIL"));
            }
            break;      
        case VG_JOIN_BEVEL:
            {
            logptr.Append(_L("VG_DRAW_IMAGE_STENCIL"));
            }
            break;      
        default:
            {
            logptr.AppendNum(value);
            }     
            break;
        };
    
    logptr.Append(_L("):size="));
    logptr.AppendNum(cmdsize);
    iLog.WriteFormat(logbuf);
    return;
    }
  
void CPseodoVGRendererImpl::LogvgSetParameteri(VGHandle handle, VGint paramType, VGint value,TInt cmdsize,TInt Lpvalue)
    {
    TBufC8<90> logbuf;
    TPtr8 logptr = logbuf.Des();
    logptr.Append(_L("vgsetparameteri("));
    logptr.AppendNum(handle);
    logptr.Append(_L(","));

    switch(paramType)
        {
        case VG_PAINT_TYPE:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;  
        case VG_PAINT_COLOR:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_COLOR_RAMP_STOPS:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
            
            /* Linear gradient paint parameters */
        case VG_PAINT_LINEAR_GRADIENT:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;  
            /* Radial gradient paint parameters */
        case VG_PAINT_RADIAL_GRADIENT:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;  
            /* Pattern paint parameters */
        case VG_PAINT_PATTERN_TILING_MODE:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;  
        default:
            {
            logptr.AppendNum(paramType);
            }
            break;    			
        };
    logptr.Append(_L(","));        

    switch(value)	    
        {
        case VG_PAINT_TYPE_COLOR:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_TYPE_LINEAR_GRADIENT:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_TYPE_RADIAL_GRADIENT:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        case VG_PAINT_TYPE_PATTERN:
            {
            logptr.Append(_L("VG_STROKE_LINE_WIDTH"));
            }
            break;    
        default:	                
            {
            logptr.AppendNum(value);
            }
            break;    
        };
    logptr.Append(_L("):size="));
    logptr.AppendNum(cmdsize);
    logptr.Append(_L(":hnum="));	    
    logptr.AppendNum(Lpvalue);
    iLog.WriteFormat(logbuf);
    return;
    }
  
void CPseodoVGRendererImpl::LogvgSetPaint(VGPaint paint, VGbitfield paintModes,TInt cmdsize,TInt Lpvalue)
    {   
    TBufC8<50> logbuf;
    TPtr8 logptr = logbuf.Des();	 
    logptr.Append(_L("vgSetPaint("));
    logptr.AppendNum(paint);
    logptr.Append(_L(","));
    
    switch(paintModes)
        {
        case VG_STROKE_PATH:                            
            {
            logptr.Append(_L("VG_STROKE_PATH"));
            }
            break;
        case VG_FILL_PATH:                              
            {
            logptr.Append(_L("VG_FILL_PATH"));
            }
            break;
        default: 
            {
            logptr.AppendNum(paintModes);
            }	        
            break;
        };
    logptr.Append(_L("):size="));	    
    logptr.AppendNum(cmdsize);
    logptr.Append(_L(":hnum="));	    
    logptr.AppendNum(Lpvalue);
    iLog.WriteFormat(logbuf);
    return;
    }

void CPseodoVGRendererImpl::LogvgDrawPath(VGbitfield paintModes,int cmdsize)
    {
    TBufC8<50> logbuf;
    TPtr8 logptr = logbuf.Des();	 
    logptr.Append(_L("vgDrawPath("));
    switch(paintModes)
        {
        case VG_STROKE_PATH:                            
            {
            logptr.Append(_L("VG_STROKE_PATH"));
            }
            break;
        case VG_FILL_PATH:                              
            {
            logptr.Append(_L("VG_FILL_PATH"));
            }
            break;
        case 3:
            {
            logptr.Append(_L("FILL & STROKE"));
            }	 
            break;       
        default: 
            {
            logptr.AppendNum(paintModes);
            }	        
            break;
        };
    logptr.Append(_L("):size="));	    
    logptr.AppendNum(cmdsize);	    
    iLog.WriteFormat(logbuf);
    return;
    }    
#endif


