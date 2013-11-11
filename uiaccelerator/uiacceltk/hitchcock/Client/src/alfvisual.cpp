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
* Description:   Visual base class.
*
*/



#include "alf/alfvisual.h"
#include "alf/alfenv.h"
#include "alf/alflayout.h"
#include "alf/alfcontrol.h"
#include "alf/alfbrusharray.h"
#include "alfclient.h"
#include "alf/alfdisplay.h"
#include "alf/alfroster.h"
#include "alf/alfgencomponent.h"
#include "alf/alftimedvalue.h"
#include "alf/alfconstants.h"
#include "alflogger.h"
#include "alf/alfmappingfunctions.h"
#include "alf/alftransformation.h"
#include "alf/alfstatic.h"
#include "alf/alfutil.h"
#include "alf/alfdropshadow.h"

#include <uiacceltk/HuiUtil.h>

NONSHARABLE_CLASS(CAlfVisualOwnerNotification) : public CBase, public MAlfAsyncOpObserver
    {
public:
    CAlfVisualOwnerNotification( CAlfVisual& aVisual );
    ~CAlfVisualOwnerNotification();
protected:
    void AlfAsyncOpCompleted(TInt aCommandId, TInt aStatus);
private:
    CAlfVisual& iVisual;
    TInt iCommandId;
    TBuf8<1> iDumBuffer;
    };
    
CAlfVisualOwnerNotification::CAlfVisualOwnerNotification( CAlfVisual& aVisual )
 : iVisual( aVisual )
    {
    iCommandId = 0;
    TRAPD( err, iCommandId = iVisual.Comms()->DoAsynchronousCmdL( EAlfVisualLayoutUpdatedNotification, iDumBuffer, iDumBuffer, this ) );
    __ASSERT_ALWAYS( err == KErrNone, USER_INVARIANT() );
    }
    
CAlfVisualOwnerNotification::~CAlfVisualOwnerNotification()
    {
    TBuf8<1> dum;
    if ( iCommandId )
        {
        iVisual.Comms()->DoSynchronousCmd( EAlfVisualLayoutUpdatedNotificationCancel, dum, dum );
        iVisual.Comms()->CancelAsynchCmd( iCommandId );
        iCommandId = 0;
        }
    }
        
void CAlfVisualOwnerNotification::AlfAsyncOpCompleted(TInt /*aCommandId*/, TInt aStatus)
    {
    iCommandId = 0;
    if ( aStatus == KErrNone )
        {
        TRAPD( err, iCommandId = iVisual.Comms()->DoAsynchronousCmdL( EAlfVisualLayoutUpdatedNotification, iDumBuffer, iDumBuffer, this ) );
        __ASSERT_ALWAYS( err == KErrNone, USER_INVARIANT() );
        iVisual.Owner().VisualLayoutUpdated( iVisual );
        }
    }

// Private data
struct CAlfVisual::TPrivateData
    {
    CAlfLayout* iLayout;            // Not owned.
    CAlfDisplay* iDisplay;          // Not Owned.
    TAlfTimedValue iOpacity;        // Owned.
    CAlfControl* iOwner;            // Not owned.
    HBufC8* iTag;                   // Owned. 
    CAlfBrushArray* iBrushes;       // Owned.
    CAlfGenComponent* iComms;       // owned 
    CAlfTransformation* iTransform;  // Owned.  
    TAlfBoxMetric iPadding;// Owned
    CAlfVisualOwnerNotification* iVisualOwnerNotification; // Own
    CAlfDropShadow* iDropShadowHandler;
    };

// ======== MACRO DEFINITIONS ========

