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
* Description:   Grid layout
*
*/



#include "alf/alfgridlayout.h"
#include "alf/alflayout.h"
#include "alf/alfcontrol.h"
#include "alfclient.h"
#include "alf/alfenv.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"


// Struct for private data
struct CAlfGridLayout::TGridLayoutPrivateData
    {
    
    };

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGridLayout::CAlfGridLayout()
    : CAlfLayout()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGridLayout::ConstructL(CAlfControl& aOwner)
    {
    CAlfLayout::ConstructL(aOwner);     
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGridLayout::~CAlfGridLayout()
    {
    if ( iGridLayoutData )
        {
        }
        
    delete iGridLayoutData;
    iGridLayoutData = NULL;
    }


// ---------------------------------------------------------------------------
// Creates new grid layout for the control.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGridLayout* CAlfGridLayout::AddNewL(
    CAlfControl& aOwnerControl,
    TInt aColumns, TInt aRows,
    CAlfLayout* aParentLayout )
    {
    CAlfGridLayout* layout = STATIC_CAST(CAlfGridLayout*,
        aOwnerControl.AppendLayoutL(EAlfLayoutTypeGrid, aParentLayout));
    layout->SetColumnsL(aColumns);
    layout->SetRowsL(aRows);
    return layout;
    }

// ---------------------------------------------------------------------------
// Sets amount of columns in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGridLayout::SetColumnsL(TInt aColumnCount)
    {
    TPckgC<TInt> buf (aColumnCount);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetColumns, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetColumnsL error %d", err )
        User::Leave(err);
        }
    }
    
// ---------------------------------------------------------------------------
// Sets amount of rows in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGridLayout::SetRowsL(TInt aRowCount)
    {
    TPckgC<TInt> buf (aRowCount);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetRows, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetRowsL error %d", err )
        User::Leave(err);
        }
    }

EXPORT_C void CAlfGridLayout::SetColumnsL(const RArray<TInt>& aWeights)
    {
    HBufC8* buffer = ExternalizeLC(aWeights);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetColumnWeights, *buffer );
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetColumnsL error %d", err )
        User::Leave(err);
        }
        
    CleanupStack::PopAndDestroy( buffer );
    }

EXPORT_C void CAlfGridLayout::SetRowsL(const RArray<TInt>& aWeights)
    {
    HBufC8* buffer = ExternalizeLC(aWeights);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetRowsWeights, *buffer );
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetRowsL error %d", err )
        User::Leave(err);
        }
        
    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------------------------
// Weight metrics
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGridLayout::FillWeightsL(TAlfGridDimension aDim, TInt aCount, const TAlfMetric& aWeight)
    {
    TAlfGridLayoutFillWeightsParams params;
    params.iDim = aDim;
    params.iCount = aCount;
    params.iWeight = aWeight;
    TPckg<TAlfGridLayoutFillWeightsParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutFillWeights, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::FillWeightsL return error %d", err )
        User::Leave(err);
        }
    }
    
EXPORT_C void CAlfGridLayout::AppendWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight)
    {
    TAlfGridLayoutAppendWeightParams params;
    params.iDim = aDim;
    params.iWeight = aWeight;
    TPckg<TAlfGridLayoutAppendWeightParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutAppendWeight, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::AppendWeightL return error %d", err )
        User::Leave(err);
        }
    }
    
EXPORT_C void CAlfGridLayout::InsertWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight, TInt aPos)
    {
    TAlfGridLayoutInsertWeightParams params;
    params.iDim = aDim;
    params.iWeight = aWeight;
    params.iPos = aPos;
    TPckg<TAlfGridLayoutInsertWeightParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutInsertWeight, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::InsertWeightL return error %d", err )
        User::Leave(err);
        }
    }
    
EXPORT_C void CAlfGridLayout::ReplaceWeightL(TAlfGridDimension aDim, const TAlfMetric& aWeight, TInt aPos)
    {
    TAlfGridLayoutReplaceWeightParams params;
    params.iDim = aDim;
    params.iWeight = aWeight;
    params.iPos = aPos;
    TPckg<TAlfGridLayoutReplaceWeightParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutReplaceWeight, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::ReplaceWeightL return error %d", err )
        User::Leave(err);
        }
    }
    
EXPORT_C void CAlfGridLayout::RemoveWeightL(TAlfGridDimension aDim, TInt aPos)
    {
    TAlfGridLayoutRemoveWeightParams params;
    params.iDim = aDim;
    params.iPos = aPos;
    TPckg<TAlfGridLayoutRemoveWeightParams> paramsPckg(params);

    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutRemoveWeight, paramsPckg, dum );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::RemoveWeightL return error %d", err )
        User::Leave(err);
        }
    
    }
    
