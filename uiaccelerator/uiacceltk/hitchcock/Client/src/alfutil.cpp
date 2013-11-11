/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Misc. utils
*
*/



//
// NOTE THAT THIS FILE IS ALMOST IDENTICAL WITH HUIUTIL.CPP !
//
// HuiUtil.h functions are not called from this file because we want alfclient.dll
// to not have unnecessary (any) static dependencies to core toolkit hitchcock.dll 
// (a.k.a huitk.dll, uiaccel.dll,...). There is no other reason to duplicate 
// this code.
//

#include "alf/alfutil.h"
#include "alf/alftimedvalue.h"

#include <e32math.h>
#include <hal.h>
#include <AknUtils.h>

const TReal32 KLengthUnitDivisor = 320.0;

/// Character used for separating tags in a tag descriptor.
const TInt KTagSeparator = ':';


// Calculates the smallest power-of-two that is equal to or greater than
// a value.
EXPORT_C TInt AlfUtil::Power2(TInt aValue)
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

EXPORT_C TInt AlfUtil::Power2RoundDown(TInt aValue)
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


EXPORT_C TReal32 AlfUtil::Interpolate(TReal32 aPos, TReal32 aMin, TReal32 aMax) __SOFTFP
    {
    aPos = Max(0.f, aPos);
    aPos = Min(aPos, 1.f);
    return (1 - aPos) * aMin + aPos * aMax;
    }


EXPORT_C void AlfUtil::WrapValue(TReal32& aValue, TReal32 aLow, TReal32 aHigh) __SOFTFP
    {
    TReal32 segments = 0;
    TReal32 length = aHigh - aLow;

    if(length <= 0)
        {
        aValue = aLow;
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


EXPORT_C TInt AlfUtil::RandomInt(TInt aMin, TInt aMax)
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


EXPORT_C TReal32 AlfUtil::RandomReal(TReal32 aMin, TReal32 aMax) __SOFTFP
    {
    /** @todo  Could use Math::FRand(). */
    TReal32 random = RandomInt(0, 10000000) / 10000000.0;
    return aMin + (aMax - aMin) * random;
    }


EXPORT_C TUint AlfUtil::FreeMemory(TUint* aTotalMemory)
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


EXPORT_C TSize AlfUtil::ScreenSize()
    {
    TSize screenSize(320, 240);

    AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EScreen, screenSize);

    return screenSize;
    }


EXPORT_C CFont* AlfUtil::NearestFontInTwipsL(const TFontSpec& aFontSpec)
    {
    /// @todo  Accessing the screen device during a display resizing event may
    ///        result in a font that is suitable for the display size that
    ///        was in use prior to the resize. Probably we should use
    ///        AknLayoutUtils here.

    CFont* font = NULL;
    CWsScreenDevice* screenDev = CCoeEnv::Static()->ScreenDevice();
    User::LeaveIfError( screenDev->GetNearestFontInTwips(font, aFontSpec) );
    return font;
    }


EXPORT_C void AlfUtil::ReleaseFont(CFont* aFont)
    {
    CCoeEnv::Static()->ScreenDevice()->ReleaseFont(aFont);
    }


EXPORT_C TReal32 AlfUtil::LengthUnit() __SOFTFP
    {
    return Max(ScreenSize().iWidth, ScreenSize().iHeight) / KLengthUnitDivisor;
    }


EXPORT_C TReal32 AlfUtil::QuickLength(TAlfRealPoint& aVector) __SOFTFP
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


EXPORT_C TReal32 AlfUtil::QuickLength(TReal32 aDx, TReal32 aDy) __SOFTFP
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


EXPORT_C void AlfUtil::QuickNormalize(TAlfRealPoint& aNormal)
    {
    TReal32 approxLength = QuickLength(aNormal);

    if(approxLength > 0)
        {
        aNormal.iX /= approxLength;
        aNormal.iY /= approxLength;
        }
    }


EXPORT_C void AlfUtil::QuickNormalize(TReal32 aVector[3]) __SOFTFP
    {
    TReal32 approxLength = QuickLength(QuickLength(aVector[0], aVector[1]), aVector[2]);

    if(approxLength > 0)
        {
        aVector[0] /= approxLength;
        aVector[1] /= approxLength;
        aVector[2] /= approxLength;
        }
    }


EXPORT_C void AlfUtil::CrossProduct(const TReal32 aA[3], const TReal32 aB[3],
                                    TReal32 aProduct[3]) __SOFTFP
    {
    aProduct[0] = aA[1] * aB[2] - aB[1] * aA[2];
    aProduct[1] = aA[2] * aB[0] - aB[2] * aA[0];
    aProduct[2] = aA[0] * aB[1] - aB[0] * aA[1];
    }


EXPORT_C void AlfUtil::NormalFromPoints(const TReal32 aPoints[3][3], TReal32 aNormal[3]) __SOFTFP
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


EXPORT_C void AlfUtil::ShadowMatrix(const TReal32 aPlanePoint[3],
                                    const TReal32 aPlaneNormal[3],
                                    const TReal32 aLightPos[4],
                                    TReal32 aDestMat[16]) __SOFTFP
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


EXPORT_C TReal32 AlfUtil::ColorLightness(const TRgb& aColor) __SOFTFP
    {
    TReal32 red = aColor.Red() / 255.0f;
    TReal32 green = aColor.Red() / 255.0f;
    TReal32 blue = aColor.Red() / 255.0f;

    return (red*2 + green*3 + blue) / 6.f;
    }

EXPORT_C void AlfUtil::ScaleFbsBitmapL(const CFbsBitmap & aSrcBitmap,
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
    gc->DrawBitmap(TRect(TPoint(0,0), aScaledBitmap.SizeInPixels()),
                   (const CFbsBitmap *)&aSrcBitmap);
    delete gc;
    CleanupStack::PopAndDestroy(targetdevice);
    }

EXPORT_C void AlfUtil::CombineMaskFbsBitmapL(const CFbsBitmap & aSrcBitmap,
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

EXPORT_C void AlfUtil::CropFbsBitmapL(const CFbsBitmap & aSrcBitmap,
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

EXPORT_C void AlfUtil::ScaleImage(TInt aComponents,
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

EXPORT_C void AlfUtil::CropImage(TInt aComponents,
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

EXPORT_C CFbsBitmap* AlfUtil::ConvertBitmapToDisplayModeLC( const CFbsBitmap& aBitmap, const TDisplayMode& aDisplaymode )
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


EXPORT_C TBool AlfUtil::TagMatches(const TDesC8& aTagsColonSeparated, const TDesC8& aTag)
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
            region.Set(region.Right(region.Length() - index - 1));
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


void AlfUtil::Assert(TBool aCondition)
    {
    // Assert that the passed condition is true.
    if (aCondition == EFalse)
        {
        // You can breakpoint here to trap asserts.
        ASSERT(EFalse);           
        }
    }


TInt AlfUtil::RoundFloatToInt(TReal32 aVal)
	{
	return (aVal < 0 ? (TInt)(aVal - 0.5f) : (TInt)(aVal + 0.5f));
	}

// ---------------------------------------------------------------------------
// DEPRECATED: Gets the Avkon Skin TAknsItemID based on string version
// ---------------------------------------------------------------------------
//
EXPORT_C TAknsItemID AlfUtil::ThemeItemIdL( CAlfEnv& /*aEnv*/, const TDesC& /*aSkinId*/)
    {
	return KAknsIIDNone;
    }
