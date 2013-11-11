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
* Description:   Declares the MHuiTargetBitmap interface. MHuiTargetBitmap is 
*                an interface for a writable bitmap in raw format.
*
*/



#ifndef __HUITARGETBITMAP_H__
#define __HUITARGETBITMAP_H__


#include <e32base.h>


/**
 * Interface for a bitmap that can be drawn on as a raw bitmap. CHuiBitgdiGc
 * uses this interface for accessing the target bitmap where drawing is 
 * being done.
 */
class MHuiTargetBitmap
    {
public:

    virtual ~MHuiTargetBitmap() {}

    /**
     * Determines the size of the bitmap.
     * 
     * @return  Size of the bitmap in pixels.
     */
    virtual TSize Size() const = 0;
    
    /**
     * Determines the pixel format of the bitmap.
     *
     * @return  Format of the bitmap as a TDisplayMode.
     *
     * @see TDisplayMode in the platform documentation.
     */
    virtual TDisplayMode DisplayMode() const = 0;

    /**
     * Locks the heap so that the memory buffer of the bitmap can be 
     * accessed directly. Prevents heap compression done by other threads.
     * The heap must be unlocked with UnlockHeap() after the buffer
     * processing has been completed.
     *
     * @see CFbsBitmap::LockHeap()
     */
    virtual void LockHeap() = 0;

    /**
     * Unlocks the heap. Must be called after the heap has been locked 
     * and operated on.
     */    
    virtual void UnlockHeap() = 0;

    /** 
     * Provides access to the memory buffer of the bitmap.
     *
     * @return  Pointer to the beginning of the pixel data.
     *
     * @see LockHeap()
     * @see UnlockHeap()
     */     
    virtual TAny* DataAddress() = 0;

    /** 
     * Sets the clipping rectangle for the bitmap. All drawing operations
     * (FillRect(), BitBlt(), BitBltMasked()) are confined into this
     * rectangle when drawn.
     *
     * @param aRect  Clipping rectangle.
     */
    virtual void SetClippingRect(const TRect& aRect) = 0;
    
    /** 
     * Sets the clipping region for the bitmap. All drawing operations
     * (FillRect(), BitBlt(), BitBltMasked()) are confined into this
     * region when drawn.
     *
     * @param aRegion  Region containing clipping rectangles.
     */
    virtual void SetClippingRegion(const TRegion& aRegion) = 0;
    
    /** 
     * Draws a solid-colored rectangle. Affected by the clipping rectangle.
     *
     * @param aRect  Rectangle to fill with a solid color.
     * @param aColor  RGBA color to use for filling. Note that the alpha
     *                component of the TRgb is also valid.
     */
    virtual void FillRect(const TRect& aRect, const TRgb& aColor) = 0;
    
    /** 
     * Draws a bitmap. Affected by the clipping rectangle. Analogous to
     * the corresponding CFbsBitGc::BitBlt() method.
     *
     * @param aPoint  Position where to draw.
     * @param aBitmap  Bitmap to draw.
     * @param aSourceRect  Rectangle within the source bitmap where to draw from.
     */
    virtual void BitBlt(const TPoint& aPoint, const CFbsBitmap *aBitmap, const TRect &aSourceRect) = 0;
    
    /** 
     * Draws a masked bitmap. Affected by the clipping rectangle.
     * Analogous to the corresponding CFbsBitGc::BitBltMasked() method.
     *
     * @param aPoint  Position where to draw.
     * @param aBitmap  Bitmap to draw.
     * @param aSourceRect  Rectangle within the source bitmap where to draw from.
     * @param aMaskBitmap  Mask bitmap.
     * @param aInvertMask  ETrue if the mask should be inverted.
     */
    virtual void BitBltMasked(const TPoint &aPoint, const CFbsBitmap *aBitmap, const TRect &aSourceRect, const CFbsBitmap *aMaskBitmap, TBool aInvertMask) = 0;
    
    /**
     * Returns pointer to the bitmap. If the class do not have CFbsBitmap this methdod returns NULL.
     * 
     * @return  Pointer to the bitmap.
     */
    virtual CFbsBitmap* FbsBitmap() = 0;
    
    /**
     * Returns the BITGDI graphics context that is used for drawing in
     * the back buffer.
     * 
     * Writing a real implementation for this might not be needed if the renderer
     * does not support symbian graphics context.
     *
     * @return  BITGDI graphics context.
     */
    virtual CFbsBitGc* Gc() = 0;
    };


#endif // __HUITARGETBITMAP_H__
