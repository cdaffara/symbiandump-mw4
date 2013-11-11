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
* Description:   ?Description
*
*/



#include <e32cmn.h>
#include "uiacceltk/huifixmath.h"


static const TInt KFixPi_2 = 0x1921F;
static const TInt KSinMagic = 0x78000;

#define FIX_PT_BITS 16
#define SWAP(x, y) {(x)^=(y);(y)^=(x);(x)^=(y);}


EXPORT_C TInt HuiFixMath::FixSin(TInt aRadians)
	{
    if (aRadians < (-KFixPi*2) || aRadians > (KFixPi*2))
        {
        aRadians%=KFixPi*2;
        }
    
    if (aRadians < -KFixPi_2)
        {
        aRadians+=2*KFixPi;
        }

    // PI/2 - PI
    if (aRadians > KFixPi_2 && aRadians < KFixPi)
        {
        aRadians = KFixPi-aRadians;
        }
    // PI-PI*3/4
    else if (aRadians >=KFixPi && aRadians < (KFixPi+KFixPi_2))    
        {
        aRadians = -(aRadians-KFixPi);
        }
    // PI3*/4-2PI
    else if (aRadians >=(KFixPi+KFixPi_2) && aRadians <=2*KFixPi)
        {
        aRadians-=KFixPi*2;
        }
    else
        {
        // for PC lint
        }

    TInt64 value = aRadians;
    value = (value*value*value);
    value = (value / KSinMagic)>>FIX_PT_BITS;
    return FixMul(aRadians-value,0xF2E0);
	}

EXPORT_C TInt HuiFixMath::FixCos(TInt aRadians)
	{
	return FixSin(KFixPi_2-aRadians);
	}
	
EXPORT_C TUint HuiFixMath::FixHypot(TInt aVal1,TInt aVal2)
    {
    TUint64 r;    
    TUint p = Abs(aVal1);
    TUint q = Abs(aVal2);
    if (q == 0)
        {
        return p;
        }
    if (p<q)
        {
        SWAP(p,q)
        }
    for (;;)
        {
        r = ((TUint64(q))<<FIX_PT_BITS)/p;
        r = (r*r) >> FIX_PT_BITS;
        if (r == 0)
            {
            return p;
            }
        r = (r<<FIX_PT_BITS)/(0x40000+r);
        p = p + 2*TUint(((r*p)>>FIX_PT_BITS));
        q = (TUint((q*r))) >> FIX_PT_BITS;
        }
    } 
