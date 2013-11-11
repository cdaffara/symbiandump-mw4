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
* Description:   Implementation of THuiTextureHandle. THuiTextureHandle is a 
*                handle to a named texture object. It does not have ownership 
*                of the texture object.
*
*/



#include "uiacceltk/HuiTextureHandle.h"
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C THuiTextureHandle::THuiTextureHandle()
        : iName(0), iSize(0, 0), iTextureSize(0, 0)
    {
    }


EXPORT_C THuiTextureHandle::THuiTextureHandle(TUint aName,
        const TSize& aSize,
        const TSize& aTextureSize)
    {
    Set(aName, aSize, aTextureSize);
    }
  
  
EXPORT_C TBool THuiTextureHandle::IsNull() const
    {
    return iName == 0;
    }  
  

EXPORT_C TBool THuiTextureHandle::HasContent() const
    {
    return !IsNull();
    }


EXPORT_C void THuiTextureHandle::Release()
    {
    CHuiStatic::Renderer().DeleteNamedTexture(iName);
    iName = 0;
    iSize = iTextureSize = TSize(0, 0);
    }


EXPORT_C void THuiTextureHandle::Set(TUint aName,
                                     const TSize& aSize,
                                     const TSize& aTextureSize)
    {
    iName = aName;
    iSize = aSize;
    iTextureSize = aTextureSize;
    }


EXPORT_C void THuiTextureHandle::SetName(TUint aName)
    {
    iName = aName;
    }


EXPORT_C void THuiTextureHandle::SetSize(const TSize& aSize)
    {
    iSize = aSize;
    }


EXPORT_C void THuiTextureHandle::SetTextureSize(const TSize& aTextureSize)
    {
    iTextureSize = aTextureSize;
    }


EXPORT_C TInt THuiTextureHandle::SegmentCount() const
    {
    return 1;
    }


EXPORT_C TSize THuiTextureHandle::Size() const
    {
    return iSize;
    }


EXPORT_C TUint THuiTextureHandle::SegmentName(TInt /*aSegment*/) const
    {
    return iName;
    }


EXPORT_C TSize THuiTextureHandle::SegmentSize(TInt /*aSegment*/) const
    {
    return iSize;
    }


EXPORT_C TSize THuiTextureHandle::SegmentTextureSize(TInt /*aSegment*/) const
    {
    return iTextureSize;
    }


EXPORT_C TPoint THuiTextureHandle::SegmentOffset(TInt /*aSegment*/) const
    {
    return TPoint(0, 0);
    }


EXPORT_C MHuiSegmentedTexture* THuiTextureHandle::SegmentedTexture()
    {
    return this;
    }
    
    
EXPORT_C MHuiShadowedTexture* THuiTextureHandle::ShadowedTexture()
    {
    return NULL;
    }


EXPORT_C const MHuiSegmentedTexture* THuiTextureHandle::SegmentedTexture() const
    {
    return this;
    }
    
    
EXPORT_C const MHuiShadowedTexture* THuiTextureHandle::ShadowedTexture() const
    {
    return NULL;
    }
    

EXPORT_C TUint THuiTextureHandle::Name() const
    {
    return iName;
    }


EXPORT_C TBool THuiTextureHandle::TextureChanged() const
    {
	return EFalse;
    }


EXPORT_C void THuiTextureHandle::TextureClearChanged() const
    {
	return;
    }

EXPORT_C void THuiTextureHandle::TextureExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    }
