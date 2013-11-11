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



#include "HuiFxColorParameter.h"
#include <gdi.h>

template<>
TUint32 RHuiFxColorTimeLine::ToFixedPoint(const TRgb& aValue)
    {
    return aValue.Internal();
    }

template<>
TRgb RHuiFxColorTimeLine::FromFixedPoint(const TUint32& aValue)
    {
    return TRgb(aValue);
    }

template<>
TUint32 RHuiFxColorTimeLine::Blend(const TUint32& aValue1, const TUint32& aValue2, TInt32 aAlpha)
    {
    TUint32 redBlue1    = (aValue1 & 0x00ff00ff);
    TUint32 alphaGreen1 = (aValue1 & 0xff00ff00) >> 8;
    TUint32 redBlue2    = (aValue2 & 0x00ff00ff);
    TUint32 alphaGreen2 = (aValue2 & 0xff00ff00) >> 8;
    
    TUint32 redBlueDelta    = redBlue2 - redBlue1; 
    TUint32 alphaGreenDelta = alphaGreen2 - alphaGreen1;

    aAlpha >>= 8;
    
    redBlueDelta    = (redBlueDelta * aAlpha) >> 8;
    alphaGreenDelta = (alphaGreenDelta * aAlpha) >> 8;

    redBlue1    += (redBlueDelta & 0xff00ff);
    alphaGreen1 += (alphaGreenDelta & 0xff00ff);
    
    return redBlue1 | (alphaGreen1 << 8);
    }

template<>
TUint32 RHuiFxColorTimeLine::WeightedSum4(const TUint32& aValue1, const TUint32& aValue2, const TUint32& aValue3, const TUint32& aValue4,
                                          TInt32 aWeight1, TInt32 aWeight2, TInt32 aWeight3, TInt32 aWeight4)
    {
    TUint32 a1 = ((aValue1 & 0xff000000) >> 24) * aWeight1;
    TUint32 a2 = ((aValue2 & 0xff000000) >> 24) * aWeight2;
    TUint32 a3 = ((aValue3 & 0xff000000) >> 24) * aWeight3;
    TUint32 a4 = ((aValue4 & 0xff000000) >> 24) * aWeight4;
    
    TUint32 r1 = ((aValue1 & 0x00ff0000) >> 16) * aWeight1;
    TUint32 r2 = ((aValue2 & 0x00ff0000) >> 16) * aWeight2;
    TUint32 r3 = ((aValue3 & 0x00ff0000) >> 16) * aWeight3;
    TUint32 r4 = ((aValue4 & 0x00ff0000) >> 16) * aWeight4;

    TUint32 g1 = ((aValue1 & 0x0000ff00) >> 8) * aWeight1;
    TUint32 g2 = ((aValue2 & 0x0000ff00) >> 8) * aWeight2;
    TUint32 g3 = ((aValue3 & 0x0000ff00) >> 8) * aWeight3;
    TUint32 g4 = ((aValue4 & 0x0000ff00) >> 8) * aWeight4;
    
    TUint32 b1 = (aValue1 & 0x000000ff) * aWeight1;
    TUint32 b2 = (aValue2 & 0x000000ff) * aWeight2;
    TUint32 b3 = (aValue3 & 0x000000ff) * aWeight3;
    TUint32 b4 = (aValue4 & 0x000000ff) * aWeight4;
    
    TUint32 a = ((a1 + a2 + a3 + a4) << 8)  & 0xff000000;
    TUint32 r = ((r1 + r2 + r3 + r4))       & 0x00ff0000;
    TUint32 g = ((g1 + g2 + g3 + g4) >> 8)  & 0x0000ff00;
    TUint32 b = ((b1 + b2 + b3 + b4) >> 16) & 0x000000ff;
    
    return (a | r | g | b);
    }
