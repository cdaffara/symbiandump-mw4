/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation for HuiUtil class, a collection of utility 
*                routines for HUITK.
*
*/



#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiRealPoint.h"

#include <e32math.h>
#include <hal.h>
#include <AknUtils.h>
#include <AknFontSpecification.h> 
#include <AknFontAccess.h> 
#include <centralrepository.h>
#include "uiacceltk/HuiDisplay.h"
#include "../../CommonInc/uiacceltkdomaincrkeys.h"

const TReal32 KLengthUnitDivisor = 320.0;

/// Character used for separating tags in a tag descriptor.
const TInt KTagSeparator = ':';


// Calculates the smallest power-of-two that is equal to or greater than
// a value.
EXPORT_C TInt HuiUtil::Power2(TInt aValue)
    {
    ASSERT(aValue>0);
    if(aValue<=0)
        {
        return 0;
        }

    TInt i;
    for(i = 1; i < aValue && i < KMaxTInt/2; i *= 2)
        {}
    return i;
    }

EXPORT_C TInt HuiUtil::Power2RoundDown(TInt aValue)
    {
    ASSERT(aValue>0);
    if(aValue <= 0)
        {
        return 0;
        }

    TInt i = 1;
    for(; (i * 4 <= aValue) && (i < KMaxTInt/4); i *= 4)
        {}
    for(; (i * 2 <= aValue) && (i < KMaxTInt/2); i *= 2)
        {}
    return i;
    }


EXPORT_C TReal32 HuiUtil::Interpolate(TReal32 aPos, TReal32 aMin, TReal32 aMax) __SOFTFP
    {
    aPos = Max(0.f, aPos);
    aPos = Min(aPos, 1.f);
    return (1.f - aPos) * aMin + aPos * aMax;
    }


