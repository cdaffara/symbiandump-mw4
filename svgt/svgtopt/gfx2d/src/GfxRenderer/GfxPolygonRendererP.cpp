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


#include "GfxPolygonRendererP.h"
#include "GfxPaint.h"
#include "GfxColor.h"
#include "GfxRendererInfoP.h"
#include "GfxEdgeListP.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// TGfxPolygonRendererP::TGfxPolygonRendererP()
// ---------------------------------------------------------------------------
TGfxPolygonRendererP::TGfxPolygonRendererP()
    {
    }

// --------------------------------------------------------------------------
// void TGfxPolygonRendererP::InitializeP( CGfxEdgeListP* aEdgeList,
// ---------------------------------------------------------------------------
void TGfxPolygonRendererP::InitializeP( CGfxEdgeListP* aEdgeList,
                                        TGfxRendererInfoP* aRenderInfo,
                                        MGfxPaint* aPaint,
                                        TBool aWindingNoneZero )
    {
    iEdgeList = aEdgeList;
    iRenderInfo = aRenderInfo;
    iPaint = aPaint;
    iWindingNoneZero = aWindingNoneZero;
    }

// --------------------------------------------------------------------------
// void TGfxPolygonRendererP::RenderScanlineL( TInt aY, TUint32 /* aAlpha */ )
// ---------------------------------------------------------------------------
void TGfxPolygonRendererP::RenderScanlineL( TInt aY, TUint32 /* aAlpha */ )
    {
    if ( iRenderInfo->iClipMinY >= aY || aY > iRenderInfo->iClipMaxY )
        return;

    TInt edgeCount = iEdgeList->iEdgesCount;
    TGfxSegEdge dummyEdge;
    dummyEdge.iEnd = -1;    // less than 0
    dummyEdge.iSign = 100;  // neither -1, 0, or 1

    TUint16 flatColor;
    flatColor = TGfxColor( iPaint->FlatColor() ).ColorRgb();

    // ************ Buffer is 2 bytes per pixel **********
    TUint16* fb;

    fb = ( ( TUint16 * ) iRenderInfo->iDstBuf ) +
           aY * iRenderInfo->iWidth;


    TInt i, j;

    TInt edgeSignSum = 0;
    TBool fill = EFalse;
    TGfxSegEdge* lastEdge =& dummyEdge; // iEdgeList->EdgeList();
    TGfxSegEdge* currentEdge = iEdgeList->EdgeList();
    for ( i = 0; i < edgeCount; i++ )
        {
        // Fill between edges
        TInt32 fbXmin, fbXmax;
        if ( fill )
            {
            fbXmin = lastEdge->iStart + iEdgeList->iMinX;
            fbXmax = currentEdge->iEnd + iEdgeList->iMinX;
            if ( fbXmax >= iRenderInfo->iClipMinX &&
                           fbXmin < iRenderInfo->iClipMaxX )  // inside clipping
            {
                if ( fbXmin < iRenderInfo->iClipMinX )
                    {
                    fbXmin = iRenderInfo->iClipMinX;
                    }
                if ( iRenderInfo->iClipMaxX <= fbXmax )
                    {
                    fbXmax = iRenderInfo->iClipMaxX - 1;
                    }
                TUint16* fbb =& fb[fbXmin];

                TInt fbw = fbXmax - fbXmin;
                if ( fbw > 10 )
                    {
                    DrawHLine( fbb, flatColor, fbXmin, fbw );
                    }
                else
                    {
                    for ( j = fbXmin; j <= fbXmax ; j++ )
                        *fbb++ = flatColor;
                    }
                }
            }   // end if (fill)

        // update sum of sign and determine fill/non-fill

        if ( iWindingNoneZero )
            {
            fill = ( edgeSignSum += currentEdge->iSign );  // Non-zero rule.
            }
        else
            {
            fill = ( ++edgeSignSum & 1 );       // Even/odd rule.
            }

        lastEdge = currentEdge;
        currentEdge = currentEdge->iNext;
        } // end for i (each edges)
    }



