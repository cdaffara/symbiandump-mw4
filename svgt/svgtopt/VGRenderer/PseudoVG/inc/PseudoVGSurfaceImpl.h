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
* Description:  CPseudoVGSurfaceImpl header file
 *
*/

#ifndef PSEUDOVGSURFACEIMPL_H
#define PSEUDOVGSURFACEIMPL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "MVGSurfaceImpl.h"

static const TUid KUidNvgProprietaryFormat = { 968435518 };
class CPseodoVGRendererImpl;
/**
 *  CPseudoVGSurfaceImpl
 * 
 */
class CPseudoVGSurfaceImpl : public CBase, public  MVGSurfaceImpl
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPseudoVGSurfaceImpl();

    /**
     * Two-phased constructor.
     */
    static CPseudoVGSurfaceImpl* NewL(CPseodoVGRendererImpl * aVGRendererImpl);

    /**
     * Two-phased constructor.
     */
    static CPseudoVGSurfaceImpl* NewLC(CPseodoVGRendererImpl * aVGRendererImpl);

    virtual TInt InitializeSurface( TSize aSize, TInt aColorSpace );
    
    virtual TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CFbsBitmap *aBitmap);
    
    virtual TInt ResizeSurface( TSize aSize );
    
    virtual TInt CopyBitmap(TInt aDisplayMode, TInt aMaskMode, CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize = TSize(0,0));

    virtual TInt PrepareToBindClientBuffer();
    virtual TInt BindClientBuffer(TInt buffer);
      
    virtual void SetConfiguration(TInt aOption, const TAny* aValue);
    
    virtual TInt UnBindClientBuffer();

    virtual void TerminateSurface();

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPseudoVGSurfaceImpl();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(CPseodoVGRendererImpl * aVGRendererImpl);

    TSize                   iSize;
    CPseodoVGRendererImpl * iPseodoVGRendererImpl;
    };

#endif // PSEUDOVGSURFACEIMPL_H
