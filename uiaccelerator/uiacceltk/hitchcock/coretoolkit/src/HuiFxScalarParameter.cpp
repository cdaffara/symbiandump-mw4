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



#include "HuiFxScalarParameter.h"

template<>
TInt32 RHuiFxScalarTimeLine::ToFixedPoint(const TReal32& aValue)
    {
    return (TInt32)(aValue * 0x10000);
    }

template<>
TReal32 RHuiFxScalarTimeLine::FromFixedPoint(const TInt32& aValue)
    {
    return aValue / (TReal32)(0x10000);
    }

template<>
TInt32 RHuiFxScalarTimeLine::Blend(const TInt32& aValue1, const TInt32& aValue2, TInt32 aAlpha)
    {
    TInt64 diff = aValue2 - aValue1;
    return (TInt32)((TInt64)aValue1 + ((diff * (aAlpha >> 8)) >> 8));
    }

template<>
TInt32 RHuiFxScalarTimeLine::WeightedSum4(const TInt32& aValue1, const TInt32& aValue2, const TInt32& aValue3, const TInt32& aValue4,
                                          TInt32 aWeight1, TInt32 aWeight2, TInt32 aWeight3, TInt32 aWeight4)
    {
    TInt64 f1 = (TInt64)aValue1 * (TInt64)aWeight1;
    TInt64 f2 = (TInt64)aValue2 * (TInt64)aWeight2;
    TInt64 f3 = (TInt64)aValue3 * (TInt64)aWeight3;
    TInt64 f4 = (TInt64)aValue4 * (TInt64)aWeight4;
    return (TInt32)((f1 + f2 + f3 + f4) >> 16);
    }
