/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Fixed point arithmetics
*
*/



#ifndef __HUIFIXMATH_H__
#define __HUIFIXMATH_H__

#include <e32def.h>

static const TInt KFixPi = 0x3243F;


class HuiFixMath
{
public:
    IMPORT_C static TUint FixHypot(TInt aVal1,TInt aVal2);
		IMPORT_C static TInt FixSin(TInt aRadians);
		IMPORT_C static TInt FixCos(TInt aRadians);
		
		static inline TInt FixMul(TInt aVal1, TInt aVal2);
		static inline TInt FixDiv(TInt aVal1, TInt aVal2);
		static inline TInt FloatToFix(TReal32 aVal);
		static inline TReal32 FixToFloat(TInt aVal);
};

TInt HuiFixMath::FixMul(TInt aVal1, TInt aVal2)
		{
		TInt64 result = (TInt64(aVal1))*(TInt64(aVal2));
		return TInt(result>>16); 
		};

TInt HuiFixMath::FixDiv(TInt aVal1, TInt aVal2)
		{
		TUint64 temp = aVal1;
		temp<<=16;
		return TInt(temp/aVal2);
		};

TInt HuiFixMath::FloatToFix(TReal32 aVal)
		{
		return TInt(aVal*65536.0f);
		}
		
TReal32 HuiFixMath::FixToFloat(TInt aVal)
		{
		return TReal32((TReal32(aVal))/65536.0f);
		}


#endif