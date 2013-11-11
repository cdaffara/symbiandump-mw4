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


#include "GfxImageTransformer.h"
#include "GfxColor.h"

#define RGB565toRGB888(rgb565) (0xff000000|((rgb565 << 8) & 0xf80000) | ((rgb565 << 5) & 0x00fc00) | ((rgb565 << 3) & 0x000f8))

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxImageTransformer::TGfxImageTransformer( TGfxAffineTransform* aTransform )
// ---------------------------------------------------------------------------
 TGfxImageTransformer::TGfxImageTransformer( TGfxAffineTransform* aTransform, CVGRenderer * aRenderer )
    {
    iTransform = aTransform;
    iVgRenderer = aRenderer;
	}




// --------------------------------------------------------------------------
//  void TGfxImageTransformer::ImageBlend( CFbsBitmap* aSrc,
// ---------------------------------------------------------------------------
 void TGfxImageTransformer::ImageBlend( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& aP,
										  TInt aDstWidth,
										  TInt aDstHeight,
                                          const TRect& aClipRect,
										  TBool aReverse)
{

    TRect srcrect( TPoint( aP.iX, aP.iY ), aSrc->SizeInPixels() );
    TRect dstrect( TPoint( 0, 0 ), TSize(aDstWidth, aDstHeight) );
	TInt srcWidth = aSrc->SizeInPixels().iWidth;
	TInt srcHeight = aSrc->SizeInPixels().iHeight;

    TGfxRectangle2D rect2d = GetTransformedBound( aSrc, aP );
    TRect rect (TPoint( rect2d.iX, rect2d.iY ), TSize( rect2d.iWidth, rect2d.iHeight ) );
	rect.Intersection(aClipRect);	// Clipping

    if ( !rect.Intersects( dstrect ) )    // clip rect with dst image
        return;

    aSrc->LockHeap();
    rect.Intersection( dstrect );

	TUint32* bufsrc32 = (TUint32*) aSrc->DataAddress();
    TUint bufLimit = aDstWidth * aDstHeight;

    TGfxAffineTransform inv = iTransform->CreateInverse();

    TInt xstart, xend, yend;
	TGfxPoint2D horizDeriv;
	TGfxPoint2D origin, top, right, corner, east, west, south, north;

	// sort four corners into appropriate order
	SortCoordinates(origin, top, north, south, right, east, west,
	    corner, aP, srcWidth, srcHeight);

	// compute derivitives for scanline in source.
    TPoint pinv;
    TGfxPoint2D p;
	p.iX = 8;
	p.iY = 0;
	inv.Transform( &p, & p, 1 );
	horizDeriv.iX = p.iX;
	horizDeriv.iY = p.iY;
	p.iX = 0;
	p.iY = 0;
	inv.Transform( &p, & p, 1 );
	horizDeriv.iX -= p.iX;
	horizDeriv.iY -= p.iY;
	horizDeriv.iX = horizDeriv.iX >> 3;
	horizDeriv.iY = horizDeriv.iY >> 3;

    yend = rect.iBr.iY;
    TInt bufLength = rect.iTl.iX + ( rect.iTl.iY * aDstWidth );
	TInt tempBufLength = bufLength;

    TInt x, y;
    TInt sx, sy;
    TUint32 pixel;
    TInt offset;
    VGint stride = CFbsBitmap::ScanLineLength( aDstWidth, aSrc->DisplayMode() );
    for ( y = rect.iTl.iY; y <= yend; y++ )
        {
        p.iY = y;
		// compute scanline starting position
		ComputeXPositions((TFloatFixPt)y, rect, north, west, east, south, xstart, xend);

		p.iX = xstart;
		// transform that starting position to the source image
		inv.Transform( &p, & p, 1 );
		bufLength += xstart - rect.iTl.iX;

	#ifdef SVG_FLOAT_BUILD
	p.iY -= TFloatFixPt( .5f );
	#else
	p.iY -= TFloatFixPt(0x8000, ETrue);
	#endif
		for ( x = xstart; x <= xend; x++ )
			{
			//increment the location of the position by one horizontal location
			pinv.iX = p.iX;
			pinv.iY = p.iY;

			if ( srcrect.Contains( pinv ) )
				{
				pinv.iX -= ( TInt32 ) aP.iX;
				pinv.iY -= ( TInt32 ) aP.iY;
                // find out cooresponding x & y in OpenVG
                offset = bufLength;
                sy = aDstHeight - (offset / aDstWidth);
                sx = offset % aDstWidth;
                if (aReverse)
                    {
                    iVgRenderer->vgReadPixels((void *)&pixel, stride, VG_sRGBA_8888_PRE, sx, sy, 1, 1);
    				*(bufsrc32 + ( TInt32 ) pinv.iX + (srcWidth)*( TInt32 ) (pinv.iY)) =
    				    (pixel >> 8) | (pixel << 24) ;
    				}
    			else
    			    {
    			    pixel = *(bufsrc32 + ( TInt32 ) pinv.iX + (srcWidth)*( TInt32 ) (pinv.iY));
    			    pixel = (pixel << 8) | (pixel >> 24);
    			    iVgRenderer->vgWritePixels((void *)&pixel, stride, VG_sRGBA_8888_PRE, sx, sy, 1, 1);
    			    }
				}
			p.iX += horizDeriv.iX;
			p.iY += horizDeriv.iY;
			bufLength++;
			}
		tempBufLength += aDstWidth;
		bufLength = tempBufLength;
		if (tempBufLength + aDstWidth >= bufLimit)
            {
            aSrc->UnlockHeap();
            return;
            }
        }
    aSrc->UnlockHeap();

    }






