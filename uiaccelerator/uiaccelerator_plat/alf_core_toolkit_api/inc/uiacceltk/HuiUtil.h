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
* Description:   Declares HuiUtil class, a collection of utility routines for HUITK.
*
*/



#ifndef __HUIUTIL_H__
#define __HUIUTIL_H__


#include <e32base.h>
#include <gdi.h>

#include <uiacceltk/HuiRealSize.h> // for HUI_ROUND_FLOAT_TO_INT macro
#include <uiacceltk/HuiImageVisual.h>

const TUint KHuiAntialiasing = 0x00000001; // antialiasing enabled (bit mask)

_LIT(KHuiUserInvariant, "User::Invariant"); // max 16 characters

#define USER_INVARIANT() \
    ( RDebug::Printf("User::Invariant in "__FILE__" on line %i", __LINE__), \
      User::Panic(KHuiUserInvariant, 0) )

/* Forward declarations. */
class THuiRealPoint;
class TAknFontSpecification;
class THuiFont;
class CHuiDisplay;


/**
 * HuiUtil is a collection of utility routines for HUITK.
 * Many of the utility routines are mathematical operations such as
 * the calculation of a vector cross product or linear interpolation between two
 * values.
 *
 * HuiUtil contains only static member functions. It cannot be instantiated.
 */
NONSHARABLE_CLASS(HuiUtil)
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
    IMPORT_C static TReal32 QuickLength(THuiRealPoint& aVector) __SOFTFP;

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
    IMPORT_C static void QuickNormalize(THuiRealPoint& aVector);

    /**
     * Normalizes a vector so that its length will be approximately 1.0.
     * This is done without calculating a square root.
     *
     * @param aVector  3D vector to be normalized as a point (X, Y, Z components
     *                 as an array of TReal32). Will be modified.
     */
    IMPORT_C static void QuickNormalize(TReal32 aVector[3]);

    /**
     * Calculates a cross product of 3D vectors. aProduct = aA x aB.
     *
     * @param aA  3D vector.
     * @param aB  3D vector.
     * @param aProduct  3D vector where the cross product is returned.
     */
    IMPORT_C static void CrossProduct(const TReal32 aA[3], const TReal32 aB[3], TReal32 aProduct[3]);

    /**
     * Calculates a normal vector for a plane defined by three points.
     *
     * @param aPoints  Array of three 3D vectors. The first index is the number
     *                 of the vector, the second index is the vector component
     *                 (x, y, z).
     * @param aNormal  Resulting normal vector.
     */
    IMPORT_C static void NormalFromPoints(const TReal32 aPoints[3][3], TReal32 aNormal[3]);

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
                                      TReal32 aDestMat[16]);

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
     * @see CHuiVisual::FindTag()
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

    /** @endAPI */

    /**
     * Calculates the scale factor for an image content which is 
     * place on a specific container are. 
     *
     * @param aContainerSize Container size where the image is been placed on.
     * @param aContentSize Size of the image original.
     * @param aScaleMode Scale mode.
     * @param aInitialScale Initial scale value. 
     * @return Result scale factor. Returns aInitialScale if aScaleMode = EScaleFit.
     *
     * @see CHuiImageVisual::TScaleMode
     */
    static TReal32 CalculateScaleFactorFromScaleMode( 
        const THuiRealSize& aContainerSize,   
        const THuiRealSize& aContentSize,
        CHuiImageVisual::TScaleMode aScaleMode,
        TReal32 aInitialScale = 1.f ); 
     
    /**
     * Compares floating points.
     *
     * @param aCompare1 Another compare value
     * @param aCompare2 Another compare value
     * @param aEpsilon Delta value in which the compared values are treated
     *        as equal float values.
     * @return ETrue if the floating points are concidered to be the same.
     */   
    IMPORT_C static TBool RealCompare( 
        TReal32 aCompare1, 
        TReal32 aCompare2, 
        TReal32 aEpsilon = 0.0001f );

    /**
     * Asserts if the given condition is false.
     */
    IMPORT_C static void Assert(TBool aCondition);


    /**
     * Creates a copy of a bitmap, ownership is transferred to caller
     */
    IMPORT_C static CFbsBitmap* CopyBitmapL(const CFbsBitmap& aSrc);
    
    /**
     * Gets an integer value from central repository
     * 
     * @param aKey key whose value is queried
     * @param aValue the value of the key
     * @return error, if OK, KErrNone
     *    if there is any error, aValue is not changed
     */
    IMPORT_C static TInt GetValueFromCentralRepository( const TUint32 aKey, TInt& aValue );

    };

