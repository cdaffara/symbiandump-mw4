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


#ifndef GFXCOLOR_H
#define GFXCOLOR_H

#include <e32std.h>
#include <fbs.h>

#include "GfxPaint.h"

#include <VG/openvg.h>

#include "VGRenderer.h"

/**
 * This class is an implementation of a color.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxColor : public MGfxPaint
    {
    public:


        /**
         * Construct a TGfxColor from an integer value.
         *
         * @since 1.0
         * @param aRgb : integer color value.
         * @return
         */
         TGfxColor( TUint32 aValue );

        /**
         * Construct a TGfxColor from r, g, b values.
         *
         * @since 1.0
         * @param aRed : red component
         * @param aGreen : green component
         * @param aBlue : blue component
         * @return
         */
         TGfxColor( TInt aRed, TInt aGreen, TInt aBlue );


        /**
         * Return the integer representation of this color.
         *
         * @since 1.0
         * @return integer representation of color.
         */
         TUint16          ColorRgb();

        /**
         * Return 4-bits for red, 4-bits for green, 4-bits for blue
         * integer representation of this color.
         *
         * @since 1.0
         * @return 4x4x4 bits integer representation of color.
         */
         TUint16          ColorRgb444();

        /**
         * Return 5-bits for red, 6-bits for green, 5-bits for blue
         * integer representation of this color.
         *
         * @since 1.0
         * @return 5x6x5 bits integer representation of color.
         */
         TUint16          ColorRgb565();

        /**
         * Return an integer representation of a color after blending
         * the two colors given by integer values.
         *
         * @since 1.0
         * @param aAlpha : the alpha channel value to blend.
         * @param aCs1 : color 1 to blend
         * @param aCs2 : color 2 to blend
         * @return integer representation of color.
         */
         static TUint32   BlendRgb444( TUint32 aAlpha,
                                               TUint32 aCs1,
                                               TUint32 aCs2 );

        /**
         * Implementation of MGfxPaint method to get an array of color values.
         * The given buffer array is filled with this color value.
         *
         * @since 1.0
        * @param aBuf : color array to holder color values
         * @param aLength : number of color values to fill.
         * @param x : not used by this class
         * @param y : not used by this class
         * @return
         */
        // void           GetScanline( TUint8* aBuf,
        //                                     TInt32 aLength,
        //                                     TInt32 x,
        //                                     TInt32 y );

        /**
         * Return an integer representation of this color.
         * This method should be renamed!
         *
         * @since 1.0
         * @return integer representation of color.
         */
         TUint32 GetColor();
         TUint32 GetARGB();
         TUint32 GetABGR();




		 void SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* GfxContext);
		public:
			TUint32 iColor;
    };


#endif      // GFXCOLOR_H