// --------------------------------------------------------------------------
//  void TGfxImageTransformer::SortCoordinates
// ---------------------------------------------------------------------------

void TGfxImageTransformer::SortCoordinates(
    TGfxPoint2D& origin, TGfxPoint2D& top,
    TGfxPoint2D& north, TGfxPoint2D& south, TGfxPoint2D& right,
    TGfxPoint2D& east, TGfxPoint2D& west, TGfxPoint2D& corner,
    const TGfxPoint2D aP, const TInt srcWidth, const TInt srcHeight)
    {

	origin.iX = aP.iX;
	origin.iY = aP.iY;
	corner.iX = aP.iX + (TFloatFixPt)srcWidth;
	corner.iY = aP.iY + (TFloatFixPt)srcHeight;
	top.iX =	aP.iX;
	top.iY =	aP.iY + (TFloatFixPt)srcHeight;
	right.iX =	aP.iX + (TFloatFixPt)srcWidth;
	right.iY =	aP.iY;

	// transform corners of the source rectangle.
    iTransform->Transform(&origin, &origin, 1);
    iTransform->Transform(&top, &top, 1);
    iTransform->Transform(&right, &right, 1);
	iTransform->Transform(&corner, &corner, 1);

	west = origin; east = corner; north = top; south = right;
	if (top.iX < origin.iX)
	    {
	    west = top;
	    east = right;
	    north = corner;
	    south = origin;
	    }
	else
	    {
	    west = origin;
	    east = corner;
	    north = top;
	    south = right;
	    }

	if (corner.iX < west.iX)
	    {
	    west = corner;
	    east = origin;
	    north = right;
	    south = top;
	    }

	if (right.iX < west.iX)
	    {
	    west = right;
	    east = top;
	    north = origin;
	    south = corner;
	    }
    }





// --------------------------------------------------------------------------
//  void TGfxImageTransformer::ComputeXPositions
// ---------------------------------------------------------------------------

void TGfxImageTransformer::ComputeXPositions(TFloatFixPt y, TRect rect,
    TGfxPoint2D north, TGfxPoint2D west, TGfxPoint2D east,
    TGfxPoint2D south, TInt& xstart, TInt& xend)
    {

	if ( y > west.iY )
	    {
		if (north.iY - west.iY == (TFloatFixPt)0)
			xstart = (TInt32) west.iX;
		else
			xstart = (TInt32) (west.iX + (north.iX - west.iX) *
			    ( (y - west.iY) / (north.iY - west.iY) ));
	    }
	else
	    {
		if (west.iY - south.iY == (TFloatFixPt)0)
			xstart = (TInt32) south.iX;
		else
			xstart = (TInt32) (south.iX + (west.iX - south.iX) *
			    ( (y - south.iY) / (west.iY - south.iY) ));
	    }

	// compute scanline edges end position
	if ( y > east.iY )
	    {
		if ( north.iY - east.iY == (TFloatFixPt)0 )
			xend = (TInt32) north.iX;
		else
			xend = (TInt32) (east.iX + (north.iX - east.iX) *
			    ( (((TFloatFixPt)y) - east.iY) / (north.iY - east.iY) ));
	    }
	else
	    {
		if ( (east.iY - south.iY) == (TFloatFixPt)0 )
			xend = (TInt32) east.iX;
		else
			xend = (TInt32) (south.iX + (east.iX - south.iX) *
			    ( (((TFloatFixPt)y) - south.iY) / (east.iY - south.iY) ));
	    }

    // does span clip against edges
	if(xstart < rect.iTl.iX)
		xstart = rect.iTl.iX;

	if(xend >= rect.iBr.iX)  // iX-1 seems to be the limit, using iX rolls over to next line
		xend = rect.iBr.iX-1;
    }

