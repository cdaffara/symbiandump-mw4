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
* Description:   Viewport layout definition.
*
*/



#ifndef C_ALFVIEWPORTLAYOUT_H
#define C_ALFVIEWPORTLAYOUT_H

#include <alf/alflayout.h>

/**
 *  Viewport layout
 *  Usage:
 *  @code
 *  // Aim is to zoom into one of the grids of icons using a viewport layout.
 *
 *  // Create a anchor layout and grid layout into the parent anchor layout and 
 *  // populate it with application icons.
 * 
 *  // The viewport layout sits in the middle of the screen.
 *   parent->SetRelativeAnchorRect(1, EAlfAnchorOriginHCenter, EAlfAnchorOriginVCenter, TAlfRealPoint(-0.2, -0.2),
 *                                    EAlfAnchorOriginHCenter, EAlfAnchorOriginVCenter, TAlfRealPoint(0.2, 0.2));
 *   
 *   // Create the viewport.
 *   CAlfViewportLayout* viewport = CAlfViewportLayout::AddNewL(*this, parent);
 * 
 *  // Set our virtual coordinate system to be based on the number of items in the grid (KNumGridItems).
 *   viewport->SetVirtualSize(TAlfRealSize(KNumGridItems, 1.0), 0);
 *   
 *   // Set the size and position of the viewport to look at the middle icon in the list.
 *   viewport->SetViewportSize(TAlfRealSize(2, 1.0/6.0), 0);
 *   viewport->SetViewportPos(TAlfRealPoint(TReal32(KNumGridItems/2)-1.0f, 5.0/12.0), 0);
 *   
 *   // This anchor layout will contain a strip of large icons clipped by the viewport window.
 *   CAlfAnchorLayout* zoomedParent = CAlfAnchorLayout::AddNewL(*this, viewport);     
 *   
 *  @endcode
 * 
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfViewportLayout : public CAlfLayout
    {

public:

    /**
     * Two-phased constructor, which attach new object into the control.
     * @param aOwnerControl Control that owns this new obejct.
     * @param aParentLayout If given, the new object is set as child.
     */
    IMPORT_C static CAlfViewportLayout* AddNewL(CAlfControl& aOwnerControl,
                                            CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfViewportLayout();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Virtual destructor.
     */
    IMPORT_C ~CAlfViewportLayout();

    /** @beginAPI */

	/**
	 * Set virtual size of the viewable area.
	 *
	 * This is different from the physical size of the viewport -
	 * it defines the coordinate space that is used for scrolling.
	 * To use a normalised (u, v) coordinate mapping system, set aSize to (,1.0, 1.0).
	 *
	 * @param aSize The bottom-right corner of the child visual, in the units you wish to use for positioning the viewport.
	 * @param aTransitionTime The time it will take to complete the resizing of the virtual size.
	 */
    IMPORT_C void SetVirtualSize(const TAlfRealSize& aSize, TInt aTransitionTime);

	/**
	 * Set size of the viewport area.
	 *
	 * This is the size of the "window" we are using to look at the visual.
	 *
	 * @param aSize The size of the viewport in the units defined by SetVirtualSize().
	 * @param aTransitionTime The time it will take to complete the resizing of the viewport area.
	 * @see SetVirtualSize, SetViewportPos
	 */
    IMPORT_C void SetViewportSize(const TAlfRealSize& aSize, TInt aTransitionTime);

	/**
	 * Set position of the viewport area.
	 *
	 * This is the offset of the "window" we are using to look at the visual.
	 *
	 * @param aSize The size of the viewport in the units defined by SetVirtualSize().
	 * @param aTransitionTime The time it will take to complete moving the viewport area.
	 * @see SetVirtualSize, SetViewportSize
	 */
    IMPORT_C void SetViewportPos(const TAlfRealPoint& aPos, TInt aTransitionTime);

    /** @endAPI */

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

    // Private data structure. Owned.
    struct TViewportLayoutPrivateData;
    TViewportLayoutPrivateData* iViewportLayoutData;

    };



#endif // C_ALFVIEWPORTLAYOUT_H
