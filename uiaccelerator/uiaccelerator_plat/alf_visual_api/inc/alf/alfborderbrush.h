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
* Description:   Border brush definition
*
*/



#ifndef C_ALFBORDERBRUSH_H
#define C_ALFBORDERBRUSH_H

#include <alf/alfbrush.h>

class CAlfEnv;
class TRgb;
class TAlfImage;
struct TAlfXYMetric;

/**
 * Border brush draws borders for visuals.
 * Border brush draws a border at a configurable distance. It can be inside
 * or outside the visual edges. The horizontal and vertical edges can be
 * configured separately. The border can either be a solid color or
 * texture-mapped. A texture offset is supported for animated texture mapping.
 *  
 *  Usage:
 *  @code
 * 
 *   // Creates a 2-unit pixel-border brush and adds it to a visual.
 *   CAlfBorderBrush* borderBrush = CAlfBorderBrush::NewL( *iEnv, TAlfXYMetric(TAlfMetric(2)),
 *              TAlfXYMetric(TAlfMetric(2, EAlfUnitPixel),
 *              TAlfMetric(2, EAlfUnitPixel)));
 * 	//Enable brush
 *   visualWithBorders->EnableBrushesL();
 * 
 * 	//Add border brush in brush array, with ownership trnasfered to brush array
 *  visualWithBorders->Brushes()->AppendL( borderBrush, EAlfHasOwnership );
 * 
 *  //Cleanup
 *   borderBrush = 0; // ownership transferred to the brush array
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfBorderBrush ) : public CAlfBrush
    {

public:
 
    /**
     * Constructor.
     * @param aEnv Environment
     * @param aThickness Border width in specified metric units.
     * @param aEdgeOffset Border edge offset in specified metric units.
     *
     */
    IMPORT_C static CAlfBorderBrush* NewL(
        CAlfEnv& aEnv,
        const TAlfXYMetric& aThickness,
        const TAlfXYMetric& aEdgeOffset);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aEnv Environment
     * @param aThickness Border width in specified metric units.
     * @param aEdgeOffset Border edge offset in specified metric units.
     */
    IMPORT_C static CAlfBorderBrush* NewLC(
        CAlfEnv& aEnv,
        const TAlfXYMetric& aThickness,
        const TAlfXYMetric& aEdgeOffset);

    /**
     * Constructor.
     * @deprecated
     * @param aEnv Environment
     * @param aThicknessWidth Border width in pixels.
     * @param aThicknessHeight Border height in pixels.
     * @param aEdgeOffsetX Edge offset X coordinate in pixels    
     * @param aEdgeOffsetY Edge offset Y coordinate in pixels    
     *
     */
    IMPORT_C static CAlfBorderBrush* NewL(
        CAlfEnv& aEnv,
        TInt aThicknessWidth, 
        TInt aThicknessHeight,
        TInt aEdgeOffsetX, 
        TInt aEdgeOffsetY );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated
     * @param aEnv Environment
     * @param aThicknessWidth Border width in pixels.
     * @param aThicknessHeight Border height in pixels.
     * @param aEdgeOffsetX Edge offset X coordinate in pixels    
     * @param aEdgeOffsetY Edge offset Y coordinate in pixels    
     */
    IMPORT_C static CAlfBorderBrush* NewLC(
        CAlfEnv& aEnv,
        TInt aThicknessWidth, 
        TInt aThicknessHeight,
        TInt aEdgeOffsetX, 
        TInt aEdgeOffsetY );
    
    /**
     * Destructor.
     */
    virtual ~CAlfBorderBrush();
    
    /**
     * Sets an image for the borders.
     *
     * @param aImage  Texture image for the borders.
     */
    IMPORT_C void SetImage(const TAlfImage& aImage);
    
    /**
     * Returns the border image.
     */
    IMPORT_C const TAlfImage& Image() const;

    /**
     * Sets a color for the borders.
     *
     * @param aColor Border color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);
    
    /**
     * Gets border thickness in currently used metric units.
     *
     * @return Border thickness
     */
    IMPORT_C const TAlfTimedPoint& Thickness() const;

    /**
     * Sets border thickness in currently used metric units.
     *
     * @param aThickness Border thickness
     */
    IMPORT_C void SetThickness(const TAlfTimedPoint& aThickness);

    /**
     * Sets border thickness and used metric unit type.
     *
     * @param aThickness Border thickness including metric unit type.
     */
    IMPORT_C void SetThickness(const TAlfXYMetric& aThickness);
    
    /**
     * Gets edge offset in currently used metric units.
     *
     * @return Border edge offset
     */
    IMPORT_C const TAlfTimedPoint& EdgeOffset() const;

    /**
     * Sets edge offset in currently used metric units.
     *
     * @param aEdgeOffset Border edge offset.
     */
    IMPORT_C void SetEdgeOffset(const TAlfTimedPoint& aEdgeOffset);

    /**
     * Sets edge offset and used metric unit type.
     *
     * @param aEdgeOffset Border edge offset including metric unit type.
     */
    IMPORT_C void SetEdgeOffset(const TAlfXYMetric& aEdgeOffset);
    
    /**
     * Gets image offset in currently used metric units.
     *
     * @return Image offset
     */
    IMPORT_C const TAlfTimedPoint& ImageOffset() const;

    /**
     * Sets image offset in currently used metric units.
     *
     * @param aImageOffset Image offset.
     */
    IMPORT_C void SetImageOffset(const TAlfTimedPoint& aImageOffset);

    /**
     * Sets image offset and used metric unit type.
     *
     * @param aImageOffset Image offset including metric unit type.
     */
    IMPORT_C void SetImageOffset(const TAlfXYMetric& aImageOffset);

private:

    /**
     * 1st pahse constructor.
     */
    CAlfBorderBrush();

    /**
     * 2nd phase constructor.
     */
    void ConstructL(
        CAlfEnv& aEnv,
        const TAlfXYMetric& aThickness,
        const TAlfXYMetric& aEdgeOffset);

    /**
     * 2nd phase constructor.
     */
    void ConstructL(
        CAlfEnv& aEnv,
        TInt aThicknessWidth, 
        TInt aThicknessHeight,
        TInt aEdgeOffsetX, 
        TInt aEdgeOffsetY);
        
private:

    struct TBorderBrushPrivateData;
    TBorderBrushPrivateData* iBorderBrushData;

    };



#endif // C_ALFBORDERBRUSH_H
