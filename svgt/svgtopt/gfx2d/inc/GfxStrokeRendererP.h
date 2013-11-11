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


#ifndef GFXSTROKERENDERERP_H
#define GFXSTROKERENDERERP_H

#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"
#include "GfxColor.h"



class TGfxColor;
class MGfxPaint;
class CGfxEdgeListP;
class TGfxRendererInfoP;


class CFbsBitGc;

/**
 *  Implementation of a rendering class for a stroke.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxStrokeRendererP
    {
    public:
        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
                            TGfxStrokeRendererP();


        /**
         * Intialize this object.
         *
         * @since 1.0
         * @param aEdgeList : list of edge points.
         * @param aRenderInfo : rendering info.
         * @param aColor : color to draw.
         * @param aPolbuf : pixel output buffer.
         * @return
         */
         void                InitializeP( CGfxEdgeListP* aEdgeList,
                                         TGfxRendererInfoP* aRenderInfo,
                                         TGfxColor aColor );

        /**
         * Render at the given horizontal line.
         *
         * @since 1.0
         * @param aY : y coordinate of line to render.
         * @return
         */
        void                RenderScanlineL( TInt aY );

        /**
         * Render at the given horizontal line.
         *
         * @since 1.0
         * @param aY : y coordinate of line to render.
         * @param aAlpha : alpha value for blending.
         * @return
         */
        void                RenderScanlineL( TInt aY, TUint32 aAlpha );

    protected:


        CGfxEdgeListP*      iEdgeList;
        TGfxRendererInfoP*  iRenderInfo;
        TGfxColor           iStrokeColor;

        CFbsBitGc*          iGdiGc;
    };


#endif      // GFXSTROKERENDERERP_H