EXPORT_C void HuiUtil::WrapValue(TReal32& aValue, TReal32 aLow, TReal32 aHigh) __SOFTFP 
    {
    TReal32 segments = 0;
    TReal32 length = aHigh - aLow;

    if(length <= 0)
        {
        aValue = aLow;
        return;
        }
    
    // check rounding errors for low limit    
    if ( HuiUtil::RealCompare( aValue, aLow ) )
        {
        aValue = aLow;
        return;
        }
       
    // check rounding errors for high limit  
    if ( HuiUtil::RealCompare( aValue, aHigh ) )
        {
        aValue = aHigh;
        return;
        }

    if(aValue < aLow)
        {
        // Wrap from below.
        segments = (aLow - aValue) / length;
        aValue += (TInt(segments) + 1) * length;
        }
    else if(aValue >= aHigh)
        {
        // Wrap from above.
        segments = (aValue - aHigh) / length;
        aValue -= (TInt(segments) + 1) * length;
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C TInt HuiUtil::RandomInt(TInt aMin, TInt aMax)
    {
    TUint32 random = Math::Random();
    TUint range = aMax - aMin;
    if(range > 0)
        {
        return aMin + (random % (range + 1));
        }
    else
        {
        return aMin;
        }
    }


EXPORT_C TReal32 HuiUtil::RandomReal(TReal32 aMin, TReal32 aMax) __SOFTFP 
    {
    /** @todo  Could use Math::FRand(). */
    TReal32 random = RandomInt(0, 10000000) / 10000000.f;
    return aMin + (aMax - aMin) * random;
    }


EXPORT_C TUint HuiUtil::FreeMemory(TUint* aTotalMemory)
    {
    TInt total = 0;
    TInt free = 0;

    HAL::Get(HALData::EMemoryRAM, total);
    HAL::Get(HALData::EMemoryRAMFree, free);
    if(aTotalMemory)
        {
        *aTotalMemory = total;
        }
    return free;
    }


EXPORT_C TSize HuiUtil::ScreenSize()
    {
    TSize screenSize(320, 240);

    if ( CCoeEnv::Static())
        {
        AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EScreen, screenSize);
        }
    else
        {
        screenSize = CHuiStatic::ScreenDevice()->SizeInPixels();
        }
    
    return screenSize;
    }


EXPORT_C TReal32 HuiUtil::LengthUnit() __SOFTFP
    {
    return Max(ScreenSize().iWidth, ScreenSize().iHeight) / KLengthUnitDivisor;
    }


EXPORT_C TReal32 HuiUtil::QuickLength(THuiRealPoint& aVector) __SOFTFP
    {
    TReal32 dx = Abs(aVector.iX);
    TReal32 dy = Abs(aVector.iY);
    if(dx < dy)
        {
        return dx + dy - dx/2;
        }
    else
        {
        return dx + dy - dy/2;
        }
    }


EXPORT_C TReal32 HuiUtil::QuickLength(TReal32 aDx, TReal32 aDy) __SOFTFP
    {
    TReal32 dx = Abs(aDx);
    TReal32 dy = Abs(aDy);
    if(dx < dy)
        {
        return dx + dy - dx/2;
        }
    else
        {
        return dx + dy - dy/2;
        }
    }


EXPORT_C void HuiUtil::QuickNormalize(THuiRealPoint& aNormal)
    {
    TReal32 approxLength = QuickLength(aNormal);

    if(approxLength > 0)
        {
        aNormal.iX /= approxLength;
        aNormal.iY /= approxLength;
        }
    }


EXPORT_C void HuiUtil::QuickNormalize(TReal32 aVector[3])
    {
    TReal32 approxLength = QuickLength(QuickLength(aVector[0], aVector[1]), aVector[2]);

    if(approxLength > 0)
        {
        aVector[0] /= approxLength;
        aVector[1] /= approxLength;
        aVector[2] /= approxLength;
        }
    }


EXPORT_C void HuiUtil::CrossProduct(const TReal32 aA[3], const TReal32 aB[3],
                                    TReal32 aProduct[3])
    {
    aProduct[0] = aA[1] * aB[2] - aB[1] * aA[2];
    aProduct[1] = aA[2] * aB[0] - aB[2] * aA[0];
    aProduct[2] = aA[0] * aB[1] - aB[0] * aA[1];
    }


EXPORT_C void HuiUtil::NormalFromPoints(const TReal32 aPoints[3][3], TReal32 aNormal[3])
    {
    TReal32 vectors[2][3];
    TInt i;

    for(i = 0; i < 3; ++i)
        {
        vectors[0][i] = aPoints[0][i] - aPoints[1][i];
        vectors[1][i] = aPoints[0][i] - aPoints[2][i];
        }

    CrossProduct(vectors[0], vectors[1], aNormal);
    QuickNormalize(aNormal);
    }


EXPORT_C void HuiUtil::ShadowMatrix(const TReal32 aPlanePoint[3],
                                    const TReal32 aPlaneNormal[3],
                                    const TReal32 aLightPos[4],
                                    TReal32 aDestMat[16])
    {
    TReal32 planeCoeff[4];
    TReal32 dot;

    // Find the plane equation coefficients
    // Find the first three coefficients the same way we find a normal.
    //NormalFromPoints(aPoints, planeCoeff);

    planeCoeff[0] = aPlaneNormal[0];
    planeCoeff[1] = aPlaneNormal[1];
    planeCoeff[2] = aPlaneNormal[2];

    // Find the last coefficient by back substitutions
    planeCoeff[3] = - ((planeCoeff[0] * aPlanePoint[0]) + (planeCoeff[1] * aPlanePoint[1]) +
                       (planeCoeff[2] * aPlanePoint[2]));

    // Dot product of plane and light position
    dot = planeCoeff[0] * aLightPos[0] + planeCoeff[1] * aLightPos[1] +
          planeCoeff[2] * aLightPos[2] + planeCoeff[3] * aLightPos[3];

    // Now do the projection
    // First column
    aDestMat[0] = dot - aLightPos[0] * planeCoeff[0];
    aDestMat[4] = 0.0f - aLightPos[0] * planeCoeff[1];
    aDestMat[8] = 0.0f - aLightPos[0] * planeCoeff[2];
    aDestMat[12] = 0.0f - aLightPos[0] * planeCoeff[3];

    // Second column
    aDestMat[1] = 0.0f - aLightPos[1] * planeCoeff[0];
    aDestMat[5] = dot - aLightPos[1] * planeCoeff[1];
    aDestMat[9] = 0.0f - aLightPos[1] * planeCoeff[2];
    aDestMat[13] = 0.0f - aLightPos[1] * planeCoeff[3];

    // Third Column
    aDestMat[2] = 0.0f - aLightPos[2] * planeCoeff[0];
    aDestMat[6] = 0.0f - aLightPos[2] * planeCoeff[1];
    aDestMat[10] = dot - aLightPos[2] * planeCoeff[2];
    aDestMat[14] = 0.0f - aLightPos[2] * planeCoeff[3];

    // Fourth Column
    aDestMat[3] = 0.0f - aLightPos[3] * planeCoeff[0];
    aDestMat[7] = 0.0f - aLightPos[3] * planeCoeff[1];
    aDestMat[11] = 0.0f - aLightPos[3] * planeCoeff[2];
    aDestMat[15] = dot - aLightPos[3] * planeCoeff[3];
    }


EXPORT_C TReal32 HuiUtil::ColorLightness(const TRgb& aColor) __SOFTFP
    {
    TReal32 red = aColor.Red() / 255.0f;
    TReal32 green = aColor.Red() / 255.0f;
    TReal32 blue = aColor.Red() / 255.0f;

    return (red*2 + green*3 + blue) / 6.f;
    }

EXPORT_C void HuiUtil::ScaleFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                       CFbsBitmap & aScaledBitmap)
    {
    CFbsDevice* targetdevice = NULL;
    CFbsBitGc* gc = NULL;
    // create device for drawing onto the target cropped bitmap area
    targetdevice = CFbsBitmapDevice::NewL(&aScaledBitmap);
    CleanupStack::PushL(targetdevice);
    // create graphics context for drawing
    User::LeaveIfError(targetdevice->CreateContext(gc));
    // Perform downscale using DrawBitmap
    gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);    
    gc->DrawBitmap(TRect(TPoint(0,0), aScaledBitmap.SizeInPixels()),
                   (const CFbsBitmap *)&aSrcBitmap);
    delete gc;
    CleanupStack::PopAndDestroy(targetdevice);
    }

