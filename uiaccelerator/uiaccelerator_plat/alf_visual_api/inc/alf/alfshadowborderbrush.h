/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Shadowed border brush
*
*/



#ifndef C_ALFSHADOWBORDERBRUSH_H
#define C_ALFSHADOWBORDERBRUSH_H

#include <alf/alfbrush.h>
#include <alf/alfmetric.h>

class CAlfEnv;

/**
 *  Shadowed border brush
 *
 * A brush that creates a shadow border around its visual.
 * When comparing a shadow border brush to a drop shadow brush, the main difference is that 
 * the shadow border brush draws only the borders, while the drop shadow brush fills 
 * the entire shadow area.
 * 
 * The shadow border brush draws the shadow on the outside of the visual rectangle. 
 * One can conveniently combine it with a regular border brush.
 * 
 * Usage:
 * @code
 * 
 * //Create ImageVisual and set image
 * CAlfImageVisual* imagevisual = CAlfImageVisual::AddNewL( control );
 * imagevisual->SetImage(*image);
 * imagevisual->SetFlags(EAlfVisualFlagManualLayout);
 * 
 * //Manually set up an image visual to be 2/3 of the original size of the image 
 * Size texSize = image->Texture().Size();
 * imagevisual->SetRect(TRect(Point(50, 50),TSize(texSize.iWidth*2/3, texSize.iHeight*2/3)));
 * imagevisual->EnableBrushesL();
 * 
 * //Create shadow border brush
 * CAlfShadowBorderBrush* brush = CAlfShadowBorderBrush::NewL(iEnv, TAlfMetric( 0.25 ) );
 * brush->SetOpacity(0.25);
 * imagevisual->Brushes()->AppendL(brush, EAlfHasOwnership);
 *
 * @endcode
 *
 *  @lib alfclient.lib
 *  @since S60 v 3.2
 */
NONSHARABLE_CLASS( CAlfShadowBorderBrush ): public CAlfBrush
    {

public:

    /**
     * Constructor.
     *
     * @param aEnv Environment
     * @param aWidth Shadow border width in given metric units.
     */
    IMPORT_C static CAlfShadowBorderBrush* NewL(
        CAlfEnv& aEnv,
        const TAlfMetric& aWidth );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     *
     * @param aEnv Environment
     * @param aWidth Shadow border width in given metric units.
     */
    IMPORT_C static CAlfShadowBorderBrush* NewLC(
        CAlfEnv& aEnv,
        const TAlfMetric& aWidth );

    /**
     * Constructor.
     * @deprecated migrate to using metrics instead.     
     */
    IMPORT_C static CAlfShadowBorderBrush* NewL(
        CAlfEnv& aEnv,
        TInt aWidth );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated migrate to using metrics instead.     
     */
    IMPORT_C static CAlfShadowBorderBrush* NewLC(
        CAlfEnv& aEnv,
        TInt aWidth );

    
    /**
     * Destructor.
     */
    virtual ~CAlfShadowBorderBrush();
    
    /**
     * Gets width of the shadow border.
     * @return Width magnitude of the border in metric units.
     */
    IMPORT_C const TAlfTimedValue& Width() const;

    /**
     * Sets width of the shadow border.
     * @param aWidth Width magnitude of the border in metric units.
     */
    IMPORT_C void SetWidth( const TAlfTimedValue& aWidth );

    /**
     * Sets width of the shadow border.
     * @param aWidth Width of the border in metric units.
     */
    IMPORT_C void SetWidth( const TAlfMetric& aWidth );


private:


    CAlfShadowBorderBrush();

    void ConstructL( CAlfEnv& aEnv, TInt aWidth );

    void ConstructL( CAlfEnv& aEnv, const TAlfMetric& aWidth );

private:

    struct TShadowBorderPrivateData;
    TShadowBorderPrivateData* iShadowBorderData;

    };



#endif // C_ALFSHADOWBORDERBRUSH_H
