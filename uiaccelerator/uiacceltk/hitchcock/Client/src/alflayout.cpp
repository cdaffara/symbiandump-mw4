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
* Description:   Base class for layouts.
*
*/



#include "alf/alflayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

// Private structure
struct CAlfLayout::TLayoutPrivateData
    {
    TLayoutPrivateData():iScrollOffset(0),iSkipServer(EFalse), 
        iTransitionTime(KAlfLayoutDefaultTransitionTime){}
    RPointerArray<CAlfVisual> iChildren;    // not owned.
    TAlfTimedPoint* iScrollOffset;          // owned.
    TBool iSkipServer;                      // should rather use flags than booleans                          
    TInt iTransitionTime;
    TAlfXYMetric iInnerPadding;
    };



// ======== LOCAL FUNCTIONS ========

void AssertInnerPaddingsWereSetUsingDeprecatedPixelAPIs(const TAlfXYMetric& aMetric)
    {
    __ASSERT_DEBUG(
        (aMetric.iX.iMagnitude == aMetric.iY.iMagnitude) && 
        (aMetric.iX.iUnit == EAlfUnitPixel) &&
        (aMetric.iY.iUnit == EAlfUnitPixel)
        , USER_INVARIANT());
#ifndef _DEBUG // Remove compile warnings
    TAlfXYMetric tmp = aMetric;
    tmp = tmp;
#endif
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayout::CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayout::ConstructL( CAlfControl& aOwner )
    {
    CAlfVisual::ConstructL( aOwner );
    
    iLayoutData = new (ELeave) TLayoutPrivateData;
    iLayoutData->iInnerPadding = TAlfXYMetric(TAlfMetric(0));
    }


// ---------------------------------------------------------------------------
// Create new layout.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayout* CAlfLayout::AddNewL(
    CAlfControl& aOwner,
    CAlfLayout* aParentLayout )
    {
    CAlfLayout* layout = aOwner.AppendLayoutL(
        EAlfLayoutTypeLayout, 
        aParentLayout);
    return layout;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayout::~CAlfLayout()
    {
    if (iLayoutData )
        {
        // Remove children in reverse order.
        for(TInt i = iLayoutData->iChildren.Count() - 1; i >= 0; --i)
            {
            Remove(iLayoutData->iChildren[i]);
            }
        iLayoutData->iChildren.Reset();
        
        delete iLayoutData->iScrollOffset;
        iLayoutData->iScrollOffset = NULL;
        }
    delete iLayoutData;
    iLayoutData = NULL;
    }

// ---------------------------------------------------------------------------
// Appends new visual as a child.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfLayout::RemoveAndDestroyAllD()
    {
    TBuf8<1> dum;
    // Remove and destory from the server side
    TInt err = Comms()->DoSynchronousCmd( EAlfVisualRemoveAndDestroyAll,KNullDesC8, dum);
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfLayout::RemoveAndDestroyAllD ignore error %d", err )
        }
            
    DoRemoveAndDestroyAllD();
    }
    
EXPORT_C void CAlfLayout::DoRemoveAndDestroyAllD()
    {
    iLayoutData->iSkipServer = ETrue;
    for(TInt i = iLayoutData->iChildren.Count() - 1; i >= 0; --i)
        {
        iLayoutData->iChildren[i]->DoRemoveAndDestroyAllD();
        }
    iLayoutData->iChildren.Reset();

    // Calls the server function and removes the link
    // from the owner
    CAlfVisual::DoRemoveAndDestroyAllD();
    }


// ---------------------------------------------------------------------------
// Appends new visual as a child.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfLayout::Append( CAlfVisual* aVisual, TBool aConstructedWithParentInformation, TInt aLayoutTransitionTime )
    {
    ASSERT( aVisual && aVisual->Identifier());
    
    __ASSERT_ALWAYS( iLayoutData->iChildren.Find(aVisual) == KErrNotFound,
                     USER_INVARIANT() );

    TInt err = iLayoutData->iChildren.Append(aVisual);
    if ( err != KErrNone )
        {
        return err;
        }
    
    if (!aConstructedWithParentInformation) // other wise visual was added to this layout 
        {                                   // during construction -> we mustn't apped visual to same layout twice 
        
        TInt2 params(aVisual->Identifier(), aLayoutTransitionTime);
        TPckgC<TInt2> buf(params);        
        TBuf8<1> dum;
    
        err = Comms()->DoSynchronousCmd(EAlfLayoutAppendVisual, buf, dum);    
        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfLayout::Append return error %d", err )
            }
        }
    
    if ( err == KErrNone )
        {
        aVisual->SetLayout(this);
        }
    else    
        {
        // remove the last one
        iLayoutData->iChildren.Remove( iLayoutData->iChildren.Count()-1 );
        }
    return err;
    }
 
