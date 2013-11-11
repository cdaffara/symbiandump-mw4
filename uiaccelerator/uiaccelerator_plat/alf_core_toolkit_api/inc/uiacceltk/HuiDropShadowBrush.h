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
* Description:   ?Description
*
*/



#ifndef __HUIDROPSHADOWBRUSH_H__
#define __HUIDROPSHADOWBRUSH_H__


#include <e32base.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/huimetric.h>


/**
 * Drop shadow is drawn behind the visual, outside of its edges.
 *
 */
NONSHARABLE_CLASS(CHuiDropShadowBrush) : public CHuiBrush
    {
public:
   
    /* Constructors and destructor. */

    /**
     * Constructor.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C static CHuiDropShadowBrush* NewL(const THuiMetric& aDepth);
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C static CHuiDropShadowBrush* NewLC(const THuiMetric& aDepth);

    /**
     * Constructor.
     * @deprecated
     * @param aDepth Shadow depth in pixels.
     */
    IMPORT_C static CHuiDropShadowBrush* NewL(TInt aDepth);
    
    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated
     * @param aDepth Shadow depth in pixels.
     */
    IMPORT_C static CHuiDropShadowBrush* NewLC(TInt aDepth);

    /**
     * Sets shadow depth.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C void SetDepth(const THuiMetric& aDepth);

    /**
     * Sets minimum width of soft edges.
     * @param aDepth Shadow depth in metric units.
     */
    IMPORT_C void SetSoftness(const THuiMetric& aSoftness);

    /**
     * Sets shadow color.
     * @param aColor Shadow color.
     */
    IMPORT_C void SetColor(const TRgb& aColor);

    /**
     * Destructor.
     */
    ~CHuiDropShadowBrush();


    /* Methods. */
    
    /**
     * From CHuiBrush.
     * @see CHuiBrush::Changed()
     */
    TBool Changed() const;
    
    /**
     * From CHuiBrush.
     * @see CHuiBrush::ClearChanged()
     */
    void ClearChanged();
        
    /**
     * Draw the brush.
     */
    void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;
    
    void ExpandVisualRect(TRect& aRect) const;
                      

protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiDropShadowBrush(TInt aDepth);

    /**
     * Constructor.
     */
    CHuiDropShadowBrush(const THuiMetric& aDepth);


private:

    /* Private methods */
    TReal32 DepthInPixels(const CHuiVisual* aVisual) const;
    TReal32 SoftnessInPixels(const CHuiVisual* aVisual) const;
    TReal32 ValueInPixels(const CHuiVisual* aVisual, const THuiTimedValue& aValue) const;

public:

    /* Public properties. */

    /** Minimum width of soft edges. */
    THuiTimedValue iSoftness;

    /** Depth of the shadow. */
    THuiTimedValue iDepth;

private:

    /** Metric for depth of the shadow. */
    THuiMetric iDepthMetric;

    /** Metric for the shadow softness */
    THuiMetric iSoftnessMetric;
    
    /** Color for depth of the shadow. */
    TRgb iColor;
    };

#endif  // __HUIDROPSHADOWBRUSH_H__