void AssertPaddingsWereSetUsingDeprecatedPixelAPIs(const TAlfBoxMetric& aMetric)
    {
    __ASSERT_DEBUG(
        (aMetric.iLeft.iMagnitude == aMetric.iRight.iMagnitude) && 
        (aMetric.iTop.iMagnitude == aMetric.iBottom.iMagnitude) &&
        (aMetric.iLeft.iUnit == EAlfUnitPixel) &&
        (aMetric.iRight.iUnit == EAlfUnitPixel) &&
        (aMetric.iTop.iUnit == EAlfUnitPixel) &&
        (aMetric.iBottom.iUnit == EAlfUnitPixel)
        , USER_INVARIANT());
#ifndef _DEBUG // Remove compile warnings
    TAlfBoxMetric tmp = aMetric;
    tmp = tmp;
#endif        
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual::CAlfVisual()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::ConstructL(CAlfControl& aOwner)
    {
    iData = new (ELeave) TPrivateData;
 
    iData->iLayout = NULL;
    iData->iDisplay = NULL;
    iData->iOwner = &aOwner;
    iData->iTag = NULL;
    iData->iBrushes = NULL;
    iData->iComms = NULL;
    iData->iTransform = NULL;
    iData->iPadding = TAlfBoxMetric(TAlfMetric(0));
    iData->iVisualOwnerNotification = NULL;
    iData->iDropShadowHandler = NULL;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::ConstructL(CAlfControl& aOwner, TInt aImplementationId, TInt aImplementationUid, const TDesC8& aConstructionParams )
    {

    ConstructL(aOwner);
   
    if (!iData->iComms)
        {
        iData->iComms = CAlfGenComponent::NewL(
            aOwner.Env(),
            aImplementationId, 
            aImplementationUid, 
            aConstructionParams);         
        }
    
    }




// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual::~CAlfVisual()
    {
    if ( iData )
        {
        delete iData->iDropShadowHandler;
        iData->iDropShadowHandler = NULL;
        
        delete iData->iVisualOwnerNotification;
        iData->iVisualOwnerNotification = NULL;
        
        // If the visual is still attached to a layout, it is time to remove it.
        if(iData->iLayout)
            {
            iData->iLayout->Remove(this);
            }
    
        delete iData->iBrushes;
        iData->iBrushes = NULL;
        
        delete iData->iTransform;
        iData->iTransform = NULL;
        
        delete iData->iTag;
        iData->iTag = NULL;
        
        delete iData->iComms;
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Destroys a hierarchy of visuals, along with this visual. The visuals
// are first removed from their owner.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::RemoveAndDestroyAllD()
    {
    TBuf8<1> dum;
    // Remove and destory from the server side
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualRemoveAndDestroyAll,KNullDesC8, dum);
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::RemoveAndDestroyAllD ignore error %d", err )
        }
            
    DoRemoveAndDestroyAllD();
    }
    
EXPORT_C void CAlfVisual::DoRemoveAndDestroyAllD()
    {
    __ASSERT_ALWAYS( iData->iOwner, USER_INVARIANT() );
   
    iData->iOwner->VisualDestroyed(*this);
    delete this;
    }
    
// ---------------------------------------------------------------------------
// Returns subsession identifier, 0 by default.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfVisual::Identifier() const
    {
    return iData->iComms->Identifier();
    }

// ---------------------------------------------------------------------------
// Sets layout for the visual.
// ---------------------------------------------------------------------------
//
void CAlfVisual::SetLayout(CAlfLayout* aLayout)
    {
    iData->iLayout = aLayout;
    }
    
// ---------------------------------------------------------------------------
// Returns current opacity
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedValue& CAlfVisual::Opacity() const
    {
    TPckg<TAlfTimedValue> buf(iData->iOpacity);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualGetOpacity, KNullDesC8, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::Opacity panic error %d", err )
        USER_INVARIANT();
        }
    
    return iData->iOpacity;
    }
        
// ---------------------------------------------------------------------------
// Sets opacity.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::SetOpacity( const TAlfTimedValue& aOpacity )
    {
    TPckgC<TAlfTimedValue> buf(aOpacity);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetOpacity, buf);
    
    if ( err == KErrNone )
        {
        iData->iOpacity = aOpacity;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfVisual::SetOpacity panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Returns position
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTimedPoint CAlfVisual::Pos() const
    {
    TAlfTimedPoint point;
    TPckg<TAlfTimedPoint> buf(point);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualPos, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::Pos panic error %d", err )
        USER_INVARIANT();
        }

    return point;
    }


// ---------------------------------------------------------------------------
// Sets the position of the visual.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::SetPos(const TAlfRealPoint& aPos, TInt aTransitionTime)
    {
    TAlfVisualPos pos = { aPos, aTransitionTime };

    TPckgC<TAlfVisualPos> buf(pos);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetPos, buf);
    
    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetPos panic error %d", err )
        USER_INVARIANT();
        }
    }
  
// ---------------------------------------------------------------------------
// Returns the size of the visual. 
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfTimedPoint CAlfVisual::Size() const
    {
    TAlfTimedPoint size;
    TPckg<TAlfTimedPoint> buf(size);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualSize, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::Size panic error %d", err )
        USER_INVARIANT();
        }
    
    return size;    
    }

// ---------------------------------------------------------------------------
// Sets the size of the visual.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::SetSize(const TAlfRealSize& aSize, TInt aTransitionTime)
    {
    TAlfVisualSize size = { aSize, aTransitionTime };

    TPckgC<TAlfVisualSize> buf(size);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetSize, buf );
    
    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetSize panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetFlag(TAlfVisualFlags aFlag)
    {
    TPckgC<TAlfVisualFlags> inBuf(aFlag);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetFlag, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetFlag panic error %d", err )
        USER_INVARIANT();
        }
    
    if ( aFlag == EAlfVisualFlagLayoutUpdateNotification && !iData->iVisualOwnerNotification )
        {
        iData->iVisualOwnerNotification = new CAlfVisualOwnerNotification( *this );
        __ASSERT_ALWAYS( iData->iVisualOwnerNotification , USER_INVARIANT() );
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetFlags(TUint aAnyFlags)
    {
    TPckgC<TUint> inBuf(aAnyFlags);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetFlags, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetFlags panic error %d", err )
        USER_INVARIANT();
        }
        
    if ( aAnyFlags & EAlfVisualFlagLayoutUpdateNotification && !iData->iVisualOwnerNotification )
        {
        iData->iVisualOwnerNotification = new CAlfVisualOwnerNotification( *this );
        __ASSERT_ALWAYS( iData->iVisualOwnerNotification , USER_INVARIANT() );
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::ClearFlag(TAlfVisualFlags aFlag)
    {
    TPckgC<TAlfVisualFlags> inBuf(aFlag);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualClearFlag, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::ClearFlag panic error %d", err )
        USER_INVARIANT();
        }
        
    if ( aFlag == EAlfVisualFlagLayoutUpdateNotification && iData->iVisualOwnerNotification )
        {
        delete iData->iVisualOwnerNotification;
        iData->iVisualOwnerNotification = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::ClearFlags(TUint aAnyFlags)
    {
    TPckgC<TUint> inBuf(aAnyFlags);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualClearFlags, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::ClearFlags panic error %d", err )
        USER_INVARIANT();
        }
        
    if ( aAnyFlags & EAlfVisualFlagLayoutUpdateNotification && iData->iVisualOwnerNotification )
        {
        delete iData->iVisualOwnerNotification;
        iData->iVisualOwnerNotification = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TUint CAlfVisual::Flags() const
    {
    TUint flags = 0;
    TPckg<TUint> outBuf(flags);
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualFlags, KNullDesC8, outBuf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::Flags panic error %d", err )
        USER_INVARIANT();
        }
    
    return flags; 
    }

// ---------------------------------------------------------------------------
// Returns layout
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfLayout* CAlfVisual::Layout() const
    {
    return iData->iLayout;
    }
    
// ---------------------------------------------------------------------------
// Returns env.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv& CAlfVisual::Env() const
    {
    if (!iData || !iData->iOwner) // try to save what there's left to save
        {
        return CAlfStatic::Env();
        }
        
    return iData->iOwner->Env();
    }
    
// ---------------------------------------------------------------------------
// Returns tag.
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CAlfVisual::Tag() const
    {
    if(iData && iData->iTag)
        {
        return *iData->iTag;
        }

    return KNullDesC8;
    }
   
// ---------------------------------------------------------------------------
// Returns display
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfDisplay* CAlfVisual::Display() const
    {
    const CAlfVisual* iter = this;

    while(iter)
        {
        if(iter->iData->iDisplay)
            {
            return iter->iData->iDisplay;
            }
        iter = iter->Layout();
        }

    // Not shown on any display.
    return NULL;
    }

// ---------------------------------------------------------------------------
// Update child layout to the server
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfVisual::UpdateChildrenLayout(TInt aTransitionTime)
    {
    TPckgC<TInt> buf(aTransitionTime);
    TBuf8<1> dum;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfVisualUpdateChildrenLayout, buf, dum);
    
    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::UpdateChildrenLayout panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Enables brush usage.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::EnableBrushesL(TBool aEnabled)
    {
    if(aEnabled && !iData->iBrushes)
        {
        iData->iBrushes = CAlfBrushArray::NewL(*this);
        }
    else if(!aEnabled && iData->iBrushes)
        {
        delete iData->iBrushes;
        iData->iBrushes = NULL;
        }
    else
        {
        // for PC lint
        }
    }

// ---------------------------------------------------------------------------
// Returns brush array
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrushArray* CAlfVisual::Brushes()
    {
    return iData->iBrushes;
    }
   
// ---------------------------------------------------------------------------
// Converts point.
// ---------------------------------------------------------------------------
// 
EXPORT_C TPoint CAlfVisual::LocalToDisplay(const TPoint& aPoint) const
    {
    const TAlfRealPoint realPoint =  LocalToDisplay(TAlfRealPoint(aPoint));
    return TPoint( AlfUtil::RoundFloatToInt( realPoint.iX ),AlfUtil::RoundFloatToInt( realPoint.iY ) );
    }

// ---------------------------------------------------------------------------
// Converts point.
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfRealPoint CAlfVisual::LocalToDisplay(
    const TAlfRealPoint& aPoint) const
    {
    TPckgC<TAlfRealPoint> pointPckg(aPoint);
    TAlfRealPoint result;
    TPckg<TAlfRealPoint> resultPckg(result);
    
    TInt err = iData->iComms->DoSynchronousCmd(EAlfVisualLocalToDisplay, pointPckg, resultPckg);
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::LocalToDisplay panic error %d", err )
        USER_INVARIANT();
        }
    
    return result;
    }
   
// ---------------------------------------------------------------------------
// Converts point.
// ---------------------------------------------------------------------------
//  
EXPORT_C TPoint CAlfVisual::DisplayToLocal(const TPoint& aPoint) const
    {
    const TAlfRealPoint realPoint = DisplayToLocal(TAlfRealPoint(aPoint));
    return TPoint( AlfUtil::RoundFloatToInt( realPoint.iX ),AlfUtil::RoundFloatToInt( realPoint.iY ) );
    }

// ---------------------------------------------------------------------------
// Converts point.
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfRealPoint CAlfVisual::DisplayToLocal(const TAlfRealPoint& aPoint) const
    {
    TPckgC<TAlfRealPoint> pointPckg(aPoint);
    TAlfRealPoint result;
    TPckg<TAlfRealPoint> resultPckg(result);
    
    TInt err = iData->iComms->DoSynchronousCmd(EAlfVisualDisplayToLocal, pointPckg, resultPckg);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::DisplayToLocal panic error %d", err )
        USER_INVARIANT();
        }
        
    return result;
    }
 
// ---------------------------------------------------------------------------
// Gets display rect
// ---------------------------------------------------------------------------
//    
EXPORT_C TAlfRealRect CAlfVisual::DisplayRectTarget() const
    {
    TAlfRealRect rect;
    TPckg<TAlfRealRect> rectPckg(rect);

    TInt err = iData->iComms->DoSynchronousCmd(EAlfVisualDisplayRectTarget, KNullDesC8, rectPckg);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::DisplayRectTarget panic error %d", err )
        USER_INVARIANT();
        }

    return rect;
    }
 