// ---------------------------------------------------------------------------
// Removes visual
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfLayout::Remove(CAlfVisual* aVisual, TInt aLayoutTransitionTime)
    {
    ASSERT( aVisual );
    TInt index = iLayoutData->iChildren.Find(aVisual);
    __ASSERT_DEBUG(index != KErrNotFound,
                   USER_INVARIANT());
    if(index != KErrNotFound)
        {
        if ( !iLayoutData->iSkipServer )
            {
            TInt2 params(aVisual->Identifier(), aLayoutTransitionTime);
            TPckgC<TInt2> buf(params);        
            TBuf8<1> dum;
            TInt err = Comms()->DoSynchronousCmd(EAlfLayoutRemoveVisual, buf, dum );
        
            if ( err )
                {
                __ALFLOGSTRING1( "CAlfLayout::Remove panic error %d", err )
                USER_INVARIANT();
                }
            }
     
        iLayoutData->iChildren.Remove(index);
        aVisual->SetLayout(NULL);
        }
    }

EXPORT_C void CAlfLayout::Reorder(CAlfVisual& aVisual, TInt aPosition, TInt aLayoutTransitionTime)
    {
    TInt index = iLayoutData->iChildren.Find(&aVisual);
    if(index == aPosition)
        {
        // No need to change anything.
        return;
        }
    
    ASSERT( aPosition >= 0 && aPosition < iLayoutData->iChildren.Count());
    ASSERT( index != KErrNotFound );
    TInt3 params(aVisual.Identifier(), aPosition, aLayoutTransitionTime);
    
    TPckgC<TInt3> buf(params);
    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutReorderVisual, buf, dum );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::Reorder panic error %d", err )
        USER_INVARIANT();  
        }
    
    // Move the children around in the array, so that aVisual ends up
    // at aPosition.
    TInt direction = (aPosition>index)? 1:-1;
    for(TInt i = index; i != aPosition; i += direction)
        {
        iLayoutData->iChildren[i] = iLayoutData->iChildren[i + direction];
        }
    iLayoutData->iChildren[aPosition] = &aVisual;        

    iLayoutData->iSkipServer = ETrue; // don't post update to server unless derived class implements its own 
                                      // override for UpdateChildrenLayout  

    // use time set in CAlfEnv::StaticSetTransitionTime()
    UpdateChildrenLayout(KAlfLayoutDefaultTransitionTime); // should use LOCAL time like Env.StaticTransitionTime()

    iLayoutData->iSkipServer = EFalse;
    }

EXPORT_C void CAlfLayout::MoveVisualToFront(CAlfVisual& aVisual, TInt aLayoutTransitionTime)
    {
    ASSERT(iLayoutData);
    Reorder(aVisual, iLayoutData->iChildren.Count() - 1, aLayoutTransitionTime);
    }


EXPORT_C void CAlfLayout::MoveVisualToBack(CAlfVisual& aVisual, TInt aLayoutTransitionTime)
    {
    Reorder(aVisual, 0, aLayoutTransitionTime);
    }


 
// ---------------------------------------------------------------------------
// Returns visual count
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CAlfLayout::Count() const
    {
    return iLayoutData->iChildren.Count();
    }
 
// ---------------------------------------------------------------------------
// Returns indexed visual
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfVisual& CAlfLayout::Visual(TInt aIndex) const
    {
    return *iLayoutData->iChildren[aIndex];
    }

// ---------------------------------------------------------------------------
// Enables scrolling
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLayout::EnableScrollingL( TBool aScrolling )
    {
    // Already enabled?
    if ( aScrolling && iLayoutData->iScrollOffset )
        {
        return;
        }
        
    // Already disabled?
    if ( !aScrolling && !iLayoutData->iScrollOffset )
        {
        return;
        }
    
    // Allocate scroll object if enabling.
    if ( aScrolling )
        {
        iLayoutData->iScrollOffset = new (ELeave) TAlfTimedPoint;
        }

    TPckgC<TInt> buf(aScrolling);
    TBuf8<1> dum;
    
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutEnableScrolling, buf, dum);
    
    // If disabled from server side, free the old scrolling object on client side.
    if ( err == KErrNone && !aScrolling )
        {
        delete iLayoutData->iScrollOffset;
        iLayoutData->iScrollOffset = NULL;
        }
    
    // If error on enabling the server side, we need to free the allocated scroll 
    // object on the client side.
    if ( err != KErrNone && aScrolling )
        {
        delete iLayoutData->iScrollOffset;
        iLayoutData->iScrollOffset = NULL;
        }
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::EnableScrollingL leave error %d", err )
        User::Leave( err );
        }
    }
    
