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



#ifndef HUIFXVG10OUTLINEFILTER_H_
#define HUIFXVG10OUTLINEFILTER_H_

#include "HuiFxVg10FilterBase.h"

class CHuiFxVg10OutlineFilter : public CHuiFxVg10FilterBase
    {
public:
    static CHuiFxVg10OutlineFilter* NewL();
protected:
    void ConstructL();
    void DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool aHasSurface);
    void CalculateMargin(TMargins &aMargin) const;  
    CHuiFxVg10OutlineFilter *CloneL() const;
private:
    void UpdateLUT();
    
    TReal32     iOutlineX;
    TReal32     iOutlineY;
    TReal32     iSteepness;
    TRgb        iColor;
    VGuint      iLUT[256];
    };

#endif /*HUIFXVG10OUTLINEFILTER_H_*/
