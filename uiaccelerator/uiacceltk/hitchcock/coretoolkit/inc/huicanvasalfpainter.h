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
* Description:   Definition of CHuiCanvasAlfPainter.
*
*/



#ifndef __HUICANVASALFPAINTER_H__
#define __HUICANVASALFPAINTER_H__

#include <uiacceltk/HuiVisual.h>
#include "uiacceltk/HuiTextMesh.h"
//#include <uiacceltk/HuiCanvasPainter.h>
#include "huicanvaspainter.h"

enum THuiCanvasCommands
    {
    // Draw ops
    EHuiCanvasDrawImage,
    EHuiCanvasDrawText,
    EHuiCanvasDrawLines,
	  EHuiCanvasDrawEllipse,
		EHuiCanvasDrawPoints,
		EHuiCanvasDrawPolygon,
		EHuiCanvasDrawRects,

		// Setters
		EHuiCanvasSetTextStyle,
    EHuiCanvasSetPenColor,
		EHuiCanvasSetPenWidth,
		EHuiCanvasSetOpacity,
		EHuiCanvasSetPolygonDrawMode,
		EHuiCanvasSetTextAlign,

    // Transformations
		EHuiCanvasLoadIdentity,
		EHuiCanvasTranslate,
		EHuiCanvasScale,
		EHuiCanvasRotate,
	
		// Misc
		EHuiCanvasPrepareDrawText
    };

NONSHARABLE_STRUCT( THuiCanvasDrawImageParams )
    {
    TInt    iTexture;
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };
    
NONSHARABLE_STRUCT( THuiCanvasDrawTextParams )
    {
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };

	
NONSHARABLE_STRUCT( THuiCanvasDrawEllipseParams )
	{
    TReal32 iDestinationRectTopLeftX;
    TReal32 iDestinationRectTopLeftY;
    TReal32 iDestinationRectBottomRightX;
    TReal32 iDestinationRectBottomRightY;
    };

NONSHARABLE_STRUCT( THuiCanvasSetTextStyleParams )
    {
    TInt  iTextStyleId;
    };
    
NONSHARABLE_STRUCT( THuiCanvasDrawTextCachedTextEntry )
    {
    TInt iId;
    CHuiTextMesh* iTextMesh;        
    };

NONSHARABLE_STRUCT( THuiCanvasSetPenColorParams )
	{
    TRgb  iPenColor;
    };
NONSHARABLE_STRUCT( THuiCanvasSetPenWidthParams )
	{
    TReal32  iPenWidth;
    };
NONSHARABLE_STRUCT( THuiCanvasSetOpacityParams )
	{
    TReal32  iOpacity;
    };
NONSHARABLE_STRUCT( THuiCanvasSetPolygonDrawModeParams )
	{
	THuiFillMode  iPolygonDrawMode;
    };
NONSHARABLE_STRUCT( THuiCanvasSetTextAlignParams )
	{
    TInt  iTextAlignHorizontal;
    TInt  iTextAlignVertical;
    };


   // Transformations
NONSHARABLE_STRUCT( THuiCanvasLoadIdentityParams )
	{
	TInt iDummy;
	};
	
NONSHARABLE_STRUCT( THuiCanvasTranslateParams )
	{
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};

NONSHARABLE_STRUCT( THuiCanvasScaleParams )
	{
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};

NONSHARABLE_STRUCT( THuiCanvasRotateParams )
	{
	TReal32 iAngle;
	TReal32 iX; 
	TReal32 iY; 
	TReal32 iZ;
	};



/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS(CHuiCanvasAlfPainter) : public CHuiCanvasPainter
    {
		
		private:
		/**
     * Constructor.
     */
    CHuiCanvasAlfPainter();

    /**
     * Second-phase constructor.
     */
    void ConstructL();


		public:
    /**
     * Constructor.
     */
    static CHuiCanvasAlfPainter* NewL();

    /**
     * Destructor.
     */
    ~CHuiCanvasAlfPainter();


		public: // From CHuiCanvasPainter
		
		/**
     * Sets buffer that contains canvas drawing commands. This
     * method clears previous buffer(s).
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
     */
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos ) ;
    virtual void HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos, TPtrC8 aCommands, CHuiCanvasCommandBuffer *buf) { }

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
    virtual TInt SetCapturingBufferL(CFbsBitmap* /*aTarget*/);

    /**
     * Tells painter to enable/disable render buffer (if it is supported by the
     * painter implementation) 
     *
     * @return KErrNotSupported if the current renderer/painter does not support the
     */
    virtual TInt EnableRenderBuffer(TBool aEnable = ETrue);
    
	virtual CHuiCanvasGc& CanvasGc() const;

private:
    void ClearHuiTextCache();
       
private:
      // Hui command buffer handling
    RArray<THuiCanvasDrawTextCachedTextEntry> iCachedTexts;
    RArray<TReal32> iPointCords;
    CHuiCanvasGc* iCanvasGc;
    THuiRealRect iPaintedRect;
};    
#endif  // __HUICANVASALFPAINTER_H__
