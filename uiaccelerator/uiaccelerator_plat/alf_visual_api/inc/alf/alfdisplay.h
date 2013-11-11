/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Display class.
*
*/



#ifndef C_ALFDISPLAY_H
#define C_ALFDISPLAY_H

#include <e32base.h>
#include <alf/alfdisplaybackgrounditem.h>
#include <alf/alfevent.h>

class CAlfEnv;
class CAlfRoster;
class MAlfInputCapabilitiesSupplier;
class MObjectProvider;
class TPointerEvent;


/* Screen buffer uids */
const TUid KAlfUidBackBufferAll = 			{0x2000e59d}; // Not used
const TUid KAlfUidBackBufferScreen0 = 		{0x2000e59e};
const TUid KAlfUidBackBufferScreen1 = 		{0x2000e59f};
const TUid KAlfUidBackBufferTvOutNormal = 	{0x2000e5a0};
const TUid KAlfUidBackBufferTvOutWide = 	{0x2000e5a1};

/**
 * Rendering quality enumerator. 
 *
 * @see CAlfDisplay::SetQuality()
 * @see CAlfDisplay::Quality()
 */
enum TAlfQuality
    {
    /** Accurate quality ensures the best visual quality. */
    EAlfQualityAccurate,

    /** Fastest rendering speed in exchange for lower quality. */
    EAlfQualityFast
    };

/**
*  The intended usage of the display. The display may use this info to 
*  prioritize the HW usage and route content.
*  Vendor specific meanings may be used at the end of the enum space.
*
*  @see CAlfDisplay::SetUsageL()
*/
enum TAlfDisplayUsageHint
    {
    EAlfDisplayUsageGeneric        = 0x00000001,
    EAlfDisplayUsage3D             = 0x00000002,
    EAlfDisplayUsageVideoPlayback  = 0x00000004,
    EAlfDisplayUsageVideoCapture   = 0x00000008,
    EAlfDisplayUsageViewfinder     = 0x00000010,
    EAlfDisplayUsageStillImage     = 0x00000011,
    EAlfDisplayUsageVendorSpecific = 0x80000000
    };

