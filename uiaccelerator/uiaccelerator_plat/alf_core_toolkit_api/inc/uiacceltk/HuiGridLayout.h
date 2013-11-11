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
* Description:   Definition for CHuiGridLayout. Grid layouts allow specifying 
*                the placement of child visuals within a fixed grid.
*
*/



#ifndef __HUIGRIDLAYOUT_H__
#define __HUIGRIDLAYOUT_H__


#include <e32base.h>
#include <uiacceltk/HuiLayout.h>


/* Forward declarations */
class CHuiEnv;
class THuiMetric;

/* Each dimension corresponds to a different axis for the grid blocks */
enum THuiGridDimension
    {
    EHuiGridColumn = 0,
    EHuiGridRow,
    
    EHuiGridSize
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
 *
 * @see SetLayoutModeFlags
 */
enum THuiGridLayoutModeFlags
    {
    /** Automatically increases innerpaddings if needed */
    EHuiGridIncreaseInnerPadding = 0x1,  // default
 
    /** Automatically decreases innerpaddings if needed */
    EHuiGridDecreaseInnerPadding = 0x2, // default  
    
    /** Automatically increases outerpaddings if needed */    
    EHuiGridIncreaseOuterPadding = 0x4,  // default

    /** Automatically decreases outerpaddings if needed */    
    EHuiGridDecreaseOuterPadding = 0x8,  // default
    };
    
/**
 * CHuiGridLayout is a layout that allows visuals to be positioned within a 
 * grid.
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
 */
class CHuiGridLayout : public CHuiLayout
    	{
public:

    /* Constructors and destructor. */

    /**
     * Construct a new grid layout and give its ownership to a control.
     *
     * @note Default expansion direction is vertical. See SetExpanding() to
     * changed the direction.
     * @note the columns and rows will be initialized with unit weights equal to 1, so that blocks 
     * will all share even proportions of the available area to start with.
     * @note in order to use a mixture of metric weights, or to set different weights, set rows 
     * and columns to zero when calling this method, then use e.g. @c AppendWeightL to 
     * subsequently populate the weights.
     * 
     * @param aOwnerControl  Owner control.
     * @param aColumns       Number of columns in the grid.
     * @param aRows          Number of rows in the grid.
     * @param aParentLayout  Parent layout for the new grid layout.
     */
    IMPORT_C static CHuiGridLayout* AddNewL(CHuiControl& aOwnerControl, 
                                            TInt aColumns, TInt aRows,
                                            CHuiLayout* aParentLayout = 0);

    IMPORT_C CHuiGridLayout(MHuiVisualOwner& aOwner);

    /** From CHuiLayout, deriving class must implement base call */
    IMPORT_C void ConstructL();
    IMPORT_C ~CHuiGridLayout();

    /* Methods inherited from CHuiLayout. */
    IMPORT_C void SetSize(const THuiRealSize& aSize, TInt aTransitionTime=0);
    IMPORT_C TInt ChildOrdinal(TInt aIndex);
    IMPORT_C TBool ChildPos(TInt aOrdinal, TPoint& aPos);
    IMPORT_C TBool ChildSize(TInt aOrdinal, TSize& aSize);
    IMPORT_C TInt ChildRect(TInt aOrdinal, THuiRealRect& aRect);
    IMPORT_C THuiXYMetric BaseUnit() const;
    IMPORT_C void RemoveAndDestroyAllD();
    IMPORT_C CHuiVisual* FindTag(const TDesC8& aTag);
    IMPORT_C TInt Count() const;
    IMPORT_C CHuiVisual& Visual(TInt aIndex) const;
    IMPORT_C void SetPos(const THuiRealPoint& aPos, TInt aTransitionTime = 0);
    IMPORT_C void UpdateChildrenLayout(TInt aTransitionTime = 0);
    IMPORT_C TBool PrepareDrawL();
    IMPORT_C void Draw(CHuiGc& aGc) const;
    IMPORT_C void DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const;
    IMPORT_C TBool Changed() const;
    IMPORT_C void ReportChanged();
    IMPORT_C void ClearChanged();
    IMPORT_C void NotifySkinChangedL();
    IMPORT_C void ExpandRectWithContent(TRect& aRect) const;
    IMPORT_C void VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
   
public: // new methods   
        
    /**
     * Sets the number of columns in this grid layout.
     * All weights are set to 1 (ie. each column is of an equal
     * width).
     *
     * @param aColumnCount The number of columns.
     */
    IMPORT_C void SetColumnsL(TInt aColumnCount);
    
    /**
     * Sets the number of rows in this grid layout.
     * All weights are set to 1 (ie. each column is of an equal
     * height).     
     *
     * @param aRowCount The number of rows.
     */    
    IMPORT_C void SetRowsL(TInt aRowCount);
    
    /**
     * Sets the number and weights of columns in this grid layout.
     * The caller specifies an array of relative weights to use for each
     * column.
     * Weights can be any integer, ie a column of weight 30 would
     * be twice the width of a column of weight 15.
     *
     * @note this is equivalent to using metrics with unit type EHuiUnitWeight.
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
     * @note this is the same as using metrics with unit type EHuiUnitWeight.
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
     * @see AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @note This API could be useful if then subsequent calls to @c ReplaceWeightL are made to 
     * change specific values, depending on how many of the values are different. 
     * @note Calling this with aCount set to 0 will clear all of the existing weights in the direction
     * of the specified dimension.
     * @note this is the same as using metrics with unit type EHuiUnitWeight.
     *
     * @param aDim the dimension along which to fill
     * @param aCount The number of lines of blocks to fill
     * @param aWeight the weights to be used for all blocks
     */    
    IMPORT_C void FillWeightsL(THuiGridDimension aDim, TInt aCount, const THuiMetric& aWeight);

    /**
     * Add a new line of blocks to this grid layout at the last position in the direction of the 
     * specified dimension. It will have the supplied weight. Weights can be any metric value, 
     * hence different units can be used for each block. In particular, EHuiUnitWeight can be used 
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
     *      be better to use EHuiUnitWeight
     * @note For example [2 weights, 1 weight, 2 weights] in a layout of 100 pixels would result 
     *      in [40 pixels, 20 pixels, 40 pixels]. 
     * @note For example [10 pixels, 1 weight, 15 pixels] in a layout of 100 pixels would result 
     *      in [10 pixels, 75 pixels, 15 pixels]. 
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that block
     */
    IMPORT_C void AppendWeightL(THuiGridDimension aDim, const THuiMetric& aWeight);

    /**
     * Add a new line of blocks to this grid layout at the specified position in the direciton of 
     * the specified dimension. It will have the supplied weight. In the case of proportional 
     * weights, the effect of this will be to cause the other blocks to resize according to the new 
     * total weight. It will also mean that many child visuals will now occupy different blocks within
     * the grid according to how the blocks wrap.
     *
     * @since S60 3.2
     *
     * @note will leave if the specified position is greater than the number of objects currently in the array, 
     * so check first by calling @c Count
     * @see AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that block
     * @param aPos the index of the block
     */
    IMPORT_C void InsertWeightL(THuiGridDimension aDim, const THuiMetric& aWeight, TInt aPos);
    /**
     * Sets the weight of a specific line of blocks in this grid layout, in the direction of the supplied dimension.
     * In the case of proportional weights, the effect of this will be to cause the 
     * other blocks to resize according to the new total weight. 
     *
     * @since S60 3.2
     *
     * @see AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aWeight the weight to be used for the block in the specified dimension, 
     *          replacing any previously existing weight for that cell
     * @param aPos the index of the cell
     */
    IMPORT_C void ReplaceWeightL(THuiGridDimension aDim, const THuiMetric& aWeight, TInt aPos);

    /**
     * Remove a line of blocks from this grid layout at the specified position in the 
     * specified dimension. In the case of proportional weights, the effect of this will 
     * be to cause the other blocks to resize according to the new total weight. It will also mean 
     * that many child visuals will now occupy different blocks within the grid according to how 
     * the blocks wrap.
     *
     * @since S60 3.2
     *
     * @see AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aPos the index of the cell
     */
    IMPORT_C void RemoveWeightL(THuiGridDimension aDim, TInt aPos);

    /**
     * Returns the weight of a specific line of blocks in this grid layout, in the
     * specified dimension. 
     *
     * @since S60 3.2
     *
     * @see AppendWeightL for more information on how weights are used to calculate layout positions.
     *
     * @param aDim the dimension to which the weight corresponds
     * @param aPos the index of the cell
     * @return the weight being used for the cell in the specified dimension, will be 0 magnitude if not set
     */
    IMPORT_C THuiMetric Weight(THuiGridDimension aDim, TInt aPos) const;
        
    /**
     * Return the number of lines of blocks in this grid, along the specified dimension.
     *
     * @since S60 3.2
     *
     * @param aDim the dimension along which to count
     * @return The number of lines of blocks in this grid.
     */        
    IMPORT_C TInt DimensionCount(THuiGridDimension aDim) const;
    
    /**
     * Sets the expansion flags to the given bitfield, made up of
     * bits from CHuiLayout::TExpansionFlags.
      *
     * With EExpandVertically, the filling starts from 
     * top left corner (right if mirrored) and a row is filled 
     * full before moving to the next row. If there are more items
     * than rows*colums, they are layouted accordingly below the grid
     * content area.
     * 
     * With EExpandHorizontally, the filling starts from 
     * top left corner (right if mirrored) and a column is filled 
     * full before moving to the next column. If there are more items
     * than rows*colums, they are layouted accordingly on the right
     * side (left if mirrored) of the content area of the grid.
     *
     * @param aFlags Determines the expansion direction.  
     *
     */
    IMPORT_C void SetExpanding(TInt aFlags);
    
    /**
     * Return the number of rows in this grid.
     *
     * @return The number of rows in this grid.
     */
    IMPORT_C TInt RowCount() const;

    /**
     * Return the number of columns in this grid.
     *
     * @return The number of columns in this grid.
     */        
    IMPORT_C TInt ColumnCount() const;

    /**
     * Calculates the (x,y) position of a child in the grid. Returns the block
     * position, where block (0, 0) is in the top left corner (top right corner
     * when the layout is mirrored).
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
    IMPORT_C void SetLayoutModeFlags(THuiGridDimension aDim, TUint aGridLayoutModeFlags);

    /**
     * Clears flag(s) which alter the children's pixel error distribution within the grid layout.
     *
     * @param aDim the dimension to which the given flag(s) affect.
     * @param aGridLayoutModeFlags Flags to be cleared
     * @see TAlfGridLayoutModeFlags, SetLayoutModeFlags
     */                
    IMPORT_C void ClearLayoutModeFlags(THuiGridDimension aDim, TUint aGridLayoutModeFlags);    
    
    /**
     * Return flag(s) which alter the children's pixel error distribution within the grid layout.
     *
     * @param aDim the dimension to return the flags for.
     */
    IMPORT_C TUint LayoutModeFlags(THuiGridDimension aDim);
    
public: // RnD utilities

    IMPORT_C void GetClassName(TDes& aName) const;
    IMPORT_C void DumpTree() const;
    IMPORT_C void GetInstanceName(TDes& aName) const;
    IMPORT_C TType Type() const;
    IMPORT_C TInt SessionId() const;
    IMPORT_C void SetSessionId(TInt aSessionId);


private:
    
    /* Private methods */
    
    /**
     * Checks if the grid filling order is from top right corner instead 
     * the top left (default)
     * 
     * @return ETrue if the fillig is started from top right.
     */
    TBool UseRightToLeftFillingOrder() const;

    /**
     * Calculates the size and position along the specified axis according to the current weights. 
     * The non-weights are resolved into pixels and the remainder after subtracting from InnerSize 
     * is shared between the other cells that are specified using weight metrics.
     *
     * @note that this calculation assumes that the inner paddings have already been subtracted 
     * from the inner size, and that they will be added back in when calculating the actual 
     * position in base units.
     * @note when called repeatedly by UpdateChildrenLayout, all the cells will be calculated for 
     * each time UpdateChildLayout is called.
     *
     * @param aDim the dimension along which to calculate
     * @param aIndex the block being calculated
     * @param aAvailablePixels the space available within the grid layout, after the paddings and 
     *      inner paddings have already been subtracted
     * @param aPos the resulting position in pixels of the block along the axis (replaces contents)
     * @param aSize the resulting size in pixels of the block along the axis (replaces contents)
     * @param aFloorPixelPos the total width assuming every item width is floored.
     * @param aCeilingPixelPos the total width assuming every item width is ceilinged.
     */
    void CalculateCellInPixelsReal(
        THuiGridDimension aDim, 
        TInt aIndex, 
        TReal32 aAvailablePixels, 
        TReal32& aPos, 
        TReal32& aSize,
        TInt& aFloorPixelPos,
        TInt& aCeilingPixelPos) const;

    /**
     * Calculates the size and position along the specified axis according to the current weights. 
     * The non-weights are resolved into pixels and the remainder after subtracting from InnerSize 
     * is shared between the other cells that are specified using weight metrics.
     * Returns integer pixel values, taking into account the TAlfGridLayoutModeFlags for error rounding.
     *
     * @param aDim the dimension along which to calculate
     * @param aIndex the block being calculated
     * @param aAvailablePixels the space available within the grid layout, after the paddings and 
     *      inner paddings have already been subtracted
     * @param aPos the resulting position in pixels of the block along the axis (replaces contents)
     * @param aSize the resulting size in pixels of the block along the axis (replaces contents)
     * @see CalculateCellInPixelsReal
     */
    void CalculateCellInPixels(
        THuiGridDimension aDim, 
        TInt aIndex, 
        TReal32 aAvailablePixels, 
        TInt& aPos, 
        TInt& aSize) const;

    /** 
     * Calculate inner/outer distribution of padding.
     * Interpolates between the previously calculated positions for
     * inner padding and outer padding of the child, according to the
     * proportion between inner and outer padding size.
     *
     * @param aDim the dimension along which to calculate
     * @param aInnerPadPos the calculated position if inner padding is applied
     * @param aOuterPadPos the calculated position if outer padding is applied
     */
    TReal32 CHuiGridLayout::ProportionalInnerOuterPadding(  
        THuiGridDimension aDim,
        TReal32 aInnerPadPos,
        TReal32 aOuterPadPos) const;

    /**
     * Retrieve the weight and pixel size from a metric.
     * Returns one or the other depending on what type the metric is.
     * Zeroes unused params.
     *
     * @param aDim the dimension along which to calculate     
     * @param aMetric The metric to examine
     * @param aWeight Contains weight from metric.
     * @param aPix Contains pixel size from metric.
     */
    void WeightFromMetric(THuiGridDimension aDim, THuiMetric& aMetric, TReal32& aWeight, TInt& aPix) const;

private:
    // Private data structure. Owned.
    struct TGridLayoutPrivateData;
    TGridLayoutPrivateData* iGridLayoutData;
    
    TAny* iSpare;
    };
	
#endif  // __HUIGRIDLAYOUT_H__
