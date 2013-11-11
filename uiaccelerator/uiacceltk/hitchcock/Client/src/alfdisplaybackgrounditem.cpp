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



#include "alf/alfdisplaybackgrounditem.h"  // Class definition
#include "alf/alfdisplay.h"  


// Constructors
EXPORT_C TAlfDisplayBackgroundItem::TAlfDisplayBackgroundItem()
    :iRect(TRect(0,0,0,0)), 
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CAlfDisplay::EClearNone)
    {   
             
    }

EXPORT_C TAlfDisplayBackgroundItem::TAlfDisplayBackgroundItem( const TRect& aRect ) 
    :iRect(aRect), 
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CAlfDisplay::EClearNone)
    {        
    }

EXPORT_C TAlfDisplayBackgroundItem::TAlfDisplayBackgroundItem( const TRect& aRect,
                           const TAknsItemID& aSkinID )
    :iRect(aRect),
     iSkinID(aSkinID), 
     iColorIndex(0), 
     iColor(KRgbWhite), 
     iClearMode(CAlfDisplay::EClearWithSkinBackground)
    {
    }

EXPORT_C TAlfDisplayBackgroundItem::TAlfDisplayBackgroundItem( const TRect& aRect,
                           const TAknsItemID& aSkinID,
                           TInt aColorIndex )
    :iRect(aRect),
     iSkinID(aSkinID), 
     iColorIndex(aColorIndex), 
     iColor(KRgbWhite), 
     iClearMode(CAlfDisplay::EClearWithColor)
    {
    }

EXPORT_C TAlfDisplayBackgroundItem::TAlfDisplayBackgroundItem( const TRect& aRect,
                           const TRgb& aColor )
    :iRect(aRect),
     iSkinID(KAknsIIDNone), 
     iColorIndex(0), 
     iColor(aColor), 
     iClearMode(CAlfDisplay::EClearWithColor)
    {        
    }

EXPORT_C void TAlfDisplayBackgroundItem::SetRect( const TRect& aRect )
    {
    iRect = aRect;   
    }

EXPORT_C void TAlfDisplayBackgroundItem::SetColor( const TRgb& aColor )
    {
    iColor = aColor;    
    iClearMode = CAlfDisplay::EClearWithColor;
    }
    
EXPORT_C void TAlfDisplayBackgroundItem::SetColor( const TAknsItemID& aSkinID, TInt aColorIndex )
    {
    iSkinID = aSkinID;
    iColorIndex = aColorIndex;      
    iClearMode = CAlfDisplay::EClearWithColor;
    }
    
EXPORT_C void TAlfDisplayBackgroundItem::SetSkinBackground( const TAknsItemID& aSkinID )
    {
    iSkinID = aSkinID;
    iClearMode = CAlfDisplay::EClearWithSkinBackground;    
    }


