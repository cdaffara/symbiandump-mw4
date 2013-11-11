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


#include "GfxStroke.h"
#include "GfxGeneralPath.h"

// --------------------------------------------------------------------------
//  TGfxStroke::TGfxStroke()
// ---------------------------------------------------------------------------
 TGfxStroke::TGfxStroke()
    {
    SetVars( 1, EGfxJoinMiter, EGfxCapButt, TFloatFixPt( 4 ) );
    }

// --------------------------------------------------------------------------
//  void TGfxStroke::SetStrokeWidth( TFloatFixPt aWidth )
// ---------------------------------------------------------------------------
 void TGfxStroke::SetStrokeWidth( TFloatFixPt aWidth )
    {
    iStrokeWidth = aWidth;
    }

// --------------------------------------------------------------------------
//  void TGfxStroke::SetMiterLimit( TFixPt aMiterLimit )
// ---------------------------------------------------------------------------
 void TGfxStroke::SetMiterLimit( TFloatFixPt aMiterLimit )
    {
    iMiterLimit = aMiterLimit;
    }

// --------------------------------------------------------------------------
//  void TGfxStroke::SetJoinType( TGfxJoinType aJoin )
// ---------------------------------------------------------------------------
 void TGfxStroke::SetJoinType( TGfxJoinType aJoin )
    {
    iJoin = aJoin;
    }

// --------------------------------------------------------------------------
//  void TGfxStroke::SetCapType( TGfxCapType aCap )
// ---------------------------------------------------------------------------
 void TGfxStroke::SetCapType( TGfxCapType aCap )
    {
    iCap = aCap;
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxStroke::StrokeWidth()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxStroke::StrokeWidth()
    {
    return iStrokeWidth;
    }

// --------------------------------------------------------------------------
//  TGfxJoinType TGfxStroke::StrokeJoin()
// ---------------------------------------------------------------------------
 TGfxJoinType TGfxStroke::StrokeJoin()
	{
	return iJoin;
	}

// --------------------------------------------------------------------------
//  TGfxCapType TGfxStroke::StrokeCap()
// ---------------------------------------------------------------------------
 TGfxCapType TGfxStroke::StrokeCap()
	{
	return iCap;
	}

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxStroke::StrokeMiterLimit()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxStroke::StrokeMiterLimit()
	{
	return iMiterLimit;
	}

// --------------------------------------------------------------------------
//  TGfxStroke::TGfxStroke( TFloatFixPt aStrokeWidth )
// ---------------------------------------------------------------------------
 TGfxStroke::TGfxStroke( TFloatFixPt aStrokeWidth )
    {
    SetVars( aStrokeWidth, EGfxJoinMiter, EGfxCapButt, TFloatFixPt( 4 ) );
    }

// --------------------------------------------------------------------------
//  TGfxStroke::TGfxStroke( TFloatFixPt aStrokeWidth,
// ---------------------------------------------------------------------------
 TGfxStroke::TGfxStroke( TFloatFixPt aStrokeWidth,
                                 TGfxJoinType aJoin,
                                 TGfxCapType aCap,
                                 TFloatFixPt& aMiterLimit )
    {
    SetVars( aStrokeWidth, aJoin, aCap, aMiterLimit );
    }

// --------------------------------------------------------------------------
// void TGfxStroke::SetVars( TFloatFixPt aStrokeWidth,
// ---------------------------------------------------------------------------
void TGfxStroke::SetVars( TFloatFixPt aStrokeWidth,
                          TGfxJoinType aJoin,
                          TGfxCapType aCap,
                          const TFloatFixPt& aMiterLimit )
    {
    iStrokeWidth = aStrokeWidth;
    iJoin = aJoin;
    iCap = aCap;
    iMiterLimit = aMiterLimit;
    }


