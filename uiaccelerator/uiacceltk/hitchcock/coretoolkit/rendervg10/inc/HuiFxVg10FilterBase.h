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



#ifndef HUIFXVG10FILTERBASE_H_
#define HUIFXVG10FILTERBASE_H_

#include "HuiFxFilter.h"
#include "HuiFxVg10RenderbufferBase.h"
#include "HuiFxVg10.h"
#include <e32math.h>

// when comparing floating point numbers against zero, if the number is below
// EPSILON it's considered zero.
const float EPSILON = 0.001;


class CHuiFxVg10FilterBase: public CHuiFxFilter
    {
public: // from CHuiFxFilter
    virtual TBool Draw(CHuiFxEngine& aEngine, CHuiGc& aGc,
            CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
            const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface);

private:
    virtual void DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool aHasSurface) = 0;
protected:
    TBool wasEnoughMemory();
    
    template <class T>
    inline T clamp(const T value, const T minimum, const T maximum) {return (Max(Min(maximum, value), minimum));}
    };
 

#endif /*HUIFXVG10FILTERBASE_H_*/
