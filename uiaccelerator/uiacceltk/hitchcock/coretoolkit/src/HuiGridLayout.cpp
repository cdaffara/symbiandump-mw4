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



#include "uiacceltk/HuiGridLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/huimetric.h"

#include <AknUtils.h>


#define FLOOR(x) (TReal32(TInt(TReal32(x))))
#define FLOOR_INT(x) (TInt((x)))

// Symbian does not have a native ceil() implementation, we need to use "near enough" rounding.
#define CEIL(x) (TReal32(TInt(TReal32(x)+0.999f)))
#define CEIL_INT(x) (TInt(TReal32(x)+0.999f))

// Fractional part of real
#define FRAC(x) (TReal32(x)-FLOOR(x))

//
// Private structures
//

struct TAxis
    {
    
    /** Array of block weights. Used to calculate relative sizes of lines of blocks. */
    RArray<THuiMetric> iWeights;
    TUint iLayoutFlags;    
    };

/*
 * grid weights
 */
struct CHuiGridLayout::TGridLayoutPrivateData
    {
public:
    TGridLayoutPrivateData();
public:
    RArray<TAxis> iAxes;
    
    /** Sets the grid dimensions to automatically expand to store more content */
    TInt iExpansionFlags;
    };

CHuiGridLayout::TGridLayoutPrivateData::TGridLayoutPrivateData()
    : 
    iExpansionFlags(0) 
    {
    
    }


//
// class CHuiGridLayout
//

EXPORT_C CHuiGridLayout* CHuiGridLayout::AddNewL(CHuiControl& aOwnerControl, 
                                                 TInt aColumns, TInt aRows,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiGridLayout* grid = STATIC_CAST(CHuiGridLayout*,
        aOwnerControl.AppendLayoutL(EHuiLayoutTypeGrid, aParentLayout));
    grid->SetColumnsL(aColumns);
    grid->SetRowsL(aRows);

    return grid;
    }


EXPORT_C CHuiGridLayout::CHuiGridLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    }


EXPORT_C void CHuiGridLayout::ConstructL()
    {    
    CHuiLayout::ConstructL();
    iGridLayoutData = new(ELeave) TGridLayoutPrivateData;
    for(TInt ii = EHuiGridColumn; ii <= EHuiGridRow; ii++)
        {
        TAxis axis;
        axis.iLayoutFlags = 0;
        
        iGridLayoutData->iAxes.AppendL(axis);
        }
    SetColumnsL(1);
    SetRowsL(1);
    }

	
EXPORT_C CHuiGridLayout::~CHuiGridLayout()
    {    
    if ( iGridLayoutData )
        {
        for(TInt ii = EHuiGridColumn; ii <= EHuiGridRow; ii++)
            {
            TAxis& axis = iGridLayoutData->iAxes[ii];
            axis.iWeights.Reset();
            }

        iGridLayoutData->iAxes.Reset();
        }

    delete iGridLayoutData;
    iGridLayoutData = 0;
    }


EXPORT_C TInt CHuiGridLayout::ChildOrdinal(TInt aIndex)
    {
    // Use the visual index as ordinal.
    return EffectiveLayoutOrdinal(Visual(aIndex));
    }


EXPORT_C TPoint CHuiGridLayout::OrdinalToBlock(TInt aOrdinal) const
    {
    TPoint block(0,0);
    TInt cols = ColumnCount();
    TInt rows = RowCount();
    
    if (cols && rows)
        {
        if ( iGridLayoutData->iExpansionFlags & EExpandHorizontally )
            {
            block.iX = aOrdinal / rows;
            block.iY = aOrdinal % rows;
            }
        else
            {
            // By default the flow is vertically
            block.iX = aOrdinal % cols;
            block.iY = aOrdinal / cols;
            }
        }
        
    return block;    
    }

EXPORT_C void CHuiGridLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }

EXPORT_C TBool CHuiGridLayout::ChildSize(TInt aOrdinal, TSize& aSize)
    {
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectSizeUpdateNeeded);
        if(result)
            {
            THuiRealPoint size(rect.Width(), rect.Height());
            aSize = LocalPointInPixels(size).AsSize();
            }
        }
    return result;
    }
    
EXPORT_C TBool CHuiGridLayout::ChildPos(TInt aOrdinal, TPoint& aPos)
    {
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectPosUpdateNeeded);
        if(result)
            {
            aPos = LocalPointInPixels(rect.iTl);
            }
        }
    return result;
    }
    
    
EXPORT_C TInt CHuiGridLayout::ChildRect(TInt aOrdinal, THuiRealRect& aPos)
    {        
    if (!ColumnCount() || !RowCount())
        {
        return THuiLayoutChildRectUpdateNotNeeded;
        }
        
    // @todo refactorize with CHuiLayout::InnerSize
    THuiRealRect paddingPx = PaddingInPixels(EHuiReferenceStateTarget);

    // find the inner border and remove the inner paddings
    THuiRealPoint innerSizePx = LocalPointInPixels(Size().RealTarget(), EHuiReferenceStateTarget);
    innerSizePx.iX -= paddingPx.iTl.iX + paddingPx.iBr.iX;
    innerSizePx.iY -= paddingPx.iTl.iY + paddingPx.iBr.iY;

    // subtract the inner padding (space between each child).
    THuiRealPoint innerPaddingPx = MetricToPixels(InnerPadding(), EHuiReferenceStateTarget);
     
    THuiRealPoint availablePx(innerSizePx.iX - ((ColumnCount() - 1) * innerPaddingPx.iX),
                              innerSizePx.iY - ((RowCount() - 1) * innerPaddingPx.iY));

    TPoint blockLogical = OrdinalToBlock(aOrdinal);
    TPoint blockPosPx;
    TSize blockSizePx;

    CalculateCellInPixels(EHuiGridColumn, blockLogical.iX, availablePx.iX, blockPosPx.iX, blockSizePx.iWidth);
    CalculateCellInPixels(EHuiGridRow, blockLogical.iY, availablePx.iY, blockPosPx.iY, blockSizePx.iHeight);    

    // now add back in the inner paddings and the padding
    blockPosPx.iX += HUI_ROUND_FLOAT_TO_INT( paddingPx.iTl.iX + (blockLogical.iX * innerPaddingPx.iX) );
    blockPosPx.iY += HUI_ROUND_FLOAT_TO_INT( paddingPx.iTl.iY + (blockLogical.iY * innerPaddingPx.iY) );

    // get metric reference, for converting between base units of this layout (from the child visual's perspective) and pixels
    THuiRealPoint layoutRefPx = MetricReferenceForLayoutInPixels(BaseUnit().Abs()); // always target reference

    // Convert the result back into child relative coordinates
    THuiXYMetric blockPosChildMetric = BaseUnit();
    THuiXYMetric blockSizeChildMetric = BaseUnit();

    ConvertPixelsToMetricLength(blockPosChildMetric.iX, TReal32(blockPosPx.iX), layoutRefPx.iX);
    ConvertPixelsToMetricLength(blockPosChildMetric.iY, TReal32(blockPosPx.iY), layoutRefPx.iY);
    ConvertPixelsToMetricLength(blockSizeChildMetric.iX, TReal32(blockSizePx.iWidth), layoutRefPx.iX);
    ConvertPixelsToMetricLength(blockSizeChildMetric.iY, TReal32(blockSizePx.iHeight), layoutRefPx.iY);

    aPos.iTl.iX =  blockPosChildMetric.iX.iMagnitude;
    aPos.iTl.iY =  blockPosChildMetric.iY.iMagnitude;
    aPos.iBr.iX = aPos.iTl.iX + blockSizeChildMetric.iX.iMagnitude;
    aPos.iBr.iY = aPos.iTl.iY + blockSizeChildMetric.iY.iMagnitude;
    
    return THuiLayoutChildRectLayoutUpdateNeeded;
    }
   
   
