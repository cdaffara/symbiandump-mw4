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
* Description:   Drop shadow brush
*
*/



#include "alf/alfdropshadowbrush.h"
#include "alf/alfconstants.h"
#include "alf/alfgencomponent.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfDropShadowBrush::TDropShadowPrivateData
    {
    TAlfTimedValue iSoftness;
    TAlfTimedValue iDepth;
    };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfDropShadowBrush::CAlfDropShadowBrush()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor (with metric depth)
// ---------------------------------------------------------------------------
//
void CAlfDropShadowBrush::ConstructL( CAlfEnv& aEnv, const TAlfMetric&  aDepth )
    {
    TPckgC<TAlfMetric> paramsPckg( aDepth );
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfDropShadowBrushCreateMetric,
        0,
        paramsPckg
        );
        
    iDropShadowData = new (ELeave) TDropShadowPrivateData;
    }

// ---------------------------------------------------------------------------
// 2-phased constructor
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDropShadowBrush* CAlfDropShadowBrush::NewL(
        CAlfEnv& aEnv,
        const TAlfMetric&  aDepth )
    {
    CAlfDropShadowBrush* self = NewLC( aEnv, aDepth );
    CleanupStack::Pop( self);
    return self;
    }

// ---------------------------------------------------------------------------
// 2-phased constructor with the object left on the cleanup stack
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDropShadowBrush* CAlfDropShadowBrush::NewLC(
        CAlfEnv& aEnv,
        const TAlfMetric&  aDepth )
    {
    CAlfDropShadowBrush* self = new (ELeave) CAlfDropShadowBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aDepth );
    return self;
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor (with pixel depth)
// ---------------------------------------------------------------------------
//    
void CAlfDropShadowBrush::ConstructL( CAlfEnv& aEnv, TInt aDepth )
    {
    TPckgC<TInt> paramsPckg( aDepth );
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfDropShadowBrushCreate,
        0,
        paramsPckg
        );
        
    iDropShadowData = new (ELeave) TDropShadowPrivateData;
    }

// ---------------------------------------------------------------------------
// 2-phased constructor
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDropShadowBrush* CAlfDropShadowBrush::NewL(
        CAlfEnv& aEnv,
        TInt aDepth )
    {
    CAlfDropShadowBrush* self = NewLC( aEnv, aDepth );
    CleanupStack::Pop( self);
    return self;
    }

// ---------------------------------------------------------------------------
// 2-phased constructor with the object left on the cleanup stack
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfDropShadowBrush* CAlfDropShadowBrush::NewLC(
        CAlfEnv& aEnv,
        TInt aDepth )
    {
    CAlfDropShadowBrush* self = new (ELeave) CAlfDropShadowBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aDepth );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//    
CAlfDropShadowBrush::~CAlfDropShadowBrush()
    {
    delete iDropShadowData;
    }

// ---------------------------------------------------------------------------
// Softness getter
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedValue& CAlfDropShadowBrush::Softness() const
    {
    TPckg<TAlfTimedValue> buf(iDropShadowData->iSoftness);
    TInt err = Comms()->DoSynchronousCmd( EAlfDropShadowBrushGetSoftness, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::Softness panic error %d", err )
        USER_INVARIANT();
        }
    
    return iDropShadowData->iSoftness;
    }

// ---------------------------------------------------------------------------
// Softness setter
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfDropShadowBrush::SetSoftness( const TAlfTimedValue& aSoftness )
    {
    TPckgC<TAlfTimedValue> buf(aSoftness);

    TInt err = Comms()->DoCmdNoReply( EAlfDropShadowBrushSetSoftness, buf );

    if ( err == KErrNone )
        {
        iDropShadowData->iSoftness = aSoftness;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::SetSoftness panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Depth getter
// ---------------------------------------------------------------------------
//    
EXPORT_C const TAlfTimedValue& CAlfDropShadowBrush::Depth() const
    {
    TPckg<TAlfTimedValue> buf(iDropShadowData->iDepth);
    TInt err = Comms()->DoSynchronousCmd( EAlfDropShadowBrushGetDepth, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::Depth panic error %d", err )
        USER_INVARIANT();
        }
    
    return iDropShadowData->iDepth;
    }

// ---------------------------------------------------------------------------
// Depth setter
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfDropShadowBrush::SetDepth( const TAlfTimedValue& aDepth )
    {
    TPckgC<TAlfTimedValue> buf(aDepth);

    TInt err = Comms()->DoCmdNoReply( EAlfDropShadowBrushSetDepth, buf );

    if ( err == KErrNone )
        {
        iDropShadowData->iDepth = aDepth;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::SetDepth panic error %d", err )
        USER_INVARIANT();
        }
    }
    
// ---------------------------------------------------------------------------
// Softness setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadowBrush::SetSoftness(const TAlfMetric& aSoftness)
    {
    TPckgC<TAlfMetric> buf(aSoftness);

    TInt err = Comms()->DoCmdNoReply( EAlfDropShadowBrushSetSoftnessMetric, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::SetSoftness panic error %d", err )
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// Depth setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadowBrush::SetDepth(const TAlfMetric& aDepth)
    {
    TPckgC<TAlfMetric> buf(aDepth);

    TInt err = Comms()->DoCmdNoReply( EAlfDropShadowBrushSetDepthMetric, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::SetDepth panic error %d", err )
        USER_INVARIANT();
        }        
    }

// ---------------------------------------------------------------------------
// Color setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDropShadowBrush::SetColor(const TRgb& aColor)
    {
    TPckgC<TRgb> buf(aColor);

    TInt err = Comms()->DoCmdNoReply( EAlfDropShadowBrushSetColor, buf );

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDropShadowBrush::SetColor panic error %d", err )
        USER_INVARIANT();
        }        
        
    }
