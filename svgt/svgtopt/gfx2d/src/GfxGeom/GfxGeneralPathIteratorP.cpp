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


#include "GfxGeneralPathIteratorP.h"
#include "GfxGeneralPath.h"



// --------------------------------------------------------------------------
//  CGfxGeneralPathIteratorP::CGfxGeneralPathIteratorP( CGfxGeneralPath* aPath,
// ---------------------------------------------------------------------------
 CGfxGeneralPathIteratorP::CGfxGeneralPathIteratorP( CGfxGeneralPath* aPath,
                                                             TGfxAffineTransform* aTransform )
    {
    iPath = aPath;
    iTransform = aTransform;
    }

//
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  CGfxGeneralPathIteratorP::~CGfxGeneralPathIteratorP()
// ---------------------------------------------------------------------------
 CGfxGeneralPathIteratorP::~CGfxGeneralPathIteratorP()
    {
    }



// --------------------------------------------------------------------------
//  TGfxSegType CGfxGeneralPathIteratorP::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
 TGfxSegType CGfxGeneralPathIteratorP::CurrentSegment( TFloatFixPt* aCoords )
    {
    TGfxSegType segtype = EGfxSegClose;
    TInt32 pts = 1;

    TFloatFixPt* pd = aCoords;
    if(iPath->iPointCoords->Count() <= 0)
		{
		return segtype;
		}
    TFloatFixPt* ps = &( iPath->iPointCoords->operator[]( 0 ) );
    switch ( ( TGfxSegType ) ( *( iPath->iPointTypes ) )[iTypeIdx] )
        {
        case EGfxSegMoveTo:
            pd[0] = ps[iPointIdx];
            pd[1] = ps[iPointIdx + 1];
            iStartPoint.iX = pd[0];
            iStartPoint.iY = pd[1];
            segtype = EGfxSegMoveTo;
            break;
        case EGfxSegLineTo:
            pd[0] = ps[iPointIdx];
            pd[1] = ps[iPointIdx + 1];
            segtype = EGfxSegLineTo;
            break;
        case EGfxSegQuadTo:
            pd[0] = ps[iPointIdx];
            pd[1] = ps[iPointIdx + 1];
            pd[2] = ps[iPointIdx + 2];
            pd[3] = ps[iPointIdx + 3];
            segtype = EGfxSegQuadTo;
            pts = 2;
            break;
        case EGfxSegCubicTo:
            pd[0] = ps[iPointIdx];
            pd[1] = ps[iPointIdx + 1];
            pd[2] = ps[iPointIdx + 2];
            pd[3] = ps[iPointIdx + 3];
            pd[4] = ps[iPointIdx + 4];
            pd[5] = ps[iPointIdx + 5];
            segtype = EGfxSegCubicTo;
            pts = 3;
            break;
        case EGfxSegClose:
            pd[0] = iStartPoint.iX;
            pd[1] = iStartPoint.iY;
            segtype = EGfxSegClose;
            break;
        default:
            segtype = EGfxSegClose;
            pts = 0;
            break;
        }

    if ( !iTransform->IsIdentity() && pts > 0 )
        {
        iTransform->Transform( aCoords, aCoords, pts );
        }

    return segtype;
    }

// --------------------------------------------------------------------------
//  TBool CGfxGeneralPathIteratorP::IsDone()
// ---------------------------------------------------------------------------
 TBool CGfxGeneralPathIteratorP::IsDone()
    {
    return ( iPath->iPointTypes->Count() <= iTypeIdx );
    }

// --------------------------------------------------------------------------
//  void CGfxGeneralPathIteratorP::Next()
// ---------------------------------------------------------------------------
 void CGfxGeneralPathIteratorP::NextL()
    {
    if ( iPath->iPointTypes->Count() <= iTypeIdx - 1 )
        {
        return;
        }
    switch ( ( TGfxSegType ) ( *( iPath->iPointTypes ) )[iTypeIdx] )
        {
        case EGfxSegMoveTo:
            iPointIdx += 2;
            break;
        case EGfxSegLineTo:
            iPointIdx += 2;
            break;
        case EGfxSegQuadTo:
            iPointIdx += 4;
            break;
        case EGfxSegCubicTo:
            iPointIdx += 6;
            break;
        default:
            break;
        }

    iTypeIdx++;
    }