EXPORT_C void CHuiGridLayout::SetColumnsL(TInt aColumnCount)
    {
    THuiMetric defaultWeight(1.f, EHuiUnitWeight);
    FillWeightsL(EHuiGridColumn, aColumnCount, defaultWeight);    
    }
   
    
EXPORT_C void CHuiGridLayout::SetRowsL(TInt aRowCount)
    {
    THuiMetric defaultWeight(1.f, EHuiUnitWeight);
    FillWeightsL(EHuiGridRow, aRowCount, defaultWeight);    
    }
   

EXPORT_C void CHuiGridLayout::SetColumnsL(const RArray<TInt>& aWeights)
    {
    TAxis& axis = iGridLayoutData->iAxes[EHuiGridColumn];
    axis.iWeights.Reset();
    for(TInt i = 0; i < aWeights.Count(); ++i)
        {
        THuiMetric weight(TReal32(aWeights[i]), EHuiUnitWeight);
        User::LeaveIfError(axis.iWeights.Append(weight));
        }
    }
    
EXPORT_C void CHuiGridLayout::SetRowsL(const RArray<TInt>& aWeights)
    {
    TAxis& axis = iGridLayoutData->iAxes[EHuiGridRow];
    axis.iWeights.Reset();
    for(TInt i = 0; i < aWeights.Count(); ++i)
        {
        THuiMetric weight(TReal32(aWeights[i]), EHuiUnitWeight);
        User::LeaveIfError(axis.iWeights.Append(weight));
        }
    }

EXPORT_C void CHuiGridLayout::FillWeightsL(THuiGridDimension aDim, TInt aCount, const THuiMetric& aWeight)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        axis.iWeights.Reset();
        for(TInt i = 0; i < aCount; ++i)
            {
            User::LeaveIfError(axis.iWeights.Append(aWeight));
            }
        }
    }
    
EXPORT_C void CHuiGridLayout::AppendWeightL(THuiGridDimension aDim, const THuiMetric& aWeight)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        User::LeaveIfError(axis.iWeights.Append(aWeight));
        }
    }
    
EXPORT_C void CHuiGridLayout::InsertWeightL(THuiGridDimension aDim, const THuiMetric& aWeight, TInt aPos)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        if(aPos < 0 || aPos >= axis.iWeights.Count())
            {
            User::Leave(KErrArgument);
            }
        else
            {
            User::LeaveIfError(axis.iWeights.Insert(aWeight, aPos));
            }
        }
    }
    
EXPORT_C void CHuiGridLayout::ReplaceWeightL(THuiGridDimension aDim, const THuiMetric& aWeight, TInt aPos)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        if(aPos < 0 || aPos >= axis.iWeights.Count())
            {
            User::Leave(KErrArgument);
            }
        else
            {
            axis.iWeights[aPos] = aWeight;
            }
        }
    }
    
EXPORT_C void CHuiGridLayout::RemoveWeightL(THuiGridDimension aDim, TInt aPos)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        if(aPos < 0 || aPos >= axis.iWeights.Count())
            {
            User::Leave(KErrArgument);
            }
        else
            {
            axis.iWeights.Remove(aPos);
            }
        }
    }
    
EXPORT_C THuiMetric CHuiGridLayout::Weight(THuiGridDimension aDim, TInt aPos) const
    {
    THuiMetric metric(0.f, EHuiUnitWeight); 
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        if(aPos < 0 || aPos >= axis.iWeights.Count())
            {
            // use empty weight
            }
        else
            {
            metric = axis.iWeights[aPos];
            }
        }
    return metric;
    }
    
EXPORT_C TInt CHuiGridLayout::DimensionCount(THuiGridDimension aDim) const
    {
    TInt count = 0;
    if(aDim <= EHuiGridRow)
        {
        count = iGridLayoutData->iAxes[aDim].iWeights.Count();
        }
    return count;
    }

