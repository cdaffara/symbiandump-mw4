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
* Description:   Layout base class
*
*/



#ifndef C_ALFGRIDLAYOUT_H
#define C_ALFGRIDLAYOUT_H

#include <alf/alflayout.h>

/* Each dimension corresponds to a different axis for the grid blocks */
enum TAlfGridDimension 
    {
    EAlfGridColumn,
    EAlfGridRow
    };

/**
 * These flags specify where pixel error is distributed in grid layout calculations.
 * 
 * If none of these flags are set, then the pixel positions of all children are calculated
 * using reals, then floored to the nearest pixel. If, however, the children must all be
 * the same pixel size, then the following flags can be set. They place extra pixels into
 * the layout's paddings to make up the difference.
 * The flags for increasing and decreasing paddings are
 * mutually exclusive, but you can apply inner and outer padding flags at the same time. This
 * will result in the error being distributed between inner and outer paddings in the same
 * proportion as inner to outer padding. Eg: if inner padding is 10 and outer padding is 20, twice
 * as many error pixels will be distributed to the outer padding.
 * @see SetLayoutModeFlags
 */
enum TAlfGridLayoutModeFlags
    {
    /** Automatically increases innerpaddings if needed. Clears EAlfGridDecreaseInnerPadding if set.*/
    EAlfGridIncreaseInnerPadding = 0x1,
 
    /** Automatically decreases innerpaddings if needed. Clears EAlfGridIncreaseInnerPadding if set.*/
    EAlfGridDecreaseInnerPadding = 0x2,
    
    /** Automatically increases outerpaddings if needed. Clears EAlfGridDecreaseOuterPadding if set.*/    
    EAlfGridIncreaseOuterPadding = 0x4,

    /** Automatically decreases outerpaddings if needed. Clears EAlfGridIncreaseOuterPadding if set.*/    
    EAlfGridDecreaseOuterPadding = 0x8,
    
    EAlfGridFlagAll = 0xF
    };

/**
 * Grid layout class.
 *
 * To use this class, a grid layout is created of a desired width and height.
 * Then, when visuals are added to grid layout, they will populate the grid.
 * By default, all slots in a grid layout are of a uniform size, but the weighting
 * of individual rows and columns can be modified, and can be specified using
 * metric units.
 * Slots will resize automatically as the grid layout size changes.
 *
 * The standard application grid is one example of a grid layout.
 * 
 * Usage:
 *
 * @code
 * // Create a new 3x4 grid layout visual.
 * CAlfGridLayout* grid = CAlfGridLayout::AddNewL( control, 3, 4 );
 * 
 * // Add a single text visual into the grid.
 * CAlfTextVisual* text = CAlfTextVisual::AddNewL( control, grid );
 * 
 * // Make the text visual visible; if no text is specified, nothing is drawn.
 * text->SetTextL(_L("T"));
 * 
 * //Setting grid dimentions with weight
 * RArry<TInt> col;
 * col.AppendL( 1 );
 * col.AppendL( 2 );
 * 
 * //sets no of columns to 2 and width of second column
 * //is double of that of first
 * grid->SetColumns( col );
 * 
 * @endcode
 * @lib alfclient.lib
 * @since S60 v3.2
 */
