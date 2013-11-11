/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder source file
*
*/

#include <openvg.h>

#include "TLVIconRenderer.h"
#include "MVGImageBinder.h"
#include "NVGUtil.h"
#include "NVGIconData.h"
#include "NVGTLVIcon.h"

CTLVIconRenderer::CTLVIconRenderer(TInt aWidth, TInt aHeight)
    : CTLVRenderer(aWidth, aHeight)
    {
    }

CTLVIconRenderer * CTLVIconRenderer::NewL(const TDesC8& aBuf, TInt aWidth, TInt aHeight)
    {
    CTLVIconRenderer* self = CTLVIconRenderer::NewLC(aBuf,aWidth, aHeight);
    CleanupStack::Pop(self);
    return self;
    }

CTLVIconRenderer * CTLVIconRenderer::NewLC(const TDesC8& aBuf, TInt aWidth, TInt aHeight)
    {
    CTLVIconRenderer* self = new (ELeave) CTLVIconRenderer(aWidth, aHeight);
    CleanupStack::PushL(self);
    self->ConstructL(aBuf);
    return self;
    }

CTLVIconRenderer::~CTLVIconRenderer()
    {
    }

TInt CTLVIconRenderer::ExecuteL(TInt aIndex)
    {
    TInt ret = KErrNone;
    
    if (aIndex == CNVGTLVIcon::EPath)
        {
        ret = DrawPathL();
        }
    else
        {
        ret = CTLVRenderer::ExecuteL(aIndex);
        }
    
    return ret;
    }

TInt CTLVIconRenderer::DrawPathL()
    {
    VGPath path = (VGPath)iNVGIconData->ReadInt32L();
    VGPaintMode paintMode = (VGPaintMode)iNVGIconData->ReadInt32L();
    vgDrawPath(path, paintMode);

    NVG_DEBUGSTMT(LogvgDrawPath(path, paintMode));

    return KErrNone;
    }