// --------------------------------------------------------------------------
//  void TGfxImageTransformer::Draw( CFbsBitmap* aSrc,
// ---------------------------------------------------------------------------
 void TGfxImageTransformer::Draw( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& aP,
										  TUint32* aDst,
										  TInt aDstWidth,
										  TInt aDstHeight,
                                          TUint8 /*aAlpha*/,
                                          TGfxColor aTransparentColor,
										  const TRect& aClipRect,
										  TBool aReverse
										  )
    {
    TRect rect;
    TInt32 x, y;

	TFloatFixPt currentScale = iTransform->ScalingFactor();
		TSize bitmapSize = aSrc->SizeInPixels();
    TRect srcrect( TPoint( aP.iX, aP.iY ),
                   TSize( bitmapSize.iWidth-1, bitmapSize.iHeight-1 ) );
    TRect dstrect( TPoint( 0, 0 ), TSize(aDstWidth-1, aDstHeight-1) );
	  TInt32 srcWidth = bitmapSize.iWidth;
	  TInt32 srcHeight = bitmapSize.iHeight;
    TInt32 pad = (srcWidth & 1);

    TGfxRectangle2D rect2d = GetTransformedBound( aSrc, aP );
    rect.SetRect( TPoint( rect2d.iX, rect2d.iY ), TSize( rect2d.iWidth, rect2d.iHeight ) );
	rect.Intersection(aClipRect);	// Clipping

    if ( !rect.Intersects( dstrect ) )    // clip rect with dst image
        return;

    aSrc->LockHeap();
    rect.Intersection( dstrect );
    TDisplayMode dispmode = aSrc->DisplayMode();

	TUint16* bufsrc16 = NULL;
	TUint32* bufsrc32 = NULL;
	if ( dispmode ==  EColor16MU)
	    {
	    bufsrc32 = (TUint32*) aSrc->DataAddress();
	    }
	else
	    {
	    bufsrc16 = (TUint16*) aSrc->DataAddress();
	    }
    TUint32* buf = aDst + rect.iTl.iX + ( rect.iTl.iY * aDstWidth );
    TUint32* bufLimit = aDst + aDstWidth * aDstHeight;
	TUint32 pix = 0;
        TInt dstStride = aDstWidth;

    TGfxAffineTransform inv = iTransform->CreateInverse();

    if ( aTransparentColor.GetARGB() == KGfxColorNull /* && aAlpha == 0xff */ &&
        (int)inv.TransformType() == KTransformIdentity && aReverse == EFalse)
        {
        // Fast case; no alpha or transform (cdm).
        TPoint p;
		TInt lScrPp = (srcWidth+pad);
        for ( y = rect.iTl.iY; y <= rect.iBr.iY; y++ )
            {
            for ( x = rect.iTl.iX; x <= rect.iBr.iX; x++ )
                {
                p.iX = x;
                p.iY = y;

                if ( srcrect.Contains( p ) )
                    {
                    p.iX -= ( TInt32 ) aP.iX;
                    p.iY -= ( TInt32 ) aP.iY;
					if ( dispmode ==  EColor16MU)
					    {
    					buf[x] = (TUint32) *(bufsrc32 + ( TInt32 ) p.iX + (lScrPp)*( TInt32 ) p.iY);
    					*buf = pix;
					    }
					else
					    {
    					// src:RGB565 -> dst:ARGB8888
    					pix = (TUint32) *(bufsrc16 + ( TInt32 ) p.iX + (lScrPp)*( TInt32 ) p.iY);
    					buf[x] = RGB565toRGB888(pix);
					    }
                    }
                }
			buf += dstStride;
			if (buf + dstStride >= bufLimit)
                {
                    aSrc->UnlockHeap();
                    return;
                }
            }
        }
    else    // Alpha blending and/or transform is required.
        {
        TPoint pinv;
        TGfxPoint2D p;

		TUint32* obuf = buf;
		TInt32 xstart, xend, yend;
		TGfxPoint2D horizDeriv;
		TGfxPoint2D origin, top, right, corner;
		TGfxPoint2D east, west, south, north;

		origin.iX = aP.iX;
		origin.iY = aP.iY;
		corner.iX = aP.iX + (TFloatFixPt)srcWidth;
		corner.iY = aP.iY + (TFloatFixPt)srcHeight;
		top.iX =	aP.iX;
		top.iY =	aP.iY + (TFloatFixPt)srcHeight;
		right.iX =	aP.iX + (TFloatFixPt)srcWidth;
		right.iY =	aP.iY;

		// transform corners of the source rectangle.
        iTransform->Transform(&origin, &origin, 1);
        iTransform->Transform(&top, &top, 1);
        iTransform->Transform(&right, &right, 1);
		iTransform->Transform(&corner, &corner, 1);

		// sort four corners into appropriate order
		west = origin; east = corner; north = top; south = right;
		if (top.iX < origin.iX)
		    {
		    west = top;
		    east = right;
		    north = corner;
		    south = origin;
		    }
		else
		    {
		    west = origin;
		    east = corner;
		    north = top;
		    south = right;
		    }

		if (corner.iX < west.iX)
		    {
		    west = corner;
		    east = origin;
		    north = right;
		    south = top;
		    }

		if (right.iX < west.iX)
		    {
		    west = right;
		    east = top;
		    north = origin;
		    south = corner;
		    }

		// compute derivitives for scanline in source.
		p.iX = 1;
		p.iY = 0;
		inv.Transform( &p, & p, 1 );
		horizDeriv.iX = p.iX;
		horizDeriv.iY = p.iY;
		p.iX = 0;
		p.iY = 0;
		inv.Transform( &p, & p, 1 );
		horizDeriv.iX -= p.iX;
		horizDeriv.iY -= p.iY;

        yend = rect.iBr.iY;
        TInt lScrPp = 0;
		if ( dispmode ==  EColor16MU)
		    {
			lScrPp = srcWidth;
		    }
		else
			{
			lScrPp = srcWidth+pad;
			}

		TUint32 transparentColorRgb565 = aTransparentColor.ColorRgb565();

        for ( y = rect.iTl.iY; y <= yend; y++ )
            {
            p.iY = y;

			// compute scanline starting position

			if ( ((TFloatFixPt)y) > west.iY )
			    {
				if(north.iY - west.iY == (TFloatFixPt)0)
					xstart = (TInt32) west.iX;
				else
					xstart = (TInt32) (west.iX + (north.iX - west.iX) * ( (((TFloatFixPt)y) - west.iY) / (north.iY - west.iY) ));
			    }
			else
			    {
				if(west.iY - south.iY == (TFloatFixPt)0)
					xstart = (TInt32) south.iX;
				else
					xstart = (TInt32) (south.iX + (west.iX - south.iX) * ( (((TFloatFixPt)y) - south.iY) / (west.iY - south.iY) ));
			    }

			// compute scanline edges end position
			if ( (TFloatFixPt)y > east.iY )
			    {
				if(north.iY - east.iY == (TFloatFixPt)0)
					xend = (TInt32) north.iX;
				else
					xend = (TInt32) (east.iX + (north.iX - east.iX) * ( (((TFloatFixPt)y) - east.iY) / (north.iY - east.iY) ));
			    }
			else
			    {
				if ( (east.iY - south.iY) == (TFloatFixPt)0 )
					xend = (TInt32) east.iX;
				else
					xend = (TInt32) (south.iX + (east.iX - south.iX) * ( (((TFloatFixPt)y) - south.iY) / (east.iY - south.iY) ));
			    }

		    // does span clip against edges
			if(xstart < rect.iTl.iX)
				xstart = rect.iTl.iX;

			if(xend >= rect.iBr.iX)  // iX-1 seems to be the limit, using iX rolls over to next line
				xend = rect.iBr.iX-1;

			p.iX = xstart;
			// transform that starting position to the source image
			inv.Transform( &p, & p, 1 );
			buf += xstart-rect.iTl.iX;

			p.iY -= TFloatFixPt(.5f) * currentScale;

			if (aTransparentColor.GetARGB() == KGfxColorNull)
				{
					for ( x = xstart; x <= xend; x++ )
    					{
						//increment the location of the position by one horizontal location
						pinv.iX = p.iX;
						pinv.iY = p.iY;
						if ( srcrect.Contains( pinv ) )
						    {
							pinv.iX -= ( TInt32 ) aP.iX;
							pinv.iY -= ( TInt32 ) aP.iY;
        					if ( dispmode == EColor16MU )
        					    {
                                *buf = (TUint32)*(bufsrc32 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY));
        					    }
        					else
        					    {
    							// src:RGB565 -> dst:ARGB8888
    							pix = (TUint32)*(bufsrc16 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY));
    							*buf = RGB565toRGB888(pix);
    						    }
						    }
						p.iX += horizDeriv.iX;
						p.iY += horizDeriv.iY;
						buf++;
					}
				}
			else
				{
				for ( x = xstart; x <= xend; x++ )
					{
					//increment the location of the position by one horizontal location
					pinv.iX = p.iX;
					pinv.iY = p.iY;

					if ( srcrect.Contains( pinv ) )
						{
						pinv.iX -= ( TInt32 ) aP.iX;
						pinv.iY -= ( TInt32 ) aP.iY;

						if ( aReverse ) // copy source to destination. Used for Text blending with final framebufer.
						    { // this used for text only
        					if ( dispmode == EColor16MU )
        					    {
    							*(bufsrc32 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY)) = *buf ;
    						    }
    						else
        					    {
    							*(bufsrc16 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY)) = *buf ;
    						    }
						    }
						else
						    {
        					if ( dispmode == EColor16MU )
        					    {
    							pix = (TUint32)*(bufsrc32 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY));
						        if (pix != transparentColorRgb565)
							        {
							        *buf = pix;
	                        	    }
							    }
							else
        					    {
    							pix = (TUint32) *(bufsrc16 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY));
                                if (pix != transparentColorRgb565)
                                    {
								    *buf = RGB565toRGB888(pix);
                                    }
							    }
							}
						}
					p.iX += horizDeriv.iX;
					p.iY += horizDeriv.iY;
					buf++;
					}
				}
			buf = obuf+aDstWidth;
			obuf += aDstWidth;
			if (obuf + aDstWidth >= bufLimit)
                {
                aSrc->UnlockHeap();
                return;
                }
            }
        }

    aSrc->UnlockHeap();
    }

