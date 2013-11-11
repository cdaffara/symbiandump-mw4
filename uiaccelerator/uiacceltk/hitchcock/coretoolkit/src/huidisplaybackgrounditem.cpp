/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines item in display background.
*
*/



#include "uiacceltk/huidisplaybackgrounditem.h"  // Class definition
#include "uiacceltk/HuiDisplay.h"  



// Constructors
EXPORT_C THuiDisplayBackgroundItem::THuiDisplayBackgroundItem()
    :iRect(TRect(0,0,0,0)), 
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CHuiDisplay::EClearNone)
    {                
    }

EXPORT_C THuiDisplayBackgroundItem::THuiDisplayBackgroundItem( const TRect& aRect ) 
    :iRect(aRect), 
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CHuiDisplay::EClearNone)
    {        
    }

EXPORT_C THuiDisplayBackgroundItem::THuiDisplayBackgroundItem( const TRect& aRect,
                           const TAknsItemID& aSkinID )
    :iRect(aRect),
     iSkinID(aSkinID), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CHuiDisplay::EClearWithSkinBackground)
    {
    }

EXPORT_C THuiDisplayBackgroundItem::THuiDisplayBackgroundItem( const TRect& aRect,
                           const TAknsItemID& aSkinID,
                           TInt aColorIndex )
    :iRect(aRect),
     iSkinID(aSkinID), 
     iColorIndex(aColorIndex), 
     iColor(KRgbWhite), 
     iClearMode(CHuiDisplay::EClearWithColor)
    {
    }

EXPORT_C THuiDisplayBackgroundItem::THuiDisplayBackgroundItem( const TRect& aRect,
                           const TRgb& aColor )
    :iRect(aRect),
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(aColor), 
     iClearMode(CHuiDisplay::EClearWithColor)
    {        
    }

EXPORT_C void THuiDisplayBackgroundItem::SetRect( const TRect& aRect )
    {
    iRect = aRect;   
    }

EXPORT_C void THuiDisplayBackgroundItem::SetColor( const TRgb& aColor )
    {
    iColor = aColor;    
    iClearMode = CHuiDisplay::EClearWithColor;
    }
    
EXPORT_C void THuiDisplayBackgroundItem::SetColor( const TAknsItemID& aSkinID, TInt aColorIndex )
    {
    iSkinID = aSkinID;
    iColorIndex = aColorIndex;      
    iClearMode = CHuiDisplay::EClearWithColor;
    }
    
EXPORT_C void THuiDisplayBackgroundItem::SetSkinBackground( const TAknsItemID& aSkinID )
    {
    iSkinID = aSkinID;
    iClearMode = CHuiDisplay::EClearWithSkinBackground;    
    }

EXPORT_C TInt THuiDisplayBackgroundItem::ClearMode() const
    {
    return iClearMode;    
    }

EXPORT_C TRect THuiDisplayBackgroundItem::Rect() const
    {
    return iRect;    
    }

EXPORT_C TRgb THuiDisplayBackgroundItem::Color() const
    {
    // If skin color has been defined, return it. Otherwise use rgb color.
    if (iSkinID != KAknsIIDNone)
        {
        TRgb skinnedColor = KRgbBlack;
        CHuiStatic::GetCachedColor(skinnedColor, iSkinID, iColorIndex);            
        return skinnedColor;
        }
    else
        {
        return iColor;            
        }    
    }

EXPORT_C TAknsItemID THuiDisplayBackgroundItem::SkinBackground() const
    {
    return iSkinID;    
    }

