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
* Description:   Drop shadow brush
*
*/



#ifndef C_ALFDROPSHADOWBRUSH_H
#define C_ALFDROPSHADOWBRUSH_H

#include <alf/alfbrush.h>

class CAlfEnv;
struct TAlfMetric; 
class TRgb;

/**
 *  Drop shadow brush.
 *  Drop shadow brush draws a shadow to a configurable depth. 
 *  Usage:
 *  @code
 *   // Creates a drop shadow brush and adds it to a visual.
 *   CAlfDropShadowBrush* dropShadowBrush = CAlfDropShadowBrush::NewL( *iEnv, TAlfMetric(5) );
 *   visualWithBorders->EnableBrushesL();
 *   visualWithBorders->Brushes()->AppendL( dropShadowBrush, EAlfHasOwnership )
 *   //Cleanup
 *   dropShadowBrush = 0; // ownership transferred to the brush array
 *  @endcode 
 *  @lib alfclient.dll
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfDropShadowBrush ): public CAlfBrush
    {

public:

    /**
     * Constructor.
     * @param aEnv Environment
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C static CAlfDropShadowBrush* NewL(
        CAlfEnv& aEnv,
        const TAlfMetric& aDepth );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aEnv Environment
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C static CAlfDropShadowBrush* NewLC(
        CAlfEnv& aEnv,
        const TAlfMetric& aDepth );

    /**
     * Constructor.
     * @deprecated
     * @param aEnv Environment
     * @param aDepth Shadow depth in pixels.
     */
    IMPORT_C static CAlfDropShadowBrush* NewL(
        CAlfEnv& aEnv,
        TInt aDepth );

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated
     * @param aEnv Environment
     * @param aDepth Shadow depth in pixels.
     */
    IMPORT_C static CAlfDropShadowBrush* NewLC(
        CAlfEnv& aEnv,
        TInt aDepth );
    
    /**
     * Destructor.
     */
    virtual ~CAlfDropShadowBrush();
    
    /**
     * Gets minimum width of soft edges.
     * @return Shadow minumum width magnitude in current metric units.
     */
    IMPORT_C const TAlfTimedValue& Softness() const;

    /**
     * Sets minimum width of soft edges.
     * @param aSoftness Shadow minumum width magnitude in current metric units.
     */
    IMPORT_C void SetSoftness( const TAlfTimedValue& aSoftness );

    /**
     * Sets minimum width of soft edges.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C void SetSoftness(const TAlfMetric& aSoftness);

    /**
     * Gets shadow depth.
     * @return Shadow depth in current metric units.
     */
    IMPORT_C const TAlfTimedValue& Depth() const;
    
    /**
     * Sets shadow depth.
     * @param aDepth Shadow depth magnitude in current metric units.
     */
    IMPORT_C void SetDepth( const TAlfTimedValue& aDepth );

    /**
     * Sets shadow depth.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C void SetDepth(const TAlfMetric& aDepth);

    /**
     * Sets shadow color.
     * @param aColor Shadow color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

private:


    CAlfDropShadowBrush();

    void ConstructL( CAlfEnv& aEnv, TInt aDepth );

    void ConstructL( CAlfEnv& aEnv, const TAlfMetric&  aDepth );

private:

    struct TDropShadowPrivateData;
    TDropShadowPrivateData* iDropShadowData;

    };



#endif // C_ALFDROPSHADOWBRUSH_H
