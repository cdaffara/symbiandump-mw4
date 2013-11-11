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
* Description:   Definition of CHuiCanvasWsPainter.
*
*/



#ifndef __HUICANVASWSPAINTER_H__
#define __HUICANVASWSPAINTER_H__

#include "huicanvaspainter.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"

#include <e32base.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiVisual.h>    
#include <uiacceltk/HuiDisplay.h>    
#include <uiacceltk/huilowmemoryobserver.h>
#include "huicanvaswshwgc.h"

const TInt KHuiCanvasWsPainterRegionGranularity = 10;

class CHuiCanvasWsSwGc;
class CHuiCanvasWsHwGc;
class CAlfCommandDebug;
class CHuiCanvasWsBitGc;
#ifdef HUI_DEBUG_TRACK_DRAWING
class CHuiCanvasDebugWsGc;
#endif
    

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS(CHuiCanvasWsPainter) : public CHuiCanvasPainter, public MHuiMemoryLevelObserver
    {    
public:

	/**
     * Constructor.
     */
    static CHuiCanvasWsPainter* NewL();
    
    /**
     * Destructor.
     */
    ~CHuiCanvasWsPainter();

public: // From CHuiCanvasPainter

    /**
     * Sets buffer that contains canvas drawing commands. This
     * method clears previous buffer(s), so this is eqvivalent
     * to calling both ClearCommandSet and AddCommandSetL.
     *
     * @param aCommands Commanbuffer
     */
    virtual void SetCommandSetL( const TDesC8& aCommands );

    /**
     * Clears buffer(s) that contains canvas drawing commands. 
     */
    virtual void ClearCommandSet();

    /**
     * Adds buffer that contains more canvas drawing commands,
     * those will be executed after the buffers that has been
     * added previously.
     * @param aMoreCommands Commanbuffer
     */
    virtual void AddCommandSetL( const TDesC8& aMoreCommands );

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
    virtual void AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart );

#ifdef HUI_DEBUG_TRACK_DRAWING
	virtual void SetTrackCommandSet( TFileName& aFileName, TBool aTrack );
#endif
	
    /**
     * Clears internal cached texts and images.
     */
    virtual void ClearCache();
  
    /**
     * Handles the command buffer(s), either draws or performs a scan.
     *
     * @param aDisplayRect Area where painter is supposed to draw
     * @param aAction Action that painter is supposed to do with the buffer.
     * @param aUser Canvas visual using this painter
     * @param aGc Toolkit graphics context
     */
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos ) ;
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos, TPtrC8 aCommands, CHuiCanvasCommandBuffer *buf);

    /**
     * Returns the number of rectangular areas which contain something that is
     * drawn by the painter.
     *
     * @return number of painted areas.
     * @see PaintedArea()
     */
    virtual TInt PaintedAreaCount() const;
    
    /**
     * Returns information of the painted area at given index. 
     *
     * @aIndex Index of the painted area.
     * @return Painted area info.
     * @see PaintedAreaCount()
     */
    virtual THuiCanvasPaintedArea PaintedArea(TInt aIndex);
    
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
    virtual TInt SetCapturingBufferL(CFbsBitmap* aTarget);
    
    /**
     * Tells painter to enable/disable render buffer (if it is supported by the
     * painter implementation)
     *  
     * @return KErrNotSupported if the current renderer/painter does not support the
     * feature.     
     */
    virtual TInt EnableRenderBuffer(TBool aEnable = ETrue);
    
    /**
     * Returns reference to canvas gc which is used by this painter.
     */
    virtual CHuiCanvasGc& CanvasGc() const;
    CHuiCanvasWsGc *CanvasWsGc() const { return iCanvasWsGc; }

private:

    /**
	 * From MHuiMemoryLevelObserver
	 */
    void SetMemoryLevel(THuiMemoryLevel aLevel);

    /**
     * Constructor.
     */
    CHuiCanvasWsPainter();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
