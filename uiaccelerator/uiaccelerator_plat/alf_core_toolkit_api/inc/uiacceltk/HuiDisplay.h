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
* Description:   Defines CHuiDisplay, a drawing buffer into which a the toolkit 
*                contents (visuals) are drawn.
*
*/



#ifndef __HUIDISPLAY_H__
#define __HUIDISPLAY_H__


#include <e32base.h>
#include <coemain.h>
#include <gdi.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiRoster.h>
#include <uiacceltk/huidisplaybackgrounditem.h>


/* Forward declarations */
class MHuiRenderSurface;
class CHuiEnv;
class CHuiVisual;
class CHuiControl;
class CHuiControlGroup;
class CHuiTransformation;
class CHuiRosterImpl;
class CHuiDisplay;
class CCoeControl;
class RDrawableWindow;

/* Screen buffer uids */
const TUid KHuiUidBackBufferAll = 			{0x2000e59d}; // Not used
const TUid KHuiUidBackBufferScreen0 = 		{0x2000e59e};
const TUid KHuiUidBackBufferScreen1 = 		{0x2000e59f};
const TUid KHuiUidBackBufferTvOutNormal = 	{0x2000e5a0};
const TUid KHuiUidBackBufferTvOutWide = 	{0x2000e5a1};

/**
*  The intended usage of the display. The display may use this info to 
*  prioritize the HW usage and route content.
*  Vendor specific meanings may be used at the end of the enum space.
*/
enum THuiDisplayUsageHint
    {
    EHuiDisplayUsageGeneric        = 0x00000001,
    EHuiDisplayUsage3D             = 0x00000002,
    EHuiDisplayUsageVideoPlayback  = 0x00000004,
    EHuiDisplayUsageVideoCapture   = 0x00000008,
    EHuiDisplayUsageViewfinder     = 0x00000010,
    EHuiDisplayUsageStillImage     = 0x00000011,
    EHuiDisplayUsageVendorSpecific = 0x80000000
    };


/**
 * Roster observers are notified before and after draw.
 */
class MHuiRosterObserver
    {
public:

    /**
     * Called to notify that roster draw is about to start. 
     */
    virtual void NotifyRosterDrawStart(CHuiDisplay& aDisplay) = 0;

    /**
     * Called to notify that roster draw is finished.    
     */
    virtual void NotifyRosterDrawEnd(CHuiDisplay& aDisplay) = 0;

    };

/**
 * Display refresh observers are notified before a display refresh begins.
 */
class MHuiDisplayRefreshObserver
    {
public:

    /**
     * Called to notify the observer that a display refresh is about to begin.
     * The observer should prepare its state for the display.
     */
    virtual void NotifyDisplayRefreshStarted(CHuiDisplay& aDisplay) = 0;

    };


/**
 * Display deletion observer is notified when the display is about to be
 * destroyed.
 */
class MHuiDisplayDeletionObserver
    {
public:

    /**
     * Called to notify the observer that a display is about to be destroyed.
     */
    virtual void NotifyDisplayDeletion(CHuiDisplay& aDisplay) = 0;

    };


/**
 * Display size observer is notified when the display is resized.
  */
class MHuiDisplayVisibleAreaObserver
    {
public:

    /**
     * Called to notify the observer that a display has been resized.
     *
     * @param aDisplay  Display that sends the notification.
     */
    virtual void NotifyDisplayVisibleAreaChanged(CHuiDisplay& aDisplay) = 0;

    };

