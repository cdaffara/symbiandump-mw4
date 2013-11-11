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
* Description:   Basic functionality of color matrix based filters.
*
*/



#include "HuiFxVg10ColorMatrixFilterBase.h"

CHuiFxVg10ColorMatrixFilterBase* CHuiFxVg10ColorMatrixFilterBase::NewL()
    {
    CHuiFxVg10ColorMatrixFilterBase* e = new (ELeave) CHuiFxVg10ColorMatrixFilterBase();
    CleanupStack::PushL(e);
    e->ConstructL();
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10ColorMatrixFilterBase::ConstructL()
    {
    CHuiFxVg10FilterBase::ConstructL();
    memset(iColorMatrix, 0, sizeof(iColorMatrix));
    }
CHuiFxVg10ColorMatrixFilterBase *CHuiFxVg10ColorMatrixFilterBase::CloneL() const
    {
    CHuiFxVg10ColorMatrixFilterBase *filter = new(ELeave)CHuiFxVg10ColorMatrixFilterBase;
    filter->CopyFromL(this);
    return filter;
    }
void CHuiFxVg10ColorMatrixFilterBase::CopyFromL(const CHuiFxVg10ColorMatrixFilterBase *aOldFilter)
    {
    CHuiFxVg10FilterBase::CopyFromL(aOldFilter);
    memcpy(iColorMatrix, aOldFilter->iColorMatrix, sizeof(iColorMatrix));
    }
void CHuiFxVg10ColorMatrixFilterBase::DrawEffect(CHuiFxEngine& /*aEngine*/, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight,TBool aHasSurface )
    {
    // take opacity into account
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);

    if(opacity > EPSILON)
        {
        if(ParameterChanged())
            {
            UpdateColorMatrix();
            }

        iColorMatrix[19] = aHasSurface ? 0.5f : 0.0f;
        vgColorMatrix(aTargetImage, aSourceImage, iColorMatrix);
        }
    else
        {
//        VGint width = vgGetParameteri(aSourceImage, VG_IMAGE_WIDTH);
//        VGint height = vgGetParameteri(aSourceImage, VG_IMAGE_HEIGHT);
        VGint width = aWidth;
        VGint height = aHeight;
        vgCopyImage(aTargetImage, 0, 0, aSourceImage, 0, 0, width, height, VG_FALSE);
        }
    }


// this is supposed to be overloaded by child classes
void CHuiFxVg10ColorMatrixFilterBase::UpdateColorMatrix()
    {
    }
