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
* Description:  CVGSurface header file
 *
*/

#ifndef VGSURFACE_H
#define VGSURFACE_H

#include <e32std.h>
#include <e32base.h>

#include "MVGSurfaceImpl.h"

class RWindow;
class CFbsBitmap;
class CVGRenderer;

class CSvgtBitmap;

/**
 *  CVGSurface
 * 
 */
class CVGSurface : public CBase
    {
public:
    enum SurfaceConfig
    {
    BTIMAP_HEADER = MVGSurfaceImpl::BTIMAP_HEADER,
    ALPHA_SIZE = MVGSurfaceImpl::ALPHA_SIZE,
    ALPHA_MASK_SIZE = MVGSurfaceImpl::ALPHA_MASK_SIZE
    };
    
    /**
     * Destructor.
     */
    ~CVGSurface();

    /*!
     * @fn          NewL()
     * @bief        Creates the renderer surface abstraction
     * @param       aVGSurfaceImpl The surface implementation
     * @return      Renderer surface abstraction
     */
    static CVGSurface* NewL(MVGSurfaceImpl *aVGSurfaceImpl);

    /*!
     * @fn          NewL()
     * @bief        Creates the renderer surface abstraction
     * @param       aVGSurfaceImpl The surface implementation
     * @return      Renderer surface abstraction
     */
    static CVGSurface* NewLC(MVGSurfaceImpl *aVGSurfaceImpl);

    /*!
     * @fn          InitializeSurface(TSize aSize, TInt aColorSpace)
     * @bief        Initialize the surface
     * @param       aSize The surface size
     * @param       aColorSpace This is optional with egl
     * @return      KErrNone in case of succesfull, appropriate error code otherwise
     */
    TInt InitializeSurface(TSize aSize, TInt aColorSpace)
        {
        return iImpl->InitializeSurface(aSize, aColorSpace);
        }

    /*!
     * @fn          CreateSurface(RWindow *aSurface, CFbsBitmap *aBitmap)
     * @bief        Creates the actual rendering surface
     * @param       aDisplayMode    The display mode
     * @param       aSurface The window object
     * @param       aBitmap  The bitmao object, is optional
     * @return      KErrNone in case of succesfull, appropriate error code otherwise
     */
    TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CFbsBitmap *aBitmap)
        {
        return iImpl->CreateSurface(aDisplayMode, aSurface, aBitmap);
        }
    
    /*!
     * @fn          ResizeSurface(TSize aSize)
     * @bief        Resizes the surface
     * @param       aSize   The new size
     * @return      KErrNone in case of succesfull, appropriate error code otherwise
     */
    TInt ResizeSurface(TSize aSize)
        {
        return iImpl->ResizeSurface(aSize);
        }

    /*!
     * @fn          SetConfiguration
     * @brief       sets surface/context/display configuration options
     * @param       aOption  name of the option
     * @param       aValue   value(s) for the option
     */
    void SetConfiguration(TInt aOption, const TAny* aValue)
        {
        return iImpl->SetConfiguration(aOption, aValue);
        }
    /*!
     * @fn          CopyBitmap(CFbsBitmap *aBitmap, CFbsBitmap *aMask, TInt aOption)
     * @brief       copies the bitmap from the surface to the given bitmap
     * @param       aDisplayMode    The display mode
     * @param       aBitmap The bitmap where the content will be copied
     * @param       aMask   Mask
     * @param       aOption Options, may be required for VGI calls
     */
    TInt CopyBitmap(TInt aDisplayMode,TInt aMaskMode,CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize)
        {
        return iImpl->CopyBitmap(aDisplayMode,aMaskMode, aBitmap, aMask, aSize);
        }
 
    TInt CopyBitmap(TInt aDisplayMode,TInt aMaskMode,CSvgtBitmap *aBitmap, CSvgtBitmap *aMask, TSize aSize)
        {
        return iImpl->CopyBitmap(aDisplayMode,aMaskMode, aBitmap, aMask, aSize);
        }

    /**
     * @fn          PrepareToBindClientBuffer
     *              Prepares the renderer to bind to an image. 
     *              This is added to allow some performance improvement the TLV renderer
     *              This method should be called before creating the VGImage, which should be made as the surface.
     *              No other image should be created after calling this function
     */
    TInt PrepareToBindClientBuffer()
        {
        return iImpl->PrepareToBindClientBuffer();
        }
    /*!
     * @fn          BindSurfaces(TInt buffer)
     * @brief       This function is used for Binding Off-Screen Rendering Surfaces To Client Buffers
     * @param       buffer is a client API reference to the buffer to be bound. buffer must be a valid VGImage handle.
     * @return      0 if success
     */
    TInt BindClientBuffer(TInt buffer)
        {
        return iImpl->BindClientBuffer(buffer);
        }
    
    /*!
     * @fn          UnbindSurface(TInt surface)
     * @brief       copies the bitmap from the surface to the given bitmap
     * @param       aBitmap The bitmap where the content will be copied
     * @param       aMask   Mask
     * @param       aOption Options, may be required for VGI calls
     * @return      0 if success
     */
    TInt UnBindClientBuffer()
        {
        return iImpl->UnBindClientBuffer();
        }
    /*!
     * @fn          TerminateSurface()
     * @brief       Will destroy the surface
     */
    void TerminateSurface()
        {
        iImpl->TerminateSurface();
        }


private:

    CVGSurface();

    /**
     * default constructor for performing 2nd stage construction
     */
    void ConstructL(MVGSurfaceImpl *aVGSurfaceImpl);

    MVGSurfaceImpl  *iImpl;
    };

#endif // VGSURFACE_H