class MHuiScreenBufferObserver
    {
public:
       
        enum THuiScreenBufferEvent
            {
			ENone,
			EBufferComplete = ENone,
            ECreated,				// Buffer created 
            EDeleted, 	   			// Buffer deleted	
            ERestored, 				// Not yet implemented
            EReleased, 				// Not yet implemented
            EBufferDrawComplete		// Buffer has been drawn to the internal window
            };
    
    /**
     * This is called when all the content have been draw to the buffer.
     * If EFalse is returned, the buffer will be locked until the observer unlocks it.
     * If ETrue is returned, the buffer is unlocked automatically, immediately after this method returns.
     *
     * @param aBufferUid Unique id of the buffer.
     * @param aDisplayRect Screen buffer rect.
     * @param aDirtyRect Dirty rect for the buffer.
     * @return Return True, if buffer can be freed automatically after this call.
     */
    virtual TBool ScreenBufferComplete(TUid aBufferUid, TRect& aDisplayRect, TRect& aDirtyRect) = 0;
    
    /**
     * Alf calls this when e.g. the back buffer is created or deleted.
     *
     * @param aId The unique id of the buffer.
     * @param aEvent @see MAlfScreenBufferObserver::TAlfScreenBufferEvent
     */
    virtual void HandleScreenBufferEvent(TUid aBufferUid, TInt aEvent) = 0;
    };


/**
 * This interface is to be used when drawing the buffer to the window etc.      
 */
class MHuiBufferDrawer
    {
public:
    // Clean all resources, usually delete this
    virtual void ReleaseDrawer() = 0;    
    };

/**
 * This interface is to be used when modifying the buffer (sent back to server).      
 * Could be attached to special content visual.
 */
class MHuiGc
    {
public:
    // Clean all resources, usually delete this
    virtual void ReleaseGc() = 0;
    };

/*
 * Internal class. Not for public API.
 */
class MHuiFbsBitmapBufferGc: public MHuiGc
    {
public:
    virtual CFbsBitmap* Bitmap() = 0;
    };


/**
 * CHuiDisplays are the drawing surfaces for the applications using the
 * toolkit.
 *
 * To be able to display any content with the toolkit, application must
 * create an instance of CHuiDisplay. For that @see CHuiDisplayCoeControl
 * class and @see CHuiEnv::NewDisplayL().
 *
 * CHuiDisplay is drawing buffer into which a roster is drawn. The drawing is
 * done on the rendering surface associated with the display. The drawing
 * surface itself can be an on-screen surface (attached to an RWindow) or an
 * off-screen surface (buffer in memory).
 *
 * The display can be set to draw only onto a portion of the surface, so that
 * multiple displays can share a single surface. The portion of the surface
 * used by the display is set with SetVisibleArea().
 *
 * Displays remain owned by an environment (a CHuiEnv instance).
 *
 */
