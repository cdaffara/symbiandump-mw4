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
* Description:   Implementation for CHuiAnchorLayout. Anchor layouts 
*                allow specifying the placement of child visuals using 
*                anchors, that can be relative or absolute coordinates and 
*                attached to a specific edge of the layout.
*
*/



#include "uiacceltk/HuiAnchorLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"

//
// Private structures
//

/*
 * Anchor that defines an edge's position and/or size. 
 */
struct CHuiAnchorLayout::TEdgeAnchor
    {
    THuiAnchorProximity iEdgeProximity;
    THuiMetric iOffset;
    THuiAnchorProximity iAttachmentProximity;
    TInt iAttachmentOrdinal;
    };

/* 
 * an axis can contain up to two edges
 */
struct CHuiAnchorLayout::TAxis
    {
public:
    inline TInt Edge(THuiAnchorProximity aEdgeProximity, TEdgeAnchor*& aEdge) const;
    inline TInt AddEdge(THuiAnchorProximity aEdgeProximity, TEdgeAnchor*& aEdge);
    
public:
    /** array of edges */
    TFixedArray<TEdgeAnchor, 2> iEdges;
    };

inline TInt CHuiAnchorLayout::TAxis::Edge(THuiAnchorProximity aEdgeProximity, TEdgeAnchor*& aEdge) const
    {
    TInt error = KErrNotFound;
    TInt count = iEdges.Count();
    for(TInt ii = 0; ii < count && error == KErrNotFound ; ii++)
        {
        if(iEdges[ii].iEdgeProximity == aEdgeProximity)
            {
            aEdge = const_cast<TEdgeAnchor*>(&(iEdges[ii])); // uses the const operator[] as this is a const method
            error = KErrNone;
            }
        }
    return error;
    }

TInt CHuiAnchorLayout::TAxis::AddEdge(THuiAnchorProximity aEdgeProximity, TEdgeAnchor*& aEdge)
    {
    TInt error = Edge(aEdgeProximity, aEdge);
    if(error == KErrNotFound)
        {
        // we haven't found the one we're looking for, so now look for an empty slot
        error = Edge(EHuiAnchorProximityNone, aEdge);
        if(error == KErrNotFound)
            {
            // couldn't find an empty slot, so actually we consider that to be an overflow
            error = KErrOverflow;
            }
        }
    return error;
    }


/*
 * axes for a given anchor. 
 *
 */
struct CHuiAnchorLayout::TAnchor
    {
public:
    TAxis& Axis(THuiAnchorAxis aAxis) const;
    
public:
    /** 
     * array of axes indexed by THuiAnchorAxis, each one contains the edges for a given axis
     */
    TFixedArray<TAxis, 2> iAxes;
    };

CHuiAnchorLayout::TAxis& CHuiAnchorLayout::TAnchor::Axis(THuiAnchorAxis aAxis) const
    {
    // index is an enumeration and cannot go outside array boundaries.
    return const_cast<CHuiAnchorLayout::TAxis&>(iAxes[aAxis]);
    }


/*
 * Internal private data struct.
 */
struct CHuiAnchorLayout::THuiAnchorLayoutPrivateData
    {
public:
    THuiAnchorLayoutPrivateData();
    
public:
    /*
     * Anchors for a given axis, indexed by ordinal. 
     * @note that if an ordinal is used that is outside the current size of the 
     * array, then empty items will be filled in up to the ordinal needed. However,
     * in general ordinals will correspond to indexes of visuals within the layout, 
     * and therefore we shouldn't expect unreasonable growth of anchors.
     */
    RArray<TAnchor> iAnchors;
    };

CHuiAnchorLayout::THuiAnchorLayoutPrivateData::THuiAnchorLayoutPrivateData()
    {
    }



//
// Methods for compatibility with deprecated APIs
//

THuiAnchorType CHuiAnchorLayout::ConvertDeprecatedAnchorTypeToType(THuiAnchor aAnchor) const
    {
    THuiAnchorType type;
    switch(aAnchor)
        {
        case EHuiAnchorTopLeft:
            type = EHuiAnchorTypeTopLeft;
            break;
        case EHuiAnchorBottomRight:
            type = EHuiAnchorTypeBottomRight;
            break;
        default:
            type = EHuiAnchorTypeNone;
            break;
        }
    return type;
    }
    
