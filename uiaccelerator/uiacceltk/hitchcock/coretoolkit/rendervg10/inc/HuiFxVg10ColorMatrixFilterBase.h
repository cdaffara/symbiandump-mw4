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
* Description:   Base functionality for vgColorMatrix() based filters. 
*
*/



#ifndef HUIFXVG10COLORMATRIXFILTERBASE_H_
#define HUIFXVG10COLORMATRIXFILTERBASE_H_

#include "HuiFxVg10FilterBase.h"
#include "HuiFxVg10.h"

class CHuiFxVg10ColorMatrixFilterBase: public CHuiFxVg10FilterBase
    {
public:
    static CHuiFxVg10ColorMatrixFilterBase* NewL();
protected:
    void ConstructL();
    void DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool aHasSurface);
    virtual void UpdateColorMatrix();
    void CopyFromL(const CHuiFxVg10ColorMatrixFilterBase *aOldFilter);
    CHuiFxVg10ColorMatrixFilterBase *CloneL() const;

    VGfloat iColorMatrix[20];
    };

#endif /*HUIFXVG10COLORMATRIXFILTERBASE_H_*/
