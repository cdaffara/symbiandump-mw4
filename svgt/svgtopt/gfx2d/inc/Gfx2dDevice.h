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


#ifndef GFX2DDEVICE_H
#define GFX2DDEVICE_H

#include <e32std.h>
#include <fbs.h>
#include <bitdev.h>

#include "GfxFloatFixPt.h"



/**
 * This class provides the interface create a graphics device representation.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfx2dDevice : public CBase
    {
    public:

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param : aBitmap -- offscreen bitmap.
         * @return
         */
         static CGfx2dDevice*   NewL( CFbsBitmap* aBitmap );

        /**
         * Destructor
         */
         virtual                ~CGfx2dDevice();

        /**
         * Create a graphics context.
         *
         * @since 1.0
         * @param : aGc -- reference to store the new context.
         * @return error code if any
         */
         TInt                   CreateContext( CGfx2dGc*& aGc, TFontSpec& aFontSpec );

        /**
         * Get size of the of the graphics context.
         *
         * @since 1.0
         * @param
         * @return size of graphics context.
         */
         TSize                  SizeInPixels();


    protected:

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param : aBitmap -- offscreen bitmap.
         * @return
         */
        void                            ConstructL( CFbsBitmap* aBitmap );

    private:

        /**
         * Constructor.
         *
         * @since 1.0
         * @param : aBitmap -- offscreen bitmap.
         * @return
         */
                                        CGfx2dDevice();

    protected:
        CFbsDevice*                     iGdiDevice;
        CFbsBitmap*                     iBitmap;



    private:
        friend class                    CGfx2dGc;
        friend class                    CGfxFont;
    };

#endif      // GFX2DDEVICE_H
