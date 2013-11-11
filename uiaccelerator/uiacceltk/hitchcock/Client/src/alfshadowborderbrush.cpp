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
* Description:   Shadow border brush
*
*/



#include "alf/alfshadowborderbrush.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfShadowBorderBrush::TShadowBorderPrivateData
    {
    TAlfTimedValue iWidth;
    TAlfMetric iMetricWidth;
    };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfShadowBorderBrush::CAlfShadowBorderBrush()
    {
    }
    
void CAlfShadowBorderBrush::ConstructL( CAlfEnv& aEnv, TInt aWidth )
    {
    TPckgC<TInt> paramsPckg( aWidth );
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfShadowBorderBrushCreate,
        0,
        paramsPckg
        );
        
    iShadowBorderData = new (ELeave) TShadowBorderPrivateData;
    iShadowBorderData->iWidth.SetValueNow( aWidth );
    }
    
EXPORT_C CAlfShadowBorderBrush* CAlfShadowBorderBrush::NewL(
        CAlfEnv& aEnv,
        TInt aWidth )
    {
    CAlfShadowBorderBrush* self = NewLC( aEnv, aWidth );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CAlfShadowBorderBrush* CAlfShadowBorderBrush::NewLC(
        CAlfEnv& aEnv,
        TInt aWidth )
    {
    CAlfShadowBorderBrush* self = new (ELeave) CAlfShadowBorderBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aWidth );
    return self;
    }

void CAlfShadowBorderBrush::ConstructL( CAlfEnv& aEnv, const TAlfMetric& aWidth )
    {
    TPckgC<TAlfMetric> paramsPckg( aWidth );
    
    CAlfBrush::ConstructL(
        aEnv,
        EAlfShadowBorderBrushCreateMetric,
        0,
        paramsPckg
        );
        
    iShadowBorderData = new (ELeave) TShadowBorderPrivateData;
    iShadowBorderData->iWidth.SetValueNow( 0 ); // Not used, just set to zero.
    iShadowBorderData->iMetricWidth = TAlfMetric(aWidth);
    }
    
EXPORT_C CAlfShadowBorderBrush* CAlfShadowBorderBrush::NewL(
        CAlfEnv& aEnv,
        const TAlfMetric& aWidth )
    {
    CAlfShadowBorderBrush* self = NewLC( aEnv, aWidth );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CAlfShadowBorderBrush* CAlfShadowBorderBrush::NewLC(
        CAlfEnv& aEnv,
        const TAlfMetric& aWidth )
    {
    CAlfShadowBorderBrush* self = new (ELeave) CAlfShadowBorderBrush;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aWidth );
    return self;
    }

    
CAlfShadowBorderBrush::~CAlfShadowBorderBrush()
    {
    delete iShadowBorderData;
    }
    
EXPORT_C const TAlfTimedValue& CAlfShadowBorderBrush::Width() const
    {
    TPckg<TAlfTimedValue> buf(iShadowBorderData->iWidth);
    TInt err = Comms()->DoSynchronousCmd( EAlfShadowBorderBrushGetWidth, buf, buf);

    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfShadowBorderBrush::Width panic error %d", err )
        USER_INVARIANT();
        }
    
    return iShadowBorderData->iWidth;
    }
    
EXPORT_C void CAlfShadowBorderBrush::SetWidth( const TAlfTimedValue& aWidth )
    {
    TPckgC<TAlfTimedValue> buf(aWidth);

    TInt err = Comms()->DoCmdNoReply( EAlfShadowBorderBrushSetWidth, buf );

    if ( err == KErrNone )
        {
        iShadowBorderData->iWidth = aWidth;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfShadowBorderBrush::SetWidth panic error %d", err )
        USER_INVARIANT();
        }
    }
    
EXPORT_C void CAlfShadowBorderBrush::SetWidth( const TAlfMetric& aWidth )
    {
    TPckgC<TAlfMetric> buf(aWidth);

    TInt err = Comms()->DoCmdNoReply( EAlfShadowBorderBrushSetWidthMetric, buf);

    if ( err == KErrNone )
        {
        iShadowBorderData->iMetricWidth = aWidth;
        }
    else
        {
        __ALFLOGSTRING1( "CAlfShadowBorderBrush::SetWidth (TAlfMetric) panic error %d", err )
        USER_INVARIANT();
        }
    }

    
    
