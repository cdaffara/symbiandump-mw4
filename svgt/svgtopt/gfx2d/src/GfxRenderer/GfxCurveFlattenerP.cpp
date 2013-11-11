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


#include "GfxShape.h"
#include "GfxAffineTransform.h"


// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// TGfxCurveFlattener::TGfxCurveFlattener( TGfxAffineTransform* aTransform )
// ---------------------------------------------------------------------------
TGfxCurveFlattener::TGfxCurveFlattener( TGfxAffineTransform* aTransform )
    {
    iTransform = aTransform;
    }



// --------------------------------------------------------------------------
// void TGfxCurveFlattener::AddSubCurveQuadL( CGfxEdgeListP* aRenderer,
// ---------------------------------------------------------------------------
void TGfxCurveFlattener::AddSubCurveQuadL( CGfxEdgeListP* aRenderer,
                                           TFixPt* aCtrlPoints,
                                           TInt32 aLimit )
    {
    if ( aLimit <= 0 )
        {
        // Add segments to renderer
        // the first point doesn't have to be transformed
        // because it is same as the last vertex
        TFixPt* p2 = aCtrlPoints + 2;
        iTransform->Transform( p2, 2 );
        aRenderer->AddPoint( aCtrlPoints[2], aCtrlPoints[3], EFalse, EFalse );
        aRenderer->AddPoint( aCtrlPoints[4], aCtrlPoints[5], EFalse, EFalse );
        return;
        }

    TFixPt left[6], right[6];
    TFixPt x0, y0, xm, ym, x1, y1;
    x0 = aCtrlPoints[0];
    y0 = aCtrlPoints[1];
    xm = aCtrlPoints[2];
    ym = aCtrlPoints[3];
    x1 = aCtrlPoints[4];
    y1 = aCtrlPoints[5];
    x0 = ( x0 + xm ) >> 1;
    y0 = ( y0 + ym ) >> 1;
    x1 = ( x1 + xm ) >> 1;
    y1 = ( y1 + ym ) >> 1;
    xm = ( x0 + x1 ) >> 1;
    ym = ( y0 + y1 ) >> 1;
    left[0] = aCtrlPoints[0];
    left[1] = aCtrlPoints[1];
    left[2] = x0;
    left[3] = y0;
    left[4] = xm;
    left[5] = ym;
    right[0] = xm;
    right[1] = ym;
    right[2] = x1;
    right[3] = y1;
    right[4] = aCtrlPoints[4];
    right[5] = aCtrlPoints[5];
    AddSubCurveQuadL( aRenderer, left, aLimit - 1 );
    AddSubCurveQuadL( aRenderer, right, aLimit - 1 );
    }

// --------------------------------------------------------------------------
// void TGfxCurveFlattener::AddSubCurveCubicL( CGfxEdgeListP* aRenderer,
// ---------------------------------------------------------------------------
void TGfxCurveFlattener::AddSubCurveCubicL( CGfxEdgeListP* aRenderer,
                                            TFixPt* aCtrlPoints,
                                            TInt32 aLimit )
    {
    if ( aLimit <= 0 )
        {
        // Add segments to renderer
        TFixPt* p2 = aCtrlPoints + 2;
        iTransform->Transform( p2, 3 );
        aRenderer->AddPoint( aCtrlPoints[2], aCtrlPoints[3], EFalse, EFalse );
        aRenderer->AddPoint( aCtrlPoints[4], aCtrlPoints[5], EFalse, EFalse );
        aRenderer->AddPoint( aCtrlPoints[6], aCtrlPoints[7], EFalse, EFalse );
        return;
        }

    TFixPt left[8], right[8];
    TFixPt x0, y0, xm, ym, xm1, ym1, xm2, ym2, x1, y1;

    x0 = aCtrlPoints[0];
    y0 = aCtrlPoints[1];
    xm1 = aCtrlPoints[2];
    ym1 = aCtrlPoints[3];
    xm2 = aCtrlPoints[4];
    ym2 = aCtrlPoints[5];
    x1 = aCtrlPoints[6];
    y1 = aCtrlPoints[7];

    x0 = ( x0 + xm1 ) >> 1;
    y0 = ( y0 + ym1 ) >> 1;
    x1 = ( x1 + xm2 ) >> 1;
    y1 = ( y1 + ym2 ) >> 1;
    xm = ( xm1 + xm2 ) >> 1;
    ym = ( ym1 + ym2 ) >> 1;
    xm1 = ( x0 + xm ) >> 1;
    ym1 = ( y0 + ym ) >> 1;
    xm2 = ( x1 + xm ) >> 1;
    ym2 = ( y1 + ym ) >> 1;
    xm = ( xm1 + xm2 ) >> 1;
    ym = ( ym1 + ym2 ) >> 1;

    left[0] = aCtrlPoints[0];
    left[1] = aCtrlPoints[1];
    left[2] = x0;
    left[3] = y0;
    left[4] = xm1;
    left[5] = ym1;
    left[6] = xm;
    left[7] = ym;
    right[0] = xm;
    right[1] = ym;
    right[2] = xm2;
    right[3] = ym2;
    right[4] = x1;
    right[5] = y1;
    right[6] = aCtrlPoints[6];
    right[7] = aCtrlPoints[7];
    AddSubCurveCubicL( aRenderer, left, aLimit - 1 );
    AddSubCurveCubicL( aRenderer, right, aLimit - 1 );
    }
