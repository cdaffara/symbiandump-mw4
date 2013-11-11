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
* Description:  MVGSurfaceImpl header file
 *
*/

#ifndef MVGSURFACEIMPL_H_
#define MVGSURFACEIMPL_H_

#include <e32std.h>
#include <e32base.h>

class RWindow;
class CFbsBitmap;
class CSvgtBitmap;

class MVGSurfaceImpl
{
public:
    enum SurfaceConfig
    {
    BTIMAP_HEADER,
    ALPHA_SIZE,
    ALPHA_MASK_SIZE
    };
      
    virtual ~MVGSurfaceImpl(){}
	
    virtual TInt InitializeSurface(TSize aSize, TInt aColorSpace) = 0;
    
    virtual TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CFbsBitmap *aBitmap) = 0;
    virtual TInt CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CSvgtBitmap */*aBitmap*/){ return KErrNotSupported; }
    
    virtual TInt ResizeSurface(TSize aSize) = 0;
    
    virtual void SetConfiguration(TInt aOption, const TAny* aValue) = 0;
    
    virtual TInt CopyBitmap(TInt aDisplayMode, TInt aMaskMode, CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize = TSize(0,0)) = 0;
    virtual TInt CopyBitmap(TInt /*aDisplayMode*/, TInt /*aMaskMode*/, CSvgtBitmap* /*aBitmap*/, CSvgtBitmap */*aMask*/, TSize /*aSize*/ = TSize(0,0)) { return KErrNotSupported; }
    
    virtual TInt BindClientBuffer(TInt aBuffer) = 0;
    
    virtual TInt PrepareToBindClientBuffer() = 0;
    virtual TInt UnBindClientBuffer() = 0;
   
    virtual void TerminateSurface() = 0;
    
protected:
	MVGSurfaceImpl(){}
};

#endif /*MVGSURFACEIMPL_H_*/