EXPORT_C void HuiUtil::CombineMaskFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                             const CFbsBitmap & aSrcMaskBitmap,
                                             CFbsBitmap & aCombinedBitmap)
    {

    ASSERT(aCombinedBitmap.DisplayMode() == EColor16MA);
    ASSERT(aSrcMaskBitmap.DisplayMode() == EGray2 || aSrcMaskBitmap.DisplayMode() == EGray256 || aSrcMaskBitmap.DisplayMode() == EGray16 || aSrcMaskBitmap.DisplayMode() == EGray4);
    // Resize the target bitmap if needed
    if (aSrcBitmap.SizeInPixels() != aCombinedBitmap.SizeInPixels())
        {
        aCombinedBitmap.Resize(aSrcBitmap.SizeInPixels());
        }
/*
    CFbsDevice* targetdevice = NULL;
    CFbsBitGc* gc = NULL;


    // create device for drawing onto the target cropped bitmap area
    targetdevice = CFbsBitmapDevice::NewL(&aCombinedBitmap);
    CleanupStack::PushL(targetdevice);
    // create graphics context for drawing
    User::LeaveIfError(targetdevice->CreateContext(gc));

    // Make the target bitmap fully transparent
    gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    TRgb blank(KRgbWhite);
    blank.SetAlpha(255);
    gc->SetBrushColor(blank);
    gc->SetPenColor(blank);
    gc->Clear();
    gc->SetDrawMode(CGraphicsContext::EDrawModePEN);

    gc->BitBltMasked(TPoint(0,0),  // target pos
                     &aSrcBitmap,  // source bitmap
                     TRect(TPoint(0, 0),aSrcBitmap.SizeInPixels()), // source rect
                     &aSrcMaskBitmap,
                     EFalse);

    delete gc;
    CleanupStack::PopAndDestroy(targetdevice);
*/
    // Alternative method to blend manually (SLOW!!):
    // Apply the alpha mask.
    TBitmapUtil color((CFbsBitmap*)&aSrcBitmap);
    TBitmapUtil alpha((CFbsBitmap*)&aSrcMaskBitmap);
    TBitmapUtil target((CFbsBitmap*)&aCombinedBitmap);
    color.Begin(TPoint(0, 0));
    alpha.Begin(TPoint(0, 0));
    target.Begin(TPoint(0, 0));
    TSize size(aCombinedBitmap.SizeInPixels());
    for(TInt y = 0; y < size.iHeight; ++y)
        {
        alpha.SetPos(TPoint(0, y));
        color.SetPos(TPoint(0, y));
        target.SetPos(TPoint(0, y));
        for(TInt x = 0; x < size.iWidth; ++x)
            {
            target.SetPixel((color.GetPixel() & 0xffffff)
                          | ((alpha.GetPixel() & 0xff) << 24));
            target.IncXPos();
            color.IncXPos();
            alpha.IncXPos();
            }
        }
    target.End();
    color.End();
    alpha.End();

    }

