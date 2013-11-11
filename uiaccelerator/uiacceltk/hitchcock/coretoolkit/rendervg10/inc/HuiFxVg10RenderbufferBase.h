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



#ifndef HUIFXVG10RENDERBUFFERBASE_H_
#define HUIFXVG10RENDERBUFFERBASE_H_

#include "HuiFxRenderbuffer.h"
#include "HuiFxVg10.h"
class CHuiFxVg10RenderbufferBase: public CHuiFxRenderbuffer
    {
public:
    virtual VGImage Image() const = 0;
    
    VGImage AcquireSubImage(const TRect& aRect);
    void ReleaseSubImage(VGImage aImage);
    };

#endif /*HUIFXVG10RENDERBUFFERBASE_H_*/
