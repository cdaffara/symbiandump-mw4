/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Drop shadow handler
*
*/



#ifndef C_HUIDROPSHADOW_H
#define C_HUIDROPSHADOW_H

#include <gdi.h>
#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiRealRect.h>
#include <uiacceltk/HuiTimedPoint.h>
#include <uiacceltk/HuiTextureHandle.h>
#include <uiacceltk/huimetric.h>

class TAknsItemID;
class MHuiSegmentedTexture;
class CHuiTexture;
class CHuiVisual;

/**
 *  Drop shadow parameter handler
 *
 *  This interface can be fetched from the visual
 *  @see CHuiVisual::EnableDropShadowL
 *  @see CHuiVisual::DropShadowHandler
 *  
 *  @code
 *   // get the pointer
 *   visual->EnableDropShadowL();
 *   CHuiDropShadow* dropShadow = visual->DropShadowHandler();
 *
 *   // use it
 *   dropShadow->SetColor( KRgbRed );
 *   dropShadow->iOffset.Set( THuiRealPoint( 10, 10 ) );
 *  @endcode
 *
 *  Disabling the drop shadow from the visual, deletes the instance and therefore loses
 *  all set paramteres. If you want to just hide the drops shadow temporally, you can 
 *   a) set the opacity into 0 or
 *   b) set the scale into 0
 *
 *  @lib hitchcock.lib
 *  @since S60 v5.0.1
 */
NONSHARABLE_CLASS( CHuiDropShadow ) : public CBase
    {
public: // Exported functions
     
    /**
     * Sets the shadow in polar coordinates. This will be converted into xy
     * coordinates
     *
     * @param aAngle Angle of the shadow. 0 degrees is on the right hand side and 90 in directly above.
     * @param aDistance Distance of the shadow in unit defined by the user component. 
     * @param aTransitionTime Time reach the target.
     */ 
    IMPORT_C void SetOffset( TReal32 aAngle, THuiMetric aDistance, TInt aTransitionTime = 0 ) __SOFTFP;
        
    /**
     * Sets the color of the shadow.
     *
     * @param aColor Text color.
     * @param aTransitionTime Time reach the target. (not supported )
     */
    IMPORT_C void SetColor(const TRgb& aColor, TInt aTransitionTime = 0 );

     /**
      * Sets the color of the shadow via skin id.
      *
      * @param aID      Skin ID of the color group to use
      * @param aIndex   Index within the color group
      * @param aTransitionTime Time reach the target. (not supported )
      */
     IMPORT_C void SetColor(const TAknsItemID& aID,const TInt aIndex, TInt aTransitionTime = 0 );
     
public: // internal methods

    CHuiDropShadow();
    void ConstructL();
    ~CHuiDropShadow();
    
    TBool Changed() const;
    void ClearChanged();
    
    TRgb Color() const;
    
    /**
     * Calculates the drawing size of the shadow texture
     *
     * @param aImageDrawingSize     Image drawing size
     * @param aShadowTextureSize    Shadow texture size
     *
     * @return Shadow drawing size
     */
    THuiRealSize ShadowDrawingSize( 
        const TSize& aImageDrawingSize, 
        const TSize& aShadowTextureSize ) const;
        
    /**
     * Checks if the shadow is visible (checks opacity and scale != 0)
     * 
     * @return ETrue if visible.
     */ 
    TBool IsShadowVisible() const;
    
    /**
     * Casts 'const MHuiTexture' into 'CHuiTexture' if applicaple.
     *
     * @param aTextureIf Original texture interface
     *
     * @return Casted to CHuiTexture instance if possible.
     */
    static CHuiTexture* ShadowedTextureInstance( const MHuiTexture& aTextureIf );
    
    /**
     * Calculates the shadow distance in pixels.
     *
     * @param aReferenceVisual Visual which determines the metrics.
     *
     * @return Shadow distance in pixels.
     */
    THuiRealPoint DistanceInPixels( const CHuiVisual& aReferenceVisual ) const;
    
    /**
     * Returns the rectangle in which the shadow is supposed to be drawn.
     * Area is given as real/floating values.
     *
     * @param aImageDrawingPosition Position where the image is drawn
     * @param aImageDrawingSize Size of the image to be drawn
     * @param aShadowTextureSize Size of the shadow texture
     * @prarm aReferenceVisual Reference visual for the relative metrics.
     *
     * @return Area used by the shadow.
     */
    THuiRealRect ShadowDrawingRealRect( 
        const TPoint& aImageDrawingPosition,
        const TSize& aImageDrawingSize, 
        const TSize& aShadowTextureSize,
        const CHuiVisual& aReferenceVisual  ) const;
    
    /**
     * Returns the rectangle in which the shadow is supposed to be drawn.
     * Area is given as integer/fixed values.
     *
     * @param aImageDrawingPosition Position where the image is drawn
     * @param aImageDrawingSize Size of the image to be drawn
     * @param aShadowTextureSize Size of the shadow texture
     * @prarm aReferenceVisual Reference visual for the relative metrics.
     *
     * @return Area used by the shadow.
     */    
    TRect ShadowDrawingTRect( 
        const TPoint& aImageDrawingPosition,
        const TSize& aImageDrawingSize, 
        const TSize& aShadowTextureSize,
        const CHuiVisual& aReferenceVisual  ) const;
    
public: // public members

    /**
     * Opacity of the shadow
     */ 
    THuiTimedValue iOpacity;
    
    /**
     * Shadow offset in timed xy-coordinates
     */
    THuiTimedPoint iOffset;
    
    /**
     * Shadow offset unit
     */
    TInt iOffsetUnit;
    
    /**
     * Sets the blur filter radius. This is used to generate the shadow from the
     * texture. Normally between 2-5 pixels (in texture coordinates).
     */
    THuiTimedValue iRadius;
    
    /**
     * Scale of the shadow - default 1.0.
     */ 
    THuiTimedValue iScale;
   
private:

    struct THuiDropShadowData;
    THuiDropShadowData* iData;

    };


#endif // C_HUIDROPSHADOW_H
