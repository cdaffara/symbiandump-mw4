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
* Description:   definitions of alf built-in visual support
*
*/



#include "alf/alfvisualhandlers.h"
#include "alf/alflayouthandlers.h"

#include <uiacceltk/HuiGridLayout.h>
#include <uiacceltk/HuiDeckLayout.h>
#include <uiacceltk/HuiAnchorLayout.h>
#include <uiacceltk/HuiFlowLayout.h>
#include <uiacceltk/huiviewportlayout.h>
#include <uiacceltk/HuiCurvePathLayout.h>
#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiUtil.h>

#include "alf/alfconstants.h"
#include "alf/alfserverutils.h"
#include "alfmappingfunctionhandlers.h"

#include <aknlayout2hierarchy.h>

//----------------------------
// Layout base
//----------------------------
EXPORT_C MAlfExtension* CAlfLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfVisualHandler* me = new (ELeave) CAlfLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }

EXPORT_C CAlfLayoutHandler::CAlfLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfVisualHandler(aResolver)
    {
    }

EXPORT_C void CAlfLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiLayout* layout = CHuiLayout::AddNewL(aOwner, aParentLayout);     
        CAlfVisualHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }

EXPORT_C CAlfLayoutHandler::~CAlfLayoutHandler()
    {
    }

EXPORT_C void CAlfLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    switch (aInterface)
        {
        case EHuiObjectTypeLayout:
            return static_cast<CHuiLayout*>(iVisual);
        default:
            return CAlfVisualHandler::GetInterface(aInterface);
                    
        }
    }


