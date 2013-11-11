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
* Description:   Implementation of CHuiAnimatedTexture base class, the base class
*                for application-specific animated textures.
*
*/



#include "uiacceltk/HuiAnimatedTexture.h" // Class definition
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiTextureHandle.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiStatic.h"


EXPORT_C CHuiAnimatedTexture::CHuiAnimatedTexture(CHuiTextureManager& aManager)
        : iManager(aManager)
    {
    }


EXPORT_C void CHuiAnimatedTexture::BaseConstructL()
    {
    iTexture = CHuiStatic::Renderer().CreateTextureL();

    // Register automatically for time updates.
    iManager.iAnimatedTextures.AppendL(*this);
    }


EXPORT_C CHuiAnimatedTexture::~CHuiAnimatedTexture()
    {
    iManager.iAnimatedTextures.RemoveIfFound(*this);

    delete iTexture;
    }


EXPORT_C CHuiTexture& CHuiAnimatedTexture::Texture()
    {
    return *iTexture;
    }
    
    
EXPORT_C MHuiSegmentedTexture* CHuiAnimatedTexture::SegmentedTexture()
    {
    return iTexture;
    }
    
    
EXPORT_C MHuiShadowedTexture* CHuiAnimatedTexture::ShadowedTexture()
    {
    return NULL;
    }


EXPORT_C const MHuiSegmentedTexture* CHuiAnimatedTexture::SegmentedTexture() const
    {
    return iTexture;
    }
    
    
EXPORT_C const MHuiShadowedTexture* CHuiAnimatedTexture::ShadowedTexture() const
    {
    return NULL;
    }
    

EXPORT_C TBool CHuiAnimatedTexture::HasContent() const
    {
    return iTexture->HasContent();
    }


EXPORT_C TSize CHuiAnimatedTexture::Size() const
    {
    return iTexture->Size();
    }


EXPORT_C TInt CHuiAnimatedTexture::SegmentCount() const
    {
    return iTexture->SegmentCount();
    }


EXPORT_C TUint CHuiAnimatedTexture::SegmentName(TInt aSegment) const
    {
    return iTexture->SegmentName(aSegment);
    }


EXPORT_C TSize CHuiAnimatedTexture::SegmentSize(TInt aSegment) const
    {
    return iTexture->SegmentSize(aSegment);
    }


EXPORT_C TSize CHuiAnimatedTexture::SegmentTextureSize(TInt aSegment) const
    {
    return iTexture->SegmentTextureSize(aSegment);
    }


EXPORT_C TPoint CHuiAnimatedTexture::SegmentOffset(TInt aSegment) const
    {
    return iTexture->SegmentOffset(aSegment);
    }
    
EXPORT_C TBool CHuiAnimatedTexture::TextureChanged() const
    {
    if (iTexture)
        {
        return iTexture->TextureChanged();    
        }
    return EFalse;
    }

EXPORT_C void CHuiAnimatedTexture::TextureClearChanged() const
    {
    if (iTexture)
        {
        iTexture->TextureClearChanged();    
        }
    return;
    }

EXPORT_C void CHuiAnimatedTexture::TextureExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    
    }