// ---------------------------------------------------------------------------
// Is scrolling enabled?
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CAlfLayout::Scrolling() const
    {
    return iLayoutData->iScrollOffset != NULL;
    }

// ---------------------------------------------------------------------------
// Returns scrolling offset
// ---------------------------------------------------------------------------
//  
EXPORT_C const TAlfTimedPoint& CAlfLayout::ScrollOffset() const
    {
    __ASSERT_ALWAYS( iLayoutData->iScrollOffset, USER_INVARIANT() );
 

    TPckg<TAlfTimedPoint> offsetPckg(*iLayoutData->iScrollOffset);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutScrollOffset, KNullDesC8, offsetPckg);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::ScrollOffset ignore error %d", err )
        }

    return *iLayoutData->iScrollOffset;
    }
  
// ---------------------------------------------------------------------------
// Sets scrolling offset
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfLayout::SetScrollOffset(const TAlfTimedPoint& aPoint )
    {
    __ASSERT_ALWAYS( iLayoutData->iScrollOffset, USER_INVARIANT() );
    
    iLayoutData->iScrollOffset->iX = aPoint.iX;
    iLayoutData->iScrollOffset->iY = aPoint.iY;


    TPckgC<TAlfTimedPoint> offsetPckg(*iLayoutData->iScrollOffset);
    
    TInt err = Comms()->DoCmdNoReply(EAlfLayoutSetScrollOffset, offsetPckg );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::SetScrollOffset panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Returns indexed child ordinal.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfLayout::ChildOrdinal(TInt aIndex)
    {
    TInt ordinal = 0;
    
    TPckgC<TInt> buf(aIndex);
    TPckg<TInt> ordinalPckg(ordinal);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutChildOrdinal, buf, ordinalPckg);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::ChildOrdinal panic error %d", err )
        USER_INVARIANT();
        }

    return ordinal;        
    }

// ---------------------------------------------------------------------------
// Returns position of the child in the given ordinal value. 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfLayout::ChildPos(TInt aOrdinal, TPoint& aPos)
    {    
    TAlfPosInt pos = 
        {
        aPos,
        aOrdinal
        };
        
    TPckgC<TAlfPosInt> posBuf(pos);
    //TBool reply(EFalse);
    TPckg<TAlfPosInt> rBuf(pos); 
   
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutChildPos, posBuf, rBuf);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::ChildPos panic error %d", err )
        USER_INVARIANT();
        }

    aPos = pos.iPoint;
    return pos.iInt;        
    }
    
// ---------------------------------------------------------------------------
// Returns size of the child in the given ordinal value. 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfLayout::ChildSize(TInt aOrdinal, TSize& aSize)
    {
    TAlfSizeInt size = 
        {
        aSize,
        aOrdinal
        };
        
    TPckgC<TAlfSizeInt> sizeBuf(size);
    //TBool reply(EFalse);
    TPckg<TAlfSizeInt> rBuf(size); 
 
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutChildSize, sizeBuf, rBuf);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::ChildSize panic error %d", err )
        USER_INVARIANT();
        }
   
    aSize = size.iSize;
   
    return size.iInt;
    }
 
// ---------------------------------------------------------------------------
// basecall only when really needed, in case of reordering, the server does 
// this automatically
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::UpdateChildrenLayout(TInt aTransitionTime)
    {
    if (!iLayoutData->iSkipServer)
        {
        CAlfVisual::UpdateChildrenLayout(aTransitionTime);
        }
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfVisual* CAlfLayout::FindTag(const TDesC8& aTag)
    {
    CAlfVisual* result = CAlfVisual::FindTag(aTag);
    if(!result && iLayoutData)
        {
        for(TInt i = 0; i < iLayoutData->iChildren.Count(); ++i)
            {
            result = iLayoutData->iChildren[i]->FindTag(aTag);
            if(result)
                {
                break;
                }
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::SetTransitionTime(TInt aTransitionTime)
    {
    TPckgC<TInt> buf(aTransitionTime);

    TInt err = Comms()->DoCmdNoReply( EAlfLayoutSetTransitionTime, buf );

    if ( err == KErrNone )
        {
        iLayoutData->iTransitionTime = aTransitionTime;
        }
    else
        {
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::TransitionTime() const
    {
    return iLayoutData->iTransitionTime;    
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TSize CAlfLayout::VirtualSize() const
    {
    TSize size(0,0);
    TPckg<TSize> buf(size);
    TInt err = Comms()->DoSynchronousCmd( EAlfLayoutVirtualSize, KNullDesC8, buf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::VirtualSize panic error %d", err )
        USER_INVARIANT();
        }
    
    return size;    
    }


// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::SetInnerPadding(const TPoint& aInnerPadding)
    {
    
    TPckgC<TPoint> buf(aInnerPadding);

    TInt err = Comms()->DoCmdNoReply( EAlfLayoutSetInnerPaddingPoint, buf );

    if ( err == KErrNone )
        {
        iLayoutData->iInnerPadding = TAlfXYMetric(TAlfMetric(aInnerPadding.iX), TAlfMetric(aInnerPadding.iY));
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLayout::SetInnerPadding panic error %d", err )
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::SetInnerPadding(const TAlfXYMetric& aInnerPadding)
    {
    TPckgC<TAlfXYMetric> buf(aInnerPadding);

    TInt err = Comms()->DoCmdNoReply( EAlfLayoutSetInnerPaddingMetric, buf );

    if ( err == KErrNone )
        {
        iLayoutData->iInnerPadding = aInnerPadding;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfLayout::SetInnerPadding panic error %d", err )
        USER_INVARIANT();
        }        
    }


// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::HorizontalInnerPadding() const
    {
    AssertInnerPaddingsWereSetUsingDeprecatedPixelAPIs(iLayoutData->iInnerPadding);
    return iLayoutData->iInnerPadding.iX.iMagnitude;
    }


// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::VerticalInnerPadding() const
    {
    AssertInnerPaddingsWereSetUsingDeprecatedPixelAPIs(iLayoutData->iInnerPadding);
    return iLayoutData->iInnerPadding.iY.iMagnitude;
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TPoint CAlfLayout::InnerPadding() const
    {
    AssertInnerPaddingsWereSetUsingDeprecatedPixelAPIs(iLayoutData->iInnerPadding);
    return TPoint(iLayoutData->iInnerPadding.iX.iMagnitude, iLayoutData->iInnerPadding.iY.iMagnitude);
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C const TAlfXYMetric& CAlfLayout::InnerPaddingAsMetric() const
    {
    return iLayoutData->iInnerPadding;
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfRealPoint CAlfLayout::InnerPaddingInBaseUnits() const
    {
    TBufC8<1> inDum;
    TAlfRealPoint value;
    TPckg<TAlfRealPoint> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutInnerPaddingInBaseUnits, inDum, outBuf);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::InnerPaddingInBaseUnits ignore error %d", err )
        }
        
    return value;            
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::FindVisual(CAlfVisual* aVisual) const
    {
    return iLayoutData->iChildren.Find(aVisual);        
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::Insert(CAlfVisual* aVisual, TInt aPosition)
    {
    ASSERT( aVisual && aVisual->Identifier());
    
    __ASSERT_ALWAYS( iLayoutData->iChildren.Find(aVisual) == KErrNotFound,
                     USER_INVARIANT() );

    CAlfLayout* oldLayout = aVisual->Layout();

	// These could be optimized to one server call if needed.   
    TInt err = Append(aVisual);        
    if ( err == KErrNone )
        {
        if (oldLayout != NULL)
    	    {
    	    // If this visual is already a member of another layout, 
    	    // remove it from the old one
        	oldLayout->Remove(aVisual);
	        }
        Reorder(*aVisual, aPosition);
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfLayout::EffectiveLayoutOrdinal(const CAlfVisual& aVisual) const
    {
    TInt ordinal = 0;

    for(TInt i = 0; i < iLayoutData->iChildren.Count(); ++i)
        {
        if(iLayoutData->iChildren[i] == &aVisual)
            {
            return ordinal;
            }

        // The visuals that are laid out completely manually do not affect
        // the layout ordinal.
        if((iLayoutData->iChildren[i]->Flags() & EAlfVisualFlagManualLayout) !=
           EAlfVisualFlagManualLayout)
            {
            ++ordinal;
            }
        }

    __ALFLOGSTRING( "CAlfLayout::EffectiveLayoutOrdinal panic visual not found")
    USER_INVARIANT();
    return 0;
    }
   
// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::SetBaseUnit(const TAlfMetric& aBaseUnit)
    {
    TPckg<TAlfMetric> inBuf(aBaseUnit);
    TInt err = Comms()->DoCmdNoReply(EAlfLayoutSetBaseUnit, inBuf);  
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::SetBaseUnit panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfLayout::SetBaseUnit(const TAlfXYMetric& aBaseUnit)
    {
    TPckg<TAlfXYMetric> inBuf(aBaseUnit);
    TInt err = Comms()->DoCmdNoReply(EAlfLayoutSetBaseUnitXY, inBuf);  
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::SetBaseUnit panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfXYMetric CAlfLayout::BaseUnit() const
    {
    TAlfXYMetric baseUnit;
    
    TBuf8<1> inDum;
    TPckg<TAlfXYMetric> outBuf(baseUnit);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfLayoutBaseUnit, 
        inDum, outBuf);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLayout::BaseUnit panic error %d", err )
        USER_INVARIANT();
        }

    return baseUnit;
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }

