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


#ifndef GFXAFFINETRANSFORM_H
#define GFXAFFINETRANSFORM_H


#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"


// These constants are used because enumeration cannot
// accept bit operation
const TInt32 KTransformIdentity = 0;
const TInt32 KTransformTranslate = 1;
const TInt32 KTransformScale = 2;
const TInt32 KTransformShear = 4;

typedef TFloatFixPt TMatrixElType;
typedef TUint32     TGfxTransformType;

class TGfxAffineTransform;
class CGfxShape;
class TGfxPoint2D;


/**
 * This class implements operations of a 2D transform matrix.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxAffineTransform
    {
    public:

        /**
         * Construct an identity matrix.
         *
         * @since 1.0
         * @param
         * @return
         */
                                    TGfxAffineTransform();

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aM00 : row 0, column 0 value of matrix.
         * @param aM10 : row 1, column 0 value of matrix.
         * @param aM01 : row 0, column 1 value of matrix.
         * @param aM11 : row 1, column 1 value of matrix.
         * @param aM02 : row 0, column 2 value of matrix.
         * @param aM12 : row 1, column 2 value of matrix.
         * @return
         */
                                    TGfxAffineTransform( TReal32 aM00,
                                                                 TReal32 aM10,
                                                                 TReal32 aM01,
                                                                 TReal32 aM11,
                                                                 TReal32 aM02,
                                                                 TReal32 aM12 );


        /**
         * Return a TGfxAffineTransform object with the rotation info
         * of the given angle applied.
         *
         * @since 1.0
         * @param aTheta : angle to create a rotation affine transform.
         * @return
         */
         static TGfxAffineTransform GetRotateInstance( TReal32 aTheta );

        /**
         * Return a TGfxAffineTransform object with the rotation info
         * of the given angle, rotated at the given point.
         *
         * @since 1.0
         * @param aTheta : angle to create a rotation affine transform.
         * @param aX : x value of rotation center.
         * @param aY : x value of rotation center.
         * @return
         */
         static TGfxAffineTransform GetRotateInstance( TReal32 aTheta,
                                                               const TFloatFixPt& aX,
                                                               const TFloatFixPt& aY );