THuiAnchorAttachmentOrigin CHuiAnchorLayout::ConvertDeprecatedOrginToAttachmentOrigin(THuiAnchorOrigin aOrigin) const
    {
    THuiAnchorAttachmentOrigin origin;
    switch(aOrigin)
        {
        case EHuiAnchorOriginLeft:
            origin = EHuiAnchorAttachmentOriginLeft;
            break;
        case EHuiAnchorOriginHCenter:
            origin = EHuiAnchorAttachmentOriginHCenter;
            break;
        case EHuiAnchorOriginRight:
            origin = EHuiAnchorAttachmentOriginRight;
            break;
        case EHuiAnchorOriginTop:
            origin = EHuiAnchorAttachmentOriginTop;
            break;
        case EHuiAnchorOriginVCenter:
            origin = EHuiAnchorAttachmentOriginVCenter;
            break;
        case EHuiAnchorOriginBottom:
            origin = EHuiAnchorAttachmentOriginBottom;
            break;
        default:
            origin = EHuiAnchorAttachmentOriginNone;
            break;
        }
    return origin;
    }

THuiAnchorAttachmentOrigin CHuiAnchorLayout::ConvertDeprecatedOriginsToAttachmentOrigin(THuiAnchorOrigin aHorizOrigin, THuiAnchorOrigin aVertOrigin) const
    {
    THuiAnchorAttachmentOrigin newHorizAttachmentOrigin = ConvertDeprecatedOrginToAttachmentOrigin(aHorizOrigin);
    THuiAnchorAttachmentOrigin newVertAttachmentOrigin = ConvertDeprecatedOrginToAttachmentOrigin(aVertOrigin);
    THuiAnchorAttachmentOrigin origin = THuiAnchorAttachmentOrigin(newHorizAttachmentOrigin | newVertAttachmentOrigin);
    return origin;
    }

THuiMetric CHuiAnchorLayout::ConvertDeprecatedAnchorMetricAndTargetToHuiMetric(THuiAnchorMetric aMetric, TReal32 aTarget) const
    {
    THuiUnit unit;
    switch(aMetric)
        {
        case EHuiAnchorMetricAbsolute:
            unit = EHuiUnitPixel;
            break;
        case EHuiAnchorMetricRelativeToSize:
            unit = EHuiUnitRelativeToMySize;
            break;
        default:
            unit = EHuiUnitPixel;
            break;
        }
    THuiMetric metric(aTarget, unit);
    return metric;
    }

THuiXYMetric CHuiAnchorLayout::ConvertDeprecatedAnchorMetricsAndOffsetToHuiMetric(
    THuiAnchorMetric aHorizMetric,
    THuiAnchorMetric aVertMetric,
    const THuiTimedPoint& aOffset) const
    {
    TReal32 horizTarget = aOffset.iX.Target();
    TReal32 vertTarget = aOffset.iY.Target();
    
    THuiMetric horizMetric = ConvertDeprecatedAnchorMetricAndTargetToHuiMetric(aHorizMetric, horizTarget);
    THuiMetric vertMetric = ConvertDeprecatedAnchorMetricAndTargetToHuiMetric(aVertMetric, vertTarget);
    
    THuiXYMetric metric(horizMetric, vertMetric);
    return metric;
    }

//
// Methods for dealing with internal data types
//

THuiAnchorProximity CHuiAnchorLayout::ConvertTypeToProximity(THuiAnchorType aType, THuiAnchorAxis& aAxis) const
    {
    THuiAnchorProximity proximity;
    
    THuiAnchorProximity proximityH = THuiAnchorProximity((aType & EHuiAnchorBitmaskHorizontal) >> EHuiAnchorBitmaskShiftHorizontal);
    THuiAnchorProximity proximityV = THuiAnchorProximity((aType & EHuiAnchorBitmaskVertical) >> EHuiAnchorBitmaskShiftVertical);
    if(proximityH)
        {
        proximity = proximityH;
        aAxis = EHuiAnchorAxisHorizontal;
        }
    else if(proximityV)
        {
        proximity = proximityV;
        aAxis = EHuiAnchorAxisVertical;
        }
    else 
        {
        proximity = EHuiAnchorProximityNone;
        }
    return proximity;
    }

THuiAnchorProximity CHuiAnchorLayout::ConvertAttachmentOriginToProximity(THuiAnchorAttachmentOrigin aAttachmentOrigin, THuiAnchorAxis& aAxis) const
    {
    THuiAnchorType type = THuiAnchorType(aAttachmentOrigin);
    return ConvertTypeToProximity(type, aAxis);
    }

void CHuiAnchorLayout::ConvertCornerToEdges(THuiAnchorType aCorner, THuiAnchorType& aEdgeH, THuiAnchorType& aEdgeV) const
    {
    aEdgeH = THuiAnchorType(aCorner & EHuiAnchorBitmaskHorizontal);
    aEdgeV = THuiAnchorType(aCorner & EHuiAnchorBitmaskVertical);
    }