EXPORT_C void HuiUtil::CropFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                    CFbsBitmap & aCroppedBitmap,
                                    TPoint aCropPosition)
    {
    CFbsDevice* targetdevice;
    CFbsBitGc* gc;
    // create device for drawing onto the target cropped bitmap area
    targetdevice = CFbsBitmapDevice::NewL(&aCroppedBitmap);
    CleanupStack::PushL(targetdevice);
    // create graphics context for drawing
    User::LeaveIfError(targetdevice->CreateContext(gc));
    // Perform cropping bitblit
    gc->BitBlt(TPoint(0,0), &aSrcBitmap,
               TRect(aCropPosition, aCroppedBitmap.SizeInPixels()));
    delete gc;
    CleanupStack::PopAndDestroy(targetdevice);
    }

EXPORT_C void HuiUtil::ScaleImage(TInt aComponents,
                                  const TSize& aSrcSize,
                                  const TUint8* aSrcBuffer,
                                  const TSize& aDestSize,
                                  TUint8* aDestBuffer)
    {
    // TODO: if there is actual use for this routine,
    // there might be better minification filters than bilinear...
    // anyway, now this routine produced acceptable results
    // when magnifying also...
    ASSERT (aDestBuffer && aSrcBuffer);
    ASSERT (aSrcSize.iWidth > 0 && aSrcSize.iHeight > 0);
    ASSERT (aDestSize.iWidth > 0 && aDestSize.iHeight > 0);
    ASSERT (aComponents > 0 && aComponents < 5);

    TUint32 xScale = ((aSrcSize.iWidth-1) << 16) / aDestSize.iWidth;
    TUint32 yScale = ((aSrcSize.iHeight-1) << 16) / aDestSize.iHeight;
    TUint32 height = aDestSize.iHeight;
    TUint8* srcptr = const_cast<TUint8*>(aSrcBuffer);
    TUint8* destPtrLimit = aDestBuffer+(aDestSize.iWidth*aComponents);
    TUint32 y = yScale&0xffff;
    do 
        {
        TUint32 fV = y&0xffff;
        TUint32 x = xScale&0xffff;
        while(aDestBuffer < destPtrLimit)
            {

            TUint32 fU = x&0xffff;
            for (TInt components = 0; components < aComponents; components++)
                {
                TUint32 componenta = srcptr[((x>>16)*aComponents)+components];
                TUint32 componentb = srcptr[((x>>16)*aComponents)+aComponents+components];
                TUint32 componentc = srcptr[((x>>16)*aComponents)+(aSrcSize.iWidth*aComponents)+components];
                TUint32 componentd = srcptr[((x>>16)*aComponents)+(aSrcSize.iWidth*aComponents)+aComponents+components];

                TUint32 componentf1 = (componenta+(((fU*((componentb-componenta)))>>16))) & 0xff;
                TUint32 componentf2 = (componentc+(((fU*((componentd-componentc)))>>16))) & 0xff;
                TUint32 finalcomponent = (componentf1+(((fV*((componentf2-componentf1)))>>16))) & 0xff;
                *aDestBuffer++ = (TUint8)finalcomponent;
                }
            x+=xScale;
            }
        y+=yScale;
        srcptr = const_cast<TUint8*>(aSrcBuffer)+((y>>16)*(aSrcSize.iWidth*aComponents));
        destPtrLimit+=aDestSize.iWidth*aComponents;
        }
    while (--height);
    }
