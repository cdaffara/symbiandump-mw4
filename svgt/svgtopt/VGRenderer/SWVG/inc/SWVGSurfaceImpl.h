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
* Description:  CSWVGSurfaceImpl source file
 *
*/

#ifndef SWVGSURFACEIMPL_H_
#define SWVGSURFACEIMPL_H_

#include "SWVG.h"
#include "MVGSurfaceImpl.h"

class CSWVGSurfaceImpl : public CBase, public MVGSurfaceImpl
{
public:
    static CSWVGSurfaceImpl* NewL();
    static CSWVGSurfaceImpl* NewLC();
    
	virtual ~CSWVGSurfaceImpl();

    virtual TInt InitializeSurface(TSize aSize, TInt aColorSpace);
    
    virtual TInt CreateSurface(TInt aDisplayMode, RWindow *aSurface, CFbsBitmap *aBitmap);
    
    virtual TInt ResizeSurface(TSize aSize);
    
    virtual void SetConfiguration(TInt aOption, const TAny* aValue);
    
    virtual TInt CopyBitmap(TInt aDisplayMode,TInt aMaskMode,CFbsBitmap *aBitmap, CFbsBitmap *aMask, TSize aSize = TSize(0,0));

    virtual TInt PrepareToBindClientBuffer();
    virtual TInt BindClientBuffer(TInt buffer);
      
    virtual TInt UnBindClientBuffer();

    virtual void TerminateSurface();
    
private:
    CSWVGSurfaceImpl();
    void ConstructL();
    TSize   iSize;
};

#endif /*SWVGSURFACEIMPL_H_*/