EXPORT_C void CHuiGridLayout::SetExpanding(TInt aFlags)
    {
    iGridLayoutData->iExpansionFlags = aFlags;    
    }
    
// ---------------------------------------------------------------------------
// Function that determines if the filling order.
// ---------------------------------------------------------------------------
//
TBool CHuiGridLayout::UseRightToLeftFillingOrder() const
    {
    return CHuiStatic::LayoutMirrored();
    }

EXPORT_C void CHuiGridLayout::GetClassName(TDes& aName) const
    {
    aName = _L("CHuiGridLayout");
    }

EXPORT_C THuiXYMetric CHuiGridLayout::BaseUnit() const
    {
    return CHuiLayout::BaseUnit();
    }

EXPORT_C CHuiVisual* CHuiGridLayout::FindTag(const TDesC8& aTag)
    {
    return CHuiLayout::FindTag(aTag);
    }
    
EXPORT_C TInt CHuiGridLayout::Count() const
    {
    return CHuiLayout::Count();
    }
    
EXPORT_C CHuiVisual& CHuiGridLayout::Visual(TInt aIndex) const
    {
    return CHuiLayout::Visual(aIndex);
    }
    
EXPORT_C void CHuiGridLayout::SetPos(const THuiRealPoint& aPos, TInt aTransitionTime)
    {
    CHuiLayout::SetPos(aPos, aTransitionTime);
    }
    
EXPORT_C void CHuiGridLayout::UpdateChildrenLayout(TInt aTransitionTime)
    {
    CHuiLayout::UpdateChildrenLayout(aTransitionTime);
    }
    
EXPORT_C TBool CHuiGridLayout::PrepareDrawL()
    {
    return CHuiLayout::PrepareDrawL();
    }
    
EXPORT_C void CHuiGridLayout::Draw(CHuiGc& aGc) const
    {
    CHuiLayout::Draw(aGc);
    }
    
EXPORT_C void CHuiGridLayout::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
    CHuiLayout::DrawSelf(aGc, aDisplayRect);
    }
    
EXPORT_C TBool CHuiGridLayout::Changed() const
    {
    return CHuiLayout::Changed();
    }
    
EXPORT_C void CHuiGridLayout::ReportChanged()
    {
    CHuiLayout::ReportChanged();
    }
    
EXPORT_C void CHuiGridLayout::ClearChanged()
    {
    CHuiLayout::ClearChanged();
    }
    
EXPORT_C void CHuiGridLayout::NotifySkinChangedL()
    {
    CHuiLayout::NotifySkinChangedL();
    }
    
EXPORT_C void CHuiGridLayout::ExpandRectWithContent(TRect& aRect) const
    {
    CHuiLayout::ExpandRectWithContent(aRect); 
    }
    
EXPORT_C void CHuiGridLayout::VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CHuiLayout::VisualExtension(aExtensionUid, aExtensionParams);
    }
    
EXPORT_C void CHuiGridLayout::DumpTree() const
    {
    CHuiLayout::DumpTree();
    }
    
EXPORT_C void CHuiGridLayout::GetInstanceName(TDes& aName) const
    {
    CHuiLayout::GetInstanceName(aName);
    }

EXPORT_C void CHuiGridLayout::RemoveAndDestroyAllD()
    {
    CHuiLayout::RemoveAndDestroyAllD();
    }

HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiGridLayout, ETypeVisual)


EXPORT_C TInt CHuiGridLayout::RowCount() const
    {
    return iGridLayoutData->iAxes[EHuiGridRow].iWeights.Count();
    }

EXPORT_C TInt CHuiGridLayout::ColumnCount() const
    {
    return iGridLayoutData->iAxes[EHuiGridColumn].iWeights.Count();
    }

   
