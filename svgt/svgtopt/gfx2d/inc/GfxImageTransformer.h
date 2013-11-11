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


#ifndef GFXIMAGETRANSFORMER_H
#define GFXIMAGETRANSFORMER_H

#include <e32std.h>
#include <fbs.h>
#include <bitstd.h>

#include "GfxFloatFixPt.h"
#include "GfxAffineTransform.h"
#include "GfxRenderingHints.h"
#include "GfxRectangle2D.h"
#include "GfxPoint2D.h"
#include "GfxColor.h"


class CVGRenderer;

/**
 * Class to perform transformation on an image.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxImageTransformer
    {
    public:

        /**
         * Contructor.
         *
         * @since 1.0
         * @param aTransform : transform to apply.
         * @param aInterpolation : image interpolation
         * @return
         */
                            TGfxImageTransformer( TGfxAffineTransform* aTransform, CVGRenderer * aRenderer);


        /**
         * Draw the source image onto the destination with the transformation defined by
         * this object, blending with the alpha value.
         *
         * @since 1.0
         * @param aSrc : source image
         * @param aPoint : origin point of the text rectangle.
         * @param aDst : destination image buffer.
         * @param aDstWidth : destination image width.
         * @param aDstHeight : destination image height.
         * @param aAlpha : blending alpha value.
         * @param aClipRect :
         * @param aReverse : ETrue if copying image from aDst to aSrc.
         *                   This is a special case and used to copy background image to text box.
         *                   EFalse is for regular use.
         * @return
         */
         void               Draw( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& p,
										  TUint32* aDst,
										  TInt aDstWidth,
										  TInt aDstHeight,
										  TUint8 aAlpha,
                                          TGfxColor aTransparentColor,
										  const TRect& aClipRect,
										  TBool aReverse = EFalse
										  );

        /**
         * Draw the source image onto the destination with the transformation defined by
         * this object.  Uses the alpha bits of the EColor16MU to blend.  Only EColor16MU mode
         * for aSrc is allowed.
         *
         * @since 1.0
         * @param aSrc : source image
         * @param aPoint : origin point of the text rectangle.
         * @param aDst : destination image buffer.
         * @param aDstWidth : destination image width.
         * @param aDstHeight : destination image height.
         * @param aClipRect :
         * @param aReverse : ETrue if copying image from aDst to aSrc.
         *                   This is a special case and used to copy background image to text box.
         *                   EFalse is for regular use.
         * @return
         */
         void               Draw( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& p,
										  TUint32* aDst,
										  TInt aDstWidth,
										  TInt aDstHeight,
										  const TRect& aClipRect
										  );


        /**
         * Get the bounding box for the given image once transformed.
         *
         * @since 1.0
         * @param aSrc : source image
         * @param aOffset : offset.
         * @return
         */
         TGfxRectangle2D    GetTransformedBound( CFbsBitmap* aSrc,
                                                         const TGfxPoint2D& aOffset );
        /**
         * Draw the source image onto the destination with the transformation defined by
         * this object, blending with the alpha value. When aReverse is ETrue, do the reverse.
         *
         * @since 1.0
         * @param aSrc : source image
         * @param aPoint : origin point of the text rectangle.
         * @param aDstWidth : destination image width.
         * @param aDstHeight : destination image height.
         * @param aClipRect :
         * @param aReverse : ETrue if copying image from aDst to aSrc.
         *                   This is a special case and used to copy background image to text box.
         *                   EFalse is for regular use.
         * @return
         */
         void       ImageBlend( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& p,
										  TInt aDstWidth,
										  TInt aDstHeight,
										  const TRect& aClipRect,
										  TBool aReverse = EFalse
										  );

        /**
         * Compute scanline starting/end position for the rectangle.
         *
         * @since 1.0
         * @param y : current rectangle's height
         * @param rect : rectangle that needs to be computed.
         * @param north : transformed corner.
         * @param west :  transformed corner.
         * @param east :  transformed corner.
         * @param south :  transformed corner.
         * @param xstart : computed x coordinates for start position
         * @param xend : computed x coordinates for end position
         *
         * @return none
         */
        void ComputeXPositions(TFloatFixPt y, TRect rect, TGfxPoint2D north, TGfxPoint2D west,
            TGfxPoint2D east, TGfxPoint2D south, TInt& xstart, TInt& xend);

        /**
         * compute coordinates from transformed 4 points.
         *
         * @since 1.0
         *
         * @return none
         */
        void SortCoordinates(TGfxPoint2D& origin, TGfxPoint2D& top,
            TGfxPoint2D& north, TGfxPoint2D& south, TGfxPoint2D& right,
            TGfxPoint2D& east, TGfxPoint2D& west, TGfxPoint2D& corner,
            TGfxPoint2D aP, const TInt srcWidth, const TInt srcHeight);

    private:

        TGfxAffineTransform*        iTransform;
        CVGRenderer *           iVgRenderer;
    };

#endif      // GFXIMAGETRANSFORMER_H
