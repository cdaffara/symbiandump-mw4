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
* Description:   Defines a CHuiFrameBrush class - a brush that draws a frame element
*
*/



#ifndef __HUIFRAMEBRUSH_H__
#define __HUIFRAMEBRUSH_H__


#include <e32base.h>
#include <AknsItemID.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/huimetric.h>

class CHuiImageLoaderUtil;

/**
 *  Frame brush
 *
 *  @lib hitchcock.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CHuiFrameBrush): public CHuiBrush, public MHuiActionObserver
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
     * NewL. Creates new instance of CHuiFrameBrush.
     *
     * @param aImage Image for the frame. Image will be used as such, 
     * framerects (if set) are ignored.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewL(
        const THuiImage& aImage,
        const THuiXYMetric& aEdgeOffset);
    
    /**
     * NewLC. Creates new instance of CHuiFrameBrush and leaves
     * it onto the cleanup stack.
     *
     * @param aImage Image for the frame. Image will be used as such, 
     * framerects (if set) are ignored.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewLC(
        const THuiImage& aImage,
        const THuiXYMetric& aEdgeOffset);


    /**
     * NewL. Creates new instance of CHuiFrameBrush.
     *
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewL(
        const TAknsItemID& aIID,
        const THuiXYMetric& aEdgeOffset);
    
    /**
     * NewLC. Creates new instance of CHuiFrameBrush and leaves
     * it onto the cleanup stack.
     *
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewLC(
        const TAknsItemID& aIID,
        const THuiXYMetric& aEdgeOffset);

    /**
     * NewL. Creates new instance of CHuiFrameBrush.
     *
     * @deprecated
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffsetX X coordinate of the offset in pixels.
     * @param aEdgeOffsetY Y coordinate of the offset in pixels.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewL(
        const TAknsItemID& aIID,
        TInt aEdgeOffsetX = 0, 
        TInt aEdgeOffsetY = 0 );
    
    /**
     * NewLC. Creates new instance of CHuiFrameBrush and leaves
     * it onto the cleanup stack.
     *
     * @deprecated     
     * @param aIID Skin ID for the frame.
     * @param aEdgeOffsetX X coordinate of the offset in pixels.
     * @param aEdgeOffsetY Y coordinate of the offset in pixels.
     * @return New instance. Onwership tranferred to the caller.
     */
    IMPORT_C static CHuiFrameBrush* NewLC(
        const TAknsItemID& aIID,
        TInt aEdgeOffsetX = 0, 
        TInt aEdgeOffsetY = 0);

    /**
     * Destructor
     */
    ~CHuiFrameBrush();

    
    /**
     * Sets the frame rectangles which define the nine frame areas.
     *
     * @param aInnerRect Inner rect (pixels) in where the EFramePartCenter is drawn.
     * @param aOuterRect Outter rect (pixels).
     */
    IMPORT_C void SetFrameRectsL(const TRect& aInnerRect, const TRect& aOuterRect);

    /**
     * Sets edge offset.
     * @param aEdgeOffset Coordinates of the offset in metric units.
     */
    IMPORT_C void SetEdgeOffset(const THuiXYMetric& aEdgeOffset);

    /**
     * Sets frame image. Whole frame will be constrcuted from this image.
     * @param aImage Image for the frame.
     */
    IMPORT_C void SetImage(const THuiImage& aImage);

    /**
     * Sets framepart image. Whole frame will be constrcuted from 9 parts, each part has
     * to be set separately using this method. Genrally using 9-piece frame is not as
     * efficient as 1-pice frame, so this method should be used only when really needed.
     * @param aFramePart Frame part to which the image is intended.
     * @param aImage Image for the frame part.
     */
    IMPORT_C void SetImage(TFrameParts aFramePart, const THuiImage& aImage);

    /**
     * From CHuiBrush.
     * @see CHuiBrush::ExpandVisualRect(TRect& aRect)
     */
    void ExpandVisualRect(TRect& aRect) const;
        
    /**
     * From CHuiBrush.
     * @see CHuiBrush::Changed() const
     */
    TBool Changed() const;
    
    /**
     * From CHuiBrush.
     * @see CHuiBrush::ClearChanged()
     */
    void ClearChanged();

    /**
     * From CHuiBrush.
     * @see CHuiBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;
     */
    void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;

protected:

    CHuiFrameBrush(const THuiImage& aImage, const THuiXYMetric& aEdgeOffset);
    CHuiFrameBrush(const TAknsItemID& aIID, const THuiXYMetric& aEdgeOffset);
    CHuiFrameBrush(const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY);
    CHuiFrameBrush();
    
    // from MHuiActionObserver
    void HandleActionL(const THuiActionCommand& aActionCommand);
       
private:
    TSize GetFramePartSize(const TFrameParts aFramePart) const;
    void ConstructFrameL();
    static void ResetAndDestroyImageArray( TAny* aAny );
    THuiRealPoint PointInPixels(const CHuiVisual* aVisual, const THuiTimedPoint& aPoint, const THuiXYMetric& aMetric) const;
    void InitImagesL(TInt aNumberOfImages);
    void ConstructL(const THuiImage& aImage);
    TRect AdjustedOuterRect(const TSize& aFullFrameSize) const;
    TRect AdjustedInnerRect(const TSize& aFullFrameSize) const;
private:
    TAknsItemID iFrameIID;
    TRect iInnerRect;
    TRect iOuterRect;
    
    NONSHARABLE_CLASS(CHuiImageWithID): public CBase
        {
        public:
          CHuiImageWithID(THuiImage aImage, TInt aId):iImage(aImage),iId(aId){}
          ~CHuiImageWithID();
          const MHuiSegmentedTexture& Texture(); 
          THuiImage& Image(); 
          void SetImage(const THuiImage& aImage);   
        private:
          THuiImage iImage;
          TInt iId;
        };
        
    RPointerArray<CHuiImageWithID> iImages;	
    CHuiImageLoaderUtil* iImageloader;
    
public:
    THuiTimedPoint iEdgeOffset;
private:
    THuiXYMetric iEdgeOffsetMetric;
    TBool iEnvReleased;
    TBool iSkinChanged;
    };

#endif 