EXPORT_C void CHuiGridLayout::SetLayoutModeFlags(THuiGridDimension aDim, TUint aGridLayoutModeFlags)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        TUint oldFlags = axis.iLayoutFlags;
        

        // The following flags are mutually exclusive, clear existing flags first
        if (aGridLayoutModeFlags&EHuiGridIncreaseInnerPadding)
            {
            axis.iLayoutFlags &= ~EHuiGridDecreaseInnerPadding;
            }
        
        if (aGridLayoutModeFlags&EHuiGridDecreaseInnerPadding)
            {
            axis.iLayoutFlags &= ~EHuiGridIncreaseInnerPadding;
            }        

        if (aGridLayoutModeFlags&EHuiGridIncreaseOuterPadding)
            {
            axis.iLayoutFlags &= ~EHuiGridDecreaseOuterPadding;
            }
        
        if (aGridLayoutModeFlags&EHuiGridDecreaseOuterPadding)
            {
            axis.iLayoutFlags &= ~EHuiGridIncreaseOuterPadding;
            }   
        
        axis.iLayoutFlags |= aGridLayoutModeFlags;
        
        if((oldFlags != axis.iLayoutFlags) && !(Flags() & EHuiVisualFlagFreezeLayout))
            {
            UpdateChildrenLayout(0);
            }          
        }             
    }
  
EXPORT_C void CHuiGridLayout::ClearLayoutModeFlags(THuiGridDimension aDim, TUint aGridLayoutModeFlags)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        TUint oldFlags = axis.iLayoutFlags;      
        axis.iLayoutFlags &= ~aGridLayoutModeFlags;
        
        if((oldFlags != axis.iLayoutFlags) && !(Flags() & EHuiVisualFlagFreezeLayout))
            {
            UpdateChildrenLayout(0);
            }                         
        }          
    }

EXPORT_C TUint CHuiGridLayout::LayoutModeFlags(THuiGridDimension aDim)
    {
    if(aDim <= EHuiGridRow)
        {
        TAxis& axis = iGridLayoutData->iAxes[aDim];
        return axis.iLayoutFlags;
        }
        
    return 0;
    }

void CHuiGridLayout::CalculateCellInPixels(
    THuiGridDimension aDim, 
    TInt aIndex, 
    TReal32 aAvailablePixels, 
    TInt& aPos, 
    TInt& aSize) const
    {
    ASSERT(aDim < EHuiGridSize);
    
    TAxis& axis = iGridLayoutData->iAxes[aDim]; 
    TInt numItems = (aDim==EHuiGridColumn)?ColumnCount():RowCount();    
    TInt itemIntegerWidths = 0;
    TInt itemCeilingedIntegerWidths = 0;
    TReal32 realPos;
    TReal32 realSize;
    TInt sumIntegerWidths;            
    TInt sumCeilingIntegerWidths;
    TReal32 layoutWidthPx;
    TReal32 layoutSizePx;    
            
    CalculateCellInPixelsReal(aDim, aIndex, aAvailablePixels, realPos, realSize, itemIntegerWidths, itemCeilingedIntegerWidths);
    
    TReal32 innerPadPos = realPos;
    
    // Decide what to do with pixel error based on flags
    if (axis.iLayoutFlags&EHuiGridDecreaseInnerPadding)
        {
        // Allow every item to grow out into the padding, this will make the padding appear smaller.
        realSize = CEIL(realSize);             
        }
    else if (axis.iLayoutFlags&EHuiGridIncreaseInnerPadding)
        {
        // Allow every item to be floored, this will make the padding appear bigger.
        realSize = FLOOR(realSize);             
        }
    else
        {
        // For PC-lint
        }

    if (axis.iLayoutFlags&EHuiGridIncreaseOuterPadding)            
        {
        // Ignore accumulated error, simply increase outer padding by the total error for the entire layout.
        
        // Ignore the rounded positions we have calculated, we get the pos by summing the integer widths,
        realPos = itemIntegerWidths-FLOOR_INT(realSize);
        
        CalculateCellInPixelsReal(aDim, numItems-1, aAvailablePixels, layoutWidthPx, layoutSizePx, sumIntegerWidths, sumCeilingIntegerWidths);            
        realPos += (aAvailablePixels-sumIntegerWidths)/2;
        realSize = FLOOR(realSize);
        }
    else if (axis.iLayoutFlags&EHuiGridDecreaseOuterPadding)
        {
        // Ignore accumulated error, simply decrease outer padding by the total error for the entire layout.
        
        // Ignore the rounded positions we have calculated, we get the pos by summing the integer widths,
        realPos = itemCeilingedIntegerWidths-CEIL_INT(realSize);
        
        CalculateCellInPixelsReal(aDim, numItems-1, aAvailablePixels, layoutWidthPx, layoutSizePx, sumIntegerWidths, sumCeilingIntegerWidths);
        realPos += (aAvailablePixels-sumCeilingIntegerWidths)/2;   
        realSize = CEIL(realSize);
        }
    else
        {
        // For PC-lint
        }

    realPos = ProportionalInnerOuterPadding(aDim, innerPadPos, realPos);
    
    aPos = TInt(realPos);
    aSize = TInt(realSize);
    }