/*
EXPORT_C void HuiUtil::ConvertFbsBitmap(const CFbsBitmap* aSrcBitmap,
                                  const CFbsBitmap* aSrcMaskBitmap,
                                  const TSize& aDestSize,
                                  TUint8* aDestBuffer)
    {

    TReal32 weight = 1.0;
    TReal32 totals[4] =
        {
        0.0, 0.0, 0.0, 0.0
        };
    TReal32 area;
    TInt  destY,destX;
    TInt  outindex;

    TInt  startX = 0, endX = 0, startY = 0, endY = 0;
    TReal32 leftOffset, rightOffset, topOffset, bottomOffset;
    TInt  convKernelSizeY,     convKernelSizeX;
    TReal32 convKernelSizeYFrac, convKernelSizeXFrac;
    TInt  x, y;

    ASSERT (aDestBuffer && aSrcBitmap);

    TSize aSrcSize = aSrcBitmap->SizeInPixels();
    TInt aComponents = (aSrcMaskBitmap != NULL) ? 4 : 3;

    ASSERT (aSrcSize.iWidth > 0 && aSrcSize.iHeight > 0);
    ASSERT (aDestSize.iWidth > 0 && aDestSize.iHeight > 0);
    ASSERT (aComponents > 0 && aComponents < 5);
    // Max aComponents in a format is 4, so...

    // NOTE: here we have to force the const bitmap to
    // non-const, since the iterator does not provide
    // a const version. However this should not be a problem
    // since the operations involved in this method are
    // strictly read-only.
    TBitmapUtil srcIterator((CFbsBitmap*)aSrcBitmap);
    TBitmapUtil srcMaskIterator((CFbsBitmap*)aSrcMaskBitmap);


    // if the source and destination sizes match,
    // just perform a direct copy
    if(aSrcSize == aDestSize)
        {
        TUint8* output = aDestBuffer;
        srcIterator.Begin(TPoint(0, 0));
        for(TInt y = 0; y < aDestSize.iHeight; ++y)
            {
            srcIterator.SetPos(TPoint(0,y));
            TUint8* output = aDestBuffer + aComponents*aDestSize.iWidth*y;
            if(aComponents == 3)
                {
                for(TInt x = 0; x < aDestSize.iWidth; x++, srcIterator.IncXPos())
                    {
                    TUint32 pixel = srcIterator.GetPixel();
                    *output++ = (pixel >> 16) & 0xff;
                    *output++ = (pixel >> 8) & 0xff;
                    *output++ = pixel & 0xff;
                    }
                }
            else
                {
                // components == 4 and alpha channel must be excluded..
                for(TInt x = 0; x < aDestSize.iWidth; x++, srcIterator.IncXPos())
                    {
                    TUint32 pixel = srcIterator.GetPixel();
                    *output++ = (pixel >> 16) & 0xff;
                    *output++ = (pixel >> 8) & 0xff;
                    *output++ = pixel & 0xff;
                    // skip alpha (expect the alpha value from the mask bitmap..
                    output++;
                    // alpha is not really supported by the
                    // scaling algorithm below. some small changes needed to implement..
                    // *output++ = pixel.Alpha();
                    }
                }

            }
        srcIterator.End();

        // then integrate the alpha channel, if
        // provided
        if(aSrcMaskBitmap!=NULL)
            {
            srcMaskIterator.Begin(TPoint(0,0));
            for(TInt y = 0; y < aDestSize.iHeight; ++y)
                {
                srcMaskIterator.SetPos(TPoint(0,y));
                TUint8* output = aDestBuffer + aComponents*aDestSize.iWidth*y + 3;
                for(TInt x = 0;
                    x < aDestSize.iWidth;
                    x++, srcMaskIterator.IncXPos(), output += aComponents)
                    {
                    // The mask bitmap is in EGray256 format, which means the return
                    // values from TBitmapUtil::GetPixel() should be in range 0...255.
                    *output = srcMaskIterator.GetPixel() & 0xff;
                    }
                }
            srcMaskIterator.End();
            }
        return;
        }



    /// @todo ScaleImaage may be optimized for halved cases, which is very common!

    // if (widthin == widthout*2 && heightin == heightout*2) {
    // halveImage_ubyte(aComponents, widthin, heightin,
    // (const GLubyte *)aSrcBuffer, (GLubyte *)dataout,
    // element_size, ysize, aComponents);
    // return;
    // }

    TReal32 convy = (TReal32)aSrcSize.iHeight/aDestSize.iHeight;
    TReal32 convx = (TReal32)aSrcSize.iWidth/aDestSize.iWidth;
    convKernelSizeY = (TInt)convy;
    convKernelSizeYFrac = convy - convKernelSizeY;
    convKernelSizeX = (TInt)convx;
    convKernelSizeXFrac = convx - convKernelSizeX;

    area = convx * convy;

    startY = 0;
    topOffset = 0;
    endY = convKernelSizeY;
    bottomOffset = convKernelSizeYFrac;

    TRgb pixel;
    // lock the bitmaps for fast access
    srcIterator.Begin(TPoint(0,0));
    if(aSrcMaskBitmap!=NULL)
        {
        srcMaskIterator.Begin(TPoint(0,0));
        }

// define a helper macros for conveniently
// accessing and summing pixels in a cfbsbitmap
#define ADD_SINGLE_PIXEL_TOTALS(x, y, mul) \
aSrcBitmap->GetPixel(pixel, TPoint(x, y)); \
totals[0] += pixel.Red() * mul; \
totals[1] += pixel.Green() * mul; \
totals[2] += pixel.Blue() * mul; \
if(aSrcMaskBitmap!=NULL) \
    { \
    aSrcMaskBitmap->GetPixel(pixel, TPoint(x, y)); \
    totals[3] += pixel.Gray256() * mul; \
    }

// add totals with an iterator (rgb channels)
#define ITER_ADD_PIXEL_TOTALS_RGB(mul) \
    pixel = TRgb(srcIterator.GetPixel()); \
    totals[0] += pixel.Red() * mul; \
    totals[1] += pixel.Green() * mul; \
    totals[2] += pixel.Blue() * mul;

// add totals with an iterator (alpha channel)
#define ITER_ADD_PIXEL_TOTALS_A(mul) \
    totals[3] += TRgb(srcMaskIterator.GetPixel()).Gray256() * mul;

// add totals for a whole row
#define ADD_ROW_TOTALS_RGBA(startX, startY, endX, weight) \
    srcIterator.SetPos(TPoint(startX,startY));
    for(x = startX; x < endX; x++, srcIterator.IncXPos())
        {
        ITER_ADD_PIXEL_TOTALS_RGB(weight);
        }
    if (aSrcMaskBitmap!=NULL) {
    srcMaskIterator.SetPos(TPoint(startX,startY));
    for(x = startX; x < endX; x++, srcMaskIterator.IncXPos())
        {
        ITER_ADD_PIXEL_TOTALS_A(weight);
        }
    }
// add totals for a whole column
#define ADD_COLUMN_TOTALS_RGBA(startY, startX, endY, weight) \
    srcIterator.SetPos(TPoint(startX,startY));
    for(y = startY; y < endY; y++, srcIterator.IncYPos())
        {
        ITER_ADD_PIXEL_TOTALS_RGB(weight);
        }
    if (aSrcMaskBitmap!=NULL) {
    srcMaskIterator.SetPos(TPoint(startX,startY));
    for(y = startY; y < endY; y++, srcMaskIterator.IncYPos())
        {
        ITER_ADD_PIXEL_TOTALS_A(weight);
        }
    }



    for (destY = 0; destY < aDestSize.iHeight; destY++)
        {
        startX = 0;
        leftOffset = 0;
        endX = convKernelSizeX;
        rightOffset = convKernelSizeXFrac;
        // ---- rowSizeBytes = aSrcSize.iWidth*aComponents; // actually: groups_per_line * aComponents

        for (destX = 0; destX < aDestSize.iWidth; destX++)
            {

            // Ok, now apply box filter to box that goes from (lowx, lowy)
            // to (highx, highy) on input data into this pixel on output
            // data.
            totals[0] = totals[1] = totals[2] = totals[3] = 0.0;

            // the usual case for minification:
            // downscale for both dimensions
            if((endY>startY) && (endX>startX))
                {
                // calculate the value for pixels in the top row..

                // first pixel with partial contribution both from left and from the top
                weight = (1-topOffset) * (1-leftOffset);
                // sums pixel to totals
                ADD_SINGLE_PIXEL_TOTALS(startX, startY, weight);

                // first row with y weighting (partial contribution from the top)
                weight = (1-topOffset);
                ADD_ROW_TOTALS_RGBA((startX+1), startY, endX, weight);

                // last pixel with contribution from the top and the right
                weight = (1-topOffset) * rightOffset;
                ADD_SINGLE_PIXEL_TOTALS(endX, startY, weight);

                // calculate the value for pixels in the last row ...

                // bottom-left corner pixel
                // first pixel: fractional weight contribution from the left and below
                weight = bottomOffset * (1-leftOffset);
                ADD_SINGLE_PIXEL_TOTALS(startX, endY, weight);

                // bottom row pixels
                weight = bottomOffset;
                ADD_ROW_TOTALS_RGBA(startX+1, endY, endX, weight);

                // bottom-right corner pixel
                weight = bottomOffset * rightOffset;
                ADD_SINGLE_PIXEL_TOTALS(endX, endY, weight);

                // calculate the value for pixels at the left and the right
                // edges..
                ADD_COLUMN_TOTALS_RGBA(startY+1, endY, startX, (1-leftOffset));
                ADD_COLUMN_TOTALS_RGBA(startY+1, endY, (endX-1), rightOffset);

                }
            else if (endY > startY)  // only squeeze in y direction?
                {
                // we just need to handle the left edge pixels

                // top-left corner pixel first
                weight = (1-topOffset)*(rightOffset - leftOffset);
                // ---- srcBufferCurrentPos = (const TUint8*)aSrcBuffer + srcXPositionInBytes + startY*rowSizeBytes;
                ADD_SINGLE_PIXEL_TOTALS(startX, startY, weight);

                // left edge
                weight = rightOffset - leftOffset;
                ADD_COLUMN_TOTALS_RGBA((startY+1), endY, startX, weight);

                // bottom-left corner pixel has two affecting weights: left edge weight and the bottom weight
                weight = (rightOffset - leftOffset) * bottomOffset;
                ADD_SINGLE_PIXEL_TOTALS(startX, endY, weight);
                }
            else if (endX > startX) // only squeeze in x direction?
                {

                // we just need to handle the top edge pixels

                // top-left corner pixel first
                weight = (1-leftOffset)*(bottomOffset - topOffset);
                // sums pixel to totals
                ADD_SINGLE_PIXEL_TOTALS(startX, startY, weight);

                // top edge pixels
                weight = bottomOffset - topOffset;
                ADD_ROW_TOTALS_RGBA((startX+1), startY, endX, weight);

                // top-right edge pixel
                weight = (bottomOffset - topOffset) * rightOffset;
                ADD_SINGLE_PIXEL_TOTALS(endX, startY, weight);
                }
            else // only single pixel? (the top-right pixel)
                {
                // this pixel is at the all corners of the area, so
                // all edge offsets affect to its weight
                weight = (bottomOffset-topOffset)*(rightOffset-leftOffset);
                ADD_SINGLE_PIXEL_TOTALS(startX, startY, weight);
                }

            // this is for the pixels in the body
            for (y = startY+1; y < endY; y++)
                {
                ADD_ROW_TOTALS_RGBA((startX+1), y, endX, 1.0f);
                }

            outindex = (destX + (destY * aDestSize.iWidth)) * aComponents;
            for (TInt component = 0; component < aComponents; component++)
                {
                aDestBuffer[outindex + component] = totals[component]/area;

                }
            // prepare new block in x direction to be filtered
            startX = endX;
            leftOffset = rightOffset;
            endX += convKernelSizeX;
            rightOffset += convKernelSizeXFrac;
            if(rightOffset > 1)
                {
                rightOffset -= 1.0;
                endX++;
                }
            } // for startX

        // prepare new row of blocks to be filtered
        startY = endY;
        topOffset = bottomOffset;
        endY += convKernelSizeY;
        bottomOffset += convKernelSizeYFrac;
        if(bottomOffset > 1)
            {
            bottomOffset -= 1.0;
            endY++;
            }
        } // for startY

    // unlock the bitmaps after usage
    srcIterator.End();
    if(aSrcMaskBitmap!=NULL)
        {
        srcMaskIterator.End();
        }

    }
*/

