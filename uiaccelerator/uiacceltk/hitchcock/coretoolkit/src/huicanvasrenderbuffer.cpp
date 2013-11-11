/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class canvas renderering target
*
*/



#include "huicanvasrenderbuffer.h"
#include "HuiRenderPlugin.h"


EXPORT_C CHuiCanvasRenderBuffer::CHuiCanvasRenderBuffer()
    {	    
    }

EXPORT_C CHuiCanvasRenderBuffer::~CHuiCanvasRenderBuffer()
    {
    }

EXPORT_C void CHuiCanvasRenderBuffer::InitializeL(const TSize& /*aSize*/)
    {
    iInitialized = ETrue;
    }

EXPORT_C void CHuiCanvasRenderBuffer::UnInitialize()
    {
    iInitialized = EFalse;
    }

EXPORT_C void CHuiCanvasRenderBuffer::Bind()
    {
    // Default implementation does nothing
    }

EXPORT_C void CHuiCanvasRenderBuffer::UnBind()
    {
    // Default implementation does nothing
    }

EXPORT_C TSize CHuiCanvasRenderBuffer::Size() const
    {
    return iSize;
    }

EXPORT_C TBool CHuiCanvasRenderBuffer::IsInitialized() const
    {
    return iInitialized;
    }

EXPORT_C void CHuiCanvasRenderBuffer::Copy(const CHuiCanvasRenderBuffer& /*aSourceBuffer*/)
    {
    // Default implementation does nothing    
    }

EXPORT_C void CHuiCanvasRenderBuffer::Copy(TPoint /*aPoint*/)   
    {
    // Default implementation does nothing    
    }

EXPORT_C void CHuiCanvasRenderBuffer::InitializeL(const TSize& /*aSize*/, TBool /*aForceClear*/)
    {
    iInitialized = ETrue;
    }