/**
         * Return a TGfxAffineTransform object with the zoom info
         * of the given scalefactor, scaled at the given point.
         *
         * @since 1.0
         * @param aScaleFactor : scaling factor
         * @param aX : x value of scaling center.
         * @param aY : x value of scaling center.
         * @return
         */
         static TGfxAffineTransform GetZoomInstance( TReal32 aScaleFactor,
                                                               const TFloatFixPt& aX,
                                                               const TFloatFixPt& aY );
       
        /**
         * Return a TGfxAffineTransform object with the scaling info
         * applied.
         *
         * @since 1.0
         * @param aSx : scaling factor in the x axis.
         * @param aSy : scaling factor in the y axis.
         * @return
         */
         static TGfxAffineTransform GetScaleInstance( TReal32 aSx,
                                                              TReal32 aSy );

        /**
         * Return a TGfxAffineTransform object with the shearing info
         * applied.
         *
         * @since 1.0
         * @param aShx : shearing factor in the x axis.
         * @param aShy : shearing factor in the y axis.
         * @return
         */
         static TGfxAffineTransform GetShearInstance( TReal32 aShx,
                                                              TReal32 aShy );

        /**
         * Return a TGfxAffineTransform object with the translation info
         * applied.
         *
         * @since 1.0
         * @param aTx : translation value in the x axis.
         * @param aTy : translation value in the y axis.
         * @return
         */
         static TGfxAffineTransform GetTranslateInstance( const TFloatFixPt& aTx,
                                                                  const TFloatFixPt& aTy );

        /**
         * Return the determinate value of this matrix.
         *
         * @since 1.0
         * @return the determinate value
         */
         TReal32                    Determinant() const;

        /**
         * Return wether this matrix is an indentity matrix.
         *
         * @since 1.0
         * @return true if this matrix is an identity matrix.
         */
         TBool                      IsIdentity() const;

        /**
         * Return the scaling factor in the x axis.
         *
         * @since 1.0
         * @return scaling factor in the x axis.
         */
         TReal32                    ScaleX() const;

        /**
         * Return the scaling factor in the y axis.
         *
         * @since 1.0
         * @return scaling factor in the y axis.
         */
         TReal32                    ScaleY() const;

        /**
         * Return the shearing factor in the x axis.
         *
         * @since 1.0
         * @return shearing factor in the x axis.
         */
         TReal32                    ShearX() const;

        /**
         * Return the shearing factor in the y axis.
         *
         * @since 1.0
         * @return shearing factor in the y axis.
         */
         TReal32                    ShearY() const;

        /**
         * Return the translation value in the x axis.
         *
         * @since 1.0
         * @return translation value in the x axis.
         */
         TReal32                    TranslateX() const;

        /**
         * Return the translation value in the y axis.
         *
         * @since 1.0
         * @return translation value in the y axis.
         */
         TReal32                    TranslateY() const;

        /**
         * Create the inverse tranformation matrix of this matrix.
         *
         * @since 1.0
         * @return inverse matrix.
         */
         TGfxAffineTransform        CreateInverse();


        /**
         * Muliply this given matrix with this matrix.
         *
         * @since 1.0
         * @return
         */
         void                       Concatenate( const TGfxAffineTransform& aTransform );


        /**
         * Appends this transform with the existing transform
         * Multiply in reverse order
         * For Ex:  In cases of zoom, rotate and pan of already transformed content.
         * If T is the existing transform and zoom operation has to be applied to it
         * the resultant transform would be Z*T.
         * @since 1.0
         * @return
         */
         void                       AppendTransform( const TGfxAffineTransform& aTransform );
        
        /**
         * Apply the given rotation angle to this matrix.
         *
         * @since 1.0
         * @param aTheta : angle of rotation
         * @return
         */
         void                       Rotate( TReal32 aTheta );

        /**
         * Apply the given rotation angle about this given point to this matrix.
         *
         * @since 1.0
         * @param aTheta : angle of rotation
         * @param aX : x coordinate for center of rotation.
         * @param aY : y coordinate for center of rotation.
         * @return
         */
         void                       Rotate( TReal32 aTheta,
                                                    const TFloatFixPt& aX,
                                                    const TFloatFixPt& aY );

        /**
         * Apply the given rotation angle about this given point to this matrix.
         *
         * @since 1.0
         * @param aTheta : angle of rotation
         * @param aX : x coordinate for center of rotation.
         * @param aY : y coordinate for center of rotation.
         * @return
         */

         void                       UserRotate( TReal32 aTheta,
                                                    const TFloatFixPt& aX,
                                                    const TFloatFixPt& aY );

        /*
         * Apply the given scaling factor for this given point to this matrix.
         *
         *
         * @since 1.0
         * @param aScaleFactor : scaling factor
         * @param aX : x value of scaling center.
         * @param aY : x value of scaling center.
         * @return
         */
       
         void                       UserZoom( TReal32 aScaleFactor,
                                                    const TFloatFixPt& aX,
                                                    const TFloatFixPt& aY );

        /**
         * Apply the given scale factors to this matrix.
         *
         * @since 1.0
         * @param aSx : scaling factor in the x axis.
         * @param aSy : scaling factor in the y axis.
         * @return
         */
         void                       Scale( TReal32 aSx, TReal32 aSy );

        /**
         * Apply the given translation values to this matrix.
         *
         * @since 1.0
         * @param aTx : translation value in the x axis.
         * @param aTy : translation value in the y axis.
         * @return
         */
         void                       Translate( const TFloatFixPt& aTx,
                                                       const TFloatFixPt& aTy );

        /**
         * Apply the given translation values to this matrix.
         *
         * @since 1.0
         * @param aTx : translation value in the x axis.
         * @param aTy : translation value in the y axis.
         * @return
         */
         void                       UserPan( const TFloatFixPt& aTx,
                                                       const TFloatFixPt& aTy );

        /**
         * Apply the given shearing values to this matrix.
         *
         * @since 1.0
         * @param aTx : shearing value in the x axis.
         * @param aTy : shearing value in the y axis.
         * @return
         */
         void                       Shear( TReal32 aShX, TReal32 aShY );

        /**
         * Set the transformation info of this matrix to the given values.
         *
         * @since 1.0
         * @param aM00 : row 0, column 0 value of matrix.
         * @param aM10 : row 1, column 0 value of matrix.
         * @param aM01 : row 0, column 1 value of matrix.
         * @param aM11 : row 1, column 1 value of matrix.
         * @param aM02 : row 0, column 2 value of matrix.
         * @param aM12 : row 1, column 2 value of matrix.
         * @return
         */
         void                       SetTransform( TReal32 aM00,
                                                          TReal32 aM10,
                                                          TReal32 aM01,
                                                          TReal32 aM11,
                                                          TReal32 aM02,
                                                          TReal32 aM12 );

        /**
         * Apply the transformation described by this matrix to the
         * array of TGfxPoint2D.
         *
         * @since 1.0
         * @param aSrcPts : array of TGfxPoint2D values to transform.
         * @param aDstPts : array to store transformed points.
         * @param aNumPts : number of points to transform.
         * @return
         */
         void                       Transform( TGfxPoint2D* aSrcPts,
                                                       TGfxPoint2D* aDstPts,
                                                       TInt32 aNumPts ) const;

        /**
         * Apply the transformation described by this matrix to the
         * array of TFloatFixPt.
         *
         * @since 1.0
         * @param aSrcPts : array of TFloatFixPt values to transform.
         * @param aDstPts : array to store transformed points.
         * @param aNumPts : number of points to transform.
         * @return
         */
         void                       Transform( TFloatFixPt* aSrcPts,
                                                       TFloatFixPt* aDstPts,
                                                       TInt32 aNumPts ) const;

        /**
         * Apply the transformation described by this matrix to the
         * array of TFloatFixPt.  Old values are replaced.
         *
         * @since 1.0
         * @param aSrcPts : array of TFloatFixPt values to transform.
         * @param aNumPts : number of points to transform.
         * @return
         */
         void                       Transform( TFloatFixPt* aSrcDstPts,
                                                       TInt32 aNumPts ) const;


        /**
         * Return the transformation type of this matrix.
         *
         * @since 1.0
         * @return transformation type
         */
        TUint32 TransformType();

         void Print();

        /**
         * Get the scaling factor set in this transform.  This function returns correctly
         * for a uniform scaling in both directions only.
         * @since 1.0
         * @return
         */
         TFloatFixPt ScalingFactor() const;

    private:

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aM00 : row 0, column 0 value of matrix.
         * @param aM10 : row 1, column 0 value of matrix.
         * @param aM01 : row 0, column 1 value of matrix.
         * @param aM11 : row 1, column 1 value of matrix.
         * @param aM02 : row 0, column 2 value of matrix.
         * @param aM12 : row 1, column 2 value of matrix.
         * @param aType : transform type.
         * @return
         */
                            TGfxAffineTransform( TReal32 aM00,
                                                 TReal32 aM10,
                                                 TReal32 aM01,
                                                 TReal32 aM11,
                                                 TReal32 aM02,
                                                 TReal32 aM12,
                                                 TGfxTransformType aType );
        /**
         * Update this matrix.
         *
         * @since 1.0
         * @return
         */
        void                UpdateState();

        /**
         * The the the rotation info to the give angle.
         *
         * @since 1.0
         * @return
         */
        void                SetToRotate( TReal32 aTheta );

	public:

        TFloatFixPt       iM00;
        TFloatFixPt        iM10;
        TFloatFixPt        iM01;
        TFloatFixPt        iM11;
        TFloatFixPt        iM02;
        TFloatFixPt        iM12;

        TGfxTransformType   iTransType;


    };

#endif      // GFXAFFINETRANSFORM_H
