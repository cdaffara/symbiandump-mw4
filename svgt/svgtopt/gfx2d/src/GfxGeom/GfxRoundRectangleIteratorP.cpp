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


#include "GfxRoundRectangleIteratorP.h"
#include "GfxRoundRectangle2D.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxRoundRectangleIteratorP::CGfxRoundRectangleIteratorP( TGfxRoundRectangle2D* aRoundRect,
// ---------------------------------------------------------------------------
CGfxRoundRectangleIteratorP::CGfxRoundRectangleIteratorP( TGfxRoundRectangle2D* aRoundRect,
                                                          TGfxAffineTransform* aAffine )
    : iRoundRect( aRoundRect )
    {
    iTransform = aAffine;



    TFloatFixPt invalid( -1 );
    TFloatFixPt KZero;

    iRx = aRoundRect->iArcWidth;
    iRy = aRoundRect->iArcHeight;

    if ( ( ( iRx == invalid ) && ( iRy == invalid ) ) ||
        ( iRx < invalid ) || ( iRy < invalid ) )
        {
        iRx = KZero;
        iRy = KZero;
        }
    else if ( iRx == invalid )
        {
        iRx = iRy;
        }
    else if ( iRy == invalid )
        {
        iRy = iRx;
        }

    if ( aRoundRect->iWidth < KZero )
        aRoundRect->iWidth = KZero;

    if ( aRoundRect->iHeight < KZero )
        aRoundRect->iHeight = KZero;

	#ifdef SVG_FLOAT_BUILD
    TFloatFixPt hw = aRoundRect->iWidth * TFloatFixPt(.5f);
    TFloatFixPt hh = aRoundRect->iHeight * TFloatFixPt(.5f);
	#else
    TFloatFixPt hw = aRoundRect->iWidth >> 1;
    TFloatFixPt hh = aRoundRect->iHeight >> 1;
	#endif
    if ( iRx > hw )
        iRx = hw;
    if ( iRy > hh )
        iRy = hh;

    aRoundRect->iArcWidth = iRx;
    aRoundRect->iArcHeight = iRy;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxRoundRectangleIteratorP::~CGfxRoundRectangleIteratorP()
// ---------------------------------------------------------------------------
CGfxRoundRectangleIteratorP::~CGfxRoundRectangleIteratorP()
{
}

// --------------------------------------------------------------------------
// TGfxSegType CGfxRoundRectangleIteratorP::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
TGfxSegType CGfxRoundRectangleIteratorP::CurrentSegment( TFloatFixPt* aCoords )
    {
    TGfxSegType segtype;
    TInt32 pts = 1;

    switch ( iIdx )
        {
        case 0:
            aCoords[0] = iRoundRect->iX + iRx;
            aCoords[1] = iRoundRect->iY;
            segtype = EGfxSegMoveTo;
            break;
        case 1:
            aCoords[0] = iRoundRect->iX + iRoundRect->iWidth - iRx;
            aCoords[1] = iRoundRect->iY ;
            segtype = EGfxSegLineTo;
            break;
        case 2:
            aCoords[0] = iRoundRect->iX + iRoundRect->iWidth;
            aCoords[1] = iRoundRect->iY ;
            aCoords[2] = iRoundRect->iX + iRoundRect->iWidth;
            aCoords[3] = iRoundRect->iY + iRy;
            segtype = EGfxSegQuadTo;
            pts = 2;
            break;
        case 3:
            aCoords[0] = iRoundRect->iX + iRoundRect->iWidth;
            aCoords[1] = iRoundRect->iY + iRoundRect->iHeight - iRy;
            segtype = EGfxSegLineTo;
            break;
        case 4:
            aCoords[0] = iRoundRect->iX + iRoundRect->iWidth;
            aCoords[1] = iRoundRect->iY + iRoundRect->iHeight;
            aCoords[2] = iRoundRect->iX + iRoundRect->iWidth - iRx;
            aCoords[3] = iRoundRect->iY + iRoundRect->iHeight;
            segtype = EGfxSegQuadTo;
            pts = 2;
            break;
        case 5:
            aCoords[0] = iRoundRect->iX + iRx;
            aCoords[1] = iRoundRect->iY + iRoundRect->iHeight;
            segtype = EGfxSegLineTo;
            break;
        case 6:
            aCoords[0] = iRoundRect->iX;
            aCoords[1] = iRoundRect->iY + iRoundRect->iHeight;
            aCoords[2] = iRoundRect->iX;
            aCoords[3] = iRoundRect->iY + iRoundRect->iHeight - iRy;
            segtype = EGfxSegQuadTo;
            pts = 2;
            break;
        case 7:
            aCoords[0] = iRoundRect->iX;
            aCoords[1] = iRoundRect->iY + iRy;
            segtype = EGfxSegLineTo;
            break;
        case 8:
            aCoords[0] = iRoundRect->iX;
            aCoords[1] = iRoundRect->iY;
            aCoords[2] = iRoundRect->iX + iRx;
            aCoords[3] = iRoundRect->iY;
            segtype = EGfxSegQuadTo;
            pts = 2;
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
// TBool CGfxRoundRectangleIteratorP::IsDone()
// ---------------------------------------------------------------------------
TBool CGfxRoundRectangleIteratorP::IsDone()
    {
    return ( iIdx > 9 );
    }

// --------------------------------------------------------------------------
// void CGfxRoundRectangleIteratorP::Next()
// ---------------------------------------------------------------------------
void CGfxRoundRectangleIteratorP::NextL()
    {
    iIdx++;
    }

