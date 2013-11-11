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
* Description:   Anchor layout
*
*/



#include "alf/alfanchorlayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

/** Specifies a cardinal point for an anchor corner. These would
 *  typically be used in pairs to specify the top left and bottom right
 *  anchor points of a layout. 
 */
NONSHARABLE_STRUCT(TCornerAnchor)
        {
        /** <code>ETrue</code>, if this corner anchor should be applied. */
        TBool iActive;

        /** Horizontal origin. */
        TAlfAnchorOrigin iHorizontalOrigin;

        /** Vertical origin. */
        TAlfAnchorOrigin iVerticalOrigin;

        /** Horizontal offset type. */
        TAlfAnchorMetric iHorizontalMetric;

        /** Horizontal offset type. */
        TAlfAnchorMetric iVerticalMetric;

        /** Offset in coordinates or relative to size (e.g., 1.0 * width). */
        TAlfTimedPoint iOffset;
        };

/** Anchor that defines a visual's position and/or size. */
NONSHARABLE_STRUCT(TAnchor)
        {
        /** Child ordinal to which the anchor applies. */
        TInt iOrdinal;

        /** Anchor for the top left corner (i.e., SetPos(tl)). */
        TCornerAnchor iTl;

        /** Anchor for the bottom right corner (i.e., SetSize(br - tl)). */
        TCornerAnchor iBr;
        };



// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfAnchorLayout::CAlfAnchorLayout()
    : CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfAnchorLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfAnchorLayout::~CAlfAnchorLayout()
    {
    }


// ---------------------------------------------------------------------------
// Creates new anchor layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfAnchorLayout* CAlfAnchorLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfAnchorLayout* layout = STATIC_CAST(CAlfAnchorLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeAnchor, aParentLayout));
    return layout;
    }

// ---------------------------------------------------------------------------
// Resets anchors.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfAnchorLayout::Reset()
    {
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutReset, KNullDesC8(), outDum );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Reset panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Sets anchor
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfAnchorLayout::SetAnchor(
    TAlfAnchor aAnchor, 
    TInt aOrdinal,
    TAlfAnchorOrigin aHorizOrigin,
    TAlfAnchorOrigin aVertOrigin,
    TAlfAnchorMetric aHorizMetric,
    TAlfAnchorMetric aVertMetric,
    const TAlfTimedPoint& aOffset)
    {
    TAlfLayoutSetAnchorParams params;
    params.iAnchor = aAnchor;
    params.iOrdinal = aOrdinal;
    params.iHorizOrigin = aHorizOrigin;
    params.iVertOrigin = aVertOrigin;
    params.iHorizMetric = aHorizMetric;
    params.iVertMetric = aVertMetric;
    params.iOffset = aOffset;
    
    TPckg<TAlfLayoutSetAnchorParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutSetAnchor, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::SetAnchor return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Set relative anchor rect
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfAnchorLayout::SetRelativeAnchorRect(TInt aOrdinal,
                                        TAlfAnchorOrigin aTlHorizOrigin,
                                        TAlfAnchorOrigin aTlVertOrigin,
                                        const TAlfRealPoint& aTopLeftOffset,
                                        TAlfAnchorOrigin aBrHorizOrigin,
                                        TAlfAnchorOrigin aBrVertOrigin,
                                        const TAlfRealPoint& aBottomRightOffset)
    {
    TAlfLayoutSetRelativeAnchorRectParams params;
    params.iOrdinal = aOrdinal;
    params.iTlHorizOrigin = aTlHorizOrigin;
    params.iTlVertOrigin = aTlVertOrigin;
    params.iTopLeftOffset = aTopLeftOffset;
    params.iBrHorizOrigin = aBrHorizOrigin;
    params.iBrVertOrigin = aBrVertOrigin;
    params.iBottomRightOffset = aBottomRightOffset;
    
    TPckg<TAlfLayoutSetRelativeAnchorRectParams> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutSetRelativeAnchorRect, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::SetRelativeAnchorRect return error %d", err )
        }
    return err;
    }
   
// ---------------------------------------------------------------------------
// Removes an anchor.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfAnchorLayout::RemoveAnchor(TAlfAnchor aAnchor, TInt aOrdinal)
    {
    TInt2 params( aAnchor, aOrdinal );
    TPckg<TInt2> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutRemoveAnchor, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::RemoveAnchor panic error %d", err )
        USER_INVARIANT(); 
        }
    }

// ---------------------------------------------------------------------------
// Attach edge
// ---------------------------------------------------------------------------
EXPORT_C TInt CAlfAnchorLayout::Attach(
    TInt aOrdinal,
    TAlfAnchorType aType, 
    const TAlfMetric& aOffset,
    TAlfAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
    {
    TAlfMetric metric(aOffset);
    
    TAlfLayoutAttachEdgeParams params = 
        {
        aOrdinal,
        aType, 
        metric,
        aAttachmentOrigin,
        aAttachmentOrdinal
        };

    TPckg<TAlfLayoutAttachEdgeParams> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutAttachEdge, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Attach error %d", err )
        }
        
    return err;
    }



// ---------------------------------------------------------------------------
// Attach corner
// ---------------------------------------------------------------------------
EXPORT_C TInt CAlfAnchorLayout::Attach(
    TInt aOrdinal,
    TAlfAnchorType aType, 
    const TAlfXYMetric& aOffset,
    TAlfAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
    {
    TAlfLayoutAttachCornerParams params = 
        {
        aOrdinal,
        aType, 
        aOffset,
        aAttachmentOrigin,
        aAttachmentOrdinal
        };

    TPckg<TAlfLayoutAttachCornerParams> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutAttachCorner, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Attach error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Attach box
// ---------------------------------------------------------------------------
EXPORT_C TInt CAlfAnchorLayout::Attach(
    TInt aOrdinal,
    const TAlfBoxMetric& aOffset,
    TAlfAnchorAttachmentOrigin aAttachmentOrigin,
    TInt aAttachmentOrdinal)
    {
    TAlfLayoutAttachBoxParams params = 
        {
        aOrdinal,
        aOffset,
        aAttachmentOrigin,
        aAttachmentOrdinal
        };

    TPckg<TAlfLayoutAttachBoxParams> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutAttachBox, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Attach error %d", err )
        }
    return err;        
    }


// ---------------------------------------------------------------------------
// Detach all anchors
// ---------------------------------------------------------------------------
EXPORT_C void CAlfAnchorLayout::Detach(TInt aOrdinal)
    {
    TInt param( aOrdinal );
    TPckg<TInt> paramsPckg(param);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutDetachAll, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Detach panic error %d", err )
        USER_INVARIANT(); 
        }    
    }
    
// ---------------------------------------------------------------------------
// Detach anchor
// ---------------------------------------------------------------------------
EXPORT_C void CAlfAnchorLayout::Detach(TInt aOrdinal, TAlfAnchorType aType)
    {
    TInt2 params( aOrdinal, aType );
    TPckg<TInt2> paramsPckg(params);
    TBuf8<1> outDum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfAnchorLayoutDetach, paramsPckg, outDum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfAnchorLayout::Detach panic error %d", err )
        USER_INVARIANT(); 
        }    
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfAnchorLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfAnchorLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfAnchorLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfAnchorLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfAnchorLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfAnchorLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    

