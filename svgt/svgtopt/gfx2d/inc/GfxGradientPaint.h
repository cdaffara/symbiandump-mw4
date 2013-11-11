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


#ifndef GFXGRADIENTPAINT_H
#define GFXGRADIENTPAINT_H

#include <e32std.h>
#include <fbs.h>

#include "GfxPaint.h"
#include "GfxAffineTransform.h"

#include <VG/openvg.h>

//
#include "VGRenderer.h"
const TUint16 KuserSpaceOnUse = 0;
const TUint16 KobjectBoundingBox = 1;
const TUint16 KspreadMethodPad = 0; //SVG_GRADIENT_PAD;
const TUint16 KspreadMethodReflect = 1; //SVG_GRADIENT_REFLECT;
const TUint16 KspreadMethodRepeat= 2; //SVG_GRADIENT_REPEAT;

/**
 * This class holds stop color information for gradient.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TSvgStopData
	{
	public:
		TFloatFixPt iOffset;
		TUint32 iStopColor;
		TFloatFixPt iStopOpacity;
	};

/**
 * This class provides the common properties and methods for linear and radial gradient.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxGradientPaint : public MGfxPaint
    {
    public:
        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
		 TGfxGradientPaint();
		 ~TGfxGradientPaint();




		void SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* GfxContext);

		TUint16 iGradientUnits;
		TUint16 iSpreadMethod;
		SVGMatrix2x3 iGradientTransform;
		RArray<TSvgStopData>  *iStopData;

		VGPaint	iFillPaint;

    };

/**
 *
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxLinearGradientPaint : public TGfxGradientPaint
    {
    public:
        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
         TGfxLinearGradientPaint();
    public:
        /**
         *
         *
         * @since 1.0
         * @param aRect : rectangle to iterator through.
         * @param aTransform : transform to apply.
         * @return
         */
        TUint32 GetColor() {return KGfxColorLinearGradient;}




		 void SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* GfxContext);
		TFloatFixPt iX1, iY1, iX2, iY2;
    };

/**
 *
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRadialGradientPaint : public TGfxGradientPaint
    {
    public:
        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
         TGfxRadialGradientPaint();

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aRect : rectangle to iterator through.
         * @param aTransform : transform to apply.
         * @return
         */
        TUint32 GetColor() {return KGfxColorRadialGradient;}




		 void SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* GfxContext);
		TFloatFixPt iCx, iCy, iR, iFx, iFy;
    };

#endif /* GFXGRADIENTPAINT_H */
