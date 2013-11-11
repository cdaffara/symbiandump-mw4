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



#include "HuiFxRenderbuffer.h"

EXPORT_C CHuiFxRenderbuffer::CHuiFxRenderbuffer()
    {
    }

EXPORT_C void CHuiFxRenderbuffer::ConstructL(const TSize& aSize, TRenderBufferType aBufferType)
    {
    iSize = aSize;
    iType = aBufferType;
    }

EXPORT_C CHuiFxRenderbuffer::~CHuiFxRenderbuffer()
    {
    }

EXPORT_C const TSize& CHuiFxRenderbuffer::Size() const
    {
    return iSize;
    }

EXPORT_C const TRenderBufferType& CHuiFxRenderbuffer::BufferType() const
    {
    return iType;
    }

EXPORT_C const TPoint& CHuiFxRenderbuffer::Position() const
    {
    return iPosition;
    }

EXPORT_C void CHuiFxRenderbuffer::SetPosition(TPoint aPosition)
    {
    iPosition = aPosition;
    }

EXPORT_C void CHuiFxRenderbuffer::EnableBackground(TBool aEnable)
    {
    iBackgroundEnabled = aEnable;
    }

