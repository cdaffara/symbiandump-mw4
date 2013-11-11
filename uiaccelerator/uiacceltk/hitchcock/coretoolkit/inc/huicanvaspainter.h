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
* Description:   Definition of CHuiCanvasPainter.
*
*/



#ifndef __HUICANVASPAINTER_H__
#define __HUICANVASPAINTER_H__


#include <e32cmn.h>
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include "huicanvastexturecache.h"


/* Forward declarations. */
class CHuiControl;
class THuiCanvasDrawImageParams;
class THuiCanvasTextParameters;
class CHuiCanvasGc ;
class CHuiGc;
class CHuiCanvasCommandBuffer;

const TInt KHuiCanvasCommandBufferRegionGranularity = 5;

enum THuiCanvasBufferStatusFlags
    {
    /** Buffer is new */
    EHuiCanvasBufferStatusNew = 0x1,
    /** Buffer has been scanned */
    EHuiCanvasBufferStatusScanned = 0x2,        
    /** Buffer has been drawn */
    EHuiCanvasBufferStatusDrawn = 0x4,    
    
    /** Flag to say wheter buffer really contains somethign that really draws something */
    EHuiCanvasBufferContainsDrawing = 0x8000,
    EHuiCanvasBufferContainsTransparentClear = 0x10000,
    /** All flags, can be used for clearing all flags */
    EHuiCanvasBufferStatusAll = 0xFFFFFFFF
    };


NONSHARABLE_CLASS( MHuiCmdBufDestroyObserver )
    {
    public:
        virtual void DestroyBuffer(CHuiCanvasCommandBuffer *aBuffer)=0;
    };

NONSHARABLE_CLASS( CHuiCanvasCommandBuffer ) : public CBase
    {
 public:
    CHuiCanvasCommandBuffer() :    
        iChanged(ETrue),
        iStatusFlags(EHuiCanvasBufferStatusNew)
            {
            iCreationTime.UniversalTime();
            }
    ~CHuiCanvasCommandBuffer()
        {
        TInt count = iObservers.Count();
        for(int i=0;i<count;i++)
            {
            iObservers[i]->DestroyBuffer(this);
            }
        iObservers.Close();
        delete iCommands;
        iUpdateRegion.Close();
        iOriginalUpdateRegion.Close();
#ifdef HUI_DEBUG_TRACK_DRAWING   
        delete iProcessName;
#endif
        }
    
    void SetStatusFlags(THuiCanvasBufferStatusFlags aStatusFlags)
        {
        iStatusFlags |= aStatusFlags;        
        }

    void ClearStatusFlags(THuiCanvasBufferStatusFlags aStatusFlags)
        {
        iStatusFlags &= ~aStatusFlags;
        }
    void AddDestroyObserver(MHuiCmdBufDestroyObserver *aObserver )
        {
        iObservers.AppendL(aObserver);
        }
    void RemoveDestroyObserver(MHuiCmdBufDestroyObserver *aObserver)
        {
        TInt count = iObservers.Count();
        for(int i=count-1;i>=0;i--)
            {
            if (iObservers[i]==aObserver)
                {
                iObservers.Remove(i);
                }
            }
        }
public:
    /** Buffer containing the drawing commands */            
	HBufC8* iCommands;
	
    /** Optional region which specifies the area where commands are to be drawn */
    RRegionBuf<KHuiCanvasCommandBufferRegionGranularity> iUpdateRegion;
    
    /** The original iUpdateRegion before any intersection with the shape region  */
    RRegionBuf<KHuiCanvasCommandBufferRegionGranularity> iOriginalUpdateRegion;
    
    /** Rect of the visual when command buffer was set/added */
    THuiRealRect iOriginalDisplayRect;
    
	/** The position of the visual, when the update region was last modfied */
    TPoint iPositionForUpdateRegion;
    
    /** Internal flag that can be used to track changes. This is similar to visual changed flag. */
    TBool iChanged;

    /** Internal flag that can be used to track buffer status. This is mainly used for optimizing drawing. */
    TInt iStatusFlags;
    
    /** Time when this buffer has been created */
    TTime iCreationTime;
    
    /** Display orientation when this buffer has been created */
    CHuiGc::TOrientation iOrientation;
    
    /** Flag to tell if buffer has unsupported commands (fallback mode is needed) */
    TBool iContainsUnsupportedCommands;
    
#ifdef HUI_DEBUG_TRACK_DRAWING	   
    TBool iTrackedBuffer;
    
    HBufC16* iProcessName;
#endif    
    
    RPointerArray<MHuiCmdBufDestroyObserver> iObservers;
    };

