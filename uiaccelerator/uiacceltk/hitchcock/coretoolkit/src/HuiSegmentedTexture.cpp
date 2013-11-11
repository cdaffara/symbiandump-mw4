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
* Description:   Implementation for non-pure-virtual methods in MHuiSegmentedTexture.
*
*/



#include "uiacceltk/HuiSegmentedTexture.h"
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiTextureHandle.h"


EXPORT_C MHuiSegmentedTexture::~MHuiSegmentedTexture()
    {
    }


EXPORT_C THuiTextureHandle MHuiSegmentedTexture::Handle(TInt aSegment) const
    {
    return THuiTextureHandle(SegmentName(aSegment), 
                             SegmentSize(aSegment), 
                             SegmentTextureSize(aSegment));
    }


EXPORT_C void MHuiSegmentedTexture::Bind(TInt aSegment, TInt aTextureUnit) const
    {
    // Activate the correct texture unit.
    if(CHuiStatic::CurrentRenderSurface())
        {
        CHuiStatic::CurrentRenderSurface()->BindTexture(aTextureUnit, *this, aSegment);
        }
    }
