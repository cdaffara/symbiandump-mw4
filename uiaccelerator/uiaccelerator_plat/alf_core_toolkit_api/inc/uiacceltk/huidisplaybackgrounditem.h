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



#ifndef __HUIDISPLAYBACKGROUNDITEM_H__
#define __HUIDISPLAYBACKGROUNDITEM_H__

#include <e32base.h>
#include <AknsUtils.h>

/**
 * This class specifies an item that is drawn in the display background.
 * Item can specify background as either:
 *
 *  "None"  - Nothing is drawn to given display area. Can be used to minimize
 *            updates to display background areas that e.g. are not visible 
 *            and thus maximize framerate.
 *
 *  "Color" - Given display area is filled with given color. Can be defined as 
 *            skinnable color or normal RGB color..
 *            
 *  "Skin background" - Given display area is filled with given skin background
 *            graphics.
 * 
 */
NONSHARABLE_CLASS( THuiDisplayBackgroundItem )
    {
public:   
   /** 
    * Constructor. Default constructor. 
    */
    IMPORT_C THuiDisplayBackgroundItem();

   /**
    * Constructor. Creates backgroun item which area is not cleared. 
    * @param aRect Item area in display coordinates (pixels).
    */
    IMPORT_C THuiDisplayBackgroundItem( const TRect& aRect );
    
   /**
    * Constructor. Creates backgroun item which area is cleared
    * using the given skin background graphics.
    * @param aRect Item area in display coordinates (pixels).
    * @param aSkinID Skin background that is used to clear the item area.
    *                Possible values are e.g.
    *                KAknsIIDQsnBgScreen
    *                KAknsIIDQsnBgScreenIdle
    *                KAknsIIDQsnBgScreenMp
    *                KAknsIIDQsnBgAreaMain
    *                KAknsIIDQsnBgAreaMainAppsGrid 
    *                KAknsIIDQsnBgAreaMainHigh 
    *                KAknsIIDQsnBgAreaMainFlat 
    *                KAknsIIDQsnBgAreaMainSmall
    *                KAknsIIDWallpaper
    *                KAknsIIDQsnBgAreaMainIdle 
    *                KAknsIIDQsnBgAreaStatus 
    *                KAknsIIDQsnBgAreaStatusIdle 
    *                KAknsIIDQsnBgAreaStaconRt 
    *                KAknsIIDQsnBgAreaStaconLt 
    *                KAknsIIDQsnBgAreaStaconRb 
    *                KAknsIIDQsnBgAreaStaconLb 
    *                KAknsIIDQsnBgAreaStaconRtIdle 
    *                KAknsIIDQsnBgAreaStaconLtIdle 
    *                KAknsIIDQsnBgAreaStaconRbIdle 
    *                KAknsIIDQsnBgAreaStaconLbIdle 
    *                KAknsIIDQsnBgAreaControl 
    *                KAknsIIDQsnBgAreaControlPopup 
    *                KAknsIIDQsnBgAreaControlMp 
    *                KAknsIIDQsnBgAreaControlMp 
    *                KAknsIIDQsnBgNavipaneSolid 
    *                KAknsIIDQsnBgNavipaneSolidIdle 
    *
    */
    IMPORT_C THuiDisplayBackgroundItem( const TRect& aRect,
                               const TAknsItemID& aSkinID );

   /**
    * Constructor. Creates backgroun item which area is cleared
    * using the given skin color. 
    * @param aRect Item area in display coordinates (pixels).
    * @param aSkinID Skin color group that is used to clear the item area.
    *                Possible values are e.g.                       
    *                KAknsIIDQsnIconColors
    *                KAknsIIDQsnTextColors
    *                KAknsIIDQsnLineColors
    *                KAknsIIDQsnOtherColors
    *                KAknsIIDQsnHighlightColors
    *                etc.
    * @param aColorIndex Skin color index.
    */
    IMPORT_C THuiDisplayBackgroundItem( const TRect& aRect,
                               const TAknsItemID& aSkinID, 
                               TInt aColorIndex );

   /**
    * Constructor. Creates backgroun item which area is cleared
    * using the given color. 
    * @param aRect Item area in display coordinates (pixels).
    * @param aColor Color that is used to clear the item area.
    */
    IMPORT_C THuiDisplayBackgroundItem( const TRect& aRect,
                               const TRgb& aColor );
    
   /**
    * Sets area for the item.
    * @param aRect Item area in display coordinates (pixels).
    */
    IMPORT_C void SetRect( const TRect& aRect );            
    
   /**
    * Sets color that is used clear the item area. This overrides
    * old color or the skin background if it has been previously given.
    * @param Item color.
    */
    IMPORT_C void SetColor( const TRgb& aColor );

   /**
    * Sets skinned color that is used clear the item area. This overrides
    * old color or skin background if it has been previously given.
    * @param aSkinID Skin color group that is used to clear the item area.
    *                Possible values are e.g.                       
    *                KAknsIIDQsnIconColors
    *                KAknsIIDQsnTextColors
    *                KAknsIIDQsnLineColors
    *                KAknsIIDQsnOtherColors
    *                KAknsIIDQsnHighlightColors
    *                etc.
    * @param aColorIndex Color index.
    */
    IMPORT_C void SetColor( const TAknsItemID& aSkinID, TInt aColorIndex );
    
   /**
    * Sets skin backgroud that is used clear the item area. This overrides
    * old color or skin background if it has been previously given.
    * 
    * @param aSkinID Skin background that is used to clear the item area.
    *                Possible values are e.g.
    *                KAknsIIDQsnBgScreen
    *                KAknsIIDQsnBgScreenIdle
    *                KAknsIIDQsnBgScreenMp
    *                KAknsIIDQsnBgAreaMain
    *                KAknsIIDQsnBgAreaMainAppsGrid 
    *                KAknsIIDQsnBgAreaMainHigh 
    *                KAknsIIDQsnBgAreaMainFlat 
    *                KAknsIIDQsnBgAreaMainSmall
    *                KAknsIIDWallpaper
    *                KAknsIIDQsnBgAreaMainIdle 
    *                KAknsIIDQsnBgAreaStatus 
    *                KAknsIIDQsnBgAreaStatusIdle 
    *                KAknsIIDQsnBgAreaStaconRt 
    *                KAknsIIDQsnBgAreaStaconLt 
    *                KAknsIIDQsnBgAreaStaconRb 
    *                KAknsIIDQsnBgAreaStaconLb 
    *                KAknsIIDQsnBgAreaStaconRtIdle 
    *                KAknsIIDQsnBgAreaStaconLtIdle 
    *                KAknsIIDQsnBgAreaStaconRbIdle 
    *                KAknsIIDQsnBgAreaStaconLbIdle 
    *                KAknsIIDQsnBgAreaControl 
    *                KAknsIIDQsnBgAreaControlPopup 
    *                KAknsIIDQsnBgAreaControlMp 
    *                KAknsIIDQsnBgAreaControlMp 
    *                KAknsIIDQsnBgNavipaneSolid 
    *                KAknsIIDQsnBgNavipaneSolidIdle 
    *
    */
    IMPORT_C void SetSkinBackground( const TAknsItemID& aSkinID );

   /**
    * Returns the proper display clear mode for drawing the item. 
    * Possible values are:
    * 
    * CHuiDisplay::EClearNone
    * CHuiDisplay::EClearWithColor
    * CHuiDisplay::EClearWithSkinBackground
    *
    * @return Display clear mode that should be used for drawing the item.
    */
    IMPORT_C TInt ClearMode() const;
    
   /**
    * Gets area for the item.
    * @return Item area in display coordinates (pixels).
    */
    IMPORT_C TRect Rect() const;
    
   /**
    * Gets color that is used clear the item area. Color is valid only if item
    * has been set to "Color" using either skin color group or RBG color.
    * @return Item color.
    */
    IMPORT_C TRgb Color() const;
    
   /**
    * Gets backgroud that is used clear the item area. 
    * @return Item skin id. Can be either skin background.
    */
    IMPORT_C TAknsItemID SkinBackground() const;
    
private:    
    /** Item draw area, specified in display pixels */
    TRect iRect;       

    /** Item skin id. Can be a skin background or skin color */
    TAknsItemID iSkinID;

    /** Item skin color index */
    TInt iColorIndex;

    /** Item color (used if draw mode is clear with color) */
    TRgb iColor;    
    
    /** Item draw mode */
    TInt iClearMode;
    
    /** Reserved for future use */
    TInt iSpare1;
    TInt iSpare2;
    TInt iSpare3;
    };

#endif