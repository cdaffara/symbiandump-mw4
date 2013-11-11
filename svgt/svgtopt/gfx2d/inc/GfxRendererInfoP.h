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


#ifndef GFXRENDERERINFOP_H
#define GFXRENDERERINFOP_H

#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"
#include "GfxRectangle2D.h"


/**
 * This class implements the base properties of a renderer.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRendererInfoP
    {
    class    CGfxFont;

    public:

        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
                        TGfxRendererInfoP();

        /**
         * Set the device info to rendering.
         *
         * @since 1.0
         * @param aDstBuf : data holder for pixel info.
         * @param aWidth : width of rendering area.
         * @param aHeight : height of rendering area.
         * @param aBytePerPixel : bytes per pixel
         * @return
         */
        void            SetDevice( TUint8* aDstBuf,
                                   TInt32 aWidth,
                                   TInt32 aHeight,
                                   TInt32 aBytePerPixel );

        /**
         * Set the area to apply rendering operations.
         *
         * @since 1.0
         * @param aClip : rectangle to define clipping area.
         * @return
         */
        void            SetClip( const TGfxRectangle2D& aClip );

    protected:

        TInt32          iWidth;
        TInt32          iHeight;

        TInt32          iClipMinX, iClipMinY;
        TInt32          iClipMaxX, iClipMaxY;


    private:
        friend class    CGfx2dGcVGR;
        friend class    CGfx2dGcOpenVG;
    };

#endif      // GFXRENDERERINFOP_H
