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



#ifndef __HUIFLOWLAYOUT_H__
#define __HUIFLOWLAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiLayout.h>


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
 */
NONSHARABLE_CLASS(CHuiFlowLayout) : public CHuiLayout
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
    

	/* Constructors and destructor. */

    /**
     * Construct a new flow layout and append it to a control.
     *
     * @param aDirection     Flow direction.
     * @param aOwnerControl  Control that will own the new layout visual.
     * @param aParentLayout  Parent layout for the new layout visual.
     *
     * @return  Pointer to the new layout visual. The owner control retains
     *          ownership of the returned visual. Do not delete the returned
     *          visual manually.
     */
    IMPORT_C static CHuiFlowLayout* AddNewL(CHuiControl& aOwnerControl, 
                                            TFlowDirection aDirection,
                                            CHuiLayout* aParentLayout = 0);

	/**
	 * Constructor.
	 */
	CHuiFlowLayout(MHuiVisualOwner& aOwner);

	/**
	 * Second-phase constructor.
	 */
	void ConstructL();

	/**
	 * Destructor.
	 */
	IMPORT_C virtual ~CHuiFlowLayout();


	/* Methods. */
	
    IMPORT_C void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);

    TBool ChildSize(TInt aOrdinal, TSize& aSize);
    
    TBool ChildPos(TInt aOrdinal, TPoint& aPos);

    TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);

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
     *
     * @note  This method could be deprecated in the future, since SetMode()
     *        does the same thing.
     */    
    IMPORT_C void SetCentering(TBool aCentering);
    
    /**
     * Sets the mode of the flow. The mode flags determine how the flow layout
     * behaves.
     *
     * @param aMode  Mode flags (OR'd together).
     *
     * @see CHuiFlowLayout::TMode
     */
    IMPORT_C void SetMode(TInt aMode);
    
    /**
     * Returns the mode flags of the flow.
     *
     * @see CHuiFlowLayout::TMode
     *
     * @return  Returns the current mode flags.
     */
    IMPORT_C TInt Mode() const;

private:
    
    /* Private methods */


private:

    /** Direction of the flow: horizontal or vertical. */
    TFlowDirection iDirection;

    /** Mode flags of the flow. */
    TInt iMode;
    
	};

#endif  // __HUIFLOWLAYOUT_H__
