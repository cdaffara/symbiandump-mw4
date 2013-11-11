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
* Description:   Definition of CHuiCanvasVisual.
*
*/



#ifndef __HUICANVASVISUAL_H__
#define __HUICANVASVISUAL_H__


#include <gdi.h>
#include <bitdraw.h>
#include <e32base.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiLayout.h>    
#include <uiacceltk/HuiDisplay.h>    


/* Forward declarations. */
class CHuiControl;
class THuiCanvasDrawImageParams;
class THuiCanvasTextParameters;
class CHuiCanvasGc;
class MHuiCanvasPainter;

 
/** Public initial values for ws painter */	
#define KHuiWsDefaultUserClipRect TRect(0,0,0,0)
#define KHuiWsDefaultLastPrintPosition TPoint(0,0)
#define KHuiWsDefaultPenColor KRgbBlack  
#define KHuiWsDefaultPenStyle CGraphicsContext::ESolidPen  
#define KHuiWsDefaultPenSize TSize(1,1)  
#define KHuiWsDefaultDrawMode CGraphicsContext::EDrawModePEN  
#define KHuiWsDefaultLinePosition TPoint(0,0)  
#define KHuiWsDefaultOrigin TPoint(0,0)  
#define KHuiWsDefaultBrushUsed EFalse  
#define KHuiWsDefaultBrushColor KRgbWhite  
#define KHuiWsDefaultBrushOrigin TPoint(0,0)  
#define KHuiWsDefaultBrushStyle CGraphicsContext::ENullBrush  
#define KHuiWsDefaultCharJustExcess 0  
#define KHuiWsDefaultCharJustNum 0  
#define KHuiWsDefaultWordJustExcess 0  
#define KHuiWsDefaultWordJustNum 0  
#define KHuiWsDefaultDitherOrigin TPoint(0,0)  
#define KHuiWsDefaultDotLength 0  
#define KHuiWsDefaultDotMask 0  
#define KHuiWsDefaultDotParam 0  
#define KHuiWsDefaultDotDirection 1  
#define KHuiWsDefaultShadowMode CFbsDrawDevice::ENoShadow  
#define KHuiWsDefaultStrikethrough EStrikethroughOff  
#define KHuiWsDefaultUnderline EUnderlineOff  
#define KHuiWsDefaultUserDisplayMode ENone  
#define KHuiWsDefaultShadowColor KRgbGray
#define KHuiWsDefaultFadeColor 0x80FFFFFF

enum THuiCanvasPaintType
    {
    /** Transparent or semitransparent */
    EHuiCanvasPaintTypeTransparent,
    
    /** Fully opaque */
    EHuiCanvasPaintTypeOpaque            
    };

NONSHARABLE_STRUCT (THuiCanvasPaintedArea)
    {
    /** Painted area in the canvas */
    THuiRealRect iPaintedRect;    
    
    /** Paint type of the area */
    TInt iPaintType;
    
    /** Reserved for future */
    TInt iFlags;
    };

