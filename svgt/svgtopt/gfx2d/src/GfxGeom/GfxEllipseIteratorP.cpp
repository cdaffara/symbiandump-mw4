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


#include "GfxEllipseIteratorP.h"
#include "GfxEllipse2D.h"



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxEllipseIteratorP::CGfxEllipseIteratorP( TGfxEllipse2D* aEllipse,
// ---------------------------------------------------------------------------
CGfxEllipseIteratorP::CGfxEllipseIteratorP( TGfxEllipse2D* aEllipse,
                                            TGfxAffineTransform* aTransform )
    {
    TFloatFixPt KZero;
    if ( aEllipse->iWidth < KZero )
        aEllipse->iWidth = KZero;

    if ( aEllipse->iHeight < KZero )
        aEllipse->iHeight = KZero;

    iEllipse = aEllipse;
    iTransform = aTransform;
    }

// --------------------------------------------------------------------------
// TGfxSegType CGfxEllipseIteratorP::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
TGfxSegType CGfxEllipseIteratorP::CurrentSegment( TFloatFixPt* aCoords )
    {
    TGfxSegType segtype;

    TInt32 pts = 3;

    TFloatFixPt px[12], py[12];

	#ifdef SVG_FLOAT_BUILD    
    TFloatFixPt cx = iEllipse->iX + (iEllipse->iWidth * TFloatFixPt(.5f));
    TFloatFixPt cy = iEllipse->iY + (iEllipse->iHeight * TFloatFixPt(.5f));
    TFloatFixPt offx = iEllipse->iWidth * TFloatFixPt(.257f);
    TFloatFixPt offy = iEllipse->iHeight * TFloatFixPt(.257f);
	#else	
    TFloatFixPt cx = iEllipse->iX + (iEllipse->iWidth >> 1);
    TFloatFixPt cy = iEllipse->iY + (iEllipse->iHeight >> 1);
    TFloatFixPt offx = TFloatFixPt( 0x41ca, ETrue ) * iEllipse->iWidth;
    TFloatFixPt offy = TFloatFixPt( 0x41ca, ETrue ) * iEllipse->iHeight;
	#endif

    px[0] = px[1] = px[11] = iEllipse->iX;
    px[2] = px[10] = cx - offx;
    px[3] = px[9] = cx;
    px[4] = px[8] = cx + offx;
    px[5] = px[6] = px[7] = iEllipse->iX + iEllipse->iWidth;

    py[2] = py[3] = py[4] = iEllipse->iY;
    py[1] = py[5] = cy - offy;
    py[0] = py[6] = cy;
    py[7] = py[11] = cy + offy;
    py[8] = py[9] = py[10] = iEllipse->iY + iEllipse->iHeight;

    TFloatFixPt* p = aCoords;
    switch ( iIdx )
        {
        case 0:
            p[0] = px[0];
            p[1] = py[0];
            segtype = EGfxSegMoveTo;
            pts = 1;
            break;
        case 1:
            p[0] = px[1];
            p[1] = py[1];
            p[2] = px[2];
            p[3] = py[2];
            p[4] = px[3];
            p[5] = py[3];
            segtype = EGfxSegCubicTo;
            break;
        case 2:
            p[0] = px[4];
            p[1] = py[4];
            p[2] = px[5];
            p[3] = py[5];
            p[4] = px[6];
            p[5] = py[6];
            segtype = EGfxSegCubicTo;
            break;
        case 3:
            p[0] = px[7];
            p[1] = py[7];
            p[2] = px[8];
            p[3] = py[8];
            p[4] = px[9];
            p[5] = py[9];
            segtype = EGfxSegCubicTo;
            break;
        case 4:
            p[0] = px[10];
            p[1] = py[10];
            p[2] = px[11];
            p[3] = py[11];
            p[4] = px[0];
            p[5] = py[0];
            segtype = EGfxSegCubicTo;
            break;
        default:
            segtype = EGfxSegClose;
            pts = 0;
        }



    if ( !iTransform->IsIdentity() && pts > 0 )
        {
        iTransform->Transform( aCoords, aCoords, pts );
        }

    return segtype;
    }

// --------------------------------------------------------------------------
// TBool CGfxEllipseIteratorP::IsDone()
// ---------------------------------------------------------------------------
TBool CGfxEllipseIteratorP::IsDone()
    {
    return ( iIdx > 4 );
    }

// --------------------------------------------------------------------------
// void CGfxEllipseIteratorP::NextL()
// ---------------------------------------------------------------------------
void CGfxEllipseIteratorP::NextL()
    {
    iIdx++;
    }