// --------------------------------------------------------------------------
//  void TGfxImageTransformer::Draw( CFbsBitmap* aSrc,
// ---------------------------------------------------------------------------
 void TGfxImageTransformer::Draw( CFbsBitmap* aSrc,
                                          const TGfxPoint2D& aP,
                                          TUint32* aDst,
                                          TInt aDstWidth,
                                          TInt aDstHeight,
                                          const TRect& aClipRect
                                          )
    {
    TDisplayMode dispmode = aSrc->DisplayMode();
    if ( dispmode != EColor16MU )
        {
        return;
        }

    TRect rect;
    TInt32 x, y;

    TFloatFixPt currentScale = iTransform->ScalingFactor();

    TRect srcrect( TPoint( aP.iX, aP.iY ), aSrc->SizeInPixels() );
    TRect dstrect( TPoint( 0, 0 ), TSize(aDstWidth, aDstHeight) );
    TInt32 srcWidth = aSrc->SizeInPixels().iWidth;
    TInt32 srcHeight = aSrc->SizeInPixels().iHeight;

    TGfxRectangle2D rect2d = GetTransformedBound( aSrc, aP );
    rect.SetRect( TPoint( rect2d.iX, rect2d.iY ), TSize( rect2d.iWidth, rect2d.iHeight ) );
    rect.Intersection(aClipRect);   // Clipping

    if ( !rect.Intersects( dstrect ) )    // clip rect with dst image
        return;

    aSrc->LockHeap();
    rect.Intersection( dstrect );

    TUint32* bufsrc32 = (TUint32*) aSrc->DataAddress();

    TUint32* buf = aDst + rect.iTl.iX + ( rect.iTl.iY * aDstWidth );
    TUint32* bufLimit = aDst + aDstWidth * aDstHeight;
    TUint32 pix = 0;

    TGfxAffineTransform inv = iTransform->CreateInverse();

        {
        TPoint pinv;
        TGfxPoint2D p;

        TUint32* obuf = buf;
        TInt32 xstart, xend, yend;
        TGfxPoint2D horizDeriv;
        TGfxPoint2D origin, top, right, corner;
        TGfxPoint2D east, west, south, north;

        origin.iX = aP.iX;
        origin.iY = aP.iY;
        corner.iX = aP.iX + (TFloatFixPt)srcWidth;
        corner.iY = aP.iY + (TFloatFixPt)srcHeight;
        top.iX =    aP.iX;
        top.iY =    aP.iY + (TFloatFixPt)srcHeight;
        right.iX =  aP.iX + (TFloatFixPt)srcWidth;
        right.iY =  aP.iY;

        // transform corners of the source rectangle.
        iTransform->Transform(&origin, &origin, 1);
        iTransform->Transform(&top, &top, 1);
        iTransform->Transform(&right, &right, 1);
        iTransform->Transform(&corner, &corner, 1);

        // sort four corners into appropriate order
        west = origin; east = corner; north = top; south = right;
        if (top.iX < origin.iX)
            {
            west = top;
            east = right;
            north = corner;
            south = origin;
            }
        else
            {
            west = origin;
            east = corner;
            north = top;
            south = right;
            }

        if (corner.iX < west.iX)
            {
            west = corner;
            east = origin;
            north = right;
            south = top;
            }

        if (right.iX < west.iX)
            {
            west = right;
            east = top;
            north = origin;
            south = corner;
            }

        // compute derivitives for scanline in source.
        p.iX = 8;
        p.iY = 0;
        inv.Transform( &p, & p, 1 );
        horizDeriv.iX = p.iX;
        horizDeriv.iY = p.iY;
        p.iX = 0;
        p.iY = 0;
        inv.Transform( &p, & p, 1 );
        horizDeriv.iX -= p.iX;
        horizDeriv.iY -= p.iY;
        horizDeriv.iX = horizDeriv.iX >> 3;
        horizDeriv.iY = horizDeriv.iY >> 3;

        yend = rect.iBr.iY;
        TInt lScrPp = 0;
        lScrPp = srcWidth;


        for ( y = rect.iTl.iY; y <= yend; y++ )
            {
            p.iY = y;

            // compute scanline starting position

            if ( ((TFloatFixPt)y) > west.iY )
                {
                if(north.iY - west.iY == (TFloatFixPt)0)
                    xstart = (TInt32) west.iX;
                else
                    xstart = (TInt32) (west.iX + (north.iX - west.iX) * ( (((TFloatFixPt)y) - west.iY) / (north.iY - west.iY) ));
                }
            else
                {
                if(west.iY - south.iY == (TFloatFixPt)0)
                    xstart = (TInt32) south.iX;
                else
                    xstart = (TInt32) (south.iX + (west.iX - south.iX) * ( (((TFloatFixPt)y) - south.iY) / (west.iY - south.iY) ));
                }

            // compute scanline edges end position
            if ( (TFloatFixPt)y > east.iY )
                {
                if(north.iY - east.iY == (TFloatFixPt)0)
                    xend = (TInt32) north.iX;
                else
                    xend = (TInt32) (east.iX + (north.iX - east.iX) * ( (((TFloatFixPt)y) - east.iY) / (north.iY - east.iY) ));
                }
            else
                {
                if ( (east.iY - south.iY) == (TFloatFixPt)0 )
                    xend = (TInt32) east.iX;
                else
                    xend = (TInt32) (south.iX + (east.iX - south.iX) * ( (((TFloatFixPt)y) - south.iY) / (east.iY - south.iY) ));
                }

            // does span clip against edges
            if(xstart < rect.iTl.iX)
                xstart = rect.iTl.iX;

            if(xend >= rect.iBr.iX)  // iX-1 seems to be the limit, using iX rolls over to next line
                xend = rect.iBr.iX-1;

            p.iX = xstart;
            // transform that starting position to the source image
            inv.Transform( &p, & p, 1 );
            buf += xstart-rect.iTl.iX;
            // Change here
            p.iY -= TFloatFixPt(0.5f) * currentScale;

                {
                    for ( x = xstart; x <= xend; x++ )
                        {
                        //increment the location of the position by one horizontal location
                        pinv.iX = p.iX;
                        pinv.iY = p.iY;
                        if ( srcrect.Contains( pinv ) )
                            {
                            pinv.iX -= ( TInt32 ) aP.iX;
                            pinv.iY -= ( TInt32 ) aP.iY;

                            pix = (TUint32)*(bufsrc32 + ( TInt32 ) pinv.iX + (lScrPp)*( TInt32 ) (pinv.iY));
                            TUint srcAlpha = ( pix & 0xFF000000 ) >> 24;
                            if ( srcAlpha == 0xFF )
                                {
                                *buf = pix;
                                }
                            else if ( srcAlpha > 0 )
                                {
                                TUint destPix = *buf;
                                TUint destAlpha = ( destPix & 0xFF000000 ) >> 24;
                                // Blending needed: blend with background as opaque
                                if ( destAlpha == 0xFF )
                                    {
                                    TReal32 alpha = 0.0039215686 * srcAlpha; // 0.0039.. = 1/255
                                    TReal32 inverseAlpha = 1.0f - alpha;
                                    *buf = ((((TUint)((pix     >> 16 & 0xff) * alpha) +
                                              (TUint)((destPix >> 16 & 0xff) * inverseAlpha))) << 16) | // red
                                           ((((TUint)((pix     >> 8  & 0xff) * alpha) +
                                              (TUint)((destPix >> 8  & 0xff) * inverseAlpha))) << 8 ) | // green
                                           ((((TUint)((pix           & 0xff) * alpha) +
                                              (TUint)((destPix       & 0xff) * inverseAlpha)))      ) | // blue
                                              (0xFF000000);                                             // full alpha (already blended)
                                    }
                                // Apply alpha to each color channel of source only
                                // Important: use original alpha
                                else
                                    {
                                    TReal32 alpha = 0.0039215686 * srcAlpha;
                                    *buf = (srcAlpha << 24)|
                                           ((TUint)((pix >> 16 & 0xff) * alpha) << 16) |
                                           ((TUint)((pix     >> 8 & 0xff) * alpha) << 8) |
                                           ((TUint)((pix & 0xff) * alpha));
                                    }
                                }
                            }
                        p.iX += horizDeriv.iX;
                        p.iY += horizDeriv.iY;
                        buf++;
                    }
                }

            buf = obuf+aDstWidth;
            obuf += aDstWidth;
            if (obuf + aDstWidth >= bufLimit)
                {
                aSrc->UnlockHeap();
                return;
                }
            }
        }

    aSrc->UnlockHeap();
    }

