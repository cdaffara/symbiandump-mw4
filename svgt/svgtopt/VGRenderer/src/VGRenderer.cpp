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
* Description:  SVG Implementation header file
 *
*/


#include "VGRenderer.h"
#include "VGSurface.h"
#include <SVGRendererId.h>

_LIT(KPseudoVGDLL, "Z:\\sys\\bin\\PseudoVG.dll");
_LIT(KSWVGDLL,     "Z:\\sys\\bin\\SWVG.dll");
_LIT(KHWVGDLL,     "Z:\\sys\\bin\\HWVG.dll");

CVGRenderer::CVGRenderer()
    {
    iImpl           = 0;
    iCurrentSurface = 0;
    }

CVGRenderer::~CVGRenderer()
    {
    
    delete iImpl;

    delete iCurrentSurface;   

    iLibrary.Close();
    }

CVGRenderer* CVGRenderer::NewLC(SVGRendererId aRendererType, TInt aOption)
    {
    CVGRenderer* self = new (ELeave)CVGRenderer();
    CleanupStack::PushL(self);
    self->ConstructL(aRendererType, aOption);
    return self;
    }

EXPORT_C CVGRenderer* CVGRenderer::NewL(SVGRendererId aRendererType, TInt aOption)
    {
    CVGRenderer* self = CVGRenderer::NewLC(aRendererType, aOption);
    CleanupStack::Pop();
    return self;
    }

void CVGRenderer::ConstructL(SVGRendererId aRendererType, TInt aOption)
    {
    TInt ret = KErrNotFound;
    if (aRendererType == ESVGRendererTLV)
        {
        ret = iLibrary.Load(KPseudoVGDLL);
        }
    else if (aRendererType == ESVGRendererSW)
        {
        ret = iLibrary.Load(KSWVGDLL);
        }
    else if (aRendererType == ESVGRendererHW)
        {
        ret = iLibrary.Load(KHWVGDLL);
        }

    User::LeaveIfError(ret);
    
    TLibraryFunction VGRendererImpl = iLibrary.Lookup(1);
    this->iImpl = reinterpret_cast<MVGRendererImpl *>(VGRendererImpl());
    User::LeaveIfNull(this->iImpl);
    iCurrentSurface = CVGSurface::NewL(iImpl->CreateVGSurfaceL(aOption));
    }
