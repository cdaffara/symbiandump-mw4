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


#include "GfxColor.h"

// --------------------------------------------------------------------------
//  TGfxColor::TGfxColor( TUint32 aValue ) 
// ---------------------------------------------------------------------------
 TGfxColor::TGfxColor( TUint32 aValue )
    {
    iColor = aValue;
    }

// --------------------------------------------------------------------------
//  TGfxColor::TGfxColor( TInt aRed, TInt aGreen, TInt aBlue )
// ---------------------------------------------------------------------------
 TGfxColor::TGfxColor( TInt aRed, TInt aGreen, TInt aBlue )
    {
    iColor = (aRed<<16)|(aGreen<<8)|aBlue;
    }

// --------------------------------------------------------------------------
//  TUint32 TGfxColor::FlatColor()
// ---------------------------------------------------------------------------
 TUint32 TGfxColor::GetColor()
   {
   return iColor;
   }
    
 TUint32 TGfxColor::GetARGB()
    {
    return iColor;
    }

 TUint32 TGfxColor::GetABGR()
    {
    return   (iColor&0xFF000000) |
    		((iColor&0x00FF0000) >> 16) |
    		 (iColor&0x0000FF00) |
    		((iColor&0x000000FF) << 16);
    }



// ==========================================================================
// BGR888 -> RGB565  - needed for future consideration
// ==========================================================================
 // --------------------------------------------------------------------------
 //  TUint16 TGfxColor::ColorRgb565()
 // ---------------------------------------------------------------------------
  TUint16 TGfxColor::ColorRgb565()
    {

    const TInt32 KRmask = 0xf800;
    const TInt32 KGmask = 0x07e0;
    const TInt32 KBmask = 0x001f;

    TUint32 r, g, b;
    TUint32 val = iColor;
    b = val << 8 & KRmask;
    g = val >> 5 & KGmask;
    r = val >> 19 & KBmask;
    return ( TUint16 ) ( r | g | b );
    }


// --------------------------------------------------------------------------
//  TUint16 TGfxColor::ColorRgb444()
// ---------------------------------------------------------------------------
 TUint16 TGfxColor::ColorRgb444()
    {
    const TInt32 KRmask = 0x0f00;
    const TInt32 KGmask = 0x00f0;
    const TInt32 KBmask = 0x000f;

    TUint32 r, g, b;
    TUint32 val = iColor;
    b = val << 4 & KRmask;
    g = val >> 8 & KGmask;
    r = val >> 20 & KBmask;
    return ( TUint16 ) ( r | g | b );
    }

// --------------------------------------------------------------------------
//  TUint16 TGfxColor::ColorRgb()
// ---------------------------------------------------------------------------
 TUint16 TGfxColor::ColorRgb()
    {
    return ColorRgb444();         // For EColor4K pixel format mode.

    }


// --------------------------------------------------------------------------
//  void TGfxColor::SetFill(VGPaint aFillPaint)
// ---------------------------------------------------------------------------
 void TGfxColor::SetFill(VGPaint aFillPaint, TGfxRectangle2D& aBBox, TFloatFixPt aOpacity,void* aPseudo)
	{
	TGfxRectangle2D lBox = aBBox;
	CVGRenderer *iVgRenderer=(CVGRenderer*) aPseudo;
	if ( aFillPaint != VG_INVALID_HANDLE )
		{
		iVgRenderer->vgSetParameteri( aFillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR );//pseudo
		TUint32 opacity = (TInt)(aOpacity * TFloatFixPt(255.0f));
		iVgRenderer->vgSetColor(  aFillPaint, iColor << 8 | opacity);//pseudo
		iVgRenderer->vgSetPaint(  aFillPaint, VG_FILL_PATH );//pseudo
		}
	}

