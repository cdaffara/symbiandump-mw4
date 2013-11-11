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


#ifndef GFXCURVEFLATTENERP_H
#define GFXCURVEFLATTENERP_H

#include <e32base.h>
#include <e32std.h>

#include "GfxFloatFixPt.h"

class TGfxAffineTransform;


/**
 * This class provides the methods to smooth a curve defined by control points.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxCurveFlattener
    {
    public:

        /**
         * Construct a TGfxCurveFlattener with a transform matrix.
         *
         * @since 1.0
         * @param aTransform : Transformation matrix.
         * @return
         */
                                TGfxCurveFlattener( TGfxAffineTransform* aTransform );

        /**
         * Add a quadratic curve to this object.
         *
         * @since 1.0
         * @param aRenderer : edge point list
         * @param aCtrlPoints : control points.
         * @param aLimit : number of points to flatten curve with.
         * @return
         */
        void                    AddSubCurveQuadL( CGfxEdgeListP* aRenderer,
                                                  TFloatFixPt * aCtrlPoints,
                                                  TInt32 aLimit );

        /**
         * Add a cubic curve to this object.
         *
         * @since 1.0
         * @param aRenderer : edge point list
         * @param aCtrlPoints : control points.
         * @param aLimit : number of points to flatten curve with.
         * @return
         */
        void                    AddSubCurveCubicL( CGfxEdgeListP* aRenderer,
                                                   TFloatFixPt * aCtrlPoints,
                                                   TInt32 aLimit );
    private:

        TGfxAffineTransform*    iTransform;
    };

#endif      // GFXCURVEFLATTENERP_H
