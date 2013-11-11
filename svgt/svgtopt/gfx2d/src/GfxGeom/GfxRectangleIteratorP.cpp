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


#include "GfxRectangleIteratorP.h"
#include "GfxRectangle2D.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxRectangleIteratorP::CGfxRectangleIteratorP( TGfxRectangularShape* aRect,
// ---------------------------------------------------------------------------
CGfxRectangleIteratorP::CGfxRectangleIteratorP( TGfxRectangularShape* aRect,
                                                TGfxAffineTransform* aTransform )
    {
    TFloatFixPt KZero;
    if ( aRect->iWidth < KZero )
        aRect->iWidth = KZero;

    if ( aRect->iHeight < KZero )
        aRect->iHeight = KZero;

    iRect = aRect;
    iTransform = aTransform;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxRectangleIteratorP::~CGfxRectangleIteratorP( )
// ---------------------------------------------------------------------------
CGfxRectangleIteratorP::~CGfxRectangleIteratorP( )
    {
    }

// --------------------------------------------------------------------------
// TGfxSegType CGfxRectangleIteratorP::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
TGfxSegType CGfxRectangleIteratorP::CurrentSegment( TFloatFixPt* aCoords )
    {
    TGfxSegType segtype;

    switch ( iIdx )
        {
        case 0:
            aCoords[0] = iRect->iX;
            aCoords[1] = iRect->iY;
            segtype = EGfxSegMoveTo;
            break;
        case 1:
            aCoords[0] = iRect->iX + iRect->iWidth;
            aCoords[1] = iRect->iY ;
            segtype = EGfxSegLineTo;
            break;
        case 2:
            aCoords[0] = iRect->iX + iRect->iWidth;
            aCoords[1] = iRect->iY + iRect->iHeight;
            segtype = EGfxSegLineTo;
            break;
        case 3:
            aCoords[0] = iRect->iX;
            aCoords[1] = iRect->iY + iRect->iHeight;
            segtype = EGfxSegLineTo;
            break;
        default:
            segtype = EGfxSegClose;
        }

    if ( !iTransform->IsIdentity() )
        {
        iTransform->Transform( aCoords, aCoords, 1 );
        }

    return segtype;
    }

// --------------------------------------------------------------------------
// TBool CGfxRectangleIteratorP::IsDone()
// ---------------------------------------------------------------------------
TBool CGfxRectangleIteratorP::IsDone()
    {
    return ( iIdx > 4 );
    }

// --------------------------------------------------------------------------
// void CGfxRectangleIteratorP::Next()
// ---------------------------------------------------------------------------
void CGfxRectangleIteratorP::NextL()
    {
    iIdx++;
    }


