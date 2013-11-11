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
* Description:   Flow layout definition.
*
*/



#ifndef C_ALFFLOWLAYOUT_H
#define C_ALFFLOWLAYOUT_H

#include <alf/alflayout.h> 

/**
 * Flow layout positions children next to each other either horizontally or
 * vertically. The sizes of child visuals are not changed by default.
 *
 * The layout inner padding is used to determine the gap between children.
 *
 * Flow layout takes system wide layout mirroring into consideration by
 * querying the mirroring state through AknLayoutUtils::LayoutMirrored().
 * In western locales the visuals are layed out from left to right whereas
 * in arabic locale for instance the visuals are layed out from right to left.
 * Mirroring is considered only when the flow is horizontal. Vertical flow layout
 * will always lay visuals from top to bottom.
 * Usage:
 * @code
 * 
 *  //create flowlayout
 *  CAlfFlowLayout* flowLayout = CAlfFlowLayout::AddNewL( control );
 *  //Add two child text visuals to flow layout
 *  CAlfTextVisual* text1 = CAlfTextVisual::AddNewL( control, flowLayout );
 *  CAlfTextVisual* text2 = CAlfTextVisual::AddNewL( control, flowLayout );
 *  text1->SetTextL( _L"Text1" );
 *  text2->SetTextL( _L"Text2" );
 * 
 *  //set Flow direction
 *  flowLayout->SetFlowDirection( CAlfFlowLayout::EFlowHorizontal );
 *  flowLayout->UpdateChildrenLayout( 1 );
 * 
 *  // Set Centring
 *  flowLayout->SetCentering( ETrue );
 *  flowLayout->UpdateChildrenLayout(transitionTimeInMilliSeconds);
 * 
 * //Set flow mode
 *  flowLayout->SetMode( CAlfFlowLayout::EModeFitPerpendicular );
 *  flowLayout->UpdateChildrenLayout( 1 );
 * 
 * @endcode
 * @lib alfclient.lib
 * @since S60 v3.2
 */
class CAlfFlowLayout : public CAlfLayout
    {

public:

    /** Flow directions. */
    enum TFlowDirection
        {
        /** 
         * Flow horizontally. System wide layout mirroring is considered
         * to determine whether visuals are layed from left to right
         * or from right to left.
         * @see AknLayoutUtils::LayoutMirrored()
         */
        EFlowHorizontal = 0,
        
        /** Flow vertically. */
        EFlowVertical
        };
        
    /** Mode flags that specify how the flow is laid out. */
    enum TMode
        {
        /** Center the children perpendicular to the flow direction.
            For example, if the flow direction is horizontal (left->right)
            this flag will center the children vertically along the flow. */
        EModeCenterPerpendicular = 0x1,
        
        /** Fit the children perpendicular to the flow direction.
            For example, if the flow direction is horizontal, the children
            will be vertically resized to fit the layout's rectangle. */
        EModeFitPerpendicular = 0x2,
        };        

    /**
     * Two-phased constructor, which attach new object into the control.
     * @param aOwnerControl Control that owns this new obejct.
     * @param aParentLayout If given, the new object is set as child.
     */
    IMPORT_C static CAlfFlowLayout* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfFlowLayout();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfFlowLayout();

    
    /**
     * Sets the direction of the flow.
     *
     * @param aDirection  Flow direction.
     */
    IMPORT_C void SetFlowDirection(TFlowDirection aDirection);

    /**
     * Sets the centering mode of the flow. This sets/clears the 
     * EModeCenterPerpendicular flag. Calling this is equivalent to calling
     * SetMode() with that flag included or missing from the mode.
     *
     * @param aCentering  <code>ETrue</code> to set the EModeCenterPerpendicular mode.
     *                    <code>EFalse</code> to clear it.
     */    
    IMPORT_C void SetCentering(TBool aCentering);
    
    /**
     * Sets the mode of the flow. The mode flags determine how the flow layout
     * behaves.
     *
     * @param aMode  Mode flags (OR'd together).
     *
     * @see CAlfFlowLayout::TMode
     */
    IMPORT_C void SetMode(TInt aMode);
    
    /**
     * Returns the mode flags of the flow.
     *
     * @see CAlfFlowLayout::TMode
     *
     * @return  Returns the current mode flags.
     */
    IMPORT_C TInt Mode() const;

    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C CAlfVisual* FindTag(const TDesC8& aTag);
    /** 
     * From CAlfLayout
     * @see CAlfLayout
     */
    IMPORT_C TAlfXYMetric BaseUnit() const; 


protected:
    /**
     * From CAlfVisual
     * @see CAlfVisual
     */
    IMPORT_C void DoRemoveAndDestroyAllD();
    IMPORT_C void PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

                                
private:

    struct TFlowLayoutPrivateData;
    TFlowLayoutPrivateData* iFlowLayoutData;

    };



#endif // C_ALFFLOWLAYOUT_H
