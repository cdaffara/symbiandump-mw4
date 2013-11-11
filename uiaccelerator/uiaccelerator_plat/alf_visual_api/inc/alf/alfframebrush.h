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
* Description:   Frame brush definition
*
*/



#ifndef C_ALFFRAMEBRUSH_H
#define C_ALFFRAMEBRUSH_H

#include <AknsItemID.h>
#include <alf/alfbrush.h>

struct TAlfXYMetric;
class TAlfImage;

/**
 * Frame brush.
 * FrameBrush draws a 1-part or 9-part frame around a visual using bitmaps provided by the
 * theme. The constructor takes a theme item identifier as a parameter. The bitmaps are rasterized
 * when CAlfFrameBrush::SetFrameRects() is called. 
 * Develper must call this method and specify the dimensions of the frame before the frame can be drawn.
 * 
 *  Usage:
 *  @code
 *  //Create framebrush instance
 *  CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewL( *iEnv, *image, TAlfXYMetric( TAlfMetric( 5 ) ) );
 * 
 *  //Create rect to set for framebrush
 *  TRect innerRect( TPoint( 10, 10 ), TPoint( 20, 20 ) );
 *  TRect outerRect( TPoint( 0, 0 ), TPoint( 30, 30 ) );
 * 
 *  //Set dimentions for framebrush
 * 	frameBrush->SetFrameRects( innerRect, outerRect );
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */

NONSHARABLE_CLASS( CAlfFrameBrush ): public CAlfBrush
    {
public:

    enum TFrameParts
        {
        EFramePartCornerTl = 0,
        EFramePartCornerTr,
        EFramePartCornerBl, 
        EFramePartCornerBr,
        EFramePartSideT,
        EFramePartSideB,
        EFramePartSideL,
        EFramePartSideR,
        EFramePartCenter
        };

    /**
     * Constructor. 
     * @param aEnv Environmnet
     * @param aImage Image for the frame. Image will be used as such, 
     * framerects (if set) are ignored.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewL(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset);
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aEnv Environmnet
     * @param aImage Image for the frame. Image will be used as such, 
     * framerects (if set) are ignored.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewLC(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset);

    /**
     * Constructor. 
     * @param aEnv Environmnet
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewL(CAlfEnv& aEnv, const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset);
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aEnv Environmnet
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewLC(CAlfEnv& aEnv, const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset);

    /**
     * Constructor. 
     * @deprecated
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffsetX X coordinate of the offset in pixels.
     * @param aEdgeOffsetY Y coordinate of the offset in pixels.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewL(CAlfEnv& aEnv, const TAknsItemID& aIID, TInt aEdgeOffsetX = 0, TInt aEdgeOffsetY = 0);
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffsetX X coordinate of the offset in pixels.
     * @param aEdgeOffsetY Y coordinate of the offset in pixels.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CAlfFrameBrush* NewLC(CAlfEnv& aEnv, const TAknsItemID& aIID, TInt aEdgeOffsetX = 0, TInt aEdgeOffsetY = 0);
        
    /**
     * Sets frame rects.
     * @param aInnerRect Inner rect of the frame
     * @param aOuterRect Outer rect of the frame
     */
    IMPORT_C void SetFrameRectsL(const TRect& aInnerRect, const TRect& aOuterRect);

    /**
     * Sets edge offset.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     */
    IMPORT_C void SetEdgeOffset(const TAlfXYMetric& aEdgeOffset);

    /**
     * Sets whole frame image. Image will be used as such, framerects (if set) are ignored.
     * @param aImage Image for the frame.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage);

    /**
     * Sets framepart image. Whole frame will be constrcuted from 9 parts, each part has
     * to be set separately using this method. Genrally using 9-piece frame is not as
     * efficient as 1-pice frame, so this method should be used only when really needed.
     * @param aFramePart Frame part to which the image is intended.
     * @param aImage Image for the frame part.
     */
    IMPORT_C void SetImage(TFrameParts aFramePart, const TAlfImage& aImage);

    /**
     * Destructor. 
     */
    virtual ~CAlfFrameBrush();

private:

    CAlfFrameBrush(const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY);

    CAlfFrameBrush(const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset);
   
    CAlfFrameBrush();
   
    void ConstructL(CAlfEnv& aEnv, const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY);

    void ConstructL(CAlfEnv& aEnv, const TAknsItemID& aIID, const TAlfXYMetric& aEdgeOffset);

    void ConstructL(CAlfEnv& aEnv, const TAlfImage& aImage, const TAlfXYMetric& aEdgeOffset);

private:

    // Private data. Owned.
    struct TFrameBrushPrivateData;
    TFrameBrushPrivateData* iFrameBrushData;
    };

#endif 