enum THuiCanvasBufferAction
    {
    /** Draw buffer content to the canvas */
    EDrawBuffer,
    
    /** Scan buffer content */
    EScanBuffer        
    };

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS (CHuiCanvasPainter) : public CBase
    {

public:

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
    virtual void ClearCache()= 0;
  
    /**
     * Handles the command buffer(s), either draws or performs a scan.
     *
     * @param aDisplayRect Area in screen coordinates where painter is supposed to draw,
     *        method implementation should modify the given rect to contain the real (smaller) 
     *        area where drawing happens.
     *
     * @param aAction Action that painter is supposed to do with the buffer.
     *
     *        Possible values are:
     *
     *        EScanBuffer - Painter is supposed to scan the buffer and
     *          1) Prepare its internal caches (if it uses such) and other
     *             similar opearations which may take relatively long time to complete. 
     *          2) Prepare PaintedAreaCount() and PaintedArea() to contain the 
     *             real rectangles where drawing is going to happen.
     *
     *        EDrawBuffer - Painter is supposed to draw the content of the
     *          buffer to the given display rect. This action is supposed to
     *          be as fast as possible.
     *
     * @param aUser Canvas visual using this painter
     * @param aGc Toolkit graphics context
	 * @param @aPos The current position of the visual. This is used for drawing 
	 *        the buffers on correct location, if the visual position changes
     */
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos ) = 0;

    /** Same as the other handlebuffer, except it allows external command buffers to be provided
     *  
     */
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos, TPtrC8 aCommands, CHuiCanvasCommandBuffer *buf) =0;
    /**
     * Returns the number of rectangular areas which contain something that is
     * drawn by the painter.
     *
     * @return number of painted areas.
     * @see PaintedArea()
     */
    virtual TInt PaintedAreaCount() const = 0;
    
    /**
     * Returns information of the painted area at given index. 
     *
     * @aIndex Index of the painted area.
     * @return Painted area info.
     * @see PaintedAreaCount()
     */
    virtual THuiCanvasPaintedArea PaintedArea(TInt aIndex) = 0;
    
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
    virtual TInt SetCapturingBufferL(CFbsBitmap* /*aTarget*/) = 0;

    /**
     * Tells painter to enable/disable render buffer (if it is supported by the
     * painter implementation)
     *  
     * @return KErrNotSupported if the current renderer/painter does not support the
     * feature.     *
     */
    virtual TInt EnableRenderBuffer(TBool aEnable = ETrue) = 0;
    
    /**
     * Destructor        
     */
    virtual ~CHuiCanvasPainter();

    /**
     * Returns reference to used canvas gc
     */
    virtual CHuiCanvasGc& CanvasGc() const = 0;
    
    /**
     * Clears capturing buffer area with completely transparent.
     * @param aRect rect to clear.
     */ 
    virtual void ClearCapturingBufferArea(const TRect& aRect);
    
    /**
     * Sets canvas visual that this painter draws into
     */
    void SetVisual(CHuiCanvasVisual* aVisual);

    /**
     * Gets canvas visual that this painter is drawing into.
     */
    CHuiCanvasVisual* Visual();
    
    /**
     * Does this painter have command buffers
     */
    TBool HasCommandBuffers();
    
    /**
     * Is any of the buffers changed
     */
    TBool BuffersChanged() const;

    /**
     * Clear every buffers changed flag.
     */
    void ClearAllBuffersChanged();    

    /**
     * Sets every buffers changed flag to the given value.
     * @param aChange flag value
     */
    void SetAllBuffersChanged(TBool aChanged);    

    /**
     * Requests env to start refresh of whole visual tree
     */
    void RequestContinueRefresh();
    
    /**
     * Sets given status for every buffer.
     */
    void SetAllBufferStatusFlags(THuiCanvasBufferStatusFlags aStatus);

    /**
     * Clears given status for every buffer.
     */
    void ClearAllBufferStatusFlags(THuiCanvasBufferStatusFlags aStatus);

    /**
     * Does this painter have command buffers with given statusflags.
     */
    TBool HasCommandBuffers(THuiCanvasBufferStatusFlags aStatusFlags);

    /**
     * Does this painter have command buffers without given statusflags.
     */
    TBool HasCommandBuffersWithoutFlags(THuiCanvasBufferStatusFlags aStatusFlags);

    /**
     * Does this painter have command buffers older than given time.
     */
    TBool HasOldCommandBuffers(TInt aLifeTimeInMs);
    
    /**
     * Returns the area covered by buffers in the given orientation
     */
    TRect CommandBufferCoverage(TInt aOrientation);

    TInt NumBuffers() const { return iCommandBuffers.Count(); }
    CHuiCanvasCommandBuffer *At(TInt aIndex) const { return iCommandBuffers[aIndex]; }
protected:
	
    /** Command buffers */
	RPointerArray<CHuiCanvasCommandBuffer> iCommandBuffers;
    
    /** Partial command buffer which will be updated with more commands soon. This buffer is not yet ready for drawing */
    HBufC8* iPartialCommandBuffer;    
    
    /** Canvas visual which uses this background */
    CHuiCanvasVisual* iCanvasVisual;    
    }; 
#endif  // __HUICANVASPAINTER_H__