/** Sign function template. */
template <class T>
inline TInt Sgn(T aVal)
    {return(aVal>0 ? 1 : aVal<0 ? -1 : 0);}

// Define our own HUI_ASSERT macro for trapping asserts.
#ifdef _DEBUG
// Define a wrapper for our assert method.
#define HUI_ASSERT(cond) HuiUtil::Assert(TBool(cond))
#define HUI_ASSERT2(cond, panic) { if(!(cond)) HUI_PANIC(panic) }
#else // not _DEBUG
// In release builds asserts are ignored.
#define HUI_ASSERT(cond) //HuiUtil::Assert(cond)
#define HUI_ASSERT2(cond, panic) 
#endif //not _DEBUG

/**
 * Debug macros for printing debug messages. The debug messages will be
 * redirected to console using RDebug::Print(), or to a log file if
 * HUI_DEBUG_WITH_PRINTF is defined and CHuiStatic::EnableLogging() has been
 * called.
 *
 * Note that the debug macros will not be compiled in to release builds,
 * unless HUI_DEBUG_WITH_PRINTF is defined.
 */
 
// Debug output is disabled in windows by default.
// It slows down the emulator, and often it is not needed
// If you want debug output in windows, comment the following definition out.
// It does not affect logging in hardware. In hardware debug version always logs.
#define HUI_NO_DEBUG_OUTPUT_IN_WINS
 
#ifndef __WINS__ 
    #ifdef _DEBUG
        #ifdef HUI_DEBUG_WITH_PRINTF
            // Debug build that prints log output via CHuiStatic::Printf()
            #  define HUI_DEBUG(s)                  CHuiStatic::Printf(s)
            #  define HUI_DEBUG1(s, t)              CHuiStatic::Printf(s, t)
            #  define HUI_DEBUG2(s, t, u)           CHuiStatic::Printf(s, t, u)
            #  define HUI_DEBUG3(s, t, u, v)        CHuiStatic::Printf(s, t, u, v)
            #  define HUI_DEBUG4(s, t, u, v, w)     CHuiStatic::Printf(s, t, u, v, w)
            #  define HUI_DEBUG5(s, t, u, v, w, x)  CHuiStatic::Printf(s, t, u, v, w, x)
            #  define HUI_DEBUG6(s, t, u, v, w, x, y)     CHuiStatic::Printf(s, t, u, v, w, x, y)
            #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)  CHuiStatic::Printf(s, t, u, v, w, x, y, z)
        #else // HUI_DEBUG_WITH_PRINTF
            // Debug build that prints log output via RDebug (preferably the WINSCW Emulator)
            #  define HUI_DEBUG(s)                  RDebug::Print(s)
            #  define HUI_DEBUG1(s, t)              RDebug::Print(s, t)
            #  define HUI_DEBUG2(s, t, u)           RDebug::Print(s, t, u)
            #  define HUI_DEBUG3(s, t, u, v)        RDebug::Print(s, t, u, v)
            #  define HUI_DEBUG4(s, t, u, v, w)     RDebug::Print(s, t, u, v, w)
            #  define HUI_DEBUG5(s, t, u, v, w, x)  RDebug::Print(s, t, u, v, w, x)
            #  define HUI_DEBUG6(s, t, u, v, w, x, y)     RDebug::Print(s, t, u, v, w, x, y)
            #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)  RDebug::Print(s, t, u, v, w, x, y, z)
        #endif // HUI_DEBUG_WITH_PRINTF
    #else // _DEBUG
        // No debug output for release builds
        #  define HUI_DEBUG(s)
        #  define HUI_DEBUG1(s, t)
        #  define HUI_DEBUG2(s, t, u)
        #  define HUI_DEBUG3(s, t, u, v)
        #  define HUI_DEBUG4(s, t, u, v, w)
        #  define HUI_DEBUG5(s, t, u, v, w, x)
        #  define HUI_DEBUG6(s, t, u, v, w, x, y)
        #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)
    #endif // _DEBUG