EXPORT_C void HuiUtil::CropImage(TInt aComponents,
                                const TSize& aSrcBufferSize,
                                const TUint8* aSrcBuffer,
                                const TPoint& aCropOffset,
                                const TSize& aCroppedSize,
                                TUint8* aDestBuffer)
    {
    ASSERT (aDestBuffer && aSrcBuffer);
    ASSERT (aSrcBufferSize.iWidth > 0 && aSrcBufferSize.iHeight > 0);
    ASSERT (aCroppedSize.iWidth > 0 && aCroppedSize.iHeight > 0);
    ASSERT (aCropOffset.iX < aSrcBufferSize.iWidth);
    ASSERT (aCropOffset.iY < aSrcBufferSize.iHeight);
    ASSERT (aComponents > 0 && aComponents < 5);

    TInt targetlinesize = aCroppedSize.iWidth*aComponents;
    TInt sourcelinesize = aSrcBufferSize.iWidth*aComponents;
    for (TInt y=0; y<aCroppedSize.iHeight; y++)
        {
        // copy line at a time..
        TAny * source = (TAny*)((const TUint8*)aSrcBuffer
                            + ((y+aCropOffset.iY)*sourcelinesize)
                            + (aCropOffset.iX * aComponents));
        TAny * target = (TAny*)((const TUint8*)aDestBuffer + (y*targetlinesize));
        memcpy(target, source, targetlinesize);
        }

    }

