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



#ifndef HUIFXRENDERBUFFER_H_
#define HUIFXRENDERBUFFER_H_

#include "HuiFxConstants.h"
#include <uiacceltk/HuiGc.h>

class CHuiFxRenderbuffer: public CBase
    {
protected:
    IMPORT_C CHuiFxRenderbuffer();
    // TODO: luminance-only render buffers
    // TODO: optional alpha channel?
    IMPORT_C void ConstructL(const TSize& aSize, TRenderBufferType aBufferType);
public:
    IMPORT_C virtual ~CHuiFxRenderbuffer();

    virtual CHuiGc& BindAsRenderTarget() = 0;
    virtual void BindAsTexture(THuiFxRenderbufferUsage aUsage) = 0;
    virtual void UnbindAsRenderTarget() = 0;
    virtual void UnbindAsTexture() = 0;
    
    virtual void PrepareForReuse(const TSize& aReusedSize) = 0;

    IMPORT_C const TSize& Size() const;
    IMPORT_C const TRenderBufferType& BufferType() const;
    
    // Position relative to screen coordinates
    IMPORT_C void SetPosition(TPoint aPosition);
    IMPORT_C const TPoint& Position() const;
    
    // Background is read (or not) from current surface when PrepareForReuse is called.
    IMPORT_C void EnableBackground(TBool aEnable); 
        
protected:    
    TSize                iSize;
    TRenderBufferType    iType;
    TPoint               iPosition;
    TBool                iBackgroundEnabled;
    };

#endif /*HUIFXRENDERBUFFER_H_*/