#else // __WINS__ 
    #ifdef HUI_NO_DEBUG_OUTPUT_IN_WINS
        // No output in windows by default
        #  define HUI_DEBUG(s)
        #  define HUI_DEBUG1(s, t)
        #  define HUI_DEBUG2(s, t, u)
        #  define HUI_DEBUG3(s, t, u, v)
        #  define HUI_DEBUG4(s, t, u, v, w)
        #  define HUI_DEBUG5(s, t, u, v, w, x)
        #  define HUI_DEBUG6(s, t, u, v, w, x, y)
        #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)
    #else // HUI_NO_DEBUG_OUTPUT_IN_WINS
        #ifdef HUI_DEBUG_WITH_PRINTF
            // Debug build that prints log output via CHuiStatic::Printf()
            #  define HUI_DEBUG(s)                  CHuiStatic::Printf(s)
            #  define HUI_DEBUG1(s, t)              CHuiStatic::Printf(s, t)
            #  define HUI_DEBUG2(s, t, u)           CHuiStatic::Printf(s, t, u)
            #  define HUI_DEBUG3(s, t, u, v)        CHuiStatic::Printf(s, t, u, v)
            #  define HUI_DEBUG4(s, t, u, v, w)     CHuiStatic::Printf(s, t, u, v, w)
            #  define HUI_DEBUG5(s, t, u, v, w, x)  CHuiStatic::Printf(s, t, u, v, w, x)
            #  define HUI_DEBUG6(s, t, u, v, w, x, y)     CHuiStatic::Printf(s, t, u, v, w, x, y)
            #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)  CHuiStatic::Printf(s, t, u, v, w, x, y, z)
        #else // HUI_DEBUG_WITH_PRINTF
            // Debug build that prints log output via RDebug (preferably the WINSCW Emulator)
            #  define HUI_DEBUG(s)                  RDebug::Print(s)
            #  define HUI_DEBUG1(s, t)              RDebug::Print(s, t)
            #  define HUI_DEBUG2(s, t, u)           RDebug::Print(s, t, u)
            #  define HUI_DEBUG3(s, t, u, v)        RDebug::Print(s, t, u, v)
            #  define HUI_DEBUG4(s, t, u, v, w)     RDebug::Print(s, t, u, v, w)
            #  define HUI_DEBUG5(s, t, u, v, w, x)  RDebug::Print(s, t, u, v, w, x)
            #  define HUI_DEBUG6(s, t, u, v, w, x, y)     RDebug::Print(s, t, u, v, w, x, y)
            #  define HUI_DEBUG7(s, t, u, v, w, x, y, z)  RDebug::Print(s, t, u, v, w, x, y, z)
        #endif // HUI_DEBUG_WITH_PRINTF
    #endif // HUI_NO_DEBUG_OUTPUT_IN_WINS
#endif // __WINS__ 

/** Framedraw debug macros for printing debug messages during frame drawing operation. */
#ifdef HUI_DEBUG_FRAMEDRAW_WITH_PRINTF
#  define HUI_DEBUGF(s)                  CHuiStatic::Printf(s)
#  define HUI_DEBUGF1(s, t)              CHuiStatic::Printf(s, t)
#  define HUI_DEBUGF2(s, t, u)           CHuiStatic::Printf(s, t, u)
#  define HUI_DEBUGF3(s, t, u, v)        CHuiStatic::Printf(s, t, u, v)
#  define HUI_DEBUGF4(s, t, u, v, w)     CHuiStatic::Printf(s, t, u, v, w)
#  define HUI_DEBUGF5(s, t, u, v, w, x)  CHuiStatic::Printf(s, t, u, v, w, x)
#  define HUI_DEBUGF6(s, t, u, v, w, x, y)     CHuiStatic::Printf(s, t, u, v, w, x, y)
#  define HUI_DEBUGF7(s, t, u, v, w, x, y, z)  CHuiStatic::Printf(s, t, u, v, w, x, y, z)
#else
#ifdef HUI_DEBUG_FRAMEDRAW
#  define HUI_DEBUGF(s)                  RDebug::Print(s)
#  define HUI_DEBUGF1(s, t)              RDebug::Print(s, t)
#  define HUI_DEBUGF2(s, t, u)           RDebug::Print(s, t, u)
#  define HUI_DEBUGF3(s, t, u, v)        RDebug::Print(s, t, u, v)
#  define HUI_DEBUGF4(s, t, u, v, w)     RDebug::Print(s, t, u, v, w)
#  define HUI_DEBUGF5(s, t, u, v, w, x)  RDebug::Print(s, t, u, v, w, x)
#  define HUI_DEBUGF6(s, t, u, v, w, x, y)     RDebug::Print(s, t, u, v, w, x, y)
#  define HUI_DEBUGF7(s, t, u, v, w, x, y, z)  RDebug::Print(s, t, u, v, w, x, y, z)
#else
#  define HUI_DEBUGF(s)
#  define HUI_DEBUGF1(s, t)
#  define HUI_DEBUGF2(s, t, u)
#  define HUI_DEBUGF3(s, t, u, v)
#  define HUI_DEBUGF4(s, t, u, v, w)
#  define HUI_DEBUGF5(s, t, u, v, w, x)
#  define HUI_DEBUGF6(s, t, u, v, w, x, y)
#  define HUI_DEBUGF7(s, t, u, v, w, x, y, z)
#endif
#endif