EXPORT_C void CAlfLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiLayout* layout = static_cast<CHuiLayout*>(iVisual);

    switch (aCommandId)
        {
        case  EAlfLayoutAppendVisual:           
            {
            // 1 - visual ID  2 - transition time
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            CHuiVisual* visual = (CHuiVisual*)iResolver.GetInterfaceL(EHuiObjectTypeVisual, params->iInt1);
            if (layout->FindVisual(visual) == KErrNotFound)
                {
                layout->AppendL(visual, params->iInt2);
                }
            else
                {
                User::Leave(KErrAlreadyExists);
                } 
            break;                
            }
        
        case  EAlfLayoutRemoveVisual:
            {
            // 1 - visual ID  2 - transition time
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            CHuiVisual* visual = NULL;
            TRAPD( err, visual = (CHuiVisual*)iResolver.GetInterfaceL(EHuiObjectTypeVisual, params->iInt1) );
            if ( err != KErrNone )
                {
                // Visual has been deleted already
                return;
                }
            layout->Remove(visual, params->iInt2);
            
            // todo: we should do the following, but it panic if this is called in RemoveAndDestroyAlld() loop
            /*
            // if visual is showing, we should add it under the host container
            CHuiControlGroup* childGroup = visual->Owner().ControlGroup();
            if ( childGroup && visual->Display() )
                {
                // must be a control
                CHuiControl* control = static_cast<CHuiControl*>(&visual->Owner());
                if ( control->Host() ) // showing if host if host is defined.
                    {
                    CHuiLayout* containerLayout = control->Host()->ContainerLayout( control );
                    __ASSERT_DEBUG( containerLayout, USER_INVARIANT() );
                    if ( containerLayout )
                        {
                        // Should we InsertL instead, so we would have the original
                        // root visual order?
                        containerLayout->AppendL( visual );
                        }
                    }
                }*/
            break;    
            }

        case  EAlfLayoutReorderVisual:
            {
            // 1 - visual ID  2 - position  3 - transition time
            TInt3* params = (TInt3*) aInputBuffer.Ptr();
            CHuiVisual* visual = (CHuiVisual*)iResolver.GetInterfaceL(EHuiObjectTypeVisual, params->iInt1);
            layout->Reorder(*visual, params->iInt2, params->iInt3);
            break;    
            }

        case EAlfLayoutVisualFromIndex:
            {
            TInt* index = (TInt*) aInputBuffer.Ptr();
            CHuiVisual& visual = layout->Visual(*index);
            // todo: can handle be negative ?
            TInt handle = iResolver.GetHandleFromInterface(EHuiObjectTypeVisual, &visual);
            //User::LeaveIfError(handle);

            TPckg<TInt> pckg(handle);
            aResponse = pckg;
            
            break;
            }

        case  EAlfLayoutEnableScrolling:           
            {
            TBool* enable = (TBool*) aInputBuffer.Ptr();
            layout->EnableScrollingL(*enable);
            break;                
            }
        
        case  EAlfLayoutScrollOffset:
            {
            TAlfTimedPoint point;
            AlfTimedPointUtility::CopyTimedPoint(layout->ScrollOffset(), point);

            TPckg<TAlfTimedPoint> buf(point);
            aResponse = buf;
            break;    
            }
        
        case EAlfLayoutSetScrollOffset:
            {
            TAlfTimedPoint* offset = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*offset, layout->ScrollOffset(), iResolver);            
            break;
            }

        case  EAlfLayoutChildOrdinal:           
            { // client should know this as well
            const TInt* index = (TInt*) aInputBuffer.Ptr();
            TInt ordinal(0);
            TPckg<TInt> ordinalPckg(ordinal);
            ordinal = layout->ChildOrdinal(*index);
            aResponse = ordinalPckg;
            break;                
            }
        
        case  EAlfLayoutChildPos:
            {
            TAlfPosInt* pos = (TAlfPosInt*) aInputBuffer.Ptr();
            TAlfPosInt result =
                {
                pos->iPoint,
                EFalse
                };
            TPckg<TAlfPosInt> resp(result);
            result.iInt = layout->ChildPos(pos->iInt, result.iPoint);
            aResponse = resp;            
            break;    
            }
        
        case EAlfLayoutChildSize:
            {
            TAlfSizeInt* size = (TAlfSizeInt*) aInputBuffer.Ptr();
            TAlfSizeInt result = 
                {
                size->iSize,
                EFalse
                };
            TPckg<TAlfSizeInt> resp(result);
            result.iInt = layout->ChildSize(size->iInt, result.iSize);
            aResponse = resp;            
            break;
            }
        case EAlfLayoutSetTransitionTime:
            {
            TInt* ttime = (TInt*) aInputBuffer.Ptr();
            layout->SetTransitionTime(*ttime);
            break;    
            }
        
        case EAlfLayoutSetInnerPaddingPoint:
            {            
            TPoint* padding = (TPoint*) aInputBuffer.Ptr();
            layout->SetInnerPadding(*padding);
            break;    
            }
        
        case EAlfLayoutSetInnerPaddingMetric:
            {            
            TAlfXYMetric* metric = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiMetric newMetricX(metric->iX.iMagnitude, metric->iX.iUnit, metric->iX.iReferenceTextStyleId);
            THuiMetric newMetricY(metric->iY.iMagnitude, metric->iY.iUnit, metric->iY.iReferenceTextStyleId);
            THuiXYMetric newMetric(newMetricX, newMetricY);
            layout->SetInnerPadding(newMetric);
            break;    
            }
        
        case EAlfLayoutInnerPaddingInBaseUnits:
            {
            THuiRealPoint point = layout->InnerPaddingInBaseUnits();
            TAlfRealPoint result(point.iX, point.iY);
            TPckg<TAlfRealPoint> resp(result);
            aResponse = resp;
            break;
            }
            
        case EAlfLayoutVirtualSize:
            {
            TSize virtualSize = layout->VirtualSize();
            TPckg<TSize> pckg(virtualSize);
            aResponse = pckg;                        
            break;    
            }
            
        case EAlfLayoutSetBaseUnit:
            {            
            TAlfMetric* metric = (TAlfMetric*) aInputBuffer.Ptr();
            THuiMetric newMetric(metric->iMagnitude, metric->iUnit, metric->iReferenceTextStyleId);
            layout->SetBaseUnit(newMetric);
            break;
            }

        case EAlfLayoutSetBaseUnitXY:
            {            
            TAlfXYMetric* metric = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiMetric newMetricX(metric->iX.iMagnitude, metric->iX.iUnit, metric->iX.iReferenceTextStyleId);
            THuiMetric newMetricY(metric->iY.iMagnitude, metric->iY.iUnit, metric->iY.iReferenceTextStyleId);
            THuiXYMetric newMetric(newMetricX, newMetricY);
            layout->SetBaseUnit(newMetric);
            break;
            }

        case EAlfLayoutBaseUnit:
            {            
            THuiXYMetric metric = layout->BaseUnit();
            TAlfMetric newMetricX(metric.iX.iMagnitude, metric.iX.iUnit, metric.iX.iReferenceTextStyleId);
            TAlfMetric newMetricY(metric.iY.iMagnitude, metric.iY.iUnit, metric.iY.iReferenceTextStyleId);
            TAlfXYMetric newMetric(newMetricX, newMetricY);
            TPckg<TAlfXYMetric> resp(newMetric);
            aResponse = resp;
            break;
            }

        case EAlfVisualCopyValuesFromOtherVisual:
        case EAlfVisualPauseProperties:
        case EAlfVisualResumeProperties:        
            {
            if (aCommandId == EAlfVisualCopyValuesFromOtherVisual)
               {
               TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
               CHuiLayout* sourceLayout = (CHuiLayout*) iResolver.GetInterfaceL( EHuiObjectTypeLayout, inptr->iInt1 );
               if (inptr->iInt2&AlfVisualProperties::ELayoutScrollOffset)
                   {
                   layout->ScrollOffset().Set(sourceLayout->ScrollOffset().Target(), inptr->iInt3 );
                   }
               }
            else if (aCommandId == EAlfVisualPauseProperties)
               {
               TInt* inptr =  (TInt*) aInputBuffer.Ptr();
               if (*inptr&AlfVisualProperties::ELayoutScrollOffset)
                   {
                   layout->ScrollOffset().iX.Suspend();
                   layout->ScrollOffset().iY.Suspend();
                   }
               }
            else // EAlfVisualResumeProperties)
               {
               TInt* inptr =  (TInt*) aInputBuffer.Ptr();
               if (*inptr&AlfVisualProperties::ELayoutScrollOffset)
                   {
                   layout->ScrollOffset().iX.Suspend(EFalse);
                   layout->ScrollOffset().iY.Suspend(EFalse);
                   }
               }
               
            } // FALL THROUGH !!

        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
        
    }

EXPORT_C void CAlfLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }

//----------------------------
// anchor layout
//----------------------------
EXPORT_C MAlfExtension* CAlfAnchorLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfAnchorLayoutHandler* me = new (ELeave) CAlfAnchorLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfAnchorLayoutHandler::CAlfAnchorLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfAnchorLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiAnchorLayout* layout = CHuiAnchorLayout::AddNewL(aOwner, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfAnchorLayoutHandler::~CAlfAnchorLayoutHandler()
    {
    }

EXPORT_C void CAlfAnchorLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfAnchorLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfAnchorLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiAnchorLayout* anchorLayout = static_cast<CHuiAnchorLayout*>(iVisual); 
    switch (aCommandId)
        {
        case EAlfAnchorLayoutReset:
            {
            anchorLayout->Reset();
            break;
            }
            
        case EAlfAnchorLayoutSetAnchor:
            {
            TAlfLayoutSetAnchorParams* params =  (TAlfLayoutSetAnchorParams*) aInputBuffer.Ptr();
            
            THuiTimedPoint tpoint;
            AlfTimedPointUtility::CopyTimedPoint(params->iOffset, tpoint, iResolver);            
            
            // Set anchor
            TInt error = anchorLayout->SetAnchor( 
                THuiAnchor(params->iAnchor),
                params->iOrdinal,
                THuiAnchorOrigin(params->iHorizOrigin),
                THuiAnchorOrigin(params->iVertOrigin),
                THuiAnchorMetric(params->iHorizMetric),
                THuiAnchorMetric(params->iVertMetric),
                tpoint
                );
                
            User::LeaveIfError( error );

            break;
            }
            
        case EAlfAnchorLayoutSetRelativeAnchorRect:
            {
            TAlfLayoutSetRelativeAnchorRectParams* params =  (TAlfLayoutSetRelativeAnchorRectParams*) aInputBuffer.Ptr();
            
            TInt error = anchorLayout->SetRelativeAnchorRect
                (
                params->iOrdinal,
                THuiAnchorOrigin(params->iTlHorizOrigin),
                THuiAnchorOrigin(params->iTlVertOrigin),
                THuiRealPoint(params->iTopLeftOffset.iX, params->iTopLeftOffset.iY),
                THuiAnchorOrigin(params->iBrHorizOrigin),
                THuiAnchorOrigin(params->iBrVertOrigin),
                THuiRealPoint(params->iBottomRightOffset.iX, params->iBottomRightOffset.iY)
                );
                
            User::LeaveIfError( error );
            break;
            }
            
        case EAlfAnchorLayoutRemoveAnchor:
            {
            TInt2* params =  (TInt2*) aInputBuffer.Ptr();
            anchorLayout->RemoveAnchor( (THuiAnchor)params->iInt1, params->iInt2 );
            break;
            }

        case EAlfAnchorLayoutAttachEdge:
            {
            TAlfLayoutAttachEdgeParams* params =  (TAlfLayoutAttachEdgeParams*) aInputBuffer.Ptr();
            THuiMetric metric(params->iOffset.iMagnitude, params->iOffset.iUnit, params->iOffset.iReferenceTextStyleId);
            
            TInt error = anchorLayout->Attach( 
                params->iOrdinal,
                THuiAnchorType(params->iType),
                metric,
                THuiAnchorAttachmentOrigin(params->iAttachmentOrigin),
                params->iAttachmentOrdinal);
                
            User::LeaveIfError( error );

            break;
            }
            
        case EAlfAnchorLayoutAttachCorner:
            {
            TAlfLayoutAttachCornerParams* params =  (TAlfLayoutAttachCornerParams*) aInputBuffer.Ptr();

            THuiMetric metricX(params->iOffset.iX.iMagnitude, params->iOffset.iX.iUnit, params->iOffset.iX.iReferenceTextStyleId);
            THuiMetric metricY(params->iOffset.iY.iMagnitude, params->iOffset.iY.iUnit, params->iOffset.iY.iReferenceTextStyleId);

            THuiXYMetric metric(metricX, metricY);
            
            TInt error = anchorLayout->Attach( 
                params->iOrdinal,
                THuiAnchorType(params->iType),
                metric,
                THuiAnchorAttachmentOrigin(params->iAttachmentOrigin),
                params->iAttachmentOrdinal);
                
            User::LeaveIfError( error );

            break;
            }

        case EAlfAnchorLayoutAttachBox:
            {
            TAlfLayoutAttachBoxParams* params =  (TAlfLayoutAttachBoxParams*) aInputBuffer.Ptr();

            THuiMetric metricLeft(params->iOffset.iLeft.iMagnitude, params->iOffset.iLeft.iUnit, params->iOffset.iLeft.iReferenceTextStyleId);
            THuiMetric metricRight(params->iOffset.iRight.iMagnitude, params->iOffset.iRight.iUnit, params->iOffset.iRight.iReferenceTextStyleId);
            THuiMetric metricTop(params->iOffset.iTop.iMagnitude, params->iOffset.iTop.iUnit, params->iOffset.iTop.iReferenceTextStyleId);
            THuiMetric metricBottom(params->iOffset.iBottom.iMagnitude, params->iOffset.iBottom.iUnit, params->iOffset.iBottom.iReferenceTextStyleId);

            THuiBoxMetric metric(metricLeft, metricRight, metricTop, metricBottom);

            TInt error = anchorLayout->Attach( 
                params->iOrdinal,
                metric,
                THuiAnchorAttachmentOrigin(params->iAttachmentOrigin),
                params->iAttachmentOrdinal);
                
            User::LeaveIfError( error );

            break;
            }
            
        case EAlfAnchorLayoutDetachAll:
            {
            TInt* params =  (TInt*) aInputBuffer.Ptr();
            anchorLayout->Detach(*params);
            break;
            }

        case EAlfAnchorLayoutDetach:
            {
            TInt2* params =  (TInt2*) aInputBuffer.Ptr();
            anchorLayout->Detach( params->iInt1, (THuiAnchorType)params->iInt2 );
            break;
            }
            
        default:
            CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfAnchorLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }



//----------------------------
// LCT Pane layout
//----------------------------
EXPORT_C MAlfExtension* CAlfLCTAnchorLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfLCTAnchorLayoutHandler* me = new (ELeave) CAlfLCTAnchorLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfLCTAnchorLayoutHandler::CAlfLCTAnchorLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfAnchorLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfLCTAnchorLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiAnchorLayout* layout = CHuiAnchorLayout::AddNewL(aOwner, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfLCTAnchorLayoutHandler::~CAlfLCTAnchorLayoutHandler()
    {
    }

EXPORT_C void CAlfLCTAnchorLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfLCTAnchorLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfLCTAnchorLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    switch (aCommandId)
        {
        case EAlfLCTAnchorLayoutSetAnchors:
            {
#ifdef RD_ALF_IN_PLATFORM
            CHuiAnchorLayout* anchorLayout = static_cast<CHuiAnchorLayout*>(iVisual); 
            TAlfLCTAnchorSetAnchorsParams* params = (TAlfLCTAnchorSetAnchorsParams*)aInputBuffer.Ptr();

            TAknLayoutHierarchyComponentHandle handle;
            handle.SetApiId(params->iApiId);
            handle.SetComponentId(params->iComponentId);
            handle.SetVarietyIndex(params->iVarietyIndex);
            handle.SetColumn(params->iColumn);
            handle.SetRow(params->iRow);

            THuiTimedPoint tpoint;
            AlfTimedPointUtility::CopyTimedPoint(params->iOffset, tpoint, iResolver);
    
            // we can access the layout data as a window line, whatever type component is specified.
            TAknWindowLineLayout componentLayout = AknLayout2Hierarchy::GetWindowComponentLayout(handle).LayoutLineNoEmptys();
            SetAnchors(anchorLayout, params->iOrdinal, componentLayout, tpoint);
#endif

            break;
            }
            
        default:
            CAlfAnchorLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfLCTAnchorLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfAnchorLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }

#ifdef RD_ALF_IN_PLATFORM
 TBool CAlfLCTAnchorLayoutHandler::IsParentRelative(TInt aVal) const
    {
    return aVal > ELayoutP-ELayoutPRange && aVal <= ELayoutP;
    }
#else //RD_ALF_IN_PLATFORM
 TBool CAlfLCTAnchorLayoutHandler::IsParentRelative(TInt /*aVal*/) const
    {
    return EFalse;
    }
#endif //  RD_ALF_IN_PLATFORM

#ifdef RD_ALF_IN_PLATFORM
void CAlfLCTAnchorLayoutHandler::SetAnchors(CHuiAnchorLayout* aAnchor, TInt aOrdinal, const TAknWindowLineLayout& aLine, const THuiTimedPoint& aTimedPoint)
    {
    DoSetAnchors(aAnchor, aOrdinal, aLine.il, aLine.it, aLine.ir, aLine.ib, aTimedPoint);
    }
#else //RD_ALF_IN_PLATFORM
void CAlfLCTAnchorLayoutHandler::SetAnchors(CHuiAnchorLayout* /*aAnchor*/, TInt /*aOrdinal*/, const TAknWindowLineLayout& /*aLine*/, const THuiTimedPoint& /*aTimedPoint*/)
    {
    }
#endif //  RD_ALF_IN_PLATFORM
    
#ifdef RD_ALF_IN_PLATFORM
void CAlfLCTAnchorLayoutHandler::DoSetAnchors(CHuiAnchorLayout* aLayout, TInt aOrdinal, TInt aLeft, TInt aTop, TInt aRight, TInt aBottom, const THuiTimedPoint& aTimedPoint)
    {
    TInt left = aLeft;
    TInt top = aTop;
    TInt right = aRight;
    TInt bottom = aBottom;
    THuiAnchorOrigin originLeft = EHuiAnchorOriginLeft;
    THuiAnchorOrigin originTop = EHuiAnchorOriginTop;
    THuiAnchorOrigin originRight = EHuiAnchorOriginRight;
    THuiAnchorOrigin originBottom = EHuiAnchorOriginBottom;

    // when measuring from the opposite anchor, have to change sign
    if(IsParentRelative(left))
        {
        left = -(ELayoutP - left);
        originLeft = EHuiAnchorOriginRight;
        }
    if(IsParentRelative(top))
        {
        top = -(ELayoutP - top);
        originTop = EHuiAnchorOriginBottom;
        }
    if(IsParentRelative(right))
        {
        right = -(ELayoutP - right);
        originRight = EHuiAnchorOriginLeft;
        }
    if(IsParentRelative(bottom))
        {
        bottom = -(ELayoutP - bottom);
        originBottom = EHuiAnchorOriginTop;
        }
    THuiTimedPoint point(aTimedPoint);

    point.iX = left;
    point.iY = top;
    aLayout->SetAnchor(
        EHuiAnchorTopLeft, aOrdinal,
        originLeft, originTop,
        EHuiAnchorMetricAbsolute, EHuiAnchorMetricAbsolute,
        point);

    // right and bottom anchors are still measured in positive x and y
    point.iX = -right;
    point.iY = -bottom;
    aLayout->SetAnchor(
        EHuiAnchorBottomRight, aOrdinal,
        originRight, originBottom,
        EHuiAnchorMetricAbsolute, EHuiAnchorMetricAbsolute,
        point); 
    }
#else // RD_ALF_IN_PLATFORM
void CAlfLCTAnchorLayoutHandler::DoSetAnchors(CHuiAnchorLayout* /*aLayout*/, TInt /*aOrdinal*/, TInt /*aLeft*/, TInt /*aTop*/, TInt /*aRight*/, TInt /*aBottom*/, const THuiTimedPoint& /*aTimedPoint*/)
    {
    }
#endif // RD_ALF_IN_PLATFORM



//----------------------------
// grid layout
//----------------------------
EXPORT_C MAlfExtension* CAlfGridLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfGridLayoutHandler* me = new (ELeave) CAlfGridLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfGridLayoutHandler::CAlfGridLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfGridLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiGridLayout* layout = CHuiGridLayout::AddNewL(aOwner, 0,0, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfGridLayoutHandler::~CAlfGridLayoutHandler()
    {
    }


EXPORT_C void CAlfGridLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfGridLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfGridLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiGridLayout* gridLayout = static_cast<CHuiGridLayout*>(iVisual); 
    switch (aCommandId)
        {
        case EAlfGridLayoutSetColumns:
            {
            TInt* count = (TInt*) aInputBuffer.Ptr();
            gridLayout->SetColumnsL(*count);
            break;
            }

        case EAlfGridLayoutSetRows:
            {
            TInt* count = (TInt*) aInputBuffer.Ptr();
            gridLayout->SetRowsL(*count);
            break;
            }
            
        case EAlfGridLayoutSetColumnWeights:
            {
            RArray<TInt> array;
            CleanupClosePushL( array );
            InternalizeL(aInputBuffer, array);
            gridLayout->SetColumnsL(array);
            CleanupStack::PopAndDestroy( &array );
            break;
            }
            
        case EAlfGridLayoutSetRowsWeights:
            {
            RArray<TInt> array;
            CleanupClosePushL( array );
            InternalizeL(aInputBuffer, array);
            gridLayout->SetRowsL(array);
            CleanupStack::PopAndDestroy( &array );
            break;
            }

        case EAlfGridLayoutSetExpanding:
            {
            TInt* flags = (TInt*) aInputBuffer.Ptr();
            gridLayout->SetExpanding(*flags);
            break;
            }
                    
        case EAlfGridLayoutOrdinalToBlock:
            {
            TInt* ordinal = (TInt*) aInputBuffer.Ptr();
            TPoint pos(0,0);
            TPckg<TPoint> posPckg(pos);
            pos = gridLayout->OrdinalToBlock(*ordinal);
            aResponse = posPckg;
            break;
            }

        case EAlfGridLayoutColumnCount:
            {
            TInt count(0);
            TPckg<TInt> countPckg(count);
            count = gridLayout->DimensionCount(EHuiGridColumn);
            aResponse = countPckg;
            break;
            }
            
        case EAlfGridLayoutRowCount:
            {
            TInt count(0);
            TPckg<TInt> countPckg(count);
            count = gridLayout->DimensionCount(EHuiGridRow);
            aResponse = countPckg;
            break;
            }
            
        case EAlfGridLayoutFillWeights:
            {
            TAlfGridLayoutFillWeightsParams* params = (TAlfGridLayoutFillWeightsParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            THuiMetric metric(params->iWeight.iMagnitude, params->iWeight.iUnit, params->iWeight.iReferenceTextStyleId);
            gridLayout->FillWeightsL(dim, params->iCount, metric);
            break;
            }

        case EAlfGridLayoutAppendWeight:
            {
            TAlfGridLayoutAppendWeightParams* params = (TAlfGridLayoutAppendWeightParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            THuiMetric metric(params->iWeight.iMagnitude, params->iWeight.iUnit, params->iWeight.iReferenceTextStyleId);
            gridLayout->AppendWeightL(dim, metric);
            break;
            }
            
        case EAlfGridLayoutInsertWeight:
            {
            TAlfGridLayoutInsertWeightParams* params = (TAlfGridLayoutInsertWeightParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            THuiMetric metric(params->iWeight.iMagnitude, params->iWeight.iUnit, params->iWeight.iReferenceTextStyleId);
            gridLayout->InsertWeightL(dim, metric, params->iPos);
            break;
            }
            
        case EAlfGridLayoutReplaceWeight:
            {
            TAlfGridLayoutReplaceWeightParams* params = (TAlfGridLayoutReplaceWeightParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            THuiMetric metric(params->iWeight.iMagnitude, params->iWeight.iUnit, params->iWeight.iReferenceTextStyleId);
            gridLayout->ReplaceWeightL(dim, metric, params->iPos);
            break;
            }
            
        case EAlfGridLayoutRemoveWeight:
            {
            TAlfGridLayoutRemoveWeightParams* params = (TAlfGridLayoutRemoveWeightParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            gridLayout->RemoveWeightL(dim, params->iPos);
            break;
            }
            
        case EAlfGridLayoutWeight:
            {
            TAlfGridLayoutWeightParams* params = (TAlfGridLayoutWeightParams*)aInputBuffer.Ptr();
            THuiGridDimension dim = (THuiGridDimension)params->iDim;
            THuiMetric huiWeight = gridLayout->Weight(dim, params->iPos);

            TAlfMetric alfWeight(huiWeight.iMagnitude, huiWeight.iUnit, huiWeight.iReferenceTextStyleId);;
            TPckg<TAlfMetric> weightPckg(alfWeight);
            aResponse = weightPckg;
            break;
            }
            
        case EAlfGridLayoutCount:
            {
            THuiGridDimension* dim = (THuiGridDimension*) aInputBuffer.Ptr();
            TInt count(0);
            TPckg<TInt> countPckg(count);
            count = gridLayout->DimensionCount(*dim);
            aResponse = countPckg;
            break;
            }
            
        case EAlfGridLayoutSetLayoutModeFlags:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            gridLayout->SetLayoutModeFlags((THuiGridDimension)params->iInt1, (TUint)params->iInt2);
            break;                
            }
                    
        case EAlfGridLayoutLayoutModeFlags:
            {
            THuiGridDimension* dim = (THuiGridDimension*) aInputBuffer.Ptr();
            TInt flags(0);
            TPckg<TInt> flagsPckg(flags);
            flags = gridLayout->LayoutModeFlags(*dim);
            aResponse = flagsPckg;            
            break;                
            }
            
        case EAlfGridLayoutClearLayoutModeFlags:                
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            gridLayout->ClearLayoutModeFlags((THuiGridDimension)params->iInt1, (TUint)params->iInt2);         
            break;                
            }
            
        default:
            CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
        
    }

EXPORT_C void CAlfGridLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }

//----------------------------
// LCT grid layout
//----------------------------
EXPORT_C MAlfExtension* CAlfLCTGridLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfLCTGridLayoutHandler* me = new (ELeave) CAlfLCTGridLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfLCTGridLayoutHandler::CAlfLCTGridLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfGridLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfLCTGridLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiGridLayout* layout = CHuiGridLayout::AddNewL(aOwner, 1, 1, aParentLayout);     
        CAlfGridLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfGridLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfLCTGridLayoutHandler::~CAlfLCTGridLayoutHandler()
    {
    }


EXPORT_C void CAlfLCTGridLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfLCTGridLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfGridLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfLCTGridLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    switch (aCommandId)
        {
        case EAlfLCTGridLayoutSetColsAndRows:
            {
#ifdef RD_ALF_IN_PLATFORM
            CHuiGridLayout* gridLayout = static_cast<CHuiGridLayout*>(iVisual); 
            TAlfLCTGridSetColsAndRowsParams* params = (TAlfLCTGridSetColsAndRowsParams*)aInputBuffer.Ptr();

            TAknLayoutHierarchyComponentHandle handle;
            handle.SetApiId(params->iApiId);
            handle.SetComponentId(params->iComponentId);
            handle.SetVarietyIndex(params->iVarietyIndex);

            THuiTimedPoint tpoint;
            AlfTimedPointUtility::CopyTimedPoint(params->iOffset, tpoint, iResolver);
    
            // get the cols and rows directly from the param limits
            // @todo maybe need to use anchors, or set rows and cols by item height?
            TAknLayoutScalableParameterLimits limits = AknLayout2Hierarchy::GetParamLimits(handle);
            gridLayout->SetColumnsL(limits.LastColumn()+1);
            gridLayout->SetRowsL(limits.LastRow()+1);
#endif
            break;
            }
        default:
            CAlfGridLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
        
    }

EXPORT_C void CAlfLCTGridLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfGridLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }
 
//----------------------------
// deck layout
//----------------------------
EXPORT_C MAlfExtension* CAlfDeckLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfDeckLayoutHandler* me = new (ELeave) CAlfDeckLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfDeckLayoutHandler::CAlfDeckLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfDeckLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiDeckLayout* layout = CHuiDeckLayout::AddNewL(aOwner, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfDeckLayoutHandler::~CAlfDeckLayoutHandler()
    {
    }

EXPORT_C TAny* CAlfDeckLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfDeckLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
    }

EXPORT_C void CAlfDeckLayoutHandler::Release()
    {
    delete this;
    }

EXPORT_C void CAlfDeckLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }


//----------------------------
// flow layout
//----------------------------
EXPORT_C MAlfExtension* CAlfFlowLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfFlowLayoutHandler* me = new (ELeave) CAlfFlowLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfFlowLayoutHandler::CAlfFlowLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfFlowLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiFlowLayout* layout = CHuiFlowLayout::AddNewL(aOwner, CHuiFlowLayout::EFlowVertical, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfFlowLayoutHandler::~CAlfFlowLayoutHandler()
    {
    }


EXPORT_C void CAlfFlowLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfFlowLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfFlowLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiFlowLayout* flowLayout = static_cast<CHuiFlowLayout*>(iVisual); 
    switch (aCommandId)
        {
        case EAlfFlowLayoutSetFlowDirection:
            {
            CHuiFlowLayout::TFlowDirection* params =  (CHuiFlowLayout::TFlowDirection*) aInputBuffer.Ptr();
            flowLayout->SetFlowDirection( *params );
            break;
            }
        case EAlfFlowLayoutSetCentering:
            {
            TBool* params =  (TBool*) aInputBuffer.Ptr();
            flowLayout->SetCentering( *params );
            break;
            }
        case EAlfFlowLayoutSetMode:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();
            flowLayout->SetMode( *params );
            break;
            }
           
        default:
            CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }


EXPORT_C void CAlfFlowLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }

 
 // CURVE PATH LAYOUT
 
 
struct CAlfCurvePathLayoutHandler::TCurvePathLayoutPrivateData
    {
    };
 
EXPORT_C MAlfExtension* CAlfCurvePathLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfCurvePathLayoutHandler* me = new (ELeave) CAlfCurvePathLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }
    
EXPORT_C CAlfCurvePathLayoutHandler::CAlfCurvePathLayoutHandler(MAlfInterfaceProvider& aResolver)
:CAlfLayoutHandler(aResolver)
    {
    }
    
EXPORT_C void CAlfCurvePathLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiCurvePathLayout* layout = CHuiCurvePathLayout::AddNewL(aOwner, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
        
    // Not needed currently
    //iCurvePathLayoutPrivateData = new (ELeave) TCurvePathLayoutPrivateData;
    }
    
EXPORT_C CAlfCurvePathLayoutHandler::~CAlfCurvePathLayoutHandler()
    {
    if ( iCurvePathLayoutPrivateData )
        {
        // Delete private data (not used currently)
        }
    delete iCurvePathLayoutPrivateData;
    iCurvePathLayoutPrivateData = NULL;
    }

EXPORT_C void CAlfCurvePathLayoutHandler::Release()
    {
    delete this;
    }
    
EXPORT_C TAny* CAlfCurvePathLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    CHuiCurvePathLayout* curvePathLayout = static_cast<CHuiCurvePathLayout*>(iVisual); 
    
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return static_cast<MHuiMappingFunction*>(curvePathLayout);
        case EHuiObjectTypeCurvePathProvider:
            return &curvePathLayout->CurvePath();
        default:
            return CAlfLayoutHandler::GetInterface(aInterface);
        }
    }
    
EXPORT_C void CAlfCurvePathLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiCurvePathLayout* curvePathLayout = static_cast<CHuiCurvePathLayout*>(iVisual); 
    
    switch( aCommandId )
        {
        case EAlfCurvePathLayoutMapValue:
            {
            TIntTReal* mapValues = (TIntTReal*) aInputBuffer.Ptr();
            const TReal32 returnValue = curvePathLayout->MapValue( mapValues->iReal, mapValues->iInt );
            TPckg<TReal32> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
        default:
            CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
            break;
        }
    }

EXPORT_C void CAlfCurvePathLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }

//----------------------------
// viewport layout
//----------------------------
EXPORT_C MAlfExtension* CAlfViewportLayoutHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfViewportLayoutHandler* me = new (ELeave) CAlfViewportLayoutHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfViewportLayoutHandler::CAlfViewportLayoutHandler(MAlfInterfaceProvider& aResolver):CAlfLayoutHandler(aResolver)
    {
    }


EXPORT_C void CAlfViewportLayoutHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiViewportLayout* layout = CHuiViewportLayout::AddNewL(aOwner, aParentLayout);     
        CAlfLayoutHandler::ConstructL(layout, aOwner, aParentLayout);
        }
    else
        {
        CAlfLayoutHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfViewportLayoutHandler::~CAlfViewportLayoutHandler()
    {
    }

EXPORT_C void CAlfViewportLayoutHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfViewportLayoutHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfLayoutHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfViewportLayoutHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiViewportLayout* viewportLayout = static_cast<CHuiViewportLayout*>(iVisual); 
    switch (aCommandId)
        {
        case EAlfViewportLayoutSetVirtualSize:
            {
            TAlfTimedPoint* newSize = (TAlfTimedPoint*) aInputBuffer.Ptr();

            THuiTimedPoint tpoint;
            AlfTimedPointUtility::CopyTimedPoint(*newSize, tpoint, iResolver);                  
            
            viewportLayout->SetVirtualSize(tpoint.RealTarget().AsSize(), HUI_ROUND_FLOAT_TO_INT(newSize->TimeToTargetinMilliSeconds().iX));            
            break;
            }
            
        case EAlfViewportLayoutSetViewportSize:
            {
            TAlfTimedPoint* newSize = (TAlfTimedPoint*) aInputBuffer.Ptr();

            THuiTimedPoint tpoint;      
            AlfTimedPointUtility::CopyTimedPoint(*newSize, tpoint, iResolver);                              
            viewportLayout->SetViewportSize(tpoint.RealTarget().AsSize(), HUI_ROUND_FLOAT_TO_INT(newSize->TimeToTargetinMilliSeconds().iX));            
            break;
            }
            
        case EAlfViewportLayoutSetViewportPos:
            {
            TAlfTimedPoint* newPos = (TAlfTimedPoint*) aInputBuffer.Ptr();

            THuiTimedPoint tpoint;
            AlfTimedPointUtility::CopyTimedPoint(*newPos, tpoint, iResolver);                              
            viewportLayout->SetViewportPos(tpoint.RealTarget(), HUI_ROUND_FLOAT_TO_INT(newPos->TimeToTargetinMilliSeconds().iX));            
            break;
            }
            
        default:
            CAlfLayoutHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfViewportLayoutHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfLayoutHandler::VisualHandlerExtension(aExtensionUid, aExtensionParameters);
    }


// end of file
