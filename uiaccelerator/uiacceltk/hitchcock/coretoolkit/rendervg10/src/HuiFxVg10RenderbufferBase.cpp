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
* Description:   
*
*/



#include "HuiFxVg10RenderbufferBase.h"

VGImage CHuiFxVg10RenderbufferBase::AcquireSubImage(const TRect& aRect)
    {
    TSize size = Size();
    TRect parentImageRect(Position(),Size());
    // Clip sub image rect to always be inside the buffer (to avoid error in vgChildImage) 
    TRect clippedRect(aRect);
    clippedRect.Intersection(parentImageRect); 
      
    if (clippedRect.iTl.iX == 0 && clippedRect.iTl.iY == 0 && clippedRect.Size() == size)
        {
        return Image(); // buffer is exactly the size of the rectangle and correctly positioned --- no need for childimage.
        }
    else
        {
        TInt x;
        TInt y;
        if(BufferType() == EBufferTypeOffscreen) 
            {
            x = 0;
            y = 0;
            }
        else
            {
            x = clippedRect.iTl.iX;
            y = size.iHeight - clippedRect.iBr.iY;    // Note: vertically flipped coordinate system. 
                                                // aRect is in Hitchcock coordinates (top-left 0,0)
                                                // but y is in VG coordinates (bottom-left 0,0).
            }
        VGImage ret = vgChildImage(Image(), x, y, clippedRect.Size().iWidth, clippedRect.Size().iHeight);
        
        HUIFX_VG_INVARIANT();
        return ret;
        }
    }

void CHuiFxVg10RenderbufferBase::ReleaseSubImage(VGImage aImage)
    {
    if (aImage != Image())
        {
        vgDestroyImage(aImage);
        HUIFX_VG_INVARIANT();
        }
    }
