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
 * Description:  CSWVGTargetSurfaceImpl header file
 *
*/

#ifndef SWOPENVGTARGETSURFACEIMPL_H_
#define SWOPENVGTARGETSURFACEIMPL_H_

#include "SWVG.h"
#include "MVGSurfaceImpl.h"

#include <displaymode.h>

class CSvgtBitmap;

class CSWVGTargetSurfaceImpl : public CBase, public MVGSurfaceImpl
{
public:
    static CSWVGTargetSurfaceImpl* NewL();
    static CSWVGTargetSurfaceImpl* NewLC();
    
    virtual ~CSWVGTargetSurfaceImpl();

    virtual TInt InitializeSurface(TSize aSize, TInt aColorSpace);
    
    virtual TInt CreateSurface(TInt /*aDisplayMode*/, RWindow */*aSurface*/, CFbsBitmap */*aBitmap*/);
    virtual TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CSvgtBitmap *aBitmap);
    
    virtual TInt ResizeSurface(TSize aSize);
    
    virtual void SetConfiguration(TInt aOption, const TAny* aValue);
    
    virtual TInt CopyBitmap(TInt /*aDisplayMode*/, TInt /*aMaskMode*/, CFbsBitmap */*aBitmap*/, CFbsBitmap */*aMask*/, TSize /*aSize*/ = TSize(0,0));
    virtual TInt CopyBitmap(TInt aDisplayMode,TInt aMaskMode,CSvgtBitmap *aBitmap, CSvgtBitmap *aMask, TSize aSize = TSize(0,0));

    virtual TInt PrepareToBindClientBuffer();
    virtual TInt BindClientBuffer(TInt buffer);
      
    virtual TInt UnBindClientBuffer();

    virtual void TerminateSurface();
    
    TInt MapSymbianDisplayModeToOpenVG( TDisplayMode aMode) const;
    TInt MapOpenVGErrorToSymbianError( TInt aError ) const;

private:
    CSWVGTargetSurfaceImpl();
    void ConstructL();
    TSize   iSize;
};

#endif /* SWOPENVGTARGETSURFACEIMPL_H_ */