class CAlfGridLayout : public CAlfLayout
    {
public:

	/* Constructors and destructor. */

    /**
     * Construct a new grid layout and give its ownership to a control.
     *
     * @note the columns and rows will be initialized with equal unit weights, so that blocks will all share 
     * even proportions of the available area to start with.
     * @note in order to use a mixture of metric weights, or to set different weights, set rows 
     * and columns to zero when calling this method, then use e.g. @c AppendWeightL to 
     * subsequently populate the weights.
     * 
     * @param aOwnerControl  Owner control.
     * @param aColumns       Number of columns in the grid.
     * @param aRows          Number of rows in the grid.
     * @param aParentLayout  Parent layout for the new grid layout.
     */
    IMPORT_C static CAlfGridLayout* AddNewL(CAlfControl& aOwnerControl, 
                                            TInt aColumns, TInt aRows,
                                            CAlfLayout* aParentLayout = 0);

	/**
	 * Constructor.
	 */
	IMPORT_C CAlfGridLayout();

	/**
	 * From CAlfLayout. Second-phase constructor.
	 */
	IMPORT_C void ConstructL(CAlfControl& aOwner);

	/**
	 * Virtual destructor.
	 */
	IMPORT_C ~CAlfGridLayout();


	/* Methods. */    

    /**
     * Sets amount of columns.
     *
     * @param aColumnCount Amount of columns.
     */
    IMPORT_C void SetColumnsL(TInt aColumnCount);
    
    /**
     * Sets amount of rows.
     *
     * @param aRowCount Amount of rows.
     */
    IMPORT_C void SetRowsL(TInt aRowCount);
    
    /**
     * Sets the number and weights of columns in this grid layout.
     * The caller specifies an array of relative weights to use for each
     * column.
     * Weights can be any integer, ie a column of weight 30 would
     * be twice the width of a column of weight 15.
     *
     * @note this is equivalent to using metrics with unit type EAlfUnitWeight.
     *
     * @param aWeights An array containing weights for each column.
     */    
    IMPORT_C void SetColumnsL(const RArray<TInt>& aWeights);
    
    /**
     * Sets the number and weights of rows in this grid layout.
     * The caller specifies an array of relative weights to use for each
     * row.
     * Weights can be any integer, ie a row of weight 30 would
     * be twice the height of a row of weight 15.
     *
     * @note this is the same as using metrics with unit type EAlfUnitWeight.
     *
     * @param aWeights An array containing weights for each row.
     */    
    IMPORT_C void SetRowsL(const RArray<TInt>& aWeights);

    /**
     * Sets the number and weights of blocks in this grid layout in the direction of the specified
     * dimension. Each block's weight will be set to equal the supplied value, the result
     * of which is that all blocks will be equally spaced, whatever the units. 
     *
     * @since S60 3.2
     *
     * @see @c AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @note This API could be useful if then subsequent calls to @c ReplaceWeightL are made to 
     * change specific values, depending on how many of the values are different. 
     * @note Calling this with aCount set to 0 will clear all of the existing weights in the direction
     * of the specified dimension.
     * @note this is the same as using metrics with unit type EAlfUnitWeight.
     *
     * @param aDim the dimension along which to fill
     * @param aCount The number of lines of blocks to fill
     * @param aWeight the weights to be used for all blocks
     */    
    IMPORT_C void FillWeightsL(TAlfGridDimension aDim, TInt aCount, const TAlfMetric& aWeight);

    /**
     * Add a new line of blocks to this grid layout at the last position in the direction of the 
     * specified dimension. It will have the supplied weight. Weights can be any metric value, 
     * hence different units can be used for each block. In particular, EAlfUnitWeight can be used 
     * to represent weight values in aribtrary proportional units.
     *
     * In the case of proportional weights, the effect of this will be to cause the other blocks to 
     * resize according to the new total weight.
     *
     * @since S60 3.2
     *
     * @note If non-relative coordinates are specified (e.g., real pixels), the combined blocks might not 
     *      fill the entire layout area. However, weights will always stretch to fill all available space 
     *      after the fixed units have been determined. Therefore, depending on circumstances it may
     *      be better to use EAlfUnitWeight
     * @note For example [2 weights, 1 weight, 2 weights] in a layout of 100 pixels would result 
     *      in [40 pixels, 20 pixels, 40 pixels]. 
     * @note For example [10 pixels, 1 weight, 15 pixels] in a layout of 100 pixels would result 
     *      in [10 pixels, 75 pixels, 15 pixels]. 
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that block
     */
    IMPORT_C void AppendWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight);

    /**
     * Add a new line of blocks to this grid layout at the specified position in the direciton of 
     * the specified dimension. It will have the supplied weight. In the case of proportional 
     * weights, the effect of this will be to cause the other blocks to resize according to the new 
     * total weight. It will also mean that many child visuals will now occupy different blocks within
     * the grid according to how the blocks wrap.
     *
     * @since S60 3.2
     *
     * @see @c AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that block
     * @param aPos the index of the block
     */
    IMPORT_C void InsertWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight, TInt aPos);

    /**
     * Sets the weight of a specific line of blocks in this grid layout, in the direction of the supplied dimension.
     * In the case of proportional weights, the effect of this will be to cause the 
     * other blocks to resize according to the new total weight. 
     *
     * @since S60 3.2
     *
     * @see @c AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that cell
     * @param aPos the index of the cell
     */
    IMPORT_C void ReplaceWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight, TInt aPos);

    /**
     * Remove a line of blocks from this grid layout at the specified position in the 
     * specified dimension. In the case of proportional weights, the effect of this will 
     * be to cause the other blocks to resize according to the new total weight. It will also mean 
     * that many child visuals will now occupy different blocks within the grid according to how 
     * the blocks wrap.
     *
     * @since S60 3.2
     *
     * @see @c AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aPos the index of the cell
     */
    IMPORT_C void RemoveWeightL(TAlfGridDimension aDim, TInt aPos);

    /**
     * Returns the weight of a specific line of blocks in this grid layout, in the
     * specified dimension. 
     *
     * @since S60 3.2
     *
     * @see @c AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aPos the index of the cell
     * @return the weight being used for the cell in the specified dimension, will be 0 magnitude if not set
     */
    IMPORT_C TAlfMetric Weight(TAlfGridDimension aDim, TInt aPos) const;
    
    /**
     * Return the number of lines of blocks in this grid, along the specified dimension.
     *
     * @since S60 3.2
     *
     * @param aDim the dimension along which to count
     * @return The number of lines of blocks in this grid.
     */        
    IMPORT_C TInt DimensionCount(TAlfGridDimension aDim) const;
    
        
    /**
     * ! Deprecated, use leaving method instead
     */
    void SetColumns(TInt aColumnCount)
        {
        TRAP_IGNORE(SetColumnsL(aColumnCount))
        }
    
    /**
     * ! Deprecated, use leaving method instead
     */
    void SetRows(TInt aRowCount)
        {
        TRAP_IGNORE(SetRowsL(aRowCount))
        }
    
    /**
     * ! Deprecated, use leaving method instead
     */
    void SetColumns(const RArray<TInt>& aWeights)
        {
        TRAP_IGNORE(SetColumnsL(aWeights))
        }
    
    /**
     * ! Deprecated, use leaving method instead
     */
    void SetRows(const RArray<TInt>& aWeights)
        {
        TRAP_IGNORE(SetRowsL(aWeights))
        }
    
    /**
     * Sets flags to control expanding behaviour.
     *
     * @param aFlags Flags
     */
    IMPORT_C void SetExpanding(TInt aFlags);
        
    /**
     * Gets amount of rows.
     *
     * @return Amount of rows.
     */
    IMPORT_C TInt RowCount() const;
        
    /**
     * Gets amount of columns.
     *
     * @return Amount of columns.
     */
    IMPORT_C TInt ColumnCount() const;

    /**
     * Calculates the position of a child in the grid. Returns the block
     * position, where block (0, 0) is in the top left corner.
     *
     * @param aOrdinal  Ordinal of a child.
     *
     * @return  Block position of the child.
     */    
    IMPORT_C TPoint OrdinalToBlock(TInt aOrdinal) const;
    
    /**
     * Sets flag(s) which alter the children's pixel error distribution within the grid layout.
     *
     * This method allows the client to force all items in the grid layout to be the same
     * pixel size. The floating point error that this introduces is distributed between the various
     * paddings of the layout.
     * 
     * Example of use that allows outer paddings to be automatically adjusted:
     * \code
     * iGridLayout->SetLayoutModeFlags(EHuiGridRow, EHuiGridAdjustOuterPadding); 
     * \endcode
     *
     * Note that some flags are mutually exclusive, and will clear some existing flags if set.
     *
     * @param aDim the dimension to which the given flag(s) affect.
     * @param aGridLayoutModeFlags Flags to be set or cleared
     * @see TAlfGridLayoutModeFlags, ClearLayoutModeFlags
     */        
    IMPORT_C void SetLayoutModeFlags(TAlfGridDimension aDim, TUint aGridLayoutModeFlags);

    /**
     * Clears flag(s) which alter the children's pixel error distribution within the grid layout.
     *
     * @param aDim the dimension to which the given flag(s) affect.
     * @param aGridLayoutModeFlags Flags to be cleared
     * @see TAlfGridLayoutModeFlags, SetLayoutModeFlags
     */        
    IMPORT_C void ClearLayoutModeFlags(TAlfGridDimension aDim, TUint aGridLayoutModeFlags);    
    
    /**
     * Return flag(s) which alter the children's pixel error distribution within the grid layout.
     *
     * @param aDim the dimension to return the flags for.
     */
    IMPORT_C TInt LayoutModeFlags(TAlfGridDimension aDim);
    
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
    
    // Private structure. Owned.
    struct TGridLayoutPrivateData;
    TGridLayoutPrivateData* iGridLayoutData;

    };


#endif // C_ALFGRIDLAYOUT_H
