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


#include "GfxLineIteratorP.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxLineIteratorP::CGfxLineIteratorP( TGfxLine2D* aLine,
// ---------------------------------------------------------------------------
CGfxLineIteratorP::CGfxLineIteratorP( TGfxLine2D* aLine,
									  TGfxAffineTransform* aAffine ) :
									  iLine( aLine ),
									  iIdx( 0 )
    {
    iTransform = aAffine;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
// CGfxLineIteratorP::~CGfxLineIteratorP()
// ---------------------------------------------------------------------------
CGfxLineIteratorP::~CGfxLineIteratorP()
{
}

// --------------------------------------------------------------------------
// TGfxSegType CGfxLineIteratorP::CurrentSegment( TFloatFixPt* aCoords )
// ---------------------------------------------------------------------------
TGfxSegType CGfxLineIteratorP::CurrentSegment( TFloatFixPt* aCoords )
    {
    TGfxSegType segtype;

    if ( iIdx == 0 )
        {
        aCoords[0] = iLine->iX1;
        aCoords[1] = iLine->iY1;
        segtype = EGfxSegMoveTo;
        }
    else
        {
        aCoords[0] = iLine->iX2;
        aCoords[1] = iLine->iY2;
        segtype = EGfxSegLineTo;
        }
    if ( !iTransform->IsIdentity() )
        {
        iTransform->Transform( aCoords, aCoords, 1 );
        }

    return segtype;
    }

// --------------------------------------------------------------------------
// TBool CGfxLineIteratorP::IsDone()
// ---------------------------------------------------------------------------
TBool CGfxLineIteratorP::IsDone()
    {
    return ( iIdx > 1 );
    }

// --------------------------------------------------------------------------
// void CGfxLineIteratorP::Next()
// ---------------------------------------------------------------------------
void CGfxLineIteratorP::NextL()
    {
    iIdx++;
    }

