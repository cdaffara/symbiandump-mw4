/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Math Utility functions
*
*/

#include "Gfxmath.h"


/*-------------------------------------------------------------------*//*!
 * \brief   Counts number of leading zeroes in an integer
 * \param   value   Integer value
 * \return  Number of leading zeroes [0,32]
 * \note    Example outputs: clz(0)=32, clz(1)=31, clz(-1)=0
 * \todo    We want to generate clz instruction for ARM if supported by the CPU
 *//*-------------------------------------------------------------------*/

static int svgClz32(unsigned int value)
    {
    int c = 0;

    if (value<(1u<<( 16 )))
        c+=( 16 );
    else
        value >>= ( 16 );;
    if (value<(1u<<( 8 )))
        c+=( 8 );
    else
        value >>= ( 8 );;
    if (value<(1u<<( 4 )))
        c+=( 4 );
    else
        value >>= ( 4 );;
    if (value<(1u<<( 2 )))
        c+=( 2 );
    else
        value >>= ( 2 );;
    c += 2 >> value;

    return c;

    }

/*-------------------------------------------------------------------*//*!
 * \brief   Returns absolute value of an integer
 * \param   a   32-bit input value
 * \return  abs(a)
 * \todo    On many platforms it might be best to use native abs() ?
 *//*-------------------------------------------------------------------*/
static int svgAbs32 (int a)
    {
    return (a >= 0) ? a : -a;
    }

static int svgGTEFloat(int lhs, int rhs)
    {
    int cmp;
    if ((lhs & rhs) >> 31)
        {
        cmp = (int)lhs <= (int)rhs;
        }
    else
        {
        cmp = (int)lhs >= (int)rhs;
        }
    return cmp;
    }

static int svgLTEFloat(int lhs, int rhs)
    {
    /* Compare signed, if both are negative, we need to invert the result */
    int cmp;
    if ((lhs & rhs) >> 31)
        {
        cmp = (int)lhs >= (int)rhs;
        }
    else
        {
        cmp = (int)lhs <= (int)rhs;
        }
    return cmp;
    }

/**
 * \brief   Clamping version of the fixed -> float conversion routine. That is, will not
 *          create incorrect results on values > ~32768.0 or < -32768.0
 *
 * \param f Input floating-point
 * \return  Fixed-point number within 16.16 fixed-point range.
 */
 int svgScalarFromFloat(float f)
    {
    int a;
    int sign;
    int exponent;

    int r;

    if (svgGTEFloat((*(int*)&f ), 0x47000000) )
        {
        return 0x7fffffff;
        }
    else
        if (svgLTEFloat((*(int*)&f ), 0xc7000000) )
            {
            return 0x80000000;
            }
        else
            {
            a = (*(int*)&f );
            sign = a >> 31;
            exponent = (127 + 15) - ((a >> 23) & 0xff);

            r = (int)((((int)(a) << 8) | (1U << 31)) >> exponent);
            r &= ((exponent - 32) >> 31);

            r = (r ^ sign) - sign;

            return r;
            }
    }

/*********************************************************************//*!
 * \internal
 *
 * \brief   Converts a 16.16 fixed point value into a floating point
 *          value
 * \param   x Fixed point value
 * \returns Corresponding floating point value
 * \note    This is essentially exactly the same routine as nglIntToFloat,
 *          except that the exponent bias value is 16 smaller (thus
 *          dividing the float by 65536). If we run out of space and
 *          want to squeeze out a few extra bytes, we could write a
 *          common routine where the scale factor is passed in as
 *          a parameter.
 *
 ************************************************************************/

float svgFixedToFloat(int x)
    {
    unsigned int d = (unsigned int)(x) & 0x80000000u; /* get copy of sign */
    int exponent;

    if (!x) /* special case handling for 0 */
        return (float)(0);

    x = svgAbs32(x); /* get rid of sign (the code works only for unsigned values -> we add the sign later back)  */

    /*exponent = nglGetHighestSetBit((unsigned int)(x))-23;*//* get exponent and divide value */
    exponent = 31 - svgClz32((unsigned int)(x)) - 23;

    if (exponent >= 0) /* "signed shift right" (we shift the mantissa to the proper place) */
        x>>=exponent;
    else
        x<<=(-exponent); /*lint !e504 *//* yes, it is an unusual shift.. */

    exponent += 127+23-16; /* convert exponent into biased form (divide by 65536 simultaneously!)  */
    exponent <<=23; /* shift exponent to proper place           */
    d |= (unsigned int)(exponent); /* combine sign with exponent               */
    x &= ((1<<23)-1); /* mask the mantissa                        */
    d |= (unsigned int)(x); /* combine sign and exponent with mantissa  */

    return *(float*)&d; /* done */
    }

/*-------------------------------------------------------------------*//*!
 * \brief   32x32->64-bit signed multiplication
 * \param   a   32-bit signed integer
 * \param   b   32-bit signed integer
 * \return  64-bit signed result of the multiplication
 *//*-------------------------------------------------------------------*/

static svgInt64 svgMul64(int a, int b)
    {
    return (svgInt64)(a) * (int)(b);
    }
/*=======================================================================*/
/*=======================================================================*/

int svgScalarMul(int r1, int r2)
    {

    svgInt64 a = svgMul64(r1, r2);
    int l = (int)(a);
    int h = (int)((svgUint64)(a)>>32);
    int r = (h << 16) | (((unsigned int)l) >> 16); /* r = 32 middle bits of the 64 bit result */
    int hs = h >> 31; /* hs is either 0 or 0xffffffff */
    /* test if there is data in the upper 17 bits of h */
    if ( (h >> 15) != hs)
        r = (~hs) ^ 0x80000000; /* saturate (~hs to avoid a big constant on ARM) */

    return r;
    }

/*=======================================================================*/
/*=======================================================================*/

int svgScalarDiv(int x, int y)
    {
    unsigned int ax = x < 0 ? -x : x;
    unsigned int ay = y < 0 ? -y : y;
    unsigned int axh = ax >> 14;
    unsigned int axl = ax << 18;
    int q = 0, i;

    if ((axh >> 1) >= ay)
        { /* saturate when overflows */
        if ( (x < 0) ^ (y < 0))
            q = 0x80000000;
        else
            q = 0x7fffffff;
        }
    else
        {
        for (i = 30; i>=0; i--)
            {
            if (axh >= ay)
                {
                axh -= ay;
                q += 1<<i;
                }
            /* lsl64 #1 */
            axh <<= 1;
            axh |= axl >> 31; /* add carry */
            axl <<= 1;
            }
        if ( (x < 0) ^ (y < 0))
            q = -q;
        }

    return q;
    }
