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


#include "GfxStrokeRendererP.h"
#include "GfxPolygonRendererP.h"
#include "GfxPaint.h"
#include "GfxColor.h"
#include "GfxRendererInfoP.h"
#include "GfxEdgeListP.h"

#include <fbs.h>


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// TGfxStrokeRendererP::TGfxStrokeRendererP() : iStrokeColor( 0 )
// ---------------------------------------------------------------------------
TGfxStrokeRendererP::TGfxStrokeRendererP() : iStrokeColor( 0 )
    {
    }

// --------------------------------------------------------------------------
// void TGfxStrokeRendererP::InitializeP( CGfxEdgeListP* aEdgeList,
// ---------------------------------------------------------------------------
void TGfxStrokeRendererP::InitializeP( CGfxEdgeListP* aEdgeList,
                                       TGfxRendererInfoP* aRenderInfo,
                                       TGfxColor aColor )
    {
    iEdgeList = aEdgeList;
    iRenderInfo = aRenderInfo;
    iStrokeColor = aColor;
    }


// ---------------------------------------------------------------------------
// Draw one pixel width stroke
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// void TGfxStrokeRendererP::RenderScanlineL( TInt aY, TUint32 /* aAlpha */ )
// ---------------------------------------------------------------------------
void TGfxStrokeRendererP::RenderScanlineL( TInt aY, TUint32 /* aAlpha */ )
    {
    if ( iRenderInfo->iClipMinY > aY || aY > iRenderInfo->iClipMaxY )
        return;

    if ( iStrokeColor.Value() == KGfxColorNull )
        return;

    TGfxSegEdge* edges = iEdgeList->iEdges;
    TInt edgeCount = iEdgeList->iEdgesCount;

    // ************ Buffer is 2 bytes per pixel **********
    TUint16* fb;
    fb = ( ( TUint16 * ) iRenderInfo->iDstBuf ) +
           aY * iRenderInfo->iWidth;

    TUint16 strokeCol = iStrokeColor.ColorRgb();

    for ( TInt i = 0; i < edgeCount; i++ )
        {
        if ( !edges[i].iOnlyForFill )
            {
            TInt32 pbufXmin, pbufXmax;
            TInt32 fbXmin, fbXmax;
            pbufXmin = edges[i].iStart;
            pbufXmax = edges[i].iEnd;
            fbXmin = pbufXmin + iEdgeList->iMinX;
            fbXmax = pbufXmax + iEdgeList->iMinX;
            if ( fbXmax >= iRenderInfo->iClipMinX &&
                           fbXmin < iRenderInfo->iClipMaxX )  // inside clipping
            {
                if ( fbXmin < iRenderInfo->iClipMinX )
                    {
                    pbufXmin += iRenderInfo->iClipMinX - fbXmin; // equivalent to 'pbufXmin += -fbXmin;'
                    fbXmin = iRenderInfo->iClipMinX;
                    }
                if ( iRenderInfo->iClipMaxX <= fbXmax )
                    {
                    pbufXmax -= fbXmax - iRenderInfo->iClipMaxX;
                    fbXmax = iRenderInfo->iClipMaxX - 1;
                    }
                TUint16* fbb =& fb[fbXmin];
                TInt fbw = fbXmax - fbXmin;
                if ( fbw > 10 )
                    {
                    TGfxPolygonRendererP::DrawHLine( fbb,
                                                     strokeCol,
                                                     fbXmin,
                                                     fbw );
                    }
                else
                    {
                    for ( TInt j = fbXmin; j <= fbXmax ; j++ )
                        *fbb++ = strokeCol;
                    }
                }
            }
        }
    }