enum THuiCanvasFlags
    {
    /** Disables canvas from drawing the canvas content */
    EHuiCanvasFlagDisableCanvasContent    = 0x1,
    /** Disables canvas from drawing the canvas background (if set) */
    EHuiCanvasFlagDisableCanvasBackground = 0x2,
    /** Canvas is drawn with external fade effect */
    EHuiCanvasFlagExternalFade = 0x4,
    /** Canvas drawing is clipped to own and parent canvas intersection */
    EHuiCanvasFlagEnableCanvasClipping = 0x8,           
    /** External fade effect (if enabled) is extended to children */
    EHuiCanvasFlagExternalFadeToChildren = 0x10,    
    /** External fade effect (if enabled) is set to a parent (or grandparent) */
    EHuiCanvasFlagExternalFadeByParent = 0x40,    
    /** Effect is excluded from parent (or grandparent) effect (if it has effect which is applied to children) */
    EHuiCanvasFlagExcludeFromParentEffect = 0x80,
    /** Hint flag which is used for optimizations. Tells if fade effect(s) exist in visual tree */
    EHuiCanvasFlagExternalFadeExistsInsideVisualTree = 0x100,
    /** Flag to indicate whether to include this visual to visibility calculation */
    EHuiCanvasFlagIncludeToVisibilityCalculation = 0x200,
    /** Flag to indicate whether surface is invisible (based on visibility calculation) */
    EHuiCanvasFlagSurfaceInvisible = 0x400,
    /** Flag to indicate whether surface is visible (based on visibility calculation) */
    EHuiCanvasFlagSurfaceVisible = 0x800,
    /** Flag to indicate avkon clearer window. This needs to be drawn, but excluded in the heuristics determining application screen coverage */
    EHuiCanvasFlagDistractionWindow = 0x1000
    };

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
class CHuiCanvasVisual : public CHuiLayout
    {
public:

    enum
        {
        /** @deprecated */
        ECommandBufferHui,
        /** @deprecated */
        ECommandBufferSgc    
        };
	
	enum
	    {
	    ECommandBufferAlf,
	    ECommandBufferWs    
	    };
	    
    /* Constructors and destructor. */

    /**
     * Constructs and appends a new Canvas visual to the owner control.
     *
     * @param aOwnerControl  Control that will own the new visual.    
     * 
     * @param aCanvasType The enumerator describing the type of concrete Canvas
     * to be created.     
     *
     * @return  The new visual.
     */
    IMPORT_C static CHuiCanvasVisual* AddNewL(CHuiControl& aOwnerControl,
                                            CHuiLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    CHuiCanvasVisual(MHuiVisualOwner& aOwner);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CHuiCanvasVisual();

    /**
     * Sets buffer that contains canvas drawing commands. This
     * method clears previous buffer(s).
     *
     * @param aCommands Commanbuffer
     */
    IMPORT_C void SetCommandSetL( const TDesC8& aCommands );

    /**
     * Sets the type of commands in the buffer.
     *
     * @param aCommandType Type of the command buffer, either
     * ECommandBufferHui or ECommandBufferSgc. In the future
     * more command set types may be supported.
     *
     */
    IMPORT_C void SetCommandType( TInt aCommandType );

    /**
     * Clears buffer(s) that contains canvas drawing commands. 
     */
    IMPORT_C void ClearCommandSet();
    
    IMPORT_C void SetTrackCommandSet( TFileName& aFileName, TBool aTrack );
        
    /**
     * Adds buffer that contains more canvas drawing commands,
     * those will be executed after the buffers that has been
     * added previously.
     * @param aMoreCommands Commanbuffer
     */
    IMPORT_C void AddCommandSetL( const TDesC8& aMoreCommands );

    /**
     * Adds partial buffer that contains more canvas drawing commands. These
     * commands are appended to the commands set previously with this method
     * until param aLastPart has value ETrue. After that whole command set
     * will be added among possibly already existing command sets and
     * it will be drawable.
     * 
     * @param aMoreCommands Commanbuffer
     * @param aLastPart EFalse if there are more commands to be added into this
     * command set before it is wanted to be available for drawing. ETrue if
     * command set is ready and no more parts are expected.
     */
    IMPORT_C void AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart );

    /**
     * Clears internal cached texts and images.
     */
    IMPORT_C void ClearCache();

    /**
     * Prepares internal cached texts and images.
     */
    IMPORT_C void PrepareCache();
    
    /**
     * Sets the flag that tells the canvas to clear the background before
     * doing a refresh. 
     *
     * Note1: EClearWithSkinBackground is not (yet) supported.
     * Note2: EClearWithColor does not do alphablending, it just clears the area 
     * and writes alpha values to the rendering surface. 
     *
     * @param aClearBackground  Background clearing mode. 
     * @see SetBackgroundColor()
     */
    IMPORT_C void SetClearBackground(CHuiDisplay::TClearMode aClearBackground);

    /**
     * Sets the background color, if background clearing has
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
     */
    IMPORT_C void SetBackgroundItems(const RArray<THuiDisplayBackgroundItem>& aItems);
    
    /**
     * Returns the number of rectangular areas which contain something that is
     * drawn by the visual.
     *
     * @return number of painted areas.
     * @see PaintedArea()
     */
    IMPORT_C TInt PaintedAreaCount() const;
    
    /**
     * Returns information of the painted area at given index. 
     *
     * @param aIndex Index of the painted area.
     * @return Painted area info.
     * @see PaintedAreaCount()()
     */
    IMPORT_C THuiCanvasPaintedArea PaintedArea(TInt aIndex);
    
    /**
     * Draws content of the window in pixel format to the given bitmap. 
     * This method may not be supported in all commanbuffertype/renderer combinations.
     *
     * @param aTargetBuffer where content of the window is to be drawn. NULL if the drawing
     * is to be stopped.
     *
     * @return KErrNotSupported if the current renderer/painter does not support the
     * feature. Leave happens in other unexpected error situtations. 
     *         
     */
    IMPORT_C TInt SetCapturingBufferL(CFbsBitmap* aTargetBuffer);
    
    /**
     * Sets canvas flags
     * @param aCanvasFlags flags to be set
     *
     */
    IMPORT_C void SetCanvasFlags(TInt aCanvasFlags);

    /**
     * Clears canvas flags
     * @param aCanvasFlags flags to be cleared.
     *
     */
    IMPORT_C void ClearCanvasFlags(TInt aCanvasFlags);

    /**
     * Gets canvas flags
     * @return flags
     *
     */
    IMPORT_C TInt CanvasFlags();

    /**
     * Set external content visual to be drawn to the canvas visual.
     * Giving a NULL pointer as a parameter removes the external content.
     * @param aVisual the visual that is used as external content
     *
     */
	IMPORT_C void SetExternalContentL(CHuiVisual* aVisual);

    /**
     * Return external content visual of the canvas visual.
     * NULL return value means there is no external content.
     * @return NULL or the visual that is used as external content
     */
    IMPORT_C CHuiVisual* ExternalContent();

    /**
     * Set parent canvas visual. Parent is used for determining clipping 
     * region incase canvasflag EHuiCanvasFlagEnableCanvasClipping is set.
     *
     * @param aParent Canvas visual which is used to determine effective 
     * clipregion.
     */
    IMPORT_C void SetParentCanvas(CHuiCanvasVisual* aParent);
    
    /**
     * Gets reference to canvas gc
     */
    CHuiCanvasGc& CanvasGc() const;

    IMPORT_C TBool IsBackgroundDrawingEnabled() const;

    const TRegion& ClipRegion() const;
    
    void AddChildCanvas(CHuiCanvasVisual* aChild);

    void RemoveChildCanvas(CHuiCanvasVisual* aChild);
        
    IMPORT_C TBool HasCommandBuffers(TBool aAskFromChildren = ETrue) const;
    
    IMPORT_C TBool HasTransParentClear() const;
    
    // to handle background surface that may be set to the Window
public:    
    
    IMPORT_C TBool LayerUsesAlphaFlag();
    IMPORT_C void SetLayerUsesAlphaFlag(TBool aEnabled);
    IMPORT_C TRect& LayerExtent(); 
    IMPORT_C void SetLayerExtent(TRect& aExtent); 

    // window area
public:
    
    /**
     * Sets shape. This affects only visible area calculation.
     */
    IMPORT_C void SetShape(const TPoint& aOrigin, const TRegion& aRegion);
    /**
     * Returns true if shape is different than display rect.
     */
    IMPORT_C TBool HasCustomShape() const;
    /**
     * Returns origin of window shape.
     */
    IMPORT_C TPoint ShapeOrigin() const;
    /**
     * Returns shape as region.
     */
    IMPORT_C const TRegion& ShapeRegion() const;

public:
    
    void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);    
    TBool ChildSize(TInt aOrdinal, TSize& aSize);    
    TBool ChildPos(TInt aOrdinal, TPoint& aPos);
    TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);        

    
