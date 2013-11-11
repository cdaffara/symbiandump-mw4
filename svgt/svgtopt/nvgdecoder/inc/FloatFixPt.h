/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder header file
*
*/

#ifndef NVGFLOATFIX_H_
#define NVGFLOATFIX_H_

#include <e32def.h>
#include <s32mem.h>

const TInt32 KFixPtFrac = 16;
const TInt32 KFixPtFracVal = (1 << KFixPtFrac);
#   define svgFloatBits(f) (*(int*)&f)

inline int svgScalarFromFloat(float f)
    {
    int a;
    int sign;
    int exponent;
    
    int r;
    
    
    a           = svgFloatBits(f);
    sign        = a >> 31;
    exponent    = (127 + 15) - ((a >> 23) & 0xff);
    
    r = (int)((((int)(a) << 8) | (1U << 31)) >> exponent);
    r &= ((exponent - 32) >> 31);
    
    r = (r ^ sign) - sign;
    
    return r;
    
    }
class TFloatFixPt
    {
    public:
        
        /**
         * Construct a TFixPt.  The default value is 0.0.
         *
         * @since 1.0
         * @return
         */
        TFloatFixPt();

        /**
         * Construct a TFixPt from the given integer value.
         *
         * @since 1.0
         * @param aVal : integer value
         * @return
         */
        TFloatFixPt(TInt aVal);
        TFloatFixPt(TInt32 aVal);
        TFloatFixPt(TReal32 aVal);
        TFloatFixPt(TInt aVal, TBool aBool);
        TFloatFixPt&   operator=(TReal32 aVal);
        TFloatFixPt           operator+(const TFloatFixPt& aVal) const;
        operator TReal32() const;
        TInt32           Round();
        TInt32           RawData();
        void copyfloatfix(TInt aVal);
    public:
        TInt                    iValue;    // Must be the first data member of this class!
        
    };

#endif

