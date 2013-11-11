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
* Description:   
*
*/



#ifndef __HUICANVASBACKGROUND_H__
#define __HUICANVASBACKGROUND_H__


#include <e32base.h>
#include <uiacceltk/HuiDisplay.h>

class CHuiCanvasGc;
class CHuiCanvasVisual;
    
	
class CHuiCanvasBackground : public CBase
    {
public:

	/**
	 * Constructor.
	 */
	CHuiCanvasBackground();


	/**
	 * Destructor.
	 */ 
	~CHuiCanvasBackground();

	/**
	 * Constructor.
	 */
	static CHuiCanvasBackground* NewL();

	/**
	 * Constructor.
	 */
    static CHuiCanvasBackground* NewLC();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
    /**
     * Sets visual which is using this background.
     */
    void SetVisual(CHuiCanvasVisual* aVisual);    
    
    /**
     * Drawing.
     */
    void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect);

    /**
     * Sets the flag that tells the canvas to clear the background before
     * doing a refresh. 
     *
     * @param aClearBackground  Background clearing mode.
     * @see SetBackgroundColor()
     */
    void SetClearBackground(CHuiDisplay::TClearMode aClearBackground);

    /**
     * Gets the flag that tells the canvas to clear the background before
     * doing a refresh. 
     *
     * @return  Background clearing mode.
     */
    TInt ClearBackground();

    /**
     * Sets the background color of the display, if background clearing has
     * been enabled.
     *
     * @param aBackgroundColor  The color to set the background to.
     * @see SetClearBackgroundL()
     */
    void SetBackgroundColor(const TRgb& aBackgroundColor);
    
    /**
    * Sets the items that specify how to clear display before display refresh.
    * Items may have overlapping rectangles, drawing order is same as item order
    * in the parameter array. But having lots of overlapping drawing 
    * causes negative performance impact.
    * 
    * This method is alternative to SetClearBackgroundL and SetBackgroundColor
    * methods.
    * 
    * @see SetBackgroundColor()
    * @see SetClearBackgroundL()
    * @param aItems Array of items that specify how to clear the background.
    */
    void SetBackgroundItems(const RArray<THuiDisplayBackgroundItem>& aItems);
    
    /**
     * Gets the number of THuiDisplayBackground items set, for the canvas visual
     *
     * @return  Number of background items
     */
    TInt BackgroundItemsCount();

    /**
    * Gets the items that specify how to clear display before display refresh.
    * @param aItems Returned items are written into the array.
    */
    void GetBackgroundItems(RArray<THuiDisplayBackgroundItem>& aItems);

    /**
     * Gets the background color of the display, if background clearing has
     * been enabled.
     *
     * @return The color to set the background to.
     */
    TRgb BackgroundColor() const;
    
private:

    void ClearWithColor(CHuiGc& aGc, const TRect& aDisplayRect);

    void ClearWithSkinBackground(const TRect& aDisplayRect);

    void ClearWithBackgroundItems(const TRect& aDisplayRect);
    
private:
    
    /** Canavas visual which uses this background */
    CHuiCanvasVisual* iCanvasVisual;

    /** Determines if the background should be cleared before drawing. */
    TInt iClearBackground;

    /** Background color for the display, if clearing enabled. */
    TRgb iBackgroundColor;

    /** Array of background items */
    RArray<THuiDisplayBackgroundItem> iBackgroundItems;
          
    };

#endif        
    