TReal32 CHuiGridLayout::ProportionalInnerOuterPadding(  
    THuiGridDimension aDim,
    TReal32 aInnerPadPos,
    TReal32 aOuterPadPos) const
    {
    THuiRealPoint innerPaddingPx = MetricToPixels(InnerPadding(), EHuiReferenceStateTarget);
    THuiRealRect outerPaddingPx = PaddingInPixels(EHuiReferenceStateTarget);
    TAxis& axis = iGridLayoutData->iAxes[aDim]; 
    TUint flag = axis.iLayoutFlags;    
    TReal32 totalPadding;
    TReal32 meanOuterPadding;
    
    if ( HuiUtil::RealCompare( aInnerPadPos, aOuterPadPos ) )
        {
        // Same pos, nothing to do.
        return aOuterPadPos;
        }
    
    if (aDim==EHuiGridColumn)
        {
        meanOuterPadding = (outerPaddingPx.iTl.iX+outerPaddingPx.iBr.iX)/2.0f;
        totalPadding = innerPaddingPx.iX+meanOuterPadding;
        }
    else
        {
        meanOuterPadding = (outerPaddingPx.iTl.iY+outerPaddingPx.iBr.iY)/2.0f;
        totalPadding = innerPaddingPx.iY+meanOuterPadding;            
        }
    
    if (totalPadding && ((flag&EHuiGridIncreaseInnerPadding) && (flag&EHuiGridIncreaseOuterPadding))
                     || ((flag&EHuiGridIncreaseInnerPadding) && (flag&EHuiGridDecreaseOuterPadding))
                     || ((flag&EHuiGridDecreaseInnerPadding) && (flag&EHuiGridIncreaseOuterPadding))
                     || ((flag&EHuiGridDecreaseInnerPadding) && (flag&EHuiGridDecreaseOuterPadding)))
        {
        // Both inner and outer padding flags are active, linearly interpolate between them based on ratio.    
        if (aDim==EHuiGridColumn)
            {
            return ((innerPaddingPx.iX*aInnerPadPos + meanOuterPadding*aOuterPadPos)/totalPadding);            
            }
        else
            {
            return ((innerPaddingPx.iY*aInnerPadPos + meanOuterPadding*aOuterPadPos)/totalPadding);            
            }
        }
        
    return aOuterPadPos;
    }  
    
    
void CHuiGridLayout::CalculateCellInPixelsReal(
    THuiGridDimension aDim, 
    TInt aIndex, 
    TReal32 aAvailablePixels, 
    TReal32& aPos, 
    TReal32& aSize,
    TInt& aFloorPixelPos,
    TInt& aCeilingPixelPos) const
    {
    // note that we will add in the inner padding later
    // total up the pixels or weights on either side of the target block, so that we know what
    // the total size is that we're sharing out by weight. 
    // while we're at it we can remember the size of the target block as well (call it "mid")
    TReal32 totalWeightsValid(0.f);
    TReal32 totalPixelsValid(0.f);  
    TReal32 error(0.f);     
    TInt pixelWeight;
    TReal32 weightWeight;    
    
    TAxis& axis = iGridLayoutData->iAxes[aDim];
    TInt weightCount = axis.iWeights.Count();
    
    aPos = 0;
    aSize = 0;
    aFloorPixelPos = 0;
    aCeilingPixelPos = 0;
    
    // First pass to get total weights and pixels.
    for(TInt ii = 0; ii < weightCount; ii++)
        {    
        THuiMetric& weight = axis.iWeights[ii];
                    
        WeightFromMetric(aDim, weight, weightWeight, pixelWeight);
        totalPixelsValid += pixelWeight;
        totalWeightsValid += weightWeight;
        }

    // All the fixed-pixel items have been accounted for, this is
    // what's left for distribution between the variable, weighted items.    
    TReal32 remainderPixels = aAvailablePixels - totalPixelsValid;

    // Second pass to calculate size and pos
    for(TInt ii = 0; ii <= aIndex; ii++)
        {
        TReal32 nextWeight(0.f);
        TReal32 nextPixels(0.f);

        // Child visuals that are outside of the specified range of blocks 
        // should take the weight of the last specified block.
        THuiMetric weight(1.f, EHuiUnitWeight);
        if(ii < weightCount)
            {
            weight = axis.iWeights[ii];
            }
        else if( weightCount ) // return weight of last element
            {
            weight = axis.iWeights[weightCount-1];
            }
        else
            {
            // For PC-lint
            }

        WeightFromMetric(aDim, weight, nextWeight, pixelWeight);
        nextPixels += pixelWeight;

        TReal32 width;
        if(totalWeightsValid != 0)
            {
            width = (nextWeight / totalWeightsValid) * remainderPixels;
            }
        else
            {
            width = 0;
            }
        
        aFloorPixelPos += FLOOR_INT(width);
        aCeilingPixelPos += CEIL_INT(width);     
        aFloorPixelPos += nextPixels;
        aCeilingPixelPos += nextPixels;             
        
        error += FRAC(width);

        if ((axis.iLayoutFlags == 0))
            {
            if (error >= 0.5)
                {
                // There is a rounding error here, so increase size if no flags forcing exact-size are set
                error--;
                width = CEIL(width);
                }
            else
                {
                width = FLOOR(width);
                }
            }
            
        if(ii < aIndex)
            {
            aPos += width;            
            aPos += nextPixels;
            }
        else
            {
            aSize += width;
            aSize += nextPixels;
            }                    
        }
    }

void CHuiGridLayout::WeightFromMetric(THuiGridDimension aDim, THuiMetric& aMetric, TReal32& aWeight, TInt& aPix) const
    {
    aWeight = 0.0f;
    aPix = 0;
    
    switch(aMetric.iUnit)
        {
        case EHuiUnitWeight:
            {
            aWeight = aMetric.iMagnitude;
            break;
            }
        case EHuiUnitPixel:
            {
            // slight optimization here as we know it's pixels
            aPix = HUI_ROUND_FLOAT_TO_INT(aMetric.iMagnitude);
            break;
            }
        case EHuiUnitNormalized:
        case EHuiUnitRelativeToDisplay:
        case EHuiUnitRelativeToMySize:
        case EHuiUnitS60:
            {
            THuiRealPoint refPx = MetricToPixels(THuiXYMetric(aMetric), EHuiReferenceStateTarget);
            aPix = (aDim == EHuiGridColumn) ? HUI_ROUND_FLOAT_TO_INT(refPx.iX) : HUI_ROUND_FLOAT_TO_INT(refPx.iY);
            break;
            }
        default:
            break;
        }
    }