private:
    
    void HandleBuffer(TRect& aDisplayRect, TInt aAction, CHuiGc* aGc) const;        

    void SetCommandTypeL(TInt aCommandType);         
 
    TBool IsContentDrawingLayered() const;
    
    TBool IsContentDrawingEnabled() const;

    TBool IsExternalContentDrawingEnabled(TBool aIncludeChildren = EFalse) const;
    
    TBool IsCanvasClippingEnabled() const;
    
    TBool EffectActive() const;
    
public: // From CHuiVisual

	TBool PrepareDrawL();
	
	void Draw(CHuiGc& aGc) const;

 	/**
     * Draw the visual.
     */
    void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;

    TBool Changed() const;

    IMPORT_C void SetChanged();
    
    IMPORT_C void ClearChanged();

    void ExpandRectWithContent(TRect& aRect) const;
    THuiRealPoint ConvertPoint(const THuiRealPoint& aPointInUnits) const;
    void ExpandRect(TRect& aRectToExpand, const TRect& aNewRect) const;

public:    
    IMPORT_C void StoreRenderBufferL();
    void DrawStoredVisualRenderBuffer(TInt aCanvasDrawMode) const;
    void DrawStoredFullScreenRenderBuffer(TInt aCanvasDrawMode, CHuiGc& aGc) const;
    IMPORT_C void FreeRenderBuffer();
    IMPORT_C TRect CommandBufferCoverage(TInt aOrientation);

protected:
    virtual void VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
       
    THuiCanvasPaintedArea CanvasPaintedArea(TInt aIndex) const;
    
    void DrawChildren(CHuiGc& aGc) const;
    void DrawCanvasChildren(CHuiGc& aGc, TInt aIncludeCanvasFlags, TInt aExcludeCanvasFlags) const;
    TBool ChildTreeChanged(TInt aExcludeCanvasFlags) const;
    
    static TBool RecursiveChildTreeChanged(CHuiVisual* aVisual, TInt aExcludeCanvasFlags);
    static TBool RecursiveIsExternalContentDrawingEnabled(CHuiVisual* aVisual);
    static TBool RecursiveHasCommandBuffers(CHuiVisual* aVisual);
    
    TBool CollectPaintedRegion(TRegion& aRecursivePaintRegion, TInt aExcludeCanvasFlags) const;
    TBool CollectRecursivePaintedRegion(TRegion& aRecursivePaintRegion, TInt aExcludeCanvasFlags) const;
    
    TBool CanSkipDrawing() const;
    TBool KeepNoCache() const;

private: 

    struct THuiCanvasVisualData;
    THuiCanvasVisualData* iCanvasVisualData;


    };

#endif  // __HUICANVASVISUAL_H__
