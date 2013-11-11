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


#include "GfxAffineTransform.h"

#include "GfxGeneralPath.h"
#include "Gfxtrignometric.h"

#include "e32debug.h"

// ==========================================================================
// Notes:
//
//    [ x']   [  m00  m01  m02  ] [ x ]   [ m00x + m01y + m02 ]
//    [ y'] = [  m10  m11  m12  ] [ y ] = [ m10x + m11y + m12 ]
//    [ 1 ]   [   0    0    1   ] [ 1 ]   [         1         ]
//
// ==========================================================================

// ==========================================================================
// Constructor
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform::TGfxAffineTransform()
// ---------------------------------------------------------------------------
 TGfxAffineTransform::TGfxAffineTransform()
   :iM00(KFloatFixOne),
    iM10(KFloatFixZero),
    iM01(KFloatFixZero),
    iM11(KFloatFixOne),
    iM02(KFloatFixZero),
    iM12(KFloatFixZero)
	{
    iTransType = KTransformIdentity;
    }


// ==========================================================================
// Constructor with elements
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform::TGfxAffineTransform( TReal32 aM00,
// ---------------------------------------------------------------------------
 TGfxAffineTransform::TGfxAffineTransform( TReal32 aM00,
                                                   TReal32 aM10,
                                                   TReal32 aM01,
                                                   TReal32 aM11,
                                                   TReal32 aM02,
                                                   TReal32 aM12 )
    {
    SetTransform( aM00, aM10, aM01, aM11, aM02, aM12 );
    }

// --------------------------------------------------------------------------
// TGfxAffineTransform::TGfxAffineTransform( TReal32 aM00,
// ---------------------------------------------------------------------------
TGfxAffineTransform::TGfxAffineTransform( TReal32 aM00,
                                          TReal32 aM10,
                                          TReal32 aM01,
                                          TReal32 aM11,
                                          TReal32 aM02,
                                          TReal32 aM12,
                                          TGfxTransformType aType )
										  :iM00(aM00),
										  iM10(aM10),
										  iM01(aM01),
										  iM11(aM11),
										  iM02(aM02),
										  iM12(aM12)
	{
    iTransType = aType;
    }

// ==========================================================================
// Generally used instance creation: Rotation matrix
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetRotateInstance( TReal32 aTheta )
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetRotateInstance( TReal32 aTheta )
    {
    TGfxAffineTransform tmp;
    tmp.SetToRotate( aTheta );
    return tmp;
    }

// ==========================================================================
// Generally used instance creation: another Rotation matrix
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetRotateInstance( TReal32 aTheta,
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetRotateInstance( TReal32 aTheta,
                                                                     const TFloatFixPt& aX,
                                                                     const TFloatFixPt& aY )
    {
    TGfxAffineTransform tmp;
    tmp.SetToRotate( aTheta );

    TFloatFixPt x = aX;
    TFloatFixPt y = aY;
    TFloatFixPt KOne( KFloatFixOne );
    tmp.iM02 = x * ( KOne - tmp.iM00 ) + y * tmp.iM10;
    tmp.iM12 = y * ( KOne - tmp.iM00 ) - x * tmp.iM10;
    tmp.iTransType |= KTransformTranslate;
    return tmp;
    }

// ==========================================================================
// Added Zoom Instance for better calculation
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetZoomInstance( sf,ax,ay);
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetZoomInstance( TReal32 aScaleFactor,
                                                                     const TFloatFixPt& aX,
                                                                     const TFloatFixPt& aY )
    {
    TFloatFixPt KOne( KFloatFixOne );
    TGfxAffineTransform tmp;
    tmp.iM00 = aScaleFactor;
    tmp.iM11 = aScaleFactor;

    TMatrixElType x = ( TMatrixElType ) aX;
    TMatrixElType y = ( TMatrixElType ) aY;
    tmp.iM02 = x * ( KOne - tmp.iM00 ); 
    tmp.iM12 = y * ( KOne - tmp.iM11 ); 
    tmp.iTransType |= KTransformTranslate | KTransformScale;
    return tmp;
    }

// --------------------------------------------------------------------------
// void TGfxAffineTransform::SetToRotate( TReal32 aTheta )
// ---------------------------------------------------------------------------
void TGfxAffineTransform::SetToRotate( TReal32 aTheta )
    {
    TFloatFixPt KZero;
	#ifdef SVG_FLOAT_BUILD
	TFloatFixPt cs = TFloatFixPt::CosFloatDouble( aTheta ); //(GfxMath::svgScalarCos((TInt32)(aTheta * TReal(0x10000))));
	TFloatFixPt sn = TFloatFixPt::SinFloatDouble( aTheta ); //(GfxMath::svgScalarSin((TInt32)(aTheta * TReal(0x10000))));
	#else
	TFloatFixPt cs (GfxMath::svgScalarCos((TInt32)(aTheta * TReal(0x10000))), ETrue);
	TFloatFixPt sn (GfxMath::svgScalarSin((TInt32)(aTheta * TReal(0x10000))), ETrue);
	#endif
    iM00 =  cs;
    iM10 =  sn;
    iM01 = KZero - sn;
    iM11 =  cs;

    iM02 = KZero;
    iM12 = KZero;
    iTransType = KTransformShear | KTransformScale;
    }

// ==========================================================================
// Generally used instance creation: Scale matrix
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetScaleInstance( TReal32 aSx,
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetScaleInstance( TReal32 aSx,
                                                                    TReal32 aSy )
    {
    return TGfxAffineTransform( ( TReal32 ) aSx,
                                0.0f,
                                0.0f,
                                ( TReal32 ) aSy,
                                0.0f,
                                0.0f,
                                KTransformScale );
    }

// ==========================================================================
// Generally used instance creation: Shear matrix
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetShearInstance( TReal32 aShx,
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetShearInstance( TReal32 aShx,
                                                                    TReal32 aShy )
    {
	#ifdef SVG_FLOAT_BUILD
	TFloatFixPt tnshx = TFloatFixPt::TanFloatDouble( aShx ); //(GfxMath::svgScalarTan(TInt(aShx*0x10000)));
	TFloatFixPt tnshy = TFloatFixPt::TanFloatDouble( aShy ); //(GfxMath::svgScalarTan(TInt(aShy*0x10000)));
	#else
	TFloatFixPt tnshx(GfxMath::svgScalarTan(TInt(aShx*0x10000)), ETrue);
	TFloatFixPt tnshy(GfxMath::svgScalarTan(TInt(aShy*0x10000)), ETrue);
	#endif

    return TGfxAffineTransform( 1.0f,
                                TReal32(tnshy),
                                TReal32(tnshx),
                                1.0f,
                                0.0f,
                                0.0f,
                                KTransformShear );
    }


// ==========================================================================
// Generally used instance creation: Translate matrix
// ==========================================================================
// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::GetTranslateInstance( const TFixPt& aTx,
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::GetTranslateInstance( const TFloatFixPt& aTx,
                                                                        const TFloatFixPt& aTy )
    {
    return TGfxAffineTransform( 1.0f,
                                0.0f,
                                0.0f,
                                1.0f,
                                ( TReal32 ) aTx,
                                ( TReal32 ) aTy,
                                KTransformTranslate );
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::Determinant() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::Determinant() const
    {
    return iM00 * iM11 - iM10 * iM01;
    }

// --------------------------------------------------------------------------
//  TBool TGfxAffineTransform::IsIdentity() const
// ---------------------------------------------------------------------------
 TBool TGfxAffineTransform::IsIdentity() const
    {
    return  ( (int)iTransType == KTransformIdentity );
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::ScaleX() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::ScaleX() const
    {
    return iM00;
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::ScaleY() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::ScaleY() const
    {
    return iM11;
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::ShearX() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::ShearX() const
    {
    return iM01;
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::ShearY() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::ShearY() const
    {
    return iM10;
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::TranslateX() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::TranslateX() const
    {
    return iM02;
    }

// --------------------------------------------------------------------------
//  TReal32 TGfxAffineTransform::TranslateY() const
// ---------------------------------------------------------------------------
 TReal32 TGfxAffineTransform::TranslateY() const
    {
    return iM12;
    }


// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Concatenate( const TGfxAffineTransform& aTransform )
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Concatenate( const TGfxAffineTransform& aTransform )
    {

    if ( IsIdentity() )
        {
        iM00 = aTransform.iM00;
        iM01 = aTransform.iM01;
        iM02 = aTransform.iM02;
        iM11 = aTransform.iM11;
        iM10 = aTransform.iM10;
        iM12 = aTransform.iM12;
        iTransType = aTransform.iTransType;
        }
    else if ( aTransform.IsIdentity() )
        {
        // Do nothing
        }
    else
        {
        TFloatFixPt m0, m1;
        m0 = iM00;
        m1 = iM01;
        iM00 = aTransform.iM00 * m0 + aTransform.iM10 * m1;
        iM01 = aTransform.iM01 * m0 + aTransform.iM11 * m1;
        iM02 += aTransform.iM02 * m0 + aTransform.iM12 * m1;
        m0 = iM10;
        m1 = iM11;
        iM11 = aTransform.iM01 * m0 + aTransform.iM11 * m1;
        iM10 = aTransform.iM00 * m0 + aTransform.iM10 * m1;
        iM12 += aTransform.iM02 * m0 + aTransform.iM12 * m1;
        iTransType |= aTransform.iTransType;
        }
    }

// --------------------------------------------------------------------------
//  TGfxAffineTransform TGfxAffineTransform::CreateInverse()
// ---------------------------------------------------------------------------
 TGfxAffineTransform TGfxAffineTransform::CreateInverse()
    {
    TFloatFixPt KZero;
    TFloatFixPt det = iM00* iM11 - iM01* iM10;
    if ( det == KZero )
        return TGfxAffineTransform();
    else
		{
	#ifdef SVG_FLOAT_BUILD	
		TFloatFixPt idet = TFloatFixPt(1.0f) / det;
	#else	
		TFloatFixPt idet = TFloatFixPt(0x10000,ETrue)/ det;

	#endif
    return TGfxAffineTransform( iM11 * idet,
                                    KZero - iM10 * idet,
                                    KZero - iM01 * idet,
                                    iM00 * idet,
                                    ( iM01 * iM12 - iM11 * iM02 ) * idet,
                                    ( iM10 * iM02 - iM00 * iM12 ) * idet );
		}
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Rotate( TReal32 aTheta )
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Rotate( TReal32 aTheta )
    {
    Concatenate( GetRotateInstance( aTheta ) );
    iTransType |= ( KTransformShear | KTransformScale );
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Rotate( TReal32 aTheta,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Rotate( TReal32 aTheta,
                                           const TFloatFixPt& aX,
                                           const TFloatFixPt& aY )
    {
    Concatenate( GetRotateInstance( aTheta, aX, aY ) );
    iTransType |= ( KTransformShear | KTransformScale | KTransformTranslate );
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Scale( TReal32 aSx, TReal32 aSy )
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Scale( TReal32 aSx, TReal32 aSy )
    {
    Concatenate( GetScaleInstance( aSx, aSy ) );
    iTransType |= KTransformScale;
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Translate( const TFixPt& aTx,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Translate( const TFloatFixPt& aTx,
                                              const TFloatFixPt& aTy )
    {
    Concatenate( GetTranslateInstance( aTx, aTy ) );
    iTransType |= KTransformTranslate;
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Shear( TReal32 aShX, TReal32 aShY )
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Shear( TReal32 aShX, TReal32 aShY )
    {
    Concatenate( GetShearInstance( aShX, aShY ) );
    iTransType |= KTransformShear;
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::SetTransform( TReal32 aM00,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::SetTransform( TReal32 aM00,
                                                 TReal32 aM10,
                                                 TReal32 aM01,
                                                 TReal32 aM11,
                                                 TReal32 aM02,
                                                 TReal32 aM12 )
    {
    iM00 = aM00;
    iM01 = aM01;
    iM02 = aM02;
    iM10 = aM10;
    iM11 = aM11;
    iM12 = aM12;
    UpdateState();
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Transform( TGfxPoint2D* aSrcPts,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Transform( TGfxPoint2D* aSrcPts,
                                              TGfxPoint2D* aDstPts,
                                              TInt32 aNumPts ) const
    {

    if ( (int)iTransType == KTransformIdentity )
        {
        if ( aSrcPts != aDstPts )
            {
            for ( TInt32 i = 0; i < aNumPts; i++ )
                {
                aDstPts[i] = aSrcPts[i];
                }
            }
        return;
        }

    if ( (int)iTransType == KTransformScale )
        {
        for ( TInt32 i = 0; i < aNumPts; i++ )
            {
            aDstPts[i].iX = aSrcPts[i].iX * iM00;
            aDstPts[i].iY = aSrcPts[i].iY * iM11;
            }
        }
    else if ( (int)iTransType == KTransformTranslate )
        {
        for ( TInt32 i = 0; i < aNumPts; i++ )
            {
            aDstPts[i].iX = aSrcPts[i].iX + iM02;
            aDstPts[i].iY = aSrcPts[i].iY + iM12;
            }
        }
    else if ( (int)iTransType == ( KTransformScale | KTransformTranslate ) )
        {
        for ( TInt32 i = 0; i < aNumPts; i++ )
            {
            aDstPts[i].iX = ( aSrcPts[i].iX * iM00 ) + iM02;
            aDstPts[i].iY = ( aSrcPts[i].iY * iM11 ) + iM12;
            }
        }
    else
        {
        TFloatFixPt x, y;

        for ( TInt32 i = 0; i < aNumPts; i++ )
            {
            x = aSrcPts[i].iX;
            y = aSrcPts[i].iY;
            aDstPts[i].iX = ( iM00 * x ) + ( iM01 * y ) + iM02;
            aDstPts[i].iY = ( iM10 * x ) + ( iM11 * y ) + iM12;
            }
        }
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Transform( TFixPt* aSrcPts,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Transform( TFloatFixPt* aSrcPts,
                                              TFloatFixPt* aDstPts,
                                              TInt32 aNumPts ) const
    {
    aNumPts <<= 1;

    if ( (int)iTransType == KTransformIdentity )
        {
        if ( aSrcPts != aDstPts )
            {
            for ( TInt32 i = 0; i < aNumPts; i++ )
                {
                *aDstPts++ = *aSrcPts++;
                *aDstPts++ = *aSrcPts++;
                }
            }
        return;
        }

    if ( (int)iTransType == KTransformScale )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aDstPts++ = *aSrcPts++ * iM00;
            *aDstPts++ = *aSrcPts++ * iM11;
            }
        }
    else if ( (int)iTransType == KTransformTranslate )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aDstPts++ = *aSrcPts++ + iM02;
            *aDstPts++ = *aSrcPts++ + iM12;
            }
        }
    else if ( (int)iTransType == ( KTransformScale | KTransformTranslate ) )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aDstPts++ = ( *aSrcPts++ * iM00 ) + iM02;
            *aDstPts++ = ( *aSrcPts++ * iM11 ) + iM12;
            }
        }
    else
        {
        TFloatFixPt x, y;

        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            x = *aSrcPts++;
            y = *aSrcPts++;
            *aDstPts++ = ( iM00 * x ) + ( iM01 * y ) + iM02;
            *aDstPts++ = ( iM10 * x ) + ( iM11 * y ) + iM12;
            }
        }
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::Transform( TFixPt* aSrcDstPts,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Transform( TFloatFixPt* aSrcDstPts,
                                              TInt32 aNumPts ) const
    {
    if ( (int)iTransType == KTransformIdentity )
        return;

    aNumPts <<= 1;

    if ( (int)iTransType == KTransformScale )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aSrcDstPts++ *= iM00;
            *aSrcDstPts++ *= iM11;
            }
        }
    else if ( (int)iTransType == KTransformTranslate )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aSrcDstPts++ += iM02;
            *aSrcDstPts++ += iM12;
            }
        }
    else if ( (int)iTransType == ( KTransformScale | KTransformTranslate ) )
        {
        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            *aSrcDstPts *= iM00;
            *aSrcDstPts++ += iM02;
            *aSrcDstPts *= iM11;
            *aSrcDstPts++ += iM12;
            }
        }
    else
        {
        TFloatFixPt x, y;

        for ( TInt32 i = 0; i < aNumPts; i += 2 )
            {
            x = *aSrcDstPts;
            y = *( aSrcDstPts + 1 );
            *aSrcDstPts++ = ( iM00 * x ) + ( iM01 * y ) + iM02;
            *aSrcDstPts++ = ( iM10 * x ) + ( iM11 * y ) + iM12;
            }
        }
    }

// --------------------------------------------------------------------------
// void TGfxAffineTransform::UpdateState()
// ---------------------------------------------------------------------------
void TGfxAffineTransform::UpdateState()
    {
    TFloatFixPt KZero;
    TFloatFixPt KOne( KFloatFixOne );
    iTransType = KTransformIdentity;

    if ( iM02 != KZero || iM12 != KZero )
        iTransType |= KTransformTranslate;

    if ( iM00 != KZero || iM11 != KZero )
        iTransType |= KTransformScale;

    if ( iM01 != KZero || iM10 != KZero )
        iTransType |= KTransformShear;

    if ( (int)iTransType == KTransformScale && iM00 == KOne && iM11 == KOne )
         iTransType = KTransformIdentity;
    }

// --------------------------------------------------------------------------
// TUint32 TGfxAffineTransform::TransformType()
// ---------------------------------------------------------------------------
TUint32 TGfxAffineTransform::TransformType()
    {
    return iTransType;
    }

// --------------------------------------------------------------------------
// void TGfxAffineTransform::Print()
// prints out the matrix
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::Print()
{
		RDebug::Printf("a=%f b=%f c=%f d=%f e=%f f=%f", (TReal32)iM00, (TReal32)iM10, (TReal32)iM01, (TReal32)iM11, (TReal32)iM02, (TReal32)iM12);
}

// --------------------------------------------------------------------------
// Get the scaling factor set in this transform.  This function returns correctly
// for a uniform scaling in both directions only.
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxAffineTransform::ScalingFactor() const
{
    TGfxPoint2D ep( 1, 0 ), org( 2, 0 );
    Transform( &ep, & ep, 1 );
    Transform( &org, & org, 1 );
    ep.iX -= org.iX;
    ep.iY -= org.iY;
    return TFloatFixPt::Sqrt( ep.iX * ep.iX + ep.iY * ep.iY );
}



// --------------------------------------------------------------------------
//
//  Appends this transform with the existing transform
//  Multiply in reverse order
//  For Ex:  In cases of zoom, rotate and pan of already transformed content.
// If T is the existing transform and zoom operation has to be applied to it
// the resultant transform would be Z*T.
//
//  void TGfxAffineTransform::AppendTransform( const TGfxAffineTransform& aTransform )
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::AppendTransform( const TGfxAffineTransform& aTransform )
    {

    if ( IsIdentity() )
        {
        iM00 = aTransform.iM00;
        iM01 = aTransform.iM01;
        iM02 = aTransform.iM02;
        iM11 = aTransform.iM11;
        iM10 = aTransform.iM10;
        iM12 = aTransform.iM12;
        iTransType = aTransform.iTransType;
        }
    else if ( aTransform.IsIdentity() )
        {
        // Do nothing
        }
    else
        {
        TMatrixElType m00, m01,m02,m10,m11,m12;
        m00 = iM00;
        m01 = iM01;
        m02 = iM02;
        m10 = iM10;
        m11 = iM11;
        m12 = iM12;
        iM00 = aTransform.iM00 * m00 + aTransform.iM01 * m10;
        iM01 = aTransform.iM00 * m01 + aTransform.iM01 * m11;
        iM02 = aTransform.iM00 * m02 + aTransform.iM01 * m12 + aTransform.iM02;

        iM10 = aTransform.iM10 * m00 + aTransform.iM11 * m10;
        iM11 = aTransform.iM10 * m01 + aTransform.iM11 * m11;
        iM12 = aTransform.iM10 * m02 + aTransform.iM11 * m12 + aTransform.iM12;
        
        iTransType |= aTransform.iTransType;
        }
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::UserRoatate( TReal32 aTheta,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::UserRotate( TReal32 aTheta,
                                           const TFloatFixPt& aX,
                                           const TFloatFixPt& aY )
    {
    AppendTransform( GetRotateInstance( aTheta, aX, aY ) );
    iTransType |= ( KTransformShear | KTransformScale | KTransformTranslate );
    }
    
// --------------------------------------------------------------------------
//  void TGfxAffineTransform::UserZoom( TReal32 aScaleFactor,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::UserZoom( TReal32 aScaleFactor,
                                           const TFloatFixPt& aX,
                                           const TFloatFixPt& aY )
    {
    AppendTransform( GetZoomInstance( aScaleFactor, aX, aY ) );
    iTransType |= ( KTransformScale | KTransformTranslate );
    }

// --------------------------------------------------------------------------
//  void TGfxAffineTransform::UserPan( const TFloatFixPt& aTx,
// ---------------------------------------------------------------------------
 void TGfxAffineTransform::UserPan( const TFloatFixPt& aTx,
                                              const TFloatFixPt& aTy )
    {
    AppendTransform( GetTranslateInstance( aTx, aTy ) );
    iTransType |= KTransformTranslate;
    }
    

//