/**
 *  Display class represents a drawing area on which UI Accelerator Toolkit
 *  draws it's contents. 
 *  
 *  The display always occupies a rectangular area of a rendering surface. 
 *  Usually the display occupies the whole rendering surface but it can also 
 *  occupy only parts of the rendering surface. Multiple displays can then 
 *  occupy multiple parts of the same rendering surface. This can be achieved 
 *  by setting the visible area of a display through 
 *  CAlfDisplay::SetVisibleArea() - method.
 *
 *  There should always be at least one display on which the contents of an
 *  application UI can be drawn to. The display can be one of the several 
 *  display types listed in CAlfDisplay::TDisplayType.
 *
 *  Before any visual content is drawn on the display, the display area is
 *  cleared. Different parts of the display can be cleared with different 
 *  methods and can be configured using the 
 *  CAlfDisplay::SetBackgroundItemsL() - method. Alternatively the clearing 
 *  mode to clear the whole display area can be assigned using 
 *  CAlfDisplay::SetClearBackgroundL() - method.
 *
 *  A display contains always a CAlfRoster - object which defines the drawing
 *  order of the CAlfControlGroup objects in the UI, controls the order in 
 *  which events are passed from control group to another and defines which
 *  control groups are shown and hidden at any given time.
 *
 *  To construct a display, use the CAlfEnv::NewDisplayL() - method.
 *  Usage:
 *  @code
 *  CAlfDisplay* display = iEnv->NewDisplayLiEnv->NewDisplayL( ClientRect(), 
 * 				CAlfEnv::ENewDisplayAsCoeControl );
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfDisplay ): public CBase
    {
public:

    /* Types. */

    /**
     * Background clear modes. Different clear modes can be taken into use 
     * with CAlfDisplay::SetClearBackgroundL() - method.
     */
    enum TClearMode
        {
        /**
         * Do not clear. This is more like an optimization flag.
         * Using this with OpenGL ES renderer results
         * to undefined (Trashed) background - so be sure to display content
         * that fills the entire screen. With the BITGDI renderer the
         * behavior is similar but the display are will show the previously
         * rendered avkon content (if any).
         */
        EClearNone = EFalse,

        /**
         * Clear with a solid color.
         */
        EClearWithColor = ETrue,

        /**
         * Clear with the skin background texture.
         */
        EClearWithSkinBackground
        };

    /** Display types. */
    enum TDisplayType
        {
        /** @deprecated: Primary LCD display. */
        EDisplayLcd0 = 0x0,              
 
        /** @deprecated: TV out */
        EDisplayTvOut = 0x2,

        /** @deprecated: TV out widescreen */
        EDisplayTvOutWide = 0x3,

        /** Physical screen */
        EDisplayNormal = 0x10,     
 
        /** Not used. Virtual display */
        EDisplayVirtual = 0x20,

        /** @deprecated: Off screen buffer */
        EDisplayOffScreenBuffer = 0x40
        };
                
    /**
     * Constructor
     *
     * To construct a new display, use CAlfEnv::NewDisplayL() - method.
     */
    CAlfDisplay();
    
    /**
     * ConstructL
     *
     * To construct a new display, use CAlfEnv::NewDisplayL() - method.
     */
    void ConstructL(
        CAlfEnv& aEnv, 
        TBool aAsCoeControl,
        const TRect& aRect, 
        CAlfRoster* aSharedRoster,
        TInt aDisplayType, 
        TUid aBufferUid);

    /**
     * Destructor.
     */
    virtual ~CAlfDisplay();

    /**
     * Sets the flag that tells the display to clear the background before
     * doing a refresh. Creates new objects required to be able to clear
     * the background with the defined mode.
     *
     * @param aClearBackground  Background clearing mode. For example EFalse,
     *                          ETrue, EClearWithColor,
     *                          EClearWithSkinBackground are the valid 
     *                          alternatives. EClearWithColor is equivalent to 
     *                          ETrue. EFalse is equivalent to EClearNone.
     *
     * @see CAlfDisplay::TClearMode
     */
    IMPORT_C void SetClearBackgroundL(TInt aClearBackground);

    /**
     * Sets the items that specify how to clear display before display
     * refresh. Items may have overlapping rectangles, drawing order is same 
     * as item order in the parameter array. But having lots of overlapping 
     * drawing causes negative performance impact.
     *
     * This method is alternative to SetClearBackgroundL().
     *
     * @see SetClearBackgroundL()
     * @param aItems Array of items that specify how to clear the background.
     *
     */
    IMPORT_C void SetBackgroundItemsL(
                    const RArray<TAlfDisplayBackgroundItem>& aItems);
    
    /**
     * Returns the roster of the display.
     * @return The roster.
     */
    IMPORT_C CAlfRoster& Roster();

    /**
     * Returns the roster of the display.
     * @return The roster.
     */
    IMPORT_C const CAlfRoster& Roster() const;
    
    /**
     * Sets the visible area of the display. The visible area is the area on
     * the display's rendering surface where the display is drawn. Normally
     * a display covers the entire rendering surface, but in the situation
     * where multiple displays are drawn on the same surface, this would be
     * used to define the visible area of each display.
     * Note: This does not set the visible area if alf server session is not focused
     * 
     * @param aArea  Visible area for the display.
     */
    IMPORT_C void SetVisibleArea(const TRect& aArea);

    /**
     * Sets the visible area of the display. Similar to SetVisibleArea.
     * But this sets the visible area even though alf server session is not focused.
     * Shall be used by client app when Dynamic layout switch happens. 
     *
     * @param aArea  Visible area for the display.
     */
    IMPORT_C void ForceSetVisibleArea(const TRect& aArea);
    
    /**
     * Returns the visible area of the display.
     *
     * @return  Visible area of the display. Affected by display orientation
     *          so that the returned rectangle uses the oriented coordinate
     *          space.
     */
    IMPORT_C TRect VisibleArea() const;
    
    /**
     * Returns the environment of the display.
     */
    IMPORT_C CAlfEnv& Env();
    
    /**
     * Return the server side handle
     *
     * @return Handle to the server side CAlfSrvDisplaySubSession object. 0 if
     *         not set.
     */
    TInt ServerHandle() const;
    
    /**
     * Set the current focused editor. 
     */
    IMPORT_C void SetFocusedEditor(
                    MAlfInputCapabilitiesSupplier* aCurrentEditor);

    /**
     * Flag the entire display as dirty.
     */
    IMPORT_C void SetDirty();
    
    /**
     * Sets the rendering quality of the display.
     *
     * @param aRenderingQuality One of the quality enumerators in TAlfQuality.
     */
    IMPORT_C void SetQuality(TAlfQuality aRenderingQuality);

    /**
     * Determines the rendering quality of the display.
     *
     * @return TAlfQuality enumerator that represents the rendering quality
     *         setting.
     */
    IMPORT_C TAlfQuality Quality() const;
    
    /**
     * Sets the flag that enables the use of depth testing.
     *
     * @param aUseDepth  <code>ETrue</code> to enable depth.
     */
    IMPORT_C void SetUseDepth(TBool aUseDepth);
        
    /**
    * Method to return an object provider interface for this display.
    *
    * @return a pointer or NULL to an MObjectProvider interface
    */
    IMPORT_C MObjectProvider* ObjectProvider() const;

    /**
     * Tells display the intended usage of the display. Display may use this
     * information to optimize the drawing.
     *
     * @param aUsageHint
     *
     *   Possible values for parameter are:
     *   - EAlfDisplayUsageGeneric 
     *   - EAlfDisplayUsage3D      
     *   - EAlfDisplayUsageVideoPlayback
     *   - EAlfDisplayUsageVideoCapture 
     *   - EAlfDisplayUsageViewfinder   
     *   - EAlfDisplayUsageStillImage   
     *   - EAlfDisplayUsageVendorSpecific + n
     *  
     *  Supported values depend on display type
     *  and underlying HW.
     * 
     *  @see TAlfDisplayUsageHint
     */    
    IMPORT_C void SetUsageL(TUint aUsageHint);

    /**
     * Suppresses automatic fading by display. This applies only to displays that are tied
     * to windows. By default, automatic fading is enabled.
     * @param aSuppress ETrue to suppress, EFalse to enable automatic fading.
     * @return error code, KErrNone upon success.
     */
    IMPORT_C TInt SuppressAutomaticFading( TBool aSuppress = ETrue );

    /**
     * Visual content will be drawn to the given window. The visual can be a visual tree i.e.
     * layout visual. The visual will not be any more drawn to the original place. The window
     * need not to be a full screen window. Giving NULL as aVisual will remove the binding,
     * and visual content will be drawn to the original place again.
     *
     * @param aWindow  Application window.
     * @param aVisual  Visual or layout visual.
     */
	IMPORT_C void SetClientWindowForDrawingL(RWindow* aWindow, CAlfVisual* aVisual);

    /** @endAPI */
    
    /**
     * Handles layout change.
     */
    void NotifyLayoutChangedL();

    /**
    * Accesses whether this CAlfDisplay is focused. 
    * A display should normally be focused. It will not be focused at least in
    * the following cases:
    *  - a non-ALF window in the current application is present in front of
    *    this AlfDisplay
    *  - a non-ALF window in another application is present in front of this
    *    AlfDisplay
    *
    * @return the focus state
    */
    TBool IsFocused() const;

    /**
     * Forwards a pointer event received from CONE. This can be called directly by application
     * to pass in external pointer events. You will have to call this from an application to pass
     * pointer events to CAlfControl - derived objects if you have created the CAlfDisplay 
     * class as offscreen buffer that has no native window to grab pointerevents automatically.
     *
     * @param aPointerEvent  Received pointer event to handle. Note that iParentPosition of the 
     * pointerevent should always be in the same coordinate space as where the displays rect 
     * was defined when it was created.
     */
    IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private:

    /**
     * Chacks screen size.
     */
    void CheckScreenRect(TRect aRect);


private:
    
    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;
     
    };

#endif // C_ALFDISPLAY_H
