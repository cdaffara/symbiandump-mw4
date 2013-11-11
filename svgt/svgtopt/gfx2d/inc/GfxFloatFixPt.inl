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
* Description:  Graphics Extension Library header file
*
*/

extern "C" {
#include "Gfxmath.h"
}
#ifdef SVG_FLOAT_BUILD
// ==========================================================================
// Constructor, default to zero
// ==========================================================================
inline TFloatFixPt::TFloatFixPt() : iValue( 0 )
    {
    }

// ==========================================================================
// Constructor
// ==========================================================================

inline TFloatFixPt::TFloatFixPt( TInt aVal )
    {
    iValue = float(aVal);
    }

// ==========================================================================
// Constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( TInt32 aVal )
    {
    iValue = float(aVal);
    }

// ==========================================================================
// Constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( TReal32 aVal )
{
    iValue = aVal;
}

// ==========================================================================
// Copy constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( const TFloatFixPt& aFixpt )
    {
    iValue = aFixpt.iValue;
    }

// ==========================================================================
// Conversion to TInt32 operator
// ==========================================================================
inline TFloatFixPt::operator TInt32() const
    {
    return ((TInt32)iValue);
    }

// ==========================================================================
// Conversion to TInt operator
// ==========================================================================
inline TFloatFixPt::operator TInt() const
    {
    return ((TInt)iValue);
    }

// ==========================================================================
// Conversion to TReal32 operator
// ==========================================================================
inline TFloatFixPt::operator TReal32() const
{
return iValue;
}

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TInt32 aVal )

    {
    iValue = (float)aVal;
    return *this;
    }

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TInt aVal )
    {
    iValue = (float)aVal;
    return *this;
    }

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TReal32 aVal )
{
    iValue = aVal;
    return *this;
}

// ==========================================================================
// Addition Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator+=( const TFloatFixPt& aVal )
    {
    iValue += aVal.iValue;
    return*this;
    }

// ==========================================================================
// Subtraction Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator-=( const TFloatFixPt& aVal )
    {
    iValue -= aVal.iValue;
    return *this;
    }


// ==========================================================================
// Multipication Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator*=( const TFloatFixPt& aVal )
{	
		iValue *= aVal.iValue;
    return *this;
}



// ==========================================================================
// Division Assignment operator
// Note:  This may require more optimization...
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator/=( const TFloatFixPt& aVal )
{
		iValue /= aVal.iValue;
    return *this;
}

// ===================================================================
// Addition operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator+( const TFloatFixPt& aVal ) const
    {
    return TFloatFixPt( iValue + aVal.iValue );
    }

// ===================================================================
// Addition operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator+( const double& aVal ) const
    {
    return TFloatFixPt( iValue + (float)aVal );
    }
    
// ===================================================================
// Addition operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator+( const float& aVal ) const
    {
    return TFloatFixPt( iValue + aVal );
    }
        
// ==========================================================================
// Subtraction operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator-( const TFloatFixPt& aVal ) const
    {
    return TFloatFixPt( iValue - aVal.iValue );
    }

// ==========================================================================
// Subtraction operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator-( const double aVal ) const
    {
    float difference = iValue - aVal;
    return TFloatFixPt( difference );
    }

// ==========================================================================
// Subtraction operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator-( const float aVal ) const
    {
    return TFloatFixPt( iValue - aVal );
    }
        
// ==========================================================================
// Multiplication operator
// ==========================================================================

inline TFloatFixPt TFloatFixPt::operator*( const TFloatFixPt& aVal ) const
    {
    TFloatFixPt tmp( iValue );
    tmp *= aVal;
    return tmp;
    }

// ==========================================================================
// Multiplication operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator*( const double aVal ) const
    {
    float multans = iValue * aVal;
    return TFloatFixPt(multans);
    }

// ==========================================================================
// Multiplication operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator*( const float aVal ) const
    {
    return TFloatFixPt(iValue * aVal);
    }
        
// ==========================================================================
// Division operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator/( const TFloatFixPt& aVal ) const
    {
    TFloatFixPt tmp( iValue );
    tmp /= aVal;
    return tmp;
    }

// ==========================================================================
// Division operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator/( const double aVal ) const
    {
    float divans = iValue / aVal;
    return TFloatFixPt(divans);
    }

// ==========================================================================
// Division operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator/( const float aVal ) const
    {
    return TFloatFixPt(iValue / aVal);
    }
        
// ==========================================================================
// Equals operator
// ==========================================================================
inline TInt TFloatFixPt::operator==( const TFloatFixPt& aVal ) const
    {
    return ( iValue == aVal.iValue );
    }

// ==========================================================================
// Not Equals operator
// ==========================================================================
inline TInt TFloatFixPt::operator!=( const TFloatFixPt& aVal ) const
    {
    return ( iValue != aVal.iValue );
    }

// ==========================================================================
// Greater than or equal to operator
// ==========================================================================
inline TInt TFloatFixPt::operator>=( const TFloatFixPt& aVal ) const
    {
    return ( iValue >= aVal.iValue );
    }

// ==========================================================================
// Less than or equal to operator
// ==========================================================================
inline TInt TFloatFixPt::operator<=( const TFloatFixPt& aVal ) const
    {
    return ( iValue <= aVal.iValue );
    }

// ==========================================================================
// Greater than operator
// ==========================================================================
inline TInt TFloatFixPt::operator>( const TFloatFixPt& aVal ) const
    {
    return ( iValue > aVal.iValue );
    }

// ==========================================================================
// Less than operator
// ==========================================================================
inline TInt TFloatFixPt::operator<( const TFloatFixPt& aVal ) const
    {
    return ( iValue < aVal.iValue );
    }

// ==========================================================================
// Round to the nearest whole number
// ==========================================================================
inline TInt32 TFloatFixPt::Round()
    {
    TInt aInt = (TInt)iValue;
    return (TInt32)aInt;
    }

// ==========================================================================
// Get 32-bit representation
// ==========================================================================
inline TInt32 TFloatFixPt::RawData()
    {
    return ((TInt32)iValue);
    }


#else

#define FIXPT_USE_INTS

// ==========================================================================
// Constructor, default to zero
// ==========================================================================
inline TFloatFixPt::TFloatFixPt() : iValue( 0 )
    {
    }

// ==========================================================================
// Constructor
// ==========================================================================

inline TFloatFixPt::TFloatFixPt( TInt aVal )
    {
    iValue = ( aVal << KFixPtFrac );
    }

inline TFloatFixPt::TFloatFixPt( TInt aVal, TBool /*a*/ )
    {
    iValue = aVal;
    }

// ==========================================================================
// Constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( TInt32 aVal )
    {
    iValue = ( aVal << KFixPtFrac );
    }

// ==========================================================================
// Constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( TReal32 aVal )
{
    iValue = svgScalarFromFloat(aVal);//( TInt32 ) ( aVal * KFixPtFracVal );
}

// ==========================================================================
// Copy constructor
// ==========================================================================
inline TFloatFixPt::TFloatFixPt( const TFloatFixPt& aFixpt )
    {
    iValue = aFixpt.iValue;
    }

// ==========================================================================
// Return the fractional portion as an integer
// ==========================================================================
inline TInt32 TFloatFixPt::FracAsInt()
    {
    return iValue & 0xFFFF;
    }

// ==========================================================================
// Conversion to TInt32 operator
// ==========================================================================
inline TFloatFixPt::operator TInt32() const
    {
    return ( iValue >> KFixPtFrac );
    }

// ==========================================================================
// Conversion to TInt operator
// ==========================================================================
inline TFloatFixPt::operator TInt() const
    {
    return ( iValue >> KFixPtFrac );
    }

// ==========================================================================
// Conversion to TReal32 operator
// ==========================================================================
inline TFloatFixPt::operator TReal32() const
{
#ifdef FIXPT_USE_INTS
    return svgFixedToFloat(iValue);
#else
    return ( ( TReal32 ) iValue ) / KFixPtFracVal;
#endif
}

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TInt32 aVal )

    {
    iValue = ( aVal << KFixPtFrac );
    return *this;
    }

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TInt aVal )
    {
    iValue = ( aVal << KFixPtFrac );
    return *this;
    }

// ==========================================================================
// Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator=( TReal32 aVal )
{
    iValue = svgScalarFromFloat(aVal);//( TInt32 ) ( aVal * KFixPtFracVal );
    return *this;
}

// ==========================================================================
// Addition Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator+=( const TFloatFixPt& aVal )
    {
    iValue += aVal.iValue;
    return*this;
    }

