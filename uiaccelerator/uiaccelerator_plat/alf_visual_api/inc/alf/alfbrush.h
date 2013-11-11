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
* Description:   Base class for all brushes.
*
*/



#ifndef C_ALFBRUSH_H
#define C_ALFBRUSH_H

#include <e32base.h>
#include <alf/alftimedvalue.h>
#include <alf/alfpropertyowner.h>

class CAlfGenComponent;
class CAlfEnv;
class CAlfBrushArray;

/** Brush layers. */
enum TAlfBrushLayer
    {
    EAlfBrushLayerBackground,
    EAlfBrushLayerForeground
    };

/**
 * A brush implements a series of graphics drawing operations. Brushes are 
 * attached to visuals and drawn whenever the visual is drawn. Brushes can 
 * be used for augmenting the normal appearance of visuals with graphical 
 * features such as background pictures, gradients, overlay icons, and 
 * selection highlights.
 * This is base class for all brushes and it cannot be instantiated. 
 * 
 *  Usage:
 *  @code
 * 
 * //Sets the brush layer
 *   myBrush->setLayer( EAlfBrushLayerForeground );
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfBrush : public CAlfPropertyOwner
    {

public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfBrush();
    
    /**
     * Get opacity
     *
     * @return Opacity in TAlfTimedValue
     */
    IMPORT_C const TAlfTimedValue& Opacity() const;
    
    /**
     * Set opacity
     *
     * @param aOpacity New opacity value.
     */
    IMPORT_C void SetOpacity( const TAlfTimedValue& aOpacity );
    
    /**
     * Set the layer
     *
     * @param aLayer Layer (foreground/backgroud).
     */
    IMPORT_C void SetLayer(TAlfBrushLayer aLayer);

    /**
     * Sets whether the brush can draw outside brush guide (i.e. visual's) rect
     * @param aClipToVisual If ETrue, drawing is clipped to visuals area. 
     */
    IMPORT_C void SetClipToVisual(TBool aClipToVisual);
    
    /**
     * Gets whether the brush can draw outside brush guide (i.e. visual's) rect
     * @return ETrue if drawing is clipped to visuals area. 
     */
    IMPORT_C TBool ClipToVisual() const;

    /**
     * Return the server side handle
     *
     * @return Handle to the server side object. 0 if not set.
     */
    IMPORT_C TInt Identifier() const;
    
    /**
     * Return communications
     *
     * @return Comms
     */
    IMPORT_C CAlfGenComponent* Comms() const;
    
    /**
     * @internal
     * 
     * Called when this brush is added into a brush array
     * @param aContainingArray Brush array in which this brush is added
     * @return Error code.
     */
    TInt AppendContainingArray( CAlfBrushArray& aContainingArray );
    
    /**
     * @internal
     * 
     * Called when this brush is removed from a brush array
     * @param aContainingArray Brush array where this brush is removed from
     */
    void RemoveContainingArray( CAlfBrushArray& aContainingArray );

protected:

    /**
     * Constructor
     */
    IMPORT_C CAlfBrush();

    /**
     * Second phase constructor
     */
    IMPORT_C void ConstructL( 
        CAlfEnv& aEnv,
        TInt aImplementationId, 
        TInt aImplementationUid, 
        const TDesC8& aConstructionParams );
    
    IMPORT_C void ConstructL();

    /** ! future proofing */
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);


private: // data

    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;

    };



#endif // C_ALFBRUSH_H
