/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Misc. utilities
*
*/



#ifndef C_ALFUTIL_H
#define C_ALFUTIL_H


#include <e32base.h>
#include <gdi.h>
#include <AknsItemID.h>
#include <alf/alfenv.h>

/* Forward declarations. */
struct TAlfRealPoint;


/**
 * AlfUtil is a collection of utility routines for Alfred.dll.
 * Many of the utility routines are mathematical operations such as
 * the calculation of a vector cross product or linear interpolation between two
 * values.
 *
 * AlfUtil contains only static member functions. It cannot be instantiated.
 */
NONSHARABLE_CLASS(AlfUtil)
    {
public:

    /** @beginAPI */

    /**
     * Calculates the smallest power-of-two that is equal to or greater than
     * a value (rounds value up to the nearest power-of-two).
     *
     * @param aValue Integer value.
     *
     * @return  Smallest power-of-two that is equal to or greater than
     *          <code>aValue</code>.
     *
     * @see Power2RoundDown() for down-rounding equivalent.
     */
    IMPORT_C static TInt Power2(TInt aValue);

    /**
     * Calculates the largest power-of-two that is equal to or smaller than
     * a value (rounds the value down to nearest power-of-two).
     *
     * @param aValue  Integer value.
     *
     * @return  Smallest power-of-two that is equal to or greater than
     *          <code>aValue</code>.
     */
    IMPORT_C static TInt Power2RoundDown(TInt aValue);

    /**
     * Interpolates between two values. Calculates a linear interpolation
     * between a minimum and maximum value.
     *
     * @param aPos  Position. 0.0 corresponds to the minimum value, 1.0
     *              corresponds to the maximum value. Positions outside this
     *              range are allowed, which results in extrapolation.
     * @param aMin  Minimum value.
     * @param aMax  Maximum value.
     *
     * @return  Interpolated value.
     */
    IMPORT_C static TReal32 Interpolate(TReal32 aPos, TReal32 aMin, TReal32 aMax) __SOFTFP;

    /**
     * Wraps a value to a range.
     *
     * @param aValue  Reference to the value to wrap. Will be modified.
     * @param aLow    Range minimum. If aValue is equal to aLow, it will remain
     *                at aLow.
     * @param aHigh   Range maximum. If aValue is equal to aHigh, it will be
     *                wrapped to aLow.
     */
    IMPORT_C static void WrapValue(TReal32& aValue, TReal32 aLow, TReal32 aHigh) __SOFTFP;

    /**
     * Generates a random integer.
     *
     * @param aMin  Minimum possible value.
     * @param aMax  Maximum possible value.
     *
     * @return  Random integer between the minimum and maximum values,
     *          inclusive.
     */
    IMPORT_C static TInt RandomInt(TInt aMin, TInt aMax);

    /**
     * Generates a random real number.
     *
     * @param aMin  Minimum possible value.
     * @param aMax  Maximum possible value.
     *
     * @return  Random real number between the minimum and maximum values,
     *          inclusive.
     */
    IMPORT_C static TReal32 RandomReal(TReal32 aMin, TReal32 aMax) __SOFTFP;

    /**
     * Approximates the length of a 2D vector. This is done without
     * calculating a square root.
     *
     * @param aVector  2D vector as a point. The X and Y components are
     *                 interpreted as deltas.
     *
     * @return  Approximate length of the vector.
     */
    IMPORT_C static TReal32 QuickLength(TAlfRealPoint& aVector) __SOFTFP;

    /**
     * Approximates the length of a 2D vector. This is done without
     * calculating a square root.
     *
     * @param aDx  X delta of the vector.
     * @param aDy  Y delta of the vector.
     *
     * @return  Approximate length of the vector.
     */
    IMPORT_C static TReal32 QuickLength(TReal32 aDx, TReal32 aDy) __SOFTFP;

    /**
     * Normalizes a vector so that its length will be approximately 1.0.
     * This is done without calculating a square root.
     *
     * @param aVector  2D vector to be normalized as a point. The X and Y
     *                 components are interpreted as deltas. Will be modified.
     */
    IMPORT_C static void QuickNormalize(TAlfRealPoint& aVector);

    /**
     * Normalizes a vector so that its length will be approximately 1.0.
     * This is done without calculating a square root.
     *
     * @param aVector  3D vector to be normalized as a point (X, Y, Z components
     *                 as an array of TReal32). Will be modified.
     */
    IMPORT_C static void QuickNormalize(TReal32 aVector[3]) __SOFTFP;

    /**
     * Calculates a cross product of 3D vectors. aProduct = aA x aB.
     *
     * @param aA  3D vector.
     * @param aB  3D vector.
     * @param aProduct  3D vector where the cross product is returned.
     */
    IMPORT_C static void CrossProduct(const TReal32 aA[3], const TReal32 aB[3], TReal32 aProduct[3]) __SOFTFP;

    /**
     * Calculates a normal vector for a plane defined by three points.
     *
     * @param aPoints  Array of three 3D vectors. The first index is the number
     *                 of the vector, the second index is the vector component
     *                 (x, y, z).
     * @param aNormal  Resulting normal vector.
     */
    IMPORT_C static void NormalFromPoints(const TReal32 aPoints[3][3], TReal32 aNormal[3]) __SOFTFP;

    /**
     * Calculates a matrix that projects 3D points onto a plane. The name
     * ShadowMatrix comes from the intended usage: when used as a modelview
     * matrix, this matrix will flatten a 3D mesh onto a projected shadow.
     *
     * @param aPlanePoint  Point on the plane onto which the shadow is casted.
     * @param aPlaneNormal  Normal of the plane onto which the shadow is casted.
     * @param aLightPos  Position of the light source.
     * @param aDestMat  Resulting matrix.
     */
    IMPORT_C static void ShadowMatrix(const TReal32 aPlanePoint[3],
                                      const TReal32 aPlaneNormal[3],
                                      const TReal32 aLightPos[4],
                                      TReal32 aDestMat[16]) __SOFTFP;

    /**
     * Determines the amount of free memory in the system.
     *
     * @param aTotalMemory  If not NULL, the total amount of memory is
     *                      returned here.
     *
     * @return  Bytes of free memory.
     */
    IMPORT_C static TUint FreeMemory(TUint* aTotalMemory = 0);

    /**
     * Determines the native resolution of the device's screen.
     *
     * @return  Screen size in pixels.
     */
    IMPORT_C static TSize ScreenSize();

    /**
     * Finds the font that is the closest match to a font specification.
     * Note that the returned font instance must be released with
     * AlfUtil::ReleaseFont(). Deleting the returned font instance is not allowed.
     *
     * @param aFontSpec  Font specification.
     *
     * @return  Font instance. Must be released with a call to AlfUtil::ReleaseFont()
     *          when no longer needed.
     */
    IMPORT_C static CFont* NearestFontInTwipsL(const TFontSpec& aFontSpec);

    /**
     * Releases a font that was returned by NearestFontInTwipsL(). Must be
     * called when the font is no longer needed.
     *
     * @param aFont  Font returned by a previous allocation by AlfUtil.
     */
    IMPORT_C static void ReleaseFont(CFont* aFont);

    /**
     * Calculates a display size dependent length unit. This can be used for
     * layout calculations that should be independent of the display size.
     * For example, this could be the distance at which a shadow is drawn
     * behind text strings (assuming the font size also depends on display
     * size).
     *
     * @return  Length unit.
     */
    IMPORT_C static TReal32 LengthUnit() __SOFTFP;

    /**
     * Approximates the lightness of a color. The returned value is in range
     * 0...1, where 0.0 means the equivalent of black and 1.0 means the
     * equivalent of white. This is similar to the Value component in the HSV
     * color model (but not exactly the same).
     *
     * @param aColor  Color whose lightness is to be evaluated.
     *
     * @return  Lightness value.
     */
    IMPORT_C static TReal32 ColorLightness(const TRgb& aColor) __SOFTFP;

    /**
     * Resamples given FBS bitmap to new size (converting
     * color modes along the way..)
     * @param aSrcBitmap Source bitmap to resample.
     * @param aScaledBitmap Output bitmap. The original size of
     * this CFbsBitmap defines the size of the downscaled bitmap.
     */
    IMPORT_C static void ScaleFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                         CFbsBitmap & aScaledBitmap);

    /**
     * Converts separate mask and color bitmaps into single
     * EColor16MA (24bpp colour plus 8bpp alpha) bitmap.
     * @param aBitmap Source color channel bitmap.
     * @param aMaskBitmap The source alpha mask bitmap. Use either a black and
     * white (binary) mask bitmap, or if aMaskBitmap's display mode is EGray256,
     * alpha blending is used. Use of any other mode may result in unpredictable
     * results.
     * @param aCombinedBitmap Bitmap that will to contain the target
     * EColor16MA bitmap with color information from aBitmap and alpha information
     * from aMaskBitmap. NOTE: Must have color mode EColor16MA!
     */
    IMPORT_C static void CombineMaskFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                               const CFbsBitmap & aSrcMaskBitmap,
                                               CFbsBitmap & aCombinedBitmap);

    /**
     * Crops an area of FBS bitmap to new image. The size of the
     * copped bitmap defines the size of the area to crop.
     */
    IMPORT_C static void CropFbsBitmapL(const CFbsBitmap & aSrcBitmap,
                                        CFbsBitmap & aCroppedBitmap,
                                        TPoint aCropPosition);

    /**
     * Resamples given unsigned byte image data to new size. Uses box
     * filtering. Can be used to downscale too large textures, for example.
     *
     * @param aComponents Number of image components (color channels) in the
     * source image. Use 3 with RGB image and 4 with RGBA. The destination will
     * have the same amount of color channels after this operation.
     * @param aSrcSize Size (width and height) of the original image (aSrcBuffer),
     * in number of pixels (width) or rows (height).
     * @param aSrcBuffer Pointer to the source image data.
     * @param aDestSize Size (width and height) of the new scaled image,
     * (aDestBuffer) in number of pixels (width) or rows (height).
     * @param aDestBuffer Output parameter: Pointer where new scaled image
     * data will be stored. You must remember to allocate the space for the
     * buffer before calling this method.
     */
    IMPORT_C static void ScaleImage(TInt aComponents,
                                    const TSize& aSrcSize,
                                    const TUint8* aSrcBuffer,
                                    const TSize& aDestSize,
                                    TUint8* aDestBuffer);

    /**
     * Crops given unsigned byte image data. Can be used to split
     * large images to smaller sections.
     *
     * @param aComponents Number of image components (color channels) in the
     * source image. Use 3 with RGB image and 4 with RGBA. The destination will
     * have the same amount of color channels after this operation.
     * @param aSrcBufferSize Size (width and height) of the original image (aSrcBuffer),
     * in number of pixels (width) or rows (height).
     * @param aSrcBuffer Pointer to the source image data.
     * @param aCropOffset Cropping offset in the source image. Location of the
     * top-left corner of the cropped area, in pixels.
     * @param aCroppedSize Size of the cropped area. Is also the size of the
     * image in aDestBuffer. Width and height in pixels.
     * @param aDestBuffer Output parameter: Pointer where the cropped image
     * data will be stored. You must remember to allocate the space for the
     * buffer before calling this method.
     */
    IMPORT_C static void CropImage(TInt aComponents,
                                    const TSize& aSrcBufferSize,
                                    const TUint8* aSrcBuffer,
                                    const TPoint& aCropOffset,
                                    const TSize& aCroppedSize,
                                    TUint8* aDestBuffer);


    /**
     * Checks if aTag is included in aTagsColonSeparated. Tags are case-sensitive.
     *
     * @param aTagsColonSeparated  One or more tags separated by colons (:).
     *                             For example: "hello:world:tag3".
     * @param aTag  Tag to look for. For example: "world".
     *
     * @return  <code>ETrue</code>, if the tag was found in the colon-separated
     *          tags descriptor. <code>EFalse</code> otherwise.
     *
     * @see CAlfVisual::FindTag()
     */
    IMPORT_C static TBool TagMatches(const TDesC8& aTagsColonSeparated, const TDesC8& aTag);

    /**
     * Converts a bitmap to specified display mode.
     * This method converts a bitmap in any display mode to specified display mode.
     * Method creates a new bitmap, leaves it in cleanup stack and returns it.
     *
     * @param aBitmap The bitmap to be converted.
     * @param aDisplaymode The target display mode to convert the bitmap into.
     * @return Newly created conversion of the source bitmap.
     */
    IMPORT_C static CFbsBitmap* ConvertBitmapToDisplayModeLC( const CFbsBitmap& aBitmap, const TDisplayMode& aDisplaymode );

 
    /**
     * @deprecated <b>Always returns KAknsIIDNone<\b>
     * 
     * Retrieves the TAknsItemID corresponding to the Textual skin identifier
     *
     * @param aEnv Alf Environment variable
     * @param aSkinId The textual identifier for which corresponding TAknsItemID is
     *	required
     *
     * @return TAknsItemID object corresponding to aSkinId
     */
	 IMPORT_C static TAknsItemID ThemeItemIdL(CAlfEnv& aEnv, const TDesC& aSkinId );

    /** @endAPI */

    /**
     * Asserts if the given condition is false.
     */
    static void Assert(TBool aCondition);

    /**
     * Rounds a float (TReal32) correctly into an integer (TInt). Takes care of both negative
     * and positive real number.
     * 
     * @param aVal The value to be rounded to the closest integer.
     * @return Rounded integer.
     */
    static TInt RoundFloatToInt(TReal32 aVal);
    
    };



#endif // C_ALFUTIL_H