/**
 * Non-exporting implementation for MHuiSessionObject
 */
#define HUI_SESSION_OBJECT_IMPL(classname, type) \
MHuiSessionObject::TType classname::Type() const \
    { \
    return type; \
    } \
\
TInt classname::SessionId() const \
    { \
    return iSessionId; \
    } \
\
void classname::SetSessionId(TInt aSessionId) \
    { \
    iSessionId = aSessionId; \
    }


/**
 * Implementation for MHuiSessionObject with exports
 */
#define HUI_SESSION_OBJECT_IMPL_EXPORT(classname, type) \
EXPORT_C MHuiSessionObject::TType classname::Type() const \
    { \
    return type; \
    } \
\
EXPORT_C TInt classname::SessionId() const \
    { \
    return iSessionId; \
    } \
\
EXPORT_C void classname::SetSessionId(TInt aSessionId) \
    { \
    iSessionId = aSessionId; \
    }


/* Probing macros. Probing is enabled only if macro HUI_PROBE_ENABLED is defined. */
#ifdef HUI_PROBE_ENABLED

/**
 * Associate this object with current session.
 */
#define HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION \
    CHuiStatic::Probe().AssociateWithCurrentSession(*this);
    
/**
 * Report to Hui Probe object; report construction of this object.
 */
#define HUI_PROBE_REPORT_CONSTRUCTED \
    CHuiStatic::Probe().ReportObjectLifeCycleEvent(*this, MHuiProbe::ELifecycleEventConstructed);
    
/**
 * Report to Hui Probe object; report destruction of this object.
 */
#define HUI_PROBE_REPORT_DESTRUCTED \
    CHuiStatic::Probe().ReportObjectLifeCycleEvent(*this, MHuiProbe::ELifecycleEventDestructed);
    
/**
 * Report to Hui Probe object; report release of this object.
 */
#define HUI_PROBE_REPORT_RELEASED \
    CHuiStatic::Probe().ReportObjectLifeCycleEvent(*this, MHuiProbe::ELifecycleEventReleased);
    
/**
 * Report to Hui Probe object; report restoration of this object.
 */
#define HUI_PROBE_REPORT_RESTORED \
    CHuiStatic::Probe().ReportObjectLifeCycleEvent(*this, MHuiProbe::ELifecycleEventRestored);

/**
 * Report to Hui Probe object; report entering on program flow point
 */
#define HUI_PROBE_PROGRAMFLOW_ENTER(point) \
    CHuiStatic::Probe().ReportProgramFlowEvent( \
        point, \
        MHuiProbe::EProgramFlowEventEnter);
        
/**
 * Report to Hui Probe object; report exiting from program flow point
 */
#define HUI_PROBE_PROGRAMFLOW_EXIT(point) \
    CHuiStatic::Probe().ReportProgramFlowEvent( \
        point, \
        MHuiProbe::EProgramFlowEventExit);

/**
 * Report to Hui Probe object; report entering on program flow point,
 * where the program flow point is in session object.
 */
#define HUI_PROBE_PROGRAMFLOW_WITHSESSION_ENTER(point) \
    CHuiStatic::Probe().ReportProgramFlowEvent( \
        *this, \
        point, \
        MHuiProbe::EProgramFlowEventEnter);
        
/**
 * Report to Hui Probe object; report exiting from program flow point,
 * where the program flow point is in session object.
 */
#define HUI_PROBE_PROGRAMFLOW_WITHSESSION_EXIT(point) \
    CHuiStatic::Probe().ReportProgramFlowEvent( \
        *this, \
        point, \
        MHuiProbe::EProgramFlowEventExit);

#else // HUI_PROBE_ENABLED

#define HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
#define HUI_PROBE_REPORT_CONSTRUCTED
#define HUI_PROBE_REPORT_DESTRUCTED
#define HUI_PROBE_REPORT_RELEASED
#define HUI_PROBE_REPORT_RESTORED
#define HUI_PROBE_PROGRAMFLOW_ENTER(point)
#define HUI_PROBE_PROGRAMFLOW_EXIT(point)
#define HUI_PROBE_PROGRAMFLOW_WITHSESSION_ENTER(point)
#define HUI_PROBE_PROGRAMFLOW_WITHSESSION_EXIT(point)

#endif // HUI_PROBE_ENABLED

#endif // __HUIUTIL_H__
