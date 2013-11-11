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
* Description:   Defines CHuiShadowBorderBrush - Shadow borders for visuals.
*
*/



#ifndef __HUISHADOWBORDERBRUSH_H__
#define __HUISHADOWBORDERBRUSH_H__


#include <e32base.h>
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/huimetric.h>


/**
 * Draws shadow borders for visuals using the skin shadow texture.
 * The shadow borders are drawn outside the edges of a visual.
 *
 * @todo  Should affect the visual's dirty region by growing it?
 */
NONSHARABLE_CLASS(CHuiShadowBorderBrush) : public CHuiBrush
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructor.
     * @param aWidth Border width in given metric units
     */
    IMPORT_C static CHuiShadowBorderBrush* NewL(const THuiMetric& aWidth);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @param aWidth Border width in given metric units
     */
    IMPORT_C static CHuiShadowBorderBrush* NewLC(const THuiMetric& aWidth);

    /**
     * Constructor.
     * @deprecated migrate to using metrics instead.
     * @param aWidth Border width in pixels
     */
    IMPORT_C static CHuiShadowBorderBrush* NewL(TInt aWidth);

    /**
     * Constructor. The new brush is left on the cleanup stack.
     * @deprecated migrate to using metrics instead.
     * @param aWidth Border width in pixels
     */
    IMPORT_C static CHuiShadowBorderBrush* NewLC(TInt aWidth);

    /**
     * Sets width of shadow border.
     * @param aWidth Border width in given metric units.
     */
    IMPORT_C void SetWidth(const THuiMetric& aWidth);

    /**
     * Destructor.
     */
    virtual ~CHuiShadowBorderBrush();


    /* Methods. */

    void ExpandVisualRect(TRect& aRect) const;
    
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
    virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;


protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiShadowBorderBrush(TInt aWidth);

    /**
     * Constructor.
     */
    CHuiShadowBorderBrush(const THuiMetric& aWidth);

private:

    TReal32 WidthInPixels(const CHuiVisual* aVisual) const;
    
public:

    /* Public properties. */

    /** Border width.*/
    THuiTimedValue iWidth;

private:

    /** Border width in independed metric units */
    THuiMetric iMetricWidth;
    
    };

#endif  // __HUISHADOWBORDERBRUSH_H__
