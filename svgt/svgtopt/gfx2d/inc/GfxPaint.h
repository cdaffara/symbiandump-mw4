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


#ifndef GFXPAINT_H
#define GFXPAINT_H

#include <e32std.h>
#include <fbs.h>

#include "GfxFloatFixPt.h"
#include "GfxRectangle2D.h"

#include <VG/openvg.h>

const TUint32 KGfxColorLinearGradient = 0x10ffffff;
const TUint32 KGfxColorRadialGradient = 0x11ffffff;
const TUint32 KGfxColorNull = 0x1ffffff;


/**
 *  Abstract class to define methods for a paint implementation, such as
 *  a gradient painter.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class MGfxPaint
    {
    public:
        /**
         * Get an array of color values for a scanline.
         * The given buffer array is filled with this color value.
         *
         * @since 1.0
         * @param aBuf : color array to holder color values
         * @param aLength : number of color values to fill.
         * @param x : x coordinate
         * @param y : y coordinate
         * @return
         */
        //virtual void    GetScanline( TUint8* aBuf,
        //                             TInt32 aLength,
        //                             TInt32 x,
        //                             TInt32 y ) = 0;

        /**
         * Get the "flat" color.
         *
         * @since 1.0
         * @return KGfxColorNull, if implementator is gradient paint, otherwise
         * an RGB888.
         */
        virtual TUint32 GetColor() = 0;


    

		virtual void SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* GfxContext) = 0;
    };

#endif      // GFXPAINT_H
