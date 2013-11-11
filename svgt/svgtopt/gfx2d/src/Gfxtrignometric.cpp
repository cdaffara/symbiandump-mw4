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
* Description:  Graphics Extension Library source file
*
*/




//File to implement functions to calculate trignomtric function

#include "Gfxtrignometric.h"


 /*-------------------------------------------------------------------*/
// --------------------------------------------------------------------------
// TInt32 GfxMath::hgAbs32 (TInt32 a)
// ---------------------------------------------------------------------------
TInt32 GfxMath::hgAbs32 (TInt32 a)
{

	return (a >= 0) ? a : -a;
}


/*-------------------------------------------------------------------*//*!
 * \brief	Performs signed 32x32->64 multiplication, returns high 32 bits
 * \param	a	First 32-bit signed integer
 * \param	b	Second 32-bit signed integer
 * \return	((int64)(a)*b)>>32
 *//*-------------------------------------------------------------------*/

// --------------------------------------------------------------------------
// TInt32 GfxMath::hgLsl64h_0_31 (
	// ---------------------------------------------------------------------------
	TInt32 GfxMath::hgLsl64h_0_31 (
							   // --------------------------------------------------------------------------
							   // const GfxMath::TGfxInt64	a,
							   // ---------------------------------------------------------------------------
							   const GfxMath::TGfxInt64	a,
							   TInt32			sh)
{

	return ((a.h << sh) | ((TUint32)(a.l) >> (32-sh)));
}

// --------------------------------------------------------------------------
// GfxMath::TGfxInt64 GfxMath::hgSet64(TInt32 hh,TInt32 ll)
// ---------------------------------------------------------------------------
GfxMath::TGfxInt64 GfxMath::hgSet64(TInt32 hh,TInt32 ll)
{
	TGfxInt64 a;
	a.l=ll;
	a.h=hh;
	return a;
}


// --------------------------------------------------------------------------
// GfxMath::TGfxInt64 GfxMath::hgMul64 (
	// ---------------------------------------------------------------------------
	GfxMath::TGfxInt64 GfxMath::hgMul64 (
	TInt32 a,
	TInt32 b)
{

	TUint32 hh		= (TUint32)((a>>16)   *(b>>16));
	TUint32 lh		= (TUint32)((a&0xFFFF)*(b>>16));
	TUint32 hl		= (TUint32)((a>>16)   *(b&0xFFFF));
	TUint32 ll		= (TUint32)((a&0xFFFF)*(b&0xFFFF));
	TUint32 oldlo;

	hh += (TInt32)(lh)>>16;
	hh += (TInt32)(hl)>>16;

	oldlo	=  ll;
	ll		+= lh<<16;
	if (ll < oldlo)
		hh++;

	oldlo	=  ll;
	ll		+= hl<<16;
	if (ll < oldlo)
		hh++;

	return hgSet64((TInt32)hh,(TInt32)ll);
}


// --------------------------------------------------------------------------
// GfxMath::TGfxInt64 GfxMath::hgAdd64 (
	// ---------------------------------------------------------------------------
	GfxMath::TGfxInt64 GfxMath::hgAdd64 (
	const TGfxInt64 a,
	const TGfxInt64 b)
{
	TInt32 hi		= a.h+b.h;
	TInt32 lo		= a.l+b.l;

	hi += ((TUint32)(lo) < (TUint32)(a.l)) ? 1 : 0;
	return hgSet64(hi, lo);
}




// --------------------------------------------------------------------------
// GfxMath::TGfxInt64 GfxMath::hgMadd64 (
	// ---------------------------------------------------------------------------
	GfxMath::TGfxInt64 GfxMath::hgMadd64 (
	 // --------------------------------------------------------------------------
	 // GfxMath::TGfxInt64 a,
	 // ---------------------------------------------------------------------------
	 GfxMath::TGfxInt64 a,
	TInt32 b,
	TInt32 c)
{

	return hgAdd64(a,hgMul64(b,c));
}




/*-------------------------------------------------------------------*//*!
 * \brief	Performs unsigned 32x32->64 multiplication, returns high
 *			32 bits
 * \param	a	First 32-bit  unsigned integer
 * \param	b	Second 32-bit unsigned integer
 * \return	((unsigned int64)(a)*b)>>32
 *//*-------------------------------------------------------------------*/
 // --------------------------------------------------------------------------
 // TUint32 GfxMath::hgMulu64h (
	 // ---------------------------------------------------------------------------
	 TUint32 GfxMath::hgMulu64h (
	TUint32 a,
	TUint32 b)
{

	TUint32 hh = (a>>16)	* (b>>16);
	TUint32 lh = (a&0xFFFF)* (b>>16);
	TUint32 hl = (a>>16)	* (b&0xFFFFu);
	TUint32 ll = (a&0xFFFF)* (b&0xFFFFu);
	TUint32 oldlo;

	hh += (lh>>16);
	hh += (hl>>16);

	oldlo = ll;
	ll += lh<<16;
	if (ll < oldlo)
		hh++;

	oldlo = ll;
	ll += hl<<16;
	if (ll < oldlo)
		hh++;

	return hh;
}

//-----------------------------------------------*/


 // --------------------------------------------------------------------------
 // GfxMath::TGfxInt64 GfxMath::hgMulu64 (
	 // ---------------------------------------------------------------------------
	 GfxMath::TGfxInt64 GfxMath::hgMulu64 (
	TUint32 a,
	TUint32 b)
{

	TUint32 hh = (a>>16)	* (b>>16);
	TUint32 lh = (a&0xFFFF)* (b>>16);
	TUint32 hl = (a>>16)	* (b&0xFFFFu);
	TUint32 ll = (a&0xFFFF)* (b&0xFFFFu);
	TUint32 oldlo;

	hh += (lh>>16);
	hh += (hl>>16);

	oldlo = ll;
	ll += lh<<16;
	if (ll < oldlo)
		hh++;

	oldlo = ll;
	ll += hl<<16;
	if (ll < oldlo)
		hh++;

	return hgSet64((TInt32)(hh),(TInt32)(ll));
}


// --------------------------------------------------------------------------
// TInt GfxMath::svgiRCos( TInt32 _x )
// ---------------------------------------------------------------------------
TInt GfxMath::svgiRCos( TInt32 _x )
{
	TUint32 fpi = 0x6487ED51;
	TUint32 y;
	TUint32 x = hgAbs32(_x);
	y = x;

	if(y > fpi>>1)
		y = fpi-y;

	y <<= 1;

	/* Taylor series expansion around point x */
	/* NOTE: to adjust approximation accuracy vs speed, remove or add iterations in pairs */
	{
		TUint32 t;
		TGfxInt64 c  = hgMulu64(y,y<<1);
		TInt32	x2 = hgLsl64h_0_31(c,1);
 		t = hgMulu64h(x2,x2);
		c = hgMadd64(c, t,-(0x55555555>>1)-1);
		t = hgMulu64h(t,x2);
		c = hgMadd64(c, t, (0x5B05B05B>>4));
		t = hgMulu64h(t,x2);
		c = hgMadd64(c, t,-(0x68068068>>8)+1);
		t = hgMulu64h(t,x2);
		c = hgMadd64(c, t, (0x49F93EDD>>12)+1);
		t = hgMulu64h(t,x2);
		c = hgMadd64(c, t,-(0x47BB63BF>>17));
		{
			TInt32 r = c.h;
			r = (0x40000000 - r)>>1;

			if(x > (fpi>>1))
				r =-r;

			return r;
		}
	}
}


/*=======================================================================*/
/*=======================================================================*/

// --------------------------------------------------------------------------
// TInt GfxMath::svgScalarSin( TInt r1 )
// ---------------------------------------------------------------------------
TInt GfxMath::svgScalarSin( TInt r1 )
{
	TInt32 foo;
	r1 -= 0x6487ED51>>14;
	foo = hgLsl64h_0_31(hgMul64(r1,0x517CC1B7),15);
	foo = hgMulu64h(foo,0x6487ED51);
	if( foo >= 0x6487ED51>>1 )
		foo -= 0x6487ED51>>0;
	return svgiRCos(foo<<1) >> 13;
}

/*=======================================================================*/
/*=======================================================================*/

// --------------------------------------------------------------------------
// TInt GfxMath::svgScalarCos( TInt r1 )
// ---------------------------------------------------------------------------
TInt GfxMath::svgScalarCos( TInt r1 )
{
	TInt32 foo;
	foo = hgLsl64h_0_31(hgMul64(r1,0x517CC1B7),15);
	foo = hgMulu64h(foo,0x6487ED51);
	if( foo >= 0x6487ED51>>1 )
		foo -= 0x6487ED51>>0;
	return svgiRCos(foo<<1) >> 13;
}

/*=======================================================================*/
/*=======================================================================*/

// --------------------------------------------------------------------------
// TInt GfxMath::svgScalarTan( TInt r1 )
// ---------------------------------------------------------------------------
TInt GfxMath::svgScalarTan( TInt r1 )
{
	TInt lSin = svgScalarSin(r1);
		if(!lSin)
			return 0xffffffff; //highest value possible to indicate error
	TInt lCos = svgScalarCos(r1);

		if ( !lCos  )
		{
		return 0xffffffff;
		}
	return (((lSin << 15) / lCos)<<1);
}