//  TGfxRectangle2D TGfxImageTransformer::GetTransformedBound( CFbsBitmap* aSrc,
// ---------------------------------------------------------------------------
 TGfxRectangle2D TGfxImageTransformer::GetTransformedBound( CFbsBitmap* aSrc,
                                                                    const TGfxPoint2D& aOffset )
    {
    TGfxRectangle2D rect;
    TGfxPoint2D rectp[4];

    rectp[0].iX = aOffset.iX;
    rectp[0].iY = aOffset.iY;
    rectp[1].iX = aOffset.iX + ( TFloatFixPt ) aSrc->SizeInPixels().iWidth;
    rectp[1].iY = aOffset.iY;
    rectp[2].iX = aOffset.iX + ( TFloatFixPt ) aSrc->SizeInPixels().iWidth;
    rectp[2].iY = aOffset.iY + ( TFloatFixPt ) aSrc->SizeInPixels().iHeight;
    rectp[3].iX = aOffset.iX;
    rectp[3].iY = aOffset.iY + ( TFloatFixPt ) aSrc->SizeInPixels().iHeight;
    iTransform->Transform( rectp, rectp, 4 );

    TInt32 i;
    //MinX
    rect.iX = rectp[0].iX;
    rect.iY = rectp[0].iY;
    rect.iWidth = rectp[0].iX;
    rect.iHeight = rectp[0].iY;

    for ( i = 1; i < 4; i++ )
	    {
        if ( rectp[i].iX < rect.iX )
            rect.iX = rectp[i].iX;
    //MinY
        if ( rectp[i].iY < rect.iY )
            rect.iY = rectp[i].iY;
    //MaxX -> width
        if ( rectp[i].iX > rect.iWidth )
            rect.iWidth = rectp[i].iX;
    //MaxY -> height
        if ( rectp[i].iY > rect.iHeight )
            rect.iHeight = rectp[i].iY;
    	}
    rect.iWidth -= rect.iX;
    rect.iHeight -= rect.iY;

    return rect;
    }