EXPORT_C TAlfMetric CAlfGridLayout::Weight(TAlfGridDimension aDim, TInt aPos) const
    {
    TAlfGridLayoutWeightParams params;
    params.iDim = aDim;
    params.iPos = aPos;
    TPckg<TAlfGridLayoutWeightParams> paramsPckg(params);

    TAlfMetric metric;
    TPckg<TAlfMetric> metricPckg(metric);
        
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutWeight, paramsPckg, metricPckg);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::RowCount ignore error %d", err )
        }
    return metric;
    }
    
EXPORT_C TInt CAlfGridLayout::DimensionCount(TAlfGridDimension aDim) const
    {
    TPckgC<TInt> buf (aDim);
    TInt count = 0;
    TPckg<TInt> countPckg(count);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutCount, buf, countPckg);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::RowCount ignore error %d", err )
        }
    
    return count;
    }
    
    
// ---------------------------------------------------------------------------
// Sets expanding flags in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGridLayout::SetExpanding(TInt aFlags)
    {
    TPckgC<TInt> buf (aFlags);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetExpanding, buf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetExpanding ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// Gets amount of rows in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfGridLayout::RowCount() const
    {
    TBuf8<1> dum;
    TInt count = 0;
    TPckg<TInt> countPckg(count);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutRowCount, dum, countPckg);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::RowCount ignore error %d", err )
        }
    
    return count;
    }
    
// ---------------------------------------------------------------------------
// Gets amount of columns in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfGridLayout::ColumnCount() const
    {
    TBuf8<1> dum;
    TInt count = 0;
    TPckg<TInt> countPckg(count);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutColumnCount, dum, countPckg);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::ColumnCount ignore error %d", err )
        }
    
    return count;
    }
    
// ---------------------------------------------------------------------------
// Calculates the position of a child in the grid.
// ---------------------------------------------------------------------------
//
EXPORT_C TPoint CAlfGridLayout::OrdinalToBlock(TInt aOrdinal) const
    {
    TPckgC<TInt> buf (aOrdinal);
    TPoint result;
    TPckg<TPoint> resBuf(result);
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutOrdinalToBlock, buf, resBuf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::OrdinalToBlock ignore error %d", err )
        }
        
    return result;                                 
    }


// ---------------------------------------------------------------------------
// Set Layout Mode Flags
// ---------------------------------------------------------------------------
// 

EXPORT_C void CAlfGridLayout::SetLayoutModeFlags(TAlfGridDimension aDim, TUint aGridLayoutModeFlags)
{
    TInt2 params( aDim, aGridLayoutModeFlags );
    TPckg<TInt2> paramsPckg(params);

    TInt err = Comms()->DoCmdNoReply(EAlfGridLayoutSetLayoutModeFlags, paramsPckg );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::SetLayoutModeFlags ignore error %d", err )
        }
}

// ---------------------------------------------------------------------------
// Get Layout Mode Flags
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfGridLayout::LayoutModeFlags(TAlfGridDimension aDim)
{
    TUint flags = 0;
    TPckg<TUint> flagPckg(flags);
    TPckgC<TInt> paramsPckg (aDim);  
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutLayoutModeFlags, paramsPckg, flagPckg);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::LayoutModeFlags ignore error %d", err )
        }
        
   return flags;
}
    
// ---------------------------------------------------------------------------
// Clear Layout Mode Flags
// ---------------------------------------------------------------------------
// 

EXPORT_C void CAlfGridLayout::ClearLayoutModeFlags(TAlfGridDimension aDim, TUint aGridLayoutModeFlags)
{    
    TInt2 params( aDim, aGridLayoutModeFlags );
    TPckg<TInt2> paramsPckg(params);
    TBuf8<1> resBuf;    
    
    TInt err = Comms()->DoSynchronousCmd(EAlfGridLayoutClearLayoutModeFlags, paramsPckg, resBuf);
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfGridLayout::ClearLayoutModeFlags ignore error %d", err )
        }  
}
    

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfGridLayout::RemoveAndDestroyAllD()
    {
    CAlfLayout::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfGridLayout::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfLayout::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfGridLayout::FindTag(const TDesC8& aTag)
    {
    return CAlfLayout::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfGridLayout::DoRemoveAndDestroyAllD()
    {
    CAlfLayout::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfLayout
// ---------------------------------------------------------------------------
//  
EXPORT_C TAlfXYMetric CAlfGridLayout::BaseUnit() const
    {
    return CAlfLayout::BaseUnit();
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfGridLayout::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfLayout::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }

