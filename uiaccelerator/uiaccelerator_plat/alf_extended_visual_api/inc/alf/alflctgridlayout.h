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
* Description:   Grid Anchor layout definition.
*
*/



#ifndef C_ALFLCTGRIDLAYOUT_H
#define C_ALFLCTGRIDLAYOUT_H

#include <alf/alfgridlayout.h> 

class TAknLayoutHierarchyComponentHandle;

/**
 *  LCT Grid layout
 * 
 * This is a specialised anchor layout, which is aware of the 
 * avkon LCT layout system. It provides additional APIs
 * that allow grid layouts to be set by referring to LCT layout 
 * components from the Layout Specifcations.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class CAlfLCTGridLayout : public CAlfGridLayout
    {

public:

    /**
     * Two-phased constructor, which attach new object into the control.
     * @param aOwnerControl Control that owns this new obejct.
     * @param aParentLayout If given, the new object is set as child.
     */
    IMPORT_C static CAlfLCTGridLayout* AddNewL(CAlfControl& aOwnerControl,
                                              CAlfLayout* aParentLayout = 0);

    /**
     * Constructor.
     */
    IMPORT_C CAlfLCTGridLayout();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void ConstructL(CAlfControl& aOwner);

    /**
     * Destructor.
     */
    IMPORT_C ~CAlfLCTGridLayout();

    /**
     * Sets the rows and columns for the grid.
     *     
     * @param aComponentHandle reference to the layout data that corresponds to 
     *                                                  the grid.
     * @ param aOffset timed point, note that the position of the timed point will be ignored.
     */
    IMPORT_C void SetColsAndRows(
        TAknLayoutHierarchyComponentHandle& aComponentHandle,
        const TAlfTimedPoint& aOffset);

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

    struct TLCTGridLayoutPrivateData;
    TLCTGridLayoutPrivateData* iLCTGridLayoutData;
    };



#endif // C_ALFLCTGRIDLAYOUT_H
