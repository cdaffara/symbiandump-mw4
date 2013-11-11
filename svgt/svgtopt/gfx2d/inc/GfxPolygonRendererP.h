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


#ifndef GFXPOLYGONRENDERERP_H
#define GFXPOLYGONRENDERERP_H

#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"
#include "GfxColor.h"



class TGfxColor;
class MGfxPaint;
class CGfxEdgeListP;
class TGfxRendererInfoP;


/**
 * Implementation of a rendering class for polygons.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */

class TGfxPolygonRendererP
    {
    public:

        /**
         * constructor
         */
                            TGfxPolygonRendererP();

        /**
         * initiatize
         */
        void                InitializeP( CGfxEdgeListP* aEdgeList,
                                         TGfxRendererInfoP* aRenderInfo,
                                         MGfxPaint* aPaint,
                                         TBool aWindingNoneZero );

        /**
         * render scan line
         */
        void                RenderScanlineL( TInt aY, TUint32 aAlpha );


        /**
         * blit scan line
         */
        void                BlitScanline( TUint8* aSrc,
                                          TInt aY,
                                          TInt aStart,
                                          TInt aEnd );


        /**
         * draw HLine
         */
        static void         DrawHLine( TUint16* aBuf,
                                       TUint32 aColor,
                                       TInt aLeftX,
                                       TInt aWidth );

    protected:



        CGfxEdgeListP*      iEdgeList;
        TGfxRendererInfoP*  iRenderInfo;
        MGfxPaint*          iPaint;



        TBool               iWindingNoneZero;
    };


#endif      // GFXPOLYGONRENDERERP_H
