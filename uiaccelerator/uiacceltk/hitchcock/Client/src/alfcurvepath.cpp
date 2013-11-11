/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Curve paths are composed of path segments
*                and can be used as mapping functions, and in line visuals.
*
*/



#include "alf/alfcurvepath.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfCurvePath::TPrivateData
    {
    CAlfGenComponent* iComms; // Own
    TAlfTimedPoint iOffset; // Own.
    };

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfCurvePath::CAlfCurvePath()
    {
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CAlfCurvePath::ConstructL( CAlfEnv& aEnv, TInt aCurvePathProviderHandle )
    {
    iData = new (ELeave) TPrivateData;

    iData->iComms = NULL;
    
    TPckgC<TInt> constructionParamBuf( aCurvePathProviderHandle );
    iData->iComms = CAlfGenComponent::NewL(
        aEnv,
        EAlfCurvePathCreate, 
        0, 
        constructionParamBuf );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePath* CAlfCurvePath::NewL( CAlfEnv& aEnv, TInt aCurvePathProviderHandle )
    {
    CAlfCurvePath* self = CAlfCurvePath::NewLC(aEnv, aCurvePathProviderHandle);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor, leave on cleanup stack
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePath* CAlfCurvePath::NewLC( CAlfEnv& aEnv, TInt aCurvePathProviderHandle )
    {
    CAlfCurvePath* self = new( ELeave ) CAlfCurvePath;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv,aCurvePathProviderHandle);
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfCurvePath::~CAlfCurvePath()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
        
    delete iData;
    iData = NULL;
    }


// ---------------------------------------------------------------------------
// Reset path
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePath::Reset()
    {
    TBufC8<1> inDummy;
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathReset, 
        inDummy, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::Reset panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Enable closed loop path
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePath::EnableLoop(TBool aEnable)
    {
    TPckgC<TBool> inBuf( aEnable );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathEnableLoop, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::EnableLoop panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Is path a loop
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfCurvePath::Loop() const
    {
    TBool result = EFalse;
    TPckg<TBool> outBuf( result );
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathLoop, 
        KNullDesC8(), 
        outBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::Loop panic error %d", err )
        USER_INVARIANT();
        }
    
    return result;
    }
    
// ---------------------------------------------------------------------------
// Set origin of path
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePath::SetOrigin(TReal32 aPosOrigin) __SOFTFP
    {
    TPckgC<TReal32> inBuf( aPosOrigin );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathSetOrigin, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::SetOrigin panic error %d", err )
        USER_INVARIANT();
        }
    }
   
// ---------------------------------------------------------------------------
// Add line segment
// @deprecated
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfCurvePath::AppendLineL(
    const TPoint& aStart, 
    const TPoint& aEnd,
    TReal32 aLength ) __SOFTFP
    {
    TAlfCurvePathLineParams params;
    params.iStart = aStart;
    params.iEnd = aEnd;
    params.iLength = aLength;
    
    TPckgC<TAlfCurvePathLineParams> inBuf( params );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathAppendLine, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::AppendLineL panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Add line segment
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfCurvePath::AppendLineL(
    const TAlfRealPoint& aStart, 
    const TAlfRealPoint& aEnd,
    TReal32 aLength ) __SOFTFP
    {
    TAlfCurvePathLineRealParams params;
    params.iStart = aStart;
    params.iEnd = aEnd;
    params.iLength = aLength;
    
    TPckgC<TAlfCurvePathLineRealParams> inBuf( params );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathAppendLine2, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::AppendLineL panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Add arc
// @deprecated
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePath::AppendArcL(
    const TPoint& aOrigin, 
    const TSize& aSize,
    TReal32 aStartAngle, 
    TReal32 aEndAngle,
    TReal32 aLength ) __SOFTFP
    {
    TAlfCurvePathArcParams params;
    params.iOrigin = aOrigin;
    params.iSize = aSize;
    params.iStartAngle = aStartAngle;
    params.iEndAngle = aEndAngle;
    params.iLength = aLength;
    
    TPckgC<TAlfCurvePathArcParams> inBuf( params );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathAppendArc, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::AppendArcL panic error %d", err )
        USER_INVARIANT();
        }
    }
 
// ---------------------------------------------------------------------------
// Add arc
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfCurvePath::AppendArcL(
    const TAlfRealPoint& aOrigin, 
    const TAlfRealSize& aSize,
    TReal32 aStartAngle, 
    TReal32 aEndAngle,
    TReal32 aLength ) __SOFTFP
    {
    TAlfCurvePathArcRealParams params;
    params.iOrigin = aOrigin;
    params.iSize = aSize;
    params.iStartAngle = aStartAngle;
    params.iEndAngle = aEndAngle;
    params.iLength = aLength;
    
    TPckgC<TAlfCurvePathArcRealParams> inBuf( params );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathAppendArc2, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::AppendArcL panic error %d", err )
        USER_INVARIANT();
        }
    }
 
// ---------------------------------------------------------------------------
// Get length of curved path
// ---------------------------------------------------------------------------
//   
EXPORT_C TReal32 CAlfCurvePath::Length() const __SOFTFP
    {
    TReal32 result = 0.0;
    TPckg<TReal32> outBuf( result );
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathLength, 
        KNullDesC8(), 
        outBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::Length panic error %d", err )
        USER_INVARIANT();
        }
    
    return result;
    }
   
// ---------------------------------------------------------------------------
// Calculate a point on the path
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfCurvePath::Evaluate(TReal32 aPos, TAlfRealPoint& aPoint) const __SOFTFP
    {
    TPckgC<TReal32> inBuf( aPos );
    TPckg<TAlfRealPoint> outBuf( aPoint );
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathEvaluate, 
        inBuf, 
        outBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::Evaluate panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Get offset of path
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedPoint& CAlfCurvePath::Offset() const
    {
    TPckg<TAlfTimedPoint> outBuf( iData->iOffset );
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathOffset, 
        KNullDesC8(), 
        outBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::Offset panic error %d", err )
        USER_INVARIANT();
        }
    
    return iData->iOffset;
    }
    
// ---------------------------------------------------------------------------
// Set offset of path
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfCurvePath::SetOffset( const TAlfTimedPoint& aOffset )
    {
    TPckgC<TAlfTimedPoint> inBuf( aOffset );
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathSetOffset, 
        inBuf, 
        outDummy );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::SetOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iOffset = aOffset;
    }

// ---------------------------------------------------------------------------
// Get the mapping function ID of this path
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfCurvePath::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
  
// ---------------------------------------------------------------------------
// Evaluate an x/y value on the path
// ---------------------------------------------------------------------------
//  
EXPORT_C TReal32 CAlfCurvePath::MapValue(TReal32 aValue, TInt aMode) const __SOFTFP
    {
    TIntTReal mapValues( aMode, aValue );
    TPckgC<TIntTReal> mapValuesPckg( mapValues );
    
    TReal32 returnValue = aValue;
    TPckg<TReal32> returnBuf(returnValue);
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfCurvePathMapValue, 
        mapValuesPckg, 
        returnBuf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCurvePath::MapValue panic error %d", err )
        USER_INVARIANT();
        }
    
    return returnValue;
    }
    