// ==========================================================================
// Subtraction Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator-=( const TFloatFixPt& aVal )
    {
    iValue -= aVal.iValue;
    return *this;
    }


// ==========================================================================
// Multipication Assignment operator
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator*=( const TFloatFixPt& aVal )
{
#ifdef FIXPT_USE_INTS
    iValue = svgScalarMul(iValue, aVal.iValue);
    return *this;
#else
    TInt64 rslt( ( TInt ) iValue ), val( ( TInt ) aVal.iValue );
    rslt *= val;
    iValue = ( I64HIGH(rslt) << KFixPtFrac ) | ( I64LOW ( rslt)  >> KFixPtFrac );

    return *this;
#endif
}



// ==========================================================================
// Division Assignment operator
// Note:  This may require more optimization...
// ==========================================================================
inline TFloatFixPt& TFloatFixPt::operator/=( const TFloatFixPt& aVal )
{
#ifdef FIXPT_USE_INTS
    iValue = svgScalarDiv(iValue, aVal.iValue);
    return *this;
#else
    TInt64 z = MAKE_TINT64( iValue ,0 )  ;
	TInt64 temp = aVal.iValue ;
    z /= temp;
    z >>= KFixPtFrac;
    iValue = I64INT ( z );
    return *this;
#endif
}

// ===================================================================
// Addition operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator+( const TFloatFixPt& aVal ) const
    {
    return TFloatFixPt( iValue + aVal.iValue, ETrue );
    }
// ==========================================================================
// Subtraction operation
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator-( const TFloatFixPt& aVal ) const
    {
    return TFloatFixPt( iValue - aVal.iValue, ETrue );
    }
    
// ==========================================================================
// Multiplication operator
// ==========================================================================

inline TFloatFixPt TFloatFixPt::operator*( const TFloatFixPt& aVal ) const
    {
    TFloatFixPt tmp( iValue, ETrue );
    tmp *= aVal;
    return tmp;
    }
// ==========================================================================
// Division operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator/( const TFloatFixPt& aVal ) const
    {
    TFloatFixPt tmp( iValue, ETrue );
    tmp /= aVal;
    return tmp;
    }
// ==========================================================================
// Equals operator
// ==========================================================================
inline TInt TFloatFixPt::operator==( const TFloatFixPt& aVal ) const
    {
    return ( iValue == aVal.iValue );
    }

// ==========================================================================
// Not Equals operator
// ==========================================================================
inline TInt TFloatFixPt::operator!=( const TFloatFixPt& aVal ) const
    {
    return ( iValue != aVal.iValue );
    }

// ==========================================================================
// Greater than or equal to operator
// ==========================================================================
inline TInt TFloatFixPt::operator>=( const TFloatFixPt& aVal ) const
    {
    return ( iValue >= aVal.iValue );
    }

// ==========================================================================
// Less than or equal to operator
// ==========================================================================
inline TInt TFloatFixPt::operator<=( const TFloatFixPt& aVal ) const
    {
    return ( iValue <= aVal.iValue );
    }

// ==========================================================================
// Greater than operator
// ==========================================================================
inline TInt TFloatFixPt::operator>( const TFloatFixPt& aVal ) const
    {
    return ( iValue > aVal.iValue );
    }

// ==========================================================================
// Less than operator
// ==========================================================================
inline TInt TFloatFixPt::operator<( const TFloatFixPt& aVal ) const
    {
    return ( iValue < aVal.iValue );
    }
// ==========================================================================
// Left shift operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator<<( const TInt aN ) const
    {
    TFloatFixPt tmp( iValue<<aN, ETrue );
    return tmp;
    }
// ==========================================================================
// Right shift operator
// ==========================================================================
inline TFloatFixPt TFloatFixPt::operator>>( const TInt aN ) const
    {
    TFloatFixPt tmp( iValue>>aN, ETrue );
    return tmp;
    }

// ==========================================================================
// Round to the nearest whole number
// ==========================================================================
inline TInt32 TFloatFixPt::Round()
    {
    if ( 0 != ( iValue & 0x8000 ) )
        return ( iValue >> KFixPtFrac ) + 1;
    else
        return ( iValue >> KFixPtFrac );
    }

// ==========================================================================
// Get 32-bit representation
// ==========================================================================
inline TInt32 TFloatFixPt::RawData()
    {
    return iValue;
    }

#endif
