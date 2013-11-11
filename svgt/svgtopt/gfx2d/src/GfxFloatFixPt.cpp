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


#include <e32std.h>
#include "GfxFloatFixPt.h"


#ifdef SVG_FLOAT_BUILD
#include <e32math.h>

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 void TFloatFixPt::GetString( float aFloat, TDes& aBuf )
    {
    TRealFormat rf( 10, 3 );
    rf.iType = KRealFormatFixed | KDoNotUseTriads;
    aBuf.Num( ( aFloat ), rf );
    }

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::ConvertString( const TDesC& aVal )
    {
    TLex aString( aVal );
    TReal32 val;
    aString.SkipSpace();
    aString.Val( val, '.' );
    return float( val );
    }

// --------------------------------------------------------------------------
// Error code is returned
// ---------------------------------------------------------------------------
 TInt TFloatFixPt::ConvertString( const TDesC& aString, float& aValue )
    {
    TLex lex( aString );
    lex.SkipSpace();
    return lex.Val( aValue, '.' );
    }

// --------------------------------------------------------------------------
// Error code is returned
// ---------------------------------------------------------------------------
 TInt TFloatFixPt::ConvertString( const TDesC& aString, TFloatFixPt& aValue )
    {
    TLex lex( aString );
    lex.SkipSpace();
    return lex.Val( aValue.iValue, '.' );
    }
    
// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::Sqrt( float number )
{   
    long i;
    float x, y;
    const float f = 1.5F;

    x = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( f - ( x * y * y ) );
    y  = y * ( f - ( x * y * y ) );
    return number * y;
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::SinFloatDouble(float angle)
{
	double sn = 0;
	Math::Sin(sn, double(angle));
	return float(sn);	
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::CosFloatDouble(float angle)
{
	//double cs = 0;
	float cs = 0;
	//Math::Cos(cs, double(angle));
	//cs = CosineDouble(double(angle));
	cs = CosineDouble(angle);
	return float(cs);
}

float TFloatFixPt::CosineDouble(float x)
{
float p0,p1,p2,p3,p4,p5,y,t,absx,frac,quad,pi2;
p0= 0.999999999781;
p1=-0.499999993585;
p2= 0.041666636258;
p3=-0.0013888361399;
p4= 0.00002476016134;
p5=-0.00000026051495;
pi2=1.570796326794896; 		/* pi/2 */
absx=x;
if (x<0) absx=-absx; 	     /* absolute value of input */
quad=(int) (absx/pi2);       	/* quadrant (0 to 3) */
frac= (absx/pi2) - quad;     	/* fractional part of input */
if(quad==0) t=frac * pi2;
if(quad==1) t=(1-frac) * pi2;
if(quad==2) t=frac * pi2;
if(quad==3) t=(frac-1) * pi2;
t=t * t;
y=p0 + (p1*t) + (p2*t*t) + (p3*t*t*t) + (p4*t*t*t*t) + (p5*t*t*t*t*t);
if(quad==2 | quad==1) y=-y;  /* correct sign */
return(y);
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::TanFloatDouble(float angle)
{
	double tn = 0;
	Math::Tan(tn, double(angle));
	return float(tn);
}


// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::SinApprox(float angle)
{
	float c = 0.70710678118654752440f;
	return ((2 - 4 * c) * angle * angle + c + angle);	
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::CosApprox(float angle)
{
	float c = 0.70710678118654752440f;
	return ((2 - 4 * c) * angle * angle + c - angle);
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastSin(const float val)
{
    float fASqr = val*val;
    float fResult = -2.39e-08f;
    fResult *= fASqr;
    fResult += 2.7526e-06f;
    fResult *= fASqr;
    fResult -= 1.98409e-04f;
    fResult *= fASqr;
    fResult += 8.3333315e-03f;
    fResult *= fASqr;
    fResult -= 1.666666664e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    fResult *= val;

    return fResult;
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastCos(const float val)
{
    float fASqr = val*val;
    float fResult = -2.605e-07f;
    fResult *= fASqr;
    fResult += 2.47609e-05f;
    fResult *= fASqr;
    fResult -= 1.3888397e-03f;
    fResult *= fASqr;
    fResult += 4.16666418e-02f;
    fResult *= fASqr;
    fResult -= 4.999999963e-01f;
    fResult *= fASqr;
    fResult += 1.0f;

    return fResult;  
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastTan(const float val)
{
    float fASqr = val*val;
    float fResult = 9.5168091e-03f;
    fResult *= fASqr;
    fResult += 2.900525e-03f;
    fResult *= fASqr;
    fResult += 2.45650893e-02f;
    fResult *= fASqr;
    fResult += 5.33740603e-02f;
    fResult *= fASqr;
    fResult += 1.333923995e-01f;
    fResult *= fASqr;
    fResult += 3.333314036e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    fResult *= val;

    return fResult;

}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastASin(float val)
{
    float fRoot = Sqrt(1.0f-val);
    float fResult = -0.0187293f;
    fResult *= val;
    fResult += 0.0742610f;
    fResult *= val;
    fResult -= 0.2121144f;
    fResult *= val;
    fResult += 1.5707288f;
    fResult = 1.57079632679489661923 - fRoot*fResult;

    return fResult;
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastACos(float val)
{
    float fRoot = Sqrt(1.0f-val);
    float fResult = -0.0187293f;
    fResult *= val;
    fResult += 0.0742610f;
    fResult *= val;
    fResult -= 0.2121144f;
    fResult *= val;
    fResult += 1.5707288f;
    fResult *= fRoot;

    return fResult;
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::FastATan(float val)
{
	float fVSqr = val*val;
    float fResult = 0.0028662257f;
    fResult *= fVSqr;
    fResult -= 0.0161657367f;
    fResult *= fVSqr;
    fResult += 0.0429096138f;
    fResult *= fVSqr;
    fResult -= 0.0752896400f;
    fResult *= fVSqr;
    fResult += 0.1065626393f;
    fResult *= fVSqr;
    fResult -= 0.1420889944f;
    fResult *= fVSqr;
    fResult += 0.1999355085f;
    fResult *= fVSqr;
    fResult -= 0.3333314528f;
    fResult *= fVSqr;
    fResult += 1.0f;
    fResult *= val;

    return fResult;
}

// --------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
 float TFloatFixPt::Cos(float angle) 
/* computes cos of x (x in radians) by an expansion */
{
   float result = 1.0;
   int factor = 1;  
   float power = angle;
   
   for ( int i = 2; i <= 10; i++ ) 
   {
      factor = factor * i;  
      power = power * angle;
      
      if ( (i & 1) == 0 )
      {
        if ( (i & 3) == 0 )
        {
        	result += power/factor;
        }
        else
        {
        	result -= power/factor;
        } 
      }
   }
   return (result);
}

// --------------------------------------------------------------------------
//  TFloatFixPt TFloatFixPt::Sqrt( TFloatFixPt aA )
// ---------------------------------------------------------------------------
 TFloatFixPt TFloatFixPt::Sqrt( TFloatFixPt aA )
    {
    TFloatFixPt tmp;
    tmp.iValue = Sqrt( aA.iValue );
    return tmp;
    }
 
// --------------------------------------------------------------------------
//  TFloatFixPt TFloatFixPt::Abs( TFixPt& aA )
// ---------------------------------------------------------------------------
 TFloatFixPt TFloatFixPt::Abs( TFloatFixPt& aA )
    {
    return ( aA.iValue > 0.0f ) ? TFloatFixPt( aA.iValue ) : ( TFloatFixPt( -aA.iValue ) );
    }
 
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------        
#else /*FIXED POINT BUILD*/
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
//  void TFixPt::GetString( TDes& aBuf ) const
// ---------------------------------------------------------------------------
 void TFloatFixPt::GetString( TDes& aBuf ) const
    {
    TRealFormat rf( 10, 3 );
    rf.iType = KRealFormatFixed | KDoNotUseTriads;
    aBuf.Num( ( ( TReal32 ) iValue ) / KFixPtFracVal, rf );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TFloatFixPt::ConvertString( const TDesC& aVal )
// ---------------------------------------------------------------------------
 TFloatFixPt TFloatFixPt::ConvertString( const TDesC& aVal )
    {
    TLex aString( aVal );
    TReal32 val;
    aString.SkipSpace();
    aString.Val( val, '.' );
    return TFloatFixPt( val );
    }

// --------------------------------------------------------------------------
//  TInt TFloatFixPt::ConvertString( const TDesC& aValueString, TFloatFixPt& aValue )
// ---------------------------------------------------------------------------
 TInt TFloatFixPt::ConvertString( const TDesC& aValueString, TFloatFixPt& aValue )
    {
    TLex aString( aValueString );
    TReal32 value;
    aString.SkipSpace();
    TInt errorCode = aString.Val( value, '.' );
    aValue = (float)value;
    return errorCode;
    }
// --------------------------------------------------------------------------
//  TFloatFixPt TFloatFixPt::Abs( TFixPt& aA )
// ---------------------------------------------------------------------------
 TFloatFixPt TFloatFixPt::Abs( TFloatFixPt& aA )
    {
    TFloatFixPt KZero;
    return ( aA > KZero ) ? aA : ( KZero - aA );
    }

// ==========================================================================
// fixed point SQRT from Graphics Gems
// ==========================================================================
// --------------------------------------------------------------------------
//  TFloatFixPt TFloatFixPt::Sqrt( TFloatFixPt aA )
// ---------------------------------------------------------------------------
 TFloatFixPt TFloatFixPt::Sqrt( TFloatFixPt aA )
    {
    TFloatFixPt tmp;
    tmp.iValue = FixedSqrtGeneral( aA.iValue, KFixPtFrac );
    return tmp;
    }

// ==========================================================================
// Fixed point sqrt from Graphics Gems
// ==========================================================================
// --------------------------------------------------------------------------
//  TInt32 TFloatFixPt::FixedSqrtGeneral( TInt32 aX, TUint32 aFracbits )
// ---------------------------------------------------------------------------
 TInt32 TFloatFixPt::FixedSqrtGeneral( TInt32 aX, TUint32 aFracbits )
    {
    TUint32 root, remHi, remLo, testDiv, count;
    root = 0;
    remHi = 0;
    remLo = aX;
    count = 15 + ( aFracbits >> 1 );

    do
        {
        remHi = ( remHi << 2 ) | ( remLo >> 30 );
        remLo <<= 2;
        root <<= 1;
        testDiv = ( root << 1 ) + 1;
        if ( remHi >= testDiv )
            {
            remHi -= testDiv;
            root += 1;
            }
        }
    while ( count-- != 0 );

    return root;
    }
#endif
