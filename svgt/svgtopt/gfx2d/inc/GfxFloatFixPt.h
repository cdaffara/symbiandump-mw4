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


#ifndef GFXFIXPT_H
#define GFXFIXPT_H
#include <VG/openvg.h>
// Enumeration
//#define SVG_FLOAT_BUILD

enum TGfxTextAnchor
    {
    EGfxTextAnchorStart,
    EGfxTextAnchorMiddle,
    EGfxTextAnchorEnd,
    EGfxTextAnchorNone = -1
    };

enum TGfxTextDecoration
    {
    EGfxTextDecorationReserved,
    EGfxTextDecorationUnderLine,
    EGfxTextDecorationOverLine,
    EGfxTextDecorationLineThrough,
    EGfxTextDecorationNone = -1
    };

enum TGfxCapType
    {
    EGfxCapButt = 0,
    EGfxCapRound = 1,
    EGfxCapSquare = 2
    };

enum TGfxFilterEdgeCondition
    {
    EGfxEdgeNoOp,
    EGfxEdgeZeroFill,
    EGfxEdgeMirror
    };

// Image interporation enumeration
enum TGfxImageInterpolation
    {
    EGfxInterpolationBiLinear,
    EGfxInterpolationBiCubic,
    EGfxInterpolationNearestNeighbor
    };

// Join type enumeration
enum TGfxJoinType
    {
    EGfxJoinMiter = 0,
    EGfxJoinRound = 1,
    EGfxJoinBevel = 2
    };

enum TGfxWindingRule
    {
    EGfxWindEvenOdd,
    EGfxWindNonZero
    };

enum TGfxSegType
    {
    EGfxSegMoveTo=VG_MOVE_TO_ABS,
    EGfxSegLineTo=VG_LINE_TO_ABS,
    EGfxSegQuadTo=VG_QUAD_TO_ABS,
    EGfxSegCubicTo=VG_CUBIC_TO_ABS,
    EGfxSegClose=VG_CLOSE_PATH
    };

#ifdef SVG_FLOAT_BUILD

typedef float           SVGReal;

class TFloatFixPt
    {
    public:

        static void  GetString( float aFloat, TDes& aBuf );
       
        static float ConvertString( const TDesC& aVal );

	   	  static TInt ConvertString( const TDesC& aString, float& aValue );

		    static TInt ConvertString( const TDesC& aString, TFloatFixPt& aValue );
		 
        static float Sqrt( float aValue );
		
	    static float Cos(float angle); 
	   
	    static float SinFloatDouble(float angle);
	   
	    static float CosFloatDouble(float angle);
	   
	   static float CosineDouble(float x);
	   
	    static float TanFloatDouble(float angle);
	   
	    static float SinApprox(float angle);
	   
	    static float CosApprox(float angle);
    
      static float FastSin( const float val );
    
      static float FastCos( const float val );
    
      static float FastTan( const float val );
       
      static float FastASin( const float val );
       
      static float FastACos( const float val );
      
      static float FastATan( const float val );
     
     inline TFloatFixPt();

     inline TFloatFixPt( TInt aVal );

     inline TFloatFixPt( TInt32 aVal );

     inline TFloatFixPt( TReal32 aVal );

     inline TFloatFixPt( const TFloatFixPt& aFixpt );

     inline                  operator TInt32() const;

     inline                  operator TInt() const;

     inline                  operator TReal32() const;

     inline TFloatFixPt&          operator=( TInt32 aVal );

     inline TFloatFixPt&          operator=( TInt aVal );

     inline TFloatFixPt&          operator=( TReal32 aVal );

     inline TFloatFixPt&          operator+=( const TFloatFixPt& aVal );

     inline TFloatFixPt&          operator-=( const TFloatFixPt& aVal );

     inline TFloatFixPt&          operator*=( const TFloatFixPt& aVal );

     inline TFloatFixPt&          operator/=( const TFloatFixPt& aVal );

     inline TFloatFixPt           operator+( const TFloatFixPt& aVal ) const;
		 
		 inline TFloatFixPt           operator+( const double& aVal ) const;
		 
		 inline TFloatFixPt           operator+( const float& aVal ) const;
		 
     inline TFloatFixPt           operator-( const TFloatFixPt& aVal ) const;

		 inline TFloatFixPt           operator-( const double aVal ) const;
		
		 inline TFloatFixPt           operator-( const float aVal ) const;
		 	
     inline TFloatFixPt           operator*( const TFloatFixPt& aVal ) const;
	
		 inline TFloatFixPt           operator*( const double aVal ) const;
		 
		 inline TFloatFixPt           operator*( const float aVal ) const;
		 
     inline TFloatFixPt           operator/( const TFloatFixPt& aVal ) const;
	
		 inline TFloatFixPt           operator/( const double aVal ) const;
		 
		 inline TFloatFixPt           operator/( const float aVal ) const;
		 
     inline TInt		              operator==( const TFloatFixPt& aVal ) const;

     inline TInt             			operator!=( const TFloatFixPt& aVal ) const;

     inline TInt             			operator>=( const TFloatFixPt& aVal ) const;

     inline TInt		              operator<=( const TFloatFixPt& aVal ) const;
     
     inline TInt			            operator>( const TFloatFixPt& aVal ) const;

     inline TInt			            operator<( const TFloatFixPt& aVal ) const;

        /**
         * Return the number rounded to the next whole number.
         *
         * @since 1.0
         * @return
         */
        inline TInt32           Round();

        /**
         * Return the 32-bit data for this TFixPt.
         *
         * @since 1.0
         * @return 32-bit value
         */
        inline TInt32           RawData();

        /**
         * Return the absolute value for the given number.
         *
         * @since 1.0
         * @param aValue : real number to find absolute value
         * @return absolute value
         */
         TFloatFixPt         Abs( TFloatFixPt& aValue );


        /**
         * Return the square root value for this number.
         *
         * @since 1.0
         * @param aValue : real number to find square root
         * @return square root value
         */
         static TFloatFixPt  Sqrt( TFloatFixPt aValue );
        
     public:

     float                    iValue;    // Must be the first data member of this class!
        
    };

#include "GfxFloatFixPt.inl"

#else	/*FIXED POINT BUILD*/

typedef int             SVGReal;

const TInt32 KFixPtFrac = 16;
const TInt32 KFixPtFracVal = ( 1 << KFixPtFrac );

/**
 * Implementation of a real number using 16.16 bits.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TFloatFixPt
    {
    public:

        /**
         * Construct a TFixPt.  The default value is 0.0.
         *
         * @since 1.0
         * @return
         */
        inline TFloatFixPt();

        /**
         * Construct a TFixPt from the given integer value.
         *
         * @since 1.0
         * @param aVal : integer value
         * @return
         */
        inline TFloatFixPt( TInt aVal );

        /**
         * Construct a TFixPt from the given integer value.
         *
         * @since 1.0
         * @param aVal : integer value
         * @return
         */
        inline TFloatFixPt( TInt32 aVal );

        /**
         * Construct a TFixPt from the given real 32-bit int.
         *
         * @since 1.0
         * @param aVal : integer value
         * @param aBool : not used.
         * @return
         */
        inline TFloatFixPt( TInt aVal, TBool aBool );

        /**
         * Construct a TFixPt from the given TReal32.
         *
         * @since 1.0
         * @param aVal : value to copy from
         * @return
         */
        inline TFloatFixPt( TReal32 aVal );

        /**
         * Construct a TFixPt from the given TFixPt.
         *
         * @since 1.0
         * @param aFixpt : value to copy from
         * @return
         */
        inline TFloatFixPt( const TFloatFixPt& aFixpt );


        /**
         * Return the fractional portion as an int representation.
         *
         * @since 1.0
         * @return the fractional portion value
         */
        inline TInt32 FracAsInt();


        /**
         * Casting operator to TInt32.
         *
         * @since 1.0
         * @return
         */

        inline                  operator TInt32() const;

        /**
         * Casting operator to TInt.
         *
         * @since 1.0
         * @return
         */
        inline                  operator TInt() const;

        /**
         * Casting operator to TReal32.
         *
         * @since 1.0
         * @return
         */
        inline                  operator TReal32() const;

        /**
         * Assignment operator.
         *
         * @since 1.0
         * @param aVal : value to assign from.
         * @return
         */
        inline TFloatFixPt&          operator=( TInt32 aVal );

        /**
         * Assignment operator.
         *
         * @since 1.0
         * @param aVal : value to assign from.
         * @return
         */
        inline TFloatFixPt&          operator=( TInt aVal );

        /**
         * Assignment operator.
         *
         * @since 1.0
         * @param aVal : value to assign from.
         * @return
         */
        inline TFloatFixPt&          operator=( TReal32 aVal );

        /**
         * Addition assignment operator.
         *
         * @since 1.0
         * @param aVal : value to add with.
         * @return
         */
        inline TFloatFixPt&          operator+=( const TFloatFixPt& aVal );

        /**
         * Subtraction assignment operator.
         *
         * @since 1.0
         * @param aVal : value to substract.
         * @return
         */
        inline TFloatFixPt&          operator-=( const TFloatFixPt& aVal );

        /**
         * Multiplication assignment operator.
         *
         * @since 1.0
         * @param aVal : value to multiply with.
         * @return
         */
        inline TFloatFixPt&          operator*=( const TFloatFixPt& aVal );

        /**
         * Division assignment operator.
         *
         * @since 1.0
         * @param aVal : value to divide with.
         * @return
         */
        inline TFloatFixPt&          operator/=( const TFloatFixPt& aVal );


        /**
         * Addition operator.
         *
         * @since 1.0
         * @param aVal : value to add with.
         * @return
         */
        inline TFloatFixPt           operator+( const TFloatFixPt& aVal ) const;

        /**
         * Subtraction operator.
         *
         * @since 1.0
         * @param aVal : value to subtract.
         * @return
         */
        inline TFloatFixPt           operator-( const TFloatFixPt& aVal ) const;
			
        /**
         * Multiplication operator.
         *
         * @since 1.0
         * @param aVal : value to multiply with.
         * @return
         */

        inline TFloatFixPt           operator*( const TFloatFixPt& aVal ) const;

        /**
         * Division operator.
         *
         * @since 1.0
         * @param aVal : value to divide with.
         * @return
         */
        inline TFloatFixPt           operator/( const TFloatFixPt& aVal ) const;

        /**
         * Equal operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if equal
         */

        inline TInt             operator==( const TFloatFixPt& aVal ) const;

        /**
         * Not Equals operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if not equal
         */
        inline TInt             operator!=( const TFloatFixPt& aVal ) const;

        /**
         * Greater than or equal to operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if greater than or equal to
         */
        inline TInt             operator>=( const TFloatFixPt& aVal ) const;

        /**
         * Less than or equal to operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if less than or equal to
         */
        inline TInt             operator<=( const TFloatFixPt& aVal ) const;

        /**
         * Greater than operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if greater than
         */
        inline TInt             operator>( const TFloatFixPt& aVal ) const;

        /**
         * Less than operator.
         *
         * @since 1.0
         * @param aVal : value to compare.
         * @return non-zero if less than
         */
        inline TInt             operator<( const TFloatFixPt& aVal ) const;

		/**
         * Operator to left shift
         *
         * @since 1.0
         * @param aN : bits to be shifted.
         * @return
         */

        inline TFloatFixPt           operator<<( const TInt aN ) const;

        /**
         * operator to right shift.
         *
         * @since 1.0
         * @param aBuf : string output storage.
         * @return value obtained after shifeting
         */

        inline TFloatFixPt           operator>>( const TInt aN ) const;



		/**
         * Get the string representation of this real number.
         *
         * @since 1.0
         * @param aN : bits to be shifted
         * @return value obtained after shifeting
         */
         void           GetString( TDes& aBuf ) const;

        /**
         * Parse the given string for a TFixPt.
         *
         * @since 1.0
         * @param aVal : string containing a real number.
         * @return TFixPt equal to string representation.
         */
         static TFloatFixPt  ConvertString( const TDesC& aVal );

        /**
         * Parse the given string for a TFixPt.
         *
         * @since 1.0
         * @param aVal : string containing a real number.
         * @param aVal : parsed number.
         * @return Error code (KErrNone if no errors).
         */
         static TInt ConvertString( const TDesC& aValueString, TFloatFixPt& aValue );

        /**
         * Return the number rounded to the next whole number.
         *
         * @since 1.0
         * @return
         */
        inline TInt32           Round();

        /**
         * Return the 32-bit data for this TFixPt.
         *
         * @since 1.0
         * @return 32-bit value
         */
        inline TInt32           RawData();

        /**
         * Return the absolute value for the given number.
         *
         * @since 1.0
         * @param aValue : real number to find absolute value
         * @return absolute value
         */
         TFloatFixPt         Abs( TFloatFixPt& aValue );


        /**
         * Return the square root value for this number.
         *
         * @since 1.0
         * @param aValue : real number to find square root
         * @return square root value
         */
         static TFloatFixPt  Sqrt( TFloatFixPt aValue );

        /**
         * Return the square root value for the given number.
         *
         * @since 1.0
         * @param aValue : 32-bit representation of real number
         * @param aFracbits: number of bits allocated for fractional portion.
         * @return square root value
         */
         static TInt32  FixedSqrtGeneral( TInt32 aValue, TUint32 aFracbits );

    private:

        /**
         * Optimized method to do multiplication using assembly.
         *
         * @since 1.0
         * @param aValue : real number to find multiply with
         * @return
         */
        inline void             GfxMultiplyAsm( TInt aValue );

	public:

        TInt                    iValue;    // Must be the first data member of this class!

    };

#ifdef SVG_FLOAT_BUILD
#define KFloatFixOne = (TFloatFixPt(1))
#define KFloatFixZero = (TFloatFixPt())
#define KMAXFLOATFIXVALUE (1e38f)
#define KMINFLOATFIXVALUE (-1e38f)
#define KMAXFLOATFIX (TFloatFixPt( KMAXFLOATFIXVALUE ))
#define KMINFLOATFIX (TFloatFixPt( KMINFLOATFIXVALUE ))
#else
#define KFloatFixOne (TFloatFixPt( 0x10000, ETrue ))
#define KFloatFixZero (TFloatFixPt())
#define KMAXFLOATFIXVALUE  (0x7fff)
#define KMINFLOATFIXVALUE (-0x7fff)
#define KMAXFLOATFIX (TFloatFixPt( 0x7fff0000, ETrue ))
#define KMINFLOATFIX (TFloatFixPt( -0x7fff0000, ETrue ))

#endif

typedef struct
    {
    SVGReal         matrix[2][3];
    } SVGMatrix2x3;
    
#include "GfxFloatFixPt.inl"

#endif
#endif      // GFXFIXPT_H