EXPORT_C CFbsBitmap* HuiUtil::ConvertBitmapToDisplayModeLC( const CFbsBitmap& aBitmap, const TDisplayMode& aDisplaymode )
    {
    // Create target bitmap
    CFbsBitmap* targetBitmap = new CFbsBitmap();
    CleanupStack::PushL( targetBitmap );
    targetBitmap->Create( aBitmap.SizeInPixels(), aDisplaymode );

    // Create bitmap device for target rendering
    CFbsBitmapDevice* targetDevice = CFbsBitmapDevice::NewL( targetBitmap );
    CleanupStack::PushL( targetDevice );

    // Create bitmap graphics context
    CFbsBitGc* bitgc = CFbsBitGc::NewL();
    CleanupStack::PushL( bitgc );
    bitgc->Activate( targetDevice );

    // BitBlt the given bitmap to target device.
    bitgc->BitBlt( TPoint( 0, 0 ), &aBitmap );

    CleanupStack::PopAndDestroy( bitgc );
    CleanupStack::PopAndDestroy( targetDevice );

    return targetBitmap;
    }


EXPORT_C TBool HuiUtil::TagMatches(const TDesC8& aTagsColonSeparated, const TDesC8& aTag)
    {
    TPtrC8 region = aTagsColonSeparated;
    TPtrC8 tag;
    TInt index = 0;

    if(!aTag.Length())
        {
        // No tag specified; doesn't match anything.
        return EFalse;
        }

    while(region.Length() > 0)
        {
        // Is there a colon in the region?
        index = region.Locate(TChar(KTagSeparator));
        if(index != KErrNotFound)
            {
            // A separator exists in the region.
            tag.Set(region.Left(index));
            region.Set(region.Right((region.Length() - index) - 1));
            }
        else
            {
            tag.Set(region);
            region.Set(region.Right(0));
            }

        if(!tag.Compare(aTag))
            {
            // Matches.
            return ETrue;
            }
        }

    // No match could be found.
    return EFalse;
    }