void CHuiAnchorLayout::ConvertCornerAttachmentOriginToEdgeAttachmentOrigins(THuiAnchorAttachmentOrigin aCornerOrigin, THuiAnchorAttachmentOrigin& aAttachmentOriginH, THuiAnchorAttachmentOrigin& aAttachmentOriginV) const
    {
    aAttachmentOriginH = THuiAnchorAttachmentOrigin(aCornerOrigin & EHuiAnchorBitmaskHorizontal);
    aAttachmentOriginV = THuiAnchorAttachmentOrigin(aCornerOrigin & EHuiAnchorBitmaskVertical);
    }


//
// Methods
//

EXPORT_C CHuiAnchorLayout* CHuiAnchorLayout::AddNewL(CHuiControl& aOwnerControl,
                                                     CHuiLayout* aParentLayout)
    {
    CHuiAnchorLayout* layout = STATIC_CAST(CHuiAnchorLayout*,
        aOwnerControl.AppendLayoutL(EHuiLayoutTypeAnchor, aParentLayout));
    return layout;
    }


CHuiAnchorLayout::CHuiAnchorLayout(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    }


void CHuiAnchorLayout::ConstructL()
    {
    CHuiLayout::ConstructL();
    iHuiAnchorLayoutPrivateData = new (ELeave) THuiAnchorLayoutPrivateData;
    }


EXPORT_C CHuiAnchorLayout::~CHuiAnchorLayout()
    {
    Reset();
    delete iHuiAnchorLayoutPrivateData;
    }


EXPORT_C void CHuiAnchorLayout::Reset()
    {
    if ( iHuiAnchorLayoutPrivateData ) // a Fix for OOM situations, tried to dereference NULL pointer
    	{
	    for(TInt ii = EHuiAnchorAxisHorizontal; ii <= EHuiAnchorAxisVertical; ii++)
	        {
	        iHuiAnchorLayoutPrivateData->iAnchors.Reset();
	        }
    	}
    }


EXPORT_C void CHuiAnchorLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    if(!(Flags() & EHuiVisualFlagFreezeLayout))
        {    
        UpdateChildrenLayout(aTransitionTime);
        }
    }


EXPORT_C TBool CHuiAnchorLayout::ChildSize(TInt aOrdinal, TSize& aSize)
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
            aSize = LocalPointInPixels(size, EHuiReferenceStateTarget).AsSize();
            }
        }
    return result;
    }
    
EXPORT_C TBool CHuiAnchorLayout::ChildPos(TInt aOrdinal, TPoint& aPos)
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
            aPos = LocalPointInPixels(rect.iTl, EHuiReferenceStateTarget);
            }
        }
    return result;
    }

EXPORT_C TInt CHuiAnchorLayout::ChildRect(TInt aOrdinal, THuiRealRect& aRect)
    {
    TInt result(THuiLayoutChildRectUpdateNotNeeded);

    // split inner area (in pixels) into separate axes
    TFixedArray<TReal32, 2> tl;
    TFixedArray<TReal32, 2> br;

    // evaluate the anchor along each axis in turn
    if(aOrdinal < iHuiAnchorLayoutPrivateData->iAnchors.Count())
        {
        TAnchor& anchor = iHuiAnchorLayoutPrivateData->iAnchors[aOrdinal];
        for(TInt ii = EHuiAnchorAxisHorizontal; ii <= EHuiAnchorAxisVertical; ii++)
            {
            THuiAnchorAxis axisId = THuiAnchorAxis(ii);
            TAxis& axis = anchor.Axis(axisId);
            // index is an enumeration and cannot go outside array boundaries
            result |= EvaluateAxis(axisId, axis, tl[axisId], br[axisId]);
            }
        aRect.iTl.iX = tl[EHuiAnchorAxisHorizontal];
        aRect.iTl.iY = tl[EHuiAnchorAxisVertical];
        aRect.iBr.iX = br[EHuiAnchorAxisHorizontal];
        aRect.iBr.iY = br[EHuiAnchorAxisVertical];
        }

    if(result == THuiLayoutChildRectUpdateNotNeeded)
        {
        // No anchor defined at all. Use the default anchor.
        THuiRealPoint topLeft = InnerTopLeft();
        THuiRealSize inner = InnerSize();
        aRect.iTl = topLeft;
        aRect.iBr.iX = inner.iWidth;
        aRect.iBr.iY = inner.iHeight;
        result = THuiLayoutChildRectLayoutUpdateNeeded;
        }

    return result;
    }


/* 
 * the values from the paddings are in parent base units, whilst the anchor edges have 
 * their own units, so we must first convert everything to real pixels, then calculate 
 * the pixel positions, then convert everything back into base units of this layout visual
 */
TInt CHuiAnchorLayout::EvaluateAxis(THuiAnchorAxis aAxisId, const TAxis& aAxis, TReal32& aNear, TReal32& aFar) const
    {
    TInt result(THuiLayoutChildRectUpdateNotNeeded);
    // @todo use cached value if optimization is needed

    // @todo refactorize with CHuiLayout::InnerSize
    THuiRealRect paddingRectPx = PaddingInPixels(EHuiReferenceStateTarget);
    THuiRealPoint sizePointPx = LocalPointInPixels(Size().RealTarget(), EHuiReferenceStateTarget);
    THuiRealPoint innerPaddingPointPx = MetricToPixels(InnerPadding(), EHuiReferenceStateTarget);
    
    // switch to near and far along this axis
    TReal32 nearPx = (aAxisId == EHuiAnchorAxisHorizontal) ? paddingRectPx.iTl.iX : paddingRectPx.iTl.iY;
    TReal32 farPaddingPx = (aAxisId == EHuiAnchorAxisHorizontal) ? paddingRectPx.iBr.iX : paddingRectPx.iBr.iY;
    TReal32 sizePx = (aAxisId == EHuiAnchorAxisHorizontal) ? sizePointPx.iX : sizePointPx.iY;
    TReal32 farPx = sizePx - farPaddingPx;
    TReal32 innerPaddingPx= (aAxisId == EHuiAnchorAxisHorizontal) ? innerPaddingPointPx.iX : innerPaddingPointPx.iY;

    // these will be the calculated values, which we build up one edge at a time
    TReal32 nearCalculatedPx(nearPx);
    TReal32 farCalculatedPx(farPx);

    // we iterate over the edges, but we don't know what order
    // they were defined, so have to build up the answer as we go
    TInt count = aAxis.iEdges.Count();
    for(TInt ii = 0; ii < count; ii++)
        {
        const TEdgeAnchor& edge = aAxis.iEdges[ii];
        if(edge.iEdgeProximity != EHuiAnchorProximityNone)
            {
            // now convert this edge anchor's metric offset into pixels
            TReal32 offsetPx = EdgeOffsetInPixels(aAxisId, edge);
            result |= EvaluateEdgeAnchorInPixels(aAxisId, edge, innerPaddingPx , offsetPx, nearPx, nearCalculatedPx, farPx, farCalculatedPx);
            }
        }

    // Get metric reference, for converting between base units of this layout (from the child visual's perspective) and pixels.
    // We must calculate metric reference in both directions, but we only need it along this axis
    THuiRealPoint childMetricRefSizePx = MetricReferenceForLayoutInPixels(BaseUnit().Abs()); // always target reference
    TReal32 childMetricRefPx = (aAxisId == EHuiAnchorAxisHorizontal) ? childMetricRefSizePx.iX :childMetricRefSizePx.iY;
        
    // Convert the result back into child relative coordinates
    THuiMetric nearCalculated = (aAxisId == EHuiAnchorAxisHorizontal) ? BaseUnit().iX :BaseUnit().iY;
    THuiMetric farCalculated(nearCalculated);
    ConvertPixelsToMetricLength(nearCalculated, nearCalculatedPx, childMetricRefPx);
    ConvertPixelsToMetricLength(farCalculated, farCalculatedPx, childMetricRefPx);

    // back to point and size
    aNear = nearCalculated.iMagnitude; 
    aFar = farCalculated.iMagnitude;

    return result;
    }

TReal32 CHuiAnchorLayout::EdgeOffsetInPixels(THuiAnchorAxis aAxisId, const TEdgeAnchor& aEdge) const
    {
    THuiRealPoint offsetRefSizePx = MetricReferenceInPixels(THuiXYMetric(aEdge.iOffset), EHuiReferenceStateTarget);
    TReal32 offsetRefPx = (aAxisId == EHuiAnchorAxisHorizontal) ? offsetRefSizePx.iX :offsetRefSizePx.iY;
    TReal32 offsetPx(0);
    ConvertMetricLengthToPixels(offsetPx, aEdge.iOffset, offsetRefPx);
    return offsetPx;
    }
    
TInt CHuiAnchorLayout::EvaluateEdgeAnchorInPixels(
    THuiAnchorAxis aAxisId,
    const TEdgeAnchor& aEdge,
    TReal32 aInnerPaddingPx,
    TReal32 aOffsetPx,
    TReal32 aNearPx, TReal32& aNearCalculatedPx, 
    TReal32 aFarPx, TReal32& aFarCalculatedPx) const
    {
    TInt result(THuiLayoutChildRectUpdateNotNeeded);

    // first calculate the offset caused if there is an attached anchor
    TReal32 attachOriginNearCalculatedPx(aNearPx);
    TReal32 attachOriginFarCalculatedPx(aFarPx);
    result |= EvaluateEdgeAttachmentInPixels(aAxisId, aEdge, aInnerPaddingPx, aNearPx, attachOriginNearCalculatedPx, aFarPx, attachOriginFarCalculatedPx);
    
    TReal32 origDimensionPx = attachOriginFarCalculatedPx - attachOriginNearCalculatedPx;
    TReal32 nearOriginTransform = attachOriginNearCalculatedPx - aNearPx;
    TReal32 farOriginTransform = attachOriginFarCalculatedPx - aFarPx;

    // Transform to origin
    switch(aEdge.iAttachmentProximity)
        {
        case EHuiAnchorProximitySize:
            // @todo not yet implemented
            return(EFalse);
        case EHuiAnchorProximityNear:
            nearOriginTransform = 0;
            farOriginTransform -= origDimensionPx;
            break;
        case EHuiAnchorProximityCenter:
            nearOriginTransform += origDimensionPx / 2;
            farOriginTransform -= origDimensionPx / 2;
            break;
        case EHuiAnchorProximityFar:
            nearOriginTransform += origDimensionPx;
            farOriginTransform = 0;
            break;
        default:
            break;
        }

    // Apply edge offset.
    switch(aEdge.iEdgeProximity)
        {
        case EHuiAnchorProximitySize:
            // @todo not yet implemented
            return(EFalse);
        case EHuiAnchorProximityNear:
            aNearCalculatedPx = attachOriginNearCalculatedPx + aOffsetPx + nearOriginTransform;
            result |= THuiLayoutChildRectPosUpdateNeeded;
            break;
        case EHuiAnchorProximityCenter:
            // @todo not yet implemented
            return(EFalse);
        case EHuiAnchorProximityFar:
            aFarCalculatedPx = attachOriginFarCalculatedPx + aOffsetPx + farOriginTransform;
            result |= THuiLayoutChildRectSizeUpdateNeeded;
            break;
        default:
            break;
        }

    return result;
    }

TInt CHuiAnchorLayout::EvaluateEdgeAttachmentInPixels(
    THuiAnchorAxis aAxisId,
    const TEdgeAnchor& aEdge,
    TReal32 aInnerPaddingPx,
    TReal32 aNearPx, TReal32& aNearCalculatedPx, 
    TReal32 aFarPx, TReal32& aFarCalculatedPx) const
    {
    TInt result(THuiLayoutChildRectUpdateNotNeeded);
    
    // if there is an attachment, follow that
    if(aEdge.iAttachmentOrdinal != EHuiAnchorAttachToParent)
        {
        // retrieve the edge to which we are attached
        const TAnchor* nextAnchor = Anchor(aEdge.iAttachmentOrdinal); 
        if(nextAnchor)
            {
            // @todo note that with size attachments, the axis id might change here
            TAxis& nextAxis = nextAnchor->Axis(aAxisId);
            TEdgeAnchor* nextEdge = NULL;
            TInt error = nextAxis.Edge(aEdge.iAttachmentProximity, nextEdge);
            if(error == KErrNone)
                {
                // now need to calculate near and far for the attached edge
                // @todo in the case of size attachments, need to evaluate both next edges
                TReal32 nextOffsetPx = EdgeOffsetInPixels(aAxisId, *nextEdge);
                TReal32 attachOriginNearCalculatedPx(aNearPx);
                TReal32 attachOriginFarCalculatedPx(aFarPx);
                result |= EvaluateEdgeAnchorInPixels(aAxisId, *nextEdge, aInnerPaddingPx , nextOffsetPx, aNearPx, attachOriginNearCalculatedPx, aFarPx, attachOriginFarCalculatedPx);
 
                switch(aEdge.iAttachmentProximity)
                    {
                    case EHuiAnchorProximitySize:
                         // @todo not yet implemented
                        return(EFalse);
                    case EHuiAnchorProximityNear:
                        aNearCalculatedPx =  attachOriginNearCalculatedPx + aInnerPaddingPx;
                        break;
                    case EHuiAnchorProximityCenter:
                        aNearCalculatedPx = (attachOriginNearCalculatedPx + attachOriginFarCalculatedPx + aInnerPaddingPx) / 2;
                        aFarCalculatedPx = aNearCalculatedPx;
                        break;
                    case EHuiAnchorProximityFar:
                        aFarCalculatedPx = attachOriginFarCalculatedPx - aInnerPaddingPx;
                        break;
                    default:
                        break;                    
                    }
                }
            }
        }
    return result;
    }
    
const CHuiAnchorLayout::TAnchor* CHuiAnchorLayout::Anchor(TInt aOrdinal) const
    {
    TAnchor* anchor(NULL);
    if(aOrdinal < iHuiAnchorLayoutPrivateData->iAnchors.Count())
        {
        anchor = &(iHuiAnchorLayoutPrivateData->iAnchors[aOrdinal]);
        }
    return anchor;
    }

EXPORT_C TInt CHuiAnchorLayout::SetRelativeAnchorRect
        (TInt aOrdinal,
         THuiAnchorOrigin aTlHorizOrigin,
         THuiAnchorOrigin aTlVertOrigin,
         const THuiRealPoint& aTopLeftOffset,
         THuiAnchorOrigin aBrHorizOrigin,
         THuiAnchorOrigin aBrVertOrigin,
         const THuiRealPoint& aBottomRightOffset)
    {
    // Create top left anchor first
    TInt error = SetAnchor(
        EHuiAnchorTopLeft, 
        aOrdinal, 
        aTlHorizOrigin, 
        aTlVertOrigin,
        EHuiAnchorMetricRelativeToSize, 
        EHuiAnchorMetricRelativeToSize,
        THuiTimedPoint(aTopLeftOffset.iX, aTopLeftOffset.iY));

    // if TL is OK, create BR anchor
    if ( error == KErrNone )
        {
        error = SetAnchor(
            EHuiAnchorBottomRight, 
            aOrdinal, 
            aBrHorizOrigin, 
            aBrVertOrigin,
            EHuiAnchorMetricRelativeToSize, 
            EHuiAnchorMetricRelativeToSize,
            THuiTimedPoint(aBottomRightOffset.iX, aBottomRightOffset.iY));
        
        // If BR fails try to roll back by removing the TL anchor.
        if ( error != KErrNone )
            {
            RemoveAnchor( EHuiAnchorTopLeft, aOrdinal );
            }
        }
    return error;
    }
    
EXPORT_C TInt CHuiAnchorLayout::SetAnchor(THuiAnchor aAnchor, TInt aOrdinal,
                                          THuiAnchorOrigin aHorizOrigin,
                                          THuiAnchorOrigin aVertOrigin,
                                          THuiAnchorMetric aHorizMetric,
                                          THuiAnchorMetric aVertMetric,
                                          const THuiTimedPoint& aOffset)
    {
    // convert deprecated API call into new format
    THuiAnchorType newType = ConvertDeprecatedAnchorTypeToType(aAnchor);
    THuiAnchorAttachmentOrigin newAttachmentOrigin = ConvertDeprecatedOriginsToAttachmentOrigin(aHorizOrigin, aVertOrigin);
    THuiXYMetric newOffset = ConvertDeprecatedAnchorMetricsAndOffsetToHuiMetric(aHorizMetric, aVertMetric, aOffset);
    return Attach(aOrdinal, newType, newOffset, newAttachmentOrigin, EHuiAnchorAttachToParent);
    }


EXPORT_C void CHuiAnchorLayout::RemoveAnchor(THuiAnchor aAnchor, TInt aOrdinal)
    {
    // convert deprecated API call into new format
    THuiAnchorType newType = ConvertDeprecatedAnchorTypeToType(aAnchor);
    Detach(aOrdinal, newType);
    }

EXPORT_C TInt CHuiAnchorLayout::Attach(
    TInt aOrdinal,
    THuiAnchorType aType, 
    const THuiMetric& aOffset,
    THuiAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
    {
    TInt error = KErrNone;
    
    // ensure that it's an edge
    THuiAnchorType edgeH;
    THuiAnchorType edgeV;
    ConvertCornerToEdges(aType, edgeH, edgeV);
    TBool bothAxes = (edgeH != EHuiAnchorTypeNone) && (edgeV != EHuiAnchorTypeNone);
    if(bothAxes)
            {
        return KErrArgument;
            }
    
    THuiAnchorAxis axisId(EHuiAnchorAxisHorizontal);
    THuiAnchorProximity edgeProximity = ConvertTypeToProximity(aType, axisId);
    THuiAnchorProximity attachmentProximity = ConvertAttachmentOriginToProximity(aAttachmentOrigin, axisId);

    // size currently not supported, center only supported for attachment proximity
    TBool sizeProx = (edgeProximity == EHuiAnchorProximitySize) || (attachmentProximity == EHuiAnchorProximitySize);
    TBool centerProx = (edgeProximity == EHuiAnchorProximityCenter);
    if(sizeProx || centerProx)
        {
        return KErrArgument;
        }

    // avoid issue of Anchor() returning pointer to const
    TAnchor* anchor(NULL);
    TInt count = iHuiAnchorLayoutPrivateData->iAnchors.Count();
    if(aOrdinal < count)
        {
        anchor = &(iHuiAnchorLayoutPrivateData->iAnchors[aOrdinal]);
        }
    else
        {
        // Create a new anchor. Fill any gaps.
        for(TInt ii = count; ii <= aOrdinal; ii++)
            {
            TInt appendError = iHuiAnchorLayoutPrivateData->iAnchors.Append(TAnchor());
            if ( appendError != KErrNone )
                {
                return appendError;
                }
            anchor = &(iHuiAnchorLayoutPrivateData->iAnchors[ii]);
            Mem::FillZ(anchor, sizeof(*anchor));
            }
        }
    
    ASSERT(anchor);
    
    TEdgeAnchor* edge(NULL);
    TAxis& axis = anchor->Axis(axisId);
    error = axis.AddEdge(edgeProximity, edge);
    if(error == KErrNone)
        {
        edge->iEdgeProximity = edgeProximity;
        edge->iOffset = aOffset;
        edge->iAttachmentProximity = attachmentProximity;
        edge->iAttachmentOrdinal = aAttachmentOrdinal;

        // ensure that the attachment doesn't attach an edge to itself, 
        // however we don't need to do anything additional here, as this will
        // be checked automatically when we check for cycles.
        
        // ensure that adding the attachment will not cause a circular dependency...
        if(CheckForCycles(axisId, edgeProximity, aOrdinal, edge))
            {
            edge->iEdgeProximity = EHuiAnchorProximityNone;
            error = KErrArgument;
            }
        }
    
    return error;
    }

EXPORT_C TInt CHuiAnchorLayout::Attach(
    TInt aOrdinal,
    THuiAnchorType aType, 
    const THuiXYMetric& aOffset,
    THuiAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
        {
    // ensure that it's a corner
    THuiAnchorType typeH;
    THuiAnchorType typeV;
    ConvertCornerToEdges(aType, typeH, typeV);
    TBool bothAxes = (typeH != EHuiAnchorTypeNone) && (typeV != EHuiAnchorTypeNone);
    if(!bothAxes)
        {
        return KErrArgument;
        }

    // split the corner into two edges
    THuiAnchorAttachmentOrigin attachmentOriginH;
    THuiAnchorAttachmentOrigin attachmentOriginV;
    ConvertCornerAttachmentOriginToEdgeAttachmentOrigins(aAttachmentOrigin, attachmentOriginH, attachmentOriginV);
            
    TInt error = Attach(aOrdinal, typeH, aOffset.iX, attachmentOriginH, aAttachmentOrdinal);

    // if H is OK, create V
    if ( error == KErrNone )
        {
        error = Attach(aOrdinal, typeV, aOffset.iY, attachmentOriginV, aAttachmentOrdinal);
        // If V fails try to roll back by removing H
        if ( error != KErrNone )
            {
            Detach(aOrdinal, typeH);
            }
        }
    
    return error;
    }

EXPORT_C TInt CHuiAnchorLayout::Attach(
    TInt aOrdinal,
    const THuiBoxMetric& aOffset,
    THuiAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
    {
    // first detach all other edges
    Detach(aOrdinal);
    
    // if no attachment origin is specified, interpret this as meaning "attach each edge to its neighbour's corresponding edge"
    THuiAnchorAttachmentOrigin leftAttachmentOrigin = (aAttachmentOrigin == EHuiAnchorAttachmentOriginNone) ? EHuiAnchorAttachmentOriginLeft : aAttachmentOrigin;
    THuiAnchorAttachmentOrigin rightAttachmentOrigin = (aAttachmentOrigin == EHuiAnchorAttachmentOriginNone) ? EHuiAnchorAttachmentOriginRight : aAttachmentOrigin;
    THuiAnchorAttachmentOrigin topAttachmentOrigin = (aAttachmentOrigin == EHuiAnchorAttachmentOriginNone) ? EHuiAnchorAttachmentOriginTop : aAttachmentOrigin;
    THuiAnchorAttachmentOrigin bottomAttachmentOrigin = (aAttachmentOrigin == EHuiAnchorAttachmentOriginNone) ? EHuiAnchorAttachmentOriginBottom : aAttachmentOrigin;
        
    // just attach all four edges separately
    TInt error = Attach(aOrdinal, EHuiAnchorTypeLeft, aOffset.iLeft, leftAttachmentOrigin, aAttachmentOrdinal);
    if ( error == KErrNone )
        {
        error = Attach(aOrdinal, EHuiAnchorTypeRight, aOffset.iRight, rightAttachmentOrigin, aAttachmentOrdinal);
        if ( error == KErrNone )
            {
            error = Attach(aOrdinal, EHuiAnchorTypeTop, aOffset.iTop, topAttachmentOrigin, aAttachmentOrdinal);
            if ( error == KErrNone )
                {
                error = Attach(aOrdinal, EHuiAnchorTypeBottom, aOffset.iBottom, bottomAttachmentOrigin, aAttachmentOrdinal);
                // if any of them fail, roll back the ones we've already added, unwinding as we go
                if ( error != KErrNone )
                    {
                    DetachEdge(aOrdinal, EHuiAnchorTypeBottom);
                    }
                }
            if ( error != KErrNone )
                {
                DetachEdge(aOrdinal, EHuiAnchorTypeTop);
                }
            }
        if ( error != KErrNone )
            {
            DetachEdge(aOrdinal, EHuiAnchorTypeRight);
            }
        }
    if ( error != KErrNone )
        {
        DetachEdge(aOrdinal, EHuiAnchorTypeLeft);
        }
    return error;
    }

EXPORT_C void CHuiAnchorLayout::Detach(TInt aOrdinal)
    {
    if(aOrdinal < iHuiAnchorLayoutPrivateData->iAnchors.Count())
        {
        TAnchor& anchor = iHuiAnchorLayoutPrivateData->iAnchors[aOrdinal];
        for(TInt axisId = EHuiAnchorAxisHorizontal; axisId <= EHuiAnchorAxisVertical; axisId++)
            {
            TAxis& axis = anchor.Axis(THuiAnchorAxis(axisId));
            TInt count = axis.iEdges.Count();
            for(TInt ii = 0; ii < count; ii++)
                {
                TEdgeAnchor& edge = axis.iEdges[ii];
                edge.iEdgeProximity = EHuiAnchorProximityNone; 
                edge.iOffset = THuiMetric();
                edge.iAttachmentProximity = EHuiAnchorProximityNone;
                edge.iAttachmentOrdinal = EHuiAnchorAttachToParent;
                }
            }
        }
    }
    
EXPORT_C void CHuiAnchorLayout::Detach(TInt aOrdinal, THuiAnchorType aType)
    {
    // split into edges
    THuiAnchorType typeH;
    THuiAnchorType typeV;
    ConvertCornerToEdges(aType, typeH, typeV);
    if(typeH != EHuiAnchorTypeNone)
        {
        DetachEdge(aOrdinal, typeH);
        }
    if(typeV != EHuiAnchorTypeNone)
        {
        DetachEdge(aOrdinal, typeV);
        }
    }

void CHuiAnchorLayout::DetachEdge(TInt aOrdinal, THuiAnchorType aType)
    {
    // Try to find an existing edge anchor entry for this
    TEdgeAnchor* edge(NULL);
    THuiAnchorAxis axisId(EHuiAnchorAxisHorizontal);
    THuiAnchorProximity edgeProximity = ConvertTypeToProximity(aType, axisId);

    // avoid issue of Anchor() returning pointer to const
    TAnchor* anchor(NULL);
    if(aOrdinal < iHuiAnchorLayoutPrivateData->iAnchors.Count())
        {
        anchor = &(iHuiAnchorLayoutPrivateData->iAnchors[aOrdinal]);
        TAxis& axis = anchor->Axis(axisId);
        TInt error = axis.Edge(edgeProximity, edge);
        if(error == KErrNone)
            {
            edge->iEdgeProximity = EHuiAnchorProximityNone; 
            edge->iOffset = THuiMetric();
            edge->iAttachmentProximity = EHuiAnchorProximityNone;
            edge->iAttachmentOrdinal = EHuiAnchorAttachToParent;
            }
        }
    }

/*
 * The model of anchors attached to other anchors should represent a directed 
 * acyclic graph (DAG), so check that insertion of a new attachment (/edge) does not 
 * cause a cycle. 
 * 
 * @note This method assumes that all previous insertion attempts have maintained 
 *               the DAG property.
 * @note This method is implemented using a depth first recursive tree search. 
 * @note Because there can be only one attachment (which is along a specfic axis) starting 
 *              at each edge, it is not necessary to keep a visited list.
 */
TBool CHuiAnchorLayout::CheckForCycles(
    THuiAnchorAxis aStartAxisId,
    THuiAnchorProximity aStartAnchorProximity,
    TInt aStartOrdinal, 
    const TEdgeAnchor* aEdge) const
    {
    TInt nextOrdinal = aEdge->iAttachmentOrdinal;
    if(nextOrdinal == EHuiAnchorAttachToParent)
        {
        // we've found the end of a path, as this corner attaches to the parent.
        return EFalse;
        }

    const TAnchor* nextAnchor = Anchor(nextOrdinal); 
    if(!nextAnchor)
        {
        // anchor not specified, so we've found the end of a path
        return EFalse;
        }

    // @todo note that with size attachments, the axis might change here
    TAxis& nextAxis = nextAnchor->Axis(aStartAxisId);
    TEdgeAnchor* nextEdge = NULL;
    TInt error = nextAxis.Edge(aEdge->iAttachmentProximity, nextEdge);

    // @todo, when implementing size attachments, need to evaluate both edges at this point
    // in order to determine the dependencies for this edge
    if(error != KErrNone)
        {
        return EFalse;
        }

    // so we now know the next proximity, and the next anchor.
    if(aStartAnchorProximity == nextEdge->iEdgeProximity && aStartOrdinal == nextOrdinal)
        {
        // we have found a cycle!
        // note that this will also detect an attempt to attach to the same edge on same ordinal
        return ETrue;
        }
    else
        {
        return CheckForCycles(aStartAxisId, aStartAnchorProximity, aStartOrdinal, nextEdge);
        }
    }

// end of file        