private:

    void DoHandleAllBuffersL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint& aPos, 
            TBool aIgnoreUnChangedBuffers, 
            TBool aIgnoreClippedBuffers, 
            TBool aIgnoreHandledBuffers,
            TBool aClearBeforeHandlingBuffers);
    void DoHandleBufferL(TInt aIndex, TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint& aPos); 
    void DoHandleBufferStringL(TInt aIndex, TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint& aPos, TPtr8 aCommandBuffer, CHuiCanvasCommandBuffer *aBuffer);

    void DoPeekBufferAndUpdateShapeRegionL();
    void DoPeekBufferL(TInt aIndex);     
    void DoDigestPaddingL();

    TBool RemoveRedundantBuffers();    
    void RemoveBuffersWithOldDisplayRect();
    void RemoveBuffersWithEmptyUpdateRegion();
    void RemoveBuffersWithOverlappingUpdateRegion();        
    void RemoveBuffersWithoutRealDrawing();
    void DestroyBuffer(TInt aIndex);
    void UpdateBufferUpdateRegions(TPoint aPos);
    
    void RemoveBuffersWithMovedDisplayRect();
    void ModifyBuffersWithChangedDisplayRect();
    
public:    
    void EnableRenderBufferIfNeeded(TBool aEnable);
private:
    
    void SelectGcL();
    CHuiGc::TOrientation CalculateRelativeOrientation();    
    TRect DirtyRect()const;
    TBool IsBufferCompletelyOutisideClippingRegion(const CHuiCanvasCommandBuffer& aCommandbuffer) const;
    void RenewAllBuffers();    
    
    void WsSetUpdateRegionL(TInt aIndex);
    void WsBitBltL() ;
    void WsBitBltRectL() ;
    void WsBitBltMaskedL() ;
    void WsCombinedBitBlitMaskedL();
    void WsBitBltMaskedPointL() ;
    void WsResetClippingRegionL() ;
    void WsClearL() ;
    void WsClearRectL() ;    
    void WsResetBrushPatternL() ;
    void WsResetFontL() ;
    void WsDrawArcL() ;
    void WsDrawPieL() ;
    void WsDrawBitmap1L() ;
    void WsDrawBitmap2L() ;
    void WsDrawBitmap3L() ;
    void WsDrawBitmapMaskedL() ;
    void WsDrawRoundRectL() ;
    void WsDrawPolyLineL();
    void WsDrawPolyLineNoEndPointL() ;
    void WsDrawPolygonL() ;
    void WsDrawEllipseL() ;
    void WsDrawLineL() ;
    void WsDrawLineToL() ;
    void WsDrawLineByL() ;
    void WsDrawRectL() ;
    void WsDrawText1L() ;
    void WsDrawText2L() ;
    void WsDrawText3L() ;
    void WsDrawText4L() ;
    void WsDrawText5L() ;
    void WsDrawTextVertical1L() ;
    void WsDrawTextVertical2L() ;
    void WsDrawTextVertical3L() ;
    void WsDrawTextVertical4L() ;
    void WsDrawTextVertical5L() ;
    void WsMoveToL() ;
    void WsMoveByL() ;
    void WsPlotL() ;
    void WsResetL() ;
    void WsSetBrushColorL() ;
    void WsSetBrushOriginL() ;
    void WsSetBrushStyleL() ;
    void WsSetClippingRegionL() ;
    void WsSetDrawModeL() ;
    void WsSetOriginL() ;
    void WsSetPenColorL() ;
    void WsSetPenStyleL() ;
    void WsSetPenSizeL() ;
    void WsSetTextShadowColorL() ;
    void WsSetCharJustificationL() ;
    void WsSetWordJustificationL() ;
    void WsSetUnderlineStyleL() ;
    void WsSetStrikethroughStyleL() ;
    void WsSetBrushPatternL() ;
    void WsSetFontL() ;
    void WsContentTagL();
    void WsCopyRectL() ;
    void WsUpdateJustificationL() ;
    void WsUpdateJustificationVerticalL() ;
    void WsSetFontNoDuplicateL() ;
    void WsCopySettingsL() ;
    void WsSetClippingRectL() ;
    void WsCancelClippingRectL() ;
    void WsSetFadedL() ;
    void WsSetFadingParametersL() ;
    void WsFadeAreaL() ;
    void WsMapColorsL() ;
    void WsSetUserDisplayModeL() ;
    void WsUseFontL() ;
	void WsSetShapeRegionL( TInt aIndex );
    
    virtual void ClearCapturingBufferArea(const TRect& aRect);
    
    void ApplyShapeRegion();
    void TranslateShapeRegion(const TPoint& aNewOrigin);
    
private:
    
    /** Region which this painter updates with current command buffers */
    RRegionBuf<KHuiCanvasWsPainterRegionGranularity> iFullUpdateRegion;
	
    /** Window shape region */
    RRegionBuf<KHuiCanvasCommandBufferRegionGranularity> iShapeRegion;
    
    /** Command buffer reader instance */
    CHuiCanvasCmdBufferReader* iWsCommandBufferReader;    
 
    /** Pointer to active canvaswsgc instance */
    CHuiCanvasWsGc *iCanvasWsGc;
    
    /** Fully bitgdi based version of canvaswsgc */
    CHuiCanvasWsBitGc *iCanvasWsBitGc;

    /** Partially SW/HW based version of canvaswsgc */
    CHuiCanvasWsSwGc *iCanvasWsSwGc;
    
    /** Fully HW accelerated canvaswsgc */
    CHuiCanvasWsHwGc *iCanvasWsHwGc;

    /** Flag to tell whether cache has been cleared */
    TBool iCacheCleared;
    
    /** Use of render buffer has been disabled/enabled automatically */
    TBool iAutomaticRenderBufferUsage;
    
    /** Flags to tell whether the window shape region has been changed  */
    TBool iShapeRegionAvailable;
    TPoint iShapeRegionOrigin;
    TBool iShapeRegionClearingPending;
    TBool iShapeRegionClippingPending;
    
	#ifdef __ALF_SYMBIAN_RWINDOW_CLEARING_BUG_WORKAROUND__
    TRgb iTestBrushColor;
    TRect iTestFullDisplayRect;
    #endif
	
	TRgb iBrushColor;
	
	// Stream read optimizations
    struct TStructBitBlitRect
        {
        TPoint iPoint;
        TInt iBitmapHandle;
        TRect iRect;
        };

    struct TStructBitBlitMasked
        {
        TPoint iPoint;
        TInt iBitmapHandle;
        TRect iRect;
        TInt iMaskHandle;
        TInt iInvertMask;
        };

    struct TStructDrawBitmapMasked
        {
        TRect iRect1;
        TInt iBitmapHandle;
        TRect iRect2;
        TInt iMaskHandle;
        TInt iInvertMask;
        };
    
    struct TStructDrawTextGeneral
        {
        TRect iRect;
        TInt iValue1;
        TInt iValue2;
        TInt iValue3;
        TInt iValue4;
        
        };
    
    class TBlitStruct
        {
    public:
        TBlitStruct( TInt aHandle, TInt aMaskHandle, TPoint aTl ) : iHandle( aHandle ), iMaskHandle( aMaskHandle ), iTl( aTl ){};
        TInt iHandle;
        TInt iMaskHandle;
        TPoint iTl;
        };
    
	TStructBitBlitMasked    iStructBitBlitMasked;
    
	TStructBitBlitRect      iStructBitBlitRect;
    
	TStructDrawBitmapMasked iStructDrawBitmapMasked;
    
	TStructDrawTextGeneral  iStructDrawTextGeneral;
	
	THuiCanvasTextParameters iTextParametersGeneral;
  
    /** Private temp region for general purpose, used to avoid excessive allocations of RRegion heap objects */
    mutable RRegionBuf<KHuiCanvasWsPainterRegionGranularity> iTempRegion;
    mutable RRegionBuf<KHuiCanvasWsPainterRegionGranularity> iTempCurrentSubRegion;
    mutable RRegionBuf<KHuiCanvasWsPainterRegionGranularity> iTempIntersectingRegion;

#ifdef HUI_DEBUG_TRACK_DRAWING
    CHuiCanvasDebugWsGc* iCanvasDebugWsGc; 
#endif
    
    // Boolean flag indicating if render buffer ought to be used.
    TBool iEnableRenderBuffer;
    };
    
#endif  // __HUICANVASWSPAINTER_H__