TReal32 HuiUtil::CalculateScaleFactorFromScaleMode( 
        const THuiRealSize& aContainerSize,   
        const THuiRealSize& aContentSize,
        CHuiImageVisual::TScaleMode aScaleMode,
        TReal32 aInitialScale )
    {
    TReal32 scale = aInitialScale;

    // Scaling factor adjustment.
    if(aScaleMode == CHuiImageVisual::EScaleFitHeight)
        {
        TReal32 contentHeight = aContentSize.iHeight;
        if(contentHeight > 0)
            {
            scale *= aContainerSize.iHeight / contentHeight;
            }
        }
    else if(aScaleMode == CHuiImageVisual::EScaleFitWidth)
        {
        TReal32 contentWidth = aContentSize.iWidth;
        if(contentWidth > 0)
            {
            scale *= aContainerSize.iWidth / contentWidth;
            }
        }
    else if(aScaleMode == CHuiImageVisual::EScaleCover || 
            aScaleMode == CHuiImageVisual::EScaleFitInside)
        {
        TReal32 heightScale = scale;
        TReal32 contentHeight = aContentSize.iHeight;
        if(contentHeight > 0)
            {
            heightScale *= aContainerSize.iHeight / contentHeight;
            }

        TReal32 widthScale = scale;
        TReal32 contentWidth = aContentSize.iWidth;
        if(contentWidth > 0)
            {
            widthScale *= aContainerSize.iWidth / contentWidth;
            }

        if(aScaleMode == CHuiImageVisual::EScaleCover)
            {
            scale = Max(widthScale, heightScale);
            }
        else
            {
            scale = Min(widthScale, heightScale);
            }
        }
    else
        {
        // for PC lint
        }
    return scale;
    }
    
EXPORT_C TBool HuiUtil::RealCompare( 
    TReal32 aCompare1, 
    TReal32 aCompare2, 
    TReal32 aEpsilon )
    {
    if ( Abs(aCompare1 - aCompare2) < aEpsilon )
        {
        return ETrue;
        }
    return EFalse;
    }

EXPORT_C void HuiUtil::Assert(TBool aCondition)
    {
    // Assert that the passed condition is true.
    if (aCondition == EFalse)
        {
        // You can breakpoint here to trap asserts.
        ASSERT(EFalse);           
        }
    }

EXPORT_C CFbsBitmap* HuiUtil::CopyBitmapL(const CFbsBitmap& aSrc)
    {
    CFbsBitmap* newBitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( newBitmap );
    User::LeaveIfError( newBitmap->Create( aSrc.SizeInPixels(), aSrc.DisplayMode() ) );
    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( newBitmap );
    CleanupStack::PushL( dev );
    CFbsBitGc* gc = NULL;
    User::LeaveIfError( dev->CreateContext( gc ) );
    CleanupStack::PushL( gc );
    gc->BitBlt( TPoint(0,0), &aSrc );
    CleanupStack::PopAndDestroy(2); // dev, gc        
    CleanupStack::Pop(); // newBitmap
    return newBitmap;
    }

EXPORT_C TInt HuiUtil::GetValueFromCentralRepository( const TUint32 aKey, TInt& aValue )
    {
    CRepository* centralRepository = NULL;    
    TRAPD( error, centralRepository = CRepository::NewL( KCRUidUIAccelTK ) );
    TInt result = 0;
    if ( error == KErrNone )
        {
        error = centralRepository->Get( aKey, result );
        }
    delete centralRepository;
    centralRepository = NULL;
    if ( error == KErrNone )
        {
        aValue = result;
        }
    return error;
    }