NONSHARABLE_CLASS(CHuiDisplay) : public CBase,
                                 public MCoeMessageMonitorObserver
    {
public:

    /* Types. */

    /** Background clear modes. */
    enum TClearMode
        {
        /** Do not clear. This is more like an optimization flag.
         * Using this with OpenGL ES renderer results
         * to undefined (Trashed) background - so be sure to display content that
         * fills the entire screen. With the BITGDI renderer the
         * behavior is similar but the display are will show the previously
         * rendered avkon content (if any). */
        EClearNone,

        /** Clear with a solid color. */
        EClearWithColor,

        /** Clear with the skin background texture. */
        EClearWithSkinBackground
        };

    /** Display types. */

    enum TDisplayType
        {
        /** @deprecated: Primary LCD display. */
        EDisplayLcd0 = 0x0,              
 
        /** @deprecated: Secondary LCD display, e.g. the cover display. */
        EDisplayLcd1 = 0x1,
        
        /** @deprecated: TV out */
        EDisplayTvOut = 0x2,

        /** @deprecated: TV out widescreen */
        EDisplayTvOutWide = 0x3,

        /** Physical screen */
        EDisplayNormal = 0x10,     
 
        /** Virtual display */
        EDisplayVirtual = 0x20,

        /** @deprecated: Off screen buffer */
        EDisplayOffScreenBuffer = 0x40
        };
              
    /** Texture bitmap format info */
    
    class CTextureBitmapFormat: public CBase
		{
	public:
		CTextureBitmapFormat(TDisplayMode aImage, TDisplayMode aMask)
			: iImage(aImage), iMask(aMask)
			{}
	private:
		CTextureBitmapFormat(const CTextureBitmapFormat& p); 		
	public:		
		TDisplayMode iImage;
		TDisplayMode iMask;	
		};	    
		
    
    /* RnD: visual outline drawing */
    enum TDrawVisualOutline
        {
        EDrawVisualOutlineNone,
        EDrawVisualOutlineAllVisuals,      //draw outline for all visuals
        EDrawVisualOutlineSelectedVisuals  //draw outline for selected visuals and their child visuals
        };
    	

    /* Constructors and destructor. */

    /**
     * Constructors.
     * This constructor should never be called explicitly by application developers,
     * new displays should be created through the environment instead.
     *
     * @param aEnv  The environment that manages this display.
     * @param aNativeControl  Native control where the display will be used (or <code>NULL</code>).
     * @param aDisplayType  Display type.
     * @param aScreenBufferUid  Screen buffer uid.
     * @see CHuiEnv::NewDisplayL().
     */     
    CHuiDisplay(CHuiEnv& aEnv, CCoeControl* aNativeControl, TInt aDisplayType, TUid aScreenBufferUid);
    CHuiDisplay(CHuiEnv& aEnv, RWindow* aNativeWindow, TInt aDisplayType, TUid aScreenBufferUid);


    /**
    * Destructor 
    */
    IMPORT_C ~CHuiDisplay();

    /**
     * Second-phase constructor. Creates a window and a rendering surface.
     * Not exported because displays must be created using CHuiEnv.
     *
     * @param aRect  Frame rectangle for container.
     * @param aSharedRoster  Existing roster to use.
     */
    void ConstructL(const TRect& aRect, CHuiRoster* aSharedRoster);

    /* Methods. */

    /** @beginAPI */

    /**
     * Returns the environment of the display.
     */
    IMPORT_C CHuiEnv& Env();

    /**
     * Activates or deactivates the display. Active displays are refreshed,
     * deactivated ones are not. Display activation and deactivation is
     * normally carried out automatically.
     *
     * @param aActivate  <code>ETrue</code> to activate.
     */
    IMPORT_C void Activate(TBool aActivate);

    /**
     * Determines if the display is active.
     *
     * @return  <code>ETrue</code>, if the display is active.
     */
    IMPORT_C TBool Active() const;

    /**
     * Sets a new size for the display. The display's rendering surface will
     * also be resized.
     *
     * @param aSize  New size for the display.
     */
    IMPORT_C void SetSizeL(const TSize& aSize);

    /**
     * Returns the size of the display. The rendering surface of the display
     * has this same size. This size may be greater than the visible display
     * area, for example when multiple displays are drawn onto the same surface.
     *
     * @return  Size of the display and its rendering surface.
     * @see VisibleArea()
     * @see SetVisibleArea()
     */
    IMPORT_C TSize Size() const;

    /**
     * Sets the visible area of the display. The visible area is the area on
     * the display's rendering surface where the display is drawn. Normally
     * a display covers the entire rendering surface, but in the situation
     * where multiple displays are drawn on the same surface, this would be
     * used to define the visible area of each display.
     * If there is no graphics context, this method has no effect.
     *
     * @param aArea  Visible area for the display.
     * @see Size()
     */
    IMPORT_C void SetVisibleArea(const TRect& aArea);

    /**
     * Returns the visible area of the display.
     *
     * @return  Visible area of the display. Affected by display orientation
     *          so that the returned rectangle uses the oriented coordinate
     *          space.
     *
     * @todo  Test that the coordinates are calculated correctly.
     * @see Size()
     */
    IMPORT_C TRect VisibleArea() const;

    /**
     * Sets display orientation.
     *
     * @param aOrientation  Display orientation.
     */
    IMPORT_C void SetOrientation(CHuiGc::TOrientation aOrientation);

    /**
     * Determines the orientation of the display, such as horizontal
     * or vertical.
     *
     * @return  Orientation.
     * @see CHuiGc::TOrientation
     */
    IMPORT_C CHuiGc::TOrientation Orientation() const;

    /**
     * Sets the flag that enables the use of depth testing.
     *
     * @param aUseDepth  <code>ETrue</code> to enable depth.
     */
    IMPORT_C void SetUseDepth(TBool aUseDepth);

    /**
     * Sets the flag that tells the display to clear the background before
     * doing a refresh. Creates new objects required to be able to clear
     * the background with the defined mode.
     *
     * @param aClearBackground  Background clearing mode.
     * @see SetBackgroundColor()
     */
    IMPORT_C void SetClearBackgroundL(TClearMode aClearBackground);

    /**
     * Sets the background color of the display, if background clearing has
     * been enabled.
     *
     * @param aBackgroundColor  The color to set the background to.
     * @see SetClearBackgroundL()
     */
    IMPORT_C void SetBackgroundColor(const TRgb& aBackgroundColor);

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
     * 
     */
    IMPORT_C void SetBackgroundItemsL(const RArray<THuiDisplayBackgroundItem>& aItems);
    
    /**
     * Makes a screen capture of the display's current content.
     * This method uses the screen capture method of the current
     * graphics context. This may return differing data depending on the
     * chosen graphics context, and screen capture may not be supported.
     * The returned pointer may be allocated from inside the graphics context,
     * it may need to be deleted afterwards.
     *
     * @param aSize  Size of the captured image.
     *
     * @return  Captured pixel data, in the form returned by the HuiGc.
     * @see HuiGles10Gc::CaptureLC()
     */
    IMPORT_C TUint8* CaptureLC(TSize& aSize) const;

    /**
     * Returns the transformation for the entire display.
     */
    IMPORT_C CHuiTransformation& Transformation() const;

    /**
     * Returns the roster of the display.
     */
    IMPORT_C CHuiRoster& Roster();
    
    /**
     * Returns the const roster of the display.
     */    
    IMPORT_C const CHuiRoster& Roster() const;

    /**
     * Shows or hides the display. The application should call this if
     * the display goes to background.
     *
     * @param aShow ETrue if display is shown. EFalse if display is hidden.
     */
    IMPORT_C void Show(TBool aShow);

    /**
     * Flag the entire display as dirty.
     */
    IMPORT_C void SetDirty();
    /**
     * Sets the rendering quality of the display.
     */
    IMPORT_C void SetQuality(THuiQuality aRenderingQuality);

    /**
     * Determines the rendering quality of the display.
     */
    IMPORT_C THuiQuality Quality() const;
    
    /**
     * Tells display the intended usage of the display. Display may use this
     * information to optimize the drawing.
     *
     * @param aUsage 
     *
     *   Possible values for parameter are:
     *    EHuiDisplayUsageGeneric 
     *    EHuiDisplayUsage3D      
     *    EHuiDisplayUsageVideoPlayback
     *    EHuiDisplayUsageVideoCapture 
     *    EHuiDisplayUsageViewfinder   
     *    EHuiDisplayUsageStillImage   
     *    EHuiDisplayUsageVendorSpecific + n
     *  
     *  Supported values depend on display type
     *  and underlying HW.
     *
     * 
     */    
    IMPORT_C void SetUsageL(TUint aUsageHint);

    /**
     * Temporarily release the rendering surfaces used by this display. You
     * can call this method to free up resources related to display for example 
     * when the display goes to background (ie. is not visible).
     * @see RestoreL() 
     */
    IMPORT_C void Release();
    
    /**
     * Recreate any released resources of this display. You
     * can call this method to recreate resources related to display for example 
     * when the display comes back to foreground (ie. becomes visible).
     * @see Release() 
     */
    IMPORT_C void RestoreL();
    
    /**
     * Lock or unlock the screen buffer. If the buffer is locked the system cannot draw to it. 
     *
     * @param TBool Lock or unlock the screen buffer
     */
	IMPORT_C void SetScreenBufferLock(TBool aLock);
    
    /**
     * Returns locking state of the screen buffer. 
     *
     * @return TBool Locking state of the screen buffer
     */
	IMPORT_C TBool IsScreenBufferLocked() const;
    
    /**
     * Returns an instance of an interface for drawing the buffer e.g. to a window. 
     *
     * @param aInterfaceUid Unique id of the interface.
     */
    IMPORT_C MHuiBufferDrawer* GetDrawingInterface(const TUid& aInterfaceUid);    
    
    /**
     * Returns an instance of an interface for drawing to the buffer. 
     *
     * @param aInterfaceUid Unique id of the interface.
     */
    IMPORT_C MHuiGc* GetGraphicsContext(const TUid& aInterfaceUid);  
    
    /**
     * Add an observer for a buffer. There can be only one observer at a time, so this overrides 
     * the previous one.
     *
     * @param aObserver Pointer to the observer.
     */
    IMPORT_C void AddScreenBufferObserverL(MHuiScreenBufferObserver* aObserver);  
    
    /**
     * Remove observer from the buffer.
     */
    IMPORT_C void RemoveScreenBufferObserver();
    
    /**
     * Return the screen buffer observer.
     *
     * @return Screen buffer observer
     */
	IMPORT_C MHuiScreenBufferObserver* ScreenBufferObserver();
	
    /**
     * Draw the screen buffer to the internal window 
     */
	IMPORT_C void DrawScreenBuffer(); 
	
    /**
     * Return the screen buffer uid
     *
     * @return Screen buffer uid
     */
	IMPORT_C TUid ScreenBufferUid(); 
	
    /**
     * Return the display type.
     *
     * @return Display type
     */
	IMPORT_C TInt DisplayType(); 
	
	/**
	 * For debugging the dirty regions. Do not call in a release version.
	 * Valid only with BitGDI renderer.
	 * 
	 * @param aShow ETrue shows the dirty regioins
	 *              EFalse hides the dirty regions (default)
	 */
	IMPORT_C void ShowDirtyRegions( TBool aShow = ETrue );
	
	/**
	 * Get a list of preferred texture formats that are optimal for the renderer. The formats are
	 * in descending preference order, if applicable.
	 * 
	 * This method can be used for finding suitable format for textures to be created with 
	 * EHuiTextureFlagAllowDirectBitmapUsage flag.  
	 *
	 * The ownership of array items is given to caller. The application should call ResetAndDestroy()
	 * for the array before it goes out of scope, even if an error is returned.
	 * 
	 * @param aTextureFormats returns a list of preferred formats for this display.
     * @return Error code.  
	 */
	IMPORT_C TInt GetPreferredTextureFormats(RPointerArray<CHuiDisplay::CTextureBitmapFormat>& aTextureFormats);

    /**
     * Sets clipping rect for display visible area. Does not modify actual visible area,
     * but drawing outside the given clipping rect is clipped. Setting new VisibleArea
     * cancels automatically clipping rect that has possibly been set with this method.
     * @param aVisibleAreaClippingRect Clipping rect for visible area. Setting rect to value 
     * TRect(0,0,0,0) disables clipping.
     */
     IMPORT_C void SetVisibleAreaClippingRect(const TRect& aVisibleAreaClippingRect);
     
    /**
     * For debugging purposes. Do not call in a release version - needs a RnD central
     * repository key for activation. See programmers guide for more information.
     * 
     * @param aDrawVisualOutline Holds the flag value for drawing visual outline.
     * @see TDrawVisualOutline
     */
    IMPORT_C void SetDrawVisualOutline( TUint aDrawVisualOutline );
     
    /** @endAPI */

    /**
     * Returns the graphics context of the display.
     */
    CHuiGc* Gc();

    /**
     * Returns the native control associated with the display, if there is one.
     *
     * @return  CCoeControl in which the display is shown, or <code>NULL</code>.
     */
    IMPORT_C CCoeControl* NativeControl();

    /**
     * If there is a native window associated with the display (e.g., the
     * display is using a native window as a rendering surface), returns
     * a pointer to the window.
     *
     * @return  Pointer to native window, or <code>NULL</code>.
     */
    IMPORT_C RDrawableWindow* NativeWindow();

    /**
     * Returns the rendering surface of the display.
     */
    IMPORT_C MHuiRenderSurface& RenderSurface() const;

    /**
     * Determines whether the display contains any dirty regions. Dirty
     * regions are areas of the display that have changed but the changes
     * are not yet visible on the screen.
     *
     * @return  <code>ETrue</code>, if one or more dirty regions have been
     *          defined. Otherwise <code>EFalse</code>.
     */
    TBool IsDirty() const;

    /**
     * Defines a new dirty region that needs to be updated when the next
     * refresh cycle is triggered. Called by visuals when they are modified.
     * The dirty regions are stored within the CHuiDisplay class, and are used
     * to decide which visuals need to be redrawn during the next refresh.
     *
     * @param aDirtyRegion  The area of the display that needs to be redrawn
     *                      in screen coordinates.
     */
    void AddDirtyRegion(const TRect& aDirtyRegion);

    /**
     * Defines a new dirty region that needs to be updated when the next
     * refresh cycle is triggered. Combines given previous and current dirty regions.
     * Current dirty region is returned to the caller, before clipping and combining. 
     * Called by visuals when they are modified. The dirty regions are stored within
     * the CHuiDisplay class, and are used to decide which visuals need to be redrawn
     * during the next refresh.
     *
     * @param aPrevDirtyRegion  The previous dirty area.
     * @param aDirtyRegion  	The area of the display that needs to be redrawn
     *                      	in screen coordinates.
     */
	void CombineAndAddDirtyRegion(const TRect& aPrevDirtyRegion, TRect& aDirtyRegion);

    /**
     * Refresh the display. This is only meaningful if there are dirty
     * regions in the display. All dirty regions are cleared after the
     * refresh is complete.
     *
     * @return  <code>ETrue</code>, if something was done that may lead to
     *          visible changes on the display. <code>EFalse</code>, if there
     *          was nothing to refresh.
     */
    TBool Refresh();

    /**
     * Returns the implementation of the roster.
     */
    CHuiRosterImpl& RosterImpl() const;
    
    /**
     * Clear all change flags of visuals in the display's roster.
     */
    void ClearChanged();
    
    /**
     * Return the display intended usage.
     */
	IMPORT_C TUint Usage() const;

    /**
     * Return the value of the S60 Platform specific unit.
     */
    TReal32 UnitValue() const;

    /**
     * Returns the window transformation for the entire display.
     */
     CHuiTransformation& WindowTransformation() const;

    /**
     * Returns ETrue if this display is for TV-out
     */
     IMPORT_C TBool IsDisplayTypeTvOut() const;

    /**
     * Returns ETrue if this display is able to utilize transformations when calculating dirty areas.
     */
     TBool UseTransformedDirtyRegions() const;
     
    /**
     * Returns EFalse if this display is already set dirty enough so that adding new dirty regions does
     * not make anymore diffenrence.
     */
     TBool IsDirtyScanNeeded() const;

    /**
     * Rnd: returns the draw Visual outline flag value
     *
     * @return Visual outline setting.
     */     
    TUint DrawVisualOutline() const;

    /**
     * Sets texture which is drawn on top of the the frame.
     * Ownership is transferred to CHuiDisplay.
     */     
    IMPORT_C void SetForegroundTexture(CHuiTexture* aTexture);

    /**
     * Gets texture which is drawn on top of the the frame.
     */     
    IMPORT_C CHuiTexture* ForegroundTexture() const;
    
    /**
     * Sets foreground texture / bitmap options.
     * @param aBlendRequired ETrue if texture should be blend, EFalse if copy
	 *        is sufficient.
     * @internal
     */
    IMPORT_C void SetForegroundTextureOptions(TBool aBlendRequired);
    
    /**
     * Sets bitmap which is drawn on top of the frame
     * (when used with @c SetForegroundTexture).
     * 
     * This bitmap is modified before visuals are rendered;
     * dirty areas are cleared. Thus, the bitmap should 
     * have alpha channel.
     * @param aBitmap foreground bitmap.
     */
    IMPORT_C void SetForegroundBitmapL(CFbsBitmap* aBitmap);
    
    /**
     * Retuns foreground bitmap set in @c SetForegroundBitmapL.
     */
    IMPORT_C CFbsBitmap* ForegroundBitmap() const;
    
    /**
     * Copy raw screen data to bitmap.
     */
    IMPORT_C void CopyScreenToBitmapL(CFbsBitmap* aBitmap);
    
    
    /**
     * Does background clearing for native alf application
     */
    void DoBackgroundClear();
    
    void ScanningAlfContent(TBool aScanning);
    void SetAlfContentChanged(TBool aChanged);
    TBool AlfContentChanged();
    IMPORT_C void SetSkinSizeChangePending();
    TBool QueryAndResetSkinSizeChangePendingStatus();
    TRect CurrentDirtyRect();
    
private:

    /**
     * Uploads foreground bitmap to texture.
     */
    void UpdateForegroundTexture(const TRect& aRect);
    
    /**
     * Actual upload foreground bitmap to texture.
     */
    void DoUpdateForegroundTextureL(const TRect& aRect);
    
    /**
     * Draws foreground texture.
     */     
    void DrawForegroundTexture();

    /**
     * Returns ETrue if this displays renderer is BitGdi
     */
     TBool IsRendererBitgdi() const;

     /**
      * Returns ETrue if this displays renderer is OpenGLES or OpenVG (even if those
      * are implemented using SW). Returns EFalse if the render is bitgdi.
      */
     TBool IsRendererHWAccelerated() const;
     
    /**
     * Clips given dirty rect to the visible area.
     */
    void ClipDirtyRect(TRect& aRect);

    void ClipDirtyRect(TRect& aRect, TRect aClippingRect);

    /**
     * Transforms given dirty rect.
     */
    void TransformDirtyRect(TRect& aRect); 

    void ClearWithColor(TRect aRect);
    void ClearWithSkinBackground(TRect aRect);
    void ClearWithBackgroundItems(TRect aRect);
    
    void BitgdiClearWithColor(TRect aRect);
    void BitgdiClearWithSkinBackground(TRect aRect);
    void BitgdiClearWithBackgroundItems(TRect aRect);
    
    void HWAcceleratedClearWithColor(TRect aRect);
    void HWAcceleratedClearWithSkinBackground(TRect aRect);
    void HWAcceleratedClearWithBackgroundItems(TRect aRect);
    
    TRect VisibleAreaClippingRect() const;
    TBool IsVisibleAreaClippingEnabled() const;

public: // from MCoeMessageMonitorObserver
	    
    void MonitorWsMessage(const TWsEvent& aEvent);
	
protected:

    /* Types. */

    /** Array of dirty region rectangles. */
    typedef RArray<TRect> RDirtyRegions;


    /* Methods. */

    /**
     * Add to a specific array.
     */
    static void AddDirtyRegion(const TRect& aDirtyRegion, RDirtyRegions& aRegions, TBool aGrow = ETrue);


private:

    /** The environment that manages this display. */
    CHuiEnv& iEnv;

    /** The display type. */
	TInt iDisplayType;
	
    /** Pointer to the native window of this display (if any). */
    CCoeControl* iNativeControl;

    /** Size of the display. */
    TSize iSize;

    /** Rendering surface of the display. */
    MHuiRenderSurface* iRenderSurface;

    /** Graphics context for drawing. */
    CHuiGc* iGc;

    /** Transformation for the entire display. */
    CHuiTransformation* iTransform;

    /** Control roster of the display. */
    RHuiOwnedPointer<CHuiRoster> iRoster;

    /** Flag that tells whether the display should currently be refreshed. */
    TBool iActive;

    /** Determines if depth testing can be used during rendering. */
    TBool iUseDepth;

    /** Determines if the background should be cleared before drawing. */
    TInt iClearBackground;

    /** Background color for the display, if clearing enabled. */
    TRgb iBackgroundColor;

    /** If true, updates the renderer state before the next frame is drawn.
        Normally it is assumed that it is necessary to set the state only
        once. */
    TBool iUpdateRenderState;


    /** @todo  The dirty tracker should support both frame copying and swapping. */
    /** Is ETrue if the screen was dirtied at all in the last update.*/
    TBool iGotDirtyReports;

    /**
     * Array of all the dirty regions of the display.
     * This is flipped with iDirtyRegions2 every other frame.
     */
    RDirtyRegions iDirtyRegions;

    /**
     * Secondary array of all the dirty regions of the display.
     * This is flipped with iDirtyRegions every other frame.
     */
    RDirtyRegions iDirtyRegions2;

    /** Points to either iDirtyRegions or iDirtyRegions2. */
    RDirtyRegions* iCurrentDirtyRegions;

    /** Points to either iDirtyRegions or iDirtyRegions2. */
    RDirtyRegions* iPreviousDirtyRegions;

    /** Info how display is used */
    TInt iUsageHint;

    /** Determines whether this display is on the foreground in the system or not. */
    TBool iOnForeground;

public:

    /* Public properties. */

    /** Observers notified before a refresh begins. */
    RHuiObserverArray<MHuiDisplayRefreshObserver> iRefreshObservers;

    /** Observers notified before display deleted. */
    RHuiObserverArray<MHuiDisplayDeletionObserver> iDeletionObservers;

    /** Observers notified after display visible area changed. */
    RHuiObserverArray<MHuiDisplayVisibleAreaObserver> iVisibleAreaObservers;

    /** Observers notified before a roster draw begins and ends. */
    RHuiObserverArray<MHuiRosterObserver> iRosterObservers;
    
private:

	/** Screen buffer uid */
	TUid iScreenBufferUid;
	
	/** Screen buffer lock */
	TBool iIsScreenBufferLocked;
	
	/** Screen buffer observer */
	MHuiScreenBufferObserver* iScreenBufferObserver;
	
    /** Transformation for the entire display. */
    CHuiTransformation* iWindowTransform;

    /** Is ETrue if the screen was reported as fully dirtied in the last update */
    TBool iWholeDisplayAreaIsDirty;    
    
    /** Is ETrue if dirty regions are transformed */
    TBool iUseTransformedDirtyRegions; 

    // RnD
    TBool iDrawDirtyRegions;
    TUint iDrawVisualOutline;
    
    RArray<THuiDisplayBackgroundItem> iBackgroundItems;
    
    TRect iVisibleAreaClippingRect;
    RWindow* iNativeWindow; // Not owned
    
    /** Texture which is draw on top of the roster */
    CHuiTexture* iForegroundTexture;

    /** Foreground bitmap */
    CFbsBitmap* iForegroundBitmap;
    /** Device for foreground bitmap */
    CFbsBitmapDevice* iForegroundBitmapDevice;
    /** Graphics context for foreground bitmap */
    CFbsBitGc* iForegroundBitmapGc;
    /** ETrue if foreground texture should be blend, EFalse otherwise */
    TBool iForegroundTextureTransparency;
    
    /** dirty region index that is being drawn currently */ 
    TInt iCurrentDirtyIndx;
    
    /** Current dirty region for this refresh instance */
    RDirtyRegions iTempDirtyRegions;
    TRect iCurrentDirtyRect;

    
	/** For handling alf content update */
    TBool iScanningAlfContent;
    TBool iDisplayContainsChangedAlfContent;
    TBool iDisplaySizeChangePending;
    };

#endif  // __HUIDISPLAY_H__
