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



#ifndef HUIFXVG10BLURFILTER_H_
#define HUIFXVG10BLURFILTER_H_

#include "HuiFxVg10FilterBase.h"

class CHuiFxVg10BlurFilter : public CHuiFxVg10FilterBase
    {
public:
    static CHuiFxVg10BlurFilter* NewL();
    TBool Draw(CHuiFxEngine& aEngine, CHuiGc& aGc,
            CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
            const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface);

    void DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool aHasSurface);
    void CalculateMargin(TMargins &aMargin) const;  
    CHuiFxVg10BlurFilter *CloneL() const;
protected:
    void ConstructL();
private:
    void UpdateAlphaLUT();
    TBool OpacityChanged();
    
    TReal32                 iOldOpacity;
    TReal32                 iBlurX;
    TReal32                 iBlurY;
    const static VGubyte    iColorLUT[256];
    VGubyte                 iAlphaLUT[256];
    };

#endif /*HUIFXVG10BLURFILTER_H_*/