// ---------------------------------------------------------------------------
// Moves visual to front.
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfVisual::MoveToFront( TInt aTransitionTime )
    {
    if (iData->iLayout)
        {
        iData->iLayout->MoveVisualToFront(*this, aTransitionTime);
        }  
    }
    
// ---------------------------------------------------------------------------
// Enables transformation.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfVisual::EnableTransformationL(TBool aIsTransformed)
    {
    if ( aIsTransformed && iData->iTransform )
        {
        // Already enabled
        return;
        }
        
    if ( !aIsTransformed && !iData->iTransform )
        {
        // Already disabled
        return;
        }
    
    if ( aIsTransformed )
        {
        iData->iTransform = CAlfTransformation::NewL( Env() );
        }
   
    // use the server handle from the transformation object if present
    TInt transformationHandle = 
        iData->iTransform ? 
            iData->iTransform->ServerHandle() : 
            0 ;
            
    TInt2 inData(transformationHandle,aIsTransformed);
    TPckgC<TInt2> inPckg(inData);
    
    TBuf8<1> outDum;
    
    TInt err = iData->iComms->DoSynchronousCmd(EAlfVisualEnableTransformation, inPckg, outDum);
     
    if ( err == KErrNone && !aIsTransformed )
        {
        delete iData->iTransform;
        iData->iTransform = NULL;
        }
    
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::EnableTransformationL leave error %d", err )
        delete iData->iTransform;
        iData->iTransform = NULL;
        User::Leave( err );
        }
    }
    
// ---------------------------------------------------------------------------
// Returns transformation
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfTransformation& CAlfVisual::Transformation()
    {
    __ASSERT_ALWAYS( iData->iTransform, USER_INVARIANT() );
    return *iData->iTransform;        
    }


    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetPadding(TInt aPadding)
    {
    TPckgC<TInt> inBuf(aPadding);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetPaddingInt, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetPadding panic error %d", err )
        USER_INVARIANT();
        }
        
    iData->iPadding = TAlfBoxMetric(TAlfMetric(aPadding));
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetPadding(const TPoint& aPadding)
    {
    TPckgC<TPoint> inBuf(aPadding);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetPaddingPoint, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetPadding panic error %d", err )
        USER_INVARIANT();
        }
        
    iData->iPadding = TAlfBoxMetric(TAlfXYMetric(TAlfMetric(aPadding.iX), TAlfMetric(aPadding.iY)));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetPadding(const TAlfBoxMetric& aPadding)
    {
    TPckgC<TAlfBoxMetric> inBuf(aPadding);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetPaddingBox, inBuf );

    if (err) // for now
        {
        __ALFLOGSTRING1( "CAlfVisual::SetPadding panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iPadding = aPadding;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfRealRect CAlfVisual::PaddingInPixels() const
    {
    TBufC8<1> inDum;
    TAlfRealRect value;
    TPckg<TAlfRealRect> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualPaddingInPixels, inDum, outBuf);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::DisplayRect ignore error %d", err )
        }
        
    return value;            
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetTagL(const TDesC8& aTag)
    {
    ASSERT(iData);
    
    delete iData->iTag;
    iData->iTag = 0;
    
    iData->iTag = aTag.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CAlfVisual::IsTagged() const
    {
    return (iData && iData->iTag);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfVisual::FindTag(const TDesC8& aTag)
    {
    // Look for the tag in this visual's tag descriptor.
    if(iData && iData->iTag && TagMatches(aTag))
        {
        return this;
        }
    else
        {
        // Could not find it.
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// Returns comms module for accessing the client-server APIs.
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfGenComponent* CAlfVisual::Comms() const
    {
    ASSERT(iData);
    return iData->iComms;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CAlfVisual::TagMatches(const TDesC8& aTag) const
    {    
    if(!aTag.Length() || !iData || !iData->iTag )
        {
        // No tag specified; doesn't match anything.
        return EFalse;
        }

    TPtrC8 region = iData->iTag->Des();
    TPtrC8 tag;
    TInt index = 0;

    while(region.Length() > 0)
        {
        // Is there a colon in the region?
        index = region.Locate(KAlfTagSeparator);
        if(index != KErrNotFound)
            {
            // A separator exists in the region.
            tag.Set(region.Left(index));
            region.Set(region.Right(region.Length() - index - 1));
            }
        else
            {
            tag.Set(region);
            region.Set(region.Right(0));
            }

        if(!tag.Compare(aTag))
            {
            // Matches.
            return ETrue;
            }
        }

    // No match could be found.
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfTimedValue CAlfVisual::DepthOffset() const
    {
    TBufC8<1> inDum;
    TAlfTimedValue value;
    TPckg<TAlfTimedValue> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualDepthOffset, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::DepthOffset ignore error %d", err )
        }
    return value;                                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetDepthOffset(const TAlfTimedValue& aDepthOffset)
    {
    TPckgC<TAlfTimedValue> inPckg(aDepthOffset);
    TInt err = Comms()->DoCmdNoReply(EAlfVisualSetDepthOffset, inPckg );            
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetDepthOffset ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfControl& CAlfVisual::Owner() const
    {
    return *iData->iOwner;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetOwner(CAlfControl& aOwner)
    {
    iData->iOwner = &aOwner;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetOrigin(TAlfVisualHOrigin aHoriz, 
    TAlfVisualVOrigin aVert)
    {
    ClearFlags(EAlfVisualFlagHOriginCenter | EAlfVisualFlagHOriginRight);
    switch(aHoriz)
        {
        case EAlfVisualHOriginCenter:
            SetFlag(EAlfVisualFlagHOriginCenter);
            break;

        case EAlfVisualHOriginRight:
            SetFlag(EAlfVisualFlagHOriginRight);
            break;

        default:
            break;
        }

    ClearFlags(EAlfVisualFlagVOriginCenter | EAlfVisualFlagVOriginBottom);
    switch(aVert)
        {
        case EAlfVisualVOriginCenter:
            SetFlag(EAlfVisualFlagVOriginCenter);
            break;

        case EAlfVisualVOriginBottom:
            SetFlag(EAlfVisualFlagVOriginBottom);
            break;

        default:
            break;
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfVisualHOrigin CAlfVisual::HorizontalOrigin() const
    {
    TUint flags = Flags();
    if(flags & EAlfVisualFlagHOriginCenter)
        {
        return EAlfVisualHOriginCenter;
        }
    if(flags & EAlfVisualFlagHOriginRight)
        {
        return EAlfVisualHOriginRight;
        }
    return EAlfVisualHOriginLeft;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfVisualVOrigin CAlfVisual::VerticalOrigin() const
    {
    TUint flags = Flags();
    if(flags & EAlfVisualFlagVOriginCenter)
        {
        return EAlfVisualVOriginCenter;
        }
    if(flags & EAlfVisualFlagVOriginBottom)
        {
        return EAlfVisualVOriginBottom;
        }
    return EAlfVisualVOriginTop;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::AlignByOrigin(TAlfRealRect& aRect) const
    {      
    switch(HorizontalOrigin())
        {
        case EAlfVisualHOriginCenter:
            aRect.Move(-aRect.Width()/2.0, 0.0);
            break;

        case EAlfVisualHOriginRight:
            aRect.Move(-aRect.Width(), 0.0);
            break;

        default:
            break;
        }

    switch(VerticalOrigin())
        {
        case EAlfVisualVOriginCenter:
            aRect.Move(0.0, -aRect.Height()/2.0);
            break;

        case EAlfVisualVOriginBottom:
            aRect.Move(0.0, -aRect.Height());
            break;

        default:
            break;
        }
    }
    
// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetMinSize(const TSize& aMinSize)
    {
    TPckgC<TSize> inPckg(aMinSize);
    TInt err = Comms()->DoCmdNoReply(EAlfVisualSetMinSize, inPckg );  
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetMinSize ignore error %d", err )
        }
    }   

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetMinSize(const TAlfRealSize& aMinSize)
    {
    TPckgC<TAlfRealSize> inPckg(aMinSize);
    TInt err = Comms()->DoCmdNoReply(EAlfVisualSetMinSizeInBaseUnits, inPckg );  
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetMinSize ignore error %d", err )
        }
    }


// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C TSize CAlfVisual::MinSize() const
    {
    TBufC8<1> inDum;
    TSize value;
    TPckg<TSize> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualGetMinSize, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::MinSize ignore error %d", err )
        }
          
    return value;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfRealSize CAlfVisual::MinSizeInBaseUnits() const
    {
    TBufC8<1> inDum;
    TAlfRealSize value;
    TPckg<TAlfRealSize> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualGetMinSizeInBaseUnits, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::MinSizeInBaseUnit ignore error %d", err )
        }
          
    return value;
    }
    
// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetMaxSize(const TSize& aMaxSize)
    {
    TPckgC<TSize> inPckg(aMaxSize);
    TInt err = Comms()->DoCmdNoReply(EAlfVisualSetMaxSize, inPckg); 
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetMaxSize ignore error %d", err )
        }
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetMaxSize(const TAlfRealSize& aMaxSize)
    {
    TPckgC<TAlfRealSize> inPckg(aMaxSize);
    TInt err = Comms()->DoCmdNoReply(EAlfVisualSetMaxSizeInBaseUnits, inPckg); 
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetMaxSize ignore error %d", err )
        }
    }    

// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C TSize CAlfVisual::MaxSize() const
    {
    TBufC8<1> inDum;
    TSize value;
    TPckg<TSize> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualGetMaxSize, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::MaxSize ignore error %d", err )
        }
          
    return value;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfRealSize CAlfVisual::MaxSizeInBaseUnits() const
    {
    TBufC8<1> inDum;
    TAlfRealSize value;
    TPckg<TAlfRealSize> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualGetMaxSizeInBaseUnits, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::MaxSizeInBaseUnits ignore error %d", err )
        }
          
    return value;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetRect(const TAlfRealRect& aRect, TInt aTransitionTime)
    {
    SetPos(aRect.iTl, aTransitionTime);
    SetSize(TAlfRealSize(aRect.Size().iWidth, aRect.Size().iHeight),
        aTransitionTime);
    }
    
// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::Move(const TPoint& aOffset, TInt aTransitionTime)
    {
    TAlfTimedPoint pos = Pos();
    TReal32 xpos = pos.Target().iX + aOffset.iX;
    TReal32 ypos = pos.Target().iY + aOffset.iY;
    TAlfRealPoint newPos(xpos, ypos);
    SetPos(newPos, aTransitionTime);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::Move(const TAlfRealPoint& aOffset, TInt aTransitionTime)
    {
    TAlfVisualPos pos = { aOffset, aTransitionTime };
    
    TPckgC<TAlfVisualPos> buf(pos);
    
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualMove, buf );
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::Move ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::Mimic(CAlfVisual& aVisual, TInt aTransitionTime)
    {
    SetPos(aVisual.Pos().Target(), aTransitionTime);
    SetSize(aVisual.Size().Target(), aTransitionTime);        
    }
    
// ---------------------------------------------------------------------------
// deprecated
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetCenteredPosAndSize(const TPoint& aCenterPoint,
                                        const TSize& aSize,
                                        TInt aTransitionTime)
    {
    SetPos(TPoint(aCenterPoint.iX - aSize.iWidth/2,
                  aCenterPoint.iY - aSize.iHeight/2), aTransitionTime);
    SetSize(aSize, aTransitionTime);        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetCenteredPosAndSize(const TAlfRealPoint& aCenterPoint,
                                        const TAlfRealSize& aSize,
                                        TInt aTransitionTime)
    {
    TAlfVisualPosSize posSize = { aCenterPoint, aSize, aTransitionTime };
    
    TPckgC<TAlfVisualPosSize> buf(posSize);
    
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetCenteredPosAndSize, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::SetCenteredPosAndSize ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfRealRect CAlfVisual::DisplayRect() const
    {
    TBufC8<1> inDum;
    TAlfRealRect value;
    TPckg<TAlfRealRect> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualDisplayRect, inDum, outBuf);    
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::DisplayRect ignore error %d", err )
        }
        
    return value;                                                                
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TReal32 CAlfVisual::EffectiveOpacity() const __SOFTFP
    {
    // We could do this locally, but because Opacity()-method calls anyway
    // server we do one server call here instead of multiple Opacity()-calls.
    TBufC8<1> inDum;
    TReal32 value = 1.0;
    TPckg<TReal32> outBuf(value);    
    TInt err = Comms()->DoSynchronousCmd(EAlfVisualEffectiveOpacity, inDum, 
        outBuf);  
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfVisual::EffectiveOpacity ignore error %d", err )
        }
          
    return value;                                                                
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetClipping(TBool aClipping)
    {
    if(aClipping)
        {
        SetFlag(EAlfVisualFlagClipping);
        }
    else
        {
        ClearFlag(EAlfVisualFlagClipping);
        }        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CAlfVisual::Clipping() const
    {
    return (Flags() & EAlfVisualFlagClipping) != 0;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//  
EXPORT_C TPoint CAlfVisual::Padding() const
    {
    AssertPaddingsWereSetUsingDeprecatedPixelAPIs(iData->iPadding);
    return TPoint(iData->iPadding.iLeft.iMagnitude, iData->iPadding.iTop.iMagnitude);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C const TAlfBoxMetric& CAlfVisual::PaddingAsMetric() const
    {
    return iData->iPadding;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfVisual::HorizontalPadding() const
    {
    AssertPaddingsWereSetUsingDeprecatedPixelAPIs(iData->iPadding);
    return iData->iPadding.iLeft.iMagnitude;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfVisual::VerticalPadding() const
    {
    AssertPaddingsWereSetUsingDeprecatedPixelAPIs(iData->iPadding);
    return iData->iPadding.iTop.iMagnitude;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CAlfVisual::SetDisplay(CAlfDisplay* aDisplay)
    {
    iData->iDisplay = aDisplay;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetPos(const TAlfTimedPoint& aPos)
    {
    TPckgC<TAlfTimedPoint> buf(aPos);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetPosTimed, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetPos panic error %d", err )
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::SetSize(const TAlfTimedPoint& aSize)
    {
    TPckgC<TAlfTimedPoint> buf(aSize);
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualSetSizeTimed, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetSize panic error %d", err )
        USER_INVARIANT();
        }                
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfVisual::CopyValues(const CAlfVisual& aVisual, TInt aBitmask, TInt aTransitionTime )
    {
    if (aVisual.Identifier())
        {
        return KErrArgument;
        }
    
    TInt3 inData(aVisual.Identifier(),aBitmask,aTransitionTime);
    TPckgC<TInt3> inPckg(inData);
    TBuf8<1> dum;
        
    return iData->iComms->DoSynchronousCmd( EAlfVisualCopyValuesFromOtherVisual, inPckg, dum );    
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfPropertyOwner::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfVisual::SetTactileFeedbackL( TInt aEventType, TInt aFeedbackType )
    {
    __ASSERT_ALWAYS( aFeedbackType >= 0, USER_INVARIANT() );
    __ASSERT_ALWAYS( aEventType >= 0, USER_INVARIANT() );
    
    TInt2 inData(aEventType,aFeedbackType);
    TPckgC<TInt2> inPckg(inData);
    TBuf8<1> dum;
        
    TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualSetTactileFeedback, inPckg, dum ); 

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::SetTactileFeedbackL leave error %d", err )
        User::Leave( err );
        }   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CAlfVisual::HasTactileFeedback( TInt aEventType ) const
    {
    TInt inData(aEventType);
    TPckgC<TInt> inPckg(inData);
    
    TBool answer = EFalse;
    TPckg<TBool> outPckg(answer);
        
    TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualHasTactileFeedback, inPckg, outPckg ); 
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::HasTactileFeedback ignore error %d", err )
        }   
    return answer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfVisual::RemoveTactileFeedback( TInt aEventType  )
    {
    TInt inData(aEventType);
    TPckgC<TInt> inPckg(inData);
        
    TInt err = iData->iComms->DoCmdNoReply( EAlfVisualRemoveTactileFeedback, inPckg );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::RemoveTactileFeedback ignore error %d", err )
        }  
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::Pause(TInt aPropertiesBitmask)
    {
    TPckgC<TInt> inPckg(aPropertiesBitmask);
    TBuf8<1> dum;
     
    TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualPauseProperties, inPckg, dum ); 
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::Pause ignored error %d", err )
        }   
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::Resume(TInt aPropertiesBitmask)
    {
    TPckgC<TInt> inPckg(aPropertiesBitmask);
    TBuf8<1> dum;
     
    TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualResumeProperties, inPckg, dum ); 
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfVisual::Resume ignored error %d", err )
        }   
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::EnableDropShadowL( TBool aEnable  )
    {
    if ( aEnable && !iData->iDropShadowHandler )
        {
        // create instance.
        CAlfDropShadow* dropShadowHandler = CAlfDropShadow::NewL( *Comms() );
        
        TPckgC<TBool> inPckg(aEnable);
        TBuf8<1> dum;
        TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualEnableDropShadow, inPckg, dum ); 
        
        if (err!=KErrNone)
            {
            __ALFLOGSTRING1( "CAlfVisual::EnableDropShadowL leave error %d", err )
            delete dropShadowHandler;
            User::Leave( err );
            }
        iData->iDropShadowHandler = dropShadowHandler;
        }
    else if ( !aEnable && iData->iDropShadowHandler )
        {
        // delete the instance.
        TPckgC<TBool> inPckg(aEnable);
        TBuf8<1> dum;
        TInt err =  iData->iComms->DoSynchronousCmd( EAlfVisualEnableDropShadow, inPckg, dum ); 
        
        if (err!=KErrNone)
            {
            __ALFLOGSTRING1( "CAlfVisual::EnableDropShadowL leave error %d", err )
            User::Leave( err );
            }
            
        delete iData->iDropShadowHandler;
        iData->iDropShadowHandler = NULL;
        }
    else
        {
        // For PC lint
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfDropShadow* CAlfVisual::DropShadowHandler() const
    {
    return iData->iDropShadowHandler;
    }

EXPORT_C void CAlfVisual::SetEffectL( const TDesC& aEffectFile )
    {
    TBuf8<1> dumDum;
    TAlfVisualEffectParams params;
    params.iFileName = aEffectFile;
    params.iHandle = Identifier();
    
    TPckg<TAlfVisualEffectParams> paramsPckg(params);
    
    TInt err = KErrNone;
    if ( aEffectFile.Length() == 0 )
        {
        err = iData->iComms->DoSynchronousCmd(EAlfVisualRemoveEffect, paramsPckg, dumDum);
        }
    else
        {
        err = iData->iComms->DoSynchronousCmd(EAlfVisualSetEffect, paramsPckg, dumDum);
        }
    if (err)
        {
        __ALFLOGSTRING1( "CAlfVisual::SetEffectL leave error %d", err )
        User::Leave( err );
        } 
    }

// ---------------------------------------------------------------------------
// SetGroupEffectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfVisual::SetGroupEffectL( const TDesC& aEffectFile, TInt aGroupHandle )
    {
	TBuf8<1> dumDum;
	TAlfVisualEffectParams params;
	params.iFileName = aEffectFile;
	params.iGroupHandle = aGroupHandle;
	params.iHandle = Identifier();
	
	TPckg<TAlfVisualEffectParams> paramsPckg(params);
	
	TInt err = KErrNone;
	if ( aEffectFile.Length() == 0 )
		{
		err = iData->iComms->DoSynchronousCmd(EAlfVisualRemoveEffect, paramsPckg, dumDum);
		}
	else
		{
		err = iData->iComms->DoSynchronousCmd(EAlfVisualSetGroupEffect, paramsPckg, dumDum);
		}
	if (err)
		{
		__ALFLOGSTRING1( "CAlfVisual::SetGroupEffectL leave error %d", err )
		User::Leave( err );
		}    
    }
    
