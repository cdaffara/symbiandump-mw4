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


#ifndef GFXRENDERINGHINTS_H
#define GFXRENDERINGHINTS_H

#include <e32std.h>
#include <fbs.h>
#include "GfxFloatFixPt.h"


/**
 * This class implements the properties for renderer hints.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRenderingHints
    {
    public:

        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
                                TGfxRenderingHints();

        /**
         * Set the flag for visiblity.
         *
         * @since 1.0
         * @param aVisibility : ETrue = visible
         * @return
         */
         void                   SetVisibility( TBool aVisibility );

        /**
         * Get the flag for visiblity.
         *
         * @since 1.0
         * @param aVisibility : ETrue = visible
         * @return visibility flag
         */
         TBool                  Visibility();

        /**
         * Set the level of curve quality.
         *
         * @since 1.0
         * @param aCurveQuality : level of quality
         * @return
         */
         void                   SetCurveQuality( TInt aCurveQuality );

        /**
         * Get the level of curve quality.
         *
         * @since 1.0
         * @return level of quality of curves.
         */
         TInt                   CurveQuality();

        /**
         * Set the flag for fast font rendering
         *
         * @since 1.0
         * @param aFastFontRendering : ETrue = fast
         * @return
         */
         void                   SetFastFontRendering( TBool aFastFontRendering );

        /**
         * Get the flag for fast font rendering
         *
         * @since 1.0
         * @return fast font rendering flag
         */
         TBool                  FastFontRendering();

        /**
         * Set the flag for anti-aliasing rendering of shapes
         *
         * @since 1.0
         * @param aFastFontRendering : ETrue = should render with anti-alias.
         * @return
         */
         void                   SetShapeAntiAliasing( TBool aShapeAntiAliasing );

        /**
         * Get the flag for anti-aliasing rendering of shapes
         *
         * @since 1.0
         * @return anti-aliasing rendering of shapes flag
         */
         TBool                  ShapeAntiAliasing();

        /**
         * Get the image interpolation object.
         *
         * @since 1.0
         * @return image interpolation object.
         */
         TGfxImageInterpolation ImageInterpolation();

    private:
        TUint32                         iHintBits;

    };

#endif      // GFXRENDERINGHINTS_H
