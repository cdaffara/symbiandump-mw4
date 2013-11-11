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
* Description:   Drop shadow parameter handler
*
*/



#ifndef C_ALFDROPSHADOW_H
#define C_ALFDROPSHADOW_H

#include <gdi.h>
#include <alf/alftimedvalue.h>
#include <alf/alfmetric.h>

class TAknsItemID;
class CAlfGenComponent;

/**
 *  Drop shadow parameter handler
 *
 *  This interface can be fetched from the visual
 *  @see CAlfVisual::EnableDropShadowL
 *  @see CAlfVisual::DropShadowHandler
 *  
 *  @code
 *   // get the pointer
 *   visual->EnableDropShadowL();
 *   CAlfDropShadow* dropShadow = visual->DropShadowHandler();
 *
 *   // use it
 *   dropShadow->SetColor( KRgbRed );
 *   dropShadow->SetOffset( TAlfTimedPoint( 10, 10 ) );
 *  @endcode
 *
 *  Disabling the drop shadow from the visual, deletes the instance and therefore loses
 *  all set paramteres. If you want to just hide the drops shadow temporally, you can 
 *   a) set the opacity into 0 or
 *   b) set the scale into 0
 *
 *  @lib alfclient.lib
 *  @since S60 v5.0.1
 */
NONSHARABLE_CLASS( CAlfDropShadow ) : public CBase
    {
public: // Exported functions
     
    /**
     * Sets the shadow offset in polar coordinates. This will be converted into xy
     * coordinates
     *
     * @param aAngle Angle of the shadow. 0 degrees is on the right hand side and 90 in directly above.
     * @param aDistance Distance of the shadow in unit defined by the user component. 
     * @param aTransitionTime Time reach the target.
     */ 
    IMPORT_C void SetOffset( TReal32 aAngle, TAlfMetric aDistance, TInt aTransitionTime = 0 ) __SOFTFP;
    
    /**
     * Sets the shadow offset in cartesian (XY) coordinates.
     *
     * @param aOffset Position offset.
     * @param aOffsetUnit Used unit, see TAlfUnit from alfmetric.h
     *                    KErrNotFound uses previous/defaut base unit.
     */ 
    IMPORT_C void SetOffset( const TAlfTimedPoint& aOffset, TInt aOffsetUnit = KErrNotFound );
        
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
     
    /**
     * Sets the shadow opacity. The visual's opactity will be multiplied with this value.
     *
     * @param aOpacity Opacity of the drop shadow.
     */
    IMPORT_C void SetOpacity( const TAlfTimedValue& aOpacity );
    
    /**
     * Sets the blur filter radius. This is used to generate the shadow from the
     * texture. Normally between 2-5 pixels (in texture coordinates).
     *
     * @param aRadius Blur filter radius.
     */
    IMPORT_C void SetRadius( const TAlfTimedValue& aRadius );
    
    /**
     * Scale of the shadow - default 1.0.
     *
     * @param aScale Scale factor.
     */ 
    IMPORT_C void SetScale( const TAlfTimedValue& aScale );

     
public: // internal methods

    /**
     * @internal
     */ 
    static CAlfDropShadow* NewL(CAlfGenComponent& aComms);
    
    /**
     * @internal
     */ 
    ~CAlfDropShadow();
    
private:

    /**
     * @internal
     */ 
    CAlfDropShadow();
    
    /**
     * @internal
     */ 
    void ConstructL(CAlfGenComponent& aComms);
   
private:

    struct TAlfDropShadowData;
    TAlfDropShadowData* iData;

    };


#endif // C_ALFDROPSHADOW_H
